// Code generated from InspectorInstrumentation.idl

#include "config.h"

#include "InspectorConsoleInstrumentationInl.h"
#include "InspectorInstrumentationInl.h"
#include "InspectorOverridesInl.h"
#include "InstrumentingAgentsInl.h"
#include "core/CoreExport.h"
#include "core/inspector/AsyncCallTracker.h"
#include "core/inspector/InspectorAnimationAgent.h"
#include "core/inspector/InspectorApplicationCacheAgent.h"
#include "core/inspector/InspectorCSSAgent.h"
#include "core/inspector/InspectorConsoleAgent.h"
#include "core/inspector/InspectorDOMAgent.h"
#include "core/inspector/InspectorDOMDebuggerAgent.h"
#include "core/inspector/InspectorDebuggerAgent.h"
#include "core/inspector/InspectorLayerTreeAgent.h"
#include "core/inspector/InspectorPageAgent.h"
#include "core/inspector/InspectorProfilerAgent.h"
#include "core/inspector/InspectorResourceAgent.h"
#include "core/inspector/InspectorWorkerAgent.h"
#include "core/inspector/PageConsoleAgent.h"
#include "core/inspector/PageDebuggerAgent.h"
#include "core/inspector/PageRuntimeAgent.h"

namespace blink {

InstrumentingAgents::InstrumentingAgents()
    : m_asyncCallTracker(nullptr)
    , m_inspectorAnimationAgent(nullptr)
    , m_inspectorApplicationCacheAgent(nullptr)
    , m_inspectorCSSAgent(nullptr)
    , m_inspectorConsoleAgent(nullptr)
    , m_inspectorDOMAgent(nullptr)
    , m_inspectorDOMDebuggerAgent(nullptr)
    , m_inspectorDebuggerAgent(nullptr)
    , m_inspectorLayerTreeAgent(nullptr)
    , m_inspectorPageAgent(nullptr)
    , m_inspectorProfilerAgent(nullptr)
    , m_inspectorResourceAgent(nullptr)
    , m_inspectorWorkerAgent(nullptr)
    , m_pageConsoleAgent(nullptr)
    , m_pageDebuggerAgent(nullptr)
    , m_pageRuntimeAgent(nullptr)
{
}

DEFINE_TRACE(InstrumentingAgents)
{
    visitor->trace(m_asyncCallTracker);
    visitor->trace(m_inspectorAnimationAgent);
    visitor->trace(m_inspectorApplicationCacheAgent);
    visitor->trace(m_inspectorCSSAgent);
    visitor->trace(m_inspectorConsoleAgent);
    visitor->trace(m_inspectorDOMAgent);
    visitor->trace(m_inspectorDOMDebuggerAgent);
    visitor->trace(m_inspectorDebuggerAgent);
    visitor->trace(m_inspectorLayerTreeAgent);
    visitor->trace(m_inspectorPageAgent);
    visitor->trace(m_inspectorProfilerAgent);
    visitor->trace(m_inspectorResourceAgent);
    visitor->trace(m_inspectorWorkerAgent);
    visitor->trace(m_pageConsoleAgent);
    visitor->trace(m_pageDebuggerAgent);
    visitor->trace(m_pageRuntimeAgent);
}

void InstrumentingAgents::reset()
{
    m_asyncCallTracker = nullptr;
    m_inspectorAnimationAgent = nullptr;
    m_inspectorApplicationCacheAgent = nullptr;
    m_inspectorCSSAgent = nullptr;
    m_inspectorConsoleAgent = nullptr;
    m_inspectorDOMAgent = nullptr;
    m_inspectorDOMDebuggerAgent = nullptr;
    m_inspectorDebuggerAgent = nullptr;
    m_inspectorLayerTreeAgent = nullptr;
    m_inspectorPageAgent = nullptr;
    m_inspectorProfilerAgent = nullptr;
    m_inspectorResourceAgent = nullptr;
    m_inspectorWorkerAgent = nullptr;
    m_pageConsoleAgent = nullptr;
    m_pageDebuggerAgent = nullptr;
    m_pageRuntimeAgent = nullptr;
}

namespace InspectorInstrumentation {

void didClearDocumentOfWindowObjectImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (PageDebuggerAgent* agent = agents->pageDebuggerAgent())
        agent->didClearDocumentOfWindowObject(paramLocalFrame);
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->didClearDocumentOfWindowObject(paramLocalFrame);
    if (PageRuntimeAgent* agent = agents->pageRuntimeAgent())
        agent->didClearDocumentOfWindowObject(paramLocalFrame);
    if (InspectorAnimationAgent* agent = agents->inspectorAnimationAgent())
        agent->didClearDocumentOfWindowObject(paramLocalFrame);
}

void willCloseWindowImpl(InstrumentingAgents* agents)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willCloseWindow();
}

