/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
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

#ifndef WebFrameWidgetImpl_h
#define WebFrameWidgetImpl_h

#include "platform/graphics/GraphicsLayer.h"
#include "platform/scroll/ScrollTypes.h"
#include "public/platform/WebPoint.h"
#include "public/platform/WebSize.h"
#include "public/web/WebFrameWidget.h"
#include "public/web/WebInputEvent.h"
#include "web/PageWidgetDelegate.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebViewImpl.h"
#include "wtf/HashSet.h"
#include "wtf/OwnPtr.h"
#include "wtf/RefCounted.h"

namespace blink {
class Frame;
class Element;
class LocalFrame;
class Page;
class DeprecatedPaintLayerCompositor;
class UserGestureToken;
class WebCompositorAnimationTimeline;
class WebLayer;
class WebLayerTreeView;
class WebMouseEvent;
class WebMouseWheelEvent;

class WebFrameWidgetImpl final : public WebFrameWidget
    , public PageWidgetEventHandler
    , public RefCounted<WebFrameWidgetImpl> {
public:
    static WebFrameWidgetImpl* create(WebWidgetClient*, WebLocalFrame*);
    static HashSet<WebFrameWidgetImpl*>& allInstances();

    // WebWidget functions:
    void close() override;
    WebSize size() override;
    void willStartLiveResize() override;
    void resize(const WebSize&) override;
    void resizePinchViewport(const WebSize&) override;
    void willEndLiveResize() override;
    void willEnterFullScreen() override;
    void didEnterFullScreen() override;
    void willExitFullScreen() override;
    void didExitFullScreen() override;
    void beginFrame(const WebBeginFrameArgs&) override;
    void layout() override;
    void paint(WebCanvas*, const WebRect&) override;
    void layoutAndPaintAsync(WebLayoutAndPaintAsyncCallback*) override;
    void compositeAndReadbackAsync(WebCompositeAndReadbackAsyncCallback*) override;
    void themeChanged() override;
    bool handleInputEvent(const WebInputEvent&) override;
    void setCursorVisibilityState(bool isVisible) override;
    bool hasTouchEventHandlersAt(const WebPoint&) override;

    void applyViewportDeltas(
        const WebFloatSize& pinchViewportDelta,
        const WebFloatSize& mainFrameDelta,
        const WebFloatSize& elasticOverscrollDelta,
        float pageScaleDelta,
        float topControlsDelta) override;
    void mouseCaptureLost() override;
    void setFocus(bool enable) override;
    bool setComposition(
        const WebString& text,
        const WebVector<WebCompositionUnderline>& underlines,
        int selectionStart, int selectionEnd) override;
    bool confirmComposition() override;
    bool confirmComposition(ConfirmCompositionBehavior selectionBehavior) override;
    bool confirmComposition(const WebString& text) override;
    bool compositionRange(size_t* location, size_t* length) override;
    WebTextInputInfo textInputInfo() override;
    WebColor backgroundColor() const override;
    bool selectionBounds(WebRect& anchor, WebRect& focus) const override;
    bool selectionTextDirection(WebTextDirection& start, WebTextDirection& end) const override;
    bool isSelectionAnchorFirst() const override;
    bool caretOrSelectionRange(size_t* location, size_t* length) override;
    void setTextDirection(WebTextDirection) override;
    bool isAcceleratedCompositingActive() const override;
    void willCloseLayerTreeView() override;
    void didChangeWindowResizerRect() override;

    WebWidgetClient* client() const { return m_client; }

    Frame* focusedCoreFrame() const;

    // Returns the currently focused Element or null if no element has focus.
    Element* focusedElement() const;

    void scheduleAnimation();

    DeprecatedPaintLayerCompositor* compositor() const;
    void suppressInvalidations(bool enable);
    void setRootGraphicsLayer(GraphicsLayer*);
    void attachCompositorAnimationTimeline(WebCompositorAnimationTimeline*);
    void detachCompositorAnimationTimeline(WebCompositorAnimationTimeline*);

    void setVisibilityState(WebPageVisibilityState, bool) override;

    // Exposed for the purpose of overriding device metrics.
    void sendResizeEventAndRepaint();

    void updateMainFrameLayoutSize();

    void setIgnoreInputEvents(bool newValue);

    // Returns the page object associated with this widget. This may be null when
    // the page is shutting down, but will be valid at all other times.
    Page* page() const { return view()->page(); }

    WebLayerTreeView* layerTreeView() const { return m_layerTreeView; }

    // Returns true if the event leads to scrolling.
    static bool mapKeyCodeForScroll(
        int keyCode,
        ScrollDirection*,
        ScrollGranularity*);

private:
    friend class WebFrameWidget; // For WebFrameWidget::create.
    friend class WTF::RefCounted<WebFrameWidgetImpl>;

    explicit WebFrameWidgetImpl(WebWidgetClient*, WebLocalFrame*);
    ~WebFrameWidgetImpl();

    // Returns true if the event was actually processed.
    bool keyEventDefault(const WebKeyboardEvent&);

    // Returns true if the view was scrolled.
    bool scrollViewWithKeyboard(int keyCode, int modifiers);

    void initializeLayerTreeView();

    void setIsAcceleratedCompositingActive(bool);
    void updateLayerTreeViewport();
    void updateLayerTreeBackgroundColor();
    void updateLayerTreeDeviceScaleFactor();

    bool isTransparent() const;

    // PageWidgetEventHandler functions
    void handleMouseLeave(LocalFrame&, const WebMouseEvent&) override;
    void handleMouseDown(LocalFrame&, const WebMouseEvent&) override;
    void handleMouseUp(LocalFrame&, const WebMouseEvent&) override;
    bool handleMouseWheel(LocalFrame&, const WebMouseWheelEvent&) override;
    bool handleGestureEvent(const WebGestureEvent&) override;
    bool handleKeyEvent(const WebKeyboardEvent&) override;
    bool handleCharEvent(const WebKeyboardEvent&) override;

    WebViewImpl* view() const { return m_localRoot->viewImpl(); }

    WebWidgetClient* m_client;

    // WebFrameWidget is associated with a subtree of the frame tree, corresponding to a maximal
    // connected tree of LocalFrames. This member points to the root of that subtree.
    WebLocalFrameImpl* m_localRoot;

    WebSize m_size;

    // If set, the (plugin) node which has mouse capture.
    RefPtrWillBePersistent<Node> m_mouseCaptureNode;
    RefPtr<UserGestureToken> m_mouseCaptureGestureToken;

    WebLayerTreeView* m_layerTreeView;
    WebLayer* m_rootLayer;
    GraphicsLayer* m_rootGraphicsLayer;
    bool m_isAcceleratedCompositingActive;
    bool m_layerTreeViewClosed;

    bool m_suppressNextKeypressEvent;

    bool m_ignoreInputEvents;

    static const WebInputEvent* m_currentInputEvent;
};

} // namespace blink

#endif
