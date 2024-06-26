
#ifndef common_LiveIdDetect_h
#define common_LiveIdDetect_h

#include <set>
#include <map>
#include <windows.h>

namespace common {

class LiveIdDetect {
public:
    LiveIdDetect()
    {
        m_idGen = 0;
        ::InitializeCriticalSection(&m_lock);
    }

    ~LiveIdDetect()
    {
    }

    int64_t constructed(void* ptr)
    {
        if (isActivating(ptr))
            DebugBreak();

        ::EnterCriticalSection(&m_lock);
        int64_t id = ++m_idGen;
        m_liveSelf.insert(std::pair<int64_t, void*>(id, ptr));

        std::map<void*, int64_t> ::const_iterator it = m_ptrToIdMaps.find(ptr);
        if (it != m_ptrToIdMaps.end())
            DebugBreak();
        m_ptrToIdMaps.insert(std::pair<void*, int64_t>(ptr, id));
        ::LeaveCriticalSection(&m_lock);
        return id;
    }

    void deconstructed(int64_t id)
    {
        ::EnterCriticalSection(&m_lock);
        std::map<int64_t, void*> ::const_iterator it = m_liveSelf.find(id);
        if (it == m_liveSelf.end()) {
            ::LeaveCriticalSection(&m_lock);
            return;
        }
        void* ptr = it->second;
        m_liveSelf.erase(it);

        std::map<void*, int64_t> ::const_iterator it2 = m_ptrToIdMaps.find(ptr);
        if (it2 == m_ptrToIdMaps.end())
            DebugBreak();
        m_ptrToIdMaps.erase(it2);
        ::LeaveCriticalSection(&m_lock);
    }

    void* getPtr(int64_t id)
    {
        ::EnterCriticalSection(&m_lock);
        std::map<int64_t, void*>::const_iterator it = m_liveSelf.find(id);
        if (it == m_liveSelf.end()) {
            ::LeaveCriticalSection(&m_lock);
            return nullptr;
        }
        void* ptr = it->second;
        ::LeaveCriticalSection(&m_lock);

        return ptr;
    }

    void* getPtrLocked(int64_t id)
    {
        ::EnterCriticalSection(&m_lock);
        std::map<int64_t, void*>::const_iterator it = m_liveSelf.find(id);
        if (it == m_liveSelf.end()) {
            ::LeaveCriticalSection(&m_lock);
            return nullptr;
        }
        void* ptr = it->second;

        return ptr;
    }

    int64_t getIdLocked(void* ptr)
    {
        int64_t id = -1;
        ::EnterCriticalSection(&m_lock);
        std::map<void*, int64_t> ::const_iterator it = m_ptrToIdMaps.find(ptr);
        
        if (it == m_ptrToIdMaps.end())
            ::LeaveCriticalSection(&m_lock);
        else
            id = it->second;

        return id;
    }

    void unlock(int64_t id, void*)
    {
        ::LeaveCriticalSection(&m_lock);
    }

    static LiveIdDetect* get()
    {
        if (!m_inst)
            m_inst = new LiveIdDetect();
        return m_inst;
    }

    bool isLive(int64_t id)
    {
        ::EnterCriticalSection(&m_lock);
        std::map<int64_t, void*> ::const_iterator it = m_liveSelf.find(id);
        bool b = it != m_liveSelf.end();
        ::LeaveCriticalSection(&m_lock);
        return b;
    }

    bool isActivating(void* ptr)
    {
        bool b = false;
        ::EnterCriticalSection(&m_lock);

        std::map<void*, int64_t> ::const_iterator it = m_ptrToIdMaps.find(ptr);
        b = it != m_ptrToIdMaps.end();
       
        ::LeaveCriticalSection(&m_lock);
        return b;
    }

private:
    int64_t m_idGen;
    std::map<int64_t, void*> m_liveSelf;
    std::map<void*, int64_t> m_ptrToIdMaps;
    CRITICAL_SECTION m_lock;
    static LiveIdDetect* m_inst;
};

}

#endif // common_LiveIdDetect_h