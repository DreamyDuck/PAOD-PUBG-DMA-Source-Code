#pragma once
#include <stdint.h>
#include <string>
#include <memory>
#include <vector>
#include <limits>

namespace IronMan
{
	typedef std::numeric_limits<float> FltInfo;
	float const FLT_NAN = FltInfo::quiet_NaN();

	typedef std::numeric_limits<double> DbInfo;
	double const DBL_NAN = DbInfo::quiet_NaN();



	using ptr_t = uint64_t;
	using module_t = ptr_t;  // Module base pointer

	// Type of barrier
	enum eBarrier
	{
		wow_32_32 = 0,  // Both processes are WoW64 
		wow_64_64,      // Both processes are x64
		wow_32_64,      // Managing x64 process from WoW64 process
		wow_64_32,      // Managing WOW64 process from x64 process
	};

	struct Wow64Barrier
	{
		eBarrier type = wow_32_32;
		bool sourceWow64 = false;
		bool targetWow64 = false;
		bool x86OS = false;
		bool mismatch = false;
	};


	// Module type
	enum eModType
	{
		mt_mod32,       // 64 bit module
		mt_mod64,       // 32 bit module
		mt_default,     // type is deduced from target process
		mt_unknown      // Failed to detect type
	};

	// Module search method
	enum eModSeachType
	{
		LdrList,        // InLoadOrder list
		Sections,       // Scan for section objects
		PEHeaders,      // Scan for PE headers in memory
	};

	// Module info
	struct ModuleData
	{
		module_t baseAddress;   // Base image address
		std::wstring name;      // File name
		std::wstring fullPath;  // Full file path
		uint32_t size;          // Size of image
		eModType type;          // Module type
		ptr_t ldrPtr;           // LDR_DATA_TABLE_ENTRY_BASE_T address
		bool manual;            // Image is manually mapped
		char unknown[7];

		bool operator ==(const ModuleData& other) const
		{
			return (baseAddress == other.baseAddress);
		}

		bool operator <(const ModuleData& other)
		{
			return baseAddress < other.baseAddress;
		}
	};
	using ModuleDataPtr = std::shared_ptr<const ModuleData>;

	enum MEMORY_INFORMATION_CLASS
	{
		MemoryBasicInformation = 0,
		MemoryWorkingSetList,
		MemorySectionName,
		MemoryBasicVlmInformation,
		MemoryWorkingSetExList
	};

	enum SECTION_INFORMATION_CLASS
	{
		SectionBasicInformation,
		SectionImageInformation
	};

	enum POOL_TYPE
	{
		NonPagedPool,
		PagedPool,
		NonPagedPoolMustSucceed,
		DontUseThisType,
		NonPagedPoolCacheAligned,
		PagedPoolCacheAligned,
		NonPagedPoolCacheAlignedMustS
	};

	//
	// Loader related
	//
	enum _LDR_DDAG_STATE
	{
		LdrModulesMerged = -5,
		LdrModulesInitError = -4,
		LdrModulesSnapError = -3,
		LdrModulesUnloaded = -2,
		LdrModulesUnloading = -1,
		LdrModulesPlaceHolder = 0,
		LdrModulesMapping = 1,
		LdrModulesMapped = 2,
		LdrModulesWaitingForDependencies = 3,
		LdrModulesSnapping = 4,
		LdrModulesSnapped = 5,
		LdrModulesCondensed = 6,
		LdrModulesReadyToInit = 7,
		LdrModulesInitializing = 8,
		LdrModulesReadyToRun = 9
	};

	enum _LDR_DLL_LOAD_REASON
	{
		LoadReasonStaticDependency = 0,
		LoadReasonStaticForwarderDependency = 1,
		LoadReasonDynamicForwarderDependency = 2,
		LoadReasonDelayloadDependency = 3,
		LoadReasonDynamicLoad = 4,
		LoadReasonAsImageLoad = 5,
		LoadReasonAsDataLoad = 6,
		LoadReasonUnknown = -1
	};

	enum CoreState
	{
		CoreState_None,
		CoreState_ScanPatterns,
		CoreState_UEDecryptBuilder,
		CoreState_WaitingForAnimation,
		CoreState_CacheObjects,
		CoreState_LoadResources,
		CoreState_LoadItems,
		CoreState_WaitingForMenuShow,
		CoreState_LoadCompleted,
		CoreState_LoadError
	};

	struct ServerResult
	{
		char username[64];
		char password[64];
		char md5[64];
		char serverIP[64];
		char ntpServer[64];
		char deviceName[64];
		char cfile[260];
		char unknown[4];
		unsigned long long serverTime;
		unsigned long long currentServerTime;
		unsigned long long beijingTime;
		unsigned long long expireDate;
		bool English;
		char unknown2[3];
		int Version=3;
	};
}

extern IronMan::ServerResult g_ServerResult;
