#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

//#define ENABLE_CONSOLE
#define CPPHTTPLIB_OPENSSL_SUPPORT
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "Types.hpp"
#include "Macro.hpp"
#include "VMProtectSDK.h"
#include "delegate.hpp"
#include "Helper/Misc/Console.hpp"
#include "Helper/Misc/Utils.hpp"
#include "Helper/Misc/ThreadMutex.h"
#include "allocMem.h"
#include "Core/PatternLoader.h"
#include "ApiSet.h"
#include <windows.h>
#include <winternl.h>
#include <winioctl.h>
#include <TlHelp32.h>
#include <Shlwapi.h>
#include <sstream>
#include <fstream>
#include <iomanip> 
#include <algorithm>
#include <intrin.h>
#include <mutex>  
#include <condition_variable>  
#include "../Core/MouseController/KMBOX/KmboxB.h"
#include "../Core/MouseController/KMBOX/KmboxNet.h"
//#include "Core/_asm.h"
#include <PxPhysicsAPI.h>
#include "Helper/HttpLib/httplib.h"
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "wininet.lib")
#pragma warning(push)
#pragma warning(disable : 4005)
#pragma warning(disable : 4305)
#pragma warning(disable : 4267)
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
#pragma warning(disable : 4312)
#include <ntstatus.h>
#pragma warning(pop)
#define NoWriteMem
class WfirstRWLock
{
public:
	WfirstRWLock() = default;
	~WfirstRWLock() = default;
public:
	void lock_read()
	{
		std::unique_lock<std::mutex> ulk(counter_mutex);
		cond_r.wait(ulk, [=]()->bool {return write_cnt == 0; });
		++read_cnt;
	}
	void lock_write()
	{
		std::unique_lock<std::mutex> ulk(counter_mutex);
		++write_cnt;
		cond_w.wait(ulk, [=]()->bool {return read_cnt == 0 && !inwriteflag; });
		inwriteflag = true;
	}
	void release_read()
	{
		std::unique_lock<std::mutex> ulk(counter_mutex);
		if (--read_cnt == 0 && write_cnt > 0)
		{
			cond_w.notify_one();
		}
	}
	void release_write()
	{
		std::unique_lock<std::mutex> ulk(counter_mutex);
		if (--write_cnt == 0)
		{
			cond_r.notify_all();
		}
		else
		{
			cond_w.notify_one();
		}
		inwriteflag = false;
	}

private:
	volatile size_t read_cnt{ 0 };
	volatile size_t write_cnt{ 0 };
	volatile bool inwriteflag{ false };
	std::mutex counter_mutex;
	std::condition_variable cond_w;
	std::condition_variable cond_r;
};

template <typename _RWLockable>
class unique_writeguard
{
public:
	explicit unique_writeguard(_RWLockable& rw_lockable)
		: rw_lockable_(rw_lockable)
	{
		rw_lockable_.lock_write();
	}
	~unique_writeguard()
	{
		rw_lockable_.release_write();
	}
private:
	unique_writeguard() = delete;
	unique_writeguard(const unique_writeguard&) = delete;
	unique_writeguard& operator=(const unique_writeguard&) = delete;
private:
	_RWLockable& rw_lockable_;
};
template <typename _RWLockable>
class unique_readguard
{
public:
	explicit unique_readguard(_RWLockable& rw_lockable)
		: rw_lockable_(rw_lockable)
	{
		rw_lockable_.lock_read();
	}
	~unique_readguard()
	{
		rw_lockable_.release_read();
	}
private:
	unique_readguard() = delete;
	unique_readguard(const unique_readguard&) = delete;
	unique_readguard& operator=(const unique_readguard&) = delete;
private:
	_RWLockable& rw_lockable_;
};
#define IsAddrValid(ptr) ((IronMan::ptr_t)ptr >= 0x100000 && (IronMan::ptr_t)ptr < IronMan::Core::g_PatternData.ModuleHandle)
#ifdef __clang__
#define NO_SANITIZE_ADDRESS __attribute__((no_sanitize("address")))
#elif defined(__GNUC__)
#define NO_SANITIZE_ADDRESS __attribute__((no_sanitize_address))
#else
#define NO_SANITIZE_ADDRESS
#endif