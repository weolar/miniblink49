/*
 * Copyright (C) 2006, 2007, 2008, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/frame/LocalDOMWindow.h"

#include "bindings/core/v8/ScriptController.h"
#include "core/css/CSSComputedStyleDeclaration.h"
#include "core/css/CSSRuleList.h"
#include "core/css/DOMWindowCSS.h"
#include "core/css/MediaQueryList.h"
#include "core/css/MediaQueryMatcher.h"
#include "core/css/StyleMedia.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/dom/DOMImplementation.h"
#include "core/dom/FrameRequestCallback.h"
#include "core/dom/SandboxFlags.h"
#include "core/editing/Editor.h"
#include "core/events/DOMWindowEventQueue.h"
#include "core/events/HashChangeEvent.h"
#include "core/events/MessageEvent.h"
#include "core/events/PageTransitionEvent.h"
#include "core/events/PopStateEvent.h"
#include "core/frame/BarProp.h"
#include "core/frame/Console.h"
#include "core/frame/EventHandlerRegistry.h"
#include "core/frame/FrameConsole.h"
#include "core/frame/FrameView.h"
#include "core/frame/History.h"
#include "core/frame/Navigator.h"
#include "core/frame/Screen.h"
#include "core/frame/ScrollToOptions.h"
#include "core/frame/Settings.h"
#include "core/frame/SuspendableTimer.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/input/EventHandler.h"
#include "core/inspector/ConsoleMessageStorage.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/loader/DocumentLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/loader/SinkDocument.h"
#include "core/loader/appcache/ApplicationCache.h"
#include "core/page/ChromeClient.h"
#include "core/page/CreateWindow.h"
#include "core/page/Page.h"
#include "core/page/WindowFeatures.h"
#include "core/page/scrolling/ScrollingCoordinator.h"
#include "platform/EventDispatchForbiddenScope.h"
#include "public/platform/Platform.h"
#include "public/platform/WebScreenInfo.h"
#include "wtf/RefCountedLeakCounter.h"

namespace blink {

LocalDOMWindow::WindowFrameObserver::WindowFrameObserver(LocalDOMWindow* window, LocalFrame& frame)
    : LocalFrameLifecycleObserver(&frame)
    , m_window(window)
{
}

PassOwnPtrWillBeRawPtr<LocalDOMWindow::WindowFrameObserver> LocalDOMWindow::WindowFrameObserver::create(LocalDOMWindow* window, LocalFrame& frame)
{
    return adoptPtrWillBeNoop(new WindowFrameObserver(window, frame));
}

#if !ENABLE(OILPAN)
LocalDOMWindow::WindowFrameObserver::~WindowFrameObserver()
{
}
#endif

DEFINE_TRACE(LocalDOMWindow::WindowFrameObserver)
{
    visitor->trace(m_window);
    LocalFrameLifecycleObserver::trace(visitor);
}

void LocalDOMWindow::WindowFrameObserver::willDetachFrameHost()
{
    m_window->willDetachFrameHost();
}

void LocalDOMWindow::WindowFrameObserver::contextDestroyed()
{
    m_window->frameDestroyed();
    LocalFrameLifecycleObserver::contextDestroyed();
}

class PostMessageTimer final : public NoBaseWillBeGarbageCollectedFinalized<PostMessageTimer>, public SuspendableTimer {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(PostMessageTimer);
public:
    PostMessageTimer(LocalDOMWindow& window, PassRefPtrWillBeRawPtr<MessageEvent> event, PassRefPtrWillBeRawPtr<LocalDOMWindow> source, SecurityOrigin* targetOrigin, PassRefPtrWillBeRawPtr<ScriptCallStack> stackTrace, UserGestureToken* userGestureToken)
        : SuspendableTimer(window.document())
        , m_event(event)
        , m_window(&window)
        , m_targetOrigin(targetOrigin)
        , m_stackTrace(stackTrace)
        , m_userGestureToken(userGestureToken)
        , m_preventDestruction(false)
    {
        m_asyncOperationId = InspectorInstrumentation::traceAsyncOperationStarting(executionContext(), "postMessage");
    }

    PassRefPtrWillBeRawPtr<MessageEvent> event() const { return m_event.get(); }
    SecurityOrigin* targetOrigin() const { return m_targetOrigin.get(); }
    ScriptCallStack* stackTrace() const { return m_stackTrace.get(); }
    UserGestureToken* userGestureToken() const { return m_userGestureToken.get(); }
    virtual void stop() override
    {
        SuspendableTimer::stop();

        if (!m_preventDestruction) {
            // Will destroy this object
            m_window->removePostMessageTimer(this);
        }
    }

    // Eager finalization is needed to promptly stop this timer object.
    // (see DOMTimer comment for more.)
    EAGERLY_FINALIZE();
    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_event);
        visitor->trace(m_window);
        visitor->trace(m_stackTrace);
        SuspendableTimer::trace(visitor);
    }

private:
    virtual void fired() override
    {
        InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncOperationCompletedCallbackStarting(executionContext(), m_asyncOperationId);
        // Prevent calls to stop triggered from the event handler to
        // kill this object.
        m_preventDestruction = true;
        m_window->postMessageTimerFired(this);
        // Will destroy this object
        m_window->removePostMessageTimer(this);
        InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
    }

    RefPtrWillBeMember<MessageEvent> m_event;
    RawPtrWillBeMember<LocalDOMWindow> m_window;
    RefPtr<SecurityOrigin> m_targetOrigin;
    RefPtrWillBeMember<ScriptCallStack> m_stackTrace;
    RefPtr<UserGestureToken> m_userGestureToken;
    int m_asyncOperationId;
    bool m_preventDestruction;
};

static void updateSuddenTerminationStatus(LocalDOMWindow* domWindow, bool addedListener, FrameLoaderClient::SuddenTerminationDisablerType disablerType)
{
    Platform::current()->suddenTerminationChanged(!addedListener);
    if (domWindow->frame() && domWindow->frame()->loader().client())
        domWindow->frame()->loader().client()->suddenTerminationDisablerChanged(addedListener, disablerType);
}

typedef HashCountedSet<LocalDOMWindow*> DOMWindowSet;

static DOMWindowSet& windowsWithUnloadEventListeners()
{
    DEFINE_STATIC_LOCAL(DOMWindowSet, windowsWithUnloadEventListeners, ());
    return windowsWithUnloadEventListeners;
}

static DOMWindowSet& windowsWithBeforeUnloadEventListeners()
{
    DEFINE_STATIC_LOCAL(DOMWindowSet, windowsWithBeforeUnloadEventListeners, ());
    return windowsWithBeforeUnloadEventListeners;
}

static void addUnloadEventListener(LocalDOMWindow* domWindow)
{
    DOMWindowSet& set = windowsWithUnloadEventListeners();
    if (set.isEmpty())
        updateSuddenTerminationStatus(domWindow, true, FrameLoaderClient::UnloadHandler);

    set.add(domWindow);
}

static void removeUnloadEventListener(LocalDOMWindow* domWindow)
{
    DOMWindowSet& set = windowsWithUnloadEventListeners();
    DOMWindowSet::iterator it = set.find(domWindow);
    if (it == set.end())
        return;
    set.remove(it);
    if (set.isEmpty())
        updateSuddenTerminationStatus(domWindow, false, FrameLoaderClient::UnloadHandler);
}

static void removeAllUnloadEventListeners(LocalDOMWindow* domWindow)
{
    DOMWindowSet& set = windowsWithUnloadEventListeners();
    DOMWindowSet::iterator it = set.find(domWindow);
    if (it == set.end())
        return;
    set.removeAll(it);
    if (set.isEmpty())
        updateSuddenTerminationStatus(domWindow, false, FrameLoaderClient::UnloadHandler);
}

static void addBeforeUnloadEventListener(LocalDOMWindow* domWindow)
{
    DOMWindowSet& set = windowsWithBeforeUnloadEventListeners();
    if (set.isEmpty())
        updateSuddenTerminationStatus(domWindow, true, FrameLoaderClient::BeforeUnloadHandler);

    set.add(domWindow);
}

static void removeBeforeUnloadEventListener(LocalDOMWindow* domWindow)
{
    DOMWindowSet& set = windowsWithBeforeUnloadEventListeners();
    DOMWindowSet::iterator it = set.find(domWindow);
    if (it == set.end())
        return;
    set.remove(it);
    if (set.isEmpty())
        updateSuddenTerminationStatus(domWindow, false, FrameLoaderClient::BeforeUnloadHandler);
}

static void removeAllBeforeUnloadEventListeners(LocalDOMWindow* domWindow)
{
    DOMWindowSet& set = windowsWithBeforeUnloadEventListeners();
    DOMWindowSet::iterator it = set.find(domWindow);
    if (it == set.end())
        return;
    set.removeAll(it);
    if (set.isEmpty())
        updateSuddenTerminationStatus(domWindow, false, FrameLoaderClient::BeforeUnloadHandler);
}

static bool allowsBeforeUnloadListeners(LocalDOMWindow* window)
{
    ASSERT_ARG(window, window);
    LocalFrame* frame = window->frame();
    if (!frame)
        return false;
    return frame->isMainFrame();
}

unsigned LocalDOMWindow::pendingUnloadEventListeners() const
{
    return windowsWithUnloadEventListeners().count(const_cast<LocalDOMWindow*>(this));
}

bool LocalDOMWindow::allowPopUp(LocalFrame& firstFrame)
{
    if (UserGestureIndicator::processingUserGesture())
        return true;

    Settings* settings = firstFrame.settings();
    return settings && settings->javaScriptCanOpenWindowsAutomatically();
}

bool LocalDOMWindow::allowPopUp()
{
    return frame() && allowPopUp(*frame());
}

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, localDOMWindowCounter, ("LocalDOMWindowCounter"));
#endif

LocalDOMWindow::LocalDOMWindow(LocalFrame& frame)
    : m_frameObserver(WindowFrameObserver::create(this, frame))
    , m_shouldPrintWhenFinishedLoading(false)
#if ENABLE(ASSERT)
    , m_hasBeenReset(false)
#endif
{
#if ENABLE(OILPAN)
    ThreadState::current()->registerPreFinalizer(this);
#endif

#ifndef NDEBUG
    localDOMWindowCounter.increment();
#endif
}

void LocalDOMWindow::clearDocument()
{
    if (!m_document)
        return;

    ASSERT(!m_document->isActive());

    // FIXME: This should be part of ActiveDOMObject shutdown
    clearEventQueue();

    m_document->clearDOMWindow();
    m_document = nullptr;
}

void LocalDOMWindow::clearEventQueue()
{
    if (!m_eventQueue)
        return;
    m_eventQueue->close();
    m_eventQueue.clear();
}

void LocalDOMWindow::acceptLanguagesChanged()
{
    if (m_navigator)
        m_navigator->setLanguagesChanged();

    dispatchEvent(Event::create(EventTypeNames::languagechange));
}

PassRefPtrWillBeRawPtr<Document> LocalDOMWindow::createDocument(const String& mimeType, const DocumentInit& init, bool forceXHTML)
{
    RefPtrWillBeRawPtr<Document> document = nullptr;
    if (forceXHTML) {
        // This is a hack for XSLTProcessor. See XSLTProcessor::createDocumentFromSource().
        document = Document::create(init);
    } else {
        document = DOMImplementation::createDocument(mimeType, init, init.frame() ? init.frame()->inViewSourceMode() : false);
        if (document->isPluginDocument() && document->isSandboxed(SandboxPlugins))
            document = SinkDocument::create(init);
    }

    return document.release();
}

PassRefPtrWillBeRawPtr<Document> LocalDOMWindow::installNewDocument(const String& mimeType, const DocumentInit& init, bool forceXHTML)
{
    ASSERT(init.frame() == frame());

    clearDocument();

    m_document = createDocument(mimeType, init, forceXHTML);
    m_eventQueue = DOMWindowEventQueue::create(m_document.get());
    m_document->attach();

    if (!frame())
        return m_document;

    frame()->script().updateDocument();
    m_document->updateViewportDescription();

    if (frame()->page() && frame()->view()) {
        if (ScrollingCoordinator* scrollingCoordinator = frame()->page()->scrollingCoordinator()) {
            scrollingCoordinator->scrollableAreaScrollbarLayerDidChange(frame()->view(), HorizontalScrollbar);
            scrollingCoordinator->scrollableAreaScrollbarLayerDidChange(frame()->view(), VerticalScrollbar);
            scrollingCoordinator->scrollableAreaScrollLayerDidChange(frame()->view());
        }
    }

    frame()->selection().updateSecureKeyboardEntryIfActive();
    return m_document;
}

EventQueue* LocalDOMWindow::eventQueue() const
{
    return m_eventQueue.get();
}

void LocalDOMWindow::enqueueWindowEvent(PassRefPtrWillBeRawPtr<Event> event)
{
    if (!m_eventQueue)
        return;
    event->setTarget(this);
    m_eventQueue->enqueueEvent(event);
}

void LocalDOMWindow::enqueueDocumentEvent(PassRefPtrWillBeRawPtr<Event> event)
{
    if (!m_eventQueue)
        return;
    event->setTarget(m_document.get());
    m_eventQueue->enqueueEvent(event);
}

void LocalDOMWindow::dispatchWindowLoadEvent()
{
    ASSERT(!EventDispatchForbiddenScope::isEventDispatchForbidden());
    dispatchLoadEvent();
}

void LocalDOMWindow::documentWasClosed()
{
    dispatchWindowLoadEvent();
    enqueuePageshowEvent(PageshowEventNotPersisted);
    if (m_pendingStateObject)
        enqueuePopstateEvent(m_pendingStateObject.release());
}

void LocalDOMWindow::enqueuePageshowEvent(PageshowEventPersistence persisted)
{
    // FIXME: https://bugs.webkit.org/show_bug.cgi?id=36334 Pageshow event needs to fire asynchronously.
    // As per spec pageshow must be triggered asynchronously.
    // However to be compatible with other browsers blink fires pageshow synchronously.
    dispatchEvent(PageTransitionEvent::create(EventTypeNames::pageshow, persisted), m_document.get());
}

void LocalDOMWindow::enqueueHashchangeEvent(const String& oldURL, const String& newURL)
{
    enqueueWindowEvent(HashChangeEvent::create(oldURL, newURL));
}

void LocalDOMWindow::enqueuePopstateEvent(PassRefPtr<SerializedScriptValue> stateObject)
{
    // FIXME: https://bugs.webkit.org/show_bug.cgi?id=36202 Popstate event needs to fire asynchronously
    dispatchEvent(PopStateEvent::create(stateObject, history()));
}

void LocalDOMWindow::statePopped(PassRefPtr<SerializedScriptValue> stateObject)
{
    if (!frame())
        return;

    // Per step 11 of section 6.5.9 (history traversal) of the HTML5 spec, we
    // defer firing of popstate until we're in the complete state.
    if (document()->isLoadCompleted())
        enqueuePopstateEvent(stateObject);
    else
        m_pendingStateObject = stateObject;
}

LocalDOMWindow::~LocalDOMWindow()
{
#if ENABLE(OILPAN)
    // Cleared when detaching document.
    ASSERT(!m_eventQueue);
#else
    ASSERT(m_hasBeenReset);
    ASSERT(m_document->isStopped());
    clearDocument();
#endif

#ifndef NDEBUG
    localDOMWindowCounter.decrement();
#endif
}

void LocalDOMWindow::dispose()
{
    // Oilpan: should the LocalDOMWindow be GCed along with its LocalFrame without the
    // frame having first notified its observers of imminent destruction, the
    // LocalDOMWindow will not have had an opportunity to remove event listeners.
    //
    // Arrange for that removal to happen using a prefinalizer action. Making LocalDOMWindow
    // eager finalizable is problematic as other eagerly finalized objects may well
    // want to access their associated LocalDOMWindow from their destructors.
    //
    // (Non-Oilpan, LocalDOMWindow::reset() will always be invoked, the last opportunity
    // being via ~LocalFrame's setDOMWindow() call. Asserted for in the destructor.)
    if (!frame())
        return;

    removeAllEventListeners();
}

ExecutionContext* LocalDOMWindow::executionContext() const
{
    return m_document.get();
}

LocalDOMWindow* LocalDOMWindow::toDOMWindow()
{
    return this;
}

PassRefPtrWillBeRawPtr<MediaQueryList> LocalDOMWindow::matchMedia(const String& media)
{
    return document() ? document()->mediaQueryMatcher().matchMedia(media) : nullptr;
}

Page* LocalDOMWindow::page()
{
    return frame() ? frame()->page() : nullptr;
}

void LocalDOMWindow::willDetachFrameHost()
{
    frame()->host()->eventHandlerRegistry().didRemoveAllEventHandlers(*this);
    frame()->host()->consoleMessageStorage().frameWindowDiscarded(this);
    LocalDOMWindow::notifyContextDestroyed();
}

void LocalDOMWindow::frameDestroyed()
{
    willDestroyDocumentInFrame();
    resetLocation();
    m_properties.clear();
    removeAllEventListeners();
}

void LocalDOMWindow::willDestroyDocumentInFrame()
{
    for (const auto& domWindowProperty : m_properties)
        domWindowProperty->willDestroyGlobalObjectInFrame();
}

void LocalDOMWindow::willDetachDocumentFromFrame()
{
    for (const auto& domWindowProperty : m_properties)
        domWindowProperty->willDetachGlobalObjectFromFrame();
}

void LocalDOMWindow::registerProperty(DOMWindowProperty* property)
{
    m_properties.add(property);
}

void LocalDOMWindow::unregisterProperty(DOMWindowProperty* property)
{
    m_properties.remove(property);
}

void LocalDOMWindow::reset()
{
    frameDestroyed();

    m_screen = nullptr;
    m_history = nullptr;
    m_locationbar = nullptr;
    m_menubar = nullptr;
    m_personalbar = nullptr;
    m_scrollbars = nullptr;
    m_statusbar = nullptr;
    m_toolbar = nullptr;
    m_console = nullptr;
    m_navigator = nullptr;
    m_media = nullptr;
    m_applicationCache = nullptr;
#if ENABLE(ASSERT)
    m_hasBeenReset = true;
#endif

    resetLocation();

    LocalDOMWindow::notifyContextDestroyed();
}

void LocalDOMWindow::sendOrientationChangeEvent()
{
    ASSERT(RuntimeEnabledFeatures::orientationEventEnabled());
    ASSERT(frame()->isMainFrame());

    // Before dispatching the event, build a list of all frames in the page
    // to send the event to, to mitigate side effects from event handlers
    // potentially interfering with others.
    WillBeHeapVector<RefPtrWillBeMember<Frame>> frames;
    for (Frame* f = frame(); f; f = f->tree().traverseNext())
        frames.append(f);

    for (size_t i = 0; i < frames.size(); ++i) {
        if (!frames[i]->isLocalFrame())
            continue;
        toLocalFrame(frames[i].get())->localDOMWindow()->dispatchEvent(Event::create(EventTypeNames::orientationchange));
    }
}

int LocalDOMWindow::orientation() const
{
    ASSERT(RuntimeEnabledFeatures::orientationEventEnabled());

    if (!frame() || !frame()->host())
        return 0;

    int orientation = frame()->host()->chromeClient().screenInfo().orientationAngle;
    // For backward compatibility, we want to return a value in the range of
    // [-90; 180] instead of [0; 360[ because window.orientation used to behave
    // like that in WebKit (this is a WebKit proprietary API).
    if (orientation == 270)
        return -90;
    return orientation;
}

Screen* LocalDOMWindow::screen() const
{
    if (!m_screen)
        m_screen = Screen::create(frame());
    return m_screen.get();
}

History* LocalDOMWindow::history() const
{
    if (!m_history)
        m_history = History::create(frame());
    return m_history.get();
}

BarProp* LocalDOMWindow::locationbar() const
{
    if (!m_locationbar)
        m_locationbar = BarProp::create(frame(), BarProp::Locationbar);
    return m_locationbar.get();
}

BarProp* LocalDOMWindow::menubar() const
{
    if (!m_menubar)
        m_menubar = BarProp::create(frame(), BarProp::Menubar);
    return m_menubar.get();
}

BarProp* LocalDOMWindow::personalbar() const
{
    if (!m_personalbar)
        m_personalbar = BarProp::create(frame(), BarProp::Personalbar);
    return m_personalbar.get();
}

BarProp* LocalDOMWindow::scrollbars() const
{
    if (!m_scrollbars)
        m_scrollbars = BarProp::create(frame(), BarProp::Scrollbars);
    return m_scrollbars.get();
}

BarProp* LocalDOMWindow::statusbar() const
{
    if (!m_statusbar)
        m_statusbar = BarProp::create(frame(), BarProp::Statusbar);
    return m_statusbar.get();
}

BarProp* LocalDOMWindow::toolbar() const
{
    if (!m_toolbar)
        m_toolbar = BarProp::create(frame(), BarProp::Toolbar);
    return m_toolbar.get();
}

Console* LocalDOMWindow::console() const
{
    if (!m_console)
        m_console = Console::create(frame());
    return m_console.get();
}

FrameConsole* LocalDOMWindow::frameConsole() const
{
    if (!isCurrentlyDisplayedInFrame())
        return nullptr;
    return &frame()->console();
}

ApplicationCache* LocalDOMWindow::applicationCache() const
{
    if (!isCurrentlyDisplayedInFrame())
        return nullptr;
    if (!m_applicationCache)
        m_applicationCache = ApplicationCache::create(frame());
    return m_applicationCache.get();
}

Navigator* LocalDOMWindow::navigator() const
{
    if (!isCurrentlyDisplayedInFrame() && (!m_navigator || m_navigator->frame())) {
        // We return a navigator with null frame instead of returning null
        // pointer as other functions do, in order to allow users to access
        // functions such as navigator.product.
        m_navigator = Navigator::create(nullptr);
    }
    if (!m_navigator)
        m_navigator = Navigator::create(frame());
    // As described above, when not dispayed in the frame, the returning
    // navigator should not be associated with the frame.
    ASSERT(isCurrentlyDisplayedInFrame() || !m_navigator->frame());
    return m_navigator.get();
}

void LocalDOMWindow::schedulePostMessage(PassRefPtrWillBeRawPtr<MessageEvent> event, LocalDOMWindow* source, SecurityOrigin* target, PassRefPtrWillBeRawPtr<ScriptCallStack> stackTrace)
{
    // Schedule the message.
    OwnPtrWillBeRawPtr<PostMessageTimer> timer = adoptPtrWillBeNoop(new PostMessageTimer(*this, event, source, target, stackTrace, UserGestureIndicator::currentToken()));
    timer->startOneShot(0, FROM_HERE);
    timer->suspendIfNeeded();
    m_postMessageTimers.add(timer.release());
}

void LocalDOMWindow::postMessageTimerFired(PostMessageTimer* timer)
{
    if (!isCurrentlyDisplayedInFrame()) {
        return;
    }

    RefPtrWillBeRawPtr<MessageEvent> event = timer->event();

    UserGestureIndicator gestureIndicator(timer->userGestureToken());

    event->entangleMessagePorts(document());
    dispatchMessageEventWithOriginCheck(timer->targetOrigin(), event, timer->stackTrace());
}

void LocalDOMWindow::removePostMessageTimer(PostMessageTimer* timer)
{
    m_postMessageTimers.remove(timer);
}

void LocalDOMWindow::dispatchMessageEventWithOriginCheck(SecurityOrigin* intendedTargetOrigin, PassRefPtrWillBeRawPtr<Event> event, PassRefPtrWillBeRawPtr<ScriptCallStack> stackTrace)
{
    if (intendedTargetOrigin) {
        // Check target origin now since the target document may have changed since the timer was scheduled.
        if (!intendedTargetOrigin->isSameSchemeHostPortAndSuborigin(document()->securityOrigin())) {
            String message = ExceptionMessages::failedToExecute("postMessage", "DOMWindow", "The target origin provided ('" + intendedTargetOrigin->toString() + "') does not match the recipient window's origin ('" + document()->securityOrigin()->toString() + "').");
            RefPtrWillBeRawPtr<ConsoleMessage> consoleMessage = ConsoleMessage::create(SecurityMessageSource, ErrorMessageLevel, message);
            consoleMessage->setCallStack(stackTrace);
            frameConsole()->addMessage(consoleMessage.release());
            return;
        }
    }

    dispatchEvent(event);
}

DOMSelection* LocalDOMWindow::getSelection()
{
    if (!isCurrentlyDisplayedInFrame())
        return nullptr;

    return frame()->document()->getSelection();
}

Element* LocalDOMWindow::frameElement() const
{
    if (!frame())
        return nullptr;

    // The bindings security check should ensure we're same origin...
    return toHTMLFrameOwnerElement(frame()->owner());
}

void LocalDOMWindow::focus(ExecutionContext* context)
{
    if (!frame())
        return;

    FrameHost* host = frame()->host();
    if (!host)
        return;

    ASSERT(context);

    bool allowFocus = context->isWindowInteractionAllowed();
    if (allowFocus) {
        context->consumeWindowInteraction();
    } else {
        ASSERT(isMainThread());
        allowFocus = opener() && (opener() != this) && (toDocument(context)->domWindow() == opener());
    }

    // If we're a top level window, bring the window to the front.
    if (frame()->isMainFrame() && allowFocus)
        host->chromeClient().focus();

    frame()->eventHandler().focusDocumentView();
}

void LocalDOMWindow::blur()
{
}

void LocalDOMWindow::print()
{
    if (!frame())
        return;

    FrameHost* host = frame()->host();
    if (!host)
        return;

    if (frame()->document()->isSandboxed(SandboxModals)) {
        UseCounter::count(frame()->document(), UseCounter::DialogInSandboxedContext);
        if (RuntimeEnabledFeatures::sandboxBlocksModalsEnabled()) {
            frameConsole()->addMessage(ConsoleMessage::create(SecurityMessageSource, ErrorMessageLevel, "Ignored call to 'print()'. The document is sandboxed, and the 'allow-modals' keyword is not set."));
            return;
        }
    }

    if (frame()->isLoading()) {
        m_shouldPrintWhenFinishedLoading = true;
        return;
    }
    m_shouldPrintWhenFinishedLoading = false;
    host->chromeClient().print(frame());
}

void LocalDOMWindow::stop()
{
    if (!frame())
        return;
    frame()->loader().stopAllLoaders();
}

void LocalDOMWindow::alert(const String& message)
{
    if (!frame())
        return;

    if (frame()->document()->isSandboxed(SandboxModals)) {
        UseCounter::count(frame()->document(), UseCounter::DialogInSandboxedContext);
        if (RuntimeEnabledFeatures::sandboxBlocksModalsEnabled()) {
            frameConsole()->addMessage(ConsoleMessage::create(SecurityMessageSource, ErrorMessageLevel, "Ignored call to 'alert()'. The document is sandboxed, and the 'allow-modals' keyword is not set."));
            return;
        }
    }

    frame()->document()->updateLayoutTreeIfNeeded();

    FrameHost* host = frame()->host();
    if (!host)
        return;

    host->chromeClient().openJavaScriptAlert(frame(), message);
}

bool LocalDOMWindow::confirm(const String& message)
{
    if (!frame())
        return false;

    if (frame()->document()->isSandboxed(SandboxModals)) {
        UseCounter::count(frame()->document(), UseCounter::DialogInSandboxedContext);
        if (RuntimeEnabledFeatures::sandboxBlocksModalsEnabled()) {
            frameConsole()->addMessage(ConsoleMessage::create(SecurityMessageSource, ErrorMessageLevel, "Ignored call to 'confirm()'. The document is sandboxed, and the 'allow-modals' keyword is not set."));
            return false;
        }
    }

    frame()->document()->updateLayoutTreeIfNeeded();

    FrameHost* host = frame()->host();
    if (!host)
        return false;

    return host->chromeClient().openJavaScriptConfirm(frame(), message);
}

String LocalDOMWindow::prompt(const String& message, const String& defaultValue)
{
    if (!frame())
        return String();

    if (frame()->document()->isSandboxed(SandboxModals)) {
        UseCounter::count(frame()->document(), UseCounter::DialogInSandboxedContext);
        if (RuntimeEnabledFeatures::sandboxBlocksModalsEnabled()) {
            frameConsole()->addMessage(ConsoleMessage::create(SecurityMessageSource, ErrorMessageLevel, "Ignored call to 'prompt()'. The document is sandboxed, and the 'allow-modals' keyword is not set."));
            return String();
        }
    }

    frame()->document()->updateLayoutTreeIfNeeded();

    FrameHost* host = frame()->host();
    if (!host)
        return String();

    String returnValue;
    if (host->chromeClient().openJavaScriptPrompt(frame(), message, defaultValue, returnValue))
        return returnValue;

    return String();
}

bool LocalDOMWindow::find(const String& string, bool caseSensitive, bool backwards, bool wrap, bool wholeWord, bool /*searchInFrames*/, bool /*showDialog*/) const
{
    if (!isCurrentlyDisplayedInFrame())
        return false;

    // |frame()| can be destructed during |Editor::findString()| via
    // |Document::updateLayout()|, e.g. event handler removes a frame.
    RefPtrWillBeRawPtr<LocalFrame> protectFrame(frame());

    // FIXME (13016): Support searchInFrames and showDialog
    FindOptions options = (backwards ? Backwards : 0) | (caseSensitive ? 0 : CaseInsensitive) | (wrap ? WrapAround : 0) | (wholeWord ? WholeWord | AtWordStarts : 0);
    return frame()->editor().findString(string, options);
}

