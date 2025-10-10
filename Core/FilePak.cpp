#include "Include/WinHeaders.hpp"
#include "FilePak.h"
#include <istream>

FILE* ImFileOpen(const char* filename, const char* mode);

namespace IronMan::Core
{

	Package* Package::m_Instance = nullptr;
	Package* Package::GetPackage()
	{
		if (m_Instance == nullptr) { m_Instance = new Package(); }
		return m_Instance;
	}
	Package::~Package()
	{
		if (fileBuf)
		{
			RtlSecureZeroMemory((void*)fileBuf, fileSize);
			delete[](void*)fileBuf;
			//FreeMem(PID, fileBuf, fileSize, MEM_RELEASE);
		}
	}

	bool Package::Load(std::string packagePath)
	{
		FILE* f;
		if ((f = fopen(packagePath.c_str(), ("rb"))) == NULL)
			return false;

		long file_size_signed;
		if (fseek(f, 0, SEEK_END) || (file_size_signed = ftell(f)) == -1 || fseek(f, 0, SEEK_SET))
		{
			fclose(f);
			return false;
		}

		auto file_size = fileSize = (size_t)file_size_signed;

		fileBuf = (ptr_t)new char[fileSize];
		/*if (!NT_SUCCESS(AllocateMem(PID, fileBuf, fileSize, MEM_COMMIT, PAGE_READWRITE)))
			return false;

		if (!NT_SUCCESS(ConcealVAD(PID, fileBuf, fileSize)))
			return false;*/


		void* file_data = (void*)fileBuf;
		if (file_data == NULL)
		{
			fclose(f);
			return false;
		}
		if (fread(file_data, 1, file_size, f) != file_size)
		{
			fclose(f);
			return false;
		}
		fclose(f);
		std::string str((char*)file_data, file_size);
		m_PackageFile = std::istringstream(str, std::ios_base::in | std::ios_base::binary);
		if (m_PackageFile.good())
		{
			m_PackageFile.read((char*)&m_Header, sizeof(PackageHeader));

			if (strcmp(m_Header.fileID, ("PAK")) != 0 || !(m_Header.numberFiles > 0) || m_Header.version != 1)
			{
				return false;
			}

			m_Entries.clear();
			char* buffer;
			for (int i = 0; i < m_Header.numberFiles; i++)
			{
				buffer = new char[sizeof(FileEntry)];
				FileEntry entry;
				m_PackageFile.read(buffer, sizeof(FileEntry));

				for (int j = 0; j < sizeof(FileEntry); j++)
				{
					if (m_Header.additionEncrypt) buffer[j] -= m_Header.encryptVal; //decrypt each byte
					else buffer[j] += m_Header.encryptVal;
				}

				memcpy(&entry, buffer, sizeof(FileEntry)); //store the decrypted stuff into the entry

				FileEntryHash entryHash;
				entryHash.name = hash_(entry.name);
				entryHash.fullname = hash_(entry.fullname);
				entryHash.size = entry.size;
				entryHash.offset = entry.offset;
				m_Entries.push_back(entryHash); //append to the vector
				delete[] buffer;
			}
			m_bLoaded = true;
			str.clear();
			return true;
		}
		return false;
	}

	Package::FileEntryHash* Package::FindFileEntryHash(hash_t fileNameHash)
	{
		if (m_bLoaded)
		{
			for (auto& entry : m_Entries)
			{
				if (entry.name == fileNameHash)
					return &entry;
			}
		}
		return nullptr;//PAK file isn't loaded, or entry isn't found
	}
	size_t Package::FindFile(hash_t fileNameHash, void** outData)
	{
		if (outData != nullptr)
		{
			char* buffer = nullptr;
			if (auto entry = FindFileEntryHash(fileNameHash))
			{
				buffer = (char*)malloc(entry->size);
				m_PackageFile.seekg(entry->offset, std::ifstream::beg); //seek to the offset of the file in the .pak file
				m_PackageFile.read(buffer, entry->size);

				for (unsigned int j = 0; j < entry->size; j++)
				{
					if (m_Header.additionEncrypt) buffer[j] -= m_Header.encryptVal; //decrypt it
					else buffer[j] += m_Header.encryptVal;
				}
				*outData = buffer;

				return entry->size;
			}
		}
		return 0;
	}
	void Package::Release()
	{
		if (m_Instance)
		{
			delete m_Instance;
			m_Instance = nullptr;
		}
	}
	Package::Package()
		:m_bLoaded(false),
		m_Header()
	{

	}


}

