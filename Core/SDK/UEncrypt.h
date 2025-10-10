#pragma once

#include "Helper/Asm/AsmFactory.hpp"
#include "Helper/Analysis/DecryptAnalysis.h"
#include "Helper/Analysis/UObjectAnalysis.h"
namespace IronMan::Core::SDK
{
	bool DecryptDataByCache(ptr_t encryptData, ptr_t& outData);
	void CacheDecryptData(ptr_t endata, ptr_t dedata);

	enum class DecryptFunc
	{
		UObjectFlags,
		UObjectNameComparisonIndex,
		UObjectNameNumber,
		UObjectIndex,
		UObjectClass,
		UObjectOuter,
		General
	};

	enum class BuildType
	{
		Default,
		UObject,
		Health
	};

	class UEDecryptBuilder
	{
	public:
		static bool Initialize();
		static bool SetExchange();
		inline uint8_t* GetFunction() { return functionBuf; }

	public:
		UEDecryptBuilder(ptr_t StartPosition, int DecryptIndex, BuildType type = BuildType::Default, std::function< void(void*)> callback = nullptr);
		~UEDecryptBuilder();
	private:
		uint8_t* functionBuf = nullptr;
		static uint8_t* GlobalFunctionBuffer;
		static ptr_t BufferUsedSize;
	};

	extern std::unordered_map<DecryptFunc, std::unique_ptr<UEDecryptBuilder>> g_DecryptFuncs;


	template<typename ElementType, DecryptFunc Func>
	struct UEEncryptedObjectProperty
	{
		typedef uint64_t(__fastcall* DECRYPT_FUNC)(uint64_t);
	public:
		explicit UEEncryptedObjectProperty(uint64_t encrypt = NULL) :encryptedData(encrypt) {}
	public:
		static ElementType STATIC_Get(uint64_t enData)
		{
			DECRYPT_FUNC func = reinterpret_cast<DECRYPT_FUNC>(g_DecryptFuncs[Func]->GetFunction());
			if (!func || !enData || Utils::IsSafeReadPtr(func, 1))
				return (ElementType)(uint64_t)0;
			return (ElementType)func(enData);
		}
		static ptr_t STATIC_GetDecFunc()
		{
			return reinterpret_cast<ptr_t>(g_DecryptFuncs[Func]->GetFunction());
		}
		ElementType Get() const
		{
			return STATIC_Get(encryptedData);
		}
		ElementType operator()(void) const { return Get(); }
		ElementType operator->() const { return Get(); }
		auto operator[](size_t idx) const { return Get()[idx]; }
	private:
		uint64_t encryptedData;
	};

	//template<typename ElementType, DecryptFunc Func>
	//struct UEEncryptedObjectProperty
	//{
	//	template<typename T>
	//	using cleanup_t = std::remove_cv_t<std::remove_pointer_t<T>>;

	//	template<typename T, typename S>
	//	static constexpr bool is_string_ptr = (std::is_pointer_v<T> && std::is_same_v<cleanup_t<T>, S>);

	//	template<typename T>
	//	static constexpr bool is_number = (std::is_integral_v<T> || std::is_enum_v<T>);

	//	template<typename T>
	//	static constexpr bool is_void_ptr = (std::is_pointer_v<T> && std::is_void_v<cleanup_t<T>>);

	//	using RAW_T = std::decay_t<ElementType>;

	//	typedef uint64_t(__fastcall* DECRYPT_FUNC)(uint64_t);
	//public:
	//	explicit UEEncryptedObjectProperty(uint64_t encrypt = NULL) :encryptedData(encrypt)
	//	{

	//	}
	//public:
	//	static ElementType STATIC_Get(uint64_t enData, size_t idx = 0)
	//	{
	//		static DECRYPT_FUNC func = nullptr;

	//		if (func == nullptr)
	//			func = reinterpret_cast<DECRYPT_FUNC>(g_DecryptFuncs[Func]->GetFunction());

	//		constexpr size_t argSize = sizeof(RAW_T);

	//		ptr_t decryptData;
	//		if (!DecryptDataByCache(enData, decryptData))
	//		{
	//			decryptData = func(enData);
	//			CacheDecryptData(enData, decryptData);
	//		}

	//		if constexpr (std::is_pointer_v<RAW_T> && !is_void_ptr<RAW_T>)
	//		{
	//			static auto res = new cleanup_t<RAW_T>();
	//			Game.memory().Read(decryptData + idx * sizeof(cleanup_t<RAW_T>), sizeof(cleanup_t<RAW_T>), res);
	//			return res;
	//			//set(dataPtr, sizeof(cleanup_t<RAW_T>), reinterpret_cast<uint64_t>(arg));
	//		}
	//		else
	//		{
	//			return (ElementType)decryptData;
	//		}
	//	}
	//	ElementType Get(size_t idx = 0) const
	//	{
	//		return STATIC_Get(encryptedData, idx);
	//	}
	//	ElementType operator()(void) const { return Get(); }
	//	ElementType operator->() const { return Get(); }
	//	auto operator[](size_t idx) const { return Get(idx)[0]; }
	//private:
	//	uint64_t encryptedData;
	//};


	/*template<typename ElementType, DecryptFunc Func>
	struct UEEncryptedObjectProperty;*/

}