bool LocalDOMWindow::offscreenBuffering() const
{
    return true;
}

int LocalDOMWindow::outerHeight() const
{
    if (!frame())
        return 0;

    FrameHost* host = frame()->host();
    if (!host)
        return 0;

    if (host->settings().reportScreenSizeInPhysicalPixelsQuirk())
        return lroundf(host->chromeClient().windowRect().height() * host->deviceScaleFactor());
    return host->chromeClient().windowRect().height();
}

int LocalDOMWindow::outerWidth() const
{
    if (!frame())
        return 0;

    FrameHost* host = frame()->host();
    if (!host)
        return 0;

    if (host->settings().reportScreenSizeInPhysicalPixelsQuirk())
        return lroundf(host->chromeClient().windowRect().width() * host->deviceScaleFactor());
    return host->chromeClient().windowRect().width();
}

static FloatSize getViewportSize(LocalFrame* frame)
{
    FrameView* view = frame->view();
    if (!view)
        return FloatSize();

    FrameHost* host = frame->host();
    if (!host)
        return FloatSize();

    // The main frame's viewport size depends on the page scale. Since the
    // initial page scale depends on the content width and is set after a
    // layout, perform one now so queries during page load will use the up to
    // date viewport.
    if (host->settings().viewportEnabled() && frame->isMainFrame())
        frame->document()->updateLayoutIgnorePendingStylesheets();

    // FIXME: This is potentially too much work. We really only need to know the dimensions of the parent frame's layoutObject.
    if (Frame* parent = frame->tree().parent()) {
        if (parent && parent->isLocalFrame())
            toLocalFrame(parent)->document()->updateLayoutIgnorePendingStylesheets();
    }

    return frame->isMainFrame()
        ? host->pinchViewport().visibleRect().size()
        : view->visibleContentRect(IncludeScrollbars).size();
}

