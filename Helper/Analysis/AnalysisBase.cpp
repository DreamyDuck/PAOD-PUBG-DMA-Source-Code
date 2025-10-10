#include "Include/WinHeaders.hpp"
#include "AnalysisBase.h"

namespace IronMan::Analysis
{
	AnalysisBase::AnalysisBase(ptr_t base, ptr_t size)
	{
		mBase = base;
		mSize = size;
		mData = nullptr;
	}

	AnalysisBase::~AnalysisBase()
	{
	}

	ZydisRegister AnalysisBase::GpRegTo64(ZydisRegister reg)
	{
		if (inRange(reg, ZYDIS_REGISTER_AL, ZYDIS_REGISTER_BL))
			return static_cast<ZydisRegister>(ZYDIS_REGISTER_RAX + reg - ZYDIS_REGISTER_AL);
		else if (inRange(reg, ZYDIS_REGISTER_R8B, ZYDIS_REGISTER_R15B))
			return static_cast<ZydisRegister>(ZYDIS_REGISTER_R8 + reg - ZYDIS_REGISTER_R8B);

		// General purpose registers 16-bit
		else if (inRange(reg, ZYDIS_REGISTER_AX, ZYDIS_REGISTER_R15W))
			return static_cast<ZydisRegister>(ZYDIS_REGISTER_RAX + reg - ZYDIS_REGISTER_AX);

		// General purpose registers 32-bit
		else if (inRange(reg, ZYDIS_REGISTER_EAX, ZYDIS_REGISTER_R15D))
			return static_cast<ZydisRegister>(ZYDIS_REGISTER_RAX + reg - ZYDIS_REGISTER_EAX);

		// General purpose registers 64-bit
		else if (inRange(reg, ZYDIS_REGISTER_RAX, ZYDIS_REGISTER_R15))
			return static_cast<ZydisRegister>(ZYDIS_REGISTER_RAX + reg - ZYDIS_REGISTER_RAX);
		return reg;
	}

	asmjit::GpReg AnalysisBase::ToGpReg(ZydisRegister reg)
	{
		if (inRange(reg, ZYDIS_REGISTER_AL, ZYDIS_REGISTER_BL))
			return asmjit::x86RegData.gpbLo[reg - ZYDIS_REGISTER_AL];
		else if (inRange(reg, ZYDIS_REGISTER_AH, ZYDIS_REGISTER_BH))
			return asmjit::x86RegData.gpbHi[reg - ZYDIS_REGISTER_AH];
		else if (inRange(reg, ZYDIS_REGISTER_SPL, ZYDIS_REGISTER_R15B))
			return asmjit::x86RegData.gpbLo[reg - ZYDIS_REGISTER_AH];

		// General purpose registers 16-bit
		else if (inRange(reg, ZYDIS_REGISTER_AX, ZYDIS_REGISTER_R15W))
			return asmjit::x86RegData.gpw[reg - ZYDIS_REGISTER_AX];

		// General purpose registers 32-bit
		else if (inRange(reg, ZYDIS_REGISTER_EAX, ZYDIS_REGISTER_R15D))
			return asmjit::x86RegData.gpd[reg - ZYDIS_REGISTER_EAX];

		// General purpose registers 64-bit
		else if (inRange(reg, ZYDIS_REGISTER_RAX, ZYDIS_REGISTER_R15))
			return asmjit::x86RegData.gpq[reg - ZYDIS_REGISTER_RAX];

		// TODO:
		else
			assert(0);
		return asmjit::GpReg();
	}
}