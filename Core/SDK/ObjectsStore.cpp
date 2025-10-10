#include "Include/WinHeaders.hpp"
#include "../Config.h"
#include "ObjectsStore.h"
#include "UEncrypt.h"
#include "SDK.h"
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <filesystem>
#include <bitset>
namespace IronMan::Core::SDK
{
	std::unordered_map<hash_t, UObject> ObjectCache;
	TUObjectArray* GlobalObjects = nullptr;

	bool ObjectsStore::Initialize()
	{
		auto startTick = GetTickCount64();
		CONSOLE_INFO("ObjectsStore");
		void* TempObj = malloc(sizeof(TUObjectArray));
		GetDMA().Read(g_PatternData.UObjectArray, (ptr_t)TempObj, sizeof(TUObjectArray));
		GlobalObjects = (TUObjectArray*)TempObj;
		auto Totle = ObjectsStore().GetObjectsNum();
		while (!Totle)
		{
			GetDMA().Read(g_PatternData.UObjectArray, (ptr_t)TempObj, sizeof(TUObjectArray));
			GlobalObjects = (TUObjectArray*)TempObj;
			Totle = ObjectsStore().GetObjectsNum();
		}
		CONSOLE_INFO("ObjectNum:%d", Totle);

		if (Vars.Menu.firstInit)
		{
			ObjectCache.clear();
			ClearObjectCache();
		}
		std::unique_ptr<std::vector<FUObjectItem>>ObjectArray(new std::vector<FUObjectItem>());
		if (Totle > 100000)
			Totle = 100000;
		ObjectArray->resize(Totle);
		GetDMA().Read((ptr_t)GlobalObjects->Objects.Get(), (ptr_t)ObjectArray->data(), sizeof(FUObjectItem) * Totle);


		std::unique_ptr<UObject> Test(new UObject((*ObjectArray)[0].Object));
		int NameMethod = Test->TestName();
		CONSOLE_INFO("NameMehtod %d", NameMethod);
		if (NameMethod == 2)
		{
			auto back = g_PatternData.offset_UObjectNameComparisonIndex;
			g_PatternData.offset_UObjectNameComparisonIndex = g_PatternData.offset_UObjectNameNumber;
			g_PatternData.offset_UObjectNameNumber = back;
			UEDecryptBuilder::SetExchange();
		}

		std::unique_ptr < std::vector<int32_t>>ObjectFNameArray(new std::vector<int32_t>(Totle));
		std::unique_ptr < std::vector<FName>>ObjectDecFNameArray(new std::vector<FName>(Totle));
		std::unique_ptr < std::vector<ptr_t>>ClassArray(new std::vector<ptr_t>(Totle));
		std::unique_ptr < std::vector<int32_t>>ClassFNameArray(new std::vector<int32_t>(Totle));
		std::unique_ptr < std::vector<FName>>ClassDecFNameArray(new std::vector<FName>(Totle));
		std::unique_ptr < std::vector<ptr_t>>OuterBackupArray(new std::vector<ptr_t>(Totle));
		std::unique_ptr < std::vector<FName>> OuterFNameArray(new std::vector<FName>(Totle));
		std::unique_ptr < std::vector<ptr_t>>OuterArray(new std::vector<ptr_t>(Totle));
		std::unique_ptr < std::vector<std::string>> OuterNameArray(new std::vector<std::string>(Totle));
		auto ObjectHanlde = GetDMA().createScatterHandle();
		for (int i = 0; i < Totle; i++)
		{
			GetDMA().queueScatterReadEx(ObjectHanlde, (ptr_t)(*ObjectArray)[i].Object + g_PatternData.offset_UObjectNameComparisonIndex, &(*ObjectFNameArray)[i], 4);
			GetDMA().queueScatterReadEx(ObjectHanlde, (ptr_t)(*ObjectArray)[i].Object + g_PatternData.offset_UObjectClass, &(*ClassArray)[i], 8);
			GetDMA().queueScatterReadEx(ObjectHanlde, (ptr_t)(*ObjectArray)[i].Object + g_PatternData.offset_UObjectOuter, &(*OuterBackupArray)[i], 8);
		}
		GetDMA().executeScatterRead(ObjectHanlde);

		bool stopLoop = false;
		std::unordered_map<ptr_t, std::string>OuterNameCache;

		int MaxOuterCount = 0;
		auto start = std::chrono::high_resolution_clock::now();
	regetOuter:
		for (int i = 0; i < Totle; i++)
		{
			auto ObjClass = UEEncryptedObjectProperty<ptr_t, DecryptFunc::UObjectClass>::STATIC_Get((*ClassArray)[i]);
			if (!ObjClass)
			{
				(*OuterBackupArray)[i] = 0;
				continue;
			}
			if (MaxOuterCount == 0)
			{
				GetDMA().queueScatterReadEx(ObjectHanlde, ObjClass + g_PatternData.offset_UObjectNameComparisonIndex, &(*ClassFNameArray)[i], 4);
			}
			(*OuterArray)[i] = UEEncryptedObjectProperty<ptr_t, DecryptFunc::UObjectOuter>::STATIC_Get((*OuterBackupArray)[i]);
			(*OuterBackupArray)[i] = 0;
			if (IsAddrValid((*OuterArray)[i]))
				GetDMA().queueScatterReadEx(ObjectHanlde, (*OuterArray)[i] + g_PatternData.offset_UObjectNameComparisonIndex, &(*OuterFNameArray)[i].ComparisonIndex, 4);
		}
		GetDMA().executeScatterRead(ObjectHanlde);
		stopLoop = true;
		for (int i = 0; i < Totle; i++)
		{
			auto ObjClass = UEEncryptedObjectProperty<ptr_t, DecryptFunc::UObjectClass>::STATIC_Get((*ClassArray)[i]);
			if (!ObjClass)
				continue;
			if (!IsAddrValid((*OuterArray)[i]))
				continue;
			(*OuterFNameArray)[i].CompositeComparisonValue = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get((*OuterFNameArray)[i].CompositeComparisonValue);
			(*OuterNameArray)[i] = (*OuterFNameArray)[i].GetName2() + "." + (*OuterNameArray)[i];
			auto pos = (*OuterNameArray)[i].rfind('/');
			if (pos != std::string::npos)
				(*OuterNameArray)[i] = (*OuterNameArray)[i].substr(pos + 1);
			(*OuterFNameArray)[i].CompositeComparisonValue = 0;
			GetDMA().queueScatterReadEx(ObjectHanlde, (*OuterArray)[i] + g_PatternData.offset_UObjectOuter, &(*OuterBackupArray)[i], 8);
			if (stopLoop)
				stopLoop = false;
		}
		GetDMA().executeScatterRead(ObjectHanlde);
		MaxOuterCount++;
		if (MaxOuterCount < 10 && !stopLoop)
			goto regetOuter;

		for (int i = 0; i < Totle; i++)
		{
			(*ObjectDecFNameArray)[i].ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get((*ObjectFNameArray)[i]);
			(*ClassDecFNameArray)[i].ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get((*ClassFNameArray)[i]);

			std::string ClassName = (*ClassDecFNameArray)[i].GetName2(true);
			std::string ObjectName = (*ObjectDecFNameArray)[i].GetName2(true);
			auto posClass = ClassName.rfind('/');
			if (posClass != std::string::npos)
				ClassName = ClassName.substr(posClass + 1);
			auto posObject = ObjectName.rfind('/');
			if (posObject != std::string::npos)
				ObjectName = ObjectName.substr(posObject + 1);
			std::string FullName = ClassName + " " + (*OuterNameArray)[i] + ObjectName;
			ObjectCache.emplace(hash_(FullName), (*ObjectArray)[i].Object);
		}

		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		return GlobalObjects != nullptr;
	}

