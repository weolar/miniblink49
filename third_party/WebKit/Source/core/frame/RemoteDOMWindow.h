// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RemoteDOMWindow_h
#define RemoteDOMWindow_h

#include "core/frame/DOMWindow.h"
#include "core/frame/RemoteFrame.h"

namespace blink {

class RemoteDOMWindow final : public DOMWindow {
public:
    static PassRefPtrWillBeRawPtr<RemoteDOMWindow> create(RemoteFrame& frame)
    {
        return adoptRefWillBeNoop(new RemoteDOMWindow(frame));
    }

    // EventTarget overrides:
    ExecutionContext* executionContext() const override;

    // DOMWindow overrides:
    DECLARE_VIRTUAL_TRACE();
    bool isRemoteDOMWindow() const override { return true; }
    RemoteFrame* frame() const override;
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
    void focus(ExecutionContext* = 0) override;
    void blur() override;
    void print() override;
    void stop() override;
    void alert(const String& message = String()) override;
    bool confirm(const String& message) override;
    String prompt(const String& message, const String& defaultValue) override;
    bool find(const String&, bool caseSensitive, bool backwards, bool wrap, bool wholeWord, bool searchInFrames, bool showDialog) const override;
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

private:
    explicit RemoteDOMWindow(RemoteFrame&);

    RawPtrWillBeMember<RemoteFrame> m_frame;
};

} // namespace blink

#endif // DOMWindow_h