int LocalDOMWindow::innerHeight() const
{
    if (!frame())
        return 0;

    FloatSize viewportSize = getViewportSize(frame());
    return adjustForAbsoluteZoom(expandedIntSize(viewportSize).height(), frame()->pageZoomFactor());
}

int LocalDOMWindow::innerWidth() const
{
    if (!frame())
        return 0;

    FloatSize viewportSize = getViewportSize(frame());
    return adjustForAbsoluteZoom(expandedIntSize(viewportSize).width(), frame()->pageZoomFactor());
}

int LocalDOMWindow::screenX() const
{
    if (!frame())
        return 0;

    FrameHost* host = frame()->host();
    if (!host)
        return 0;

    if (host->settings().reportScreenSizeInPhysicalPixelsQuirk())
        return lroundf(host->chromeClient().windowRect().x() * host->deviceScaleFactor());
    return host->chromeClient().windowRect().x();
}

int LocalDOMWindow::screenY() const
{
    if (!frame())
        return 0;

    FrameHost* host = frame()->host();
    if (!host)
        return 0;

    if (host->settings().reportScreenSizeInPhysicalPixelsQuirk())
        return lroundf(host->chromeClient().windowRect().y() * host->deviceScaleFactor());
    return host->chromeClient().windowRect().y();
}

