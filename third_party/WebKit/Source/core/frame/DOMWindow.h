// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMWindow_h
#define DOMWindow_h

#include "core/CoreExport.h"
#include "core/events/EventTarget.h"
#include "core/frame/DOMWindowBase64.h"
#include "core/frame/Location.h"
#include "platform/heap/Handle.h"
#include "platform/scroll/ScrollableArea.h"

#include "wtf/Forward.h"

namespace blink {

class ApplicationCache;
class BarProp;
class CSSRuleList;
class CSSStyleDeclaration;
class Console;
class DOMSelection;
class DOMWindowCSS;
class Document;
class Element;
class Frame;
class FrameRequestCallback;
class History;
class LocalDOMWindow;
class MediaQueryList;
class Navigator;
class Screen;
class ScrollToOptions;
class SerializedScriptValue;
class Storage;
class StyleMedia;
class External;

typedef HeapVector<Member<MessagePort>, 1> MessagePortArray;

class CORE_EXPORT DOMWindow : public EventTargetWithInlineData, public RefCountedWillBeNoBase<DOMWindow>, public DOMWindowBase64 {
    DEFINE_WRAPPERTYPEINFO();
    REFCOUNTED_EVENT_TARGET(DOMWindow);
public:
    virtual ~DOMWindow();

    // RefCountedWillBeGarbageCollectedFinalized overrides:
    DECLARE_VIRTUAL_TRACE();

    virtual bool isLocalDOMWindow() const { return false; }
    virtual bool isRemoteDOMWindow() const { return false; }

    virtual Frame* frame() const = 0;

    // ScriptWrappable overrides:
    v8::Local<v8::Object> wrap(v8::Isolate*, v8::Local<v8::Object> creationContext) override final;
    v8::Local<v8::Object> associateWithWrapper(v8::Isolate*, const WrapperTypeInfo*, v8::Local<v8::Object> wrapper) override final;

    // EventTarget overrides:
    const AtomicString& interfaceName() const override;

    // DOM Level 0
    virtual Screen* screen() const = 0;
    virtual History* history() const = 0;
    virtual BarProp* locationbar() const = 0;
    virtual BarProp* menubar() const = 0;
    virtual BarProp* personalbar() const = 0;
    virtual BarProp* scrollbars() const = 0;
    virtual BarProp* statusbar() const = 0;
    virtual BarProp* toolbar() const = 0;
    virtual Navigator* navigator() const = 0;
    Navigator* clientInformation() const { return navigator(); }
    Location* location() const;

    virtual bool offscreenBuffering() const = 0;

    virtual int outerHeight() const = 0;
    virtual int outerWidth() const = 0;
    virtual int innerHeight() const = 0;
    virtual int innerWidth() const = 0;
    virtual int screenX() const = 0;
    virtual int screenY() const = 0;
    int screenLeft() const { return screenX(); }
    int screenTop() const { return screenY(); }
    virtual double scrollX() const = 0;
    virtual double scrollY() const = 0;
    double pageXOffset() const { return scrollX(); }
    double pageYOffset() const { return scrollY(); }

    bool closed() const;

    // FIXME: This is not listed as a cross-origin accessible attribute, but in
    // Blink, it's currently marked as DoNotCheckSecurity.
    unsigned length() const;

    virtual const AtomicString& name() const = 0;
    virtual void setName(const AtomicString&) = 0;

    virtual String status() const = 0;
    virtual void setStatus(const String&) = 0;
    virtual String defaultStatus() const = 0;
    virtual void setDefaultStatus(const String&) = 0;

    // Self-referential attributes
    DOMWindow* self() const;
    DOMWindow* window() const { return self(); }
    DOMWindow* frames() const { return self(); }

    DOMWindow* opener() const;
    DOMWindow* parent() const;
    DOMWindow* top() const;

    // DOM Level 2 AbstractView Interface
    virtual Document* document() const = 0;

    // CSSOM View Module
    virtual StyleMedia* styleMedia() const = 0;

    // WebKit extensions
    virtual double devicePixelRatio() const = 0;

    virtual ApplicationCache* applicationCache() const = 0;

    // This is the interface orientation in degrees. Some examples are:
    //  0 is straight up; -90 is when the device is rotated 90 clockwise;
    //  90 is when rotated counter clockwise.
    virtual int orientation() const = 0;

    virtual Console* console() const  = 0;

    virtual DOMSelection* getSelection() = 0;

