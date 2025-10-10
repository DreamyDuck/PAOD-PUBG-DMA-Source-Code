// ReSharper disable CppCStyleCast
#include "DMAHandler.h"
#include <filesystem>
#include <algorithm>
#include <unordered_set>
#include <random>
#include <ShlObj.h>
#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xA) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))
namespace IronMan
{
	const uint32_t VMMDLL_FLAG = VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOCACHEPUT | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_NOPAGING_IO | VMMDLL_FLAG_NO_PREDICTIVE_READ;
	ptr_t FindPatternFrom(const char* pattern, ptr_t start, ptr_t end)
	{
		char* pat = const_cast<char*>(pattern);
		ptr_t firstMatch = 0;
		ptr_t rangeEnd = end;
		for (auto pCur = start; pCur < end; pCur++)
		{
			if (!*pat)
				return firstMatch;

			if (*(BYTE*)pat == '\?' || *(BYTE*)pCur == getByte(pat))
			{
				if (!firstMatch)
					firstMatch = pCur;

				if (!pat[2])
					return firstMatch;

				if (*(BYTE*)pat == '\?')
					pat += 2;
				else
					pat += 3;
			}
			else
			{
				pat = const_cast<char*>(pattern);
				firstMatch = 0;
			}
		}
		return 0;
	}

	DMAHandler& DMAHandler::Instance()
	{
		static DMAHandler instance;
		return instance;
	}

	std::vector<int> DMAHandler::GetProcessidByEnum(const char* process_name)
	{
		std::vector<int> list = { };
		if (!DMA_HANDLE)
			return list;
		auto Exist = GetProcessidByName(process_name);
		if (!Exist)
			return list;
		SIZE_T pcPIDs = 0;
		VMMDLL_PidList(DMA_HANDLE, nullptr, &pcPIDs);
		if (!pcPIDs)
			return list;
		bool ret = false;
		DWORD* pPIDs = (DWORD*)malloc(pcPIDs * sizeof(DWORD));
		ret = VMMDLL_PidList(DMA_HANDLE, pPIDs, &pcPIDs);
		if (ret)
		{
			for (int i = 0; i < pcPIDs; i++)  // 遍历所有进程
			{
				try
				{
					VMMDLL_PROCESS_INFORMATION ProcessInformation = { 0 };
					ProcessInformation.magic = VMMDLL_PROCESS_INFORMATION_MAGIC;
					ProcessInformation.wVersion = VMMDLL_PROCESS_INFORMATION_VERSION;
					SIZE_T pcbProcessInformation = sizeof(VMMDLL_PROCESS_INFORMATION);
					VMMDLL_ProcessGetInformation(DMA_HANDLE, pPIDs[i], &ProcessInformation, &pcbProcessInformation);

					if (ProcessInformation.szName == std::string(process_name))
					{
						list.push_back(pPIDs[i]);
					}
				}
				catch (...)
				{
					CONSOLE_INFO2(u8"ProcessGetInformation 出现异常");
					continue;
				}
			}
		}
		free(pPIDs);
		return list;
	}

	DWORD DMAHandler::GetProcessidByName(const char* process_name)
	{
		if (!DMA_HANDLE)
			return 0;
		DWORD pid = 0;
		VMMDLL_PidGetFromName(DMA_HANDLE, (LPSTR)process_name, &pid);
		return pid;
	}

	std::shared_ptr<std::vector<int>>DMAHandler::GetPidListFromName(std::string name)
	{
		std::unique_ptr<PVMMDLL_PROCESS_INFORMATION> process_info = std::make_unique<PVMMDLL_PROCESS_INFORMATION>();
		DWORD total_processes = 0;
		std::shared_ptr <std::vector<int>> list = std::make_shared<std::vector<int>>();
		if (!DMA_HANDLE)
			return list;
		auto Exist = GetProcessidByName(name.c_str());
		if (!Exist)
			return list;
		try
		{
			if (!VMMDLL_ProcessGetInformationAll(DMA_HANDLE, &(*process_info), &total_processes))
			{
				CONSOLE_INFO2(u8"[!] 无法获取进程列表\n");
				return list;
			}
		}
		catch (...)
		{
			CONSOLE_INFO2(u8"[!] 无法获取进程列表 2\n");
			return list;
		}

		if (total_processes < 10000)
		{
			for (size_t i = 0; i < total_processes; i++)
			{
				try
				{
					auto process = (*process_info)[i];
					std::string processName(process.szNameLong);
					if (processName.find(name) != std::string::npos)
						list->push_back(process.dwPID);
				}
				catch (...)
				{
					continue;
				}
			}
		}
		if (process_info != nullptr)
			VMMDLL_MemFree(*process_info);

		return list;
	}

	bool DMAHandler::AttachProcess(DWORD pid, std::string Name)
	{
		const std::wstring str(Name.begin(), Name.end());
		processInfo.name = Name;
		processInfo.wname = str.c_str();
		processInfo.pid = pid;
		PROCESS_INITIALIZED = TRUE;
		return true;
	}

	DMAHandler::DMAHandler()
	{
		if (!DMA_HANDLE)
		{
			modules.VMM = LoadLibraryA("vmm.dll");
			modules.FTD3XX = LoadLibraryA("FTD3XX.dll");
			modules.LEECHCORE = LoadLibraryA("leechcore.dll");

			if (!modules.VMM || !modules.FTD3XX || !modules.LEECHCORE)
				exit(-1);

			LPCSTR args[] = { (LPCSTR)"", (LPCSTR)"-device", (LPCSTR)"fpga", (LPCSTR)"-norefresh" ,(LPCSTR)"",(LPCSTR)"" };
			DWORD argc = 4;
			//if (!DumpMemoryMap())
			//{
			//}
			//else
			//{
			//	std::string directoryPath;
			//	directoryPath += "C:\\mmap.txt";
			//	args[argc++] = const_cast<LPSTR>("-memmap");
			//	args[argc++] = const_cast<LPSTR>(directoryPath.c_str());
			//}

			DMA_HANDLE = VMMDLL_Initialize(argc, args);
			if (!DMA_HANDLE)
			{
				Sleep(3000);
				//TerminateProcess(GetCurrentProcess(), -1);
			}

			ULONG64 FPGA_ID = 0, DEVICE_ID = 0, REFRESH_ENABLED = 0;

			VMMDLL_ConfigGet(DMA_HANDLE, LC_OPT_FPGA_FPGA_ID, &FPGA_ID);
			VMMDLL_ConfigGet(DMA_HANDLE, LC_OPT_FPGA_DEVICE_ID, &DEVICE_ID);
			VMMDLL_ConfigGet(DMA_HANDLE, VMMDLL_OPT_REFRESH_ALL, &REFRESH_ENABLED);

			ScatterReadArray = std::make_shared<std::unordered_map<void*, std::vector<ScatterStruct>>>();
			ScatterReadArray->reserve(20);
			ScatterWriteArray = std::make_shared<std::unordered_map<void*, std::vector<ScatterStruct>>>();
			ScatterWriteArray->reserve(20);
		}
	}

	DMAHandler::~DMAHandler()
	{
		CONSOLE_INFO2("~DMAHandler");
		this->closeDMA();
		//TerminateProcess((HANDLE)-1, 1);
	}

	BOOL DMAHandler::FreshAll()
	{
		if (!DMA_HANDLE)
			return false;
		return VMMDLL_ConfigSet(DMA_HANDLE, VMMDLL_OPT_REFRESH_ALL, 0);
	}

	BOOL DMAHandler::FreshFREQ()
	{
		if (!DMA_HANDLE)
			return false;
		//BOOL Res1 = VMMDLL_ConfigSet(DMA_HANDLE, VMMDLL_OPT_REFRESH_FREQ_MEM_PARTIAL, 1);
		BOOL Res1 = true;
		BOOL Res2 = VMMDLL_ConfigSet(DMA_HANDLE, VMMDLL_OPT_REFRESH_FREQ_TLB_PARTIAL, 1);
		return Res1 && Res2;
	}

