#pragma once
#include "ObjectMacros.h"
#include "../UnrealNames.h"
#include "Core/Core.hpp"
namespace IronMan::Core::SDK
{
	enum UObjectType
	{
		EPlayer,
		EVehicle,
		EBoat,
		EItemGroup,
		EItem,
		EDeathBox,
		EAirdrop,
		EProjectile,
		EDroppedItem,
		EOther,
		EUnknown
	};
	void ClearObjectCache();
	bool DecryptDataByCache(ptr_t encryptData, ptr_t& outData);
	void CacheDecryptData(ptr_t endata, ptr_t dedata);

	template<typename T>
	using cleanup_t = std::remove_cv_t<std::remove_pointer_t<T>>;

	template<typename T, typename S>
	static constexpr bool is_string_ptr = (std::is_pointer_v<T> && std::is_same_v<cleanup_t<T>, S>);

	template<typename T>
	static constexpr bool is_number = (std::is_integral_v<T> || std::is_enum_v<T>);

	template<typename T>
	static constexpr bool is_void_ptr = (std::is_pointer_v<T> && std::is_void_v<cleanup_t<T>>);

	template<class TEnum>
	class TEnumAsByte
	{
	public:
		inline TEnumAsByte()
		{
		}

		inline TEnumAsByte(TEnum _value)
			: value(static_cast<uint8_t>(_value))
		{
		}

		explicit inline TEnumAsByte(int32_t _value)
			: value(static_cast<uint8_t>(_value))
		{
		}

		explicit inline TEnumAsByte(uint8_t _value)
			: value(_value)
		{
		}

		inline operator TEnum() const
		{
			return (TEnum)value;
		}

		inline TEnum GetValue() const
		{
			return (TEnum)value;
		}

	private:
		uint8_t value;
	};
	class FUObjectItem
	{
	public:
		FUObjectItem(void* object = nullptr) :Object(object)
		{

		}

		//__int32 unk1;// 7/8 added //0x0000
		//__int32 Flags; //0x004
		//void* Object;  //0x0008
		//__int32 ClusterIndex; //0x0010
		//__int32 unk2;      //0x0014
		//__int32 pad_0x18;  //0x0018
		//__int32 pad_0x1c;  //0x001c
		//__int32 pad_0x20;  //0x0020
		//__int32 SerialNumber; //0x0024
		//__int32 pad_0x28;  //0x0028
		//__int32 pad_0x30;  //0x0030
		void* Object; //0x0000
		__int32 Flags; //0x0008
		__int32 ClusterIndex; //0x000C
		__int32 SerialNumber; //0x0010
		__int32 pad_0x14;  //0x0014

		enum class ObjectFlags : int32_t
		{
			None = 0,
			//~ All the other bits are reserved, DO NOT ADD NEW FLAGS HERE!
			ReachableInCluster = 1 << 23, ///< External reference to object in cluster exists
			ClusterRoot = 1 << 24, ///< Root of a cluster
			Native = 1 << 25, ///< Native (UClass only).
			Async = 1 << 26, ///< Object exists only on a different thread than the game thread.
			AsyncLoading = 1 << 27, ///< Object is being asynchronously loaded.
			Unreachable = 1 << 28, ///< Object is not reachable on the object graph.
			PendingKill = 1 << 29, ///< Objects that are pending destruction (invalid for gameplay but valid objects)
			RootSet = 1 << 30, ///< Object will not be garbage collected, even if unreferenced.
			NoStrongReference = 1 << 31,
			GarbageCollectionKeepFlags = Native | Async | AsyncLoading,
			//~ Make sure this is up to date!
			AllFlags = ReachableInCluster | ClusterRoot | Native | Async | AsyncLoading | Unreachable | PendingKill | RootSet
		};

		inline bool IsUnreachable() const
		{
			return !!(Flags & static_cast<std::underlying_type_t<ObjectFlags>>(ObjectFlags::Unreachable));
		}
		inline bool IsPendingKill() const
		{
			return !!(Flags & static_cast<std::underlying_type_t<ObjectFlags>>(ObjectFlags::PendingKill));
		}
	};

	template<class T>
	struct TArray
	{
		friend struct FString;
		friend struct FString_CallArg;

	public:
		inline TArray(void* ptr)
			:TArray()
		{

			if (ptr > 0)
				GetDMA().Read((ptr_t)ptr, (ptr_t)this, sizeof(TArray<T>));
		};
		inline TArray()
		{
			Data = nullptr;
			Count = Max = 0;
		};

