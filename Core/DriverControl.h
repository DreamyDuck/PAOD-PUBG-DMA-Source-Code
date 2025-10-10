#pragma once
//#include "F:\DMAEsDriver\EsDef.h"
namespace IronMan::Core
{
	class DriverControl
	{
	public:
		DriverControl();
		~DriverControl();

		static DriverControl& Instance();

		/// <summary>
		/// Try to load driver if it isn't loaded
		/// </summary>
		/// <param name="path">Path to the driver file</param>
		/// <returns>Status code</returns>
		NTSTATUS EnsureLoaded();

		/// <summary>
		/// Write process memory
		/// </summary>
		/// <param name="pid">Target PID</param>
		/// <param name="base">Target base</param>
		/// <param name="size">Data size</param>
		/// <param name="buffer">Buffer address</param>
		/// <returns>Status code</returns>
		NTSTATUS ReadMemCon(DWORD pid, uint64_t base, size_t size, PVOID buffer);

		NTSTATUS NotifyRegistry(LPCVOID lpBuffer, ULONG length, bool Reg = false);

		NTSTATUS ReadMem(DWORD pid, uint64_t base, size_t size, PVOID buffer);

		NTSTATUS ProtectProcess();

		/// <summary>
		/// Check if driver is loaded
		/// </summary>
		/// <returns></returns>
		inline bool loaded() const { return NT_SUCCESS(_loadStatus); }
		inline NTSTATUS status() const { return _loadStatus; }
		DWORD	 _localPid = NULL;
	private:
		DriverControl(const DriverControl&) = delete;
		DriverControl& operator = (const DriverControl&) = delete;
	private:
		NTSTATUS _loadStatus = STATUS_NOT_FOUND;

	};

	// Syntax sugar
	inline DriverControl& Driver() { return DriverControl::Instance(); }
}