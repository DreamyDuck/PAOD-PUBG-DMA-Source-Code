#include "Include/Winheaders.hpp"
#include "InitOnce.hpp"
#include "Include/Macro.hpp"
#include "DynImport.hpp"

#include <string>
#include <cassert>
namespace IronMan
{
	class InitOnce
	{
	public:
		InitOnce() = delete;
		InitOnce(const InitOnce&) = delete;
		InitOnce& operator=(const InitOnce&) = delete;
		InitOnce(InitOnce&&) = delete;
		InitOnce& operator=(InitOnce&&) = delete;

		static bool Exec()
		{
			if (!_InterlockedCompareExchange(&_done, TRUE, FALSE))
			{
				LoadFuncs();
			}
			
			return true;
		}

		static void LoadFuncs()
		{
			HMODULE hNtdll = GetModuleHandleA(("ntdll.dll"));
			HMODULE hKernel32 = GetModuleHandleA(("kernel32.dll"));
			HMODULE hUser32 = GetModuleHandleA(("user32.dll"));

			LOAD_IMPORT(("NtCreateEvent"), hNtdll);
			LOAD_IMPORT(("NtQuerySystemInformation"), hNtdll);
			LOAD_IMPORT(("NtQueryInformationProcess"), hNtdll);
			LOAD_IMPORT(("NtSetInformationProcess"), hNtdll);
			LOAD_IMPORT(("NtQueryVirtualMemory"), hNtdll);
			LOAD_IMPORT(("NtSuspendProcess"), hNtdll);
			LOAD_IMPORT(("NtResumeProcess"), hNtdll);
			LOAD_IMPORT(("NtDuplicateObject"), hNtdll);
			LOAD_IMPORT(("NtQueryObject"), hNtdll);
			LOAD_IMPORT(("NtQuerySection"), hNtdll);
			LOAD_IMPORT(("RtlDosApplyFileIsolationRedirection_Ustr"), hNtdll);
			LOAD_IMPORT(("RtlInitUnicodeString"), hNtdll);
			LOAD_IMPORT(("RtlFreeUnicodeString"), hNtdll);
			LOAD_IMPORT(("RtlHashUnicodeString"), hNtdll);
			LOAD_IMPORT(("NtCreateThreadEx"), hNtdll);
			LOAD_IMPORT(("NtSetValueKey"), hNtdll);
			LOAD_IMPORT(("RtlImageNtHeader"), hNtdll);


			LOAD_IMPORT(("GetTickCount64"), hKernel32);
			LOAD_IMPORT(("CreateThread"), hKernel32);
			LOAD_IMPORT(("VirtualProtect"), hKernel32);
			LOAD_IMPORT(("CloseHandle"), hKernel32);
			LOAD_IMPORT(("DeviceIoControl"), hKernel32);
			LOAD_IMPORT(("QueryFullProcessImageNameW"), hKernel32);
			LOAD_IMPORT(("VirtualQuery"), hKernel32);
			LOAD_IMPORT(("QueryPerformanceFrequency"), hKernel32);
			LOAD_IMPORT(("QueryPerformanceCounter"), hKernel32);
			LOAD_IMPORT(("OpenEventA"), hKernel32);
			LOAD_IMPORT(("GetProcessDEPPolicy"), hKernel32);

			LOAD_IMPORT(("SetCursor"), hUser32);
			LOAD_IMPORT(("SetCapture"), hUser32);
			LOAD_IMPORT(("GetCapture"), hUser32);
			LOAD_IMPORT(("ReleaseCapture"), hUser32);
			LOAD_IMPORT(("LoadCursorW"), hUser32);
			LOAD_IMPORT(("ClientToScreen"), hUser32);
			LOAD_IMPORT(("ScreenToClient"), hUser32);
			LOAD_IMPORT(("SetCursorPos"), hUser32);
			LOAD_IMPORT(("GetCursorPos"), hUser32);
			LOAD_IMPORT(("GetForegroundWindow"), hUser32);
			LOAD_IMPORT(("IsChild"), hUser32);
			LOAD_IMPORT(("GetClientRect"), hUser32);
			LOAD_IMPORT(("GetKeyState"), hUser32);
			LOAD_IMPORT(("SendInput"), hUser32);
			LOAD_IMPORT(("SetWindowDisplayAffinity"), hUser32);
			
			/*LOAD_IMPORT("NtQuerySystemInformation", hNtdll);
			LOAD_IMPORT("RtlDosApplyFileIsolationRedirection_Ustr", hNtdll);
			LOAD_IMPORT("RtlInitUnicodeString", hNtdll);
			LOAD_IMPORT("RtlFreeUnicodeString", hNtdll);
			LOAD_IMPORT("RtlHashUnicodeString", hNtdll);
			LOAD_IMPORT("RtlUpcaseUnicodeChar", hNtdll);
			LOAD_IMPORT("NtQueryInformationProcess", hNtdll);
			LOAD_IMPORT("NtSetInformationProcess", hNtdll);
			LOAD_IMPORT("NtQueryInformationThread", hNtdll);
			LOAD_IMPORT("NtDuplicateObject", hNtdll);
			LOAD_IMPORT("NtQueryObject", hNtdll);
			LOAD_IMPORT("NtQuerySection", hNtdll);
			LOAD_IMPORT("RtlCreateActivationContext", hNtdll);
			LOAD_IMPORT("NtQueryVirtualMemory", hNtdll);
			LOAD_IMPORT("NtCreateThreadEx", hNtdll);
			LOAD_IMPORT("NtLockVirtualMemory", hNtdll);
			LOAD_IMPORT("NtSuspendProcess", hNtdll);
			LOAD_IMPORT("NtResumeProcess", hNtdll);
			LOAD_IMPORT("RtlImageNtHeader", hNtdll);
			LOAD_IMPORT("NtLoadDriver", hNtdll);
			LOAD_IMPORT("NtUnloadDriver", hNtdll);
			LOAD_IMPORT("RtlDosPathNameToNtPathName_U", hNtdll);
			LOAD_IMPORT("NtOpenEvent", hNtdll);
			LOAD_IMPORT("NtCreateEvent", hNtdll);
			LOAD_IMPORT("NtQueueApcThread", hNtdll);
			LOAD_IMPORT("RtlEncodeSystemPointer", hNtdll);
			LOAD_IMPORT("RtlQueueApcWow64Thread", hNtdll);
			LOAD_IMPORT("NtWow64QueryInformationProcess64", hNtdll);
			LOAD_IMPORT("NtWow64ReadVirtualMemory64", hNtdll);
			LOAD_IMPORT("NtWow64WriteVirtualMemory64", hNtdll);
			LOAD_IMPORT("Wow64GetThreadContext", hKernel32);
			LOAD_IMPORT("Wow64SetThreadContext", hKernel32);
			LOAD_IMPORT("Wow64SuspendThread", hKernel32);
			LOAD_IMPORT("GetProcessDEPPolicy", hKernel32);
			LOAD_IMPORT("QueryFullProcessImageNameA", hKernel32);
			LOAD_IMPORT("QueryFullProcessImageNameW", hKernel32);*/
		}

	private:
		static volatile long _done;
	};

	/// <summary>
	/// Exported InitOnce wrapper
	/// </summary>
	/// <returns>true on initialization completion</returns>
	bool InitializeOnce()
	{
		// Static flag is still unsafe because of magic statics
		return InitOnce::Exec();
	}

	volatile long InitOnce::_done = 0;
}