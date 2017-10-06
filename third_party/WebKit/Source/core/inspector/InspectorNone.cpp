#include "config.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/frame/ConsoleTypes.h"
#include "core/inspector/ConsoleAPITypes.h"
#include "core/inspector/ScriptCallStack.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

#include "ConsoleMessage.h"
#include "core/InstrumentingAgentsInl.h"
#include "platform/EventTracer.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/GraphicsLayer.h"
#include "core/dom/Element.h"
#include "core/dom/CharacterData.h"
#include "core/dom/Document.h"
#include "core/page/Page.h"
#include "core/css/invalidation/DescendantInvalidationSet.h"
#include "core/css/CSSSelector.h"
#include "core/css/CSSStyleSheet.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/xmlhttprequest/XMLHttpRequest.h"
#include "core/events/Event.h"
#include "core/inspector/ScriptArguments.h"
#include "core/inspector/ScriptAsyncCallStack.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/inspector/WorkerInspectorController.h"
#include "core/inspector/InspectorStateClient.h"
#include "core/inspector/InspectorTaskRunner.h"

namespace blink {

// ScriptArguments::ScriptArguments(ScriptState* scriptState, Vector<ScriptValue>& arguments)
// 	: m_scriptState(scriptState)
// {
// 	notImplemented();
// 	m_arguments.swap(arguments);
// }
// 

// ScriptArguments::~ScriptArguments()
// {
// 	;
// }


//////////////////////////////////////////////////////////////////////////

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorScheduleStyleInvalidationTrackingEvent::attributeChange(Element&, const DescendantInvalidationSet&, const QualifiedName&)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorScheduleStyleInvalidationTrackingEvent::classChange(Element&, const DescendantInvalidationSet&, const AtomicString&)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorScheduleStyleInvalidationTrackingEvent::idChange(Element&, const DescendantInvalidationSet&, const AtomicString&)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorScheduleStyleInvalidationTrackingEvent::pseudoChange(Element&, const DescendantInvalidationSet&, CSSSelector::PseudoType)
{
    notImplemented();
    return nullptr;
}

InspectorInstrumentationCookie::InspectorInstrumentationCookie()
{
}

InspectorInstrumentationCookie::~InspectorInstrumentationCookie()
{
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorLayoutInvalidationTrackingEvent::data(const LayoutObject* layoutObject, LayoutInvalidationReasonForTracing reason)
{
	notImplemented();
	return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorPaintInvalidationTrackingEvent::data(const LayoutObject* layoutObject, const LayoutObject& paintContainer)
{
	notImplemented();
	return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorScrollInvalidationTrackingEvent::data(const LayoutObject& layoutObject)
{
	notImplemented();
	return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorSendRequestEvent::data(unsigned long identifier, LocalFrame* frame, const ResourceRequest& request)
{
	notImplemented();
	return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorReceiveResponseEvent::data(unsigned long identifier, LocalFrame* frame, const ResourceResponse& response)
{
	notImplemented();
	return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorReceiveDataEvent::data(unsigned long identifier, LocalFrame* frame, int encodedDataLength)
{
	notImplemented();
	return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorResourceFinishEvent::data(unsigned long identifier, double finishTime, bool didFail)
{
	notImplemented();
	return nullptr;
}

const char InspectorStyleInvalidatorInvalidateEvent::ElementHasPendingInvalidationList[] = "Element has pending invalidation list";
const char InspectorStyleInvalidatorInvalidateEvent::InvalidateCustomPseudo[] = "Invalidate custom pseudo element";
const char InspectorStyleInvalidatorInvalidateEvent::InvalidationSetMatchedAttribute[] = "Invalidation set matched attribute";
const char InspectorStyleInvalidatorInvalidateEvent::InvalidationSetMatchedClass[] = "Invalidation set matched class";
const char InspectorStyleInvalidatorInvalidateEvent::InvalidationSetMatchedId[] = "Invalidation set matched id";
const char InspectorStyleInvalidatorInvalidateEvent::InvalidationSetMatchedTagName[] = "Invalidation set matched tagName";
const char InspectorStyleInvalidatorInvalidateEvent::PreventStyleSharingForParent[] = "Prevent style sharing for parent";


const char InspectorLayerInvalidationTrackingEvent::SquashingLayerGeometryWasUpdated[] = "Squashing layer geometry was updated";
const char InspectorLayerInvalidationTrackingEvent::AddedToSquashingLayer[] = "The layer may have been added to an already-existing squashing layer";
const char InspectorLayerInvalidationTrackingEvent::RemovedFromSquashingLayer[] = "Removed the layer from a squashing layer";
const char InspectorLayerInvalidationTrackingEvent::ReflectionLayerChanged[] = "Reflection layer change";
const char InspectorLayerInvalidationTrackingEvent::NewCompositedLayer[] = "Assigned a new composited layer";

#if 0
PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorLayerInvalidationTrackingEvent::data(const DeprecatedPaintLayer* layer, const char* reason)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorPaintEvent::data(LayoutObject* layoutObject, const LayoutRect& clipRect, const GraphicsLayer* graphicsLayer)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> frameEventData(LocalFrame* frame)
{
    notImplemented();
    return nullptr;
}


PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorCommitLoadEvent::data(LocalFrame* frame)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorMarkLoadEvent::data(LocalFrame* frame)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorScrollLayerEvent::data(LayoutObject* layoutObject)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorUpdateLayerTreeEvent::data(LocalFrame* frame)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorEvaluateScriptEvent::data(LocalFrame* frame, const String& url, int lineNumber)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorFunctionCallEvent::data(ExecutionContext* context, int scriptId, const String& scriptName, int scriptLine)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorPaintImageEvent::data(const LayoutImage& layoutImage)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorPaintImageEvent::data(const LayoutObject& owningLayoutObject, const StyleImage& styleImage)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorPaintImageEvent::data(const LayoutObject* owningLayoutObject, const ImageResource& imageResource)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorUpdateCountersEvent::data()
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorInvalidateLayoutEvent::data(LocalFrame* frame)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorRecalculateStylesEvent::data(LocalFrame* frame)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorEventDispatchEvent::data(const Event& event)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTimeStampEvent::data(ExecutionContext* context, const String& message)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTracingSessionIdForWorkerEvent::data(const String& sessionId, const String& workerId, WorkerThread* workerThread)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTracingStartedInFrame::data(const String& sessionId, LocalFrame* frame)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorSetLayerTreeId::data(const String& sessionId, int layerTreeId)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorAnimationEvent::data(const Animation& player)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorAnimationStateEvent::data(const Animation& player)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorHitTestEvent::endData(const HitTestRequest& request, const HitTestLocation& location, const HitTestResult& result)
{
    notImplemented();
    return nullptr;
}
#endif

// PassRefPtrWillBeRawPtr<ScriptArguments> ScriptArguments::create(ScriptState* scriptState, Vector<ScriptValue>& arguments)
// {
//     notImplemented();
//     return nullptr;
// }
// 
// bool ScriptArguments::getFirstArgumentAsString(String& result) const
// {
//     notImplemented();
//     return true;
// }

//////////////////////////////////////////////////////////////////////////

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
//     visitor->trace(m_asyncCallTracker);
//     visitor->trace(m_inspectorAnimationAgent);
//     visitor->trace(m_inspectorApplicationCacheAgent);
//     visitor->trace(m_inspectorCSSAgent);
//     visitor->trace(m_inspectorConsoleAgent);
//     visitor->trace(m_inspectorDOMAgent);
//     visitor->trace(m_inspectorDOMDebuggerAgent);
//     visitor->trace(m_inspectorDebuggerAgent);
//     visitor->trace(m_inspectorLayerTreeAgent);
//     visitor->trace(m_inspectorPageAgent);
//     visitor->trace(m_inspectorProfilerAgent);
//     visitor->trace(m_inspectorResourceAgent);
//     visitor->trace(m_inspectorWorkerAgent);
//     visitor->trace(m_pageConsoleAgent);
//     visitor->trace(m_pageDebuggerAgent);
//     visitor->trace(m_pageRuntimeAgent);
}

void InstrumentingAgents::reset()
{
//     m_asyncCallTracker = nullptr;
//     m_inspectorAnimationAgent = nullptr;
//     m_inspectorApplicationCacheAgent = nullptr;
//     m_inspectorCSSAgent = nullptr;
//     m_inspectorConsoleAgent = nullptr;
//     m_inspectorDOMAgent = nullptr;
//     m_inspectorDOMDebuggerAgent = nullptr;
//     m_inspectorDebuggerAgent = nullptr;
//     m_inspectorLayerTreeAgent = nullptr;
//     m_inspectorPageAgent = nullptr;
//     m_inspectorProfilerAgent = nullptr;
//     m_inspectorResourceAgent = nullptr;
//     m_inspectorWorkerAgent = nullptr;
//     m_pageConsoleAgent = nullptr;
//     m_pageDebuggerAgent = nullptr;
//     m_pageRuntimeAgent = nullptr;
}
//////////////////////////////////////////////////////////////////////////

class WorkerGlobalScope;

namespace InspectorInstrumentation {

bool consoleAgentEnabled(ExecutionContext* executionContext)
{
    //notImplemented();
    return false;
}

bool collectingHTMLParseErrorsImpl(InstrumentingAgents* instrumentingAgents)
{
    //notImplemented();
    return false;
}

bool shouldPauseDedicatedWorkerOnStartImpl(InstrumentingAgents*)
{
    return false;
}



#define GEN_instrumentingAgentsFor_FUNC(type) \
InstrumentingAgents* instrumentingAgentsFor(type*) \
{ \
    return nullptr; \
}

#define GEN_instrumentingAgentsForNonDocumentContext_FUNC(type) \
InstrumentingAgents* instrumentingAgentsForNonDocumentContext(type*) \
{ \
    return nullptr; \
}

//GEN_instrumentingAgentsFor_FUNC(Document)
GEN_instrumentingAgentsFor_FUNC(Page)
GEN_instrumentingAgentsForNonDocumentContext_FUNC(ExecutionContext)
GEN_instrumentingAgentsFor_FUNC(EventTarget)
GEN_instrumentingAgentsFor_FUNC(LayoutObject)
GEN_instrumentingAgentsFor_FUNC(LocalFrame)
GEN_instrumentingAgentsFor_FUNC(WorkerGlobalScope)

//////////////////////////////////////////////////////////////////////////
void consoleProfileImpl(InstrumentingAgents*, ExecutionContext*, const String&) { /*notImplemented();*/ }
void consoleProfileEndImpl(InstrumentingAgents*, const String&) { /*notImplemented();*/ }
void consoleTimeImpl(InstrumentingAgents*, ExecutionContext*, const String&) { /*notImplemented();*/ }
bool isDebuggerPausedImpl(InstrumentingAgents*) { /*notImplemented();*/ return false; }
bool shouldForceCORSPreflightImpl(InstrumentingAgents*) { /*notImplemented();*/ return false; }
void didClearDocumentOfWindowObjectImpl(InstrumentingAgents*, LocalFrame*) { /*notImplemented();*/ }
bool forcePseudoStateImpl(InstrumentingAgents*, Element*, CSSSelector::PseudoType) { /*notImplemented();*/ return false; }
void willCloseWindowImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void didPerformElementShadowDistributionImpl(InstrumentingAgents*, Element*) { /*notImplemented();*/ }
void characterDataModifiedImpl(InstrumentingAgents*, CharacterData*) { /*notImplemented();*/ }
void willPaintImpl(InstrumentingAgents*, LayoutObject*, const GraphicsLayer*) { /*notImplemented();*/ }
void didPaintImpl(InstrumentingAgents*, LayoutObject*, const GraphicsLayer*, GraphicsContext*, const LayoutRect&) { /*notImplemented();*/ }
void willPaintImageImpl(InstrumentingAgents*, LayoutImage*) { /*notImplemented();*/ }
void didPaintImageImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void continueWithPolicyIgnoreImpl(LocalFrame*, DocumentLoader*, unsigned long, const ResourceResponse&) { /*notImplemented();*/ }
void didReceiveDataImpl(InstrumentingAgents*, LocalFrame*, unsigned long, const char*, int, int) { /*notImplemented();*/ }
void didFinishLoadingImpl(InstrumentingAgents*, unsigned long, DocumentLoader*, double, int64_t) { /*notImplemented();*/ }
void didReceiveCORSRedirectResponseImpl(InstrumentingAgents*, LocalFrame*, unsigned long, DocumentLoader*, const ResourceResponse&, ResourceLoader*) { /*notImplemented();*/ }
void didFailLoadingImpl(InstrumentingAgents*, unsigned long, const ResourceError&) { /*notImplemented();*/ }
void documentThreadableLoaderStartedLoadingForClientImpl(InstrumentingAgents*, unsigned long, ThreadableLoaderClient*) { /*notImplemented();*/ }
void willLoadXHRImpl(InstrumentingAgents*, XMLHttpRequest*, ThreadableLoaderClient*, const AtomicString&, const KURL&, bool, PassRefPtr<FormData>, const HTTPHeaderMap&, bool) { /*notImplemented();*/ }
void didFailXHRLoadingImpl(InstrumentingAgents*, XMLHttpRequest*, ThreadableLoaderClient*) { /*notImplemented();*/ }
void didFinishXHRLoadingImpl(InstrumentingAgents*, XMLHttpRequest*, ThreadableLoaderClient*, unsigned long, ScriptString, const AtomicString&, const String&, const String&, unsigned) { /*notImplemented();*/ }
void scriptImportedImpl(InstrumentingAgents*, unsigned long, const String&) { /*notImplemented();*/ }
void scriptExecutionBlockedByCSPImpl(InstrumentingAgents*, const String&) { /*notImplemented();*/ }
void didReceiveScriptResponseImpl(InstrumentingAgents*, unsigned long) { /*notImplemented();*/ }
void domContentLoadedEventFiredImpl(InstrumentingAgents*, LocalFrame*) { /*notImplemented();*/ }
void loadEventFiredImpl(InstrumentingAgents*, LocalFrame*) { /*notImplemented();*/ }
void frameAttachedToParentImpl(InstrumentingAgents*, LocalFrame*) { /*notImplemented();*/ }
void frameDetachedFromParentImpl(InstrumentingAgents*, LocalFrame*) { /*notImplemented();*/ }
void didCommitLoadImpl(InstrumentingAgents*, LocalFrame*, DocumentLoader*) { /*notImplemented();*/ }
void frameDocumentUpdatedImpl(InstrumentingAgents*, LocalFrame*) { /*notImplemented();*/ }
void loaderDetachedFromFrameImpl(InstrumentingAgents*, DocumentLoader*) { /*notImplemented();*/ }
void frameStartedLoadingImpl(InstrumentingAgents*, LocalFrame*) { /*notImplemented();*/ }
void frameStoppedLoadingImpl(InstrumentingAgents*, LocalFrame*) { /*notImplemented();*/ }
void frameScheduledNavigationImpl(InstrumentingAgents*, LocalFrame*, double) { /*notImplemented();*/ }
void frameClearedScheduledNavigationImpl(InstrumentingAgents*, LocalFrame*) { /*notImplemented();*/ }
InspectorInstrumentationCookie willRunJavaScriptDialogImpl(InstrumentingAgents*, const String&) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didRunJavaScriptDialogImpl(const InspectorInstrumentationCookie&) { /*notImplemented();*/ }
void willDestroyResourceImpl(Resource*) { /*notImplemented();*/ }
InspectorInstrumentationCookie willWriteHTMLImpl(InstrumentingAgents*, Document*, unsigned) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didWriteHTMLImpl(const InspectorInstrumentationCookie&, unsigned) { /*notImplemented();*/ }
void didRequestAnimationFrameImpl(InstrumentingAgents*, Document*, int) { /*notImplemented();*/ }
void didCancelAnimationFrameImpl(InstrumentingAgents*, Document*, int) { /*notImplemented();*/ }
InspectorInstrumentationCookie willFireAnimationFrameImpl(InstrumentingAgents*, Document*, int) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didFireAnimationFrameImpl(const InspectorInstrumentationCookie&) { /*notImplemented();*/ }
//void didDispatchDOMStorageEventImpl(InstrumentingAgents*, const String&, const String&, const String&, StorageType, SecurityOrigin*) { /*notImplemented();*/ }
void didStartWorkerGlobalScopeImpl(InstrumentingAgents*, WorkerGlobalScopeProxy*, const KURL&) { /*notImplemented();*/ }
void willEvaluateWorkerScriptImpl(InstrumentingAgents*, WorkerGlobalScope*, int) { /*notImplemented();*/ }
void workerGlobalScopeTerminatedImpl(InstrumentingAgents*, WorkerGlobalScopeProxy*) { /*notImplemented();*/ }
void willProcessTaskImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void didProcessTaskImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void willEnterNestedRunLoopImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void didLeaveNestedRunLoopImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void didCreateWebSocketImpl(InstrumentingAgents*, Document*, unsigned long, const KURL&, const String&) { /*notImplemented();*/ }
void willSendWebSocketHandshakeRequestImpl(InstrumentingAgents*, Document*, unsigned long, const WebSocketHandshakeRequest*) { /*notImplemented();*/ }
void didReceiveWebSocketHandshakeResponseImpl(InstrumentingAgents*, Document*, unsigned long, const WebSocketHandshakeRequest*, const WebSocketHandshakeResponse*) { /*notImplemented();*/ }
void didCloseWebSocketImpl(InstrumentingAgents*, Document*, unsigned long) { /*notImplemented();*/ }
void didReceiveWebSocketFrameImpl(InstrumentingAgents*, unsigned long, int, bool, const char*, size_t) { /*notImplemented();*/ }
void didSendWebSocketFrameImpl(InstrumentingAgents*, unsigned long, int, bool, const char*, size_t) { /*notImplemented();*/ }
void didReceiveWebSocketFrameErrorImpl(InstrumentingAgents*, unsigned long, const String&) { /*notImplemented();*/ }
void networkStateChangedImpl(InstrumentingAgents*, bool) { /*notImplemented();*/ }
void updateApplicationCacheStatusImpl(InstrumentingAgents*, LocalFrame*) { /*notImplemented();*/ }
void willUpdateLayerTreeImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void layerTreeDidChangeImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void didUpdateLayerTreeImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void pseudoElementCreatedImpl(InstrumentingAgents*, PseudoElement*) { /*notImplemented();*/ }
void pseudoElementDestroyedImpl(InstrumentingAgents*, PseudoElement*) { /*notImplemented();*/ }
void willExecuteCustomElementCallbackImpl(InstrumentingAgents*, Element*) { /*notImplemented();*/ }
void willSendXMLHttpRequestImpl(InstrumentingAgents*, const String&) { /*notImplemented();*/ }
void didFireWebGLErrorImpl(InstrumentingAgents*, const String&) { /*notImplemented();*/ }
void didFireWebGLWarningImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void didFireWebGLErrorOrWarningImpl(InstrumentingAgents*, const String&) { /*notImplemented();*/ }
void didInstallTimerImpl(InstrumentingAgents*, ExecutionContext*, int, int, bool) { /*notImplemented();*/ }
void didRemoveTimerImpl(InstrumentingAgents*, ExecutionContext*, int) { /*notImplemented();*/ }
InspectorInstrumentationCookie willCallFunctionImpl(InstrumentingAgents*, ExecutionContext*, int, const String&, int) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didCallFunctionImpl(const InspectorInstrumentationCookie&) { /*notImplemented();*/ }
InspectorInstrumentationCookie willDispatchXHRReadyStateChangeEventImpl(InstrumentingAgents*, ExecutionContext*, XMLHttpRequest*) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didDispatchXHRReadyStateChangeEventImpl(const InspectorInstrumentationCookie&) { /*notImplemented();*/ }
InspectorInstrumentationCookie willDispatchEventImpl(InstrumentingAgents*, Document*, const Event&, LocalDOMWindow*, Node*, const EventPath&) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didDispatchEventImpl(const InspectorInstrumentationCookie&) { /*notImplemented();*/ }
void didEnqueueEventImpl(InstrumentingAgents*, EventTarget*, Event*) { /*notImplemented();*/ }
void didRemoveEventImpl(InstrumentingAgents*, EventTarget*, Event*) { /*notImplemented();*/ }
InspectorInstrumentationCookie willHandleEventImpl(InstrumentingAgents*, EventTarget*, Event*, EventListener*, bool) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didHandleEventImpl(const InspectorInstrumentationCookie&) { /*notImplemented();*/ }
InspectorInstrumentationCookie willDispatchEventOnWindowImpl(InstrumentingAgents*, const Event&, LocalDOMWindow*) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didDispatchEventOnWindowImpl(const InspectorInstrumentationCookie&) { /*notImplemented();*/ }
void didEnqueueMutationRecordImpl(InstrumentingAgents*, ExecutionContext*, MutationObserver*) { /*notImplemented();*/ }
void didClearAllMutationRecordsImpl(InstrumentingAgents*, ExecutionContext*, MutationObserver*) { /*notImplemented();*/ }
void willDeliverMutationRecordsImpl(InstrumentingAgents*, ExecutionContext*, MutationObserver*) { /*notImplemented();*/ }
void didDeliverMutationRecordsImpl(InstrumentingAgents*) { /*notImplemented();*/ }
InspectorInstrumentationCookie willEvaluateScriptImpl(InstrumentingAgents*, LocalFrame*, const String&, int) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didEvaluateScriptImpl(const InspectorInstrumentationCookie&) { /*notImplemented();*/ }
void didCreateIsolatedContextImpl(InstrumentingAgents*, LocalFrame*, ScriptState*, SecurityOrigin*) { /*notImplemented();*/ }
InspectorInstrumentationCookie willFireTimerImpl(InstrumentingAgents*, ExecutionContext*, int) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didFireTimerImpl(const InspectorInstrumentationCookie&) { /*notImplemented();*/ }
void didInvalidateLayoutImpl(InstrumentingAgents*, LocalFrame*) { /*notImplemented();*/ }
InspectorInstrumentationCookie willLayoutImpl(InstrumentingAgents*, LocalFrame*) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didLayoutImpl(const InspectorInstrumentationCookie&, LayoutObject*) { /*notImplemented();*/ }
void didScrollImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void didResizeMainFrameImpl(InstrumentingAgents*) { /*notImplemented();*/ }
InspectorInstrumentationCookie willDispatchXHRLoadEventImpl(InstrumentingAgents*, ExecutionContext*, XMLHttpRequest*) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didDispatchXHRLoadEventImpl(const InspectorInstrumentationCookie&) { /*notImplemented();*/ }
void willScrollLayerImpl(InstrumentingAgents*, LayoutObject*) { /*notImplemented();*/ }
void didScrollLayerImpl(InstrumentingAgents*) { /*notImplemented();*/ }
InspectorInstrumentationCookie willRecalculateStyleImpl(InstrumentingAgents*, Document*) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didRecalculateStyleImpl(const InspectorInstrumentationCookie&, int) { /*notImplemented();*/ }
void didScheduleStyleRecalculationImpl(InstrumentingAgents*, Document*) { /*notImplemented();*/ }
void applyUserAgentOverrideImpl(InstrumentingAgents*, String*) { /*notImplemented();*/ }
bool applyViewportStyleOverrideImpl(InstrumentingAgents*, StyleResolver*) { /*notImplemented();*/ return false; }
void applyEmulatedMediaImpl(InstrumentingAgents*, String*) { /*notImplemented();*/ }
void willSendRequestImpl(InstrumentingAgents*, unsigned long, DocumentLoader*, ResourceRequest&, const ResourceResponse&, const FetchInitiatorInfo&) { /*notImplemented();*/ }
void markResourceAsCachedImpl(InstrumentingAgents*, unsigned long) { /*notImplemented();*/ }
void didReceiveResourceResponseImpl(InstrumentingAgents*, LocalFrame*, unsigned long, DocumentLoader*, const ResourceResponse&, ResourceLoader*) { /*notImplemented();*/ }
void continueAfterXFrameOptionsDeniedImpl(LocalFrame*, DocumentLoader*, unsigned long, const ResourceResponse&) { /*notImplemented();*/ }
void continueWithPolicyDownloadImpl(LocalFrame*, DocumentLoader*, unsigned long, const ResourceResponse&) { /*notImplemented();*/ }
//InstrumentingAgents* instrumentingAgentsFor(LocalFrame*) { /*notImplemented();*/ return nullptr; }

void didPushShadowRootImpl(InstrumentingAgents*, Element*, ShadowRoot*) { /*notImplemented();*/ }
void willPopShadowRootImpl(InstrumentingAgents*, Element*, ShadowRoot*) { /*notImplemented();*/ }
void willSetInnerHTMLImpl(InstrumentingAgents*) { /*notImplemented();*/ }
InspectorInstrumentationCookie willCallFunctionImpl(InstrumentingAgents*, ExecutionContext*, const DevToolsFunctionInfo&) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }

void didPostExecutionContextTaskImpl(InstrumentingAgents*, ExecutionContext*, ExecutionContextTask*) { /*notImplemented();*/ }
void didKillAllExecutionContextTasksImpl(InstrumentingAgents*, ExecutionContext*) { /*notImplemented();*/ }
void willPerformExecutionContextTaskImpl(InstrumentingAgents*, ExecutionContext*, ExecutionContextTask*) { /*notImplemented();*/ }
void didPerformExecutionContextTaskImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void didCreateScriptContextImpl(InstrumentingAgents*, LocalFrame*, ScriptState*, SecurityOrigin*, int) { /*notImplemented();*/ }
void willReleaseScriptContextImpl(InstrumentingAgents*, LocalFrame*, ScriptState*) { /*notImplemented();*/ }
void didLayoutImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void didDispatchXHRLoadendEventImpl(InstrumentingAgents*, XMLHttpRequest*) { /*notImplemented();*/ }
void didFinishLoadingImpl(InstrumentingAgents*, unsigned long, double, int64_t) { /*notImplemented();*/ }
void willSendEventSourceRequestImpl(InstrumentingAgents*, ThreadableLoaderClient*) { /*notImplemented();*/ }
void willDispachEventSourceEventImpl(InstrumentingAgents*, ThreadableLoaderClient*, const AtomicString&, const AtomicString&, const Vector<UChar>&) { /*notImplemented();*/ }
void didFinishEventSourceRequestImpl(InstrumentingAgents*, ThreadableLoaderClient*) { /*notImplemented();*/ }
void didFailXHRLoadingImpl(InstrumentingAgents*, ExecutionContext*, XMLHttpRequest*, ThreadableLoaderClient*, const AtomicString&, const String&) { /*notImplemented();*/ }
void didFinishXHRLoadingImpl(InstrumentingAgents*, ExecutionContext*, XMLHttpRequest*, ThreadableLoaderClient*, const AtomicString&, const String&) { /*notImplemented();*/ }
void didStartProvisionalLoadImpl(InstrumentingAgents*, LocalFrame*) { /*notImplemented();*/ }
InspectorInstrumentationCookie willRunJavaScriptDialogImpl(InstrumentingAgents*, const String&, ChromeClient::DialogType) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didRunJavaScriptDialogImpl(const InspectorInstrumentationCookie&, bool) { /*notImplemented();*/ }
void didRequestAnimationFrameImpl(InstrumentingAgents*, ExecutionContext*, int) { /*notImplemented();*/ }
void didCancelAnimationFrameImpl(InstrumentingAgents*, ExecutionContext*, int) { /*notImplemented();*/ }
InspectorInstrumentationCookie willFireAnimationFrameImpl(InstrumentingAgents*, ExecutionContext*, int) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void didStartWorkerImpl(InstrumentingAgents*, WorkerInspectorProxy*, const KURL&) { /*notImplemented();*/ }
void workerTerminatedImpl(InstrumentingAgents*, WorkerInspectorProxy*) { /*notImplemented();*/ }
void networkStateChangedImpl(InstrumentingAgents*, LocalFrame*, bool) { /*notImplemented();*/ }
int traceAsyncOperationStartingImpl(InstrumentingAgents*, ExecutionContext*, const String&) { /*notImplemented();*/ return 0; }
int traceAsyncOperationStartingImpl(InstrumentingAgents*, ExecutionContext*, const String&, int) { /*notImplemented();*/ return 0; }
void traceAsyncOperationCompletedImpl(InstrumentingAgents*, ExecutionContext*, int) { /*notImplemented();*/ }
InspectorInstrumentationCookie traceAsyncOperationCompletedCallbackStartingImpl(InstrumentingAgents*, ExecutionContext*, int) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
InspectorInstrumentationCookie traceAsyncCallbackStartingImpl(InstrumentingAgents*, ExecutionContext*, int) { /*notImplemented();*/ return InspectorInstrumentationCookie(); }
void traceAsyncCallbackCompletedImpl(const InspectorInstrumentationCookie&) { /*notImplemented();*/ }
void didCreateAnimationImpl(InstrumentingAgents*, Animation*) { /*notImplemented();*/ }
void didCancelAnimationImpl(InstrumentingAgents*, Animation*) { /*notImplemented();*/ }
void activeStyleSheetsUpdatedImpl(InstrumentingAgents*, Document*) { /*notImplemented();*/ }

void willInsertDOMNodeImpl(InstrumentingAgents*, Node*) { /*notImplemented();*/ }
void didInsertDOMNodeImpl(InstrumentingAgents*, Node*) { /*notImplemented();*/ }
void willRemoveDOMNodeImpl(InstrumentingAgents*, Node*) { /*notImplemented();*/ }
void documentDetachedImpl(InstrumentingAgents*, Document*) { /*notImplemented();*/ }
void mediaQueryResultChangedImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void didInvalidateStyleAttrImpl(InstrumentingAgents*, Node*) { /*notImplemented();*/ }

void willModifyDOMAttrImpl(InstrumentingAgents*, Element*, const AtomicString&, const AtomicString&) { /*notImplemented();*/ }
void didModifyDOMAttrImpl(InstrumentingAgents*, Element*, const QualifiedName&, const AtomicString&) { /*notImplemented();*/ }
void didRemoveDOMAttrImpl(InstrumentingAgents*, Element*, const QualifiedName&) { /*notImplemented();*/ }
void addMessageToConsoleImpl(InstrumentingAgents*, ConsoleMessage*) { /*notImplemented();*/ }
void consoleMessagesClearedImpl(InstrumentingAgents*) { /*notImplemented();*/ }
void appendAsyncCallStack(ExecutionContext*, ScriptCallStack*) { /*notImplemented();*/ }
int FrontendCounter::s_frontendCounter = 0;

} // InspectorInstrumentation

class InspectorCompositeState;

InspectorAgentRegistry::InspectorAgentRegistry(InstrumentingAgents*, InspectorCompositeState*)
{

}

WorkerInspectorController::WorkerInspectorController(WorkerGlobalScope*)
    : m_agents(nullptr, nullptr)
{

}

WorkerInspectorController::~WorkerInspectorController()
{

}

void WorkerInspectorController::resumeStartup()
{

}

bool WorkerInspectorController::isRunRequired()
{
    return false;
}

void WorkerInspectorController::dispose()
{

}

DEFINE_TRACE(WorkerInspectorController)
{

}


void WorkerInspectorController::connectFrontend()
{

}

void WorkerInspectorController::disconnectFrontend()
{

}


void WorkerInspectorController::dispatchMessageFromFrontend(const String& message)
{

}

void WorkerInspectorController::interruptAndDispatchInspectorCommands()
{

}

void WorkerInspectorController::workerContextInitialized(bool shouldPauseOnStart)
{

}

class InspectorTaskRunner::ThreadSafeTaskQueue {
    WTF_MAKE_NONCOPYABLE(ThreadSafeTaskQueue);
public:
    ThreadSafeTaskQueue() {}
    PassOwnPtr<Task> tryTake()
    {
        MutexLocker lock(m_mutex);
        if (m_queue.isEmpty())
            return nullptr;
        return m_queue.takeFirst();
    }
    void append(PassOwnPtr<Task> task)
    {
        MutexLocker lock(m_mutex);
        m_queue.append(task);
    }
private:
    Mutex m_mutex;
    Deque<OwnPtr<Task>> m_queue;
};

InspectorTaskRunner::~InspectorTaskRunner()
{

}

InspectorTaskRunner::IgnoreInterruptsScope::IgnoreInterruptsScope(InspectorTaskRunner* taskRunner)
    : m_wasIgnoring(taskRunner->m_ignoreInterrupts)
    , m_taskRunner(taskRunner)
{
    // There may be nested scopes e.g. when tasks are being executed on XHR breakpoint.
    m_taskRunner->m_ignoreInterrupts = true;
}

InspectorTaskRunner::IgnoreInterruptsScope::~IgnoreInterruptsScope()
{
    m_taskRunner->m_ignoreInterrupts = m_wasIgnoring;
}

} // namespace blink