void willInsertDOMNodeImpl(InstrumentingAgents* agents, Node* parent)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willInsertDOMNode(parent);
}

void didInsertDOMNodeImpl(InstrumentingAgents* agents, Node* paramNode)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->didInsertDOMNode(paramNode);
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didInsertDOMNode(paramNode);
}

void willRemoveDOMNodeImpl(InstrumentingAgents* agents, Node* paramNode)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willRemoveDOMNode(paramNode);
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->willRemoveDOMNode(paramNode);
}

void willModifyDOMAttrImpl(InstrumentingAgents* agents, Element* paramElement, const AtomicString& oldValue, const AtomicString& newValue)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willModifyDOMAttr(paramElement, oldValue, newValue);
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->willModifyDOMAttr(paramElement, oldValue, newValue);
}

void didModifyDOMAttrImpl(InstrumentingAgents* agents, Element* paramElement, const QualifiedName& name, const AtomicString& value)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->didModifyDOMAttr(paramElement, name, value);
}

void didRemoveDOMAttrImpl(InstrumentingAgents* agents, Element* paramElement, const QualifiedName& name)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->didRemoveDOMAttr(paramElement, name);
}

void characterDataModifiedImpl(InstrumentingAgents* agents, CharacterData* paramCharacterData)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->characterDataModified(paramCharacterData);
}

void didInvalidateStyleAttrImpl(InstrumentingAgents* agents, Node* paramNode)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->didInvalidateStyleAttr(paramNode);
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didInvalidateStyleAttr(paramNode);
}

void didPerformElementShadowDistributionImpl(InstrumentingAgents* agents, Element* paramElement)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->didPerformElementShadowDistribution(paramElement);
}

void documentDetachedImpl(InstrumentingAgents* agents, Document* paramDocument)
{
    if (InspectorCSSAgent* agent = agents->inspectorCSSAgent())
        agent->documentDetached(paramDocument);
}

void activeStyleSheetsUpdatedImpl(InstrumentingAgents* agents, Document* paramDocument)
{
    if (InspectorCSSAgent* agent = agents->inspectorCSSAgent())
        agent->activeStyleSheetsUpdated(paramDocument);
}

void mediaQueryResultChangedImpl(InstrumentingAgents* agents)
{
    if (InspectorCSSAgent* agent = agents->inspectorCSSAgent())
        agent->mediaQueryResultChanged();
}

void didPushShadowRootImpl(InstrumentingAgents* agents, Element* host, ShadowRoot* paramShadowRoot)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->didPushShadowRoot(host, paramShadowRoot);
}

void willPopShadowRootImpl(InstrumentingAgents* agents, Element* host, ShadowRoot* paramShadowRoot)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->willPopShadowRoot(host, paramShadowRoot);
}

void willSetInnerHTMLImpl(InstrumentingAgents* agents)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willSetInnerHTML();
}

void willSendXMLHttpRequestImpl(InstrumentingAgents* agents, const String& url)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willSendXMLHttpRequest(url);
}

void didFireWebGLErrorImpl(InstrumentingAgents* agents, const String& errorName)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didFireWebGLError(errorName);
}

void didFireWebGLWarningImpl(InstrumentingAgents* agents)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didFireWebGLWarning();
}

void didFireWebGLErrorOrWarningImpl(InstrumentingAgents* agents, const String& message)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didFireWebGLErrorOrWarning(message);
}

void didInstallTimerImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, int timerId, int timeout, bool singleShot)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->didInstallTimer(paramExecutionContext, timerId, timeout, singleShot);
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didInstallTimer(paramExecutionContext, timerId, timeout, singleShot);
}

void didRemoveTimerImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, int timerId)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->didRemoveTimer(paramExecutionContext, timerId);
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didRemoveTimer(paramExecutionContext, timerId);
}

InspectorInstrumentationCookie willCallFunctionImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, const DevToolsFunctionInfo& paramDevToolsFunctionInfo)
{
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->willCallFunction(paramExecutionContext, paramDevToolsFunctionInfo);
    return InspectorInstrumentationCookie(agents);
}

void didCallFunctionImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (InspectorDebuggerAgent* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->inspectorDebuggerAgent())
        agent->didCallFunction();
}

void didEnqueueEventImpl(InstrumentingAgents* agents, EventTarget* paramEventTarget, Event* paramEvent)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->didEnqueueEvent(paramEventTarget, paramEvent);
}

void didRemoveEventImpl(InstrumentingAgents* agents, EventTarget* paramEventTarget, Event* paramEvent)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->didRemoveEvent(paramEventTarget, paramEvent);
}

InspectorInstrumentationCookie willHandleEventImpl(InstrumentingAgents* agents, EventTarget* paramEventTarget, Event* paramEvent, EventListener* listener, bool useCapture)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->willHandleEvent(paramEventTarget, paramEvent, listener, useCapture);
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willHandleEvent(paramEventTarget, paramEvent, listener, useCapture);
    return InspectorInstrumentationCookie(agents);
}

void didHandleEventImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (AsyncCallTracker* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->asyncCallTracker())
        agent->didHandleEvent();
    if (InspectorDebuggerAgent* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->inspectorDebuggerAgent())
        agent->didHandleEvent();
}

void didEnqueueMutationRecordImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, MutationObserver* paramMutationObserver)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->didEnqueueMutationRecord(paramExecutionContext, paramMutationObserver);
}

void didClearAllMutationRecordsImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, MutationObserver* paramMutationObserver)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->didClearAllMutationRecords(paramExecutionContext, paramMutationObserver);
}

void willDeliverMutationRecordsImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, MutationObserver* paramMutationObserver)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->willDeliverMutationRecords(paramExecutionContext, paramMutationObserver);
}

void didDeliverMutationRecordsImpl(InstrumentingAgents* agents)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->didDeliverMutationRecords();
}

void didPostExecutionContextTaskImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, ExecutionContextTask* paramExecutionContextTask)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->didPostExecutionContextTask(paramExecutionContext, paramExecutionContextTask);
}

void didKillAllExecutionContextTasksImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->didKillAllExecutionContextTasks(paramExecutionContext);
}

void willPerformExecutionContextTaskImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, ExecutionContextTask* paramExecutionContextTask)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->willPerformExecutionContextTask(paramExecutionContext, paramExecutionContextTask);
}

void didPerformExecutionContextTaskImpl(InstrumentingAgents* agents)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->didPerformExecutionContextTask();
}

InspectorInstrumentationCookie willEvaluateScriptImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, const String& url, int lineNumber)
{
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->willEvaluateScript(paramLocalFrame, url, lineNumber);
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willEvaluateScript(paramLocalFrame, url, lineNumber);
    return InspectorInstrumentationCookie(agents);
}

void didEvaluateScriptImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (InspectorDebuggerAgent* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->inspectorDebuggerAgent())
        agent->didEvaluateScript();
}

void didCreateScriptContextImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, ScriptState* paramScriptState, SecurityOrigin* paramSecurityOrigin, int worldId)
{
    if (PageRuntimeAgent* agent = agents->pageRuntimeAgent())
        agent->didCreateScriptContext(paramLocalFrame, paramScriptState, paramSecurityOrigin, worldId);
}

void willReleaseScriptContextImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, ScriptState* paramScriptState)
{
    if (PageRuntimeAgent* agent = agents->pageRuntimeAgent())
        agent->willReleaseScriptContext(paramLocalFrame, paramScriptState);
}

