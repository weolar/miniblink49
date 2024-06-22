// Code generated from InspectorInstrumentation.idl

#ifndef InspectorInstrumentationInl_h
#define InspectorInstrumentationInl_h

#include "core/dom/PseudoElement.h"
#include "core/inspector/InspectorInstrumentation.h"

namespace blink {



namespace InspectorInstrumentation {

CORE_EXPORT void didClearDocumentOfWindowObjectImpl(InstrumentingAgents*, LocalFrame*);

inline void didClearDocumentOfWindowObject(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didClearDocumentOfWindowObjectImpl(agents, paramLocalFrame);
}

CORE_EXPORT void willCloseWindowImpl(InstrumentingAgents*);

inline void willCloseWindow(ExecutionContext* paramExecutionContext)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        willCloseWindowImpl(agents);
}

CORE_EXPORT void willInsertDOMNodeImpl(InstrumentingAgents*, Node*);

inline void willInsertDOMNode(Node* parent)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(parent))
        willInsertDOMNodeImpl(agents, parent);
}

CORE_EXPORT void didInsertDOMNodeImpl(InstrumentingAgents*, Node*);

inline void didInsertDOMNode(Node* paramNode)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramNode))
        didInsertDOMNodeImpl(agents, paramNode);
}

CORE_EXPORT void willRemoveDOMNodeImpl(InstrumentingAgents*, Node*);

inline void willRemoveDOMNode(Node* paramNode)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramNode))
        willRemoveDOMNodeImpl(agents, paramNode);
}

CORE_EXPORT void willModifyDOMAttrImpl(InstrumentingAgents*, Element*, const AtomicString&, const AtomicString&);

inline void willModifyDOMAttr(Element* paramElement, const AtomicString& oldValue, const AtomicString& newValue)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramElement))
        willModifyDOMAttrImpl(agents, paramElement, oldValue, newValue);
}

CORE_EXPORT void didModifyDOMAttrImpl(InstrumentingAgents*, Element*, const QualifiedName&, const AtomicString&);

inline void didModifyDOMAttr(Element* paramElement, const QualifiedName& name, const AtomicString& value)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramElement))
        didModifyDOMAttrImpl(agents, paramElement, name, value);
}

CORE_EXPORT void didRemoveDOMAttrImpl(InstrumentingAgents*, Element*, const QualifiedName&);

inline void didRemoveDOMAttr(Element* paramElement, const QualifiedName& name)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramElement))
        didRemoveDOMAttrImpl(agents, paramElement, name);
}

CORE_EXPORT void characterDataModifiedImpl(InstrumentingAgents*, CharacterData*);

inline void characterDataModified(CharacterData* paramCharacterData)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramCharacterData))
        characterDataModifiedImpl(agents, paramCharacterData);
}

CORE_EXPORT void didInvalidateStyleAttrImpl(InstrumentingAgents*, Node*);

inline void didInvalidateStyleAttr(Node* paramNode)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramNode))
        didInvalidateStyleAttrImpl(agents, paramNode);
}

CORE_EXPORT void didPerformElementShadowDistributionImpl(InstrumentingAgents*, Element*);

inline void didPerformElementShadowDistribution(Element* paramElement)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramElement))
        didPerformElementShadowDistributionImpl(agents, paramElement);
}

CORE_EXPORT void documentDetachedImpl(InstrumentingAgents*, Document*);

inline void documentDetached(Document* paramDocument)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        documentDetachedImpl(agents, paramDocument);
}

CORE_EXPORT void activeStyleSheetsUpdatedImpl(InstrumentingAgents*, Document*);

inline void activeStyleSheetsUpdated(Document* paramDocument)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        activeStyleSheetsUpdatedImpl(agents, paramDocument);
}

CORE_EXPORT void mediaQueryResultChangedImpl(InstrumentingAgents*);

inline void mediaQueryResultChanged(Document* paramDocument)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        mediaQueryResultChangedImpl(agents);
}

CORE_EXPORT void didPushShadowRootImpl(InstrumentingAgents*, Element*, ShadowRoot*);

inline void didPushShadowRoot(Element* host, ShadowRoot* paramShadowRoot)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(host))
        didPushShadowRootImpl(agents, host, paramShadowRoot);
}

CORE_EXPORT void willPopShadowRootImpl(InstrumentingAgents*, Element*, ShadowRoot*);

inline void willPopShadowRoot(Element* host, ShadowRoot* paramShadowRoot)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(host))
        willPopShadowRootImpl(agents, host, paramShadowRoot);
}

CORE_EXPORT void willSetInnerHTMLImpl(InstrumentingAgents*);