double LocalDOMWindow::scrollX() const
{
    if (!frame())
        return 0;

    FrameView* view = frame()->view();
    if (!view)
        return 0;

    FrameHost* host = frame()->host();
    if (!host)
        return 0;

    frame()->document()->updateLayoutIgnorePendingStylesheets();

    double viewportX = view->scrollableArea()->scrollPositionDouble().x();
    return adjustScrollForAbsoluteZoom(viewportX, frame()->pageZoomFactor());
}

double LocalDOMWindow::scrollY() const
{
    if (!frame())
        return 0;

    FrameView* view = frame()->view();
    if (!view)
        return 0;

    FrameHost* host = frame()->host();
    if (!host)
        return 0;

    frame()->document()->updateLayoutIgnorePendingStylesheets();

    double viewportY = view->scrollableArea()->scrollPositionDouble().y();
    return adjustScrollForAbsoluteZoom(viewportY, frame()->pageZoomFactor());
}

const AtomicString& LocalDOMWindow::name() const
{
    if (!isCurrentlyDisplayedInFrame())
        return nullAtom;

    return frame()->tree().name();
}

void LocalDOMWindow::setName(const AtomicString& name)
{
    if (!isCurrentlyDisplayedInFrame())
        return;

    frame()->tree().setName(name);
    ASSERT(frame()->loader().client());
    frame()->loader().client()->didChangeName(name);
}

