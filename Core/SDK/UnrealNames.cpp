#include "Include/WinHeaders.hpp"
#include "UnrealNames.h"
#include "../Core.hpp"
#include "../Config.h"
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <filesystem>
#include <bitset>
bool useCache = true;
namespace IronMan::Core::SDK
{

	UEEncryptedObjectProperty<TNameEntryArray*, DecryptFunc::General> GlobalNames;
	ptr_t GlobalNames1;
	ptr_t GlobalNames2;
	ptr_t GlobalNames3;
	std::unordered_map<hash_t, int>GNameStore;
	std::vector<StringA> ScatterNameArray;

	bool NamesStore::Initialize()
	{
		static bool bInitNameFirst = false;
		auto start = GetTickCount64();
		do
		{
			auto encryptName = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(GetDMA().Read<uint64_t>(g_PatternData.GNames));
			if (encryptName)
			{
				auto tempName = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(GetDMA().Read<uint64_t>((ptr_t)encryptName));
				if (tempName)
				{
					encryptName = GetDMA().Read<void*>((ptr_t)tempName);
					auto tempName2 = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(GetDMA().Read<uint64_t>((ptr_t)tempName));
					if (tempName2)
					{
						*(void**)(&GlobalNames) = (void*)encryptName;
						GlobalNames1 = ((ptr_t)tempName2 + 0x10);
						GlobalNames2 = ((ptr_t)tempName2 + 0x08);
						GlobalNames3 = ((ptr_t)tempName2);
						break;

					}
				}
			}
		} while (true);
		if (!bInitNameFirst)
		{
			ScatterNameArray.resize(1000000);
			ZeroMemory(ScatterNameArray.data(), sizeof(StringA) * 1000000);
		}
		else
		{
			ZeroMemory(ScatterNameArray.data(), sizeof(StringA) * 1000000);
		}
		std::vector<ptr_t>pSerialsArray;
		std::vector<ptr_t>pNameArray;
		pSerialsArray.resize(1000000);
		pNameArray.resize(1000000);
		auto HandleScatter = GetDMA().createScatterHandle();
		auto Name = GlobalNames.Get();
		auto GNamePtr = UEEncryptedObjectProperty<ptr_t, DecryptFunc::General>::STATIC_Get(GetDMA().Read<ptr_t>(GlobalNames2));
		if (!IsAddrValid(GNamePtr))
			GNamePtr = UEEncryptedObjectProperty<ptr_t, DecryptFunc::General>::STATIC_Get(GetDMA().Read<ptr_t>(GlobalNames3));
		if (!IsAddrValid(GNamePtr))
			GNamePtr = UEEncryptedObjectProperty<ptr_t, DecryptFunc::General>::STATIC_Get(GetDMA().Read<ptr_t>(GlobalNames1));
		if (!IsAddrValid(GNamePtr))
		{
			MessageBoxA(NULL, u8"获取数据失败!", 0, 0);
			//TerminateProcess(GetCurrentProcess(), -1);
		}
		for (int i = 0; i < 1000000; i++)
		{
			GetNameByID1(HandleScatter, i, GNamePtr, &pSerialsArray[i]);
		}
		GetDMA().executeScatterRead(HandleScatter);
		for (int i = 0; i < 1000000; i++)
		{
			GetNameByID2(HandleScatter, i, pSerialsArray[i], &pNameArray[i]);
		}
		GetDMA().executeScatterRead(HandleScatter);
		for (int i = 0; i < 1000000; i++)
		{
			GetNameByID3(HandleScatter, i, pNameArray[i], ScatterNameArray[i].buffer);
		}
		GetDMA().executeScatterRead(HandleScatter);
		GetDMA().closeScatterHandle(HandleScatter);
		auto validate_and_clear = [=](struct StringA* strA) {
			// 找到字符串终止符的位置
			size_t len = strnlen(strA->buffer, sizeof(strA->buffer));
			// 如果终止符位置小于128，清零剩余内存
			if (len < sizeof(strA->buffer)) {
				memset(strA->buffer + len, 0, sizeof(strA->buffer) - len);
			}
			else {
				// 如果没有找到终止符，则清零整个内存
				memset(strA->buffer, 0, sizeof(strA->buffer));
			}
		};

		for (int i = 0; i < 1000000; i++)
			validate_and_clear(&ScatterNameArray[i]);

		CONSOLE_INFO("NameStoreEnd:%d", GetTickCount64() - start);
		bInitNameFirst = true;
		return std::string(ScatterNameArray[0].buffer) == "None";
	}