inline void willSetInnerHTML(Element* element)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(element))
        willSetInnerHTMLImpl(agents);
}

CORE_EXPORT void willSendXMLHttpRequestImpl(InstrumentingAgents*, const String&);

inline void willSendXMLHttpRequest(ExecutionContext* paramExecutionContext, const String& url)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        willSendXMLHttpRequestImpl(agents, url);
}

CORE_EXPORT void didFireWebGLErrorImpl(InstrumentingAgents*, const String&);

inline void didFireWebGLError(Element* paramElement, const String& errorName)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramElement))
        didFireWebGLErrorImpl(agents, errorName);
}

CORE_EXPORT void didFireWebGLWarningImpl(InstrumentingAgents*);

inline void didFireWebGLWarning(Element* paramElement)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramElement))
        didFireWebGLWarningImpl(agents);
}

CORE_EXPORT void didFireWebGLErrorOrWarningImpl(InstrumentingAgents*, const String&);

inline void didFireWebGLErrorOrWarning(Element* paramElement, const String& message)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramElement))
        didFireWebGLErrorOrWarningImpl(agents, message);
}

CORE_EXPORT void didInstallTimerImpl(InstrumentingAgents*, ExecutionContext*, int, int, bool);

inline void didInstallTimer(ExecutionContext* paramExecutionContext, int timerId, int timeout, bool singleShot)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didInstallTimerImpl(agents, paramExecutionContext, timerId, timeout, singleShot);
}

CORE_EXPORT void didRemoveTimerImpl(InstrumentingAgents*, ExecutionContext*, int);

inline void didRemoveTimer(ExecutionContext* paramExecutionContext, int timerId)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didRemoveTimerImpl(agents, paramExecutionContext, timerId);
}

CORE_EXPORT InspectorInstrumentationCookie willCallFunctionImpl(InstrumentingAgents*, ExecutionContext*, const DevToolsFunctionInfo&);

inline InspectorInstrumentationCookie willCallFunction(ExecutionContext* paramExecutionContext, const DevToolsFunctionInfo& paramDevToolsFunctionInfo)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        return willCallFunctionImpl(agents, paramExecutionContext, paramDevToolsFunctionInfo);
    return InspectorInstrumentationCookie();
}

CORE_EXPORT void didCallFunctionImpl(const InspectorInstrumentationCookie&);

inline void didCallFunction(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didCallFunctionImpl(paramInspectorInstrumentationCookie);
}

CORE_EXPORT void didEnqueueEventImpl(InstrumentingAgents*, EventTarget*, Event*);

inline void didEnqueueEvent(EventTarget* paramEventTarget, Event* paramEvent)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramEventTarget))
        didEnqueueEventImpl(agents, paramEventTarget, paramEvent);
}

CORE_EXPORT void didRemoveEventImpl(InstrumentingAgents*, EventTarget*, Event*);

inline void didRemoveEvent(EventTarget* paramEventTarget, Event* paramEvent)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramEventTarget))
        didRemoveEventImpl(agents, paramEventTarget, paramEvent);
}

CORE_EXPORT InspectorInstrumentationCookie willHandleEventImpl(InstrumentingAgents*, EventTarget*, Event*, EventListener*, bool);

inline InspectorInstrumentationCookie willHandleEvent(EventTarget* paramEventTarget, Event* paramEvent, EventListener* listener, bool useCapture)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramEventTarget))
        return willHandleEventImpl(agents, paramEventTarget, paramEvent, listener, useCapture);
    return InspectorInstrumentationCookie();
}

CORE_EXPORT void didHandleEventImpl(const InspectorInstrumentationCookie&);

inline void didHandleEvent(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didHandleEventImpl(paramInspectorInstrumentationCookie);
}

CORE_EXPORT void didEnqueueMutationRecordImpl(InstrumentingAgents*, ExecutionContext*, MutationObserver*);

inline void didEnqueueMutationRecord(ExecutionContext* paramExecutionContext, MutationObserver* paramMutationObserver)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didEnqueueMutationRecordImpl(agents, paramExecutionContext, paramMutationObserver);
}

CORE_EXPORT void didClearAllMutationRecordsImpl(InstrumentingAgents*, ExecutionContext*, MutationObserver*);

inline void didClearAllMutationRecords(ExecutionContext* paramExecutionContext, MutationObserver* paramMutationObserver)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didClearAllMutationRecordsImpl(agents, paramExecutionContext, paramMutationObserver);
}

CORE_EXPORT void willDeliverMutationRecordsImpl(InstrumentingAgents*, ExecutionContext*, MutationObserver*);

