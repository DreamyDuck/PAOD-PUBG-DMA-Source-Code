#pragma once
#include <string>
#include <utility>
#include <unordered_map>
#include <algorithm>
#include <codecvt>
#include <winternl.h>
#include "../../Core/DMA/Include/vmmdll.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>

typedef uintptr_t ptr_t;

namespace IronMan
{

	// hook t
	struct hookStruct
	{
		size_t   _origSize = 0;             // Original code size
		uint32_t _origCodeAllLen = 0;

		uint8_t* _buf;
		uint8_t* _origCode;
		uint8_t* _newCode;
		uint8_t* _callCode;
		uint8_t* _retCode;

		void* _original = nullptr;          // Original function address
		void* _callOriginal = nullptr;      // Pointer to original function 
	};

	namespace Utils
	{
		static __inline bool ConvertToNtPath(const wchar_t* path, wchar_t* normalized, size_t normalizedLen)
		{
			typedef struct _RTL_RELATIVE_NAME {
				UNICODE_STRING RelativeName;
				HANDLE         ContainingDirectory;
				void* CurDirRef;
			} RTL_RELATIVE_NAME, * PRTL_RELATIVE_NAME;

			typedef BOOLEAN(NTAPI* RtlDosPathNameToRelativeNtPathName_U_Prototype)(
				_In_       PCWSTR DosFileName,
				_Out_      PUNICODE_STRING NtFileName,
				_Out_opt_  PWSTR* FilePath,
				_Out_opt_  PRTL_RELATIVE_NAME RelativeName
				);

			static RtlDosPathNameToRelativeNtPathName_U_Prototype RtlDosPathNameToRelativeNtPathName_U = (RtlDosPathNameToRelativeNtPathName_U_Prototype)GetProcAddress(
				GetModuleHandleW(L"ntdll.dll"),
				("RtlDosPathNameToRelativeNtPathName_U")
			);;

			UNICODE_STRING ntPath;
			DWORD size;
			bool result = false;

			size = GetFullPathNameW(path, (DWORD)normalizedLen, normalized, NULL);
			if (size == 0)
				return false;

			memset(&ntPath, 0, sizeof(ntPath));

			if (RtlDosPathNameToRelativeNtPathName_U(normalized, &ntPath, NULL, NULL) == FALSE)
				return false;

			if (normalizedLen * sizeof(wchar_t) > ntPath.Length)
			{
				memcpy(normalized, ntPath.Buffer, ntPath.Length);
				normalized[ntPath.Length / sizeof(wchar_t)] = L'\0';
				result = true;
			}

			HeapFree(GetProcessHeap(), 0, ntPath.Buffer);

			return result;
		}

		static __inline bool AllocNormalizedPath(const wchar_t* path, wchar_t** normalized)
		{
			enum { NORMALIZATION_OVERHEAD = 32 };
			wchar_t* buf;
			size_t len;

			len = wcslen(path) + NORMALIZATION_OVERHEAD;

			buf = (wchar_t*)malloc(len * sizeof(wchar_t));
			if (!buf)
				return false;

			if (!ConvertToNtPath(path, buf, len))
			{
				free(buf);
				return false;
			}

			*normalized = buf;
			return true;
		}

		//std::string sha256(const std::string& input);

		// 读取文件并将其内容存储为字节数组
		__forceinline std::string readFileToByteArray(const std::string& filename) {
			std::ifstream file(filename, std::ios::binary);

			// 检查文件是否成功打开
			if (!file) {
				std::cerr << "Unable to open file: " << filename << std::endl;
				return "";
			}

			// 将文件内容读取到std::string中
			std::ostringstream oss;
			oss << file.rdbuf();

			return oss.str();
		}

		NTSTATUS CreateFackThread(OUT PHANDLE ThreadHandle, IN LPTHREAD_START_ROUTINE lpStartAddress, IN LPVOID lpParameter, IN ULONG_PTR jmpRcx = 0);

		NTSTATUS QueryMemory(ptr_t lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer);

		std::string GetTextBetween(const std::string& Str, const char* Begin, const char* End, size_t* outPos = NULL, size_t inPos = 0);

		std::vector<std::string> split(std::string str, std::string pat);
		/// <summary>
		/// Format string
		/// </summary>
		/// <param name="fmt">Format specifier</param>
		/// <param name="">Arguments</param>
		/// <returns>Formatted string</returns>
		std::wstring FormatString(const wchar_t* fmt, ...);