InspectorInstrumentationCookie willFireTimerImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, int timerId)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->willFireTimer(paramExecutionContext, timerId);
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willFireTimer(paramExecutionContext, timerId);
    return InspectorInstrumentationCookie(agents);
}

void didFireTimerImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (AsyncCallTracker* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->asyncCallTracker())
        agent->didFireTimer();
    if (InspectorDebuggerAgent* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->inspectorDebuggerAgent())
        agent->didFireTimer();
}

void didLayoutImpl(InstrumentingAgents* agents)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->didLayout();
}

void didScrollImpl(InstrumentingAgents* agents)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->didScroll();
}

void didResizeMainFrameImpl(InstrumentingAgents* agents)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->didResizeMainFrame();
}

void didDispatchXHRLoadendEventImpl(InstrumentingAgents* agents, XMLHttpRequest* paramXMLHttpRequest)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->didDispatchXHRLoadendEvent(paramXMLHttpRequest);
}

void didPaintImpl(InstrumentingAgents* agents, LayoutObject* paramLayoutObject, const GraphicsLayer* paramGraphicsLayer, GraphicsContext* paramGraphicsContext, const LayoutRect& paramLayoutRect)
{
    if (InspectorLayerTreeAgent* agent = agents->inspectorLayerTreeAgent())
        agent->didPaint(paramLayoutObject, paramGraphicsLayer, paramGraphicsContext, paramLayoutRect);
}

InspectorInstrumentationCookie willRecalculateStyleImpl(InstrumentingAgents* agents, Document* paramDocument)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->willRecalculateStyle(paramDocument);
    return InspectorInstrumentationCookie(agents);
}

void didRecalculateStyleImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie, int elementCount)
{
    if (InspectorResourceAgent* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->inspectorResourceAgent())
        agent->didRecalculateStyle(elementCount);
}

void didScheduleStyleRecalculationImpl(InstrumentingAgents* agents, Document* paramDocument)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didScheduleStyleRecalculation(paramDocument);
}

void applyUserAgentOverrideImpl(InstrumentingAgents* agents, String* userAgent)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->applyUserAgentOverride(userAgent);
}

void willSendRequestImpl(InstrumentingAgents* agents, unsigned long identifier, DocumentLoader* paramDocumentLoader, ResourceRequest& paramResourceRequest, const ResourceResponse& redirectResponse, const FetchInitiatorInfo& paramFetchInitiatorInfo)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->willSendRequest(identifier, paramDocumentLoader, paramResourceRequest, redirectResponse, paramFetchInitiatorInfo);
}

void markResourceAsCachedImpl(InstrumentingAgents* agents, unsigned long identifier)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->markResourceAsCached(identifier);
}

void didReceiveResourceResponseImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, unsigned long identifier, DocumentLoader* paramDocumentLoader, const ResourceResponse& paramResourceResponse, ResourceLoader* paramResourceLoader)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didReceiveResourceResponse(paramLocalFrame, identifier, paramDocumentLoader, paramResourceResponse, paramResourceLoader);
}

void didReceiveDataImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, unsigned long identifier, const char* data, int dataLength, int encodedDataLength)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didReceiveData(paramLocalFrame, identifier, data, dataLength, encodedDataLength);
}

void didFinishLoadingImpl(InstrumentingAgents* agents, unsigned long identifier, double finishTime, int64_t encodedDataLength)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didFinishLoading(identifier, finishTime, encodedDataLength);
}

void didReceiveCORSRedirectResponseImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, unsigned long identifier, DocumentLoader* paramDocumentLoader, const ResourceResponse& paramResourceResponse, ResourceLoader* paramResourceLoader)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didReceiveCORSRedirectResponse(paramLocalFrame, identifier, paramDocumentLoader, paramResourceResponse, paramResourceLoader);
}

void didFailLoadingImpl(InstrumentingAgents* agents, unsigned long identifier, const ResourceError& paramResourceError)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didFailLoading(identifier, paramResourceError);
}

void documentThreadableLoaderStartedLoadingForClientImpl(InstrumentingAgents* agents, unsigned long identifier, ThreadableLoaderClient* client)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->documentThreadableLoaderStartedLoadingForClient(identifier, client);
}

