
#include "common/IdLiveDetect.h"

namespace atom {

IdLiveDetect* IdLiveDetect::m_inst = nullptr;

IdLiveDetect::IdLiveDetect() {
    m_idGen = 0;
    ::InitializeCriticalSection(&m_liveSelfLock);    
}

IdLiveDetect::~IdLiveDetect() {
}

int IdLiveDetect::constructed(void* ptr) {
    ::EnterCriticalSection(&m_liveSelfLock);
    int id = ++m_idGen;
    m_liveSelf.insert(std::pair<int, void*>(id, ptr));
    ::LeaveCriticalSection(&m_liveSelfLock);
    return id;
}

void IdLiveDetect::deconstructed(int id) {
    ::EnterCriticalSection(&m_liveSelfLock);
    std::map<int, void*>::const_iterator it = m_liveSelf.find(id);
    if (it == m_liveSelf.end()) {
        ::LeaveCriticalSection(&m_liveSelfLock);
        return;
    }
    m_liveSelf.erase(it);
    ::LeaveCriticalSection(&m_liveSelfLock);
}

void* IdLiveDetect::getPtrById(int id) {
    void* ptr = nullptr;
    ::EnterCriticalSection(&m_liveSelfLock);
    std::map<int, void*>::const_iterator it = m_liveSelf.find(id);
    if (it == m_liveSelf.end()) {
        ::LeaveCriticalSection(&m_liveSelfLock);
        return nullptr;
    }
    ptr = it->second;
    ::LeaveCriticalSection(&m_liveSelfLock);
    return ptr;
}

IdLiveDetect* IdLiveDetect::get() {
    if (!m_inst)
        m_inst = new IdLiveDetect();
    return m_inst;
}

bool IdLiveDetect::isLive(int id) {
    ::EnterCriticalSection(&m_liveSelfLock);
    std::map<int, void*>::const_iterator it = m_liveSelf.find(id);
    bool b = it != m_liveSelf.end();
    ::LeaveCriticalSection(&m_liveSelfLock);
    return b;
}

}