		inline uint32_t Num() const
		{
			return Count;
		};

		inline uint32_t Num()
		{
			return Count;
		};

		inline bool Empty() const
		{
			return Count == 0;
		};


		inline void* GetData() const { return Data; }
		inline void* GetData() { return Data; }

		inline T operator[](size_t i)
		{
			return GetDMA().Read<T>((ptr_t)Data + i * sizeof(T));
		};

		inline const T operator[](size_t i) const
		{

			return GetDMA().Read<T>((ptr_t)Data + i * sizeof(T));
		};

		inline auto operator->() { return this; }

		inline bool IsValidIndex(size_t i) const
		{
			return (uint32_t)i < Num();
		}

		inline bool IsValidIndex(size_t i)
		{
			return (uint32_t)i < Num();
		}

	public:
		T* Data;
		uint32_t Count;
		uint32_t Max;
	};

	struct FString : private TArray<wchar_t>
	{
		inline FString()
		{
		};

		FString(const wchar_t* other)
		{
			if (!other)
			{
				Max = Count = 0;
				Data = 0;
				return;
			}
			Max = Count = *other ? (decltype(Max))(std::wcslen(other) + 1) : 0;

			if (Count)
			{
				Data = const_cast<wchar_t*>(other);
			}
		};

		inline bool IsValid() const
		{
			return Data != nullptr;
		}

		inline const wchar_t* c_str() const
		{
			return (const wchar_t*)Data;
		}

		std::string ToString() const
		{
			if (!IsValid())
				return ("Unknown");
			wchar_t tempString[255] = { 0 };
			ZeroMemory(tempString, 255);
			if (Data)
			{
				GetDMA().Read((ptr_t)Data, (ptr_t)tempString, 255);
				return Utils::UnicodeToUTF8(tempString);
			}
			return ("Unknown");

			/*auto length = std::wcslen(Data);

			std::string str(length, '\0');

			std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);*/

			//return str;
		}

		std::string ToAnsi() const
		{
			if (!IsValid())
				return ("Unknown");
			wchar_t tempString[255] = { 0 };
			ZeroMemory(tempString, 255);
			if (Data)
			{
				GetDMA().Read((ptr_t)Data, (ptr_t)tempString, 255);
				return Utils::UnicodeToANSI(tempString);
			}
			return ("Unknown");
		}
	};

	struct FString_CallArg : private TArray<wchar_t>
	{
		inline FString_CallArg()
		{
		};

		FString_CallArg(const wchar_t* other)
		{
			if (!other)
			{
				Max = Count = 0;
				Data = 0;
				return;
			}

			Max = Count = *other ? (decltype(Max))(std::wcslen(other) + 1) : 0;
			if (Count)
			{
				std::wcscpy(String, other);
				Data = String;
			}
		};

		inline bool IsValid() const
		{
			return Data != nullptr;
		}

		inline const wchar_t* c_str() const
		{
			return (const wchar_t*)Data;
		}

		std::string ToString() const
		{
			wchar_t tempString[255];
			ZeroMemory(tempString, 255);
			if (Data)
			{
				GetDMA().Read((ptr_t)Data, (ptr_t)tempString, 255);
				return Utils::UnicodeToUTF8(tempString);
			}
			return "Unknown";

			/*auto length = std::wcslen(Data);

			std::string str(length, '\0');

			std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);*/

			//return str;
		}

		wchar_t String[255];
	};



	template<int size = 0x20>
	class FTextData
	{
	public:
		char pad_0x0000[size];  //0x0000
		wchar_t* Name;          //0x0028 
		uint32_t Length;         //0x0030 
		uint32_t unknown;
	};
	class FTextLocalize
	{
	public:
		char pad_0x0000[0x10];  //0x0000
		FTextData<0x10>* Data;
	};

	struct FText
	{
		union
		{
			FTextData<0x20>* Data20;
			FTextData<0x28>* Data28;
			FTextLocalize* Localize;
		};

