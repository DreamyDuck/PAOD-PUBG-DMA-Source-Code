#pragma once
#include "Include/WinHeaders.hpp"
#include "Include/vmmdll.h"
#include "Include/leechcore.h"
// set to FALSE if you dont want to track the total read size of the DMA
#include <chrono>

namespace IronMan
{
	struct SysStruct
	{
		ptr_t pBase;
		DWORD pSize;
		inline SysStruct()
			: pBase(0), pSize(0) {
		}
		inline SysStruct(ptr_t _a, DWORD _b)
		{
			pBase = _a;
			pSize = _b;
		}
	};

	struct VQStruct
	{
		ptr_t pAddr;
		DWORD pSize;
		DWORD prot;
		inline VQStruct()
			: pAddr(0), pSize(0), prot(0) {
		}
		inline VQStruct(ptr_t _pAddr, DWORD _pSize, DWORD _prot)
			: pAddr(_pAddr), pSize(_pSize), prot(_prot) {
		}
	};

	struct ScatterStruct
	{
		ptr_t pAddr;
		void* pBuffer;
		DWORD pSize;
		bool pCheck;
		inline ScatterStruct()
			: pAddr(0), pBuffer(nullptr), pSize(0), pCheck(0) {
		}
		inline ScatterStruct(ptr_t _pAddr, void* _pBuffer, SIZE_T _pSize)
			: pAddr(_pAddr), pSize((DWORD)_pSize), pBuffer(_pBuffer), pCheck(false) {
		}
		inline ScatterStruct(ptr_t _pAddr, void* _pBuffer, SIZE_T _pSize, bool check)
			: pAddr(_pAddr), pSize((DWORD)_pSize), pBuffer(_pBuffer), pCheck(check) {
		}
		inline void Release() { pAddr = 0; pBuffer = nullptr; pSize = 0; pCheck = 0; };
		inline bool IsValid() { return pAddr != 0 && pBuffer != nullptr && pSize != 0; };
	};

	enum class e_registry_type
	{
		none = REG_NONE,
		sz = REG_SZ,
		expand_sz = REG_EXPAND_SZ,
		binary = REG_BINARY,
		dword = REG_DWORD,
		dword_little_endian = REG_DWORD_LITTLE_ENDIAN,
		dword_big_endian = REG_DWORD_BIG_ENDIAN,
		link = REG_LINK,
		multi_sz = REG_MULTI_SZ,
		resource_list = REG_RESOURCE_LIST,
		full_resource_descriptor = REG_FULL_RESOURCE_DESCRIPTOR,
		resource_requirements_list = REG_RESOURCE_REQUIREMENTS_LIST,
		qword = REG_QWORD,
		qword_little_endian = REG_QWORD_LITTLE_ENDIAN
	};

	inline LPSTR CC_TO_LPSTR(const char* in)
	{
		LPSTR out = new char[strlen(in) + 1];
		strcpy_s(out, strlen(in) + 1, in);

		return out;
	}

	struct LibModules
	{
		HMODULE VMM = nullptr;
		HMODULE FTD3XX = nullptr;
		HMODULE LEECHCORE = nullptr;
	};

	// multiple processes
	struct BaseProcessInfo
	{
		DWORD pid = 0;
		std::string name;
		const wchar_t* wname;
		ptr_t base = 0;
	};

	class DMAHandler
	{
		//keyboard
		static inline uint64_t gafAsyncKeyStateExport = 0;
		static inline uint8_t state_bitmap[64];
		static inline uint8_t state_bitmapdown[512];
		static inline uint8_t previous_state_bitmap[256 / 8];
		static inline uint64_t win32kbase = 0;
		static inline int win_logon_pid = 0;
		static inline std::chrono::time_point<std::chrono::system_clock> UpdateStartTime = std::chrono::system_clock::now();

		static inline LibModules modules{};

		static inline VMM_HANDLE DMA_HANDLE = nullptr;

		static inline std::shared_ptr < std::unordered_map<void*, std::vector<ScatterStruct>>>ScatterWriteArray;
		static inline std::shared_ptr<std::unordered_map<void*, std::vector<ScatterStruct>>>ScatterReadArray;

		static inline BaseProcessInfo processInfo{};

		static inline BOOLEAN PROCESS_INITIALIZED = FALSE;
		// Private log function used by the DMAHandler class
		static bool DumpMemoryMap();

	public:
		DMAHandler();
		~DMAHandler();

		static DMAHandler& Instance();

		bool AttachProcess(DWORD pid, std::string Name);

		BOOL FreshAll();
		BOOL FreshFREQ();
		// Whether the DMA and Process are initialized
		bool isInitialized() const;
		std::string QueryValue(const char* path, e_registry_type type);
		ptr_t getKeyPtr();
		bool InitKeyboard();
		void UpdateKeys();
		bool IsKeyDown(uint32_t virtual_key_code);
		bool WasKeyPressed(uint32_t virtual_key_code);
		// Gets the PID of the process
		DWORD getPID() const;

		DWORD getWinPID() const;

		VMM_HANDLE GetHandle() const;

		ptr_t GetSystemModule(std::string SysModule);

		SysStruct GetSystemModuleAndSize(std::string SysModule);

		ptr_t GetProcessModule(DWORD pid, std::string ModuleName);

		// Gets the Base address of the process
		ptr_t getBaseAddress();

