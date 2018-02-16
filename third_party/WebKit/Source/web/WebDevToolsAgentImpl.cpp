/*
 * Copyright (C) 2010-2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "web/WebDevToolsAgentImpl.h"

#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/V8Binding.h"
#include "core/InspectorBackendDispatcher.h"
#include "core/InspectorFrontend.h"
#include "core/frame/FrameConsole.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/inspector/AsyncCallTracker.h"
#include "core/inspector/IdentifiersFactory.h"
#include "core/inspector/InjectedScriptHost.h"
#include "core/inspector/InjectedScriptManager.h"
#include "core/inspector/InspectorAnimationAgent.h"
#include "core/inspector/InspectorApplicationCacheAgent.h"
#include "core/inspector/InspectorCSSAgent.h"
#include "core/inspector/InspectorDOMAgent.h"
#include "core/inspector/InspectorDOMDebuggerAgent.h"
#include "core/inspector/InspectorDebuggerAgent.h"
#include "core/inspector/InspectorHeapProfilerAgent.h"
#include "core/inspector/InspectorInputAgent.h"
#include "core/inspector/InspectorInspectorAgent.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorLayerTreeAgent.h"
#include "core/inspector/InspectorMemoryAgent.h"
#include "core/inspector/InspectorOverlay.h"
#include "core/inspector/InspectorPageAgent.h"
#include "core/inspector/InspectorProfilerAgent.h"
#include "core/inspector/InspectorResourceAgent.h"
#include "core/inspector/InspectorResourceContentLoader.h"
#include "core/inspector/InspectorState.h"
#include "core/inspector/InspectorTaskRunner.h"
#include "core/inspector/InspectorTimelineAgent.h"
#include "core/inspector/InspectorTracingAgent.h"
#include "core/inspector/InspectorWorkerAgent.h"
#include "core/inspector/InstrumentingAgents.h"
#include "core/inspector/LayoutEditor.h"
#include "core/inspector/MainThreadDebugger.h"
#include "core/inspector/PageConsoleAgent.h"
#include "core/inspector/PageDebuggerAgent.h"
#include "core/inspector/PageRuntimeAgent.h"
#include "core/layout/LayoutView.h"
#include "core/page/FocusController.h"
#include "core/page/Page.h"
#include "modules/accessibility/InspectorAccessibilityAgent.h"
#include "modules/cachestorage/InspectorCacheStorageAgent.h"
#include "modules/device_orientation/DeviceOrientationInspectorAgent.h"
#include "modules/filesystem/InspectorFileSystemAgent.h"
#include "modules/indexeddb/InspectorIndexedDBAgent.h"
#include "modules/screen_orientation/ScreenOrientationInspectorAgent.h"
#include "modules/storage/InspectorDOMStorageAgent.h"
#include "modules/webdatabase/InspectorDatabaseAgent.h"
#include "platform/JSONValues.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include "public/platform/Platform.h"
#include "public/platform/WebLayerTreeView.h"
#include "public/platform/WebRect.h"
#include "public/platform/WebString.h"
#include "public/web/WebDevToolsAgentClient.h"
#include "public/web/WebSettings.h"
#include "web/DevToolsEmulator.h"
#include "web/InspectorEmulationAgent.h"
#include "web/InspectorRenderingAgent.h"
#include "web/WebFrameWidgetImpl.h"
#include "web/WebInputEventConversion.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebSettingsImpl.h"
#include "web/WebViewImpl.h"
#include "wtf/MathExtras.h"
#include "wtf/Noncopyable.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ClientMessageLoopAdapter : public MainThreadDebugger::ClientMessageLoop {
public:
    ~ClientMessageLoopAdapter() override
    {
        s_instance = nullptr;
    }

    static void ensureMainThreadDebuggerCreated(WebDevToolsAgentClient* client)
    {
        if (s_instance)
            return;
        OwnPtr<ClientMessageLoopAdapter> instance = adoptPtr(new ClientMessageLoopAdapter(adoptPtr(client->createClientMessageLoop())));
        s_instance = instance.get();
        v8::Isolate* isolate = V8PerIsolateData::mainThreadIsolate();
        V8PerIsolateData* data = V8PerIsolateData::from(isolate);
        data->setScriptDebugger(MainThreadDebugger::create(instance.release(), isolate));
    }

    static void webViewImplClosed(WebViewImpl* view)
    {
        if (s_instance)
            s_instance->m_frozenViews.remove(view);
    }

    static void webFrameWidgetImplClosed(WebFrameWidgetImpl* widget)
    {
        if (s_instance)
            s_instance->m_frozenWidgets.remove(widget);
    }

    static void continueProgram()
    {
        // Release render thread if necessary.
        if (s_instance && s_instance->m_running)
            MainThreadDebugger::instance()->debugger()->continueProgram();
    }

private:
    ClientMessageLoopAdapter(PassOwnPtr<WebDevToolsAgentClient::WebKitClientMessageLoop> messageLoop)
        : m_running(false)
        , m_messageLoop(messageLoop) { }

    void run(LocalFrame* frame) override
    {
        if (m_running)
            return;
        m_running = true;

        // 0. Flush pending frontend messages.
        WebLocalFrameImpl* frameImpl = WebLocalFrameImpl::fromFrame(frame);
        WebDevToolsAgentImpl* agent = frameImpl->devToolsAgentImpl();
        agent->flushPendingProtocolNotifications();

        Vector<WebViewImpl*> views;
        Vector<WebFrameWidgetImpl*> widgets;

        // 1. Disable input events.
        const HashSet<WebViewImpl*>& viewImpls = WebViewImpl::allInstances();
        HashSet<WebViewImpl*>::const_iterator viewImplsEnd = viewImpls.end();
        for (HashSet<WebViewImpl*>::const_iterator it =  viewImpls.begin(); it != viewImplsEnd; ++it) {
            WebViewImpl* view = *it;
            m_frozenViews.add(view);
            views.append(view);
            view->setIgnoreInputEvents(true);
        }

        const HashSet<WebFrameWidgetImpl*>& widgetImpls = WebFrameWidgetImpl::allInstances();
        HashSet<WebFrameWidgetImpl*>::const_iterator widgetImplsEnd = widgetImpls.end();
        for (HashSet<WebFrameWidgetImpl*>::const_iterator it =  widgetImpls.begin(); it != widgetImplsEnd; ++it) {
            WebFrameWidgetImpl* widget = *it;
            m_frozenWidgets.add(widget);
            widgets.append(widget);
            widget->setIgnoreInputEvents(true);
        }

        // 2. Notify embedder about pausing.
        agent->client()->willEnterDebugLoop();

        // 3. Disable active objects
        WebView::willEnterModalLoop();

        // 4. Process messages until quitNow is called.
        m_messageLoop->run();

        // 5. Resume active objects
        WebView::didExitModalLoop();

        // 6. Resume input events.
        for (Vector<WebViewImpl*>::iterator it = views.begin(); it != views.end(); ++it) {
            if (m_frozenViews.contains(*it)) {
                // The view was not closed during the dispatch.
                (*it)->setIgnoreInputEvents(false);
            }
        }
        for (Vector<WebFrameWidgetImpl*>::iterator it = widgets.begin(); it != widgets.end(); ++it) {
            if (m_frozenWidgets.contains(*it)) {
                // The widget was not closed during the dispatch.
                (*it)->setIgnoreInputEvents(false);
            }
        }

        // 7. Notify embedder about resuming.
        agent->client()->didExitDebugLoop();

        // 8. All views have been resumed, clear the set.
        m_frozenViews.clear();
        m_frozenWidgets.clear();

        m_running = false;
    }

    void quitNow() override
    {
        m_messageLoop->quitNow();
    }

    bool m_running;
    OwnPtr<WebDevToolsAgentClient::WebKitClientMessageLoop> m_messageLoop;
    typedef HashSet<WebViewImpl*> FrozenViewsSet;
    FrozenViewsSet m_frozenViews;
    typedef HashSet<WebFrameWidgetImpl*> FrozenWidgetsSet;
    FrozenWidgetsSet m_frozenWidgets;
    static ClientMessageLoopAdapter* s_instance;
};

ClientMessageLoopAdapter* ClientMessageLoopAdapter::s_instance = nullptr;

class PageInjectedScriptHostClient: public InjectedScriptHostClient {
public:
    PageInjectedScriptHostClient() { }

    ~PageInjectedScriptHostClient() override { }

    void muteWarningsAndDeprecations()
    {
        FrameConsole::mute();
        UseCounter::muteForInspector();
    }

    void unmuteWarningsAndDeprecations()
    {
        FrameConsole::unmute();
        UseCounter::unmuteForInspector();
    }
};

class DebuggerTask : public InspectorTaskRunner::Task {
public:
    DebuggerTask(PassOwnPtr<WebDevToolsAgent::MessageDescriptor> descriptor)
        : m_descriptor(descriptor)
    {
    }

    ~DebuggerTask() override {}
    virtual void run()
    {
        WebDevToolsAgent* webagent = m_descriptor->agent();
        if (!webagent)
            return;

        WebDevToolsAgentImpl* agentImpl = static_cast<WebDevToolsAgentImpl*>(webagent);
        if (agentImpl->m_attached)
            agentImpl->dispatchMessageFromFrontend(m_descriptor->message());
    }

private:
    OwnPtr<WebDevToolsAgent::MessageDescriptor> m_descriptor;
};

// static
PassOwnPtrWillBeRawPtr<WebDevToolsAgentImpl> WebDevToolsAgentImpl::create(WebLocalFrameImpl* frame, WebDevToolsAgentClient* client)
{
    WebViewImpl* view = frame->viewImpl();
    bool isMainFrame = view && view->mainFrameImpl() == frame;
    if (!isMainFrame) {
        WebDevToolsAgentImpl* agent = new WebDevToolsAgentImpl(frame, client, frame->inspectorOverlay());
        if (frame->frameWidget())
            agent->layerTreeViewChanged(frame->frameWidget()->layerTreeView());
        return adoptPtrWillBeNoop(agent);
    }

    WebDevToolsAgentImpl* agent = new WebDevToolsAgentImpl(frame, client, view->inspectorOverlay());
    agent->registerAgent(InspectorRenderingAgent::create(view));
    //agent->registerAgent(InspectorEmulationAgent::create(view));
    // TODO(dgozman): migrate each of the following agents to frame once module is ready.
    //agent->registerAgent(InspectorDatabaseAgent::create(view->page()));
    //agent->registerAgent(DeviceOrientationInspectorAgent::create(view->page()));
    //agent->registerAgent(InspectorFileSystemAgent::create(view->page()));
    //agent->registerAgent(InspectorIndexedDBAgent::create(view->page()));
    //agent->registerAgent(InspectorAccessibilityAgent::create(view->page())); // weolar
    //agent->registerAgent(InspectorDOMStorageAgent::create(view->page()));
    //agent->registerAgent(InspectorCacheStorageAgent::create());
    agent->layerTreeViewChanged(view->layerTreeView());
    return adoptPtrWillBeNoop(agent);
}

WebDevToolsAgentImpl::WebDevToolsAgentImpl(
    WebLocalFrameImpl* webLocalFrameImpl,
    WebDevToolsAgentClient* client,
    InspectorOverlay* overlay)
    : m_client(client)
    , m_webLocalFrameImpl(webLocalFrameImpl)
    , m_attached(false)
#if ENABLE(ASSERT)
    , m_hasBeenDisposed(false)
#endif
    , m_instrumentingAgents(m_webLocalFrameImpl->frame()->instrumentingAgents())
    , m_injectedScriptManager(InjectedScriptManager::createForPage())
    , m_resourceContentLoader(InspectorResourceContentLoader::create(m_webLocalFrameImpl->frame()))
    , m_state(adoptPtrWillBeNoop(new InspectorCompositeState(this)))
    , m_overlay(overlay)
    , m_cssAgent(nullptr)
    , m_resourceAgent(nullptr)
    , m_layerTreeAgent(nullptr)
    , m_agents(m_instrumentingAgents.get(), m_state.get())
    , m_deferredAgentsInitialized(false)
{
    ASSERT(isMainThread());
    ASSERT(m_webLocalFrameImpl->frame());

    long processId = Platform::current()->getUniqueIdForProcess();
    ASSERT(processId > 0);
    IdentifiersFactory::setProcessId(processId);
    InjectedScriptManager* injectedScriptManager = m_injectedScriptManager.get();

    OwnPtrWillBeRawPtr<InspectorInspectorAgent> inspectorAgentPtr(InspectorInspectorAgent::create(injectedScriptManager));
    m_inspectorAgent = inspectorAgentPtr.get();
    m_agents.append(inspectorAgentPtr.release());

    OwnPtrWillBeRawPtr<InspectorPageAgent> pageAgentPtr(InspectorPageAgent::create(m_webLocalFrameImpl->frame(), m_overlay, m_resourceContentLoader.get()));
    m_pageAgent = pageAgentPtr.get();
    m_agents.append(pageAgentPtr.release());

    OwnPtrWillBeRawPtr<InspectorDOMAgent> domAgentPtr(InspectorDOMAgent::create(m_pageAgent, injectedScriptManager, m_overlay));
    m_domAgent = domAgentPtr.get();
    m_agents.append(domAgentPtr.release());

    OwnPtrWillBeRawPtr<InspectorLayerTreeAgent> layerTreeAgentPtr(InspectorLayerTreeAgent::create(m_pageAgent));
    m_layerTreeAgent = layerTreeAgentPtr.get();
    m_agents.append(layerTreeAgentPtr.release());

    m_agents.append(InspectorTimelineAgent::create());

    ClientMessageLoopAdapter::ensureMainThreadDebuggerCreated(m_client);
    MainThreadDebugger* mainThreadDebugger = MainThreadDebugger::instance();

    OwnPtrWillBeRawPtr<PageRuntimeAgent> pageRuntimeAgentPtr(PageRuntimeAgent::create(injectedScriptManager, this, mainThreadDebugger->debugger(), m_pageAgent));
    m_pageRuntimeAgent = pageRuntimeAgentPtr.get();
    m_agents.append(pageRuntimeAgentPtr.release());

    OwnPtrWillBeRawPtr<PageConsoleAgent> pageConsoleAgentPtr = PageConsoleAgent::create(injectedScriptManager, m_domAgent, m_pageAgent);
    m_pageConsoleAgent = pageConsoleAgentPtr.get();

    OwnPtrWillBeRawPtr<InspectorWorkerAgent> workerAgentPtr = InspectorWorkerAgent::create(pageConsoleAgentPtr.get());

    OwnPtrWillBeRawPtr<InspectorTracingAgent> tracingAgentPtr = InspectorTracingAgent::create(this, workerAgentPtr.get(), m_pageAgent);
    m_tracingAgent = tracingAgentPtr.get();
    m_agents.append(tracingAgentPtr.release());

    m_agents.append(workerAgentPtr.release());
    m_agents.append(pageConsoleAgentPtr.release());

    //m_agents.append(ScreenOrientationInspectorAgent::create(*m_webLocalFrameImpl->frame())); // weolar
}

WebDevToolsAgentImpl::~WebDevToolsAgentImpl()
{
    ASSERT(m_hasBeenDisposed);
}

void WebDevToolsAgentImpl::dispose()
{
    // Explicitly dispose of the agent before destructing to ensure
    // same behavior (and correctness) with and without Oilpan.
    if (m_attached)
        Platform::current()->currentThread()->removeTaskObserver(this);
#if ENABLE(ASSERT)
    ASSERT(!m_hasBeenDisposed);
    m_hasBeenDisposed = true;
#endif
}

// static
void WebDevToolsAgentImpl::webViewImplClosed(WebViewImpl* webViewImpl)
{
    ClientMessageLoopAdapter::webViewImplClosed(webViewImpl);
}

// static
void WebDevToolsAgentImpl::webFrameWidgetImplClosed(WebFrameWidgetImpl* webFrameWidgetImpl)
{
    ClientMessageLoopAdapter::webFrameWidgetImplClosed(webFrameWidgetImpl);
}

DEFINE_TRACE(WebDevToolsAgentImpl)
{
    visitor->trace(m_instrumentingAgents);
    visitor->trace(m_injectedScriptManager);
    visitor->trace(m_resourceContentLoader);
    visitor->trace(m_state);
    visitor->trace(m_overlay);
    visitor->trace(m_asyncCallTracker);
    visitor->trace(m_inspectorAgent);
    visitor->trace(m_domAgent);
    visitor->trace(m_pageAgent);
    visitor->trace(m_cssAgent);
    visitor->trace(m_resourceAgent);
    visitor->trace(m_layerTreeAgent);
    visitor->trace(m_tracingAgent);
    visitor->trace(m_pageRuntimeAgent);
    visitor->trace(m_pageConsoleAgent);
    visitor->trace(m_inspectorBackendDispatcher);
    visitor->trace(m_agents);
}

void WebDevToolsAgentImpl::willBeDestroyed()
{
#if ENABLE(ASSERT)
    Frame* frame = m_webLocalFrameImpl->frame();
    ASSERT(frame);
    ASSERT(m_pageAgent->inspectedFrame()->view());
#endif

    detach();
    m_injectedScriptManager->disconnect();
    m_resourceContentLoader->stop();
    m_agents.discardAgents();
    m_instrumentingAgents->reset();
}

void WebDevToolsAgentImpl::initializeDeferredAgents()
{
    if (m_deferredAgentsInitialized)
        return;
    m_deferredAgentsInitialized = true;

    InjectedScriptManager* injectedScriptManager = m_injectedScriptManager.get();

    OwnPtrWillBeRawPtr<InspectorResourceAgent> resourceAgentPtr(InspectorResourceAgent::create(m_pageAgent));
    m_resourceAgent = resourceAgentPtr.get();
    m_agents.append(resourceAgentPtr.release());

    OwnPtrWillBeRawPtr<InspectorCSSAgent> cssAgentPtr(InspectorCSSAgent::create(m_domAgent, m_pageAgent, m_resourceAgent, m_resourceContentLoader.get()));
    m_cssAgent = cssAgentPtr.get();
    m_agents.append(cssAgentPtr.release());

    m_agents.append(InspectorAnimationAgent::create(m_pageAgent, m_domAgent));

    m_agents.append(InspectorMemoryAgent::create());

    m_agents.append(InspectorApplicationCacheAgent::create(m_pageAgent));

    OwnPtrWillBeRawPtr<InspectorDebuggerAgent> debuggerAgentPtr(PageDebuggerAgent::create(MainThreadDebugger::instance(), m_pageAgent, injectedScriptManager, m_overlay));
    InspectorDebuggerAgent* debuggerAgent = debuggerAgentPtr.get();
    m_agents.append(debuggerAgentPtr.release());
    m_asyncCallTracker = adoptPtrWillBeNoop(new AsyncCallTracker(debuggerAgent, m_instrumentingAgents.get()));

    m_agents.append(InspectorDOMDebuggerAgent::create(injectedScriptManager, m_domAgent, debuggerAgent));

    m_agents.append(InspectorInputAgent::create(m_pageAgent));

    m_agents.append(InspectorProfilerAgent::create(injectedScriptManager, m_overlay));

    m_agents.append(InspectorHeapProfilerAgent::create(injectedScriptManager));

    m_pageAgent->setDeferredAgents(debuggerAgent, m_cssAgent);
    m_pageConsoleAgent->setDebuggerAgent(debuggerAgent);

    MainThreadDebugger* mainThreadDebugger = MainThreadDebugger::instance();
    m_injectedScriptManager->injectedScriptHost()->init(
        m_pageConsoleAgent.get(),
        debuggerAgent,
        bind<PassRefPtr<TypeBuilder::Runtime::RemoteObject>, PassRefPtr<JSONObject>>(&InspectorInspectorAgent::inspect, m_inspectorAgent.get()),
        mainThreadDebugger->debugger(),
        adoptPtr(new PageInjectedScriptHostClient()));
}

void WebDevToolsAgentImpl::registerAgent(PassOwnPtrWillBeRawPtr<InspectorAgent> agent)
{
    m_agents.append(agent);
}

void WebDevToolsAgentImpl::attach(const WebString& hostId)
{
    if (m_attached)
        return;

    // Set the attached bit first so that sync notifications were delivered.
    m_attached = true;

    initializeDeferredAgents();
    m_resourceAgent->setHostId(hostId);
    m_overlay->setLayoutEditor(LayoutEditor::create(m_cssAgent.get()));

    m_inspectorFrontend = adoptPtr(new InspectorFrontend(this));
    // We can reconnect to existing front-end -> unmute state.
    m_state->unmute();
    m_agents.setFrontend(m_inspectorFrontend.get());

    InspectorInstrumentation::registerInstrumentingAgents(m_instrumentingAgents.get());
    InspectorInstrumentation::frontendCreated();

    m_inspectorBackendDispatcher = InspectorBackendDispatcher::create(this);
    m_agents.registerInDispatcher(m_inspectorBackendDispatcher.get());

    Platform::current()->currentThread()->addTaskObserver(this);
}

void WebDevToolsAgentImpl::reattach(const WebString& hostId, const WebString& savedState)
{
    if (m_attached)
        return;

    attach(hostId);
    m_state->loadFromCookie(savedState);
    m_agents.restore();
}

void WebDevToolsAgentImpl::detach()
{
    if (!m_attached)
        return;

    Platform::current()->currentThread()->removeTaskObserver(this);

    m_inspectorBackendDispatcher->clearFrontend();
    m_inspectorBackendDispatcher.clear();

    // Destroying agents would change the state, but we don't want that.
    // Pre-disconnect state will be used to restore inspector agents.
    m_state->mute();
    m_agents.clearFrontend();
    m_inspectorFrontend.clear();

    // Release overlay resources.
    m_overlay->clear();
    m_overlay->setLayoutEditor(nullptr);
    InspectorInstrumentation::frontendDeleted();
    InspectorInstrumentation::unregisterInstrumentingAgents(m_instrumentingAgents.get());

    m_attached = false;
}

void WebDevToolsAgentImpl::continueProgram()
{
    ClientMessageLoopAdapter::continueProgram();
}

bool WebDevToolsAgentImpl::handleInputEvent(const WebInputEvent& inputEvent)
{
    if (!m_attached)
        return false;

    if (WebInputEvent::isGestureEventType(inputEvent.type) && inputEvent.type == WebInputEvent::GestureTap) {
        // Only let GestureTab in (we only need it and we know PlatformGestureEventBuilder supports it).
        PlatformGestureEvent gestureEvent = PlatformGestureEventBuilder(m_webLocalFrameImpl->frameView(), static_cast<const WebGestureEvent&>(inputEvent));
        return handleGestureEvent(m_webLocalFrameImpl->frame(), gestureEvent);
    }
    if (WebInputEvent::isMouseEventType(inputEvent.type) && inputEvent.type != WebInputEvent::MouseEnter) {
        // PlatformMouseEventBuilder does not work with MouseEnter type, so we filter it out manually.
        PlatformMouseEvent mouseEvent = PlatformMouseEventBuilder(m_webLocalFrameImpl->frameView(), static_cast<const WebMouseEvent&>(inputEvent));
        return handleMouseEvent(m_webLocalFrameImpl->frame(), mouseEvent);
    }
    if (WebInputEvent::isTouchEventType(inputEvent.type)) {
        PlatformTouchEvent touchEvent = PlatformTouchEventBuilder(m_webLocalFrameImpl->frameView(), static_cast<const WebTouchEvent&>(inputEvent));
        return handleTouchEvent(m_webLocalFrameImpl->frame(), touchEvent);
    }
    return false;
}

bool WebDevToolsAgentImpl::handleGestureEvent(LocalFrame* frame, const PlatformGestureEvent& event)
{
    if (InspectorDOMAgent* domAgent = m_instrumentingAgents->inspectorDOMAgent())
        return domAgent->handleGestureEvent(frame, event);
    return false;
}

bool WebDevToolsAgentImpl::handleMouseEvent(LocalFrame* frame, const PlatformMouseEvent& event)
{
    if (event.type() == PlatformEvent::MouseMoved) {
        if (InspectorDOMAgent* domAgent = m_instrumentingAgents->inspectorDOMAgent())
            return domAgent->handleMouseMove(frame, event);
        return false;
    }
    if (event.type() == PlatformEvent::MousePressed) {
        if (InspectorDOMAgent* domAgent = m_instrumentingAgents->inspectorDOMAgent())
            return domAgent->handleMousePress();
    }
    return false;
}

bool WebDevToolsAgentImpl::handleTouchEvent(LocalFrame* frame, const PlatformTouchEvent& event)
{
    if (InspectorDOMAgent* domAgent = m_instrumentingAgents->inspectorDOMAgent())
        return domAgent->handleTouchEvent(frame, event);
    return false;
}

void WebDevToolsAgentImpl::didCommitLoadForLocalFrame(LocalFrame* frame)
{
    m_resourceContentLoader->didCommitLoadForLocalFrame(frame);
    m_agents.didCommitLoadForLocalFrame(frame);
}

bool WebDevToolsAgentImpl::screencastEnabled()
{
    return m_pageAgent->screencastEnabled();
}

void WebDevToolsAgentImpl::willAddPageOverlay(const GraphicsLayer* layer)
{
    m_layerTreeAgent->willAddPageOverlay(layer);
}

void WebDevToolsAgentImpl::didRemovePageOverlay(const GraphicsLayer* layer)
{
    m_layerTreeAgent->didRemovePageOverlay(layer);
}

void WebDevToolsAgentImpl::layerTreeViewChanged(WebLayerTreeView* layerTreeView)
{
    m_tracingAgent->setLayerTreeId(layerTreeView ? layerTreeView->layerTreeId() : 0);
}

void WebDevToolsAgentImpl::enableTracing(const String& categoryFilter)
{
    m_client->enableTracing(categoryFilter);
}

void WebDevToolsAgentImpl::disableTracing()
{
    m_client->disableTracing();
}

void WebDevToolsAgentImpl::dispatchOnInspectorBackend(const WebString& message)
{
    if (!m_attached)
        return;
    if (WebDevToolsAgent::shouldInterruptForMessage(message))
        MainThreadDebugger::instance()->taskRunner()->runPendingTasks();
    else
        dispatchMessageFromFrontend(message);
}

void WebDevToolsAgentImpl::dispatchMessageFromFrontend(const String& message)
{
    InspectorTaskRunner::IgnoreInterruptsScope scope(MainThreadDebugger::instance()->taskRunner());
    if (m_inspectorBackendDispatcher)
        m_inspectorBackendDispatcher->dispatch(message);
}

void WebDevToolsAgentImpl::inspectElementAt(const WebPoint& pointInRootFrame)
{
    HitTestRequest::HitTestRequestType hitType = HitTestRequest::Move | HitTestRequest::ReadOnly | HitTestRequest::AllowChildFrameContent;
    HitTestRequest request(hitType);
    WebMouseEvent dummyEvent;
    dummyEvent.type = WebInputEvent::MouseDown;
    dummyEvent.x = pointInRootFrame.x;
    dummyEvent.y = pointInRootFrame.y;
    IntPoint transformedPoint = PlatformMouseEventBuilder(m_webLocalFrameImpl->frameView(), dummyEvent).position();
    HitTestResult result(request, m_webLocalFrameImpl->frameView()->rootFrameToContents(transformedPoint));
    m_webLocalFrameImpl->frame()->contentLayoutObject()->hitTest(result);
    Node* node = result.innerNode();
    if (!node && m_webLocalFrameImpl->frame()->document())
        node = m_webLocalFrameImpl->frame()->document()->documentElement();
    m_domAgent->inspect(node);
}

void WebDevToolsAgentImpl::sendProtocolResponse(int callId, PassRefPtr<JSONObject> message)
{
    if (!m_attached)
        return;
    flushPendingProtocolNotifications();
    m_client->sendProtocolMessage(callId, message->toJSONString(), m_stateCookie);
    m_stateCookie = String();
}

void WebDevToolsAgentImpl::sendProtocolNotification(PassRefPtr<JSONObject> message)
{
    if (!m_attached)
        return;
    m_notificationQueue.append(message);
}

void WebDevToolsAgentImpl::flush()
{
    flushPendingProtocolNotifications();
}

void WebDevToolsAgentImpl::updateInspectorStateCookie(const String& state)
{
    m_stateCookie = state;
}

void WebDevToolsAgentImpl::resumeStartup()
{
    m_client->resumeStartup();
}

void WebDevToolsAgentImpl::evaluateInWebInspector(long callId, const WebString& script)
{
    m_inspectorAgent->evaluateForTestInFrontend(callId, script);
}

void WebDevToolsAgentImpl::flushPendingProtocolNotifications()
{
    if (!m_attached)
        return;

    m_agents.flushPendingProtocolNotifications();
    for (size_t i = 0; i < m_notificationQueue.size(); ++i)
        m_client->sendProtocolMessage(0, m_notificationQueue[i]->toJSONString(), WebString());
    m_notificationQueue.clear();
}

void WebDevToolsAgentImpl::willProcessTask()
{
    if (!m_attached)
        return;
    if (InspectorProfilerAgent* profilerAgent = m_instrumentingAgents->inspectorProfilerAgent())
        profilerAgent->willProcessTask();
}

void WebDevToolsAgentImpl::didProcessTask()
{
    if (!m_attached)
        return;
    if (InspectorProfilerAgent* profilerAgent = m_instrumentingAgents->inspectorProfilerAgent())
        profilerAgent->didProcessTask();
    flushPendingProtocolNotifications();
}

void WebDevToolsAgent::interruptAndDispatch(MessageDescriptor* rawDescriptor)
{
    // rawDescriptor can't be a PassOwnPtr because interruptAndDispatch is a WebKit API function.
    OwnPtr<MessageDescriptor> descriptor = adoptPtr(rawDescriptor);
    OwnPtr<DebuggerTask> task = adoptPtr(new DebuggerTask(descriptor.release()));
    MainThreadDebugger::interruptMainThreadAndRun(task.release());
}

bool WebDevToolsAgent::shouldInterruptForMessage(const WebString& message)
{
    String commandName;
    if (!InspectorBackendDispatcher::getCommandName(message, &commandName))
        return false;
    return commandName == InspectorBackendDispatcher::commandName(InspectorBackendDispatcher::kDebugger_pauseCmd)
        || commandName == InspectorBackendDispatcher::commandName(InspectorBackendDispatcher::kDebugger_setBreakpointCmd)
        || commandName == InspectorBackendDispatcher::commandName(InspectorBackendDispatcher::kDebugger_setBreakpointByUrlCmd)
        || commandName == InspectorBackendDispatcher::commandName(InspectorBackendDispatcher::kDebugger_removeBreakpointCmd)
        || commandName == InspectorBackendDispatcher::commandName(InspectorBackendDispatcher::kDebugger_setBreakpointsActiveCmd);
}

} // namespace blink