		char UnknownData[0x10];
		std::wstring Get() const
		{
			if (Data20)
			{
				auto _Data20 = GetDMA().Read<FTextData<0x20>>((ptr_t)Data20);
				wchar_t tempString[255];
				ZeroMemory(tempString, 255);
				if (_Data20.Name)
				{
					GetDMA().Read((ptr_t)_Data20.Name, (ptr_t)tempString, _Data20.Length * sizeof(wchar_t));
					return tempString;
				}
			}
			return (L"unknown");
		}
		std::string Get2() const
		{
			if (Data28)
			{
				auto _Data28 = GetDMA().Read<FTextData<0x28>>((ptr_t)Data28);
				if (_Data28.Name)
				{
					wchar_t tempString[255];
					ZeroMemory(tempString, 255);
					GetDMA().Read((ptr_t)_Data28.Name, (ptr_t)tempString, _Data28.Length * sizeof(wchar_t));
					return Utils::UnicodeToANSI(tempString);
				}
			}
			return ("unknown");
		}
		std::string ToLocalizeString() const
		{
			if (Localize)
			{
				auto lacalize = GetDMA().Read<FTextLocalize>((ptr_t)Localize);
				if (lacalize.Data)
				{
					auto textData = GetDMA().Read<std::remove_pointer_t<decltype(lacalize.Data)>>((ptr_t)lacalize.Data);
					if (textData.Name)
					{
						wchar_t tempString[255];
						ZeroMemory(tempString, 255);
						GetDMA().Read((ptr_t)textData.Name, (ptr_t)tempString, textData.Length * sizeof(wchar_t));
						return Utils::UnicodeToUTF8(tempString);
					}
				}
			}
			return ("unknown");
		}
		std::string ToString() const
		{
			if (Data20)
			{
				struct strdata
				{
					union
					{
						FTextData<0x20> Data20;
						FTextData<0x28> Data28;
					};
				};
				auto data = GetDMA().Read<strdata>((ptr_t)Data20);
				wchar_t* name = data.Data20.Name;
				auto length = data.Data20.Length;
				auto unk = data.Data20.unknown;
				if (length <= 0 || length > 255 || unk <= 0 || unk >= 255)
				{
					name = data.Data28.Name;
					length = data.Data28.Length;
				}

				if (name && length <= 255)
				{
					std::wstring tempString(length, 0);
					ZeroMemory(tempString.data(), 255);
					GetDMA().Read((ptr_t)name, (ptr_t)tempString.data(), length * sizeof(wchar_t));
					return Utils::UnicodeToANSI(tempString);
				}
			}
			return ("unknown");
		}

		std::wstring ToWString() const
		{
			if (Data20)
			{
				struct strdata
				{
					union
					{
						FTextData<0x20> Data20;
						FTextData<0x28> Data28;
					};
				};
				auto data = GetDMA().Read<strdata>((ptr_t)Data20);
				wchar_t* name = data.Data20.Name;
				auto length = data.Data20.Length;
				auto unk = data.Data20.unknown;
				if (length <= 0 || length > 255 || unk <= 0 || unk >= 255)
				{
					name = data.Data28.Name;
					length = data.Data28.Length;
				}

				if (name && length <= 255)
				{
					wchar_t tempString[255];
					ZeroMemory(tempString, 255);
					GetDMA().Read((ptr_t)name, (ptr_t)tempString, length * sizeof(wchar_t));
					return tempString;
				}
			}
			return (L"unknown");
		}

	};
	static_assert(sizeof(FText) == 0x18, "Size check");

	class UClass;
	class UObject
	{
	public:
		UObject();
		UObject(const void*& object);
		UObject(void* object);
		virtual ~UObject();

	public:
		inline bool IsValid() const { return !Utils::IsSafeReadPtr((void*)this, 8) && !bInvalid && IsAddrValid(m_self); }
		void Release() { bInvalid = true; }
		EObjectFlags GetObjectFlags();
		size_t GetIndex();
		UClass GetClass();
		UObject GetOuter();
		std::string GetName(bool number = false, bool usecache = true);
		int TestName();
		std::string GetSuperClassName();
		std::string GetFullName();
		template<typename Base>
		Base Cast() const
		{
			if (Utils::IsSafeReadPtr((void*)this, 8))
				return Base(nullptr);
			return Base(this->GetPtr());
		}

		template<typename Base>
		Base SafeCast()
		{
			if (this && IsA<Base>())
			{
				return Base(this->GetPtr());
			}
			return Base(nullptr);
		}

		template<typename Base>
		Base* CCast()
		{
			return  dynamic_cast<Base*>(this);
		}

	private:
		template <typename ClassType>
		static FORCEINLINE bool IsAWorkaround(const ClassType ObjClass, const ClassType TestCls)
		{
			return ObjClass.IsAUsingClassArray(TestCls);
		}

	public:


		template <typename OtherClassType>
		FORCEINLINE bool IsA(OtherClassType SomeBase)
		{
			// We have a cyclic dependency between UObjectBaseUtility and UClass,
			// so we use a template to allow inlining of something we haven't yet seen, because it delays compilation until the function is called.

			// 'static_assert' that this thing is actually a UClass pointer or convertible to it.
			const UClass SomeBaseClass = SomeBase;
			const UClass ThisClass = GetClass();

			return IsAWorkaround(ThisClass, SomeBaseClass);
		}

