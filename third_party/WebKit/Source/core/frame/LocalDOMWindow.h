/*
 * Copyright (C) 2006, 2007, 2009, 2010 Apple Inc. All rights reserved.
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

#ifndef LocalDOMWindow_h
#define LocalDOMWindow_h

#include "core/CoreExport.h"
#include "core/dom/MessagePort.h"
#include "core/events/EventTarget.h"
#include "core/frame/DOMWindow.h"
#include "core/frame/DOMWindowLifecycleNotifier.h"
#include "core/frame/DOMWindowLifecycleObserver.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/LocalFrameLifecycleObserver.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

#include "wtf/Assertions.h"
#include "wtf/Forward.h"

namespace blink {

class DOMWindowEventQueue;
class DOMWindowProperty;
class DocumentInit;
class EventListener;
class EventQueue;
class ExceptionState;
class FrameConsole;
class IntRect;
class MessageEvent;
class Page;
class PostMessageTimer;
class ScriptCallStack;
class SecurityOrigin;

enum PageshowEventPersistence {
    PageshowEventNotPersisted = 0,
    PageshowEventPersisted = 1
};

// Note: if you're thinking of returning something DOM-related by reference,
// please ping dcheng@chromium.org first. You probably don't want to do that.
class CORE_EXPORT LocalDOMWindow final : public DOMWindow, public WillBeHeapSupplementable<LocalDOMWindow>, public DOMWindowLifecycleNotifier {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(LocalDOMWindow);
    WILL_BE_USING_PRE_FINALIZER(LocalDOMWindow, dispose);
public:
    static PassRefPtrWillBeRawPtr<Document> createDocument(const String& mimeType, const DocumentInit&, bool forceXHTML);
    static PassRefPtrWillBeRawPtr<LocalDOMWindow> create(LocalFrame& frame)
    {
        return adoptRefWillBeNoop(new LocalDOMWindow(frame));
    }

    virtual ~LocalDOMWindow();

    DECLARE_VIRTUAL_TRACE();

    PassRefPtrWillBeRawPtr<Document> installNewDocument(const String& mimeType, const DocumentInit&, bool forceXHTML = false);

    // EventTarget overrides:
    virtual ExecutionContext* executionContext() const override;
    virtual LocalDOMWindow* toDOMWindow() override;

    // DOMWindow overrides:
    bool isLocalDOMWindow() const override { return true; }
    virtual LocalFrame* frame() const override;
    Screen* screen() const override;
    History* history() const override;
    BarProp* locationbar() const override;
    BarProp* menubar() const override;
    BarProp* personalbar() const override;
    BarProp* scrollbars() const override;
    BarProp* statusbar() const override;
    BarProp* toolbar() const override;
    Navigator* navigator() const override;
    bool offscreenBuffering() const override;
    int outerHeight() const override;
    int outerWidth() const override;
    int innerHeight() const override;
    int innerWidth() const override;
    int screenX() const override;
    int screenY() const override;
    double scrollX() const override;
    double scrollY() const override;
    const AtomicString& name() const override;
    void setName(const AtomicString&) override;
    String status() const override;
    void setStatus(const String&) override;
    String defaultStatus() const override;
    void setDefaultStatus(const String&) override;
    Document* document() const override;
    StyleMedia* styleMedia() const override;
    double devicePixelRatio() const override;
    ApplicationCache* applicationCache() const override;
    int orientation() const override;
    Console* console() const override;
    DOMSelection* getSelection() override;
    void focus(ExecutionContext*) override;
    void blur() override;
    void print() override;
    void stop() override;
    void alert(const String& message = String()) override;
    bool confirm(const String& message) override;
    String prompt(const String& message, const String& defaultValue) override;
    bool find(const String&, bool caseSensitive, bool backwards, bool wrap, bool wholeWord, bool searchInFrames, bool showDialog) const override;

    // FIXME: ScrollBehaviorSmooth is currently unsupported in PinchViewport.
    // crbug.com/434497
    void scrollBy(double x, double y, ScrollBehavior = ScrollBehaviorAuto) const override;
    void scrollBy(const ScrollToOptions&) const override;
    void scrollTo(double x, double y) const override;
    void scrollTo(const ScrollToOptions&) const override;

    void moveBy(int x, int y) const override;
    void moveTo(int x, int y) const override;
    void resizeBy(int x, int y) const override;
    void resizeTo(int width, int height) const override;
    PassRefPtrWillBeRawPtr<MediaQueryList> matchMedia(const String&) override;
    PassRefPtrWillBeRawPtr<CSSStyleDeclaration> getComputedStyle(Element*, const String& pseudoElt) const override;
    PassRefPtrWillBeRawPtr<CSSRuleList> getMatchedCSSRules(Element*, const String& pseudoElt) const override;
    int requestAnimationFrame(FrameRequestCallback*) override;
    int webkitRequestAnimationFrame(FrameRequestCallback*) override;
    void cancelAnimationFrame(int id) override;
    void schedulePostMessage(PassRefPtrWillBeRawPtr<MessageEvent>, LocalDOMWindow* source, SecurityOrigin* target, PassRefPtrWillBeRawPtr<ScriptCallStack> stackTrace);

    void registerProperty(DOMWindowProperty*);
    void unregisterProperty(DOMWindowProperty*);

    void reset();

    unsigned pendingUnloadEventListeners() const;

    bool allowPopUp(); // Call on first window, not target window.
    static bool allowPopUp(LocalFrame& firstFrame);

    Element* frameElement() const;

    PassRefPtrWillBeRawPtr<DOMWindow> open(const String& urlString, const AtomicString& frameName, const String& windowFeaturesString,
        LocalDOMWindow* callingWindow, LocalDOMWindow* enteredWindow);

    FrameConsole* frameConsole() const;

    void printErrorMessage(const String&);

    void postMessageTimerFired(PostMessageTimer*);
    void removePostMessageTimer(PostMessageTimer*);
    void dispatchMessageEventWithOriginCheck(SecurityOrigin* intendedTargetOrigin, PassRefPtrWillBeRawPtr<Event>, PassRefPtrWillBeRawPtr<ScriptCallStack>);

    // Events
    // EventTarget API
    virtual bool addEventListener(const AtomicString& eventType, PassRefPtr<EventListener>, bool useCapture = false) override;
    virtual bool removeEventListener(const AtomicString& eventType, PassRefPtr<EventListener>, bool useCapture = false) override;
    virtual void removeAllEventListeners() override;

    using EventTarget::dispatchEvent;
    bool dispatchEvent(PassRefPtrWillBeRawPtr<Event> prpEvent, PassRefPtrWillBeRawPtr<EventTarget> prpTarget);

    void dispatchLoadEvent();

    void finishedLoading();

    ApplicationCache* optionalApplicationCache() const { return m_applicationCache.get(); }

    // Dispatch the (deprecated) orientationchange event to this DOMWindow and
    // recurse on its child frames.
    void sendOrientationChangeEvent();

    void willDetachDocumentFromFrame();

    EventQueue* eventQueue() const;
    void enqueueWindowEvent(PassRefPtrWillBeRawPtr<Event>);
    void enqueueDocumentEvent(PassRefPtrWillBeRawPtr<Event>);
    void enqueuePageshowEvent(PageshowEventPersistence);
    void enqueueHashchangeEvent(const String& oldURL, const String& newURL);
    void enqueuePopstateEvent(PassRefPtr<SerializedScriptValue>);
    void dispatchWindowLoadEvent();
    void documentWasClosed();
    void statePopped(PassRefPtr<SerializedScriptValue>);

    // FIXME: This shouldn't be public once LocalDOMWindow becomes ExecutionContext.
    void clearEventQueue();

    void acceptLanguagesChanged();

private:
    // Rather than simply inheriting LocalFrameLifecycleObserver like most other
    // classes, LocalDOMWindow hides its LocalFrameLifecycleObserver with
    // composition. This prevents conflicting overloads between DOMWindow, which
    // has a frame() accessor that returns Frame* for bindings code, and
    // LocalFrameLifecycleObserver, which has a frame() accessor that returns a
    // LocalFrame*.
    class WindowFrameObserver final : public NoBaseWillBeGarbageCollected<WindowFrameObserver>, public LocalFrameLifecycleObserver {
        WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(WindowFrameObserver);
        WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(WindowFrameObserver);
        DECLARE_EMPTY_VIRTUAL_DESTRUCTOR_WILL_BE_REMOVED(WindowFrameObserver);
    public:
        static PassOwnPtrWillBeRawPtr<WindowFrameObserver> create(LocalDOMWindow*, LocalFrame&);

        DECLARE_VIRTUAL_TRACE();

    private:
        WindowFrameObserver(LocalDOMWindow*, LocalFrame&);

        // LocalFrameLifecycleObserver overrides:
        void willDetachFrameHost() override;
        void contextDestroyed() override;

        RawPtrWillBeMember<LocalDOMWindow> m_window;
    };
    friend WTF::OwnedPtrDeleter<WindowFrameObserver>;

    explicit LocalDOMWindow(LocalFrame&);
    void dispose();

    Page* page();

    void clearDocument();
    void willDestroyDocumentInFrame();

    void willDetachFrameHost();
    void frameDestroyed();

    OwnPtrWillBeMember<WindowFrameObserver> m_frameObserver;
    RefPtrWillBeMember<Document> m_document;

    bool m_shouldPrintWhenFinishedLoading;
#if ENABLE(ASSERT)
    bool m_hasBeenReset;
#endif

    WillBeHeapHashSet<RawPtrWillBeWeakMember<DOMWindowProperty>> m_properties;

    mutable PersistentWillBeMember<Screen> m_screen;
    mutable PersistentWillBeMember<History> m_history;
    mutable RefPtrWillBeMember<BarProp> m_locationbar;
    mutable RefPtrWillBeMember<BarProp> m_menubar;
    mutable RefPtrWillBeMember<BarProp> m_personalbar;
    mutable RefPtrWillBeMember<BarProp> m_scrollbars;
    mutable RefPtrWillBeMember<BarProp> m_statusbar;
    mutable RefPtrWillBeMember<BarProp> m_toolbar;
    mutable PersistentWillBeMember<Console> m_console;
    mutable PersistentWillBeMember<Navigator> m_navigator;
    mutable RefPtrWillBeMember<StyleMedia> m_media;

    String m_status;
    String m_defaultStatus;

    mutable PersistentWillBeMember<ApplicationCache> m_applicationCache;

    RefPtrWillBeMember<DOMWindowEventQueue> m_eventQueue;
    RefPtr<SerializedScriptValue> m_pendingStateObject;

    WillBeHeapHashSet<OwnPtrWillBeMember<PostMessageTimer>> m_postMessageTimers;
};

DEFINE_TYPE_CASTS(LocalDOMWindow, DOMWindow, x, x->isLocalDOMWindow(), x.isLocalDOMWindow());

inline String LocalDOMWindow::status() const
{
    return m_status;
}

inline String LocalDOMWindow::defaultStatus() const
{
    return m_defaultStatus;
}

} // namespace blink

#endif // LocalDOMWindow_h
