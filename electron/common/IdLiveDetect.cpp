
#include "common/IdLiveDetect.h"

namespace atom {

IdLiveDetect* IdLiveDetect::m_inst = nullptr;

IdLiveDetect::IdLiveDetect() {
    m_idGen = 0;
    ::InitializeCriticalSection(&m_liveSelfLock);    
}

IdLiveDetect::~IdLiveDetect() {
}

int IdLiveDetect::constructed() {
    ::EnterCriticalSection(&m_liveSelfLock);
    int id = ++m_idGen;
    m_liveSelf.insert(id);
    ::LeaveCriticalSection(&m_liveSelfLock);
    return id;
}

void IdLiveDetect::deconstructed(int id) {
    ::EnterCriticalSection(&m_liveSelfLock);
    std::set<int>::const_iterator it = m_liveSelf.find(id);
    if (it != m_liveSelf.end()) {
        ::LeaveCriticalSection(&m_liveSelfLock);
        return;
    }
    m_liveSelf.erase(it);
    ::LeaveCriticalSection(&m_liveSelfLock);
}

IdLiveDetect* IdLiveDetect::get() {
    if (!m_inst)
        m_inst = new IdLiveDetect();
    return m_inst;
}

bool IdLiveDetect::isLive(int id) {
    ::EnterCriticalSection(&m_liveSelfLock);
    std::set<int>::const_iterator it = m_liveSelf.find(id);
    bool b = it != m_liveSelf.end();
    ::LeaveCriticalSection(&m_liveSelfLock);
    return b;
}

}