    virtual void focus(ExecutionContext*) = 0;
    virtual void blur() = 0;
    void close(ExecutionContext*);
    virtual void print() = 0;
    virtual void stop() = 0;

    virtual void alert(const String& message = String()) = 0;
    virtual bool confirm(const String& message) = 0;
    virtual String prompt(const String& message, const String& defaultValue) = 0;

    virtual bool find(const String&, bool caseSensitive, bool backwards, bool wrap, bool wholeWord, bool searchInFrames, bool showDialog) const = 0;

    virtual void scrollBy(double x, double y, ScrollBehavior = ScrollBehaviorAuto) const = 0;
    virtual void scrollBy(const ScrollToOptions&) const = 0;
    virtual void scrollTo(double x, double y) const = 0;
    virtual void scrollTo(const ScrollToOptions&) const = 0;
    void scroll(double x, double y) const { scrollTo(x, y); }
    void scroll(const ScrollToOptions& scrollToOptions) const { scrollTo(scrollToOptions); }
    virtual void moveBy(int x, int y) const = 0;
    virtual void moveTo(int x, int y) const = 0;

    virtual void resizeBy(int x, int y) const = 0;
    virtual void resizeTo(int width, int height) const = 0;

    virtual PassRefPtrWillBeRawPtr<MediaQueryList> matchMedia(const String&) = 0;

    // DOM Level 2 Style Interface
    virtual PassRefPtrWillBeRawPtr<CSSStyleDeclaration> getComputedStyle(Element*, const String& pseudoElt) const = 0;

    // WebKit extensions
    virtual PassRefPtrWillBeRawPtr<CSSRuleList> getMatchedCSSRules(Element*, const String& pseudoElt) const = 0;

    // WebKit animation extensions
    virtual int requestAnimationFrame(FrameRequestCallback*) = 0;
    virtual int webkitRequestAnimationFrame(FrameRequestCallback*) = 0;
    virtual void cancelAnimationFrame(int id) = 0;

    void captureEvents() { }
    void releaseEvents() { }
    External* external();

    // FIXME: This handles both window[index] and window.frames[index]. However,
    // the spec exposes window.frames[index] across origins but not
    // window[index]...
    DOMWindow* anonymousIndexedGetter(uint32_t) const;

    void postMessage(PassRefPtr<SerializedScriptValue> message, const MessagePortArray*, const String& targetOrigin, LocalDOMWindow* source, ExceptionState&);

    String sanitizedCrossDomainAccessErrorMessage(LocalDOMWindow* callingWindow);
    String crossDomainAccessErrorMessage(LocalDOMWindow* callingWindow);
    bool isInsecureScriptAccess(LocalDOMWindow& callingWindow, const String& urlString);

    // FIXME: When this DOMWindow is no longer the active DOMWindow (i.e.,
    // when its document is no longer the document that is displayed in its
    // frame), we would like to zero out m_frame to avoid being confused
    // by the document that is currently active in m_frame.
    // See https://bugs.webkit.org/show_bug.cgi?id=62054
    bool isCurrentlyDisplayedInFrame() const;

    void resetLocation();

    DEFINE_ATTRIBUTE_EVENT_LISTENER(animationend);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(animationiteration);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(animationstart);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(search);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(transitionend);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(wheel);

    DEFINE_MAPPED_ATTRIBUTE_EVENT_LISTENER(webkitanimationstart, webkitAnimationStart);
    DEFINE_MAPPED_ATTRIBUTE_EVENT_LISTENER(webkitanimationiteration, webkitAnimationIteration);
    DEFINE_MAPPED_ATTRIBUTE_EVENT_LISTENER(webkitanimationend, webkitAnimationEnd);
    DEFINE_MAPPED_ATTRIBUTE_EVENT_LISTENER(webkittransitionend, webkitTransitionEnd);

    DEFINE_ATTRIBUTE_EVENT_LISTENER(orientationchange);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(touchstart);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(touchmove);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(touchend);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(touchcancel);

protected:
    DOMWindow();

    // Set to true when close() has been called. Needed for
    // |window.closed| determinism; having it return 'true'
    // only after the layout widget's deferred window close
    // operation has been performed, exposes (confusing)
    // implementation details to scripts.
    bool m_windowIsClosing;

private:
    mutable RefPtrWillBeMember<Location> m_location;
    RefPtrWillBeMember<External> m_external;
};

} // namespace blink

#endif // DOMWindow_h