void LocalDOMWindow::setStatus(const String& string)
{
    m_status = string;

    if (!frame())
        return;

    FrameHost* host = frame()->host();
    if (!host)
        return;

    ASSERT(frame()->document()); // Client calls shouldn't be made when the frame is in inconsistent state.
    host->chromeClient().setStatusbarText(m_status);
}

void LocalDOMWindow::setDefaultStatus(const String& string)
{
    m_defaultStatus = string;

    if (!frame())
        return;

    FrameHost* host = frame()->host();
    if (!host)
        return;

    ASSERT(frame()->document()); // Client calls shouldn't be made when the frame is in inconsistent state.
    host->chromeClient().setStatusbarText(m_defaultStatus);
}

Document* LocalDOMWindow::document() const
{
    return m_document.get();
}

StyleMedia* LocalDOMWindow::styleMedia() const
{
    if (!m_media)
        m_media = StyleMedia::create(frame());
    return m_media.get();
}

PassRefPtrWillBeRawPtr<CSSStyleDeclaration> LocalDOMWindow::getComputedStyle(Element* elt, const String& pseudoElt) const
{
    ASSERT(elt);
    return CSSComputedStyleDeclaration::create(elt, false, pseudoElt);
}

PassRefPtrWillBeRawPtr<CSSRuleList> LocalDOMWindow::getMatchedCSSRules(Element* element, const String& pseudoElement) const
{
    if (!element)
        return nullptr;

    if (!isCurrentlyDisplayedInFrame())
        return nullptr;

    unsigned colonStart = pseudoElement[0] == ':' ? (pseudoElement[1] == ':' ? 2 : 1) : 0;
    CSSSelector::PseudoType pseudoType = CSSSelector::parsePseudoType(AtomicString(pseudoElement.substring(colonStart)), false);
    if (pseudoType == CSSSelector::PseudoUnknown && !pseudoElement.isEmpty())
        return nullptr;

    unsigned rulesToInclude = StyleResolver::AuthorCSSRules;
    PseudoId pseudoId = CSSSelector::pseudoId(pseudoType);
    element->document().updateLayoutTreeIfNeeded();
    return frame()->document()->ensureStyleResolver().pseudoCSSRulesForElement(element, pseudoId, rulesToInclude);
}