inline void willDeliverMutationRecords(ExecutionContext* paramExecutionContext, MutationObserver* paramMutationObserver)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        willDeliverMutationRecordsImpl(agents, paramExecutionContext, paramMutationObserver);
}

CORE_EXPORT void didDeliverMutationRecordsImpl(InstrumentingAgents*);

inline void didDeliverMutationRecords(ExecutionContext* paramExecutionContext)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didDeliverMutationRecordsImpl(agents);
}

CORE_EXPORT void didPostExecutionContextTaskImpl(InstrumentingAgents*, ExecutionContext*, ExecutionContextTask*);

inline void didPostExecutionContextTask(ExecutionContext* paramExecutionContext, ExecutionContextTask* paramExecutionContextTask)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didPostExecutionContextTaskImpl(agents, paramExecutionContext, paramExecutionContextTask);
}

CORE_EXPORT void didKillAllExecutionContextTasksImpl(InstrumentingAgents*, ExecutionContext*);

inline void didKillAllExecutionContextTasks(ExecutionContext* paramExecutionContext)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didKillAllExecutionContextTasksImpl(agents, paramExecutionContext);
}

CORE_EXPORT void willPerformExecutionContextTaskImpl(InstrumentingAgents*, ExecutionContext*, ExecutionContextTask*);

inline void willPerformExecutionContextTask(ExecutionContext* paramExecutionContext, ExecutionContextTask* paramExecutionContextTask)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        willPerformExecutionContextTaskImpl(agents, paramExecutionContext, paramExecutionContextTask);
}

CORE_EXPORT void didPerformExecutionContextTaskImpl(InstrumentingAgents*);

inline void didPerformExecutionContextTask(ExecutionContext* paramExecutionContext)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didPerformExecutionContextTaskImpl(agents);
}

CORE_EXPORT InspectorInstrumentationCookie willEvaluateScriptImpl(InstrumentingAgents*, LocalFrame*, const String&, int);

inline InspectorInstrumentationCookie willEvaluateScript(LocalFrame* paramLocalFrame, const String& url, int lineNumber)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        return willEvaluateScriptImpl(agents, paramLocalFrame, url, lineNumber);
    return InspectorInstrumentationCookie();
}

CORE_EXPORT void didEvaluateScriptImpl(const InspectorInstrumentationCookie&);

inline void didEvaluateScript(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didEvaluateScriptImpl(paramInspectorInstrumentationCookie);
}

CORE_EXPORT void didCreateScriptContextImpl(InstrumentingAgents*, LocalFrame*, ScriptState*, SecurityOrigin*, int);

inline void didCreateScriptContext(LocalFrame* paramLocalFrame, ScriptState* paramScriptState, SecurityOrigin* paramSecurityOrigin, int worldId)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didCreateScriptContextImpl(agents, paramLocalFrame, paramScriptState, paramSecurityOrigin, worldId);
}

CORE_EXPORT void willReleaseScriptContextImpl(InstrumentingAgents*, LocalFrame*, ScriptState*);

inline void willReleaseScriptContext(LocalFrame* paramLocalFrame, ScriptState* paramScriptState)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        willReleaseScriptContextImpl(agents, paramLocalFrame, paramScriptState);
}

CORE_EXPORT InspectorInstrumentationCookie willFireTimerImpl(InstrumentingAgents*, ExecutionContext*, int);

inline InspectorInstrumentationCookie willFireTimer(ExecutionContext* paramExecutionContext, int timerId)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        return willFireTimerImpl(agents, paramExecutionContext, timerId);
    return InspectorInstrumentationCookie();
}

CORE_EXPORT void didFireTimerImpl(const InspectorInstrumentationCookie&);

inline void didFireTimer(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didFireTimerImpl(paramInspectorInstrumentationCookie);
}

CORE_EXPORT void didLayoutImpl(InstrumentingAgents*);

inline void didLayout(LocalFrame* paramLocalFrame)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didLayoutImpl(agents);
}

CORE_EXPORT void didScrollImpl(InstrumentingAgents*);

inline void didScroll(LocalFrame* paramLocalFrame)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didScrollImpl(agents);
}

CORE_EXPORT void didResizeMainFrameImpl(InstrumentingAgents*);

inline void didResizeMainFrame(LocalFrame* paramLocalFrame)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didResizeMainFrameImpl(agents);
}

CORE_EXPORT void didDispatchXHRLoadendEventImpl(InstrumentingAgents*, XMLHttpRequest*);

