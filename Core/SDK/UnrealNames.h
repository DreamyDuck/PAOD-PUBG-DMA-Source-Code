#pragma once
#include "Core/PatternLoader.h"
#include "Core/DMA/DMAHandler.h"

#include "UEncrypt.h"
#include <set>
#pragma warning(disable: 4996)
extern bool useCache;
namespace IronMan::Core::SDK
{
	struct StringA {
		char buffer[128];
	};
	extern std::vector<StringA> ScatterNameArray;

	class FNameEntry
	{
		static constexpr auto NAME_WIDE_MASK = 0x1;
		static constexpr auto NAME_INDEX_SHIFT = 1;
	public:

		inline const int32_t GetIndex() const
		{
			return Index >> NAME_INDEX_SHIFT;
		}

		inline bool IsWide() const
		{
			return Index & NAME_WIDE_MASK;
		}

		inline const char* GetAnsiName() const
		{
			return AnsiName;
		}

		inline const wchar_t* GetWideName() const
		{
			return WideName;
		}

	private:
		__int32 Index;
		char pad_0x0004[0x4];
		FNameEntry* HashNext;
		union
		{
			char AnsiName[128];
			wchar_t* WideName;
		};

	};

	template<typename ElementType, int32_t MaxTotalElements>
	class TStaticIndirectArrayThreadSafeRead
	{
	public:
		void SetCacheRead(bool state)
		{
			useCache = state;
		}

		int32_t Num() const
		{
			//CONSOLE_INFO("%p", GlobalNames.Get());
			auto encryptNum = GetDMA().Read<uint64_t>((ptr_t)GlobalNames.Get() + 8);
			//CONSOLE_INFO("Num:%p", UEEncryptedObjectProperty<int32_t, DecryptFunc::Names>::STATIC_Get(encryptNum.result()));
			return UEEncryptedObjectProperty<int32_t, DecryptFunc::General>::STATIC_Get(encryptNum);
			//return NumElements.Get();
		}

		int32_t Max() const
		{
			auto encryptNum = GetDMA().Read<int32_t>((ptr_t)GlobalNames.Get() + 0xC);
			return UEEncryptedObjectProperty<int32_t, DecryptFunc::General>::STATIC_Get(encryptNum);
			//return NumElements.Get();
		}


		bool IsValidIndex(int32_t index) const
		{
			return index >= 0 && index < Num() && GetPtrById(index) != nullptr;
		}

		ElementType* GetPtrById(int32_t index) const
		{
			return useCache ? GetDMA().ReadCache<ElementType*>((ptr_t)GetItemPtr(index)) : GetDMA().Read<ElementType*>((ptr_t)GetItemPtr(index));
		}

		ElementType GetById(int32_t index) const
		{
			return useCache ? GetDMA().ReadCache<ElementType>((ptr_t)GetPtrById(index)) : GetDMA().Read<ElementType>((ptr_t)GetPtrById(index));
		}

	private:
		ElementType** GetItemPtr(int32_t Index) const
		{
			int32_t ChunkIndex = Index / (int32_t)g_PatternData.ElementsPerChunk;
			int32_t WithinChunkIndex = Index % (int32_t)g_PatternData.ElementsPerChunk;
			UEEncryptedObjectProperty<ElementType***, DecryptFunc::General> TempChunks;
			if (useCache)
				GetDMA().ReadCache((ptr_t)&this->Chunks, (ptr_t)&TempChunks, 8);
			else
				GetDMA().Read((ptr_t)&this->Chunks, (ptr_t)&TempChunks, 8);
			if (!IsAddrValid(TempChunks.Get()))
			{
				if (useCache)
					GetDMA().ReadCache((ptr_t)&this->Unknown, (ptr_t)&TempChunks, 8);
				else
					GetDMA().Read((ptr_t)&this->Unknown, (ptr_t)&TempChunks, 8);
				if (!IsAddrValid(TempChunks.Get()))
				{
					if (useCache)
						GetDMA().ReadCache((ptr_t)&this->Unknown2, (ptr_t)&TempChunks, 8);
					else
						GetDMA().Read((ptr_t)&this->Unknown2, (ptr_t)&TempChunks, 8);
				}
			}
			ElementType** Chunk = useCache ? (ElementType**)GetDMA().ReadCache<ptr_t>((ptr_t)TempChunks.Get() + ChunkIndex * 8)
				: (ElementType**)GetDMA().Read<ptr_t>((ptr_t)TempChunks.Get() + ChunkIndex * 8);
			return Chunk + WithinChunkIndex;
		}
		/*enum
		{
			ChunkTableSize = (MaxTotalElements + ElementsPerChunk - 1) / ElementsPerChunk
		};*/
		UEEncryptedObjectProperty<ElementType***, DecryptFunc::General> Unknown;
		UEEncryptedObjectProperty<ElementType***, DecryptFunc::General> Chunks;
		UEEncryptedObjectProperty<ElementType***, DecryptFunc::General> Unknown2;
		UEEncryptedObjectProperty<uint32_t, DecryptFunc::General> NumElements;
		uint64_t EnNumChunks;
	};

