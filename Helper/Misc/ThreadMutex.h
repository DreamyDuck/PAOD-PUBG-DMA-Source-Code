#pragma once

#ifndef _RETAIL
#define THREAD_MUTEX_TRACING_SUPPORTED
#if defined(_WIN32) && defined(_DEBUG)
#define THREAD_MUTEX_TRACING_ENABLED
#endif
#endif

#if defined(_WIN32)
#if ( _MSC_VER >= 1310 )
#define USE_INTRINSIC_INTERLOCKED
#endif
#endif

#ifdef USE_INTRINSIC_INTERLOCKED
extern "C"
{
	long __cdecl _InterlockedIncrement(volatile long*);
	long __cdecl _InterlockedDecrement(volatile long*);
	long __cdecl _InterlockedExchange(volatile long*, long);
	long __cdecl _InterlockedExchangeAdd(volatile long*, long);
	long __cdecl _InterlockedCompareExchange(volatile long*, long, long);
}

#pragma intrinsic( _InterlockedCompareExchange )
#pragma intrinsic( _InterlockedDecrement )
#pragma intrinsic( _InterlockedExchange )
#pragma intrinsic( _InterlockedExchangeAdd ) 
#pragma intrinsic( _InterlockedIncrement )

inline long ThreadInterlockedIncrement(long volatile *p) { assert((size_t)p % 4 == 0); return _InterlockedIncrement(p); }
inline long ThreadInterlockedDecrement(long volatile *p) { assert((size_t)p % 4 == 0); return _InterlockedDecrement(p); }
inline long ThreadInterlockedExchange(long volatile *p, long value) { assert((size_t)p % 4 == 0); return _InterlockedExchange(p, value); }
inline long ThreadInterlockedExchangeAdd(long volatile *p, long value) { assert((size_t)p % 4 == 0); return _InterlockedExchangeAdd(p, value); }
inline long ThreadInterlockedCompareExchange(long volatile *p, long value, long comperand) { assert((size_t)p % 4 == 0); return _InterlockedCompareExchange(p, value, comperand); }
inline bool ThreadInterlockedAssignIf(long volatile *p, long value, long comperand) { assert((size_t)p % 4 == 0); return (_InterlockedCompareExchange(p, value, comperand) == comperand); }
#else
TT_INTERFACE long ThreadInterlockedIncrement(long volatile *) NOINLINE;
TT_INTERFACE long ThreadInterlockedDecrement(long volatile *) NOINLINE;
TT_INTERFACE long ThreadInterlockedExchange(long volatile *, long value) NOINLINE;
TT_INTERFACE long ThreadInterlockedExchangeAdd(long volatile *, long value) NOINLINE;
TT_INTERFACE long ThreadInterlockedCompareExchange(long volatile *, long value, long comperand) NOINLINE;
TT_INTERFACE bool ThreadInterlockedAssignIf(long volatile *, long value, long comperand) NOINLINE;
#endif

const unsigned TT_INFINITE = 0xffffffff;
namespace IronMan
{
	namespace Thread
	{
		class CThreadMutex
		{
		public:
			CThreadMutex();
			~CThreadMutex();

			//------------------------------------------------------
			// Mutex acquisition/release. Const intentionally defeated.
			//------------------------------------------------------
			void Lock();
			inline void Lock() const { (const_cast<CThreadMutex *>(this))->Lock(); }
			void Unlock();
			inline void Unlock() const { (const_cast<CThreadMutex *>(this))->Unlock(); }

			bool TryLock();
			inline bool TryLock() const { return (const_cast<CThreadMutex *>(this))->TryLock(); }

			//------------------------------------------------------
			// Use this to make deadlocks easier to track by asserting
			// when it is expected that the current thread owns the mutex
			//------------------------------------------------------
			bool AssertOwnedByCurrentThread();

			//------------------------------------------------------
			// Enable tracing to track deadlock problems
			//------------------------------------------------------
			void SetTrace(bool);

		private:
			// Disallow copying
			CThreadMutex(const CThreadMutex &);
			CThreadMutex &operator=(const CThreadMutex &);
		private:
#if defined( _WIN32 )
			// Efficient solution to breaking the windows.h dependency, invariant is tested.
#ifdef _WIN64
#define TT_SIZEOF_CRITICALSECTION 40	
#else
#ifndef _X360
#define TT_SIZEOF_CRITICALSECTION 24
#else
#define TT_SIZEOF_CRITICALSECTION 28
#endif // !_XBOX
#endif // _WIN64
			uint8_t m_CriticalSection[TT_SIZEOF_CRITICALSECTION];
#elif _LINUX
			pthread_mutex_t m_Mutex;
			pthread_mutexattr_t m_Attr;
#else
#error
#endif

#ifdef THREAD_MUTEX_TRACING_SUPPORTED
			// Debugging (always here to allow mixed debug/release builds w/o changing size)
			uint32_t	m_currentOwnerID;
			uint16_t	m_lockCount;
			bool	m_bTrace;
#endif

		};

		class CThreadFastMutex
		{
		public:
			CThreadFastMutex()
				: m_ownerID(0),
				m_depth(0)
			{
			}

		private:
			FORCEINLINE bool TryLockInline(const uint32_t threadId) volatile
			{
				if ((long)threadId != m_ownerID && !ThreadInterlockedAssignIf((volatile long *)&m_ownerID, (long)threadId, 0))
					return false;

				++m_depth;
				return true;
			}

			bool TryLock(const uint32_t threadId) volatile
			{
				return TryLockInline(threadId);
			}

			void Lock(const uint32_t threadId, unsigned nSpinSleepTime) volatile;

		public:
			bool TryLock() volatile
			{
#ifdef _DEBUG
				if (m_depth == INT_MAX)
					__debugbreak();

				if (m_depth < 0)
					__debugbreak();
#endif
				return TryLockInline(GetCurrentThreadId());
			}

#ifndef _DEBUG
			FORCEINLINE
#endif
				void Lock(unsigned nSpinSleepTime = 0) volatile
			{
				const uint32_t threadId = GetCurrentThreadId();

				if (!TryLockInline(threadId))
				{
					_mm_pause();
					Lock(threadId, nSpinSleepTime);
				}
#ifdef _DEBUG
				if (m_ownerID != GetCurrentThreadId())
					__debugbreak();

				if (m_depth == INT_MAX)
					__debugbreak();

				if (m_depth < 0)
					__debugbreak();
#endif
			}

#ifndef _DEBUG
			FORCEINLINE
#endif
				void Unlock() volatile
			{
#ifdef _DEBUG
				if (m_ownerID != GetCurrentThreadId())
					__debugbreak();

				if (m_depth <= 0)
					__debugbreak();
#endif

				--m_depth;
				if (!m_depth)
					ThreadInterlockedExchange(&m_ownerID, 0);
			}

			bool TryLock() const volatile { return (const_cast<CThreadFastMutex *>(this))->TryLock(); }
			void Lock(unsigned nSpinSleepTime = 1) const volatile { (const_cast<CThreadFastMutex *>(this))->Lock(nSpinSleepTime); }
			void Unlock() const	volatile { (const_cast<CThreadFastMutex *>(this))->Unlock(); }

			// To match regular CThreadMutex:
			bool AssertOwnedByCurrentThread() { return true; }
			void SetTrace(bool) {}

			uint32_t GetOwnerId() const { return m_ownerID; }
			int	GetDepth() const { return m_depth; }
		private:
			volatile long	m_ownerID;
			int				m_depth;
		};
	}

}