inline void didDispatchXHRLoadendEvent(ExecutionContext* paramExecutionContext, XMLHttpRequest* paramXMLHttpRequest)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didDispatchXHRLoadendEventImpl(agents, paramXMLHttpRequest);
}

CORE_EXPORT void didPaintImpl(InstrumentingAgents*, LayoutObject*, const GraphicsLayer*, GraphicsContext*, const LayoutRect&);

inline void didPaint(LayoutObject* paramLayoutObject, const GraphicsLayer* paramGraphicsLayer, GraphicsContext* paramGraphicsContext, const LayoutRect& paramLayoutRect)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLayoutObject))
        didPaintImpl(agents, paramLayoutObject, paramGraphicsLayer, paramGraphicsContext, paramLayoutRect);
}

CORE_EXPORT InspectorInstrumentationCookie willRecalculateStyleImpl(InstrumentingAgents*, Document*);

inline InspectorInstrumentationCookie willRecalculateStyle(Document* paramDocument)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        return willRecalculateStyleImpl(agents, paramDocument);
    return InspectorInstrumentationCookie();
}

CORE_EXPORT void didRecalculateStyleImpl(const InspectorInstrumentationCookie&, int);

inline void didRecalculateStyle(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie, int elementCount)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didRecalculateStyleImpl(paramInspectorInstrumentationCookie, elementCount);
}

CORE_EXPORT void didScheduleStyleRecalculationImpl(InstrumentingAgents*, Document*);

inline void didScheduleStyleRecalculation(Document* paramDocument)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didScheduleStyleRecalculationImpl(agents, paramDocument);
}

CORE_EXPORT void applyUserAgentOverrideImpl(InstrumentingAgents*, String*);

inline void applyUserAgentOverride(LocalFrame* paramLocalFrame, String* userAgent)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        applyUserAgentOverrideImpl(agents, userAgent);
}

CORE_EXPORT void willSendRequestImpl(InstrumentingAgents*, unsigned long, DocumentLoader*, ResourceRequest&, const ResourceResponse&, const FetchInitiatorInfo&);

inline void willSendRequest(LocalFrame* paramLocalFrame, unsigned long identifier, DocumentLoader* paramDocumentLoader, ResourceRequest& paramResourceRequest, const ResourceResponse& redirectResponse, const FetchInitiatorInfo& paramFetchInitiatorInfo)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        willSendRequestImpl(agents, identifier, paramDocumentLoader, paramResourceRequest, redirectResponse, paramFetchInitiatorInfo);
}

CORE_EXPORT void markResourceAsCachedImpl(InstrumentingAgents*, unsigned long);

inline void markResourceAsCached(LocalFrame* paramLocalFrame, unsigned long identifier)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        markResourceAsCachedImpl(agents, identifier);
}

CORE_EXPORT void didReceiveResourceResponseImpl(InstrumentingAgents*, LocalFrame*, unsigned long, DocumentLoader*, const ResourceResponse&, ResourceLoader*);

inline void didReceiveResourceResponse(LocalFrame* paramLocalFrame, unsigned long identifier, DocumentLoader* paramDocumentLoader, const ResourceResponse& paramResourceResponse, ResourceLoader* paramResourceLoader)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didReceiveResourceResponseImpl(agents, paramLocalFrame, identifier, paramDocumentLoader, paramResourceResponse, paramResourceLoader);
}

CORE_EXPORT void continueAfterXFrameOptionsDeniedImpl(LocalFrame*, DocumentLoader*, unsigned long, const ResourceResponse&);

inline void continueAfterXFrameOptionsDenied(LocalFrame* frame, DocumentLoader* loader, unsigned long identifier, const ResourceResponse& r)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    continueAfterXFrameOptionsDeniedImpl(frame, loader, identifier, r);
}

CORE_EXPORT void continueWithPolicyIgnoreImpl(LocalFrame*, DocumentLoader*, unsigned long, const ResourceResponse&);

inline void continueWithPolicyIgnore(LocalFrame* frame, DocumentLoader* loader, unsigned long identifier, const ResourceResponse& r)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    continueWithPolicyIgnoreImpl(frame, loader, identifier, r);
}

CORE_EXPORT void didReceiveDataImpl(InstrumentingAgents*, LocalFrame*, unsigned long, const char*, int, int);

inline void didReceiveData(LocalFrame* paramLocalFrame, unsigned long identifier, const char* data, int dataLength, int encodedDataLength)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didReceiveDataImpl(agents, paramLocalFrame, identifier, data, dataLength, encodedDataLength);
}

CORE_EXPORT void didFinishLoadingImpl(InstrumentingAgents*, unsigned long, double, int64_t);

