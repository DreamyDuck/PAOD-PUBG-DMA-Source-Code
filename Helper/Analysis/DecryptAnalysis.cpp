#include "Include/WinHeaders.hpp"
#include "DecryptAnalysis.h"
#include "Core/Core.hpp"
#include "Core/DMA/DMAHandler.h"
namespace IronMan::Analysis
{
	using namespace Core;
	void* CopyMap;


	DecryptAnalysis::DecryptAnalysis(ptr_t entryPoint)
		: mEntryPoint(entryPoint)
	{
		uintptr_t pageSize = 4096;
		mBase = (entryPoint - 0x10) & ~(pageSize - 1);
		mSize = 0x2000;
		CONSOLE_INFO("mBase:%p|mSize:%x|entryPoint:%p", mBase, mSize, entryPoint);
	}

	DecryptAnalysis::~DecryptAnalysis()
	{

	}

	bool DecryptAnalysis::Analyse()
	{
		scanDecryptions();
		analyseDecrypttions();
		return !mDecrypts.empty();
	}

	bool DecryptAnalysis::Build(std::function< void(void*)> callback)
	{
		int AllCount = 0;
		for (auto& decrypt : mDecrypts)
		{
			if (decrypt.Instructions.empty())
				continue;
			AllCount++;
			if (callback != nullptr)
				callback((void*)(&decrypt));

			auto a = AsmFactory::GetAssembler();
			auto retLabel = (*a)->newLabel();
			auto decryptLabel = (*a)->newLabel();

			a->GenPrologue();

			std::vector<asmjit::Mem> vars;
			AsmStackAllocator sa(a->assembler(), 0x28 + (int32_t)decrypt.Registers.size() * 8);

			for (const auto& varInfo : decrypt.Variables)
			{
				ALLOC_STACK_VAR_S_NOALIGN(sa, temp, (int32_t)varInfo.Size);
				vars.push_back(temp);
			}

			size_t stack_size = Align(sa.getTotalSize(), 0x10);
			(*a)->sub(asmjit::host::rsp, stack_size);

			for (int i = 0; i < decrypt.Registers.size(); i++)
				(*a)->mov(asmjit::host::qword_ptr(asmjit::host::rsp, 0x20 + i * 8), ToGpReg(decrypt.Registers[i]));

			(*a)->mov(asmjit::host::r8, asmjit::host::rcx);//mov r8,rcx
			(*a)->mov(asmjit::host::r9, asmjit::host::rcx);//mov r9,rcx
			(*a)->mov(asmjit::host::rdx, asmjit::host::rcx);//mov rdx,rcx
			(*a)->mov(asmjit::host::rax, asmjit::host::rcx);//mov rax,rcx

			if (decrypt.CallFlagAddr)
			{
				if (IronMan::GetDMA().Read<ptr_t>(decrypt.CallFlagAddr) == 1)
				{
					(*a)->mov(asmjit::host::rax, GetDMA().Read<ptr_t>(decrypt.CallFlagAddr));//mov rax,addr
					(*a)->test(asmjit::host::rax, asmjit::host::rax);
					(*a)->jne(decryptLabel);
				}
				(*a)->mov(asmjit::host::rax, asmjit::host::rcx);//mov rax,rcx

				for (ptr_t current = decrypt.CallStart; current < decrypt.CallEnd; )
				{
					if (Utils::IsSafeReadPtr((LPVOID)current, 16))
						break;
					if (!mCp.Disassemble(current, reinterpret_cast<const unsigned char*>(current)))
					{
						current++;
						continue;
					}

					if (!mCp.IsCall() && !mCp.IsJump())
					{
						const auto& op1 = mCp[0];
						const auto& op2 = mCp[1];
						if ((op2.type == ZYDIS_OPERAND_TYPE_IMMEDIATE && op1.reg.value == ZYDIS_REGISTER_RSP)
							|| (op1.type == ZYDIS_OPERAND_TYPE_REGISTER && op2.type == ZYDIS_OPERAND_TYPE_MEMORY && op2.mem.base == ZYDIS_REGISTER_RIP))
						{
							goto NextStep;
						}
						else
						{
							(*a)->embed(reinterpret_cast<const unsigned char*>(current), mCp.Size());
						}
					}
					else break;
				NextStep:
					current += mCp.Size();
				}
				(*a)->mov(asmjit::host::rdx, asmjit::host::rax);//mov rdx,rax
				ptr_t DecryptionAddress = GetDMA().Read<ptr_t>(decrypt.DecryptCall);
				g_PatternData.DecryptAddress = DecryptionAddress;
				while (true)
				{
					unsigned char data[MAX_DISASM_BUFFER];
					IronMan::GetDMA().Read(DecryptionAddress, (ptr_t)data, MAX_DISASM_BUFFER);
					if (!mCp.Disassemble(DecryptionAddress, reinterpret_cast<const unsigned char*>(data)))
					{
						DecryptionAddress++;
						continue;
					}
					if (mCp.IsRet())
						break;

					if (mCp.GetId() == ZYDIS_MNEMONIC_LEA)
					{
						const auto& op1 = mCp[0];
						const auto& op2 = mCp[1];
						ptr_t FixOff = DecryptionAddress + op2.mem.disp.value + 7;
						(*a)->db(0x48);
						(*a)->db(0xB8);
						(*a)->dq(FixOff);
					}
					else
					{
						(*a)->embed(reinterpret_cast<const unsigned char*>(data), mCp.Size());
					}
					DecryptionAddress += mCp.Size();
				}
				if (IronMan::GetDMA().Read<ptr_t>(decrypt.CallFlagAddr) == 1)
				{
					(*a)->jmp(retLabel);
				}
			}
			(*a)->bind(decryptLabel);
			if (IronMan::GetDMA().Read<ptr_t>(decrypt.CallFlagAddr) == 1)
			{
				bool isCrypt = false;
				for (int i = 0; i < decrypt.Instructions.size(); i++)
				{
					const auto& inst = decrypt.Instructions[i];

					//如果还没有运算指令,mov qword ptr ss:[rsp+0x40], r14   r14--->0  (也可以判断第二个操作数是否有立即操作数)
					//TODO: 获取原始寄存器内容.

					if (inRange(inst.Instruct.mnemonic, ZYDIS_MNEMONIC_SHL, ZYDIS_MNEMONIC_SHRX) ||  //shl shr
						inRange(inst.Instruct.mnemonic, ZYDIS_MNEMONIC_ROL, ZYDIS_MNEMONIC_RORX) ||  //rol ror
						inRange(inst.Instruct.mnemonic, ZYDIS_MNEMONIC_NOT, ZYDIS_MNEMONIC_OR) ||    //not or
						inst.Instruct.mnemonic == ZYDIS_MNEMONIC_SAR ||								 //sar
						inst.Instruct.mnemonic == ZYDIS_MNEMONIC_XOR ||								 //xor
						inst.Instruct.mnemonic == ZYDIS_MNEMONIC_SUB ||								 //sub
						inst.Instruct.mnemonic == ZYDIS_MNEMONIC_AND ||								 //and
						inst.Instruct.mnemonic == ZYDIS_MNEMONIC_ADD ||
						inst.Instruct.mnemonic == ZYDIS_MNEMONIC_LEA)								 //add
						isCrypt = true;

					/*if (inst.Instruct.operand_count == 1 || (inst.Instruct.operand_count >= 2 && inst.Instruct.operands[1].type == ZYDIS_OPERAND_TYPE_IMMEDIATE))
						isCrypt = true;*/

						//如果还没有运算指令,找到存放加密内容的寄存器
					if (!isCrypt)
					{
						for (int i = 1; i < inst.Instruct.operand_count; i++)
						{
							const auto& op = inst.Instruct.operands[i];
							if (inst.Instruct.operands[0].mem.base == ZYDIS_REGISTER_RSP || inst.Instruct.operands[0].mem.base == ZYDIS_REGISTER_RBP)//过滤对变量的赋值操作
								continue;
							if (op.type == ZYDIS_OPERAND_TYPE_REGISTER
								&& inRange(op.reg.value, ZYDIS_REGISTER_AL, ZYDIS_REGISTER_R15)
								&& GpRegTo64(op.reg.value) != ZYDIS_REGISTER_RCX)
							{
								isCrypt = true;//找到一个就够了 
								(*a)->mov(ToGpReg(GpRegTo64(op.reg.value)), asmjit::host::rcx);

								//保存寄存器
								auto reg64 = GpRegTo64(op.reg.value);
								if (reg64 != ZYDIS_REGISTER_RCX && reg64 != ZYDIS_REGISTER_RDX && reg64 != ZYDIS_REGISTER_R8 && reg64 != ZYDIS_REGISTER_R9 && reg64 != ZYDIS_REGISTER_RAX)
								{
									auto result = std::find(decrypt.Registers.begin(), decrypt.Registers.end(), reg64);
									if (result == decrypt.Registers.end())
									{
										decrypt.Registers.push_back(reg64);
									}
								}
							}

							else if ((op.mem.base == ZYDIS_REGISTER_RSP || op.mem.base == ZYDIS_REGISTER_RBP) && inst.VarIndex != -1)
								(*a)->mov(vars[inst.VarIndex], asmjit::host::rcx);//mov qword ptr ss:[var], rcx
						}
					}

					// mov rax, qword ptr ss:[rsp+]
					if (inst.Instruct.mnemonic == ZYDIS_MNEMONIC_MOV)
						MOV(inst, vars, a->assembler(), isCrypt);
					else
					{
						(*a)->embed(reinterpret_cast<const unsigned char*>(inst.Address), inst.Instruct.length);
					}

				}
				// Last instruction result
				auto lastInstruct = decrypt.Instructions.back();
				if (lastInstruct.Instruct.operand_count > 0  /*lastInstruct.Instruct.mnemonic == ZYDIS_MNEMONIC_MOV*/)
				{
					const auto& op = lastInstruct.Instruct.operands[0];
					if (op.type == ZYDIS_OPERAND_TYPE_REGISTER
						&& inRange(op.reg.value, ZYDIS_REGISTER_AL, ZYDIS_REGISTER_R15)
						&& GpRegTo64(op.reg.value) != ZYDIS_REGISTER_RAX)
						(*a)->mov(asmjit::host::rax, ToGpReg(GpRegTo64(lastInstruct.Instruct.operands[0].reg.value)));
				}
			}
			(*a)->bind(retLabel);

			for (int i = 0; i < decrypt.Registers.size(); i++)
				(*a)->mov(ToGpReg(decrypt.Registers[i]), asmjit::host::qword_ptr(asmjit::host::rsp, 0x20 + i * 8));

			(*a)->add(asmjit::host::rsp, stack_size);
			a->GenEpilogue();
			mBuildResult.push_back(std::move(a));
			if (AllCount > 5)
				break;
		}

		return !mBuildResult.empty();
	}