	bool DMAHandler::DumpMemoryMap()
	{
		LPCSTR args[] = { (LPCSTR)"",(LPCSTR)"-device", (LPCSTR)"fpga" };
		if (const VMM_HANDLE handle = VMMDLL_Initialize(3, args)) {
			PVMMDLL_MAP_PHYSMEM pPhysMemMap = nullptr;
			if (VMMDLL_Map_GetPhysMem(handle, &pPhysMemMap)) {
				if (pPhysMemMap->dwVersion != VMMDLL_MAP_PHYSMEM_VERSION) {
					VMMDLL_MemFree(pPhysMemMap);
					VMMDLL_Close(handle);
					return false;
				}
				if (pPhysMemMap->cMap == 0)
				{
					VMMDLL_MemFree(pPhysMemMap);
					VMMDLL_Close(handle);
					return false;
				}
				//Dump map to file
				std::stringstream sb;
				for (DWORD i = 0; i < pPhysMemMap->cMap; i++) {
					sb << std::setfill('0') << std::setw(4) << i << "  " << std::hex << pPhysMemMap->pMap[i].pa << "  -  " << (pPhysMemMap->pMap[i].pa + pPhysMemMap->pMap[i].cb - 1) << "  ->  " << pPhysMemMap->pMap[i].pa << std::endl;
				}
				std::ofstream nFile("C:\\mmap.txt");
				nFile << sb.str();
				nFile.close();

				VMMDLL_MemFree(pPhysMemMap);
				//Little sleep to make sure it's written to file.
				Sleep(10);
			}
			VMMDLL_Close(handle);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool DMAHandler::isInitialized() const
	{
		return DMA_HANDLE && PROCESS_INITIALIZED;
	}

	std::string DMAHandler::QueryValue(const char* path, e_registry_type type)
	{
		if (!DMA_HANDLE)
			return "19045";
		BYTE buffer[0x128];
		DWORD _type = (DWORD)type;
		DWORD size = sizeof(buffer);
		BOOL Result = FALSE;
		auto PathString = CC_TO_LPSTR(path);
		try
		{
			Result = VMMDLL_WinReg_QueryValueExU(DMA_HANDLE, PathString, &_type, buffer, &size);
		}
		catch (...)
		{
			CONSOLE_INFO2("WinReg_QueryValueExU Exception!");
		}
		if (PathString)
			delete PathString;
		if (!Result)
		{
			CONSOLE_INFO2(u8"[!] 无法获取系统版本号 \r\n如果系统低于 20000 输入 1 否则输入 2\r\n进行下一步\r\n");
			std::string in;
		rewait:
			if (GetAsyncKeyState('1') || GetAsyncKeyState(97))
			{
				CONSOLE_INFO2(u8"系统版本号:19045");
				return "19045";
			}
			else if (GetAsyncKeyState('2') || GetAsyncKeyState(65))
			{
				CONSOLE_INFO2(u8"系统版本号:22000");
				return "22300";
			}
			goto rewait;
		}

		std::wstring wstr = std::wstring((wchar_t*)buffer);
		return std::string(wstr.begin(), wstr.end());
	}

	ptr_t DMAHandler::getKeyPtr()
	{
		return gafAsyncKeyStateExport;
	}

	bool DMAHandler::InitKeyboard()
	{
		if (!DMA_HANDLE)
			return false;
		std::string win = QueryValue("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\CurrentBuild", e_registry_type::sz);
		int Winver = 0;
		if (!win.empty())
			Winver = std::stoi(win);
		else
		{
			CONSOLE_INFO2(u8"无法获取系统版本号");
			return false;
		}
		CONSOLE_INFO("winver:%d", Winver);
		do
		{
			win_logon_pid = GetProcessidByName("winlogon.exe");
			Sleep(1);
		} while (win_logon_pid == 0);
		if (Winver > 26000)
		{
			void* Dumpwin32 = NULL;
			void* Dumpwin32kbase = NULL;
			SysStruct SysStructwin32 = SysStruct();
			SysStruct SysStructwin32base = SysStruct();
			while (SysStructwin32.pBase == 0 || SysStructwin32.pSize == 0)
			{
				SysStructwin32 = GetSystemModuleAndSize("win32k.sys");
				Sleep(10);
			}
			while (SysStructwin32base.pBase == 0 || SysStructwin32base.pSize == 0)
			{
				SysStructwin32base = GetSystemModuleAndSize("win32kbase.sys");
				Sleep(10);
			}
			Dumpwin32 = malloc(SysStructwin32.pSize);
			Dumpwin32kbase = malloc(SysStructwin32base.pSize);

			uintptr_t g_session_global_slots = 0;
			uintptr_t g_AsyncKeyState_Offset = 0;

			auto pids = GetPidListFromName("csrss.exe");
			CONSOLE_INFO("pid:%d", pids->size());
			for (size_t i = 0; i < pids->size(); i++)
			{
				auto pid = (*pids)[i];
				if (Dumpwin32)
				{
					ZeroMemory(Dumpwin32, SysStructwin32.pSize);
					GetDMA().ReadFull(pid, SysStructwin32.pBase, (ptr_t)Dumpwin32, SysStructwin32.pSize);
					CONSOLE_INFO("[%d]win32:%p|%x|%p", i, SysStructwin32.pBase, SysStructwin32.pSize, *(ptr_t*)Dumpwin32);
				}
				if (Dumpwin32kbase)
				{
					ZeroMemory(Dumpwin32kbase, SysStructwin32base.pSize);
					GetDMA().ReadFull(pid, SysStructwin32base.pBase, (ptr_t)Dumpwin32kbase, SysStructwin32base.pSize);
					CONSOLE_INFO("[%d]win32kbase:%p|%x|%p", i, SysStructwin32base.pBase, SysStructwin32base.pSize, *(ptr_t*)Dumpwin32kbase);
				}
				if (*(ptr_t*)Dumpwin32 == 0x0000000300905A4D && *(ptr_t*)Dumpwin32kbase == 0x0000000300905A4D)
				{
					auto FindUserSessionState = FindPatternFrom("48 8B 05 ? ? ? ? FF C9 48 8B 04 C8 C3", (ptr_t)Dumpwin32, (ptr_t)Dumpwin32 + SysStructwin32.pSize);
					if (FindUserSessionState != 0)
					{
						g_session_global_slots = SysStructwin32.pBase + (FindUserSessionState - (ptr_t)Dumpwin32);
						g_session_global_slots = g_session_global_slots + GetDMA().Read<LONG>(pid, g_session_global_slots + 3) + 7;
						CONSOLE_INFO("g_session_global_slots:%p", g_session_global_slots);
					}
					else
						continue;
					auto FindOffsetUserSessionState = FindPatternFrom("B9 ? ? ? ? ? ? ? ? ? ? ? ? 41 0F ? ? 66", (ptr_t)Dumpwin32kbase, (ptr_t)Dumpwin32kbase + SysStructwin32base.pSize);
					if (FindOffsetUserSessionState != 0)
					{
						FindOffsetUserSessionState = SysStructwin32base.pBase + (FindOffsetUserSessionState - (ptr_t)Dumpwin32kbase);
						g_AsyncKeyState_Offset = GetDMA().Read<LONG>(pid, FindOffsetUserSessionState + 9);
						CONSOLE_INFO("g_AsyncKeyState_Offset:%x", g_AsyncKeyState_Offset);
					}
					else
						continue;
				}
				else
					continue;
				ptr_t gLowSessionGlobalSlots = GetDMA().Read<uintptr_t>(pid, g_session_global_slots);
				uintptr_t user_session_state = 0;
				for (int i = 0; i < 100; i++)
				{
					user_session_state = GetDMA().Read<uintptr_t>(pid,
						GetDMA().Read<uintptr_t>(pid,
							GetDMA().Read<uintptr_t>(pid, g_session_global_slots) + i * 8));
					CONSOLE_INFO("%d:%p", i, user_session_state);
					if (user_session_state)
						break;
				}

				gafAsyncKeyStateExport = user_session_state + g_AsyncKeyState_Offset;
				//gafAsyncKeyStateExport = user_session_state + g_AsyncKeyState_Offset;
				if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF)
					break;
			}

			if (Dumpwin32)
				free(Dumpwin32);
			if (Dumpwin32kbase)
				free(Dumpwin32kbase);

			if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF)
				return true;

			return false;
		}
		else if (Winver > 22000)
		{
			void* Dumpwin32ksgd = NULL;
			void* Dumpwin32kbase = NULL;
			SysStruct SysStructwin32ksgd = SysStruct();
			SysStruct SysStructwin32base = SysStruct();
			while (SysStructwin32ksgd.pBase == 0 || SysStructwin32ksgd.pSize == 0)
			{
				SysStructwin32ksgd = GetSystemModuleAndSize("win32ksgd.sys");
				Sleep(10);
			}
			while (SysStructwin32base.pBase == 0 || SysStructwin32base.pSize == 0)
			{
				SysStructwin32base = GetSystemModuleAndSize("win32kbase.sys");
				Sleep(10);
			}
			Dumpwin32ksgd = malloc(SysStructwin32ksgd.pSize);
			Dumpwin32kbase = malloc(SysStructwin32base.pSize);

			uintptr_t g_session_global_slots = 0;
			uintptr_t g_AsyncKeyState_Offset = 0;

			auto pids = GetPidListFromName("csrss.exe");
			CONSOLE_INFO("pid:%d", pids->size());
			for (size_t i = 0; i < pids->size(); i++)
			{
				auto pid = (*pids)[i];
				if (Dumpwin32ksgd)
				{
					ZeroMemory(Dumpwin32ksgd, SysStructwin32ksgd.pSize);
					GetDMA().ReadFull(pid, SysStructwin32ksgd.pBase, (ptr_t)Dumpwin32ksgd, SysStructwin32ksgd.pSize);
					CONSOLE_INFO("[%d]win32ksgd:%p|%x|%p", i, SysStructwin32ksgd.pBase, SysStructwin32ksgd.pSize, *(ptr_t*)Dumpwin32ksgd);
				}
				if (Dumpwin32kbase)
				{
					ZeroMemory(Dumpwin32kbase, SysStructwin32base.pSize);
					GetDMA().ReadFull(pid, SysStructwin32base.pBase, (ptr_t)Dumpwin32kbase, SysStructwin32base.pSize);
					CONSOLE_INFO("[%d]win32kbase:%p|%x|%p", i, SysStructwin32base.pBase, SysStructwin32base.pSize, *(ptr_t*)Dumpwin32kbase);
				}
				if (*(ptr_t*)Dumpwin32ksgd == 0x0000000300905A4D && *(ptr_t*)Dumpwin32kbase == 0x0000000300905A4D)
				{
					auto FindUserSessionState = FindPatternFrom("EB ? 8D 48 FF 48 8B ? ? ? ? ? 48 8B 04", (ptr_t)Dumpwin32ksgd, (ptr_t)Dumpwin32ksgd + SysStructwin32ksgd.pSize);
					if (FindUserSessionState != 0)
					{
						g_session_global_slots = SysStructwin32ksgd.pBase + (FindUserSessionState - (ptr_t)Dumpwin32ksgd) + 5;
						g_session_global_slots = g_session_global_slots + GetDMA().Read<LONG>(pid, g_session_global_slots + 3) + 7;
						CONSOLE_INFO("g_session_global_slots:%p", g_session_global_slots);
					}
					else
						continue;
					auto FindOffsetUserSessionState = FindPatternFrom("B9 ? ? ? ? ? ? ? ? ? ? ? ? 41 0F ? ? 66", (ptr_t)Dumpwin32kbase, (ptr_t)Dumpwin32kbase + SysStructwin32base.pSize);
					if (FindOffsetUserSessionState != 0)
					{
						FindOffsetUserSessionState = SysStructwin32base.pBase + (FindOffsetUserSessionState - (ptr_t)Dumpwin32kbase);
						g_AsyncKeyState_Offset = GetDMA().Read<LONG>(pid, FindOffsetUserSessionState + 9);
						CONSOLE_INFO("g_AsyncKeyState_Offset:%x", g_AsyncKeyState_Offset);
					}
					else
						continue;
				}
				else
					continue;
				ptr_t gLowSessionGlobalSlots = GetDMA().Read<uintptr_t>(pid, g_session_global_slots);
				uintptr_t user_session_state = 0;
				for (int i = 0; i < 100; i++)
				{
					user_session_state = GetDMA().Read<uintptr_t>(pid,
						GetDMA().Read<uintptr_t>(pid,
							GetDMA().Read<uintptr_t>(pid, g_session_global_slots) + i * 8));
					CONSOLE_INFO("%d:%p", i, user_session_state);
					if (user_session_state)
						break;
				}

				gafAsyncKeyStateExport = user_session_state + g_AsyncKeyState_Offset;
				//gafAsyncKeyStateExport = user_session_state + g_AsyncKeyState_Offset;
				if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF)
					break;
			}

			if (Dumpwin32ksgd)
				free(Dumpwin32ksgd);
			if (Dumpwin32kbase)
				free(Dumpwin32kbase);

			if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF)
				return true;