		template<class T>
		bool IsA()
		{
			if ((ptr_t)this < 0x10000 || Utils::IsSafeReadPtr(this, 1) || !IsValid())
				return false;

			auto cmp = T::StaticClass();
			if (!cmp.IsValid())
			{
				return false;
			}

			return IsA(cmp);
		}

		template<typename T>
		bool Is_A()
		{
			if ((ptr_t)this < 0x10000 || Utils::IsSafeReadPtr(this, 1) || !IsValid())
				return false;

			auto cmp = T::StaticClass();
			if (!cmp.IsValid())
			{
				return false;
			}
			for (auto super = GetClass(); super.IsValid(); super = super.GetSuperStruct().Cast<UClass>())
			{
				if (super.GetPtr() == cmp.GetPtr())
				{
					return true;
				}
			}

			return false;
		}

		bool IsA(UObjectType Type);
		bool SetIsA(UObjectType Type);

		void SetObjectType(UObjectType Type)
		{
			m_type = Type;
			bCached = true;
		}

		void SetAddObject(bool boolean)
		{
			bAddObject = boolean;
		}

		bool GetAddObject()
		{
			return bAddObject;
		}

		__forceinline UObjectType GetObjectType() { return m_type; }

		void ProcessEvent(class UFunction function, void* parms);
		auto operator->() { return this; }
		operator bool() { return IsValid(); }

		static UClass StaticClass();
	private:
		EObjectFlags					ObjectFlags;
		int32_t							InternalIndex;
		void* ClassPrivate;
		void* EncryptClass;
		FName							NamePrivate;
		void* OuterPrivate;
	public:
		inline void* GetPtr() const
		{
			if (!Utils::IsSafeReadPtr((void*)this, 8))
				return m_self;
			return nullptr;
		}
		void SetPtr(void* p)
		{
			if (Utils::IsSafeReadPtr(this, 8))
				return;
			if (this && this->GetPtr() != p)
			{
				ObjectFlags = (RF_INVALID);
				InternalIndex = (-1);
				ClassPrivate = (INVALID_POINTER);;
				EncryptClass = (INVALID_POINTER);;
				NamePrivate = FName();
				OuterPrivate = (INVALID_POINTER);
				m_self = p;
				bUseCache = true;
				bAddObject = false;
			}
		}

		__forceinline bool IsCached() const { return bCached; }
		void SetCached(bool value)
		{
			if (!Utils::IsSafeReadPtr(this, 8))
			{
				bUseCache = value;
			}
		}
		/*virtual void operator=(const UObject& obj)
		{
			if (this->GetPtr() != obj.GetPtr())
			{
				this->SetPtr(obj.GetPtr());
			}
		};*/
		//FORCEINLINE std::shared_ptr<unique_writeguard<WfirstRWLock>> getWriteLock() const {
		//	return std::make_shared<unique_writeguard<WfirstRWLock>>(*WriteLock);
		//}
		//FORCEINLINE std::shared_ptr<unique_readguard<WfirstRWLock>> getReadLock() const {
		//	return std::make_shared<unique_readguard<WfirstRWLock>>(*WriteLock);
		//}
	private:
		void* m_self;
		UObjectType m_type;
		std::string m_name;
		std::string m_fullName;
		std::string m_upperName;
		bool bCached;
		bool bAddObject;
		bool bUseCache;
		bool bInvalid;
		//std::shared_ptr<WfirstRWLock> WriteLock;
	};


	struct FWeakObjectPtr
	{
	public:
		inline bool SerialNumbersMatch(FUObjectItem* ObjectItem) const
		{
			return ObjectItem->SerialNumber == ObjectSerialNumber;
		}

		bool IsValid() const;

		void* Get() const;

		int32_t ObjectIndex = 0;
		int32_t ObjectSerialNumber = 0;
	};


	template<class T, class TWeakObjectPtrBase = FWeakObjectPtr>
	struct TWeakObjectPtr : public TWeakObjectPtrBase
	{
	public:
		inline T Get() const
		{
			return static_cast<T>(TWeakObjectPtrBase::Get());
		}

		inline T& operator*() const
		{
			return Get();
		}

		inline T operator->() const
		{
			return Get();
		}

		inline bool IsValid() const
		{
			return TWeakObjectPtrBase::IsValid();
		}
	};


}
