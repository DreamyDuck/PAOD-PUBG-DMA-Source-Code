#include "Include/WinHeaders.hpp"
#include "UEncrypt.h"
#include "Core/Core.hpp"
#include "Core/PatternLoader.h"

namespace IronMan::Core::SDK
{
	using namespace Analysis;

	uint8_t* UEDecryptBuilder::GlobalFunctionBuffer = nullptr;
	ptr_t UEDecryptBuilder::BufferUsedSize = 6;
	std::unordered_map<DecryptFunc, std::unique_ptr<UEDecryptBuilder>> g_DecryptFuncs;
	std::vector<std::unique_ptr<AnalysisBase>> g_DecryptAnalysis;



	bool UEDecryptBuilder::Initialize()
	{
		if (!GlobalFunctionBuffer)
		{
			GlobalFunctionBuffer = (uint8_t*)VirtualAlloc(NULL, 0x10000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if (!GlobalFunctionBuffer)
			{
				CONSOLE_INFO("GlobalFunctionBuffer Allocate Failed!");
				return false;
			}
		}
		DWORD old;
		VirtualProtect(GlobalFunctionBuffer, 0x10000, PAGE_EXECUTE_READWRITE, &old);
		ZeroMemory(GlobalFunctionBuffer, 0x10000);
		g_DecryptFuncs.clear();
		g_DecryptAnalysis.clear();
		BufferUsedSize = 6;
		CONSOLE_INFO("GlobalFunctionBuffer:%p", GlobalFunctionBuffer);
		g_DecryptFuncs.emplace(DecryptFunc::UObjectFlags, new UEDecryptBuilder(g_PatternData.decryptUObject, g_PatternData.UObjectSort[0], BuildType::UObject));
		g_DecryptFuncs.emplace(DecryptFunc::UObjectNameNumber, new UEDecryptBuilder(g_PatternData.decryptUObject, g_PatternData.UObjectSort[1], BuildType::UObject));
		g_DecryptFuncs.emplace(DecryptFunc::UObjectNameComparisonIndex, new UEDecryptBuilder(g_PatternData.decryptUObject, g_PatternData.UObjectSort[2], BuildType::UObject));
		g_DecryptFuncs.emplace(DecryptFunc::UObjectOuter, new UEDecryptBuilder(g_PatternData.decryptUObject, g_PatternData.UObjectSort[3], BuildType::UObject));
		g_DecryptFuncs.emplace(DecryptFunc::UObjectClass, new UEDecryptBuilder(g_PatternData.decryptUObject, g_PatternData.UObjectSort[4], BuildType::UObject));
		g_DecryptFuncs.emplace(DecryptFunc::UObjectIndex, new UEDecryptBuilder(g_PatternData.decryptUObjectIndex, 0, BuildType::UObject));
		CONSOLE_INFO(("g_PatternData.UObjectSize:%p"), g_PatternData.UObjectSize);
		CONSOLE_INFO(("g_PatternData.offset_UObjectFlags:%p"), g_PatternData.offset_UObjectFlags);
		CONSOLE_INFO(("g_PatternData.offset_UObjectNameNumber:%p"), g_PatternData.offset_UObjectNameNumber);
		CONSOLE_INFO(("g_PatternData.offset_UObjectNameComparisonIndex:%p"), g_PatternData.offset_UObjectNameComparisonIndex);
		CONSOLE_INFO(("g_PatternData.offset_UObjectClass:%p"), g_PatternData.offset_UObjectClass);
		CONSOLE_INFO(("g_PatternData.offset_UObjectIndex:%p"), g_PatternData.offset_UObjectIndex);

		g_DecryptFuncs.emplace(DecryptFunc::General, new UEDecryptBuilder(g_PatternData.decryptOwningGameInstance, 0));
		CONSOLE_INFO("UObjectFlags:%p", g_DecryptFuncs[DecryptFunc::UObjectFlags]->functionBuf);
		CONSOLE_INFO("UObjectNameNumber:%p", g_DecryptFuncs[DecryptFunc::UObjectNameNumber]->functionBuf);
		CONSOLE_INFO("UObjectNameComparisonIndex:%p", g_DecryptFuncs[DecryptFunc::UObjectNameComparisonIndex]->functionBuf);
		CONSOLE_INFO("UObjectOuter:%p", g_DecryptFuncs[DecryptFunc::UObjectOuter]->functionBuf);
		CONSOLE_INFO("UObjectClass:%p", g_DecryptFuncs[DecryptFunc::UObjectClass]->functionBuf);
		CONSOLE_INFO("UObjectIndex:%p", g_DecryptFuncs[DecryptFunc::UObjectIndex]->functionBuf);
		CONSOLE_INFO("General:%p", g_DecryptFuncs[DecryptFunc::General]->functionBuf);
		VirtualProtect(GlobalFunctionBuffer, 0x10000, PAGE_EXECUTE_READ, &old);
		return true;
	}

	bool UEDecryptBuilder::SetExchange()
	{
		auto back1 = g_DecryptFuncs[DecryptFunc::UObjectNameNumber]->functionBuf;
		auto back2 = g_DecryptFuncs[DecryptFunc::UObjectNameComparisonIndex]->functionBuf;
		g_DecryptFuncs[DecryptFunc::UObjectNameComparisonIndex]->functionBuf = back1;
		g_DecryptFuncs[DecryptFunc::UObjectNameNumber]->functionBuf = back2;
		g_PatternData.offset_ItemTableRowBase_ItemID += 4;
		g_PatternData.offset_ATslWeapon_FiringAttachPoint += 4;
		g_PatternData.offset_ATslWeapon_MuzzleAttachPoint += 4;
		g_PatternData.offset_SkeletalMeshSocket_BoneName += 4;
		g_PatternData.offset_SkeletalMeshSocket_SocketName += 4;
		g_PatternData.offset_UStaticMeshSocket_SocketName += 4;
		g_PatternData.offset_ATslWeapon_Gun_ScopingAttachPoint += 4;
		g_PatternData.offset_ATslWeapon_Gun_ScopingFocusPoint += 4;
		g_PatternData.offset_AController_StateName += 4;
		return true;
	}

	UEDecryptBuilder::UEDecryptBuilder(ptr_t StartPosition, int DecryptIndex, BuildType type, std::function< void(void*)> callback)
	{
		int count = 0;
	retryDec:
		auto analysis = std::find_if(g_DecryptAnalysis.begin(), g_DecryptAnalysis.end(), [&](const auto& analysis) {return analysis->GetEntryPoint() == StartPosition; });
		if (analysis == g_DecryptAnalysis.end())// not found!
		{
			AnalysisBase* _analysis = nullptr;
			switch (type)
			{
			case BuildType::Default:
				_analysis = new DecryptAnalysis(StartPosition);
				break;
			case BuildType::UObject:
				_analysis = new UObjectAnalysis(StartPosition);
				break;
			default:
				break;
			}
			if (_analysis && _analysis->Analyse() && _analysis->Build(callback))
			{
				g_DecryptAnalysis.push_back(std::unique_ptr<AnalysisBase>(std::move(_analysis)));
				analysis = g_DecryptAnalysis.end() - 1;
			}
			else
				goto Exit;
		}
		auto& functions = (*analysis)->GetDecryptFunctions();
		if (DecryptIndex < functions.size())
		{
			const auto& a = functions[DecryptIndex];
			for (int i = 0; i < 6; i++)
				(*a)->int3();

			functionBuf = GlobalFunctionBuffer + BufferUsedSize;
			BufferUsedSize += (*a)->relocCode(functionBuf);
			CONSOLE_INFO("DecryptFunction:%p", functionBuf);
			return;
		}
	Exit:
		CONSOLE_INFO("Error:Can't Build Dec!");
		if (count < 3)
		{
			count++;
			Sleep(1000);
			goto retryDec;
		}
		return;
	}
	UEDecryptBuilder::~UEDecryptBuilder()
	{
		//VirtualFree(functionBuf, 0, MEM_RELEASE);
	}


}