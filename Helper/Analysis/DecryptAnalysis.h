#pragma once
#include "AnalysisBase.h"

namespace IronMan::Analysis
{
	using namespace Core;
	class DecryptAnalysis :public AnalysisBase
	{
	public:
		struct VariableInfo
		{
			size_t Size;
			ptr_t Value;

			explicit VariableInfo(size_t size, ptr_t value)
				:Size(size), Value(value)
			{

			}
		};

		struct RegisterInfo
		{
			ZydisRegister GpReg;

			/*explicit RegisterInfo(size_t size, ptr_t value)
				:Size(size), Value(value)
			{

			}*/
		};
		struct InstructionInfo
		{
			ZydisDecodedInstruction Instruct;
			ptr_t Address;
			int VarIndex;
			int OperandIndex;

			explicit InstructionInfo(ZydisDecodedInstruction instruct, ptr_t address, int varIndex = -1, int operandIndex = 0)
				:Instruct(instruct), Address(address), VarIndex(varIndex), OperandIndex(operandIndex)
			{
			}
		};
		struct DecryptInfo
		{
			ptr_t Parent;
			ptr_t Start;
			ptr_t End;
			ptr_t CallStart;
			ptr_t CallEnd;
			ptr_t CallFlagAddr;
			ptr_t DecryptCall;
			bool isCallDecrypt;

			std::vector<InstructionInfo> Instructions;
			std::vector<VariableInfo> Variables;
			std::vector<ZydisRegister> Registers;

			int NumberOfVariables;
			size_t StackSize;

			explicit DecryptInfo(ptr_t parent, ptr_t start, ptr_t end)
				:Parent(parent),
				Start(start),
				End(end),
				CallStart(0),
				CallEnd(0),
				CallFlagAddr(0),
				isCallDecrypt(false),
				DecryptCall(0),
				NumberOfVariables(0),
				StackSize()
			{
			}
			explicit DecryptInfo()
				: DecryptInfo(0, 0, 0)
			{
			}
		};

	public:
		bool Analyse() override;
		bool Build(std::function< void(void*)> callback = nullptr) override;
		const ptr_t GetEntryPoint() const override { return mEntryPoint; }
	private:

		//template<class T>


		void MOV(const InstructionInfo& inst, const std::vector<asmjit::Mem>& vars, asmjit::X86Assembler* a, bool isCrypt);
		void scanDecryptions();
		void analyseDecrypttions();

	public:
		explicit DecryptAnalysis(ptr_t entryPoint);
		~DecryptAnalysis();
	private:

		std::vector<DecryptInfo> mDecrypts;
		ptr_t mEntryPoint;
	};

}