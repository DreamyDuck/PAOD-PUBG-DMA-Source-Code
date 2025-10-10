#pragma once
#include <Windows.h>
#include <stdint.h>
#include <assert.h>

#if defined(_MSC_VER)

#ifndef COMPILER_MSVC
#define COMPILER_MSVC 1
#endif

#elif defined(__GNUC__)
#define COMPILER_GCC
#else
#error "Unknown or unsupported compiler"
#endif


#define Randmod(x) rand()%x

// No IA64 support
#if defined (_M_AMD64) || defined (__x86_64__)
#define USE64
#elif defined (_M_IX86) || defined (__i386__)
#define USE32
#else
#error "Unknown or unsupported platform"
#endif


#ifdef _WIN32
#define ALIGNMENT __declspec(align(16))
#else
#define ALIGNMENT
#endif

#ifdef __clang__
template <typename T>
auto ArrayCountHelper(T& t) -> typename TEnableIf<__is_array(T), char(&)[sizeof(t) / sizeof(t[0]) + 1]>::Type;
#else
template <typename T, int32_t N>
char(&ArrayCountHelper(const T(&)[N]))[N + 1];

#endif

#define HIJACK_API __declspec(dllexport)


#define PI 3.1415926f

#define MM_ZERO_ACCESS         0
#define MM_READONLY            1
#define MM_EXECUTE             2
#define MM_EXECUTE_READ        3
#define MM_READWRITE           4
#define MM_WRITECOPY           5
#define MM_EXECUTE_READWRITE   6
#define MM_EXECUTE_WRITECOPY   7

//#define KEYDOWN(vk_code) (GetAsyncKeyState(vk_code)&1) 
//#define KEYUP(vk_code) (GetAsyncKeyState(vk_code)&1)

// Number of elements in an array.

#define CALL_64_86(b, f, ...) (b ? f<uint64_t>(__VA_ARGS__) : f<uint32_t>(__VA_ARGS__))

#define PID GetCurrentProcessId()
#define ARRAY_COUNT( array ) (sizeof(ArrayCountHelper(array)) - 1)


