#include "Include/WinHeaders.hpp"
#include "Object.h"
#include "Class.h"
#include "../ObjectsStore.h"
#include "../UnrealNames.h"
#include "../UEncrypt.h"
#include "../SDK.h"
namespace IronMan::Core::SDK
{
	void* INVALID_POINTER = (void*)(-1);
	static int uNormal = 0;
	static std::unordered_map<uint32_t, std::string>CacheName;
	static std::unordered_map<ptr_t, std::string>CacheOuterName;
	std::unordered_map<ptr_t, ptr_t> DecryptCache;

	bool DecryptDataByCache(ptr_t encryptData, ptr_t& outData)
	{
		auto cache = DecryptCache.find(encryptData);
		if (cache != DecryptCache.end())
		{
			outData = cache->second;
			return true;
		}
		return false;
	}

	void CacheDecryptData(ptr_t endata, ptr_t dedata)
	{
		DecryptCache.emplace(endata, dedata);
	}

	void ClearObjectCache()
	{
		uNormal = 0;
		CacheName.clear();
		CacheOuterName.clear();
	}



	UObject::UObject()
		:m_self(INVALID_POINTER),
		ObjectFlags(RF_INVALID),
		InternalIndex(-1),
		ClassPrivate(INVALID_POINTER),
		EncryptClass(INVALID_POINTER),
		NamePrivate(),
		OuterPrivate(INVALID_POINTER),
		bCached(false),
		bAddObject(false),
		bUseCache(true),
		bInvalid(false),
		m_name(),
		m_fullName(),
		m_type(UObjectType::EUnknown)
	{
		//WriteLock = std::make_shared<WfirstRWLock>();
	}
	UObject::UObject(const void*& newobject)
	{
		if (newobject && (ptr_t)this > 0x10000 && (ptr_t)newobject > 0x10000 && !Utils::IsSafeReadPtr((void*)newobject, 1))
		{
			UObject object = (UObject)newobject;
			m_self = object.m_self;
			ObjectFlags = object.ObjectFlags;
			InternalIndex = object.InternalIndex;
			ClassPrivate = object.ClassPrivate;
			EncryptClass = object.EncryptClass;
			NamePrivate = object.NamePrivate;
			OuterPrivate = object.OuterPrivate;
			bCached = object.bCached;
			bAddObject = object.bAddObject;
			bUseCache = object.bUseCache;
			bInvalid = object.bInvalid;
			m_name = object.m_name;
			m_fullName = object.m_fullName;
			m_type = object.m_type;
			//WriteLock = object.WriteLock;
		}
	}
	UObject::UObject(void* object) : UObject()
	{
		m_self = object;
		//WriteLock = std::make_shared<WfirstRWLock>();
	}
	UObject::~UObject()
	{
		//WriteLock.reset();
	}