inline void didFinishLoading(LocalFrame* frame, unsigned long identifier, double finishTime, int64_t encodedDataLength)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(frame))
        didFinishLoadingImpl(agents, identifier, finishTime, encodedDataLength);
}

CORE_EXPORT void didReceiveCORSRedirectResponseImpl(InstrumentingAgents*, LocalFrame*, unsigned long, DocumentLoader*, const ResourceResponse&, ResourceLoader*);

inline void didReceiveCORSRedirectResponse(LocalFrame* paramLocalFrame, unsigned long identifier, DocumentLoader* paramDocumentLoader, const ResourceResponse& paramResourceResponse, ResourceLoader* paramResourceLoader)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didReceiveCORSRedirectResponseImpl(agents, paramLocalFrame, identifier, paramDocumentLoader, paramResourceResponse, paramResourceLoader);
}

CORE_EXPORT void didFailLoadingImpl(InstrumentingAgents*, unsigned long, const ResourceError&);

inline void didFailLoading(LocalFrame* frame, unsigned long identifier, const ResourceError& paramResourceError)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(frame))
        didFailLoadingImpl(agents, identifier, paramResourceError);
}

CORE_EXPORT void documentThreadableLoaderStartedLoadingForClientImpl(InstrumentingAgents*, unsigned long, ThreadableLoaderClient*);

inline void documentThreadableLoaderStartedLoadingForClient(ExecutionContext* paramExecutionContext, unsigned long identifier, ThreadableLoaderClient* client)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        documentThreadableLoaderStartedLoadingForClientImpl(agents, identifier, client);
}

CORE_EXPORT void willSendEventSourceRequestImpl(InstrumentingAgents*, ThreadableLoaderClient*);

inline void willSendEventSourceRequest(ExecutionContext* paramExecutionContext, ThreadableLoaderClient* eventSource)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        willSendEventSourceRequestImpl(agents, eventSource);
}

CORE_EXPORT void willDispachEventSourceEventImpl(InstrumentingAgents*, ThreadableLoaderClient*, const AtomicString&, const AtomicString&, const Vector<UChar>&);

inline void willDispachEventSourceEvent(ExecutionContext* paramExecutionContext, ThreadableLoaderClient* eventSource, const AtomicString& eventName, const AtomicString& eventId, const Vector<UChar>& data)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        willDispachEventSourceEventImpl(agents, eventSource, eventName, eventId, data);
}

CORE_EXPORT void didFinishEventSourceRequestImpl(InstrumentingAgents*, ThreadableLoaderClient*);

inline void didFinishEventSourceRequest(ExecutionContext* paramExecutionContext, ThreadableLoaderClient* eventSource)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didFinishEventSourceRequestImpl(agents, eventSource);
}

CORE_EXPORT void willLoadXHRImpl(InstrumentingAgents*, XMLHttpRequest*, ThreadableLoaderClient*, const AtomicString&, const KURL&, bool, PassRefPtr<FormData>, const HTTPHeaderMap&, bool);

inline void willLoadXHR(ExecutionContext* paramExecutionContext, XMLHttpRequest* xhr, ThreadableLoaderClient* client, const AtomicString& method, const KURL& url, bool async, PassRefPtr<FormData> prpParamFormData, const HTTPHeaderMap& headers, bool includeCredentials)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        willLoadXHRImpl(agents, xhr, client, method, url, async, prpParamFormData, headers, includeCredentials);
}

CORE_EXPORT void didFailXHRLoadingImpl(InstrumentingAgents*, ExecutionContext*, XMLHttpRequest*, ThreadableLoaderClient*, const AtomicString&, const String&);

inline void didFailXHRLoading(ExecutionContext* paramExecutionContext, XMLHttpRequest* xhr, ThreadableLoaderClient* client, const AtomicString& method, const String& url)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didFailXHRLoadingImpl(agents, paramExecutionContext, xhr, client, method, url);
}

CORE_EXPORT void didFinishXHRLoadingImpl(InstrumentingAgents*, ExecutionContext*, XMLHttpRequest*, ThreadableLoaderClient*, const AtomicString&, const String&);

inline void didFinishXHRLoading(ExecutionContext* paramExecutionContext, XMLHttpRequest* xhr, ThreadableLoaderClient* client, const AtomicString& method, const String& url)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didFinishXHRLoadingImpl(agents, paramExecutionContext, xhr, client, method, url);
}

CORE_EXPORT void scriptImportedImpl(InstrumentingAgents*, unsigned long, const String&);

inline void scriptImported(ExecutionContext* paramExecutionContext, unsigned long identifier, const String& sourceString)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        scriptImportedImpl(agents, identifier, sourceString);
}