		/// <summary>
		/// Format string
		/// </summary>
		/// <param name="fmt">Format specifier</param>
		/// <param name="">Arguments</param>
		/// <returns>Formatted string</returns>
		std::string FormatString(const char* fmt, ...);

		std::string UnicodeToUTF8(const std::wstring& wstr);

		std::wstring UTF8ToUnicode(const std::string& str);

		std::string UnicodeToANSI(const std::wstring& wstr);

		std::wstring ANSIToUnicode(const std::string& str);

		std::string UTF8ToANSI(const std::string& str);


		std::string ANSIToUTF8(const std::string& str);

		std::vector<std::string> SplitString(const std::string str, const std::string splitter);

		std::vector<std::string> SplitText(const std::string str, std::string splitter1, std::string splitter2);
		/// <summary>
		/// Get filename from full-qualified path
		/// </summary>
		/// <param name="path">File path</param>
		/// <param name="ext">file extension. true - return value contains the file extension</param>
		/// <returns>Filename</returns>
		std::string StripPath(const std::string& path, bool ext = true);

		/// <summary>
		/// Get filename from full-qualified path
		/// </summary>
		/// <param name="path">File path</param>
		/// <param name="ext">file extension. true - return value contains the file extension</param>
		/// <returns>Filename</returns>
		std::wstring StripPath(const std::wstring& path, bool ext = true);

		/// <summary>
		/// Cast string characters to lower case
		/// </summary>
		/// <param name="str">Source string.</param>
		/// <returns>Result string</returns>
		std::wstring ToLower(std::wstring str);

		/// <summary>
		/// Cast string characters to upper case
		/// </summary>
		/// <param name="str">Source string.</param>
		/// <returns>Result string</returns>
		std::wstring ToUpper(std::wstring str);

		/// <summary>
		/// Cast string characters to lower case
		/// </summary>
		/// <param name="str">Source string.</param>
		/// <returns>Result string</returns>
		std::string ToLower(std::string str);

		/// <summary>
		/// Cast string characters to upper case
		/// </summary>
		/// <param name="str">Source string.</param>
		/// <returns>Result string</returns>
		std::string ToUpper(std::string str);

		bool hasNullTerminator(char* str, size_t maxLength);
		/// <summary>
		/// Check if file exists
		/// </summary>
		/// <param name="path">Full-qualified file path</param>
		/// <returns>true if exists</returns>
		bool FileExists(const std::wstring& path);
		bool FileExists(const std::string& path);

		/// <summary>
		/// Get current process exe file path
		/// </summary>
		/// <returns>Exe directory</returns>
		std::wstring GetExePath();

		DWORD GetModuleLen(HMODULE hModule);

		void MessageBoxTermination(std::wstring text, std::wstring caption);

		void writeToClipboard(const std::string& text);

		BOOL IsAdministratorRun();

		void* ReadLibrary(const char* Name, size_t* pSize);

		float RandomFloat();

		float RandomRange(float min, float max);

		bool isAlphanumericWithSpecialChars(const std::string& str);

		bool StringIsValid(std::string s);

		bool StringIsValid2(std::string s);

		bool checkAndCreateDirectory(const std::string& directoryPath);

		bool IsSafeReadPtr(void* ptr, size_t size);

		bool IsSafeWritePtr(void* ptr, size_t size);

		std::string SimpleHttpGet(const std::string& url, const std::string& additionalHeaders);

		hash_t GetMachineCode();
	}



	/// <summary>
	/// std::mutex alternative
	/// </summary>
	class CriticalSection
	{
	public:
		CriticalSection()
		{
			InitializeCriticalSection(&_native);
		}

		~CriticalSection()
		{
			DeleteCriticalSection(&_native);
		}

		void lock()
		{
			EnterCriticalSection(&_native);
		}

		void unlock()
		{
			LeaveCriticalSection(&_native);
		}

	private:
		CRITICAL_SECTION _native;
	};

	/// <summary>
	/// std::lock_guard alternative
	/// </summary>
	class CSLock
	{
	public:
		CSLock(CriticalSection& cs)
			: _cs(cs)
		{
			cs.lock();
		}

		~CSLock()
		{
			_cs.unlock();
		}

	private:
		CSLock(const CSLock&) = delete;
		CSLock& operator = (const CSLock&) = delete;

	private:
		CriticalSection& _cs;
	};

}