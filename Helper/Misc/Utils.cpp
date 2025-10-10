#include "Include/WinHeaders.hpp"
#include "Utils.hpp"
#include "DynImport.hpp"
#include "Core/Imgui/imgui.h"
#include "Core/Imgui/imgui_internal.h"
#include <Wincrypt.h>
#include "Core/Core.hpp"
#include <tchar.h>
#include <Psapi.h>
#include <random>
#include <direct.h> // for _mkdir
#include <io.h>     // for _access
#include <windows.h>
#include <winhttp.h>
#include <intrin.h>
#include <iphlpapi.h>
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "Iphlpapi.lib")
struct LogMemory
{
	void* StartAddr;
	size_t Size;
	inline LogMemory()
	{
		StartAddr = nullptr;
		Size = 0;
	}
	inline LogMemory(void* _a, size_t _b)
	{
		StartAddr = _a;
		Size = _b;
	}
};

namespace IronMan::Utils
{
	namespace fs = std::filesystem;
	std::unordered_map<ptr_t, LogMemory>VirutalMap;
	bool startCrc = false;

	// SHA-256常量
	/*constexpr std::array<uint32_t, 64> k = { {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
		0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
		0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
		0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
		0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
		0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
		0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
		0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
		0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	} };*/

	// SHA-256初始哈希值
	//constexpr std::array<uint32_t, 8> initial_hash = { {
	//	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	//	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
	//} };

	// 辅助函数
	uint32_t right_rotate(uint32_t value, uint32_t count) {
		return (value >> count) | (value << (32 - count));
	}

	std::vector<uint8_t> pad(const std::vector<uint8_t>& input) {
		size_t original_length = input.size();
		size_t padded_length = original_length + 1 + 8;

		while (padded_length % 64 != 0) {
			padded_length++;
		}

		std::vector<uint8_t> padded(padded_length, 0);
		std::copy(input.begin(), input.end(), padded.begin());
		padded[original_length] = 0x80;

		uint64_t bit_length = original_length * 8;
		for (size_t i = 0; i < 8; ++i) {
			padded[padded_length - 1 - i] = bit_length & 0xFF;
			bit_length >>= 8;
		}

		return padded;
	}

	/*std::string sha256(const std::string& input) {
		std::vector<uint8_t> data(input.begin(), input.end());
		data = pad(data);

		std::array<uint32_t, 8> hash = initial_hash;

		for (size_t chunk_start = 0; chunk_start < data.size(); chunk_start += 64) {
			uint32_t w[64] = { 0 };

			for (size_t i = 0; i < 16; ++i) {
				w[i] = (data[chunk_start + i * 4] << 24) |
					(data[chunk_start + i * 4 + 1] << 16) |
					(data[chunk_start + i * 4 + 2] << 8) |
					data[chunk_start + i * 4 + 3];
			}

			for (size_t i = 16; i < 64; ++i) {
				uint32_t s0 = right_rotate(w[i - 15], 7) ^ right_rotate(w[i - 15], 18) ^ (w[i - 15] >> 3);
				uint32_t s1 = right_rotate(w[i - 2], 17) ^ right_rotate(w[i - 2], 19) ^ (w[i - 2] >> 10);
				w[i] = w[i - 16] + s0 + w[i - 7] + s1;
			}

			uint32_t a = hash[0];
			uint32_t b = hash[1];
			uint32_t c = hash[2];
			uint32_t d = hash[3];
			uint32_t e = hash[4];
			uint32_t f = hash[5];
			uint32_t g = hash[6];
			uint32_t h = hash[7];

			for (size_t i = 0; i < 64; ++i) {
				uint32_t S1 = right_rotate(e, 6) ^ right_rotate(e, 11) ^ right_rotate(e, 25);
				uint32_t ch = (e & f) ^ ((~e) & g);
				uint32_t temp1 = h + S1 + ch + k[i] + w[i];
				uint32_t S0 = right_rotate(a, 2) ^ right_rotate(a, 13) ^ right_rotate(a, 22);
				uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
				uint32_t temp2 = S0 + maj;

				h = g;
				g = f;
				f = e;
				e = d + temp1;
				d = c;
				c = b;
				b = a;
				a = temp1 + temp2;
			}

			hash[0] += a;
			hash[1] += b;
			hash[2] += c;
			hash[3] += d;
			hash[4] += e;
			hash[5] += f;
			hash[6] += g;
			hash[7] += h;
		}

		std::stringstream ss;
		for (uint32_t h : hash) {
			ss << std::hex << std::setw(8) << std::setfill('0') << h;
		}

		return ss.str();
	}*/