#define PROPERTY(t,n)  __declspec( property \
( put = Set##n##, get = Get##n## ) ) t n; \
	typedef t property__tmp_type_##n
#define READONLY_PROPERTY(t,n) __declspec( property (get = Get##n##) ) t n;\
	typedef t property__tmp_type_##n
#define WRITEONLY_PROPERTY(t,n) __declspec( property (put = Set##n##) ) t n;\
	typedef t property__tmp_type_##n

#define GETOFFSET(name) ObjectsStore().FindObject<UProperty,##name##_hash>().GetOffset()
#define GETUFUNCTION(name) ObjectsStore().FindObject<UFunction,##name##_hash>().GetPtr()

#define MAKE_STRUCT_PROPERTY(type,name,hash,offset) __declspec( property \
( get = Get##name## ) ) type name; \
StructProperty<type,##hash##_hash> private_StructProperty##name;\
type Get##name##() {return private_StructProperty##name##.Get(this,offset);}\
ptr_t SGet##name##() { return private_StructProperty##name##.ScatterGet(this,offset); }\
SIZE_T SGetSize##name##() { return private_StructProperty##name##.SGetSize(this); }

#define MAKE_OBJECT_PROPERTY(type,name,hash,offset) __declspec( property \
( get = Get##name## ) ) type name; \
ObjectProperty<type,##hash##_hash> private_ObjectProperty##name;\
type Get##name##() {return private_ObjectProperty##name##.Get(this,offset);}\
ptr_t SGet##name##() { return private_ObjectProperty##name##.ScatterGet(this,offset); }\
SIZE_T SGetSize##name##() { return private_ObjectProperty##name##.SGetSize(this); }

#define MAKE_GENERAL_PROPERTY(type,name,hash,offset) \
__declspec(property (put = Set##name##, get = Get##name##)) type name; \
GeneralProperty<type, ##hash##_hash> private_GeneralProperty##name; \
type* Get##name##Ptr() { return private_GeneralProperty##name##.GetPtr(this,offset); }\
type Get##name##() { return private_GeneralProperty##name##.Get(this,offset); }\
void Set##name##(type value) { return private_GeneralProperty##name##.Set(this, value,offset); }\
ptr_t SGet##name##() { return private_GeneralProperty##name##.ScatterGet(this,offset); }\
SIZE_T SGetSize##name##(){return private_GeneralProperty##name##.SGetSize(this);}

//UObjectProperty
#define GET(n) property__tmp_type_##n Get##n##()
#define GETCPP(n,c) c##::property__tmp_type_##n c##::Get##n##()
#define SET(n) void Set##n##(const property__tmp_type_##n& value)
#define SETCPP(n,c) void c##::Set##n##(const property__tmp_type_##n& value)

#define MAKE_PROPERTY_READONLY_INLINE(NAME,TYPE,OFFSET)  __declspec( property ( get = Get##NAME## ) ) TYPE NAME; \
__forceinline TYPE Get##NAME##() {auto ptr = this->GetPtr();if(ptr != nullptr){return *reinterpret_cast<TYPE*>((ptr_t)ptr + static_cast<ptr_t>(OFFSET)); } return TYPE();}

#define MAKE_PROPERTY_READONLY_INLINE_PTR(NAME,TYPE,OFFSET)  __declspec( property ( get = Get##NAME## ) ) TYPE* NAME; \
__forceinline TYPE* Get##NAME##() {auto ptr = this->GetPtr();if(ptr != nullptr){return reinterpret_cast<TYPE*>((ptr_t)ptr + static_cast<ptr_t>(OFFSET)); }return nullptr;}

#define MAKE_PROPERTY_READONLY_INLINE_GETPTR(NAME,TYPE,OFFSET)  __declspec( property ( get = Get##NAME## ) ) TYPE* NAME; \
__forceinline TYPE* Get##NAME##() {auto ptr = this->GetPtr();if(ptr != nullptr){return *reinterpret_cast<TYPE**>((ptr_t)ptr + static_cast<ptr_t>(OFFSET)); }return nullptr;}

// Function type macros.
#define VARARGS     __cdecl											/* Functions with variable arguments */
//#define CDECL	    __cdecl											/* Standard C function */
#define STDCALL		__stdcall										/* Standard calling convention */
#define FORCEINLINE __forceinline									/* Force code to be inline */
#define FORCENOINLINE __declspec(noinline)							/* Force code to NOT be inline */
#define FUNCTION_NO_RETURN_START __declspec(noreturn)				/* Indicate that the function never returns. */
#define FUNCTION_NON_NULL_RETURN_START _Ret_notnull_				/* Indicate that the function never returns nullptr. */

#define WordSize sizeof(void*)

// Offset of 'LastStatus' field in TEB
#define LAST_STATUS_OFS (0x598 + 0x197 * WordSize)

#define LOCALIZE(en,cn) (g_ServerResult.English? (en): (cn))
#define LOCALIZED(en,cn) (g_ServerResult.English? en:cn)

using NTSTATUS = long;

// Type-unsafe cast.
template<typename _Tgt, typename _Src>
inline _Tgt brutal_cast(const _Src& src)
{
	static_assert(sizeof(_Tgt) == sizeof(_Src), "Operand size mismatch");
	union _u { _Src s; _Tgt t; } u;
	u.s = src;
	return u.t;
}

// Align value
inline size_t Align(size_t val, size_t alignment)
{
	return (val % alignment == 0) ? val : (val / alignment + 1) * alignment;
}

/// <summary>
/// Get last NT status
/// </summary>
/// <returns></returns>
inline NTSTATUS LastNtStatus()
{
	return *(NTSTATUS*)((unsigned char*)NtCurrentTeb() + LAST_STATUS_OFS);
}


/// <summary>
/// Set last NT status
/// </summary>
/// <param name="status">The status.</param>
/// <returns></returns>
inline NTSTATUS SetLastNtStatus(NTSTATUS status)
{
	return *(NTSTATUS*)((unsigned char*)NtCurrentTeb() + LAST_STATUS_OFS) = status;
}

#define HASH(s) constHash<##s##_hash>()

typedef uint64_t hash_t;

constexpr hash_t prime = 0x100000001B3ull;
constexpr hash_t basis = 0xCBF29CE484333335ull;

static constexpr const hash_t hash_compile_time(char const* str, hash_t last_value = basis)
{
	return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}

static constexpr const hash_t operator "" _hash(char const* p, size_t)
{
	return hash_compile_time(p);
}

static constexpr const hash_t hash_compile_time(wchar_t const* str, hash_t last_value = basis)
{
	return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}

static constexpr const hash_t operator "" _hash(wchar_t const* p, size_t)
{
	return hash_compile_time(p);
}

inline const hash_t hash_(char const* str)
{
	hash_t ret{ basis };

	while (*str) {
		ret ^= *str;
		ret *= prime;
		str++;
	}

	return ret;
}

inline const hash_t hash_(std::string s)
{
	return hash_(s.c_str());
}

inline const hash_t hash_(wchar_t const* str)
{
	hash_t ret{ basis };

	while (*str) {
		ret ^= *str;
		ret *= prime;
		str++;
	}

	return ret;
}


inline const hash_t hash_(std::wstring s)
{
	return hash_(s.c_str());
}


template<hash_t hash>
hash_t constHash()
{
	return hash;
}