void willSendEventSourceRequestImpl(InstrumentingAgents* agents, ThreadableLoaderClient* eventSource)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->willSendEventSourceRequest(eventSource);
}

void willDispachEventSourceEventImpl(InstrumentingAgents* agents, ThreadableLoaderClient* eventSource, const AtomicString& eventName, const AtomicString& eventId, const Vector<UChar>& data)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->willDispachEventSourceEvent(eventSource, eventName, eventId, data);
}

void didFinishEventSourceRequestImpl(InstrumentingAgents* agents, ThreadableLoaderClient* eventSource)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didFinishEventSourceRequest(eventSource);
}

void willLoadXHRImpl(InstrumentingAgents* agents, XMLHttpRequest* xhr, ThreadableLoaderClient* client, const AtomicString& method, const KURL& url, bool async, PassRefPtr<FormData> prpParamFormData, const HTTPHeaderMap& headers, bool includeCredentials)
{
    RefPtr<FormData> paramFormData = prpParamFormData;
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->willLoadXHR(xhr, client, method, url, async, paramFormData, headers, includeCredentials);
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->willLoadXHR(xhr, client, method, url, async, paramFormData, headers, includeCredentials);
}

void didFailXHRLoadingImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, XMLHttpRequest* xhr, ThreadableLoaderClient* client, const AtomicString& method, const String& url)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didFailXHRLoading(paramExecutionContext, xhr, client, method, url);
}

void didFinishXHRLoadingImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, XMLHttpRequest* xhr, ThreadableLoaderClient* client, const AtomicString& method, const String& url)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didFinishXHRLoading(paramExecutionContext, xhr, client, method, url);
}

void scriptImportedImpl(InstrumentingAgents* agents, unsigned long identifier, const String& sourceString)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->scriptImported(identifier, sourceString);
}

void scriptExecutionBlockedByCSPImpl(InstrumentingAgents* agents, const String& directiveText)
{
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->scriptExecutionBlockedByCSP(directiveText);
}

void didReceiveScriptResponseImpl(InstrumentingAgents* agents, unsigned long identifier)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didReceiveScriptResponse(identifier);
}

void didStartProvisionalLoadImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (PageDebuggerAgent* agent = agents->pageDebuggerAgent())
        agent->didStartProvisionalLoad(paramLocalFrame);
}

void domContentLoadedEventFiredImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->domContentLoadedEventFired(paramLocalFrame);
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->domContentLoadedEventFired(paramLocalFrame);
}

void loadEventFiredImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->loadEventFired(paramLocalFrame);
}

void frameAttachedToParentImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->frameAttachedToParent(paramLocalFrame);
}

void frameDetachedFromParentImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->frameDetachedFromParent(paramLocalFrame);
}

void didCommitLoadImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, DocumentLoader* paramDocumentLoader)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didCommitLoad(paramLocalFrame, paramDocumentLoader);
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->didCommitLoad(paramLocalFrame, paramDocumentLoader);
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->didCommitLoad(paramLocalFrame, paramDocumentLoader);
}

void frameDocumentUpdatedImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->frameDocumentUpdated(paramLocalFrame);
}

void frameStartedLoadingImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->frameStartedLoading(paramLocalFrame);
}

void frameStoppedLoadingImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->frameStoppedLoading(paramLocalFrame);
}

void frameScheduledNavigationImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, double delay)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->frameScheduledNavigation(paramLocalFrame, delay);
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->frameScheduledNavigation(paramLocalFrame, delay);
}

void frameClearedScheduledNavigationImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->frameClearedScheduledNavigation(paramLocalFrame);
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->frameClearedScheduledNavigation(paramLocalFrame);
}

InspectorInstrumentationCookie willRunJavaScriptDialogImpl(InstrumentingAgents* agents, const String& message, ChromeClient::DialogType dialogType)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->willRunJavaScriptDialog(message, dialogType);
    return InspectorInstrumentationCookie(agents);
}

void didRunJavaScriptDialogImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie, bool result)
{
    if (InspectorPageAgent* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->inspectorPageAgent())
        agent->didRunJavaScriptDialog(result);
}

void didRequestAnimationFrameImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, int callbackId)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->didRequestAnimationFrame(paramExecutionContext, callbackId);
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didRequestAnimationFrame(paramExecutionContext, callbackId);
}

void didCancelAnimationFrameImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, int callbackId)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->didCancelAnimationFrame(paramExecutionContext, callbackId);
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didCancelAnimationFrame(paramExecutionContext, callbackId);
}

InspectorInstrumentationCookie willFireAnimationFrameImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, int callbackId)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->willFireAnimationFrame(paramExecutionContext, callbackId);
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willFireAnimationFrame(paramExecutionContext, callbackId);
    return InspectorInstrumentationCookie(agents);
}

void didFireAnimationFrameImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (AsyncCallTracker* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->asyncCallTracker())
        agent->didFireAnimationFrame();
}

void didStartWorkerImpl(InstrumentingAgents* agents, WorkerInspectorProxy* proxy, const KURL& url)
{
    if (InspectorWorkerAgent* agent = agents->inspectorWorkerAgent())
        agent->didStartWorker(proxy, url);
}

void workerTerminatedImpl(InstrumentingAgents* agents, WorkerInspectorProxy* proxy)
{
    if (InspectorWorkerAgent* agent = agents->inspectorWorkerAgent())
        agent->workerTerminated(proxy);
    if (PageConsoleAgent* agent = agents->pageConsoleAgent())
        agent->workerTerminated(proxy);
}

void willProcessTaskImpl(InstrumentingAgents* agents)
{
    if (InspectorProfilerAgent* agent = agents->inspectorProfilerAgent())
        agent->willProcessTask();
}

void didProcessTaskImpl(InstrumentingAgents* agents)
{
    if (InspectorProfilerAgent* agent = agents->inspectorProfilerAgent())
        agent->didProcessTask();
}

void willEnterNestedRunLoopImpl(InstrumentingAgents* agents)
{
    if (InspectorProfilerAgent* agent = agents->inspectorProfilerAgent())
        agent->willEnterNestedRunLoop();
}

void didLeaveNestedRunLoopImpl(InstrumentingAgents* agents)
{
    if (InspectorProfilerAgent* agent = agents->inspectorProfilerAgent())
        agent->didLeaveNestedRunLoop();
}

void didCreateWebSocketImpl(InstrumentingAgents* agents, Document* paramDocument, unsigned long identifier, const KURL& requestURL, const String& protocol)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didCreateWebSocket(paramDocument, identifier, requestURL, protocol);
}

void willSendWebSocketHandshakeRequestImpl(InstrumentingAgents* agents, Document* paramDocument, unsigned long identifier, const WebSocketHandshakeRequest* request)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->willSendWebSocketHandshakeRequest(paramDocument, identifier, request);
}

void didReceiveWebSocketHandshakeResponseImpl(InstrumentingAgents* agents, Document* paramDocument, unsigned long identifier, const WebSocketHandshakeRequest* request, const WebSocketHandshakeResponse* response)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didReceiveWebSocketHandshakeResponse(paramDocument, identifier, request, response);
}

void didCloseWebSocketImpl(InstrumentingAgents* agents, Document* paramDocument, unsigned long identifier)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didCloseWebSocket(paramDocument, identifier);
}

void didReceiveWebSocketFrameImpl(InstrumentingAgents* agents, unsigned long identifier, int opCode, bool masked, const char* payload, size_t payloadLength)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didReceiveWebSocketFrame(identifier, opCode, masked, payload, payloadLength);
}

void didSendWebSocketFrameImpl(InstrumentingAgents* agents, unsigned long identifier, int opCode, bool masked, const char* payload, size_t payloadLength)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didSendWebSocketFrame(identifier, opCode, masked, payload, payloadLength);
}

void didReceiveWebSocketFrameErrorImpl(InstrumentingAgents* agents, unsigned long identifier, const String& errorMessage)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didReceiveWebSocketFrameError(identifier, errorMessage);
}

void networkStateChangedImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, bool online)
{
    if (InspectorApplicationCacheAgent* agent = agents->inspectorApplicationCacheAgent())
        agent->networkStateChanged(paramLocalFrame, online);
}

void updateApplicationCacheStatusImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorApplicationCacheAgent* agent = agents->inspectorApplicationCacheAgent())
        agent->updateApplicationCacheStatus(paramLocalFrame);
}

void layerTreeDidChangeImpl(InstrumentingAgents* agents)
{
    if (InspectorLayerTreeAgent* agent = agents->inspectorLayerTreeAgent())
        agent->layerTreeDidChange();
}

void pseudoElementCreatedImpl(InstrumentingAgents* agents, PseudoElement* paramPseudoElement)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->pseudoElementCreated(paramPseudoElement);
}

void pseudoElementDestroyedImpl(InstrumentingAgents* agents, PseudoElement* paramPseudoElement)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->pseudoElementDestroyed(paramPseudoElement);
}

int traceAsyncOperationStartingImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, const String& operationName)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        return agent->traceAsyncOperationStarting(paramExecutionContext, operationName);
    return 0;
}

int traceAsyncOperationStartingImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, const String& operationName, int prevOperationId)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        return agent->traceAsyncOperationStarting(paramExecutionContext, operationName, prevOperationId);
    return 0;
}

void traceAsyncOperationCompletedImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, int operationId)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->traceAsyncOperationCompleted(paramExecutionContext, operationId);
}

InspectorInstrumentationCookie traceAsyncOperationCompletedCallbackStartingImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, int operationId)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->traceAsyncOperationCompletedCallbackStarting(paramExecutionContext, operationId);
    return InspectorInstrumentationCookie(agents);
}

InspectorInstrumentationCookie traceAsyncCallbackStartingImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, int operationId)
{
    if (AsyncCallTracker* agent = agents->asyncCallTracker())
        agent->traceAsyncCallbackStarting(paramExecutionContext, operationId);
    return InspectorInstrumentationCookie(agents);
}

void traceAsyncCallbackCompletedImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (AsyncCallTracker* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->asyncCallTracker())
        agent->traceAsyncCallbackCompleted();
}

void didCreateAnimationImpl(InstrumentingAgents* agents, Animation* player)
{
    if (InspectorAnimationAgent* agent = agents->inspectorAnimationAgent())
        agent->didCreateAnimation(player);
}

void didCancelAnimationImpl(InstrumentingAgents* agents, Animation* player)
{
    if (InspectorAnimationAgent* agent = agents->inspectorAnimationAgent())
        agent->didCancelAnimation(player);
}

void addMessageToConsoleImpl(InstrumentingAgents* agents, ConsoleMessage* consoleMessage)
{
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->addMessageToConsole(consoleMessage);
}

void consoleProfileImpl(InstrumentingAgents* agents, ExecutionContext* context, const String& title)
{
    if (InspectorProfilerAgent* agent = agents->inspectorProfilerAgent())
        agent->consoleProfile(context, title);
}

void consoleProfileEndImpl(InstrumentingAgents* agents, const String& title)
{
    if (InspectorProfilerAgent* agent = agents->inspectorProfilerAgent())
        agent->consoleProfileEnd(title);
}

void consoleMessagesClearedImpl(InstrumentingAgents* agents)
{
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->consoleMessagesCleared();
}

bool forcePseudoStateImpl(InstrumentingAgents* agents, Element* element, CSSSelector::PseudoType pseudoState)
{
    if (InspectorCSSAgent* agent = agents->inspectorCSSAgent())
        return agent->forcePseudoState(element, pseudoState);
    return false;
}

bool shouldPauseDedicatedWorkerOnStartImpl(InstrumentingAgents* agents)
{
    if (InspectorWorkerAgent* agent = agents->inspectorWorkerAgent())
        return agent->shouldPauseDedicatedWorkerOnStart();
    return false;
}

bool shouldForceCORSPreflightImpl(InstrumentingAgents* agents)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        return agent->shouldForceCORSPreflight();
    return false;
}

} // namespace InspectorInstrumentation

} // namespace blink