			return false;
		}
		else
		{
			bool result = false;
			DWORD cProcessInformation = 0;
			PVMMDLL_PROCESS_INFORMATION pProcessInformationEntry, pProcessInformationAll = NULL;
			try
			{
				result = VMMDLL_ProcessGetInformationAll(DMA_HANDLE, &pProcessInformationAll, &cProcessInformation);
				if (result) {
					for (int i = 0; i < (int)cProcessInformation; i++) {
						pProcessInformationEntry = &pProcessInformationAll[i];
						if (strcmp(pProcessInformationEntry->szName, "csrss.exe") == 0 || strcmp(pProcessInformationEntry->szName, "winlogon.exe") == 0) {
							if (strcmp(pProcessInformationEntry->szName, "winlogon.exe") == 0)
								gafAsyncKeyStateExport = VMMDLL_ProcessGetProcAddressU(DMA_HANDLE, pProcessInformationEntry->dwPID | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, (LPSTR)"win32kbase.sys", (LPSTR)"gafAsyncKeyState");
							else
								gafAsyncKeyStateExport = VMMDLL_ProcessGetProcAddressU(DMA_HANDLE, pProcessInformationEntry->dwPID, (LPSTR)"win32kbase.sys", (LPSTR)"gafAsyncKeyState");

							//得到函数地址
							if (gafAsyncKeyStateExport)
							{
								BYTE keyStateBitmap[255];
								if (strcmp(pProcessInformationEntry->szName, "winlogon.exe") == 0)
									result = VMMDLL_MemReadEx(DMA_HANDLE, pProcessInformationEntry->dwPID | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, gafAsyncKeyStateExport, (PBYTE)&keyStateBitmap, sizeof(keyStateBitmap), NULL, VMMDLL_FLAG);
								else
									result = VMMDLL_MemReadEx(DMA_HANDLE, pProcessInformationEntry->dwPID, gafAsyncKeyStateExport, (PBYTE)&keyStateBitmap, sizeof(keyStateBitmap), NULL, VMMDLL_FLAG);
								//得到bitmap
								if (keyStateBitmap[0x24] == '\x2')
								{
									VMMDLL_MemFree(pProcessInformationAll);
									return true;
								}
							}
						}
					}
					VMMDLL_MemFree(pProcessInformationAll);
				}
				else
				{
					CONSOLE_INFO2(u8"GetInformationAll:获取失败");
					gafAsyncKeyStateExport = VMMDLL_ProcessGetProcAddressU(DMA_HANDLE, win_logon_pid | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, (LPSTR)"win32kbase.sys", (LPSTR)"gafAsyncKeyState");
				}
			}
			catch (...)
			{
				CONSOLE_INFO2(u8"热键查找:异常");
				gafAsyncKeyStateExport = VMMDLL_ProcessGetProcAddressU(DMA_HANDLE, win_logon_pid | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, (LPSTR)"win32kbase.sys", (LPSTR)"gafAsyncKeyState");
			}
			CONSOLE_INFO("gafAsyncKeyStateExport:%p", gafAsyncKeyStateExport);
			if (gafAsyncKeyStateExport == 0)
			{
				gafAsyncKeyStateExport = VMMDLL_ProcessGetProcAddressU(DMA_HANDLE, win_logon_pid | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, (LPSTR)"win32kbase.sys", (LPSTR)"gafAsyncKeyState");
				if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF)
					return true;
				void* Dumpwin32kbase = NULL;
				SysStruct SysStructwin32k = SysStruct();
				while (SysStructwin32k.pBase == 0 && SysStructwin32k.pSize == 0)
				{
					SysStructwin32k = GetSystemModuleAndSize("win32kbase.sys");
					if (SysStructwin32k.pBase != 0 && SysStructwin32k.pSize != 0)
						break;
					Sleep(10);
				}
				CONSOLE_INFO("SysStructwin32k:%p|%d", SysStructwin32k.pBase, SysStructwin32k.pSize);
				Dumpwin32kbase = malloc(SysStructwin32k.pSize);
				if (!Dumpwin32kbase)
				{
					CONSOLE_INFO2(u8"获取内核信息失败 4!");
					return false;
				}
				auto pids = GetPidListFromName("csrss.exe");
				for (size_t i = 0; i < pids->size(); i++)
				{
					auto pid = (*pids)[i];
					ZeroMemory(Dumpwin32kbase, SysStructwin32k.pSize);
					GetDMA().ReadFull(pid, SysStructwin32k.pBase, (ptr_t)Dumpwin32kbase, SysStructwin32k.pSize);
					CONSOLE_INFO("%d | NextState:%p", pid, *(ptr_t*)Dumpwin32kbase);
					if (*(ptr_t*)Dumpwin32kbase == 0)
					{
						GetDMA().ReadFull(win_logon_pid, SysStructwin32k.pBase, (ptr_t)Dumpwin32kbase, SysStructwin32k.pSize);
						CONSOLE_INFO("2:%d | NextState:%p", pid, *(ptr_t*)Dumpwin32kbase);
					}

					try
					{
						if (*(ptr_t*)Dumpwin32kbase == 0x0000000300905A4D)
						{
							auto FindgafAsyncKeyState = FindPatternFrom("48 8B ? 48 8D ? ? ? ? ? 48 C1 EA 02", (ptr_t)Dumpwin32kbase, (ptr_t)Dumpwin32kbase + SysStructwin32k.pSize);
							if (FindgafAsyncKeyState != 0)
							{
								FindgafAsyncKeyState = SysStructwin32k.pBase + (FindgafAsyncKeyState - (ptr_t)Dumpwin32kbase) + 3;
								ULONG g_AsyncKeyState_Offset = GetDMA().Read<LONG>(pid, FindgafAsyncKeyState + 3);
								gafAsyncKeyStateExport = FindgafAsyncKeyState + g_AsyncKeyState_Offset + 7;
								if (gafAsyncKeyStateExport)
								{
									BYTE keyStateBitmap[255];
									result = VMMDLL_MemReadEx(DMA_HANDLE, pid, gafAsyncKeyStateExport, (PBYTE)&keyStateBitmap, sizeof(keyStateBitmap), NULL, VMMDLL_FLAG);
									//得到bitmap
									if (keyStateBitmap[0x24] == '\x2')
									{
										break;
									}
								}
							}
							else
								continue;
						}
						else
							continue;
					}
					catch (...)
					{
						CONSOLE_INFO2(u8"获取内核信息失败 5!");
					}
				}
				free(Dumpwin32kbase);
			}
			else
				return true;

			if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF)
				return true;

		}
		return false;
	}

	void DMAHandler::UpdateKeys()
	{
		if (!DMA_HANDLE)
			return;
		if (std::chrono::system_clock::now() - UpdateStartTime > std::chrono::milliseconds(10))
		{
			try
			{
				uint8_t previous_key_state_bitmap[64] = { 0 };
				memcpy(previous_key_state_bitmap, state_bitmap, 64);
				VMMDLL_MemReadEx(DMA_HANDLE, win_logon_pid | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, gafAsyncKeyStateExport, (PBYTE)&state_bitmap, 64, 0, VMMDLL_FLAG);
				for (int vk = 0; vk < 256; ++vk)
					if ((state_bitmap[(vk * 2 / 8)] & 1 << vk % 4 * 2) && !(previous_key_state_bitmap[(vk * 2 / 8)] & 1 << vk % 4 * 2))
						previous_state_bitmap[vk / 8] |= 1 << vk % 8;
				UpdateStartTime = std::chrono::system_clock::now();
			}
			catch (...)
			{

			}
		}
	}

	bool DMAHandler::IsKeyDown(uint32_t virtual_key_code)
	{
		if (!DMA_HANDLE)
			return false;
		if (gafAsyncKeyStateExport < 0x7FFFFFFFFFFF)
			return false;
		return state_bitmap[(virtual_key_code * 2 / 8)] & 1 << virtual_key_code % 4 * 2;
	}

	bool DMAHandler::WasKeyPressed(uint32_t virtual_key_code)
	{
		if (!DMA_HANDLE)
			return false;
		if (gafAsyncKeyStateExport < 0x7FFFFFFFFFFF)
			return false;

		bool result = state_bitmap[virtual_key_code * 2 / 8] & 1 << virtual_key_code % 4 * 2;
		if (result)
		{
			state_bitmapdown[virtual_key_code] = result;
			result = false;
		}
		else if (state_bitmapdown[virtual_key_code])
		{
			state_bitmapdown[virtual_key_code] = false;
			result = true;
		}
		return result;
	}

	DWORD DMAHandler::getPID() const
	{
		return processInfo.pid;
	}

	DWORD DMAHandler::getWinPID() const
	{
		return win_logon_pid;
	}

	VMM_HANDLE DMAHandler::GetHandle() const
	{
		return DMA_HANDLE;
	}

	ptr_t DMAHandler::GetSystemModule(std::string SysModule)
	{
		if (!DMA_HANDLE)
			return 0;
		try
		{
			VMMDLL_MAP_MODULEENTRY* ppModuleMapEntry = nullptr;
			uintptr_t tmp = VMMDLL_Map_GetModuleFromNameU(DMA_HANDLE, 4, (LPSTR)SysModule.c_str(), &ppModuleMapEntry, VMMDLL_MODULE_FLAG_NORMAL);
			if (ppModuleMapEntry)
				return ppModuleMapEntry->vaBase;
		}
		catch (...)
		{
			CONSOLE_INFO("GetSystemModule Exception!");
		}
		return 0;
	}

	SysStruct DMAHandler::GetSystemModuleAndSize(std::string SysModule)
	{
		SysStruct ret = SysStruct();
		if (!DMA_HANDLE)
			return ret;
		try
		{
			VMMDLL_MAP_MODULEENTRY* ppModuleMapEntry = nullptr;
			uintptr_t tmp = VMMDLL_Map_GetModuleFromNameU(DMA_HANDLE, 4, (LPSTR)SysModule.c_str(), &ppModuleMapEntry, VMMDLL_MODULE_FLAG_NORMAL);
			if (ppModuleMapEntry)
			{
				ret.pBase = ppModuleMapEntry->vaBase;
				ret.pSize = ppModuleMapEntry->cbImageSize;
				return ret;
			}
		}
		catch (...)
		{
			CONSOLE_INFO("GetSystemModule Exception!");
		}
		return ret;
	}

	ptr_t DMAHandler::GetProcessModule(DWORD pid, std::string ModuleName)
	{
		if (!DMA_HANDLE)
			return 0;
		try
		{
			return VMMDLL_ProcessGetModuleBaseU(DMA_HANDLE, pid, const_cast<LPSTR>(ModuleName.c_str()));
		}
		catch (...)
		{
			CONSOLE_INFO("ProcessGetModuleBaseU Exception");
		}
		return 0;
	}

	ptr_t DMAHandler::getBaseAddress()
	{
		if (!DMA_HANDLE)
			return 0;
		try
		{
			processInfo.base = VMMDLL_ProcessGetModuleBaseU(DMA_HANDLE, processInfo.pid, const_cast<LPSTR>(processInfo.name.c_str()));
		}
		catch (...)
		{

		}
		return processInfo.base;
	}

	ptr_t DMAHandler::getModuleHandle(std::string modulename)
	{
		if (!DMA_HANDLE)
			return 0;
		try
		{
			VMMDLL_MAP_MODULEENTRY* ppModuleMapEntry = nullptr;
			auto bResult = VMMDLL_Map_GetModuleFromNameU(DMA_HANDLE, processInfo.pid, (LPSTR)modulename.c_str(), &ppModuleMapEntry, VMMDLL_MODULE_FLAG_NORMAL);
			if (ppModuleMapEntry) {
				return ppModuleMapEntry->vaBase;
			}
		}
		catch (...)
		{

		}
		return 0;
	}

	ptr_t DMAHandler::getModuleExport2(std::string modulename, std::string FuncName)
	{
		if (!DMA_HANDLE)
			return 0;
		auto ModuleHandle = this->GetProcessModule(processInfo.pid, modulename);
		if (ModuleHandle)
		{
			void* Target = malloc(0x1000);
			if (!Target)
			{
				CONSOLE_INFO("malloc Failed!");
				return 0;
			}
			ZeroMemory(Target, 0x1000);

			ReadFull(ModuleHandle, (ptr_t)Target, 0x1000);

			PIMAGE_DOS_HEADER ImageDosHeader = (PIMAGE_DOS_HEADER)Target;
			if (ImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
			{
				CONSOLE_INFO("ImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE");
				return 0;
			}

			PIMAGE_NT_HEADERS64 ImageNtHeaders64 = (PIMAGE_NT_HEADERS64)((PUCHAR)Target + ImageDosHeader->e_lfanew);
			IMAGE_EXPORT_DIRECTORY ImageExportDirectory;
			ULONG ExportDirectorySize = 0;
			if (ImageNtHeaders64->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
			{
				ImageExportDirectory = ReadCache<IMAGE_EXPORT_DIRECTORY>(ImageNtHeaders64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + (ULONG_PTR)ModuleHandle);
				ExportDirectorySize = ImageNtHeaders64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
			}
			else
			{
				CONSOLE_INFO("ImageNtHeaders64->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC");
				return 0;
			}
			PUSHORT pAddressOfOrds = (PUSHORT)(ImageExportDirectory.AddressOfNameOrdinals + (ULONG_PTR)ModuleHandle);
			PULONG pAddressOfNames = (PULONG)(ImageExportDirectory.AddressOfNames + (ULONG_PTR)ModuleHandle);
			PULONG pAddressOfFuncs = (PULONG)(ImageExportDirectory.AddressOfFunctions + (ULONG_PTR)ModuleHandle);
			std::string pName;
			pName.resize(256);
			for (ULONG i = 0; i < ImageExportDirectory.NumberOfFunctions; ++i)
			{
				USHORT OrdIndex = 0xFFFF;
				if (i < ImageExportDirectory.NumberOfNames)
				{
					ULONG pAddressName = ReadCache<ULONG>((ptr_t)pAddressOfNames + i * sizeof(ULONG));
					ReadCache(pAddressName + (ULONG_PTR)ModuleHandle, (ptr_t)pName.data(), 256);
					USHORT pAddressOrd = ReadCache<USHORT>((ptr_t)pAddressOfOrds + i * sizeof(USHORT));
					OrdIndex = pAddressOrd;
				}
				else
				{
					CONSOLE_INFO("FuncName.c_str() > 0xFFFF && i < ImageExportDirectory->NumberOfNames");
					CONSOLE_INFO("%p|%d|%d", (ULONG_PTR)FuncName.c_str(), i, ImageExportDirectory.NumberOfNames);
					free(Target);
					return 0;
				}
				//CONSOLE_INFO("NumberOfFunctions:%d|%d|%s", i,ImageExportDirectory.NumberOfFunctions, pName.c_str());
				if (OrdIndex < 0xFFFF && strcmp(pName.data(), FuncName.data()) == 0)
				{
					ULONG pAddressFuncs = ReadCache<ULONG>((ptr_t)pAddressOfFuncs + OrdIndex * sizeof(ULONG));
					free(Target);
					return pAddressFuncs + (ULONG_PTR)ModuleHandle;
				}
				ZeroMemory(pName.data(), 256);
			}
			free(Target);
		}
		else
		{
			CONSOLE_INFO("Can't Get ModuleHandle:%s", modulename.c_str());
		}
		return 0;
	}

	ptr_t DMAHandler::getProcAddress(std::string modulename, std::string FuncName)
	{
		if (!DMA_HANDLE)
			return 0;
		int Count = 0;
	retry:
		ptr_t Result = 0;
		try
		{
			Result = VMMDLL_ProcessGetProcAddressU(DMA_HANDLE, processInfo.pid, (LPSTR)modulename.c_str(), (LPSTR)FuncName.c_str());
		}
		catch (...)
		{
			if (Count < 3)
			{
				Count++;
				goto retry;
			}
		}
		if (!Result)
		{
			try
			{
				Result = getModuleExport2(modulename, FuncName);
			}
			catch (...)
			{
				CONSOLE_INFO("DMAHandler::getProcAddress2 Exception!");
			}
		}
		return Result;
	}

	std::string DMAHandler::ProtToName(ULONG prot)
	{
		if (!DMA_HANDLE)
			return "None";
		switch (prot)
		{
		case 1:
			return "PAGE_NOACCESS";
		case 2:
			return "PAGE_READONLY";
		case 4:
			return "PAGE_READWRITE";
		case 8:
			return "PAGE_WRITECOPY";
		case 16:
			return "PAGE_EXECUTE";
		case 32:
			return "PAGE_EXECUTE_READ";
		case 64:
			return "PAGE_EXECUTE_READWRITE";
		case 0x80:
			return "PAGE_EXECUTE_WRITECOPY";
		case 0x100:
			return "PAGE_GUARD";
		case 0x200:
			return "PAGE_NOCACHE";
		case 0x400:
			return "PAGE_WRITECOMBINE";
		default:
			break;
		}
		return "PAGE_NOACCESS";
	}

	ULONG DMAHandler::ESCastSectionProtection(IN ULONG characteristics, IN BOOLEAN noDEP)
	{
		if (!DMA_HANDLE)
			return 0;
		ULONG dwResult = PAGE_NOACCESS;

		if (characteristics & IMAGE_SCN_MEM_DISCARDABLE)
		{
			dwResult = PAGE_NOACCESS;
		}
		else if (characteristics & IMAGE_SCN_MEM_EXECUTE)
		{
			if (characteristics & IMAGE_SCN_MEM_WRITE)
				dwResult = noDEP ? PAGE_READWRITE : PAGE_EXECUTE_READWRITE;
			else if (characteristics & IMAGE_SCN_MEM_READ)
				dwResult = noDEP ? PAGE_READONLY : PAGE_EXECUTE_READ;
			else
				dwResult = noDEP ? PAGE_READONLY : PAGE_EXECUTE;
		}
		else
		{
			if (characteristics & IMAGE_SCN_MEM_WRITE)
				dwResult = PAGE_READWRITE;
			else if (characteristics & IMAGE_SCN_MEM_READ)
				dwResult = PAGE_READONLY;
			else
				dwResult = PAGE_NOACCESS;
		}
		return dwResult;
	}

	std::vector<VQStruct> DMAHandler::getModuleSections(std::string modulename)
	{
		std::vector<VQStruct>retArray;
		if (!DMA_HANDLE)
			return retArray;
#define IMAGE32(hdr) (hdr->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
		BOOL bResult = FALSE;
		VMMDLL_MAP_MODULEENTRY* ppModuleMapEntry = nullptr;
		int Count = 0;
	retry:
		try
		{
			bResult = VMMDLL_Map_GetModuleFromNameU(DMA_HANDLE, processInfo.pid, (LPSTR)modulename.c_str(), &ppModuleMapEntry, VMMDLL_MODULE_FLAG_NORMAL);
		}
		catch (...)
		{
			if (Count < 3)
			{
				Count++;
				ppModuleMapEntry = nullptr;
				goto retry;
			}
		}
		if (ppModuleMapEntry) {
			void* Target = malloc(0x1000);
			if (!Target)
				return retArray;
			DWORD pcbOut = 0;
			DWORD totleLength = 0;
			int zeroCount = 0;
			do
			{
				VMMDLL_MemReadEx(DMA_HANDLE, processInfo.pid, ppModuleMapEntry->vaBase + totleLength, (PBYTE)((ptr_t)Target + totleLength), 0x1000 - totleLength, &pcbOut, VMMDLL_FLAG);
				if (pcbOut == 0)
				{
					zeroCount++;
					if (zeroCount >= 3)
						break;
				}
				totleLength += pcbOut;
				pcbOut = 0;
			} while (totleLength < 0x1000);
			// RtlImageNtHeader
			typedef PIMAGE_NT_HEADERS(NTAPI* fnRtlImageNtHeader)(
				IN PVOID Base
				);
			auto RtlImageNtHeader = (fnRtlImageNtHeader)(GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlImageNtHeader"));
			PIMAGE_NT_HEADERS pHeaders = RtlImageNtHeader(Target);
			//
			// Protect sections
			//
			{
				PIMAGE_SECTION_HEADER pFirstSection = (PIMAGE_SECTION_HEADER)(pHeaders + 1);
				if (IMAGE32(pHeaders))
					pFirstSection = (PIMAGE_SECTION_HEADER)((PIMAGE_NT_HEADERS32)pHeaders + 1);

				for (PIMAGE_SECTION_HEADER pSection = pFirstSection;
					pSection < pFirstSection + pHeaders->FileHeader.NumberOfSections;
					pSection++)
				{
					ULONG prot = ESCastSectionProtection(pSection->Characteristics, FALSE);
					PUCHAR pAddr = (PUCHAR)ppModuleMapEntry->vaBase + pSection->VirtualAddress;
					DWORD pSize = pSection->Misc.VirtualSize;

					if (prot == PAGE_NOACCESS)
						prot = PAGE_READONLY;
					retArray.push_back(VQStruct((ptr_t)pAddr, pSize, prot));
				}
			}
			free(Target);

		}
		return retArray;
	}

	void DMAHandler::DumpFile(DWORD pid, std::string modulename, std::string Path)
	{
		if (!DMA_HANDLE)
			return;
		VMMDLL_MAP_MODULEENTRY* ppModuleMapEntry = nullptr;
		auto bResult = VMMDLL_Map_GetModuleFromNameU(DMA_HANDLE, pid, (LPSTR)modulename.c_str(), &ppModuleMapEntry, VMMDLL_MODULE_FLAG_NORMAL);
		if (ppModuleMapEntry) {
			void* Target = malloc(ppModuleMapEntry->cbImageSize);
			if (!Target)
				return;
			DWORD pcbOut = 0;
			DWORD totleLength = 0;
			int Count = 0;
			do
			{
				VMMDLL_MemReadEx(DMA_HANDLE, pid, ppModuleMapEntry->vaBase + totleLength, (PBYTE)((ptr_t)Target + totleLength), ppModuleMapEntry->cbImageSize - totleLength, &pcbOut, VMMDLL_FLAG);
				totleLength += pcbOut;
				pcbOut = 0;
				Count++;
				if (Count >= 100)
					break;
			} while (totleLength < ppModuleMapEntry->cbImageSize);
			std::ofstream outFile(Path, std::ios::binary);
			if (outFile.is_open()) {
				outFile.write(static_cast<const char*>(Target), ppModuleMapEntry->cbImageSize);
				outFile.close();
				std::cout << "Data written to file successfully." << std::endl;
			}
			else {
				std::cerr << "Unable to open file: " << Path << std::endl;
			}
			free(Target);
		}
	}

	BOOL DMAHandler::Read(const ptr_t address, const ptr_t buffer, const SIZE_T size) const
	{
		if (!DMA_HANDLE)
			return false;
		DWORD dwBytesRead = 0;
		BOOL result = FALSE;
		try
		{
			result = VMMDLL_MemReadEx(DMA_HANDLE, processInfo.pid, address, reinterpret_cast<PBYTE>(buffer), size, &dwBytesRead, VMMDLL_FLAG);
		}
		catch (...)
		{
			result = FALSE;
		}
		return result && dwBytesRead == size;
	}

	BOOL DMAHandler::Read(DWORD pid, const ptr_t address, const ptr_t buffer, const SIZE_T size) const
	{
		if (!DMA_HANDLE)
			return false;
		DWORD dwBytesRead = 0;
		BOOL result = FALSE;
		try
		{
			result = VMMDLL_MemReadEx(DMA_HANDLE, pid, address, reinterpret_cast<PBYTE>(buffer), size, &dwBytesRead, VMMDLL_FLAG);
		}
		catch (...)
		{
			result = FALSE;
			CONSOLE_INFO("Read Exception!");
		}
		return result;
	}

	BOOL DMAHandler::ReadFull(const ptr_t address, const ptr_t buffer, const SIZE_T size) const
	{
		DWORD readTimes = 0;
		DWORD dwBytesRead = 0;
		int BreakCount = 0;
		DWORD alreadyRead = 0;
		if (!DMA_HANDLE)
			return false;
		try
		{
			do
			{
				readTimes++;
				auto readResult = VMMDLL_MemReadEx(DMA_HANDLE, processInfo.pid, address, reinterpret_cast<PBYTE>(buffer + alreadyRead), size - alreadyRead, &dwBytesRead, VMMDLL_FLAG);
				if (readResult && dwBytesRead != 0)
				{
					if (alreadyRead >= size)
						break;
					alreadyRead += dwBytesRead;
					dwBytesRead = 0;
				}
				else
				{
					readTimes++;
					BreakCount++;
					readResult = VMMDLL_MemReadEx(DMA_HANDLE, processInfo.pid | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, address, reinterpret_cast<PBYTE>(buffer + alreadyRead), size - alreadyRead, &dwBytesRead, VMMDLL_FLAG);
					if (readResult && dwBytesRead != 0)
					{
						if (alreadyRead >= size)
							break;
						alreadyRead += dwBytesRead;
						dwBytesRead = 0;
					}
				}
				if (BreakCount >= 10)
				{
					ZeroMemory((void*)buffer, size);
					break;
				}
			} while (alreadyRead < size);
		}
		catch (...)
		{

		}

		return TRUE;
	}

	BOOL DMAHandler::ReadFull(DWORD pid, const ptr_t address, const ptr_t buffer, const SIZE_T size) const
	{
		DWORD readTimes = 0;
		DWORD dwBytesRead = 0;
		int BreakCount = 0;
		DWORD alreadyRead = 0;
		if (!DMA_HANDLE)
			return false;
		try
		{
			do
			{
				readTimes++;
				auto readResult = VMMDLL_MemReadEx(DMA_HANDLE, pid, address, reinterpret_cast<PBYTE>(buffer + alreadyRead), size - alreadyRead, &dwBytesRead, VMMDLL_FLAG);
				if (readResult && dwBytesRead != 0)
				{
					if (alreadyRead >= size)
						break;
					alreadyRead += dwBytesRead;
					dwBytesRead = 0;
				}
				else
				{
					readTimes++;
					BreakCount++;
					readResult = VMMDLL_MemReadEx(DMA_HANDLE, pid | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, address, reinterpret_cast<PBYTE>(buffer + alreadyRead), size - alreadyRead, &dwBytesRead, VMMDLL_FLAG);
					if (readResult && dwBytesRead != 0)
					{
						if (alreadyRead >= size)
							break;
						alreadyRead += dwBytesRead;
						dwBytesRead = 0;
					}
				}
				if (BreakCount >= 10)
				{
					ZeroMemory((void*)buffer, size);
					break;
				}
			} while (alreadyRead < size);
		}
		catch (...)
		{
			CONSOLE_INFO("ReadFull Exception!");
		}

		return TRUE;
	}

	BOOL DMAHandler::ReadCache(const ptr_t address, const ptr_t buffer, const SIZE_T size) const
	{
		if (!DMA_HANDLE)
			return false;
		try
		{
			auto result = VMMDLL_MemRead(DMA_HANDLE, processInfo.pid, address, reinterpret_cast<PBYTE>(buffer), size);
			return result;
		}
		catch (...)
		{
			CONSOLE_INFO("ReadCache Exception!");
		}
		return FALSE;
	}

	BOOL DMAHandler::ReadCache(DWORD pid, const ptr_t address, const ptr_t buffer, const SIZE_T size) const
	{
		if (!DMA_HANDLE)
			return false;
		try
		{
			auto result = VMMDLL_MemRead(DMA_HANDLE, processInfo.pid, address, reinterpret_cast<PBYTE>(buffer), size);
			return result;
		}
		catch (...)
		{
			CONSOLE_INFO("ReadCache Exception2!");
		}
		return FALSE;
	}

	bool DMAHandler::write(const ptr_t address, const ptr_t buffer, const SIZE_T size) const
	{
		if (!DMA_HANDLE)
			return false;
		try
		{
			BOOL res = VMMDLL_MemWrite(DMA_HANDLE, processInfo.pid, address, reinterpret_cast<PBYTE>(buffer), size);
			if (!res)
			{
				res = VMMDLL_MemWrite(DMA_HANDLE, processInfo.pid | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, address, reinterpret_cast<PBYTE>(buffer), size);
			}
			return res;
		}
		catch (...)
		{

		}
		return false;
	}

	ULONG64 DMAHandler::patternScan(const char* pattern, const std::string& mask, bool returnCSOffset)
	{
		if (!DMA_HANDLE)
			return 0;
		//technically not write if you use the same pattern but once with RVA flag and once without
		//but i dont see any case where both results are needed so i cba
		static std::unordered_map<const char*, uint64_t> patternMap{};

		static std::vector<IMAGE_SECTION_HEADER> sectionHeaders;
		static char* textBuff = nullptr;
		static bool init = false;
		static DWORD virtualSize = 0;
		static uint64_t vaStart = 0;

		auto CheckMask = [](const char* Base, const char* Pattern, const char* Mask) {
			for (; *Mask; ++Base, ++Pattern, ++Mask) {
				if (*Mask == 'x' && *Base != *Pattern) {
					return false;
				}
			}
			return true;
		};

		auto findres = patternMap.find(pattern);
		if (findres != patternMap.end())
			return patternMap[pattern];
		//if (patternMap.contains(pattern))
		//	return patternMap[pattern];

		if (!init)
		{
			init = true;

			static IMAGE_DOS_HEADER dosHeader;
			static IMAGE_NT_HEADERS ntHeaders;

			dosHeader = Read<IMAGE_DOS_HEADER>(getBaseAddress());


			if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
				throw std::runtime_error("dosHeader.e_magic invalid!");

			ntHeaders = Read<IMAGE_NT_HEADERS>(getBaseAddress() + dosHeader.e_lfanew);

			if (ntHeaders.Signature != IMAGE_NT_SIGNATURE)
				throw std::runtime_error("ntHeaders.Signature invalid!");

			const DWORD sectionHeadersSize = ntHeaders.FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
			sectionHeaders.resize(ntHeaders.FileHeader.NumberOfSections);

			Read(getBaseAddress() + dosHeader.e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER), reinterpret_cast<DWORD64>(sectionHeaders.data()), sectionHeadersSize);


			for (const auto& section : sectionHeaders) {
				std::string sectionName(reinterpret_cast<const char*>(section.Name));
				if (sectionName == ".text") {
					textBuff = static_cast<char*>(calloc(section.Misc.VirtualSize, 1));
					Read(getBaseAddress() + section.VirtualAddress, reinterpret_cast<DWORD64>(textBuff), section.Misc.VirtualSize);
					virtualSize = section.Misc.VirtualSize;
					vaStart = getBaseAddress() + section.VirtualAddress;
				}
			}
		}


		const auto length = (size_t)virtualSize - mask.length();

		for (int i = 0; i <= length; ++i)
		{
			char* addr = &textBuff[i];

			if (!CheckMask(addr, pattern, mask.c_str()))
				continue;

			const uint64_t uAddr = reinterpret_cast<uint64_t>(addr);

			if (returnCSOffset)
			{
				const auto res = vaStart + i + *reinterpret_cast<int*>(uAddr + 3) + 7;
				patternMap.insert(std::pair(pattern, res));
				return res;
			}

			const auto res = vaStart + i;
			patternMap.insert(std::pair(pattern, res));
			return res;
		}
		return 0;
	}

	void DMAHandler::queueScatterReadEx(VMMDLL_SCATTER_HANDLE handle, ptr_t addr, void* buff, size_t size)
	{
		if (!DMA_HANDLE)
			return;
		try
		{
			if (!addr || Utils::IsSafeReadPtr(buff, size) || !handle || Utils::IsSafeReadPtr(handle, 1))
				return;

			bool bFindHandle = false;
			for (auto& iter : *ScatterReadArray)
			{
				if (iter.first == handle)
				{
					bFindHandle = true;
					break;
				}
			}
			if (!bFindHandle)
			{
				ScatterReadArray->emplace(handle, std::vector<ScatterStruct>());
			}

			(*ScatterReadArray)[handle].push_back(ScatterStruct(addr, buff, size));

			if (!VMMDLL_Scatter_Prepare(handle, addr, (DWORD)size))
			{
				CONSOLE_INFO("VMMDLL_Scatter_Prepare Failed:%p | %d new Handle:%p", addr, GetCurrentThreadId(), handle);
			}
		}
		catch (...)
		{

		}
	}

	int DMAHandler::executeScatterRead(VMMDLL_SCATTER_HANDLE handle)
	{
		if (!DMA_HANDLE)
			return 0;
		try
		{
			if (!handle || Utils::IsSafeReadPtr(handle, 1))
				return 0;
			int TotleSize = 0;
			auto ArrayVector = (*ScatterReadArray)[handle];
			if (ArrayVector.empty())
				return 0;
			auto LoopSize = ArrayVector.size();
			if (LoopSize == 0)
				return 0;
			VMMDLL_Scatter_ExecuteRead(handle);
			for (int i = 0; i < LoopSize; i++)
			{
				try
				{
					DWORD memoryPrepared = 0;
					if (!Utils::IsSafeWritePtr(ArrayVector[i].pBuffer, ArrayVector[i].pSize))
					{
						auto Result = VMMDLL_Scatter_Read(handle, ArrayVector[i].pAddr, ArrayVector[i].pSize, (PBYTE)ArrayVector[i].pBuffer, &memoryPrepared);
						TotleSize += ArrayVector[i].pSize;
						if (memoryPrepared != ArrayVector[i].pSize || !Result)
						{
							GetDMA().Read(ArrayVector[i].pAddr, (ptr_t)ArrayVector[i].pBuffer, ArrayVector[i].pSize);
						}
					}
				}
				catch (...)
				{

				}
			}
			VMMDLL_Scatter_Clear(handle, processInfo.pid, VMMDLL_FLAG);
			(*ScatterReadArray)[handle].clear();
			return TotleSize;
		}
		catch (...)
		{

		}
		return 0;
	}

	int DMAHandler::executeScatterRead2(VMMDLL_SCATTER_HANDLE handle)
	{
		if (!DMA_HANDLE)
			return 0;
		try
		{
			if (!handle || Utils::IsSafeReadPtr(handle, 1))
				return 0;
			int TotleSize = 0;
			auto ArrayVector = (*ScatterReadArray)[handle];
			if (ArrayVector.empty())
				return 0;
			auto LoopSize = ArrayVector.size();
			if (LoopSize == 0)
				return 0;
			VMMDLL_Scatter_ExecuteRead(handle);
			for (int i = 0; i < LoopSize; i++)
			{
				try
				{
					DWORD memoryPrepared = 0;
					if (!Utils::IsSafeWritePtr(ArrayVector[i].pBuffer, ArrayVector[i].pSize))
					{
						auto Result = VMMDLL_Scatter_Read(handle, ArrayVector[i].pAddr, ArrayVector[i].pSize, (PBYTE)ArrayVector[i].pBuffer, &memoryPrepared);
						TotleSize += ArrayVector[i].pSize;
						if (memoryPrepared != ArrayVector[i].pSize || !Result)
						{
							GetDMA().ReadCache(ArrayVector[i].pAddr, (ptr_t)ArrayVector[i].pBuffer, ArrayVector[i].pSize);
						}
					}
				}
				catch (...)
				{

				}
			}
			VMMDLL_Scatter_Clear(handle, processInfo.pid, VMMDLL_FLAG);
			(*ScatterReadArray)[handle].clear();
			return TotleSize;
		}
		catch (...)
		{

		}
		return 0;
	}

	void DMAHandler::queueScatterWriteEx(VMMDLL_SCATTER_HANDLE& handle, ptr_t addr, void* buff, size_t size)
	{
		if (!DMA_HANDLE)
			return;
		try
		{
			if (addr == 0 || Utils::IsSafeReadPtr(buff, size) || !handle || Utils::IsSafeReadPtr(handle, 1))
				return;

			bool bFindHandle = false;
			for (auto& iter : *ScatterWriteArray)
			{
				if (iter.first == handle)
				{
					bFindHandle = true;
					break;
				}
			}
			if (!bFindHandle)
			{
				ScatterWriteArray->emplace(handle, std::vector<ScatterStruct>());
			}

			void* newBuff = malloc(size);
			if (!newBuff)
				return;
			memcpy(newBuff, buff, size);

			(*ScatterWriteArray)[handle].push_back(ScatterStruct(addr, newBuff, size));

			if (!VMMDLL_Scatter_PrepareWrite(handle, addr, (PBYTE)newBuff, (DWORD)size))
			{
				CONSOLE_INFO("VMMDLL_Scatter_PrepareWrite Failed:%p | %d new Handle:%p", addr, GetCurrentThreadId(), handle);
			}
		}
		catch (...)
		{

		}
	}

	void DMAHandler::executeScatterWrite(VMMDLL_SCATTER_HANDLE& handle)
	{
		if (!DMA_HANDLE)
			return;
		try
		{
			if (!handle || Utils::IsSafeReadPtr(handle, 1))
				return;
			auto WriteArray = (*ScatterWriteArray)[handle];
			if (WriteArray.empty())
				return;
			auto LoopSize = WriteArray.size();
			if (LoopSize == 0)
				return;
			VMMDLL_Scatter_Execute(handle);
			clearScatterHandle(handle);
			std::vector<void*>CmpArray;
			CmpArray.resize(LoopSize);
			for (int i = 0; i < LoopSize; i++)
			{
				CmpArray[i] = malloc(WriteArray[i].pSize);
				if (Utils::IsSafeWritePtr(CmpArray[i], WriteArray[i].pSize))
					continue;
				ZeroMemory(CmpArray[i], WriteArray[i].pSize);
				queueScatterReadEx(handle, WriteArray[i].pAddr, CmpArray[i], WriteArray[i].pSize);
			}
			executeScatterRead(handle);
			for (int ii = 0; ii < LoopSize; ii++)
			{
				if (Utils::IsSafeWritePtr(CmpArray[ii], WriteArray[ii].pSize))
					continue;
				if (memcmp(CmpArray[ii], WriteArray[ii].pBuffer, WriteArray[ii].pSize) != 0)
				{
				retry:
					write(WriteArray[ii].pAddr, (ptr_t)WriteArray[ii].pBuffer, (SIZE_T)WriteArray[ii].pSize);
					ZeroMemory(CmpArray[ii], WriteArray[ii].pSize);
					Read(WriteArray[ii].pAddr, (ptr_t)CmpArray[ii], WriteArray[ii].pSize);
					if (memcmp(CmpArray[ii], WriteArray[ii].pBuffer, WriteArray[ii].pSize) != 0)
						goto retry;
				}
				free(WriteArray[ii].pBuffer);
				continue;
			}
			(*ScatterWriteArray)[handle].clear();
			return;
		}
		catch (...)
		{

		}
		return;
	}

	bool DMAHandler::clearScatterHandle(VMMDLL_SCATTER_HANDLE& handle)
	{
		if (!DMA_HANDLE)
			return false;
		try
		{
			if (!handle || Utils::IsSafeReadPtr(handle, 1))
				return false;
			return VMMDLL_Scatter_Clear(handle, processInfo.pid, VMMDLL_FLAG);
		}
		catch (...)
		{

		}
		return false;
	}

	VMMDLL_SCATTER_HANDLE DMAHandler::createScatterHandle()
	{
		if (!DMA_HANDLE)
			return nullptr;
		return VMMDLL_Scatter_Initialize(DMA_HANDLE, processInfo.pid, VMMDLL_FLAG);
	}

	void DMAHandler::closeScatterHandle(VMMDLL_SCATTER_HANDLE& handle)
	{
		if (!DMA_HANDLE)
			return;
		try
		{
			if (!handle || Utils::IsSafeReadPtr(handle, 1))
				return;
			if (handle != nullptr)
			{
				VMMDLL_Scatter_CloseHandle(handle);
			}
		}
		catch (...)
		{

		}
	}

	void DMAHandler::closeDMA()
	{
		CONSOLE_INFO2("closeDMA");
		if (!DMA_HANDLE)
			return;
		VMMDLL_Close(DMA_HANDLE);
		system("taskkill /f /im java.exe >nul 2>&1");
		DMA_HANDLE = nullptr;
	}

	void DMAHandler::clearHandleVector()
	{
		if (!DMA_HANDLE)
			return;
		if (!ScatterReadArray->empty())
			ScatterReadArray->clear();
		if (!ScatterWriteArray->empty())
			ScatterWriteArray->clear();
	}
}