CORE_EXPORT void scriptExecutionBlockedByCSPImpl(InstrumentingAgents*, const String&);

inline void scriptExecutionBlockedByCSP(ExecutionContext* paramExecutionContext, const String& directiveText)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        scriptExecutionBlockedByCSPImpl(agents, directiveText);
}

CORE_EXPORT void didReceiveScriptResponseImpl(InstrumentingAgents*, unsigned long);

inline void didReceiveScriptResponse(ExecutionContext* paramExecutionContext, unsigned long identifier)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didReceiveScriptResponseImpl(agents, identifier);
}

CORE_EXPORT void didStartProvisionalLoadImpl(InstrumentingAgents*, LocalFrame*);

inline void didStartProvisionalLoad(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didStartProvisionalLoadImpl(agents, paramLocalFrame);
}

CORE_EXPORT void domContentLoadedEventFiredImpl(InstrumentingAgents*, LocalFrame*);

inline void domContentLoadedEventFired(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        domContentLoadedEventFiredImpl(agents, paramLocalFrame);
}

CORE_EXPORT void loadEventFiredImpl(InstrumentingAgents*, LocalFrame*);

inline void loadEventFired(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        loadEventFiredImpl(agents, paramLocalFrame);
}

CORE_EXPORT void frameAttachedToParentImpl(InstrumentingAgents*, LocalFrame*);

inline void frameAttachedToParent(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameAttachedToParentImpl(agents, paramLocalFrame);
}

CORE_EXPORT void frameDetachedFromParentImpl(InstrumentingAgents*, LocalFrame*);

inline void frameDetachedFromParent(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameDetachedFromParentImpl(agents, paramLocalFrame);
}

CORE_EXPORT void didCommitLoadImpl(InstrumentingAgents*, LocalFrame*, DocumentLoader*);

inline void didCommitLoad(LocalFrame* paramLocalFrame, DocumentLoader* paramDocumentLoader)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didCommitLoadImpl(agents, paramLocalFrame, paramDocumentLoader);
}

CORE_EXPORT void frameDocumentUpdatedImpl(InstrumentingAgents*, LocalFrame*);

inline void frameDocumentUpdated(LocalFrame* paramLocalFrame)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameDocumentUpdatedImpl(agents, paramLocalFrame);
}

CORE_EXPORT void frameStartedLoadingImpl(InstrumentingAgents*, LocalFrame*);

inline void frameStartedLoading(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameStartedLoadingImpl(agents, paramLocalFrame);
}

CORE_EXPORT void frameStoppedLoadingImpl(InstrumentingAgents*, LocalFrame*);

inline void frameStoppedLoading(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameStoppedLoadingImpl(agents, paramLocalFrame);
}

CORE_EXPORT void frameScheduledNavigationImpl(InstrumentingAgents*, LocalFrame*, double);

inline void frameScheduledNavigation(LocalFrame* paramLocalFrame, double delay)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameScheduledNavigationImpl(agents, paramLocalFrame, delay);
}

CORE_EXPORT void frameClearedScheduledNavigationImpl(InstrumentingAgents*, LocalFrame*);

inline void frameClearedScheduledNavigation(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameClearedScheduledNavigationImpl(agents, paramLocalFrame);
}

CORE_EXPORT InspectorInstrumentationCookie willRunJavaScriptDialogImpl(InstrumentingAgents*, const String&, ChromeClient::DialogType);

inline InspectorInstrumentationCookie willRunJavaScriptDialog(LocalFrame* paramLocalFrame, const String& message, ChromeClient::DialogType dialogType)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        return willRunJavaScriptDialogImpl(agents, message, dialogType);
    return InspectorInstrumentationCookie();
}

CORE_EXPORT void didRunJavaScriptDialogImpl(const InspectorInstrumentationCookie&, bool);

inline void didRunJavaScriptDialog(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie, bool result)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didRunJavaScriptDialogImpl(paramInspectorInstrumentationCookie, result);
}

CORE_EXPORT void willDestroyResourceImpl(Resource*);

inline void willDestroyResource(Resource* cachedResource)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    willDestroyResourceImpl(cachedResource);
}

CORE_EXPORT void didRequestAnimationFrameImpl(InstrumentingAgents*, ExecutionContext*, int);

inline void didRequestAnimationFrame(ExecutionContext* paramExecutionContext, int callbackId)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didRequestAnimationFrameImpl(agents, paramExecutionContext, callbackId);
}

CORE_EXPORT void didCancelAnimationFrameImpl(InstrumentingAgents*, ExecutionContext*, int);