	std::string readFileToByteArray(const std::string& filename);

	NTSTATUS QueryMemory(ptr_t lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer)
	{
		SetLastNtStatus(STATUS_SUCCESS);
		SAFE_CALL(VirtualQuery, reinterpret_cast<LPCVOID>(lpAddress), lpBuffer, sizeof(MEMORY_BASIC_INFORMATION));
		return LastNtStatus();
	}
	/// <summary>
	/// Format string
	/// </summary>
	/// <param name="fmt">Format specifier</param>
	/// <param name="">Arguments</param>
	/// <returns>Formatted string</returns>
	std::wstring Utils::FormatString(const wchar_t* fmt, ...)
	{

		wchar_t buf[4096] = { 0 };

		va_list vl;
		va_start(vl, fmt);
		vswprintf_s(buf, fmt, vl);
		va_end(vl);

		return buf;
	}

	/// <summary>
	/// Format string
	/// </summary>
	/// <param name="fmt">Format specifier</param>
	/// <param name="">Arguments</param>
	/// <returns>Formatted string</returns>
	std::string Utils::FormatString(const char* fmt, ...)
	{

		char buf[4096] = { 0 };

		va_list vl;
		va_start(vl, fmt);
		_vsnprintf_s(buf, 4096, fmt, vl);
		va_end(vl);

		return buf;
	}

