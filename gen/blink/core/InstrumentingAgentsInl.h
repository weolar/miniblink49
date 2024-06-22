// Code generated from InspectorInstrumentation.idl

#ifndef InstrumentingAgentsInl_h
#define InstrumentingAgentsInl_h

#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "wtf/FastAllocBase.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

class AsyncCallTracker;
class InspectorAnimationAgent;
class InspectorApplicationCacheAgent;
class InspectorCSSAgent;
class InspectorConsoleAgent;
class InspectorDOMAgent;
class InspectorDOMDebuggerAgent;
class InspectorDebuggerAgent;
class InspectorLayerTreeAgent;
class InspectorPageAgent;
class InspectorProfilerAgent;
class InspectorResourceAgent;
class InspectorWorkerAgent;
class PageConsoleAgent;
class PageDebuggerAgent;
class PageRuntimeAgent;

class CORE_EXPORT InstrumentingAgents : public RefCountedWillBeGarbageCollectedFinalized<InstrumentingAgents> {
    WTF_MAKE_NONCOPYABLE(InstrumentingAgents);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(InstrumentingAgents);
public:
    static PassRefPtrWillBeRawPtr<InstrumentingAgents> create()
    {
        return adoptRefWillBeNoop(new InstrumentingAgents());
    }
    ~InstrumentingAgents() { }
    DECLARE_TRACE();
    void reset();


    AsyncCallTracker* asyncCallTracker() const { return m_asyncCallTracker; }
    void setAsyncCallTracker(AsyncCallTracker* agent) { m_asyncCallTracker = agent; }

    InspectorAnimationAgent* inspectorAnimationAgent() const { return m_inspectorAnimationAgent; }
    void setInspectorAnimationAgent(InspectorAnimationAgent* agent) { m_inspectorAnimationAgent = agent; }

    InspectorApplicationCacheAgent* inspectorApplicationCacheAgent() const { return m_inspectorApplicationCacheAgent; }
    void setInspectorApplicationCacheAgent(InspectorApplicationCacheAgent* agent) { m_inspectorApplicationCacheAgent = agent; }

    InspectorCSSAgent* inspectorCSSAgent() const { return m_inspectorCSSAgent; }
    void setInspectorCSSAgent(InspectorCSSAgent* agent) { m_inspectorCSSAgent = agent; }

    InspectorConsoleAgent* inspectorConsoleAgent() const { return m_inspectorConsoleAgent; }
    void setInspectorConsoleAgent(InspectorConsoleAgent* agent) { m_inspectorConsoleAgent = agent; }

    InspectorDOMAgent* inspectorDOMAgent() const { return m_inspectorDOMAgent; }
    void setInspectorDOMAgent(InspectorDOMAgent* agent) { m_inspectorDOMAgent = agent; }

    InspectorDOMDebuggerAgent* inspectorDOMDebuggerAgent() const { return m_inspectorDOMDebuggerAgent; }
    void setInspectorDOMDebuggerAgent(InspectorDOMDebuggerAgent* agent) { m_inspectorDOMDebuggerAgent = agent; }

    InspectorDebuggerAgent* inspectorDebuggerAgent() const { return m_inspectorDebuggerAgent; }
    void setInspectorDebuggerAgent(InspectorDebuggerAgent* agent) { m_inspectorDebuggerAgent = agent; }

    InspectorLayerTreeAgent* inspectorLayerTreeAgent() const { return m_inspectorLayerTreeAgent; }
    void setInspectorLayerTreeAgent(InspectorLayerTreeAgent* agent) { m_inspectorLayerTreeAgent = agent; }

    InspectorPageAgent* inspectorPageAgent() const { return m_inspectorPageAgent; }
    void setInspectorPageAgent(InspectorPageAgent* agent) { m_inspectorPageAgent = agent; }

    InspectorProfilerAgent* inspectorProfilerAgent() const { return m_inspectorProfilerAgent; }
    void setInspectorProfilerAgent(InspectorProfilerAgent* agent) { m_inspectorProfilerAgent = agent; }

    InspectorResourceAgent* inspectorResourceAgent() const { return m_inspectorResourceAgent; }
    void setInspectorResourceAgent(InspectorResourceAgent* agent) { m_inspectorResourceAgent = agent; }

    InspectorWorkerAgent* inspectorWorkerAgent() const { return m_inspectorWorkerAgent; }
    void setInspectorWorkerAgent(InspectorWorkerAgent* agent) { m_inspectorWorkerAgent = agent; }

    PageConsoleAgent* pageConsoleAgent() const { return m_pageConsoleAgent; }
    void setPageConsoleAgent(PageConsoleAgent* agent) { m_pageConsoleAgent = agent; }

    PageDebuggerAgent* pageDebuggerAgent() const { return m_pageDebuggerAgent; }
    void setPageDebuggerAgent(PageDebuggerAgent* agent) { m_pageDebuggerAgent = agent; }

    PageRuntimeAgent* pageRuntimeAgent() const { return m_pageRuntimeAgent; }
    void setPageRuntimeAgent(PageRuntimeAgent* agent) { m_pageRuntimeAgent = agent; }

private:
    InstrumentingAgents();

    RawPtrWillBeMember<AsyncCallTracker> m_asyncCallTracker;
    RawPtrWillBeMember<InspectorAnimationAgent> m_inspectorAnimationAgent;
    RawPtrWillBeMember<InspectorApplicationCacheAgent> m_inspectorApplicationCacheAgent;
    RawPtrWillBeMember<InspectorCSSAgent> m_inspectorCSSAgent;
    RawPtrWillBeMember<InspectorConsoleAgent> m_inspectorConsoleAgent;
    RawPtrWillBeMember<InspectorDOMAgent> m_inspectorDOMAgent;
    RawPtrWillBeMember<InspectorDOMDebuggerAgent> m_inspectorDOMDebuggerAgent;
    RawPtrWillBeMember<InspectorDebuggerAgent> m_inspectorDebuggerAgent;
    RawPtrWillBeMember<InspectorLayerTreeAgent> m_inspectorLayerTreeAgent;
    RawPtrWillBeMember<InspectorPageAgent> m_inspectorPageAgent;
    RawPtrWillBeMember<InspectorProfilerAgent> m_inspectorProfilerAgent;
    RawPtrWillBeMember<InspectorResourceAgent> m_inspectorResourceAgent;
    RawPtrWillBeMember<InspectorWorkerAgent> m_inspectorWorkerAgent;
    RawPtrWillBeMember<PageConsoleAgent> m_pageConsoleAgent;
    RawPtrWillBeMember<PageDebuggerAgent> m_pageDebuggerAgent;
    RawPtrWillBeMember<PageRuntimeAgent> m_pageRuntimeAgent;
};

}

#endif // !defined(InstrumentingAgentsInl_h)
