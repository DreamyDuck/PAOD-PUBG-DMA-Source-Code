#include "Include/WinHeaders.hpp"
#include "ThreadMutex.h"
namespace IronMan
{
	namespace Thread
	{
		CThreadMutex::CThreadMutex()
		{
#ifdef THREAD_MUTEX_TRACING_ENABLED
			memset(&m_CriticalSection, 0, sizeof(m_CriticalSection));
#endif
			InitializeCriticalSectionAndSpinCount((CRITICAL_SECTION *)&m_CriticalSection, 4000);
#ifdef THREAD_MUTEX_TRACING_SUPPORTED
			// These need to be initialized unconditionally in case mixing release & debug object modules
			// Lock and unlock may be emitted as COMDATs, in which case may get spurious output
			m_currentOwnerID = m_lockCount = 0;
			m_bTrace = false;
#endif

		}
		CThreadMutex::~CThreadMutex()
		{
			DeleteCriticalSection((CRITICAL_SECTION *)&m_CriticalSection);
		}
		void CThreadMutex::Lock()
		{
#ifdef THREAD_MUTEX_TRACING_ENABLED
			uint32_t thisThreadID = GetCurrentThreadId();
			if (m_bTrace && m_currentOwnerID && (m_currentOwnerID != thisThreadID))
			{
				CONSOLE_INFO("Thread %u about to wait for lock %x owned by %u\n", GetCurrentThreadId(), (CRITICAL_SECTION *)&m_CriticalSection, m_currentOwnerID);
			}
				
#endif

			EnterCriticalSection((CRITICAL_SECTION *)&m_CriticalSection);

#ifdef THREAD_MUTEX_TRACING_ENABLED
			if (m_lockCount == 0)
			{
				// we now own it for the first time.  Set owner information
				m_currentOwnerID = thisThreadID;
				if (m_bTrace)
				{
					CONSOLE_INFO("Thread %u now owns lock 0x%x\n", m_currentOwnerID, (CRITICAL_SECTION *)&m_CriticalSection);
				}
					
			}
			m_lockCount++;
#endif
		}
		void CThreadMutex::Unlock()
		{

#ifdef THREAD_MUTEX_TRACING_ENABLED
			assert(m_lockCount >= 1);//"Invalid unlock of thread lock"
			m_lockCount--;
			if (m_lockCount == 0)
			{
				if (m_bTrace)
				{
					CONSOLE_INFO("Thread %u releasing lock 0x%x\n", m_currentOwnerID, (CRITICAL_SECTION *)&m_CriticalSection);
				}
				m_currentOwnerID = 0;
			}
#endif
			LeaveCriticalSection((CRITICAL_SECTION *)&m_CriticalSection);
		}
		bool CThreadMutex::TryLock()
		{

			uint32_t thisThreadID = GetCurrentThreadId();
			if (m_bTrace && m_currentOwnerID && (m_currentOwnerID != thisThreadID))
			{
				CONSOLE_INFO("Thread %u about to try-wait for lock %x owned by %u\n", GetCurrentThreadId(), (CRITICAL_SECTION *)&m_CriticalSection, m_currentOwnerID);
			}
			if (TryEnterCriticalSection((CRITICAL_SECTION *)&m_CriticalSection) != FALSE)
			{
				if (m_lockCount == 0)
				{
					// we now own it for the first time.  Set owner information
					m_currentOwnerID = thisThreadID;
					if (m_bTrace)
					{
						CONSOLE_INFO("Thread %u now owns lock 0x%x\n", m_currentOwnerID, (CRITICAL_SECTION *)&m_CriticalSection);
					}
				}
				m_lockCount++;

				return true;
			}
			return false;
		}
		bool CThreadMutex::AssertOwnedByCurrentThread()
		{
#ifdef THREAD_MUTEX_TRACING_ENABLED
			if (GetCurrentThreadId() == m_currentOwnerID)
				return true;
			assert(0);//"Expected thread %u as owner of lock 0x%x, but %u owns", ThreadGetCurrentId(), (CRITICAL_SECTION *)&m_CriticalSection, m_currentOwnerID
			return false;
#else
			return true;
#endif
		}
		void CThreadMutex::SetTrace(bool bTrace)
		{
#ifdef THREAD_MUTEX_TRACING_ENABLED
			m_bTrace = bTrace;
#endif
		}
		void CThreadFastMutex::Lock(const uint32_t threadId, unsigned nSpinSleepTime) volatile
		{
			int i;
			if (nSpinSleepTime != TT_INFINITE)
			{
				for (i = 1000; i != 0; --i)
				{
					if (TryLock(threadId))
					{
						return;
					}
					__debugbreak();
				}

#ifdef _WIN32
				if (!nSpinSleepTime && GetThreadPriority(GetCurrentThread()) > THREAD_PRIORITY_NORMAL)
				{
					nSpinSleepTime = 1;
				}
				else
#endif

					if (nSpinSleepTime)
					{
						for (i = 4000; i != 0; --i)
						{
							if (TryLock(threadId))
							{
								return;
							}

							_mm_pause();
							Sleep(0);
						}

					}

				for (;; ) // coded as for instead of while to make easy to breakpoint success
				{
					if (TryLock(threadId))
					{
						return;
					}

					_mm_pause();
					Sleep(nSpinSleepTime);
				}
			}
			else
			{
				for (;; ) // coded as for instead of while to make easy to breakpoint success
				{
					if (TryLock(threadId))
					{
						return;
					}

					_mm_pause();
				}
			}
		}
}

}