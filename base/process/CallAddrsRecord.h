#ifndef base_process_CallAddrsRecord_h
#define base_process_CallAddrsRecord_h

#include <xmmintrin.h>
#include <map>
#include <set>
#include <intrin.h>

#ifdef _DEBUG
#define ENABLE_MEM_COUNT 1
#endif // _DEBUG

class CallAddrsRecord {
public:
    CallAddrsRecord()
    {
        ::InitializeCriticalSection(&m_mutex);
    }

    ~CallAddrsRecord()
    {
        ::DeleteCriticalSection(&m_mutex);
    }

    void recordMalloc(void* ptrAddr, void* funcAddr, bool canRecover)
    {
        //::EnterCriticalSection(&m_mutex);
        std::map<void*, void*>::iterator it = m_callAddrs.find(ptrAddr);
        if (it != m_callAddrs.end()) {
            //ASSERT(canRecover);
            if (!canRecover)
                DebugBreak();
            it->second = funcAddr;
        }
        else {
            //ASSERT(!canRecover);
            if (canRecover)
                DebugBreak();
            m_callAddrs.insert(std::pair<void*, void*>(ptrAddr, funcAddr));
        }
        //::LeaveCriticalSection(&m_mutex);
    }

    void recordRealloc(void* oldPtrAddr, void* newPtrAddr, void* funcAddr)
    {
        //::EnterCriticalSection(&m_mutex);
        std::map<void*, void*>::iterator it = m_callAddrs.find(oldPtrAddr);
        if (it == m_callAddrs.end())
            DebugBreak();
        m_callAddrs.erase(it);
        m_callAddrs.insert(std::pair<void*, void*>(newPtrAddr, funcAddr));
        //::LeaveCriticalSection(&m_mutex);
    }

    void recordFree(void* ptrAddr, void* funcAddr)
    {
        ::EnterCriticalSection(&m_mutex);
        std::map<void*, void*>::iterator it = m_callAddrs.find(ptrAddr);
        if (it == m_callAddrs.end())
            DebugBreak();
        m_callAddrs.erase(it);
        ::LeaveCriticalSection(&m_mutex);
    }

    void lock()
    {
        ::EnterCriticalSection(&m_mutex);
    }

    void unlock()
    {
        ::LeaveCriticalSection(&m_mutex);
    }

private:
    CRITICAL_SECTION m_mutex;
    std::map<void*, void*> m_callAddrs;
};

extern CallAddrsRecord* g_callAddrsRecord;

#define RECORD_MALLOC(ptrAddr, canRecover) \
    g_callAddrsRecord->recordMalloc(ptrAddr, _ReturnAddress(), canRecover);

#define RECORD_FREE(ptrAddr) \
    g_callAddrsRecord->recordFree(ptrAddr, _ReturnAddress());

#define RECORD_REALLOC(oldPtrAddr, newPtrAddr) \
    g_callAddrsRecord->recordRealloc(oldPtrAddr, newPtrAddr, _ReturnAddress());

#define RECORD_LOCK() \
    if (!g_callAddrsRecord) \
        g_callAddrsRecord = new CallAddrsRecord(); \
    g_callAddrsRecord->lock();

#define RECORD_UNLOCK() \
    g_callAddrsRecord->unlock();

#endif // base_process_CallAddrsRecord_h