double LocalDOMWindow::devicePixelRatio() const
{
    if (!frame())
        return 0.0;

    return frame()->devicePixelRatio();
}

void LocalDOMWindow::scrollBy(double x, double y, ScrollBehavior scrollBehavior) const
{
    if (!isCurrentlyDisplayedInFrame())
        return;

    document()->updateLayoutIgnorePendingStylesheets();

    FrameView* view = frame()->view();
    if (!view)
        return;

    x = ScrollableArea::normalizeNonFiniteScroll(x);
    y = ScrollableArea::normalizeNonFiniteScroll(y);

    DoublePoint currentOffset = view->scrollableArea()->scrollPositionDouble();
    DoubleSize scaledDelta(x * frame()->pageZoomFactor(), y * frame()->pageZoomFactor());

    view->scrollableArea()->setScrollPosition(currentOffset + scaledDelta, ProgrammaticScroll, scrollBehavior);
}

void LocalDOMWindow::scrollBy(const ScrollToOptions& scrollToOptions) const
{
    double x = 0.0;
    double y = 0.0;
    if (scrollToOptions.hasLeft())
        x = scrollToOptions.left();
    if (scrollToOptions.hasTop())
        y = scrollToOptions.top();
    ScrollBehavior scrollBehavior = ScrollBehaviorAuto;
    ScrollableArea::scrollBehaviorFromString(scrollToOptions.behavior(), scrollBehavior);
    scrollBy(x, y, scrollBehavior);
}