	std::string UnicodeToUTF8(const std::wstring& wstr)
	{

		char* pElementText;
		int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)wstr.c_str(), -1, NULL, 0, NULL, NULL);
		pElementText = new char[iTextLen + 1];
		memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
		::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)wstr.c_str(), -1, pElementText, iTextLen, NULL, NULL);
		std::string strReturn(pElementText);
		delete[] pElementText;

		return strReturn;
	}

	std::wstring UTF8ToUnicode(const std::string& str)
	{

		//int len = str.length();
		int unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pUnicode;
		pUnicode = new wchar_t[unicodeLen + 1];
		memset((void*)pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
		::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, (LPWSTR)pUnicode, unicodeLen);
		std::wstring wstrReturn(pUnicode);
		delete[] pUnicode;

		return wstrReturn;
	}

	std::string UnicodeToANSI(const std::wstring& wstr)
	{
		std::string str;
		int nLen = (int)wstr.length() + 1;
		str.resize(nLen, ' ');
		int nResult = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wstr.c_str(), nLen, (LPSTR)str.c_str(), nLen, NULL, NULL);
		if (nResult == 0)
		{
			return "";
		}
		return str;
	}

	std::wstring ANSIToUnicode(const std::string& str)
	{

		int num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		wchar_t* wide = new wchar_t[num];
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, num);
		std::wstring w_str(wide);
		delete[] wide;

		return w_str;
	}

	std::string UTF8ToANSI(const std::string& str)
	{
		return  UnicodeToANSI(UTF8ToUnicode(str));
	}

	std::string ANSIToUTF8(const std::string& str)
	{
		return UnicodeToUTF8(ANSIToUnicode(str));
	}

	std::vector<std::string> SplitString(const std::string str, std::string splitter)
	{

		std::vector<std::string> result;
		std::string::size_type pos1, pos2;
		pos2 = str.find(splitter);
		pos1 = 0;
		while (std::string::npos != pos2)
		{
			result.push_back(str.substr(pos1, pos2 - pos1));

			pos1 = pos2 + splitter.size();
			pos2 = str.find(splitter, pos1);
		}
		if (pos1 != str.length())
			result.push_back(str.substr(pos1));

		return result;
	}

	std::vector<std::string> SplitText(const std::string str, std::string splitter1, std::string splitter2)
	{

		std::vector<std::string> result;
		std::string::size_type pos1, pos2, pos3;
		pos2 = str.find(splitter1);
		pos1 = 0;
		while (std::string::npos != pos2)
		{
			pos3 = str.find(splitter2, pos1);
			if (std::string::npos != pos3)
			{
				result.push_back(str.substr(pos2 + splitter1.size(), pos3 - pos2 - splitter1.size()));
				pos1 = pos3 + splitter2.size();
				pos2 = str.find(splitter1, pos1);
			}
		}
		return result;
	}

	std::string GetTextBetween(const std::string& Str, const char* Begin, const char* End, size_t* outPos, size_t inPos)
	{
		if (outPos)
			*outPos = 0;
		auto FindStr = Str.find(Begin, inPos);
		if (FindStr == std::string::npos)
			return "";
		FindStr += strlen(Begin);
		std::string NewStr = Str.substr(FindStr, Str.size() - FindStr);
		auto res = NewStr.find(End);
		if (res == std::string::npos)
			return "";
		if (outPos)
			*outPos = FindStr;
		NewStr = NewStr.substr(0, res);
		if (NewStr.size() > 0)
			return NewStr;
		return "";
	}

	std::vector<std::string> split(std::string str, std::string pat)
	{
		std::vector<std::string> bufStr;

		while (true)
		{
			size_t index = str.find(pat);

			std::string subStr = str.substr(0, index);
			if (!subStr.empty())
				bufStr.push_back(subStr);

			str.erase(0, index + pat.size());

			if (index == -1)
				break;
		}
		return bufStr;
	}
	/// <summary>
		/// Get filename from full-qualified path
		/// </summary>
		/// <param name="path">File path</param>
		/// <param name="ext">file extension. true - return value contains the file extension</param>
		/// <returns>Filename</returns>
	std::string Utils::StripPath(const std::string& path, bool ext /*= true*/)
	{

		if (path.empty())
			return path;

		auto idx = path.rfind('\\');
		if (idx == path.npos)
			idx = path.rfind('/');
		if (idx != path.npos)
		{
			auto fileName = path.substr(idx + 1);
			if (!ext)
			{
				idx = fileName.rfind('.');
				if (idx != path.npos)
					return fileName.substr(static_cast<std::string::size_type>(0), idx);
				else
					return fileName;
			}
			else
				return fileName;
		}
		else
			return path;

	}

	/// <summary>
	/// Get filename from full-qualified path
	/// </summary>
	/// <param name="path">File path</param>
	/// <param name="ext">file extension. true - return value contains the file extension</param>
	/// <returns>Filename</returns>
	std::wstring Utils::StripPath(const std::wstring& path, bool ext /*= true*/)
	{

		if (path.empty())
			return path;

		auto idx = path.rfind('\\');
		if (idx == path.npos)
			idx = path.rfind('/');
		if (idx != path.npos)
		{
			auto fileName = path.substr(idx + 1);
			if (!ext)
			{
				idx = fileName.rfind('.');
				if (idx != path.npos)
					return fileName.substr(static_cast<std::wstring::size_type>(0), idx);
				else
					return fileName;
			}
			else
				return fileName;
		}
		else
			return path;

	}

	/// <summary>
	/// Cast string characters to lower case
	/// </summary>
	/// <param name="str">Source string.</param>
	/// <returns>Result string</returns>
	std::wstring Utils::ToLower(std::wstring str)
	{

		std::transform(str.begin(), str.end(), str.begin(), ::towlower);

		return str;
	}

	/// <summary>
	/// Cast string characters to upper case
	/// </summary>
	/// <param name="str">Source string.</param>
	/// <returns>Result string</returns>
	std::wstring Utils::ToUpper(std::wstring str)
	{

		std::transform(str.begin(), str.end(), str.begin(), ::towupper);

		return str;
	}

	/// <summary>
	/// Cast string characters to lower case
	/// </summary>
	/// <param name="str">Source string.</param>
	/// <returns>Result string</returns>
	std::string Utils::ToLower(std::string str)
	{

		std::transform(str.begin(), str.end(), str.begin(), ::tolower);

		return str;
	}

	/// <summary>
	/// Cast string characters to upper case
	/// </summary>
	/// <param name="str">Source string.</param>
	/// <returns>Result string</returns>
	std::string Utils::ToUpper(std::string str)
	{

		std::transform(str.begin(), str.end(), str.begin(), ::toupper);

		return str;
	}

	/// <summary>
	/// Check if file exists
	/// </summary>
	/// <param name="path">Full-qualified file path</param>
	/// <returns>true if exists</returns>
	bool Utils::FileExists(const std::wstring& path)
	{
		return (GetFileAttributesW(path.c_str()) != 0xFFFFFFFF);
	}
	bool Utils::FileExists(const std::string& path)
	{
		return (GetFileAttributesA(path.c_str()) != 0xFFFFFFFF);
	}
	/*void* LoadFileToMemory(const char * filename, const char * file_open_mode, size_t * out_file_size, int padding_bytes)
	{

		assert(filename && file_open_mode);
		if (out_file_size)
			*out_file_size = 0;

		FILE* f;
		if ((f = ImFileOpen(filename, file_open_mode)) == NULL)
			return NULL;

		long file_size_signed;
		if (fseek(f, 0, SEEK_END) || (file_size_signed = ftell(f)) == -1 || fseek(f, 0, SEEK_SET))
		{
			fclose(f);
			return NULL;
		}

		size_t file_size = (size_t)file_size_signed;
		void* file_data = ImGui::MemAlloc(file_size + padding_bytes);
		if (file_data == NULL)
		{
			fclose(f);
			return NULL;
		}
		if (fread(file_data, 1, file_size, f) != file_size)
		{
			fclose(f);
			ImGui::MemFree(file_data);
			return NULL;
		}
		if (padding_bytes > 0)
			memset((void*)(((char*)file_data) + file_size), 0, (size_t)padding_bytes);

		fclose(f);
		if (out_file_size)
			*out_file_size = file_size;
		return file_data;
	}*/

	//std::string GetFileMd5(std::string filePath)
	//{
	//	std::string md5 = "";
	//	size_t fileDataSize = 0;
	//	auto fileData = Utils::LoadFileToMemory(filePath.c_str(), ("rb"), &fileDataSize, 0);
	//	if (fileData)
	//	{
	//		HCRYPTPROV hProv = NULL;
	//		if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))       //获得CSP中一个密钥容器的句柄
	//		{
	//			HCRYPTPROV hHash = NULL;
	//			//初始化对数据流的hash，创建并返回一个与CSP的hash对象相关的句柄。这个句柄接下来将被    CryptHashData调用。
	//			if (CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
	//			{
	//				if (CryptHashData(hHash, (const BYTE*)fileData, (DWORD)fileDataSize, 0))      //hash文件  
	//				{
	//					

	//					BYTE *pbHash;
	//					DWORD dwHashLen = sizeof(DWORD);
	//					if (CryptGetHashParam(hHash, HP_HASHVAL, NULL, &dwHashLen, 0)) //我也不知道为什么要先这样调用CryptGetHashParam，这块是参照的msdn       
	//					{
	//						pbHash = (byte*)malloc(dwHashLen);
	//						if (CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwHashLen, 0))//获得md5值 
	//						{
	//							for (DWORD i = 0; i < dwHashLen; i++)         //输出md5值 
	//							{
	//								char tmp[3] = { 0 };
	//								sprintf(tmp, "%02x", pbHash[i]);
	//								md5 += tmp;
	//							}
	//						}
	//						free(pbHash);
	//					}
	//					ImGui::MemFree(fileData);
	//				}
	//				CryptDestroyHash(hHash);
	//			}
	//			CryptReleaseContext(hProv, 0);
	//		}
	//	}
	//	return ToUpper(md5);
	//}

	//std::string Md5(std::string str, bool _16Bit)
	//{
	//	std::string md5 = "";
	//	HCRYPTPROV hProv = NULL;
	//	if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))       //获得CSP中一个密钥容器的句柄
	//	{
	//		HCRYPTPROV hHash = NULL;
	//		//初始化对数据流的hash，创建并返回一个与CSP的hash对象相关的句柄。这个句柄接下来将被    CryptHashData调用。
	//		if (CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
	//		{
	//			if (CryptHashData(hHash, (const BYTE*)str.data(), (DWORD)str.length(), 0))      //hash文件  
	//			{
	//				BYTE *pbHash;
	//				DWORD dwHashLen = sizeof(DWORD);
	//				if (CryptGetHashParam(hHash, HP_HASHVAL, NULL, &dwHashLen, 0)) //我也不知道为什么要先这样调用CryptGetHashParam，这块是参照的msdn       
	//				{
	//					pbHash = (byte*)malloc(dwHashLen);
	//					if (CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwHashLen, 0))//获得md5值 
	//					{
	//						for (DWORD i = 0; i < dwHashLen; i++)         //输出md5值 
	//						{
	//							char tmp[3] = { 0 };
	//							sprintf(tmp, "%02x", pbHash[i]);
	//							md5 += tmp;
	//						}
	//					}
	//					free(pbHash);
	//				}
	//			}
	//			CryptDestroyHash(hHash);
	//		}
	//		CryptReleaseContext(hProv, 0);
	//	}
	//	md5 = ToUpper(md5);
	//	if (_16Bit)
	//		return md5.substr(8, 16);
	//	return md5;
	//}

	/// <summary>
	/// Get current process exe file path
	/// </summary>
	/// <returns>Exe directory</returns>
	std::wstring Utils::GetExePath()
	{
		wchar_t imgName[MAX_PATH] = { 0 };
		DWORD len = ARRAYSIZE(imgName);

		auto pFunc = GET_IMPORT(QueryFullProcessImageNameW);
		if (pFunc != nullptr)
			pFunc(GetCurrentProcess(), 0, imgName, &len);
		else
			GetModuleFileNameW(NULL, imgName, len);

		return imgName;
	}

	DWORD Utils::GetModuleLen(HMODULE hModule)
	{
		PBYTE pImage = (PBYTE)hModule;
		PIMAGE_DOS_HEADER pImageDosHeader;
		PIMAGE_NT_HEADERS pImageNtHeader;
		pImageDosHeader = (PIMAGE_DOS_HEADER)pImage;
		if (pImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		{
			return 0;
		}
		pImageNtHeader = (PIMAGE_NT_HEADERS)&pImage[pImageDosHeader->e_lfanew];
		if (pImageNtHeader->Signature != IMAGE_NT_SIGNATURE)
		{
			return 0;
		}
		return pImageNtHeader->OptionalHeader.SizeOfImage;
	}

	void Utils::MessageBoxTermination(std::wstring text, std::wstring caption)
	{
	}

	void Utils::writeToClipboard(const std::string& text) {
		// 打开剪贴板
		if (!OpenClipboard(nullptr)) {
			CONSOLE_INFO2("Failed to open clipboard");
			return;
		}

		// 清空剪贴板
		if (!EmptyClipboard()) {
			CONSOLE_INFO2("Failed to empty clipboard");
			CloseClipboard();
			return;
		}

		// 分配全局内存
		HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, text.length() + 1);
		if (hClipboardData == nullptr) {
			CONSOLE_INFO2("Failed to allocate memory for clipboard data");
			CloseClipboard();
			return;
		}

		// 锁定内存
		char* pClipboardData = static_cast<char*>(GlobalLock(hClipboardData));
		if (pClipboardData == nullptr) {
			CONSOLE_INFO2("Failed to lock memory for clipboard data");
			GlobalFree(hClipboardData);
			CloseClipboard();
			return;
		}

		// 将字符串复制到内存中
		strcpy_s(pClipboardData, text.length() + 1, text.c_str());

		// 解锁内存
		GlobalUnlock(hClipboardData);

		// 将内存句柄放入剪贴板
		SetClipboardData(CF_TEXT, hClipboardData);

		// 关闭剪贴板
		CloseClipboard();
		CONSOLE_INFO2("Copy the URL successfully,Press CTRL+V To Browser");
	}

	BOOL Utils::IsAdministratorRun()
	{
#define ACCESS_READ 1 
#define ACCESS_WRITE 2 

		// if(g_bIsNT==FALSE)  return TRUE; 
		HANDLE hToken;
		DWORD dwStatus;
		DWORD dwAccessMask;
		DWORD dwAccessDesired;
		DWORD dwACLSize;
		DWORD dwStructureSize = sizeof(PRIVILEGE_SET);
		PACL pACL = NULL;
		PSID psidAdmin = NULL;
		BOOL bReturn = FALSE;
		PRIVILEGE_SET ps;
		GENERIC_MAPPING GenericMapping;
		PSECURITY_DESCRIPTOR psdAdmin = NULL;
		SID_IDENTIFIER_AUTHORITY SystemSidAuthority = SECURITY_NT_AUTHORITY;

		if (!ImpersonateSelf(SecurityImpersonation))
			goto LeaveIsAdmin;

		if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, FALSE, &hToken))
		{
			if (GetLastError() != ERROR_NO_TOKEN)
				goto LeaveIsAdmin;

			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
				goto LeaveIsAdmin;

			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
				goto LeaveIsAdmin;
		}

		if (!AllocateAndInitializeSid(&SystemSidAuthority, 2,
			SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
			0, 0, 0, 0, 0, 0, &psidAdmin))
			goto LeaveIsAdmin;

		psdAdmin = LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (psdAdmin == NULL)
			goto LeaveIsAdmin;

		if (!InitializeSecurityDescriptor(psdAdmin,
			SECURITY_DESCRIPTOR_REVISION))
			goto LeaveIsAdmin;

		dwACLSize = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) +
			GetLengthSid(psidAdmin) - sizeof(DWORD);

		pACL = (PACL)LocalAlloc(LPTR, dwACLSize);
		if (pACL == NULL)
			goto LeaveIsAdmin;

		if (!InitializeAcl(pACL, dwACLSize, ACL_REVISION2))
			goto LeaveIsAdmin;

		dwAccessMask = ACCESS_READ | ACCESS_WRITE;

		if (!AddAccessAllowedAce(pACL, ACL_REVISION2, dwAccessMask, psidAdmin))
			goto LeaveIsAdmin;

		if (!SetSecurityDescriptorDacl(psdAdmin, TRUE, pACL, FALSE))
			goto LeaveIsAdmin;

		if (!SetSecurityDescriptorGroup(psdAdmin, psidAdmin, FALSE))
			goto LeaveIsAdmin;
		if (!SetSecurityDescriptorOwner(psdAdmin, psidAdmin, FALSE))
			goto LeaveIsAdmin;

		if (!IsValidSecurityDescriptor(psdAdmin))
			goto LeaveIsAdmin;

		dwAccessDesired = ACCESS_READ;

		GenericMapping.GenericRead = ACCESS_READ;
		GenericMapping.GenericWrite = ACCESS_WRITE;
		GenericMapping.GenericExecute = 0;
		GenericMapping.GenericAll = ACCESS_READ | ACCESS_WRITE;

		if (!AccessCheck(psdAdmin, hToken, dwAccessDesired,
			&GenericMapping, &ps, &dwStructureSize, &dwStatus, &bReturn))
			goto LeaveIsAdmin;

		if (!RevertToSelf())
			bReturn = FALSE;

	LeaveIsAdmin:

		if (pACL) LocalFree(pACL);
		if (psdAdmin) LocalFree(psdAdmin);
		if (psidAdmin) FreeSid(psidAdmin);

		return bReturn;
	}

	bool Utils::isAlphanumericWithSpecialChars(const std::string& str) {
		for (char const& c : str) {
			// 检查字符是否是字母数字、连字符或下划线，并且字符的 ASCII 值要大于0
			if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-' && c != '_') {
				return false;
			}
			// 检查字符的 ASCII 值是否大于0
			if (c <= 0) {
				return false;
			}
		}
		return true; // 所有字符都是字母、数字、连字符或下划线
	};

	bool Utils::checkAndCreateDirectory(const std::string& directoryPath) {
		// 检查目录是否存在
		if (_access(directoryPath.c_str(), 0) == 0) {
			return true;
		}
		else {
			// 创建目录
			if (_mkdir(directoryPath.c_str()) == 0) {
				return true;
			}
			else {
				return false;
			}
		}
	}

	bool Utils::StringIsValid(std::string s)
	{
		hasNullTerminator(s.data(), s.size() + 1);
		if (Utils::IsSafeReadPtr(s.data(), s.size()) || s.size() <= 3 || s.size() >= 256 || s.find("Unknown") != std::string::npos || s.find("None") != std::string::npos
			|| !Utils::isAlphanumericWithSpecialChars(s))
		{
			return false;
		}
		return true;
	}

	bool Utils::StringIsValid2(std::string s)
	{
		hasNullTerminator(s.data(), s.size() + 1);
		if (Utils::IsSafeReadPtr(s.data(), s.size()) || s.size() < 1 || s.size() >= 256 || s.find("Unknown") != std::string::npos || s.find("None") != std::string::npos
			|| !Utils::isAlphanumericWithSpecialChars(s))
		{
			return false;
		}
		return true;
	}

	bool Utils::hasNullTerminator(char* str, size_t maxLength) {
		__try
		{
			for (size_t i = 0; i < maxLength; ++i) {
				if (str[i] == '\0') {
					return true;
				}
			}
			// 如果没有找到终止符，并且有足够的空间，则添加终止符
			if (maxLength > 0) {
				str[maxLength - 1] = '\0';
				return false;
			}
		}
		__except (1)
		{

		}
		return false;
	}

	bool Utils::IsSafeReadPtr(void* ptr, size_t size) {
		if (!ptr)
			return true;
		__try {
			// Attempt to read the memory block
			//volatile char temp;
			//for (size_t i = 0; i < size; ++i) {
			//	temp = static_cast<volatile char*>(ptr)[i];
			//}
			//return false;
			return IsBadReadPtr(ptr, size);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return true;
		}
	}

	bool Utils::IsSafeWritePtr(void* ptr, size_t size) {
		if (!ptr)
			return true;
		__try {
			// Attempt to read the memory block
			//volatile char temp;
			//for (size_t i = 0; i < size; ++i) {
			//	temp = static_cast<volatile char*>(ptr)[i];
			//}
			//return false;
			return IsBadWritePtr(ptr, size);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return true;
		}
	}

	std::string Utils::SimpleHttpGet(const std::string& url, const std::string& additionalHeaders) {
		std::string result;

		// Initialize WinHTTP session
		HINTERNET hSession = WinHttpOpen(L"WinHTTP Example/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0);
		if (!hSession) {
			CONSOLE_INFO("WinHttpOpen failed: %x", GetLastError());
			return "";
		}

		// Split the URL into components
		URL_COMPONENTS urlComp;
		ZeroMemory(&urlComp, sizeof(urlComp));
		urlComp.dwStructSize = sizeof(urlComp);

		// Set required component lengths to non-zero 
		urlComp.dwSchemeLength = (DWORD)-1;
		urlComp.dwHostNameLength = (DWORD)-1;
		urlComp.dwUrlPathLength = (DWORD)-1;
		urlComp.dwExtraInfoLength = (DWORD)-1;

		std::wstring wUrl(url.begin(), url.end());
		if (!WinHttpCrackUrl(wUrl.c_str(), (DWORD)wUrl.length(), 0, &urlComp)) {
			CONSOLE_INFO("WinHttpCrackUrl failed: %x", GetLastError());
			WinHttpCloseHandle(hSession);
			return "";
		}

		std::wstring hostName(urlComp.lpszHostName, urlComp.dwHostNameLength);
		std::wstring urlPath(urlComp.lpszUrlPath, urlComp.dwUrlPathLength + urlComp.dwExtraInfoLength);

		// Create an HTTP connection
		HINTERNET hConnect = WinHttpConnect(hSession, hostName.c_str(), urlComp.nPort, 0);
		if (!hConnect) {
			CONSOLE_INFO("WinHttpConnect failed: %x", GetLastError());
			WinHttpCloseHandle(hSession);
			return "";
		}

		// Open an HTTP request handle
		HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", urlPath.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
			(urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);
		if (!hRequest) {
			CONSOLE_INFO("WinHttpOpenRequest failed: %x", GetLastError());
			WinHttpCloseHandle(hConnect);
			WinHttpCloseHandle(hSession);
			return "";
		}

		// Add additional headers if provided
		if (!additionalHeaders.empty()) {
			std::wstring wHeaders(additionalHeaders.begin(), additionalHeaders.end());
			if (!WinHttpAddRequestHeaders(hRequest, wHeaders.c_str(), (DWORD)-1, WINHTTP_ADDREQ_FLAG_ADD)) {
				CONSOLE_INFO("WinHttpAddRequestHeaders failed: %x", GetLastError());
				WinHttpCloseHandle(hRequest);
				WinHttpCloseHandle(hConnect);
				WinHttpCloseHandle(hSession);
				return "";
			}
		}

		// Send a request
		if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
			CONSOLE_INFO("WinHttpSendRequest failed: %x", GetLastError());
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			WinHttpCloseHandle(hSession);
			return "";
		}

		// Receive a response
		if (!WinHttpReceiveResponse(hRequest, NULL)) {
			CONSOLE_INFO("WinHttpReceiveResponse failed: %x", GetLastError());
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			WinHttpCloseHandle(hSession);
			return "";
		}

		// Read the data
		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		LPSTR pszOutBuffer = nullptr;

		do {
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
				CONSOLE_INFO("WinHttpQueryDataAvailable failed: %x", GetLastError());
				break;
			}

			if (dwSize == 0)
				break;

			pszOutBuffer = new char[dwSize + 1];
			if (!pszOutBuffer) {
				CONSOLE_INFO("Out of memory");
				break;
			}

			ZeroMemory(pszOutBuffer, dwSize + 1);

			if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
				CONSOLE_INFO("WinHttpReadData failed: %x", GetLastError());
				delete[] pszOutBuffer;
				break;
			}

			result.append(pszOutBuffer, dwDownloaded);
			delete[] pszOutBuffer;

		} while (dwSize > 0);

		// Cleanup
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return result;
	}

	void* ReadLibrary(const char* Name, size_t* pSize) {
		size_t read;
		void* result;
		FILE* fp;

		fp = fopen(Name, "rb");
		if (fp == NULL)
		{
			CONSOLE_INFO("Can't open DLL file \"%s\".", Name);
			return NULL;
		}

		fseek(fp, 0, SEEK_END);
		*pSize = static_cast<size_t>(ftell(fp));
		if (*pSize == 0)
		{
			fclose(fp);
			return NULL;
		}

		result = (unsigned char*)malloc(*pSize);
		if (result == NULL)
		{
			return NULL;
		}

		fseek(fp, 0, SEEK_SET);
		read = fread(result, 1, *pSize, fp);
		fclose(fp);
		if (read != *pSize)
		{
			free(result);
			return NULL;
		}

		return result;
	}

	float RandomFloat() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		return dis(gen);
	}

	float RandomRange(float min, float max) {
		if (min > max) {
			float Temp = min;
			min = max;
			max = Temp;
		}
		return RandomFloat() * (max - min) + min;
	}

	std::string GetCPUID() {
		int CPUInfo[4] = { -1 };
		__cpuid(CPUInfo, 0);
		std::string id = std::to_string(CPUInfo[0]) + std::to_string(CPUInfo[1]) + std::to_string(CPUInfo[2]) + std::to_string(CPUInfo[3]);
		return id;
	}

	std::string GetVolumeSerialNumber() {
		DWORD serialNumber = 0;
		GetVolumeInformationA(
			"C:\\",
			NULL,
			0,
			&serialNumber,
			NULL,
			NULL,
			NULL,
			0
		);
		return std::to_string(serialNumber);
	}

	std::string GetMACAddress() {
		PIP_ADAPTER_INFO AdapterInfo;
		DWORD dwBufLen = sizeof(AdapterInfo);
		AdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
		if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
			AdapterInfo = (IP_ADAPTER_INFO*)malloc(dwBufLen);
		}
		if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
			PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
			std::string macAddr = "";
			for (UINT i = 0; i < pAdapterInfo->AddressLength; i++) {
				char hex[3];
				sprintf(hex, "%02X", pAdapterInfo->Address[i]);
				macAddr += hex;
				if (i < pAdapterInfo->AddressLength - 1) {
					macAddr += ":";
				}
			}
			free(AdapterInfo);
			return macAddr;
		}
		free(AdapterInfo);
		return "";
	}

	hash_t Utils::GetMachineCode() {
		hash_t cpuID = hash_(GetCPUID());
		hash_t volumeSerial = hash_(GetVolumeSerialNumber());
		hash_t macAddress = hash_(GetMACAddress());
		hash_t machineCode = ((((cpuID + 52) ^ volumeSerial) - 0x10065) + macAddress) ^ macAddress;
		return machineCode;
	}
}