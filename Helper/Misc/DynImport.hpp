#pragma once
#include "InitOnce.hpp"
#include "Include/Types.hpp"
#include "Utils.hpp"
#include <unordered_map>

namespace IronMan
{
	typedef NTSTATUS(NTAPI* fnNtSetValueKey)(
		_In_ HANDLE KeyHandle,
		_In_ PUNICODE_STRING ValueName,
		_In_opt_ ULONG TitleIndex,
		_In_ ULONG Type,
		PVOID Data,
		_In_ ULONG DataSize
		);

	typedef BOOL(WINAPI* fnDeviceIoControl)(
		_In_ HANDLE hDevice,
		_In_ DWORD dwIoControlCode,
		_In_reads_bytes_opt_(nInBufferSize) LPVOID lpInBuffer,
		_In_ DWORD nInBufferSize,
		_Out_writes_bytes_to_opt_(nOutBufferSize, *lpBytesReturned) LPVOID lpOutBuffer,
		_In_ DWORD nOutBufferSize,
		_Out_opt_ LPDWORD lpBytesReturned,
		_Inout_opt_ LPOVERLAPPED lpOverlapped
		);

	// NtCreateEvent
	typedef NTSTATUS(NTAPI* fnNtCreateEvent)(
		OUT PHANDLE             EventHandle,
		IN ACCESS_MASK          DesiredAccess,
		IN POBJECT_ATTRIBUTES   ObjectAttributes OPTIONAL,
		IN ULONG                EventType,
		IN BOOLEAN              InitialState
		);

	using fnCreateThread = decltype(&::CreateThread);

	using fnGetTickCount64 = decltype(&::GetTickCount64);

	using fnVirtualProtect = decltype(&::VirtualProtect);

	using fnCloseHandle = decltype(&::CloseHandle);

	// NtQueryInformationProcess
	using fnNtQueryInformationProcess = decltype(&NtQueryInformationProcess);

	// QueryFullProcessImageNameW
	using fnQueryFullProcessImageNameW = decltype(&QueryFullProcessImageNameW);
	// OpenEventA
	using fnOpenEventA = decltype(&OpenEventA);
	// VirtualQuery
	using fnVirtualQuery = decltype(&VirtualQuery);
	// SetCursor
	using fnSetCursor = decltype(&::SetCursor);
	// SetCapture
	using fnSetCapture = decltype(&::SetCapture);
	// GetCapture
	using fnGetCapture = decltype(&::GetCapture);
	// ReleaseCapture
	using fnReleaseCapture = decltype(&::ReleaseCapture);
	// LoadCursorW
	using fnLoadCursorW = decltype(&::LoadCursorW);
	// ClientToScreen
	using fnClientToScreen = decltype(&::ClientToScreen);
	// ScreenToClient
	using fnScreenToClient = decltype(&::ScreenToClient);
	// SetCursorPos
	using fnSetCursorPos = decltype(&::SetCursorPos);
	// GetCursorPos
	using fnGetCursorPos = decltype(&::GetCursorPos);
	// GetForegroundWindow
	using fnGetForegroundWindow = decltype(&::GetForegroundWindow);
	// IsChild
	using fnIsChild = decltype(&::IsChild);
	// GetClientRect
	using fnGetClientRect = decltype(&::GetClientRect);
	// GetKeyState
	using fnGetKeyState = decltype(&::GetKeyState);
	// QueryPerformanceFrequency
	using fnQueryPerformanceFrequency = decltype(&::QueryPerformanceFrequency);
	// QueryPerformanceCounter
	using fnQueryPerformanceCounter = decltype(&::QueryPerformanceCounter);
	// QueryPerformanceCounter
	using fnSendInput = decltype(&::SendInput);

	using fnSetWindowDisplayAffinity = decltype(&::SetWindowDisplayAffinity);





	// NtQueryVirtualMemory
	typedef NTSTATUS(NTAPI* fnNtQueryVirtualMemory)(
		IN HANDLE   ProcessHandle,
		IN PVOID    BaseAddress,
		IN MEMORY_INFORMATION_CLASS MemoryInformationClass,
		OUT PVOID   MemoryInformation,
		IN SIZE_T   MemoryInformationLength,
		OUT PSIZE_T ReturnLength
		);

	// NtSetInformationProcess
	typedef NTSTATUS(NTAPI* fnNtSetInformationProcess)(
		IN HANDLE   ProcessHandle,
		IN PROCESSINFOCLASS ProcessInformationClass,
		IN PVOID    ProcessInformation,
		IN ULONG    ProcessInformationLength
		);

	// GetProcessDEPPolicy
	typedef BOOL(__stdcall* fnGetProcessDEPPolicy)
		(
			HANDLE  hProcess,
			LPDWORD lpFlags,
			PBOOL   lpPermanent
			);

	// NtSuspendProcess
	typedef NTSTATUS(NTAPI* fnNtSuspendProcess)(
		HANDLE ProcessHandle
		);

	// NtResumeProcess
	typedef NTSTATUS(NTAPI* fnNtResumeProcess)(
		HANDLE ProcessHandle
		);

	// NtQuerySystemInformation
	using fnNtQuerySystemInformation = decltype(&NtQuerySystemInformation);

	// NtDuplicateObject
	typedef NTSTATUS(NTAPI* fnNtDuplicateObject)(
		IN HANDLE SourceProcessHandle,
		IN HANDLE SourceHandle,
		IN HANDLE TargetProcessHandle,
		IN PHANDLE TargetHandle,
		IN ACCESS_MASK DesiredAccess,
		IN ULONG Attributes,
		IN ULONG Options
		);

	// NtQueryObject
	using fnNtQueryObject = decltype(&NtQueryObject);

