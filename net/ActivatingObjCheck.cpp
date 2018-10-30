
#include "net/ActivatingObjCheck.h"
#include <vector>

namespace net {

ActivatingObjCheck* ActivatingObjCheck::m_inst = nullptr;

ActivatingObjCheck::ActivatingObjCheck()
{
    m_newestId = 1;
    m_activatingObjs = new std::set<intptr_t>();
    ::InitializeCriticalSection(&m_mutex);
}

ActivatingObjCheck::~ActivatingObjCheck()
{
    ::DeleteCriticalSection(&m_mutex);
}

ActivatingObjCheck* ActivatingObjCheck::inst()
{
    if (!m_inst)
        m_inst = new ActivatingObjCheck();
    return m_inst;
}

void ActivatingObjCheck::destroy()
{
    delete m_activatingObjs;
    m_activatingObjs = nullptr;

    if (m_inst)
        delete m_inst;
}

void ActivatingObjCheck::add(intptr_t loader)
{
    ::EnterCriticalSection(&m_mutex);
    m_activatingObjs->insert(loader);
    ::LeaveCriticalSection(&m_mutex);
}

void ActivatingObjCheck::remove(intptr_t loader)
{
    ::EnterCriticalSection(&m_mutex);
    m_activatingObjs->erase(loader);
    ::LeaveCriticalSection(&m_mutex);
}

bool ActivatingObjCheck::isActivating(intptr_t loader)
{
    ::EnterCriticalSection(&m_mutex);
    bool isActivating = m_activatingObjs->find(loader) != m_activatingObjs->end();
    ::LeaveCriticalSection(&m_mutex);
    return isActivating;
}

int ActivatingObjCheck::genId()
{
    InterlockedIncrement((long *)&m_newestId);
    return m_newestId;
}

void ActivatingObjCheck::testPrint()
{
#ifdef _DEBUG
    std::set<intptr_t>::iterator it = m_activatingObjs->begin();
    for (; it != m_activatingObjs->end(); ++it) {
        char* output = (char*)malloc(0x100);
        sprintf(output, "ActivatingObjCheck::testPrint %p\n", (void*)(*it));
        OutputDebugStringA(output);
        free(output);
    }
#endif
}

void ActivatingObjCheck::doGarbageCollected(bool forceGC)
{
    if (!forceGC) {
        ::EnterCriticalSection(&m_mutex);
        int size = m_activatingObjs->size();
        ::LeaveCriticalSection(&m_mutex);
        if (size < 10)
            return;
    }

    ::EnterCriticalSection(&m_mutex);
    std::vector<intptr_t> activatingLoaders;
    activatingLoaders.resize(m_activatingObjs->size());

    std::set<intptr_t>::iterator setIt = m_activatingObjs->begin();
    std::set<intptr_t>::iterator end = m_activatingObjs->end();
    for (unsigned i = 0; setIt != end; ++setIt, ++i)
        activatingLoaders[i] = *setIt;

    ::LeaveCriticalSection(&m_mutex);

//     std::vector<intptr_t>::iterator it = activatingLoaders.begin();
//     for (; it != activatingLoaders.end(); ++it) {
//         intptr_t loader = *it;
//         double time = WTF::currentTimeMS();
//         if (time - loader->startTime() > 10000 || forceGC) {
//             loader->onTimeout();
//         }
//     }
}

void ActivatingObjCheck::shutdown()
{
    doGarbageCollected(true);
}

} // net