void LocalDOMWindow::scrollTo(double x, double y) const
{
    if (!isCurrentlyDisplayedInFrame())
        return;

    FrameView* view = frame()->view();
    if (!view)
        return;

    x = ScrollableArea::normalizeNonFiniteScroll(x);
    y = ScrollableArea::normalizeNonFiniteScroll(y);

    // It is only necessary to have an up-to-date layout if the position may be clamped,
    // which is never the case for (0, 0).
    if (x || y)
        document()->updateLayoutIgnorePendingStylesheets();

    DoublePoint layoutPos(x * frame()->pageZoomFactor(), y * frame()->pageZoomFactor());
    view->scrollableArea()->setScrollPosition(layoutPos, ProgrammaticScroll, ScrollBehaviorAuto);
}

void LocalDOMWindow::scrollTo(const ScrollToOptions& scrollToOptions) const
{
    if (!isCurrentlyDisplayedInFrame())
        return;

    FrameView* view = frame()->view();
    if (!view)
        return;

    // It is only necessary to have an up-to-date layout if the position may be clamped,
    // which is never the case for (0, 0).
    if (!scrollToOptions.hasLeft()
        || !scrollToOptions.hasTop()
        || scrollToOptions.left()
        || scrollToOptions.top()) {
        document()->updateLayoutIgnorePendingStylesheets();
    }

    double scaledX = 0.0;
    double scaledY = 0.0;

    DoublePoint currentOffset = view->scrollableArea()->scrollPositionDouble();
    scaledX = currentOffset.x();
    scaledY = currentOffset.y();

    if (scrollToOptions.hasLeft())
        scaledX = ScrollableArea::normalizeNonFiniteScroll(scrollToOptions.left()) * frame()->pageZoomFactor();

    if (scrollToOptions.hasTop())
        scaledY = ScrollableArea::normalizeNonFiniteScroll(scrollToOptions.top()) * frame()->pageZoomFactor();

    ScrollBehavior scrollBehavior = ScrollBehaviorAuto;
    ScrollableArea::scrollBehaviorFromString(scrollToOptions.behavior(), scrollBehavior);
    view->scrollableArea()->setScrollPosition(DoublePoint(scaledX, scaledY), ProgrammaticScroll, scrollBehavior);
}

void LocalDOMWindow::moveBy(int x, int y) const
{
    if (!frame() || !frame()->isMainFrame())
        return;

    FrameHost* host = frame()->host();
    if (!host)
        return;

    IntRect windowRect = host->chromeClient().windowRect();
    windowRect.move(x, y);
    // Security check (the spec talks about UniversalBrowserWrite to disable this check...)
    host->chromeClient().setWindowRectWithAdjustment(windowRect);
}

void LocalDOMWindow::moveTo(int x, int y) const
{
    if (!frame() || !frame()->isMainFrame())
        return;

    FrameHost* host = frame()->host();
    if (!host)
        return;

    IntRect windowRect = host->chromeClient().windowRect();
    windowRect.setLocation(IntPoint(x, y));
    // Security check (the spec talks about UniversalBrowserWrite to disable this check...)
    host->chromeClient().setWindowRectWithAdjustment(windowRect);
}

void LocalDOMWindow::resizeBy(int x, int y) const
{
    if (!frame() || !frame()->isMainFrame())
        return;

    FrameHost* host = frame()->host();
    if (!host)
        return;

    IntRect fr = host->chromeClient().windowRect();
    IntSize dest = fr.size() + IntSize(x, y);
    IntRect update(fr.location(), dest);
    host->chromeClient().setWindowRectWithAdjustment(update);
}

void LocalDOMWindow::resizeTo(int width, int height) const
{
    if (!frame() || !frame()->isMainFrame())
        return;

    FrameHost* host = frame()->host();
    if (!host)
        return;

    IntRect fr = host->chromeClient().windowRect();
    IntSize dest = IntSize(width, height);
    IntRect update(fr.location(), dest);
    host->chromeClient().setWindowRectWithAdjustment(update);
}

int LocalDOMWindow::requestAnimationFrame(FrameRequestCallback* callback)
{
    callback->m_useLegacyTimeBase = false;
    if (Document* d = document())
        return d->requestAnimationFrame(callback);
    return 0;
}

int LocalDOMWindow::webkitRequestAnimationFrame(FrameRequestCallback* callback)
{
    callback->m_useLegacyTimeBase = true;
    if (Document* d = document())
        return d->requestAnimationFrame(callback);
    return 0;
}

void LocalDOMWindow::cancelAnimationFrame(int id)
{
    if (Document* d = document())
        d->cancelAnimationFrame(id);
}

bool LocalDOMWindow::addEventListener(const AtomicString& eventType, PassRefPtr<EventListener> prpListener, bool useCapture)
{
    RefPtr<EventListener> listener = prpListener;
    if (!EventTarget::addEventListener(eventType, listener, useCapture))
        return false;

    if (frame() && frame()->host())
        frame()->host()->eventHandlerRegistry().didAddEventHandler(*this, eventType);

    if (Document* document = this->document()) {
        document->addListenerTypeIfNeeded(eventType);
    }

    notifyAddEventListener(this, eventType);

    if (eventType == EventTypeNames::unload) {
        UseCounter::count(document(), UseCounter::DocumentUnloadRegistered);
        addUnloadEventListener(this);
    } else if (eventType == EventTypeNames::beforeunload) {
        UseCounter::count(document(), UseCounter::DocumentBeforeUnloadRegistered);
        if (allowsBeforeUnloadListeners(this)) {
            // This is confusingly named. It doesn't actually add the listener. It just increments a count
            // so that we know we have listeners registered for the purposes of determining if we can
            // fast terminate the renderer process.
            addBeforeUnloadEventListener(this);
        } else {
            // Subframes return false from allowsBeforeUnloadListeners.
            UseCounter::count(document(), UseCounter::SubFrameBeforeUnloadRegistered);
        }
    }

    return true;
}

bool LocalDOMWindow::removeEventListener(const AtomicString& eventType, PassRefPtr<EventListener> listener, bool useCapture)
{
    if (!EventTarget::removeEventListener(eventType, listener, useCapture))
        return false;

    if (frame() && frame()->host())
        frame()->host()->eventHandlerRegistry().didRemoveEventHandler(*this, eventType);

    notifyRemoveEventListener(this, eventType);

    if (eventType == EventTypeNames::unload) {
        removeUnloadEventListener(this);
    } else if (eventType == EventTypeNames::beforeunload && allowsBeforeUnloadListeners(this)) {
        removeBeforeUnloadEventListener(this);
    }

    return true;
}