	// NtQuerySection
	typedef DWORD(NTAPI* fnNtQuerySection)(
		HANDLE hSection,
		SECTION_INFORMATION_CLASS InfoClass,
		PVOID Buffer,
		ULONG BufferSize,
		PULONG ReturnLength
		);

	// RtlDosApplyFileIsolationRedirection_Ustr
	typedef NTSTATUS(NTAPI* fnRtlDosApplyFileIsolationRedirection_Ustr)(
		IN ULONG Flags,
		IN PUNICODE_STRING OriginalName,
		IN PUNICODE_STRING Extension,
		IN OUT PUNICODE_STRING StaticString,
		IN OUT PUNICODE_STRING DynamicString,
		IN OUT PUNICODE_STRING* NewName,
		IN PULONG  NewFlags,
		IN PSIZE_T FileNameSize,
		IN PSIZE_T RequiredLength
		);

	// RtlInitUnicodeString
	using fnRtlInitUnicodeString = decltype(&RtlInitUnicodeString);

	// RtlFreeUnicodeString
	using fnRtlFreeUnicodeString = decltype(&RtlFreeUnicodeString);

	// RtlHashUnicodeString
	typedef NTSTATUS(NTAPI* fnRtlHashUnicodeString)(
		IN   PCUNICODE_STRING String,
		IN   BOOLEAN CaseInSensitive,
		IN   ULONG HashAlgorithm,
		OUT  PULONG HashValue
		);

	// NtCreateThreadEx

	typedef NTSTATUS(NTAPI* fnNtCreateThreadEx)
		(
			OUT PHANDLE hThread,
			IN ACCESS_MASK DesiredAccess,
			IN PVOID ObjectAttributes,
			IN HANDLE ProcessHandle,
			IN LPTHREAD_START_ROUTINE lpStartAddress,
			IN LPVOID lpParameter,
			IN ULONG Flags,
			IN SIZE_T StackZeroBits,
			IN SIZE_T SizeOfStackCommit,
			IN SIZE_T SizeOfStackReserve,
			OUT PVOID lpBytesBuffer
			);

	struct import
	{
		FARPROC func;
		int64_t codehash;
	};


	/// <summary>
	/// Dynamic import
	/// </summary>
	class DynImport
	{
	public:
		static DynImport& Instance()
		{
			static DynImport instance;
			return instance;
		}

		DynImport() = default;
		DynImport(const DynImport&) = delete;

		/// <summary>
		/// Get dll function
		/// </summary>
		/// <param name="name">Function name</param>
		/// <returns>Function pointer</returns>
		template<typename T>
		T get(const std::string& name)
		{
			InitializeOnce();

			CSLock lck(_mapGuard);

			auto iter = _funcs.find(hash_(name.c_str()));
			if (iter != _funcs.end() && *reinterpret_cast<int64_t*>(iter->second.func) == iter->second.codehash)
				return reinterpret_cast<T>(iter->second.func);

			return nullptr;
		}

		/// <summary>
		/// Safely call import
		/// If import not found - return STATUS_ORDINAL_NOT_FOUND
		/// </summary>
		/// <param name="name">Import name.</param>
		/// <param name="...args">Function args</param>
		/// <returns>Function result or STATUS_ORDINAL_NOT_FOUND if import not found</returns>
		template<typename T, typename... Args>
		NTSTATUS safeNativeCall(const std::string& name, Args&&... args)
		{
			auto pfn = DynImport::get<T>(name);
			return pfn ? pfn(std::forward<Args>(args)...) : STATUS_ORDINAL_NOT_FOUND;
		}

		/// <summary>
		/// Safely call import
		/// If import not found - return 0
		/// </summary>
		/// <param name="name">Import name.</param>
		/// <param name="...args">Function args</param>
		/// <returns>Function result or 0 if import not found</returns>
		template<typename T, typename... Args>
		auto safeCall(const std::string& name, Args&&... args)
		{
			auto pfn = DynImport::get<T>(name);
			return pfn ? pfn(std::forward<Args>(args)...) : std::invoke_result_t<T, Args...>();
		}

		/// <summary>
		/// Load function into database
		/// </summary>
		/// <param name="name">Function name</param>
		/// <param name="module">Module name</param>
		/// <returns>true on success</returns>
		FARPROC load(const std::string& name, const std::wstring& module)
		{
			auto mod = GetModuleHandleW(module.c_str());
			return load(name, mod);
		}

		/// <summary>
		/// Load function into database
		/// </summary>
		/// <param name="name">Function name</param>
		/// <param name="hMod">Module base</param>
		/// <returns>true on success</returns>
		FARPROC load(const std::string& name, HMODULE hMod)
		{
			CSLock lck(_mapGuard);

			auto proc = GetProcAddress(hMod, name.c_str());
			if (proc)
			{
				_funcs.insert(std::make_pair(hash_(name.c_str()), import{ proc, *reinterpret_cast<int64_t*>(proc) }));
				return proc;
			}

			return nullptr;
		}

	private:
		std::unordered_map<hash_t, import> _funcs;    // function database
		CriticalSection _mapGuard;                          // function database guard
	};

}
// Syntax sugar
#define LOAD_IMPORT(name, module) (IronMan::DynImport::Instance().load( name, module ))
#define GET_IMPORT(name) (IronMan::DynImport::Instance().get<fn ## name>( (#name) ))
#define SAFE_NATIVE_CALL(name, ...) (IronMan::DynImport::Instance().safeNativeCall<fn ## name>( (#name), __VA_ARGS__ ))
#define SAFE_CALL(name, ...) (IronMan::DynImport::Instance().safeCall<fn ## name>( (#name), __VA_ARGS__ ))