	using TNameEntryArray = TStaticIndirectArrayThreadSafeRead<FNameEntry, 2 * 1024 * 1024>;

	extern UEEncryptedObjectProperty<TNameEntryArray*, DecryptFunc::General> GlobalNames;


	struct FName
	{
		union
		{
			struct
			{
				int32_t ComparisonIndex;
				int32_t Number;
			};

			uint64_t CompositeComparisonValue;
		};

		inline FName()
			: ComparisonIndex(0),
			Number(0)
		{
		};

		inline FName(int32_t i)
			: ComparisonIndex(i),
			Number(0)
		{
		};

		FName(const char* nameToFind)
			: ComparisonIndex(0),
			Number(0)
		{
			static std::set<int> cache;

			for (auto i : cache)
			{
				if (!std::strcmp(GlobalNames->GetById(i).GetAnsiName(), nameToFind))
				{
					ComparisonIndex = i;

					return;
				}
			}

			for (auto i = 0; i < GlobalNames->Num(); ++i)
			{
				if (GlobalNames->GetPtrById(i) != nullptr)
				{
					if (!std::strcmp(GlobalNames->GetById(i).GetAnsiName(), nameToFind))
					{
						cache.insert(i);

						ComparisonIndex = i;

						return;
					}
				}
			}
		};

		FORCEINLINE std::string GetName() const
		{
			if (ComparisonIndex >= 0)
			{
				return GlobalNames->GetById(ComparisonIndex).GetAnsiName();
			}
			return "Unknown";
		};

		FORCEINLINE std::string GetName2(bool Cache = false) const
		{
			if (ComparisonIndex >= 0 && ComparisonIndex < ScatterNameArray.size())
			{
				Utils::hasNullTerminator(ScatterNameArray[ComparisonIndex].buffer, 128);
				auto CacheString = std::string(ScatterNameArray[ComparisonIndex].buffer);
				if (CacheString != "")
				{
					return std::string(ScatterNameArray[ComparisonIndex].buffer);
				}
				else
				{
					auto validate_and_clear2 = [=](struct StringA* strA) {
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
					GlobalNames->SetCacheRead(Cache);
					auto ResultName = GetName();
					Utils::hasNullTerminator(ResultName.data(), 128);
					if (ResultName != "" && ResultName.size() >= 3)
					{
						memcpy(ScatterNameArray[ComparisonIndex].buffer, ResultName.data(), ResultName.size());
						validate_and_clear2(&ScatterNameArray[ComparisonIndex]);
					}
					else
						return "Unknown";
					return std::string(ResultName);
				}
			}
			return "Unknown";
		};

		FORCEINLINE bool operator==(const FName& other) const
		{
			return ComparisonIndex == other.ComparisonIndex;
		};

		FORCEINLINE bool operator!=(const FName& Other) const
		{
			return !(*this == Other);
		}

	};

	static FName NAME_None = 0;



	class NamesIterator;
	class NamesStore
	{
		friend NamesIterator;
	public:
		/// <summary>
		/// Initializes this object.
		/// </summary>
		static bool Initialize();


		static void GetNameByID(VMMDLL_SCATTER_HANDLE Handle, int num, ptr_t GName, void* ret);
		static void GetNameByID1(VMMDLL_SCATTER_HANDLE Handle, int num, ptr_t GName, void* ret);
		static void GetNameByID2(VMMDLL_SCATTER_HANDLE Handle, int num, ptr_t GName, void* ret);
		static void GetNameByID3(VMMDLL_SCATTER_HANDLE Handle, int num, ptr_t GName, void* ret);

		/// <summary>Gets the address of the global names store.</summary>
		/// <returns>The address of the global names store.</returns>
		static void* GetAddress();

		NamesIterator begin();

		NamesIterator begin() const;

		NamesIterator end();

		NamesIterator end() const;

		/// <summary>
		/// Gets the number of available names.
		/// </summary>
		/// <returns>The number of names.</returns>
		size_t GetNamesNum() const;

		/// <summary>
		/// Test if the given id is valid.
		/// </summary>
		/// <param name="id">The identifier.</param>
		/// <returns>true if valid, false if not.</returns>
		bool IsValid(size_t id) const;

		/// <summary>
		/// Gets a name by id.
		/// </summary>
		/// <param name="id">The identifier.</param>
		/// <returns>The name.</returns>
		std::string GetById(size_t id) const;
	};

	struct UENameInfo
	{
		size_t Index;
		std::string Name;
	};

	class NamesIterator : public std::iterator<std::forward_iterator_tag, UENameInfo>
	{
		const NamesStore& store;
		size_t index;

	public:
		NamesIterator(const NamesStore& store);

		explicit NamesIterator(const NamesStore& store, size_t index);

		void swap(NamesIterator& other) noexcept;

		NamesIterator& operator++();

		NamesIterator operator++ (int);

		bool operator==(const NamesIterator& rhs) const;

		bool operator!=(const NamesIterator& rhs) const;

		UENameInfo operator*() const;

		UENameInfo operator->() const;
	};


}