void LocalDOMWindow::dispatchLoadEvent()
{
    RefPtrWillBeRawPtr<Event> loadEvent(Event::create(EventTypeNames::load));
    if (frame() && frame()->loader().documentLoader() && !frame()->loader().documentLoader()->timing().loadEventStart()) {
        // The DocumentLoader (and thus its DocumentLoadTiming) might get destroyed while dispatching
        // the event, so protect it to prevent writing the end time into freed memory.
        RefPtrWillBeRawPtr<DocumentLoader> documentLoader = frame()->loader().documentLoader();
        DocumentLoadTiming& timing = documentLoader->timing();
        timing.markLoadEventStart();
        dispatchEvent(loadEvent, document());
        timing.markLoadEventEnd();
    } else
        dispatchEvent(loadEvent, document());

    // For load events, send a separate load event to the enclosing frame only.
    // This is a DOM extension and is independent of bubbling/capturing rules of
    // the DOM.
    FrameOwner* owner = frame() ? frame()->owner() : nullptr;
    if (owner)
        owner->dispatchLoad();

    TRACE_EVENT_INSTANT1("devtools.timeline", "MarkLoad", TRACE_EVENT_SCOPE_THREAD, "data", InspectorMarkLoadEvent::data(frame()));
    InspectorInstrumentation::loadEventFired(frame());
}

bool LocalDOMWindow::dispatchEvent(PassRefPtrWillBeRawPtr<Event> prpEvent, PassRefPtrWillBeRawPtr<EventTarget> prpTarget)
{
    ASSERT(!EventDispatchForbiddenScope::isEventDispatchForbidden());

    RefPtrWillBeRawPtr<EventTarget> protect(this);
    RefPtrWillBeRawPtr<Event> event = prpEvent;

    event->setTarget(prpTarget ? prpTarget : this);
    event->setCurrentTarget(this);
    event->setEventPhase(Event::AT_TARGET);

    TRACE_EVENT1("devtools.timeline", "EventDispatch", "data", InspectorEventDispatchEvent::data(*event));
    return fireEventListeners(event.get());
}

void LocalDOMWindow::removeAllEventListeners()
{
    EventTarget::removeAllEventListeners();

    notifyRemoveAllEventListeners(this);
    if (frame() && frame()->host())
        frame()->host()->eventHandlerRegistry().didRemoveAllEventHandlers(*this);

    removeAllUnloadEventListeners(this);
    removeAllBeforeUnloadEventListeners(this);
}

void LocalDOMWindow::finishedLoading()
{
    if (m_shouldPrintWhenFinishedLoading) {
        m_shouldPrintWhenFinishedLoading = false;
        print();
    }
}

void LocalDOMWindow::printErrorMessage(const String& message)
{
    if (!isCurrentlyDisplayedInFrame())
        return;

    if (message.isEmpty())
        return;

    frameConsole()->addMessage(ConsoleMessage::create(JSMessageSource, ErrorMessageLevel, message));
}

PassRefPtrWillBeRawPtr<DOMWindow> LocalDOMWindow::open(const String& urlString, const AtomicString& frameName, const String& windowFeaturesString,
    LocalDOMWindow* callingWindow, LocalDOMWindow* enteredWindow)
{
    if (!isCurrentlyDisplayedInFrame())
        return nullptr;
    Document* activeDocument = callingWindow->document();
    if (!activeDocument)
        return nullptr;
    LocalFrame* firstFrame = enteredWindow->frame();
    if (!firstFrame)
        return nullptr;

    UseCounter::count(*activeDocument, UseCounter::DOMWindowOpen);
    if (!windowFeaturesString.isEmpty())
        UseCounter::count(*activeDocument, UseCounter::DOMWindowOpenFeatures);

    if (!enteredWindow->allowPopUp()) {
        // Because FrameTree::find() returns true for empty strings, we must check for empty frame names.
        // Otherwise, illegitimate window.open() calls with no name will pass right through the popup blocker.
        if (frameName.isEmpty() || !frame()->tree().find(frameName))
            return nullptr;
    }

    // Get the target frame for the special cases of _top and _parent.
    // In those cases, we schedule a location change right now and return early.
    Frame* targetFrame = nullptr;
    if (frameName == "_top")
        targetFrame = frame()->tree().top();
    else if (frameName == "_parent") {
        if (Frame* parent = frame()->tree().parent())
            targetFrame = parent;
        else
            targetFrame = frame();
    }

    if (targetFrame) {
        if (!activeDocument->frame() || !activeDocument->frame()->canNavigate(*targetFrame))
            return nullptr;

        KURL completedURL = firstFrame->document()->completeURL(urlString);

        if (targetFrame->domWindow()->isInsecureScriptAccess(*callingWindow, completedURL))
            return targetFrame->domWindow();

        if (urlString.isEmpty())
            return targetFrame->domWindow();

        targetFrame->navigate(*activeDocument, completedURL, false, UserGestureStatus::None);
        return targetFrame->domWindow();
    }

    return createWindow(urlString, frameName, WindowFeatures(windowFeaturesString), *callingWindow, *firstFrame, *frame());
}

DEFINE_TRACE(LocalDOMWindow)
{
#if ENABLE(OILPAN)
    visitor->trace(m_frameObserver);
    visitor->trace(m_document);
    visitor->trace(m_properties);
    visitor->trace(m_screen);
    visitor->trace(m_history);
    visitor->trace(m_locationbar);
    visitor->trace(m_menubar);
    visitor->trace(m_personalbar);
    visitor->trace(m_scrollbars);
    visitor->trace(m_statusbar);
    visitor->trace(m_toolbar);
    visitor->trace(m_console);
    visitor->trace(m_navigator);
    visitor->trace(m_media);
    visitor->trace(m_applicationCache);
    visitor->trace(m_eventQueue);
    visitor->trace(m_postMessageTimers);
    HeapSupplementable<LocalDOMWindow>::trace(visitor);
#endif
    DOMWindow::trace(visitor);
    DOMWindowLifecycleNotifier::trace(visitor);
}

LocalFrame* LocalDOMWindow::frame() const
{
    return m_frameObserver->frame();
}

} // namespace blink
