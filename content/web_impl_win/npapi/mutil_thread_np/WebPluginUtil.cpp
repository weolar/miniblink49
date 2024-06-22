
#include "content/web_impl_win/npapi/WebPluginUtil.h"

#include "third_party/WebKit/Source/platform/WebThreadSupportingGC.h"

namespace content {

static void closureCallWrap(int* countPtr, void* param)
{
    std::function<void(void)>* closure = (std::function<void(void)>*)param;
    (*closure)();
    delete closure;
    WTF::atomicIncrement(countPtr);
}

WebPluginUtil::WebPluginUtil()
{
    m_idGen = 0;
    m_thread = blink::WebThreadSupportingGC::create("WebPluginUtil").leakPtr();
    ::InitializeCriticalSection(&m_lock);
}

void WebPluginUtil::postTaskToNpThread(const blink::WebTraceLocation& location, std::function<void(void)>&& closure)
{
    int count = 0;
    std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
    m_thread->postTask(location, WTF::bind(&closureCallWrap, &count, closureDummy));
}

void WebPluginUtil::postTaskToBlinkThread(const blink::WebTraceLocation& location, std::function<void(void)>&& closure)
{
    int count = 0;
    std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
    blink::Platform::current()->mainThread()->postTask(location, WTF::bind(&closureCallWrap, &count, closureDummy));
}

void WebPluginUtil::sendTaskToNpThread(const blink::WebTraceLocation& location, std::function<void(void)>&& closure)
{
    int count = 0;
    std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
    m_thread->postTask(location, WTF::bind(&closureCallWrap, &count, closureDummy));

    while (count > 0) {
        Sleep(5);
    }
}

void WebPluginUtil::sendTaskToBlinkThread(const blink::WebTraceLocation& location, std::function<void(void)>&& closure)
{
    int count = 0;
    std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
    blink::Platform::current()->mainThread()->postTask(location, WTF::bind(&closureCallWrap, &count, closureDummy));

    while (count > 0) {
        Sleep(5);
    }
}

int WebPluginUtil::genId()
{
    ++m_idGen;
    return m_idGen;
}

bool WebPluginUtil::isLive(int id)
{
    ::EnterCriticalSection(&m_lock);
    std::set<int>::const_iterator it = m_liveSet.find(id);
    bool b = it != m_liveSet.end();
    ::LeaveCriticalSection(&m_lock);
    return b;
}

void WebPluginUtil::addId(int id)
{
    ::EnterCriticalSection(&m_lock);
    m_liveSet.insert(id);
    ::LeaveCriticalSection(&m_lock);
}

void WebPluginUtil::removeId(int id)
{
    ::EnterCriticalSection(&m_lock);
    m_liveSet.erase(id);
    ::LeaveCriticalSection(&m_lock);
}


}