	void NamesStore::GetNameByID(VMMDLL_SCATTER_HANDLE Handle, int ID, ptr_t GName, void* ret)
	{
		ULONG IdDiv = ID / g_PatternData.ElementsPerChunk;
		ULONG Idtemp = ID % g_PatternData.ElementsPerChunk;
		ptr_t Serial = GetDMA().ReadCache<ptr_t>(GName + IdDiv * 0x8);
		if (!Serial)
			return;

		ptr_t pName = GetDMA().ReadCache<ptr_t>(Serial + 0x8 * Idtemp);
		if (!pName)
			return;
		GetDMA().queueScatterReadEx(Handle, pName + 0x10, ret, 64);
		return;
	}

	void NamesStore::GetNameByID1(VMMDLL_SCATTER_HANDLE Handle, int ID, ptr_t GName, void* ret)
	{
		ULONG IdDiv = ID / g_PatternData.ElementsPerChunk;
		ULONG Idtemp = ID % g_PatternData.ElementsPerChunk;
		GetDMA().queueScatterReadEx(Handle, GName + IdDiv * 0x8, ret, 8);
		return;
	}

	void NamesStore::GetNameByID2(VMMDLL_SCATTER_HANDLE Handle, int ID, ptr_t PtrAddr, void* ret)
	{
		ULONG IdDiv = ID / g_PatternData.ElementsPerChunk;
		ULONG Idtemp = ID % g_PatternData.ElementsPerChunk;
		if (!IsAddrValid(PtrAddr))
			return;
		GetDMA().queueScatterReadEx(Handle, PtrAddr + 0x8 * Idtemp, ret, 8);
		return;
	}

	void NamesStore::GetNameByID3(VMMDLL_SCATTER_HANDLE Handle, int ID, ptr_t GNameAddr, void* ret)
	{
		if (!IsAddrValid(GNameAddr))
			return;
		GetDMA().queueScatterReadEx(Handle, GNameAddr + 0x10, ret, 128);
		return;
	}

	void* NamesStore::GetAddress()
	{
		return GlobalNames.Get();
	}

	NamesIterator NamesStore::begin()
	{
		return NamesIterator(*this, 0);
	}

	NamesIterator NamesStore::begin() const
	{
		return NamesIterator(*this, 0);
	}

	NamesIterator NamesStore::end()
	{
		return NamesIterator(*this);
	}

	NamesIterator NamesStore::end() const
	{
		return NamesIterator(*this);
	}

	size_t NamesStore::GetNamesNum() const
	{
		return GlobalNames->Num();
	}

	bool NamesStore::IsValid(size_t id) const
	{
		return GlobalNames->IsValidIndex(static_cast<int32_t>(id));
	}

	std::string NamesStore::GetById(size_t id) const
	{
		return GlobalNames->GetById(static_cast<int32_t>(id)).GetAnsiName();
	}



	NamesIterator::NamesIterator(const NamesStore& _store)
		: store(_store),
		index(_store.GetNamesNum())
	{
	}

	NamesIterator::NamesIterator(const NamesStore& _store, size_t _index)
		: store(_store),
		index(_index)
	{
	}

	void NamesIterator::swap(NamesIterator& other) noexcept
	{
		std::swap(index, other.index);
	}

	NamesIterator& NamesIterator::operator++()
	{
		for (++index; index < store.GetNamesNum(); ++index)
		{
			if (store.IsValid(index))
			{
				break;
			}
		}
		return *this;
	}

	NamesIterator NamesIterator::operator++ (int)
	{
		auto tmp(*this);
		++(*this);
		return tmp;
	}

	bool NamesIterator::operator==(const NamesIterator& rhs) const
	{
		return index == rhs.index;
	}

	bool NamesIterator::operator!=(const NamesIterator& rhs) const
	{
		return index != rhs.index;
	}

	UENameInfo NamesIterator::operator*() const
	{
		return { index, store.GetById(index) };
	}

	UENameInfo NamesIterator::operator->() const
	{
		return { index, store.GetById(index) };
	}
}