#pragma once
#include "AnalysisBase.h"
#include <map>
namespace IronMan::Analysis
{
	using namespace Core;
	class UObjectAnalysis :public AnalysisBase
	{
	public:
		struct InstructionInfo
		{
			ZydisDecodedInstruction Instruct;
			ptr_t Address;
			ptr_t Offset;
			explicit InstructionInfo(ZydisDecodedInstruction instruct, ptr_t address)
				:Instruct(instruct), Address(address), Offset(0)
			{
			}
		};
		struct DecryptInfo
		{
			ptr_t Parent;
			ZydisRegister BaseRegister;
			uint32_t Index;
			uint32_t Offset;
			uint32_t EndRegister;
			uint32_t LastValue;
			std::vector<InstructionInfo> Instructions;
			std::vector<ZydisRegister> SaveRegisters;

			explicit DecryptInfo(ptr_t parent, ZydisRegister BaseRegisterIn, uint32_t offset, uint32_t index)
				:Parent(parent),
				BaseRegister(BaseRegisterIn),
				Index(index),
				Offset(offset),

				EndRegister(0),
				LastValue(0)
			{

			}
			explicit DecryptInfo()
				: DecryptInfo(0, ZYDIS_REGISTER_RAX, 0, -1)
			{
			}
		};

	public:
		bool Analyse() override;
		bool Build(std::function< void(void*)> callback = nullptr) override;
		const ptr_t GetEntryPoint() const override { return mEntryPoint; }
	private:
		void analyseDecrypttions();

	public:
		explicit UObjectAnalysis(ptr_t entryPoint);
		~UObjectAnalysis();
	private:
		SelfMap<int32_t, DecryptInfo> mDecrypts;
		ptr_t mEntryPoint;
	};

}