	void DecryptAnalysis::MOV(const InstructionInfo& inst, const std::vector<asmjit::Mem>& vars, asmjit::X86Assembler* a, bool isCrypt)
	{
		const auto& op1 = inst.Instruct.operands[0];
		const auto& op2 = inst.Instruct.operands[1];

		if (op1.type == ZYDIS_OPERAND_TYPE_REGISTER && op2.type == ZYDIS_OPERAND_TYPE_REGISTER)
		{
			a->mov(ToGpReg(op1.reg.value), ToGpReg(op2.reg.value));
		}
		else if (op1.mem.base == ZYDIS_REGISTER_RSP || op1.mem.base == ZYDIS_REGISTER_RBP)
		{
			switch (op2.type)
			{
			case ZYDIS_OPERAND_TYPE_IMMEDIATE:
				a->mov(vars[inst.VarIndex], op2.imm.value.u);//mov qword ptr ss:[rsp+0x30], 0x00
				break;
			case ZYDIS_OPERAND_TYPE_REGISTER:

				if (op2.size / 8 == 8 || !isCrypt)
					a->mov(vars[inst.VarIndex], 0);//mov qword ptr ss:[rsp+0x30], 0
				else
					a->mov(vars[inst.VarIndex], ToGpReg(op2.reg.value));//mov qword ptr ss:[rsp+0x30], rax

				break;
			default:
				assert(0);
				break;
			}
		}
		else if (op2.mem.base == ZYDIS_REGISTER_RSP || op2.mem.base == ZYDIS_REGISTER_RBP)
		{
			switch (op1.type)
			{
			case ZYDIS_OPERAND_TYPE_REGISTER:	// mov rax, qword ptr ss:[rsp+0x38]
				a->mov(ToGpReg(op1.reg.value), vars[inst.VarIndex]);
				break;
			default:
				assert(0);
				break;
			}
		}
		else if (op2.type == ZYDIS_OPERAND_TYPE_MEMORY && op2.mem.base == ZYDIS_REGISTER_RIP)
		{
			a->mov(ToGpReg(op1.reg.value), size_t(op2.mem.disp.value) + inst.Address + inst.Instruct.length);
		}

		else
		{
			a->embed(reinterpret_cast<const unsigned char*>(inst.Address), inst.Instruct.length);
		}
	}

