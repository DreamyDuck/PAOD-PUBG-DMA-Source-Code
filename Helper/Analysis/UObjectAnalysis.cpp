#include "Include/WinHeaders.hpp"
#include "UObjectAnalysis.h"
#include "Core/Core.hpp"
#include "Core/DMA/DMAHandler.h"

namespace IronMan::Analysis
{
	using namespace Core;
	void* CopyMap2;

	UObjectAnalysis::UObjectAnalysis(ptr_t entryPoint)
		: mEntryPoint(entryPoint)
	{
		uintptr_t pageSize = 4096;
		mBase = (entryPoint - 0x10) & ~(pageSize - 1);
		mSize = 0x2000;
	}

	UObjectAnalysis::~UObjectAnalysis()
	{

	}

	bool UObjectAnalysis::Analyse()
	{
		analyseDecrypttions();
		return !mDecrypts.empty();
	}

	bool UObjectAnalysis::Build(std::function< void(void*)> callback)
	{
		for (const auto& decrypt : mDecrypts)
		{
			if (callback != nullptr)
				callback((void*)(&decrypt.second));


			auto a = AsmFactory::GetAssembler();

			a->GenPrologue();

			AsmStackAllocator sa(a->assembler(), 0x30 + (int32_t)decrypt.second.SaveRegisters.size() * 8);

			size_t stack_size = Align(sa.getTotalSize(), 0x10);
			(*a)->sub(asmjit::host::rsp, stack_size);

			// Save registers
			for (int i = 0; i < decrypt.second.SaveRegisters.size(); i++)
				(*a)->mov(asmjit::host::qword_ptr(asmjit::host::rsp, 0x28 + i * 8), ToGpReg(decrypt.second.SaveRegisters[i]));
			(*a)->mov(asmjit::host::r15, asmjit::host::rcx);//mov r15,rcx
			(*a)->mov(ToGpReg(GpRegTo64(decrypt.second.BaseRegister)), asmjit::host::rcx);//mov rdx,rcx


			for (int i = 0; i < decrypt.second.Instructions.size(); i++)
			{
				const auto& inst = decrypt.second.Instructions[i];
				Zydis BuildCp;
				if (!BuildCp.Disassemble(inst.Address, reinterpret_cast<const unsigned char*>(inst.Address)))
					continue;
				const auto& op1 = BuildCp[0];
				const auto& op2 = BuildCp[1];
				if (op1.type == ZYDIS_OPERAND_TYPE_REGISTER
					&& op2.type == ZYDIS_OPERAND_TYPE_MEMORY
					&& op2.mem.base != ZYDIS_REGISTER_RSP
					&& op2.mem.disp.value == inst.Offset
					&& (BuildCp.GetId() == ZYDIS_MNEMONIC_XOR
						|| BuildCp.GetId() == ZYDIS_MNEMONIC_ROL
						|| BuildCp.GetId() == ZYDIS_MNEMONIC_ROR
						|| BuildCp.GetId() == ZYDIS_MNEMONIC_SHL
						|| BuildCp.GetId() == ZYDIS_MNEMONIC_SHR))
				{
					switch (BuildCp.GetId())
					{
					case ZYDIS_MNEMONIC_XOR:
						(*a)->xor_(ToGpReg(op1.reg.value), asmjit::host::r15);
						break;
					case ZYDIS_MNEMONIC_ROL:
						(*a)->rol(ToGpReg(op1.reg.value), asmjit::host::r15);
						break;
					case ZYDIS_MNEMONIC_ROR:
						(*a)->ror(ToGpReg(op1.reg.value), asmjit::host::r15);
						break;
					case ZYDIS_MNEMONIC_SHL:
						(*a)->shl(ToGpReg(op1.reg.value), asmjit::host::r15);
						break;
					case ZYDIS_MNEMONIC_SHR:
						(*a)->shr(ToGpReg(op1.reg.value), asmjit::host::r15);
						break;
					default:
						break;
					}
				}
				else {
					(*a)->embed(reinterpret_cast<const unsigned char*>(inst.Address), inst.Instruct.length);
				}
			}

			// Last instruction result
			auto lastInstruct = decrypt.second.Instructions.back();
			if (lastInstruct.Instruct.operand_count > 0  /*lastInstruct.Instruct.mnemonic == ZYDIS_MNEMONIC_MOV*/)
			{
				const auto& op = lastInstruct.Instruct.operands[0];
				if (op.type == ZYDIS_OPERAND_TYPE_REGISTER
					&& inRange(op.reg.value, ZYDIS_REGISTER_AL, ZYDIS_REGISTER_R15)
					&& GpRegTo64(op.reg.value) != ZYDIS_REGISTER_RAX)
					(*a)->mov(asmjit::host::rax, ToGpReg(GpRegTo64(lastInstruct.Instruct.operands[0].reg.value)));
			}


			// Restore registers 
			for (int i = 0; i < decrypt.second.SaveRegisters.size(); i++)
				(*a)->mov(ToGpReg(decrypt.second.SaveRegisters[i]), asmjit::host::qword_ptr(asmjit::host::rsp, 0x28 + i * 8));


			(*a)->add(asmjit::host::rsp, stack_size);
			a->GenEpilogue();
			mBuildResult.push_back(std::move(a));
		}
		return !mBuildResult.empty();
	}
	void UObjectAnalysis::analyseDecrypttions()
	{
		if (CopyMap2)
		{
			free(CopyMap2);
			CopyMap2 = nullptr;
		}
		CopyMap2 = malloc(mSize);
		if (!CopyMap2)
			return;
		ZeroMemory(CopyMap2, mSize);

		IronMan::GetDMA().Read((ULONG64)mBase, (ULONG64)CopyMap2, mSize);
		if (*(ptr_t*)CopyMap2 == 0 && *(ptr_t*)((ptr_t)CopyMap2 + 8) == 0)
			return;

		ptr_t maxaddr = (ptr_t)CopyMap2 + mSize;
		ptr_t start = (ptr_t)CopyMap2 + (mEntryPoint - mBase);

		std::map<ZydisRegister, ZydisRegister> bindRegister;
		std::vector<InstructionInfo> pendingInstruction;

		int index = 0;
		while (true)
		{
			if (start >= (maxaddr - 16) || Utils::IsSafeReadPtr((LPVOID)start, 16))
			{
				CONSOLE_INFO("address invalid 2");
				break;
			}
			if (!mCp.Disassemble(start, reinterpret_cast<const unsigned char*>(start)))
			{
				start++;
				continue;
			}
			if (start + mCp.Size() > maxaddr) // out of range
				break;

			auto& tempInfo = InstructionInfo(*mCp.GetInstr(), mCp.Address());
			if (mCp.GetId() == ZYDIS_MNEMONIC_MOV)
			{
				const auto& op1 = mCp[0];
				const auto& op2 = mCp[1];

				//分析偏移
				if (op1.type == ZYDIS_OPERAND_TYPE_REGISTER
					&& op2.type == ZYDIS_OPERAND_TYPE_MEMORY
					&& op2.mem.base != ZYDIS_REGISTER_RSP)
				{
					if (mDecrypts.find(op2.mem.disp.value) == nullptr)
					{
						mDecrypts.insert(op2.mem.disp.value, DecryptInfo(mBase, op1.reg.value, (int32_t)op2.mem.disp.value, index++));
						bindRegister.erase(op1.reg.value);
					}


					auto info = &mDecrypts[op2.mem.disp.value];
					auto regValue64 = GpRegTo64(op1.reg.value);
					if (regValue64 != ZYDIS_REGISTER_RCX
						&& regValue64 != ZYDIS_REGISTER_RDX
						&& regValue64 != ZYDIS_REGISTER_R8
						&& regValue64 != ZYDIS_REGISTER_R9
						&& regValue64 != ZYDIS_REGISTER_RAX)
					{

						auto result = std::find(info->SaveRegisters.begin(), info->SaveRegisters.end(), regValue64);
						if (result == info->SaveRegisters.end())
							info->SaveRegisters.push_back(regValue64);
					}
				}
				else if (op1.type == ZYDIS_OPERAND_TYPE_REGISTER)
				{

					if (op2.type == ZYDIS_OPERAND_TYPE_REGISTER)// mov ax,register
					{
						bindRegister[op1.reg.value] = op2.reg.value;

						auto info = mDecrypts.findValue([&op2](const DecryptInfo& value) {return value.BaseRegister == op2.reg.value; });
						if (info != nullptr)
						{
							info->Instructions.push_back(tempInfo);
							info->EndRegister = op1.reg.value;
						}
					}
					if (op2.type == ZYDIS_OPERAND_TYPE_IMMEDIATE)// mov ax,xxx
						pendingInstruction.push_back(tempInfo);
				}
				else if (op1.type == ZYDIS_OPERAND_TYPE_MEMORY
					&& op2.type == ZYDIS_OPERAND_TYPE_REGISTER
					&& op1.mem.base == ZYDIS_REGISTER_RSP)
				{
					auto info = mDecrypts.findValue([&op2](const DecryptInfo& value)
						{
							return value.BaseRegister == op2.reg.value;
						});

					if (info == nullptr)
					{
						auto bindInfo = mDecrypts.findValue([&bindRegister, &op2](const DecryptInfo& value) {return value.BaseRegister == bindRegister[op2.reg.value]; });
						if (bindRegister.find(op2.reg.value) != bindRegister.end() && bindInfo != nullptr)
						{
							bindInfo->EndRegister = ZYDIS_REGISTER_RSP;
							bindInfo->LastValue = op1.mem.disp.value;
						}
					}
					else
					{
						info->EndRegister = ZYDIS_REGISTER_RSP;
						info->LastValue = op1.mem.disp.value;
					}
				}
			}
			//分析运算指令
			else if (mCp.GetId() == ZYDIS_MNEMONIC_XOR
				|| mCp.GetId() == ZYDIS_MNEMONIC_ROL
				|| mCp.GetId() == ZYDIS_MNEMONIC_ROR
				|| mCp.GetId() == ZYDIS_MNEMONIC_SHL
				|| mCp.GetId() == ZYDIS_MNEMONIC_SHR)
			{
				auto regValue = mCp[0].reg.value;
				auto info = mDecrypts.findValue([&regValue](const DecryptInfo& value) {return value.BaseRegister == regValue; });
				//DecryptInfo* info;
				//if (info == nullptr)
				//{
				if (bindRegister.find(regValue) != bindRegister.end())
					info = mDecrypts.findValue([&bindRegister, &regValue](const DecryptInfo& value) {return value.BaseRegister == bindRegister[regValue]; });
				//}

				if (info != nullptr)
				{
					auto regValue64 = GpRegTo64(regValue);
					if (regValue64 != ZYDIS_REGISTER_RCX
						&& regValue64 != ZYDIS_REGISTER_RDX
						&& regValue64 != ZYDIS_REGISTER_R8
						&& regValue64 != ZYDIS_REGISTER_R9
						&& regValue64 != ZYDIS_REGISTER_RAX)
					{

						auto result = std::find(info->SaveRegisters.begin(), info->SaveRegisters.end(), regValue64);
						if (result == info->SaveRegisters.end())
							info->SaveRegisters.push_back(regValue64);
					}

					if (!pendingInstruction.empty())
					{
						if (mCp[1].type == ZYDIS_OPERAND_TYPE_REGISTER && mCp[1].reg.value == pendingInstruction.back().Instruct.operands[0].reg.value)
						{
							info->Instructions.insert(info->Instructions.end(), pendingInstruction.begin(), pendingInstruction.end());
							pendingInstruction.clear();
						}
					}
					const auto& op1 = mCp[0];
					const auto& op2 = mCp[1];
					if (op1.type == ZYDIS_OPERAND_TYPE_REGISTER
						&& op2.type == ZYDIS_OPERAND_TYPE_MEMORY
						&& op2.mem.base != ZYDIS_REGISTER_RSP
						&& op2.mem.disp.value == info->Offset)
					{
						tempInfo.Offset = op2.mem.disp.value;
					}
					info->Instructions.push_back(tempInfo);
				}
			}
			else if (mCp.IsJump())
				break;
			else if (mCp.IsCondJump() && !mDecrypts.empty())
				break;

			start += mCp.Size();
		}
	}
}