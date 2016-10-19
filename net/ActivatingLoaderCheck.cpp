
#include <vector>
#include "net/WebURLLoaderWinINet.h"

#include "net/ActivatingLoaderCheck.h"

namespace net {

ActivatingLoaderCheck* ActivatingLoaderCheck::m_inst = nullptr;

ActivatingLoaderCheck::ActivatingLoaderCheck()
{
    m_activatingLoaders = new std::set<WebURLLoaderWinINet*>();
    ::InitializeCriticalSection(&m_mutex);
}

ActivatingLoaderCheck::~ActivatingLoaderCheck()
{
    ::DeleteCriticalSection(&m_mutex);
}

ActivatingLoaderCheck* ActivatingLoaderCheck::inst()
{
    if (!m_inst)
        m_inst = new ActivatingLoaderCheck();
    return m_inst;
}

void ActivatingLoaderCheck::destroy()
{
    delete m_activatingLoaders;
    m_activatingLoaders = nullptr;

    if (m_inst)
        delete m_inst;
}

void ActivatingLoaderCheck::addActivatingLoader(WebURLLoaderWinINet* loader)
{
    ::EnterCriticalSection(&m_mutex);
    m_activatingLoaders->insert(loader);
    ::LeaveCriticalSection(&m_mutex);
}

void ActivatingLoaderCheck::removeActivatingLoader(WebURLLoaderWinINet* loader)
{
    ::EnterCriticalSection(&m_mutex);
    m_activatingLoaders->erase(loader);
    ::LeaveCriticalSection(&m_mutex);
}

bool ActivatingLoaderCheck::isActivating(WebURLLoaderWinINet* loader)
{
    ::EnterCriticalSection(&m_mutex);
    bool isActivating = m_activatingLoaders->find(loader) != m_activatingLoaders->end();
    ::LeaveCriticalSection(&m_mutex);
    return isActivating;
}

void ActivatingLoaderCheck::testPrint()
{
#ifdef _DEBUG
    std::set<WebURLLoaderWinINet*>::iterator it = m_activatingLoaders->begin();
    for (; it != m_activatingLoaders->end(); ++it) {
        WTF::String outstr = String::format("ActivatingLoaderCheck::testPrint: %p\n", *it);
        OutputDebugStringW(outstr.charactersWithNullTermination().data());
    }
#endif
}

void ActivatingLoaderCheck::doGarbageCollected(bool forceGC)
{
    if (!forceGC) {
        ::EnterCriticalSection(&m_mutex);
        if (m_activatingLoaders->size() < 10)
            return;
        ::LeaveCriticalSection(&m_mutex);
    }

    ::EnterCriticalSection(&m_mutex);
    std::vector<WebURLLoaderWinINet*> activatingLoaders;
    activatingLoaders.resize(m_activatingLoaders->size());

    std::set<WebURLLoaderWinINet*>::iterator setIt = m_activatingLoaders->begin();
    std::set<WebURLLoaderWinINet*>::iterator end = m_activatingLoaders->end();
    for (unsigned i = 0; setIt != end; ++setIt, ++i)
        activatingLoaders[i] = *setIt;

    ::LeaveCriticalSection(&m_mutex);

    std::vector<WebURLLoaderWinINet*>::iterator it = activatingLoaders.begin();
    for (; it != activatingLoaders.end(); ++it) {
        WebURLLoaderWinINet* loader = *it;
        double time = WTF::currentTimeMS();
        if (time - loader->startTime() > 10000 || forceGC) {
            //loader->onTimeout();
        }
    }
}

void ActivatingLoaderCheck::shutdown()
{
    doGarbageCollected(true);
}

} // net