	void DecryptAnalysis::scanDecryptions()
	{
		DecryptInfo node;
		if (CopyMap)
		{
			free(CopyMap);
			CopyMap = nullptr;
		}
		CopyMap = malloc(mSize);
		if (!CopyMap)
			return;
		ZeroMemory(CopyMap, mSize);
		IronMan::GetDMA().Read((ULONG64)mBase, (ULONG64)CopyMap, mSize);
		if (*(ptr_t*)CopyMap == 0 && *(ptr_t*)((ptr_t)CopyMap + 8) == 0)
		{
			CONSOLE_INFO("scanDecryptions Failed");
			return;
		}

		ptr_t maxaddr = (ptr_t)CopyMap + mSize;
		ptr_t start = (ptr_t)CopyMap + (mEntryPoint - mBase);

		size_t ReadSize = 0;
		bool $signed = false;
		while (true)
		{
			if (start >= (maxaddr - 16) || Utils::IsSafeReadPtr((LPVOID)start, 16))
			{
				CONSOLE_INFO("address invalid");
				break;
			}
			if (!mCp.Disassemble(start, reinterpret_cast<const unsigned char*>(start)))
			{
				start++;
				continue;
			}
			ReadSize += mCp.Size();
			if (ReadSize >= 0x1000)
				break;
			if (start + mCp.Size() > maxaddr) // out of range
				break;

			if (mCp.IsCondJump() && !$signed)
			{
				node.CallStart = start + mCp.Size();
				node.Start = mCp.BranchDestination();
			}

			if (node.Start && mCp.IsCall() && mCp.OpCount())
			{
				if (mCp[0].type == ZYDIS_OPERAND_TYPE_MEMORY && mCp[0].mem.base == ZYDIS_REGISTER_RIP)// call qword ptr [?]
				{
					auto dest = reinterpret_cast<ptr_t*>(mCp.ResolveOpValue(0, [](ZydisRegister) {return 0; }));
					dest = (ptr_t*)(((ptr_t)dest - (ptr_t)CopyMap) + mBase);

					if (dest && IronMan::GetDMA().Read<ptr_t>((ptr_t)dest) == 0)
					{
						node.DecryptCall = (ptr_t)dest;
						node.isCallDecrypt = false;
						node.CallEnd = start + mCp.Size();
						$signed = true;
					}
					else if (dest && IronMan::GetDMA().Read<ptr_t>((ptr_t)dest) != 0)
					{
						node.CallFlagAddr = (ptr_t)dest - 0x28;
						if (node.CallFlagAddr && IronMan::GetDMA().Read<ptr_t>(node.CallFlagAddr) == 0)
						{
							node.DecryptCall = (ptr_t)dest;
							node.isCallDecrypt = true;
							node.CallEnd = start + mCp.Size();
							$signed = true;
							if (mCp.IsJump())
							{
								start += mCp.Size();
								while (true)
								{
									if (!mCp.Disassemble(start, reinterpret_cast<const unsigned char*>(start)))
									{
										start++;
										continue;
									}
									if (start + mCp.Size() > maxaddr) // out of range
										break;
									if (mCp.IsRet())
										break;
									start += mCp.Size();
								}
								node.Parent = mBase;
								node.End = start - 4;
								mDecrypts.push_back(node);
								memset(&node, 0, sizeof(node));
								$signed = false;
								start = start - 4;
								break;
							}
						}
						//CONSOLE_INFO("isCallDecrypt");
					}
				}
				else
				{
					$signed = false;
					node.Start = 0;
				}


			}
			else if ($signed && mCp.IsJump() && mCp.OpCount() && mCp[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE)// jmp 
			{
				auto dest = ptr_t(mCp[0].imm.value.u);
				if (dest > node.Start)
				{
					node.Parent = mBase;
					node.End = dest;

					/*CONSOLE_INFO("node.Start:%p", node.Start);
					CONSOLE_INFO("node.End:%p", node.End);
					CONSOLE_INFO("node.CallStart:%p", node.CallStart);
					CONSOLE_INFO("node.CallEnd:%p", node.CallEnd);
					CONSOLE_INFO("node.CallFlagAddr:%p", node.CallFlagAddr);
					CONSOLE_INFO("node.isCallDecrypt:%d", node.isCallDecrypt);*/


					mDecrypts.push_back(node);
					memset(&node, 0, sizeof(node));
					$signed = false;
					start = dest;
					continue;
				}
			}
			else if (mCp.IsRet())
				break;

			start += mCp.Size();
		}
		if (mDecrypts.empty())
		{
			maxaddr = (ptr_t)CopyMap + mSize;
			start = (ptr_t)CopyMap + (mEntryPoint - mBase);

			$signed = false;
			while (true)
			{
				if (start >= (maxaddr - 16) || Utils::IsSafeReadPtr((LPVOID)start, 16))
					break;
				if (!mCp.Disassemble(start, reinterpret_cast<const unsigned char*>(start)))
				{
					start++;
					continue;
				}
				if (start + mCp.Size() > maxaddr) // out of range
					break;

				if (mCp.IsCondJump() && !$signed)
				{
					node.CallStart = start + mCp.Size();
					node.Start = mCp.BranchDestination();
				}

				if (node.Start && mCp.IsCall() && mCp.OpCount())
				{
					if (mCp[0].type == ZYDIS_OPERAND_TYPE_MEMORY && mCp[0].mem.base == ZYDIS_REGISTER_RIP)// call qword ptr [?]
					{
						auto dest = reinterpret_cast<ptr_t*>(mCp.ResolveOpValue(0, [](ZydisRegister) {return 0; }));
						dest = (ptr_t*)(((ptr_t)dest - (ptr_t)CopyMap) + mBase);
						if (dest && IronMan::GetDMA().Read<ptr_t>((ptr_t)dest) == 0)
						{
							node.DecryptCall = (ptr_t)dest;
							node.isCallDecrypt = false;
							node.CallEnd = start + mCp.Size();
							$signed = true;
						}
						else if (dest && IronMan::GetDMA().Read<ptr_t>((ptr_t)dest) != 0)
						{
							node.CallFlagAddr = (ptr_t)dest - 0x28;
							if (node.CallFlagAddr && IronMan::GetDMA().Read<ptr_t>(node.CallFlagAddr) == 0)
							{
								node.DecryptCall = (ptr_t)dest;
								node.isCallDecrypt = true;
								node.CallEnd = start + mCp.Size();
								$signed = true;
								if (mCp.IsJump())
								{
									start += mCp.Size();
									while (true)
									{
										if (!mCp.Disassemble(start, reinterpret_cast<const unsigned char*>(start)))
										{
											start++;
											continue;
										}
										if (start + mCp.Size() > maxaddr) // out of range
											break;
										if (mCp.IsRet())
											break;
										start += mCp.Size();
									}
									node.Parent = mBase;
									node.End = start - 4;
									mDecrypts.push_back(node);
									memset(&node, 0, sizeof(node));
									$signed = false;
									start = start - 4;
									break;
								}
							}
							//CONSOLE_INFO("isCallDecrypt");
						}
					}
				}
				else if ($signed && (mCp.IsJump() || mCp.IsCondJump()) && mCp.OpCount() && mCp[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE)// jmp 
				{
					auto dest = ptr_t(mCp[0].imm.value.u);
					if (dest > node.Start)
					{
						node.Parent = mBase;
						node.End = dest;

						/*CONSOLE_INFO("node.Start:%p", node.Start);
						CONSOLE_INFO("node.End:%p", node.End);
						CONSOLE_INFO("node.CallStart:%p", node.CallStart);
						CONSOLE_INFO("node.CallEnd:%p", node.CallEnd);
						CONSOLE_INFO("node.CallFlagAddr:%p", node.CallFlagAddr);
						CONSOLE_INFO("node.isCallDecrypt:%d", node.isCallDecrypt);*/

						mDecrypts.push_back(node);
						memset(&node, 0, sizeof(node));
						$signed = false;
						start = dest;
						continue;
					}
					else
					{
						Zydis m_Cp;
						ptr_t reversestart = node.Start;
						while (true)
						{
							if (!m_Cp.Disassemble(reversestart, reinterpret_cast<const unsigned char*>(reversestart)))
							{
								reversestart++;
								continue;
							}
							if (reversestart + m_Cp.Size() > maxaddr) // out of range
								break;
							if (m_Cp.Size() > 3)
							{
								const auto& op1 = m_Cp[0];
								const auto& op2 = m_Cp[1];
								if (op1.type == ZYDIS_OPERAND_TYPE_REGISTER && op2.mem.type == ZYDIS_MEMOP_TYPE_MEM)
								{
									auto dest = reversestart + mCp.Size();
									if (dest > node.Start)
									{
										node.Parent = mBase;
										node.End = dest;
										//CONSOLE_INFO("node.Start:%p", node.Start);
										//CONSOLE_INFO("node.End:%p", node.End);
										//CONSOLE_INFO("node.CallStart:%p", node.CallStart);
										//CONSOLE_INFO("node.CallEnd:%p", node.CallEnd);
										//CONSOLE_INFO("node.CallFlagAddr:%p", node.CallFlagAddr);
										//CONSOLE_INFO("node.isCallDecrypt:%d", node.isCallDecrypt);
										mDecrypts.push_back(node);
										memset(&node, 0, sizeof(node));
										$signed = false;
										start = dest;
										break;
									}
								}

							}
							else if (m_Cp.IsRet())
								break;
							reversestart += m_Cp.Size();
						}
					}
				}
				else if (mCp.IsRet())
					break;
				start += mCp.Size();
			}
		}
	}
	void DecryptAnalysis::analyseDecrypttions()
	{
		for (auto& decrypt : mDecrypts)
		{
			decrypt.Registers.push_back(ZYDIS_REGISTER_RDI);
			//CONSOLE_INFO("Begin:%p    End:%p", decrypt.Start, decrypt.End);
			//CONSOLE_INFO("----------------------------------------------");

			for (ptr_t current = decrypt.Start; current < decrypt.End; )
			{
				if (Utils::IsSafeReadPtr((LPVOID)current, 16))
					break;
				if (!mCp.Disassemble(current, reinterpret_cast<const unsigned char*>(current)))
				{
					current++;
					continue;
				}

				auto varIndex = -1;
				auto operandIndex = -1;
				for (int i = 0; i < mCp.OpCount(); i++)
				{
					const auto& op = mCp[i];

					if (op.type == ZYDIS_OPERAND_TYPE_REGISTER && i == 0) // Save Register
					{
						auto reg64 = GpRegTo64(op.reg.value);
						if (reg64 != ZYDIS_REGISTER_RCX && reg64 != ZYDIS_REGISTER_RDX && reg64 != ZYDIS_REGISTER_R8 && reg64 != ZYDIS_REGISTER_R9 && reg64 != ZYDIS_REGISTER_RAX)
						{
							auto result = std::find(decrypt.Registers.begin(), decrypt.Registers.end(), reg64);
							if (result == decrypt.Registers.end())
								decrypt.Registers.push_back(reg64);
						}
					}
					if (op.type == ZYDIS_OPERAND_TYPE_MEMORY && (op.mem.base == ZYDIS_REGISTER_RSP || op.mem.base == ZYDIS_REGISTER_RBP))//分析堆栈变量
					{
						auto result = std::find_if(decrypt.Variables.begin(), decrypt.Variables.end(), [&](const auto& o) {return o.Value == op.mem.disp.value; });
						if (result == decrypt.Variables.end())
						{
							decrypt.Variables.push_back(VariableInfo(op.size / 8, op.mem.disp.value));
							varIndex = (int)decrypt.Variables.size() - 1;
						}
						else
						{
							result->Size = std::min(static_cast<int>(result->Size), static_cast<int>(op.size / 8));
							varIndex = (int)(result - decrypt.Variables.begin());
						}
						operandIndex = i;
						break;
					}
				}

				decrypt.Instructions.push_back(InstructionInfo(*mCp.GetInstr(), current, varIndex, operandIndex));

				if (decrypt.Instructions.size() > 5 && mCp.GetId() == ZYDIS_MNEMONIC_MOV && mCp[0].type == ZYDIS_OPERAND_TYPE_REGISTER && mCp[1].mem.base == ZYDIS_REGISTER_RSP)// mov rdx,[rsp+??]  可能是最后一条指令
					break;

				//CONSOLE_INFO("%016" PRIX64 "	%s", mCp.Address(), mCp.InstructionText().c_str());
				current += mCp.Size();
			}
		}
	}

}