	EObjectFlags UObject::GetObjectFlags()
	{
		if (ObjectFlags == RF_INVALID && IsValid())
		{
			auto encryptData = bUseCache ? GetDMA().ReadCache<uint32_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectFlags)
				: GetDMA().Read<uint32_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectFlags);
			if (encryptData)
				ObjectFlags = UEEncryptedObjectProperty<EObjectFlags, DecryptFunc::UObjectFlags>::STATIC_Get(encryptData);
		}
		return ObjectFlags;
	}
	size_t UObject::GetIndex()
	{
		if (!IsValid())
			return 0;
		if (InternalIndex == -1)
		{
			auto encryptData = bUseCache ? GetDMA().ReadCache<uint32_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectIndex)
				: GetDMA().Read<uint32_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectIndex);
			if (encryptData)
				InternalIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectIndex>::STATIC_Get(encryptData);
		}
		return InternalIndex;
	}
	UClass UObject::GetClass()
	{
		if (ClassPrivate == INVALID_POINTER && IsValid())
		{
			auto encryptData = bUseCache ? GetDMA().ReadCache<ptr_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectClass)
				: GetDMA().Read<ptr_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectClass);
			if (encryptData)
				ClassPrivate = UEEncryptedObjectProperty<void*, DecryptFunc::UObjectClass>::STATIC_Get(encryptData);
		}
		return UClass(ClassPrivate);
	}

	UObject UObject::GetOuter()
	{
		if (OuterPrivate == INVALID_POINTER && IsValid())
		{
			auto encryptData = bUseCache ? GetDMA().ReadCache<ptr_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectOuter)
				: GetDMA().Read<ptr_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectOuter);
			if (encryptData)
				OuterPrivate = UEEncryptedObjectProperty<void*, DecryptFunc::UObjectOuter>::STATIC_Get(encryptData);
		}
		return UObject(OuterPrivate);
	}
	std::string UObject::GetName(bool number, bool usecache)
	{
		if (!IsValid())
			return ("(null)");
		if (!m_name.empty())
			return m_name;
		if (NamePrivate.CompositeComparisonValue == 0)
		{
			if (usecache)
			{
				NamePrivate.ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(
					GetDMA().ReadCache<uint32_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectNameComparisonIndex));
				NamePrivate.Number = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameNumber>::STATIC_Get(
					GetDMA().ReadCache<uint32_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectNameNumber));
			}
			else
			{
				NamePrivate.ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(
					GetDMA().Read<uint32_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectNameComparisonIndex));
				NamePrivate.Number = 0;
			}
		}
		if (uNormal != 1)
		{
			if (NamePrivate.ComparisonIndex == 0 && NamePrivate.Number != 0)
			{
				auto TempV = NamePrivate.Number;
				NamePrivate.Number = NamePrivate.ComparisonIndex;
				NamePrivate.ComparisonIndex = TempV;
			}
		}

		auto Result = CacheName.find(NamePrivate.ComparisonIndex);
		if (Result == CacheName.end())
		{
			m_name = NamePrivate.GetName();
			CacheName.emplace(NamePrivate.ComparisonIndex, m_name);
		}
		else
			m_name = Result->second;
		//if (number && NamePrivate.Number > 0)
		//{
		//	m_name += '_' + std::to_string(NamePrivate.Number);
		//}



		auto pos = m_name.rfind('/');
		if (pos == std::string::npos)
		{
			return m_name;
		}

		return m_name.substr(pos + 1);
	}

	int UObject::TestName()
	{
		if (!IsValid())
			return 0;
		if (!m_name.empty())
			return 0;
		if (NamePrivate.CompositeComparisonValue == 0)
		{
			if (uNormal == 0)
			{
				NamePrivate.ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(
					GetDMA().Read<uint32_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectNameComparisonIndex));
				NamePrivate.Number = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameNumber>::STATIC_Get(
					GetDMA().Read<uint32_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectNameNumber));
				auto TestName = NamePrivate.GetName();
				if (TestName.find("CoreUObject") != std::string::npos)
				{
					CONSOLE_INFO("1:%s", TestName.c_str());
					uNormal = 1;
					return 1;
				}
				else
				{
					NamePrivate.Number = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(
						GetDMA().Read<uint32_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectNameComparisonIndex));
					NamePrivate.ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameNumber>::STATIC_Get(
						GetDMA().Read<uint32_t>((ptr_t)GetPtr() + g_PatternData.offset_UObjectNameNumber));
					TestName = NamePrivate.GetName();
					if (TestName.find("CoreUObject") != std::string::npos)
					{
						CONSOLE_INFO("2:%s", TestName.c_str());
						uNormal = 2;
						return 2;
					}
					else
					{
						MessageBoxA(0, "Name Error To Find", 0, 0);
						//TerminateProcess(GetCurrentProcess(), -1);
					}
				}
			}
		}
		else
		{
			CONSOLE_INFO("0:%p", NamePrivate.CompositeComparisonValue);
		}
		return 0;
	}

	std::string UObject::GetSuperClassName()
	{
		if (!IsValid())
			return ("(null)");
		if (!m_upperName.empty())
			return m_upperName;

		//if (this->Is_A<UClass>())
		{
			for (auto c = this->Cast<UStruct>(); c; c = c.GetSuperStruct()) {
				if (!m_upperName.empty())
					m_upperName += ("->");
				m_upperName += c.GetFullName();
			}
		}

		return m_upperName;
	}

	std::string UObject::GetFullName()
	{
		if (!IsValid() || !GetClass()->IsValid())
			return ("1:(null)");

		if (!m_fullName.empty())
			return m_fullName;

		std::string temp;

		auto objectOuter = GetOuter();
		if (objectOuter->IsValid())
		{
			auto findResult = CacheOuterName.find((ptr_t)objectOuter->GetPtr());
			if (findResult == CacheOuterName.end())
			{
				int i = 0;
				for (auto outer = GetOuter(); outer.IsValid(); outer = outer.GetOuter())
				{
					if (i++ > 10)
						return ("2:(null)");
					temp = outer.GetName() + "." + temp;
				}
				CacheOuterName.emplace((ptr_t)objectOuter->GetPtr(), temp);
			}
			else
				temp = findResult->second;
		}



		m_fullName = GetClass().GetName();
		m_fullName += " ";
		m_fullName += temp;
		m_fullName += GetName(false);
		return m_fullName;
	}

	bool UObject::IsA(UObjectType Type)
	{
		if (!IsValid())
			return false;

		if (!bCached || m_type == UObjectType::EUnknown)
		{
			if (Type == UObjectType::EPlayer && IsA<ATslCharacter>())
				m_type = UObjectType::EPlayer;
			else if (Type == UObjectType::EVehicle && IsA<ATslWheeledVehicle>())
				m_type = UObjectType::EVehicle;
			else if (Type == UObjectType::EBoat && IsA<ATslFloatingVehicle>())
				m_type = UObjectType::EBoat;
			else if (Type == UObjectType::EItemGroup && IsA<ADroppedItemGroup>())
				m_type = UObjectType::EItemGroup;
			else if (Type == UObjectType::EItem && IsA<ADroppedItem>())
				m_type = UObjectType::EItem;
			else if (Type == UObjectType::EDeathBox && IsA<AFloorSnapItemPackage>())
				m_type = UObjectType::EDeathBox;
			else if (Type == UObjectType::EAirdrop && IsA<ACarePackageItem>())
				m_type = UObjectType::EAirdrop;
			else if (Type == UObjectType::EProjectile && IsA<ATslProjectile>())
				m_type = UObjectType::EProjectile;
			else if (Type == UObjectType::EDroppedItem && IsA<UDroppedItemInteractionComponent>())
				m_type = UObjectType::EDroppedItem;
			else
				return false;

			bCached = true;
			return true;
		}
		return m_type == Type;
	}

	bool UObject::SetIsA(UObjectType Type)
	{
		if (!IsValid())
			return false;

		if (!bCached || m_type == UObjectType::EUnknown)
		{
			if (Type == UObjectType::EPlayer)
				m_type = UObjectType::EPlayer;
			else if (Type == UObjectType::EVehicle)
				m_type = UObjectType::EVehicle;
			else if (Type == UObjectType::EBoat)
				m_type = UObjectType::EBoat;
			else if (Type == UObjectType::EItemGroup)
				m_type = UObjectType::EItemGroup;
			else if (Type == UObjectType::EItem)
				m_type = UObjectType::EItem;
			else if (Type == UObjectType::EDeathBox)
				m_type = UObjectType::EDeathBox;
			else if (Type == UObjectType::EAirdrop)
				m_type = UObjectType::EAirdrop;
			else if (Type == UObjectType::EProjectile)
				m_type = UObjectType::EProjectile;
			else if (Type == UObjectType::EDroppedItem)
				m_type = UObjectType::EDroppedItem;
			else
				return false;

			bCached = true;
			return true;
		}
		return m_type == Type;
	}

	void UObject::ProcessEvent(UFunction function, void* parms)
	{
	}

	UClass UObject::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class CoreUObject.Object"_hash>();
		return c;
	}

	UClass UClass::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class CoreUObject.Class"_hash>();
		return c;
	}

	bool FWeakObjectPtr::IsValid() const
	{
		if (ObjectSerialNumber == 0)
		{
			return false;
		}
		if (ObjectIndex < 0)
		{
			return false;
		}
		auto ObjectItem = ObjectsStore().GetItemById(ObjectIndex);
		if (!ObjectItem.Object)
		{
			return false;
		}
		if (!SerialNumbersMatch(&ObjectItem))
		{
			return false;
		}
		return !(ObjectItem.IsUnreachable() || ObjectItem.IsPendingKill());
	}

	void* FWeakObjectPtr::Get() const
	{
		if (IsValid())
		{
			auto ObjectItem = ObjectsStore().GetItemById(ObjectIndex);
			if (ObjectItem.Object)
			{
				return ObjectItem.Object;
			}
		}
		return nullptr;
	}

}