inline void didCancelAnimationFrame(ExecutionContext* paramExecutionContext, int callbackId)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didCancelAnimationFrameImpl(agents, paramExecutionContext, callbackId);
}

CORE_EXPORT InspectorInstrumentationCookie willFireAnimationFrameImpl(InstrumentingAgents*, ExecutionContext*, int);

inline InspectorInstrumentationCookie willFireAnimationFrame(ExecutionContext* paramExecutionContext, int callbackId)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        return willFireAnimationFrameImpl(agents, paramExecutionContext, callbackId);
    return InspectorInstrumentationCookie();
}

CORE_EXPORT void didFireAnimationFrameImpl(const InspectorInstrumentationCookie&);

inline void didFireAnimationFrame(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didFireAnimationFrameImpl(paramInspectorInstrumentationCookie);
}

CORE_EXPORT void didStartWorkerImpl(InstrumentingAgents*, WorkerInspectorProxy*, const KURL&);

inline void didStartWorker(ExecutionContext* paramExecutionContext, WorkerInspectorProxy* proxy, const KURL& url)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didStartWorkerImpl(agents, proxy, url);
}

CORE_EXPORT void workerTerminatedImpl(InstrumentingAgents*, WorkerInspectorProxy*);

inline void workerTerminated(ExecutionContext* paramExecutionContext, WorkerInspectorProxy* proxy)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        workerTerminatedImpl(agents, proxy);
}

CORE_EXPORT void willProcessTaskImpl(InstrumentingAgents*);

inline void willProcessTask(WorkerGlobalScope* context)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        willProcessTaskImpl(agents);
}

CORE_EXPORT void didProcessTaskImpl(InstrumentingAgents*);

inline void didProcessTask(WorkerGlobalScope* context)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        didProcessTaskImpl(agents);
}

CORE_EXPORT void willEnterNestedRunLoopImpl(InstrumentingAgents*);

inline void willEnterNestedRunLoop(WorkerGlobalScope* context)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        willEnterNestedRunLoopImpl(agents);
}

CORE_EXPORT void didLeaveNestedRunLoopImpl(InstrumentingAgents*);

inline void didLeaveNestedRunLoop(WorkerGlobalScope* context)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        didLeaveNestedRunLoopImpl(agents);
}

CORE_EXPORT void didCreateWebSocketImpl(InstrumentingAgents*, Document*, unsigned long, const KURL&, const String&);

inline void didCreateWebSocket(Document* paramDocument, unsigned long identifier, const KURL& requestURL, const String& protocol)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didCreateWebSocketImpl(agents, paramDocument, identifier, requestURL, protocol);
}

CORE_EXPORT void willSendWebSocketHandshakeRequestImpl(InstrumentingAgents*, Document*, unsigned long, const WebSocketHandshakeRequest*);

inline void willSendWebSocketHandshakeRequest(Document* paramDocument, unsigned long identifier, const WebSocketHandshakeRequest* request)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        willSendWebSocketHandshakeRequestImpl(agents, paramDocument, identifier, request);
}

CORE_EXPORT void didReceiveWebSocketHandshakeResponseImpl(InstrumentingAgents*, Document*, unsigned long, const WebSocketHandshakeRequest*, const WebSocketHandshakeResponse*);

inline void didReceiveWebSocketHandshakeResponse(Document* paramDocument, unsigned long identifier, const WebSocketHandshakeRequest* request, const WebSocketHandshakeResponse* response)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didReceiveWebSocketHandshakeResponseImpl(agents, paramDocument, identifier, request, response);
}

CORE_EXPORT void didCloseWebSocketImpl(InstrumentingAgents*, Document*, unsigned long);

inline void didCloseWebSocket(Document* paramDocument, unsigned long identifier)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didCloseWebSocketImpl(agents, paramDocument, identifier);
}

CORE_EXPORT void didReceiveWebSocketFrameImpl(InstrumentingAgents*, unsigned long, int, bool, const char*, size_t);

inline void didReceiveWebSocketFrame(Document* paramDocument, unsigned long identifier, int opCode, bool masked, const char* payload, size_t payloadLength)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didReceiveWebSocketFrameImpl(agents, identifier, opCode, masked, payload, payloadLength);
}

CORE_EXPORT void didSendWebSocketFrameImpl(InstrumentingAgents*, unsigned long, int, bool, const char*, size_t);

inline void didSendWebSocketFrame(Document* paramDocument, unsigned long identifier, int opCode, bool masked, const char* payload, size_t payloadLength)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didSendWebSocketFrameImpl(agents, identifier, opCode, masked, payload, payloadLength);
}