	void* ObjectsStore::GetAddress()
	{
		return GlobalObjects;
	}

	size_t ObjectsStore::GetObjectsNum() const
	{
		return GlobalObjects->NumElements;
	}

	FUObjectItem ObjectsStore::GetItemById(size_t id) const
	{

		GlobalObjects = &sdk::GetObjectArray();
		if (id < GlobalObjects->NumElements)
		{
			ptr_t Objects_Temp = (ptr_t)GlobalObjects->Objects.Get() + id * sizeof(FUObjectItem);
			return GetDMA().ReadCache<FUObjectItem>(Objects_Temp);
			//auto ItemObj = Game.memory().Read<FUObjectItem>(Objects_Temp);
			//if (NT_SUCCESS(ItemObj.status))
			//	return ItemObj.result(0);
		}
		return nullptr;
	}

	FUObjectItem ObjectsStore::GetItemByIdRealTime(size_t id) const
	{

		GlobalObjects = &sdk::GetObjectArray();
		if (id < GlobalObjects->NumElements)
		{
			ptr_t Objects_Temp = (ptr_t)GlobalObjects->Objects.Get() + id * sizeof(FUObjectItem);
			return GetDMA().Read<FUObjectItem>(Objects_Temp);
			//auto ItemObj = Game.memory().Read<FUObjectItem>(Objects_Temp);
			//if (NT_SUCCESS(ItemObj.status))
			//	return ItemObj.result(0);
		}
		return nullptr;
	}