		//取模块句柄
		ptr_t getModuleHandle(std::string modulename);

		//取导出函数
		ptr_t getModuleExport2(std::string modulename, std::string FuncName);

		ptr_t getProcAddress(std::string modulename, std::string FuncName);
		//取保护
		ULONG ESCastSectionProtection(IN ULONG characteristics, IN BOOLEAN noDEP);

		//保护转换名称
		std::string ProtToName(ULONG prot);

		//取模块section属性
		std::vector<VQStruct> getModuleSections(std::string modulename);

		//dump
		void DumpFile(DWORD pid, std::string modulename, std::string Path);

		//获取PID
		std::vector<int> GetProcessidByEnum(const char* process_name);

		//获取PID
		DWORD GetProcessidByName(const char* process_name);

		std::shared_ptr<std::vector<int>> GetPidListFromName(std::string name);

		BOOL Read(ptr_t address, ptr_t buffer, SIZE_T size) const;

		template <typename T>
		T Read(ptr_t address)
		{
			T buffer{};
			memset(&buffer, 0, sizeof(T));
			Read(address, reinterpret_cast<ptr_t>(&buffer), sizeof(T));
			return buffer;
		}

		template <typename T>
		T Read(void* address)
		{
			return Read<T>(reinterpret_cast<ptr_t>(address));
		}

		BOOL Read(DWORD pid, ptr_t address, ptr_t buffer, SIZE_T size) const;

		template <typename T>
		T Read(DWORD pid, ptr_t address)
		{
			T buffer{};
			memset(&buffer, 0, sizeof(T));
			Read(pid, address, reinterpret_cast<ptr_t>(&buffer), sizeof(T));
			return buffer;
		}

		template <typename T>
		T Read(DWORD pid, void* address)
		{
			return Read<T>(pid, reinterpret_cast<ptr_t>(address));
		}

		BOOL ReadFull(ptr_t address, ptr_t buffer, SIZE_T size) const;

		BOOL ReadFull(DWORD pid, ptr_t address, ptr_t buffer, SIZE_T size) const;

		template <typename T>
		T ReadFull(ptr_t address)
		{
			T buffer{};
			memset(&buffer, 0, sizeof(T));
			ReadFull(address, reinterpret_cast<ptr_t>(&buffer), sizeof(T));
			return buffer;
		}

		template <typename T>
		T ReadFull(void* address)
		{
			return ReadFull(reinterpret_cast<ptr_t>(address));
		}

		BOOL ReadCache(ptr_t address, ptr_t buffer, SIZE_T size) const;
		BOOL ReadCache(DWORD pid, ptr_t address, ptr_t buffer, SIZE_T size) const;

		template <typename T>
		T ReadCache(ptr_t address)
		{
			T buffer{};
			memset(&buffer, 0, sizeof(T));
			ReadCache(address, reinterpret_cast<ptr_t>(&buffer), sizeof(T));
			return buffer;
		}

		template <typename T>
		T ReadCache(void* address)
		{
			return ReadCache<T>(reinterpret_cast<ptr_t>(address));
		}

		bool write(ptr_t address, ptr_t buffer, SIZE_T size) const;

		template <typename T>
		bool write(ptr_t address, T* buffer)
		{
			return write(address, reinterpret_cast<ptr_t>(buffer), sizeof(T));
		}

		template <typename T>
		bool write(void* address, T* buffer)
		{
			return write(reinterpret_cast<ptr_t>(address), reinterpret_cast<ptr_t>(buffer), sizeof(T));
		}

		template <typename T>
		bool write(ptr_t address, T value)
		{
			return write(address, reinterpret_cast<ptr_t>(&value), sizeof(T));
		}

		template <typename T>
		bool write(void* address, T value)
		{
			return write(reinterpret_cast<ptr_t>(address), reinterpret_cast<ptr_t>(&value), sizeof(T));
		}

		void queueScatterReadEx(VMMDLL_SCATTER_HANDLE handle, ptr_t addr, void* bffr, size_t size);
		int executeScatterRead(VMMDLL_SCATTER_HANDLE handle);
		int executeScatterRead2(VMMDLL_SCATTER_HANDLE handle);

		void queueScatterWriteEx(VMMDLL_SCATTER_HANDLE& handle, ptr_t addr, void* bffr, size_t size);
		void executeScatterWrite(VMMDLL_SCATTER_HANDLE& handle);

		bool clearScatterHandle(VMMDLL_SCATTER_HANDLE& handle);

		template <typename T>
		void WriteScatterEx(VMMDLL_SCATTER_HANDLE& handle, void* addressT, T buff)
		{
			queueScatterWriteEx(handle, reinterpret_cast<ptr_t>(addressT), reinterpret_cast<void*>(&buff), sizeof(T));
		}

		template <typename T>
		void WriteScatterEx(VMMDLL_SCATTER_HANDLE& handle, ptr_t address, T buff)
		{
			WriteScatterEx<T>(handle, reinterpret_cast<void*>(address), buff);
		}

		VMMDLL_SCATTER_HANDLE createScatterHandle();

		void closeScatterHandle(VMMDLL_SCATTER_HANDLE& handle);

		ptr_t patternScan(const char* pattern, const std::string& mask, bool returnCSOffset = true);

		static void closeDMA();

		void clearHandleVector();
	};

	inline DMAHandler& GetDMA() { return DMAHandler::Instance(); };
}