CORE_EXPORT void didReceiveWebSocketFrameErrorImpl(InstrumentingAgents*, unsigned long, const String&);

inline void didReceiveWebSocketFrameError(Document* paramDocument, unsigned long identifier, const String& errorMessage)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didReceiveWebSocketFrameErrorImpl(agents, identifier, errorMessage);
}

CORE_EXPORT void networkStateChangedImpl(InstrumentingAgents*, LocalFrame*, bool);

inline void networkStateChanged(LocalFrame* paramLocalFrame, bool online)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        networkStateChangedImpl(agents, paramLocalFrame, online);
}

CORE_EXPORT void updateApplicationCacheStatusImpl(InstrumentingAgents*, LocalFrame*);

inline void updateApplicationCacheStatus(LocalFrame* paramLocalFrame)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        updateApplicationCacheStatusImpl(agents, paramLocalFrame);
}

CORE_EXPORT void layerTreeDidChangeImpl(InstrumentingAgents*);

inline void layerTreeDidChange(LocalFrame* paramLocalFrame)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        layerTreeDidChangeImpl(agents);
}

CORE_EXPORT void pseudoElementCreatedImpl(InstrumentingAgents*, PseudoElement*);

inline void pseudoElementCreated(PseudoElement* paramPseudoElement)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramPseudoElement))
        pseudoElementCreatedImpl(agents, paramPseudoElement);
}

CORE_EXPORT void pseudoElementDestroyedImpl(InstrumentingAgents*, PseudoElement*);

inline void pseudoElementDestroyed(PseudoElement* paramPseudoElement)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramPseudoElement))
        pseudoElementDestroyedImpl(agents, paramPseudoElement);
}

CORE_EXPORT int traceAsyncOperationStartingImpl(InstrumentingAgents*, ExecutionContext*, const String&);

inline int traceAsyncOperationStarting(ExecutionContext* paramExecutionContext, const String& operationName)
{   
    FAST_RETURN_IF_NO_FRONTENDS(0);
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        return traceAsyncOperationStartingImpl(agents, paramExecutionContext, operationName);
    return 0;
}

CORE_EXPORT int traceAsyncOperationStartingImpl(InstrumentingAgents*, ExecutionContext*, const String&, int);

inline int traceAsyncOperationStarting(ExecutionContext* paramExecutionContext, const String& operationName, int prevOperationId)
{   
    FAST_RETURN_IF_NO_FRONTENDS(0);
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        return traceAsyncOperationStartingImpl(agents, paramExecutionContext, operationName, prevOperationId);
    return 0;
}

CORE_EXPORT void traceAsyncOperationCompletedImpl(InstrumentingAgents*, ExecutionContext*, int);

inline void traceAsyncOperationCompleted(ExecutionContext* paramExecutionContext, int operationId)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        traceAsyncOperationCompletedImpl(agents, paramExecutionContext, operationId);
}

CORE_EXPORT InspectorInstrumentationCookie traceAsyncOperationCompletedCallbackStartingImpl(InstrumentingAgents*, ExecutionContext*, int);

inline InspectorInstrumentationCookie traceAsyncOperationCompletedCallbackStarting(ExecutionContext* paramExecutionContext, int operationId)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        return traceAsyncOperationCompletedCallbackStartingImpl(agents, paramExecutionContext, operationId);
    return InspectorInstrumentationCookie();
}

CORE_EXPORT InspectorInstrumentationCookie traceAsyncCallbackStartingImpl(InstrumentingAgents*, ExecutionContext*, int);

inline InspectorInstrumentationCookie traceAsyncCallbackStarting(ExecutionContext* paramExecutionContext, int operationId)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        return traceAsyncCallbackStartingImpl(agents, paramExecutionContext, operationId);
    return InspectorInstrumentationCookie();
}

CORE_EXPORT void traceAsyncCallbackCompletedImpl(const InspectorInstrumentationCookie&);

inline void traceAsyncCallbackCompleted(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        traceAsyncCallbackCompletedImpl(paramInspectorInstrumentationCookie);
}

CORE_EXPORT void didCreateAnimationImpl(InstrumentingAgents*, Animation*);

inline void didCreateAnimation(Document* paramDocument, Animation* player)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didCreateAnimationImpl(agents, player);
}

CORE_EXPORT void didCancelAnimationImpl(InstrumentingAgents*, Animation*);

inline void didCancelAnimation(Document* paramDocument, Animation* player)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didCancelAnimationImpl(agents, player);
}

} // namespace InspectorInstrumentation

} // namespace blink

#endif // !defined(InspectorInstrumentationInl_h)