	UObject ObjectsStore::GetById(size_t id) const
	{
		static FUObjectItem* UObjectArray = nullptr;
		static uint32_t Elements = 0;
		if (id < GlobalObjects->NumElements)
		{
			if (Elements != GlobalObjects->NumElements)
			{
				if (!Utils::IsSafeReadPtr(UObjectArray,sizeof(FUObjectItem)))
				{
					free(UObjectArray);
					UObjectArray = NULL;
				}
				UObjectArray = (FUObjectItem*)malloc(GlobalObjects->NumElements * sizeof(FUObjectItem));
				Elements = GlobalObjects->NumElements;
				GetDMA().Read((ULONG64)GlobalObjects->Objects.Get(), (ULONG64)UObjectArray, GlobalObjects->NumElements * sizeof(FUObjectItem));
				if (Utils::IsSafeReadPtr(UObjectArray, sizeof(FUObjectItem)))
				{
					free(UObjectArray);
					UObjectArray = NULL;
					Elements = 0;
					return nullptr;
				}
			}
			if (GlobalObjects->NumElements == id)
			{
				free(UObjectArray);
				UObjectArray = NULL;
				Elements = 0;
			}
			if (UObjectArray)
				return UObjectArray[id].Object;
			return NULL;
		}
		return nullptr;
	}


	ObjectsIterator ObjectsStore::begin()
	{
		return ObjectsIterator(*this, 0);
	}

	ObjectsIterator ObjectsStore::begin() const
	{
		return ObjectsIterator(*this, 0);
	}

	ObjectsIterator ObjectsStore::end()
	{
		return ObjectsIterator(*this);
	}

	ObjectsIterator ObjectsStore::end() const
	{
		return ObjectsIterator(*this);
	}

	ObjectsIterator::ObjectsIterator(const ObjectsStore& _store)
		: store(_store),
		index(_store.GetObjectsNum())
	{
	}

	ObjectsIterator::ObjectsIterator(const ObjectsStore& _store, size_t _index)
		: store(_store),
		index(_index),
		current(_store.GetById(_index))
	{
	}

	ObjectsIterator::ObjectsIterator(const ObjectsIterator& other)
		: store(other.store),
		index(other.index),
		current(other.current)
	{
	}

	ObjectsIterator::ObjectsIterator(ObjectsIterator&& other) noexcept
		: store(other.store),
		index(other.index),
		current(other.current)
	{
	}

	ObjectsIterator& ObjectsIterator::operator=(const ObjectsIterator& rhs)
	{
		index = rhs.index;
		current = rhs.current;
		return *this;
	}

	void ObjectsIterator::swap(ObjectsIterator& other) noexcept
	{
		std::swap(index, other.index);
		std::swap(current, other.current);
	}

	ObjectsIterator& ObjectsIterator::operator++()
	{
		for (++index; index < store.GetObjectsNum(); ++index)
		{
			current = store.GetById(index);
			if (current.IsValid())
			{
				break;
			}
		}
		return *this;
	}

	ObjectsIterator ObjectsIterator::operator++(int)
	{
		auto tmp(*this);
		++(*this);
		return tmp;
	}

	bool ObjectsIterator::operator==(const ObjectsIterator& rhs) const
	{
		return index == rhs.index;
	}

	bool ObjectsIterator::operator!=(const ObjectsIterator& rhs) const
	{
		return index != rhs.index;
	}

	UObject ObjectsIterator::operator*() const
	{
		assert(current.IsValid() && "ObjectsIterator::current is not valid!");

		return current;
	}

	UObject ObjectsIterator::operator->() const
	{
		return operator*();
	}

}