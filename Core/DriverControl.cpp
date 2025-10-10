#include "Include/WinHeaders.hpp"
#include "DriverControl.h"
#include "Helper/Misc/DynImport.hpp"
#include <filesystem>
#include <fstream>
namespace IronMan::Core
{
	NTSTATUS DriverControl::NotifyRegistry(LPCVOID lpBuffer, ULONG length, bool Reg)
	{
		UNICODE_STRING sign;
		SAFE_CALL(RtlInitUnicodeString, &sign, L"...");
		NTSTATUS status = SAFE_NATIVE_CALL(NtSetValueKey, (HANDLE)0x55667788, (PUNICODE_STRING)&sign, (ULONG)0, (ULONG)REG_BINARY, (PVOID)lpBuffer, (ULONG)length);
		return status == STATUS_BAD_DATA ? STATUS_SUCCESS : status;
	}

	NTSTATUS DriverControl::ProtectProcess()
	{
		// Not loaded
		if (!NT_SUCCESS(_loadStatus))
			return STATUS_DEVICE_DOES_NOT_EXIST;

		/*DWORD bytes = 0;
		PROTECT_PROCESS pProcess = { 0 };
		pProcess.OperationId = EASYSUPPORT_PROTECTPROCESS;
		pProcess.ProcessId = GetCurrentProcessId();

		return NotifyRegistry(&pProcess, sizeof(pProcess));*/
	}

	
	NTSTATUS DriverControl::ReadMem(DWORD pid, uint64_t base, size_t size, PVOID buffer)
	{
		if (!base) return STATUS_DEVICE_DOES_NOT_EXIST;
		if (buffer) RtlZeroMemory(buffer, size);

		// Not loaded
		if (!NT_SUCCESS(_loadStatus))
			return STATUS_DEVICE_DOES_NOT_EXIST;


		DWORD bytes = 0;
		//COPY_MEMORY copyMem = { 0 };
		//copyMem.OperationId = EASYSUPPORT_COPY_MEMORY;
		/*copyMem.pid = pid ^ 0x5007;
		copyMem.targetPtr = base ^ 0xFF50071234;
		copyMem.localbuf = (ULONGLONG)buffer ^ 0xFF12345007;
		copyMem.size = (ULONG)size ^ 0x5007;
		copyMem.write = FALSE;

		return NotifyRegistry(&copyMem, sizeof(copyMem));*/
	}

	DriverControl::DriverControl()
	{

	}

	DriverControl::~DriverControl()
	{
	}

	DriverControl& DriverControl::Instance()
	{
		static DriverControl instance;
		return instance;
	}

	/// <summary>
	/// Try to load driver if it isn't loaded
	/// </summary>
	/// <returns>Status code</returns>
	NTSTATUS DriverControl::EnsureLoaded()
	{
		NTSTATUS status = SAFE_NATIVE_CALL(NtSetValueKey, (HANDLE)0x11223344, (PUNICODE_STRING)NULL, (ULONG)0, (ULONG)0, (PVOID)NULL, (ULONG)0);

		CONSOLE_INFO("EnsureLoaded:%08X NtSetValueKey1", status);

		UNICODE_STRING sign;
		SAFE_CALL(RtlInitUnicodeString, &sign, L"...");

		if (NT_SUCCESS(status))
			status = SAFE_NATIVE_CALL(NtSetValueKey, (HANDLE)0x55667788, (PUNICODE_STRING)&sign, (ULONG)0, (ULONG)REG_BINARY, (PVOID)NULL, (ULONG)0);
		CONSOLE_INFO("EnsureLoaded:%08X NtSetValueKey2   :%wZ", status, sign);
		return status == STATUS_NO_KEY ? (_loadStatus = STATUS_SUCCESS) : status;
	}

	NTSTATUS DriverControl::ReadMemCon(DWORD pid, uint64_t base, size_t size, PVOID buffer)
	{
		if (!base) return STATUS_DEVICE_DOES_NOT_EXIST;
		if (buffer) RtlZeroMemory(buffer, size);

		// Not loaded
		if (!NT_SUCCESS(_loadStatus))
			return STATUS_DEVICE_DOES_NOT_EXIST;

		//DWORD bytes = 0;
		//COPY_MEMORY copyMem = { 0 };
		//copyMem.OperationId = PROTO_READ;
		//copyMem.pid = pid;
		//copyMem.targetPtr = base;
		//copyMem.localbuf = (ULONGLONG)buffer;
		//copyMem.size = (ULONG)size;
		//copyMem.write = FALSE;
		//copyMem.mdl = FALSE;

		//return NotifyRegistry(&copyMem, sizeof(copyMem));
	}

}