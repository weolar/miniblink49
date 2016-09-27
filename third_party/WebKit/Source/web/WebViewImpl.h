/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef WebViewImpl_h
#define WebViewImpl_h

#include "core/page/ContextMenuProvider.h"
#include "core/page/EventWithHitTestResults.h"
#include "platform/geometry/IntPoint.h"
#include "platform/geometry/IntRect.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebDisplayMode.h"
#include "public/platform/WebFloatSize.h"
#include "public/platform/WebGestureCurveTarget.h"
#include "public/platform/WebLayer.h"
#include "public/platform/WebPoint.h"
#include "public/platform/WebRect.h"
#include "public/platform/WebSize.h"
#include "public/platform/WebString.h"
#include "public/platform/WebVector.h"
#include "public/web/WebInputEvent.h"
#include "public/web/WebNavigationPolicy.h"
#include "public/web/WebView.h"
#include "web/ChromeClientImpl.h"
#include "web/ContextMenuClientImpl.h"
#include "web/DragClientImpl.h"
#include "web/EditorClientImpl.h"
#include "web/MediaKeysClientImpl.h"
#include "web/PageOverlayList.h"
#include "web/PageWidgetDelegate.h"
#include "web/SpellCheckerClientImpl.h"
#include "web/StorageClientImpl.h"
#include "wtf/HashSet.h"
#include "wtf/OwnPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/Vector.h"

namespace blink {

class DataObject;
class DevToolsEmulator;
class Frame;
class FullscreenController;
class InspectorOverlay;
class InspectorOverlayImpl;
class LinkHighlight;
class PageScaleConstraintsSet;
class DeprecatedPaintLayerCompositor;
class TopControls;
class UserGestureToken;
class WebActiveGestureAnimation;
class WebCompositorAnimationTimeline;
class WebDevToolsAgentImpl;
class WebElement;
class WebLayerTreeView;
class WebLocalFrameImpl;
class WebImage;
class WebPagePopupImpl;
class WebPlugin;
class WebSelection;
class WebSettingsImpl;

class WebViewImpl final : public WebView
    , public RefCounted<WebViewImpl>
    , public WebGestureCurveTarget
    , public PageWidgetEventHandler {
public:
    static WebViewImpl* create(WebViewClient*);
    static HashSet<WebViewImpl*>& allInstances();

    // WebWidget methods:
    void close() override;
    WebSize size() override;
    void willStartLiveResize() override;
    void resize(const WebSize&) override;
    void resizePinchViewport(const WebSize&) override;
    void willEndLiveResize() override;
    void didEnterFullScreen() override;
    void didExitFullScreen() override;

    void beginFrame(const WebBeginFrameArgs&) override;

    void setNeedsLayoutAndFullPaintInvalidation() override;
    void layout() override;
    void paint(WebCanvas*, const WebRect&) override;
#if OS(ANDROID)
    void paintCompositedDeprecated(WebCanvas*, const WebRect&) override;
#endif
    void layoutAndPaintAsync(WebLayoutAndPaintAsyncCallback*) override;
    void compositeAndReadbackAsync(WebCompositeAndReadbackAsyncCallback*) override;
    void themeChanged() override;
    bool handleInputEvent(const WebInputEvent&) override;
    void setCursorVisibilityState(bool isVisible) override;
    bool hasTouchEventHandlersAt(const WebPoint&) override;

    void applyViewportDeltas(
        const WebFloatSize& pinchViewportDelta,
        const WebFloatSize& layoutViewportDelta,
        const WebFloatSize& elasticOverscrollDelta,
        float pageScaleDelta,
        float topControlsShownRatioDelta) override;
    void recordFrameTimingEvent(enum FrameTimingEventType, int64_t, const WebVector<WebFrameTimingEvent>&) override;
    void mouseCaptureLost() override;
    void setFocus(bool enable) override;
    bool setComposition(
        const WebString& text,
        const WebVector<WebCompositionUnderline>& underlines,
        int selectionStart,
        int selectionEnd) override;
    bool confirmComposition() override;
    bool confirmComposition(ConfirmCompositionBehavior selectionBehavior) override;
    bool confirmComposition(const WebString& text) override;
    bool compositionRange(size_t* location, size_t* length) override;
    WebTextInputInfo textInputInfo() override;
    WebColor backgroundColor() const override;
    WebPagePopup* pagePopup() const override;
    bool selectionBounds(WebRect& anchor, WebRect& focus) const override;
    bool selectionTextDirection(WebTextDirection& start, WebTextDirection& end) const override;
    bool isSelectionAnchorFirst() const override;
    bool caretOrSelectionRange(size_t* location, size_t* length) override;
    void setTextDirection(WebTextDirection) override;
    bool isAcceleratedCompositingActive() const override;
    void willCloseLayerTreeView() override;
    void didAcquirePointerLock() override;
    void didNotAcquirePointerLock() override;
    void didLosePointerLock() override;
    void didChangeWindowResizerRect() override;

    // WebView methods:
    virtual bool isWebView() const { return true; }
    void setMainFrame(WebFrame*) override;
    void setCredentialManagerClient(WebCredentialManagerClient*) override;
    void setPrerendererClient(WebPrerendererClient*) override;
    void setSpellCheckClient(WebSpellCheckClient*) override;
    WebSettings* settings() override;
    WebString pageEncoding() const override;
    void setPageEncoding(const WebString&) override;
    bool isTransparent() const override;
    void setIsTransparent(bool value) override;
    void setBaseBackgroundColor(WebColor) override;
    bool tabsToLinks() const override;
    void setTabsToLinks(bool value) override;
    bool tabKeyCyclesThroughElements() const override;
    void setTabKeyCyclesThroughElements(bool value) override;
    bool isActive() const override;
    void setIsActive(bool value) override;
    void setDomainRelaxationForbidden(bool, const WebString& scheme) override;
    void setWindowFeatures(const WebWindowFeatures&) override;
    void setOpenedByDOM() override;
    WebFrame* mainFrame() override;
    WebFrame* findFrameByName(
        const WebString& name, WebFrame* relativeToFrame) override;
    WebFrame* focusedFrame() override;
    void setFocusedFrame(WebFrame*) override;
    void setInitialFocus(bool reverse) override;
    void clearFocusedElement() override;
    bool scrollFocusedNodeIntoRect(const WebRect&) override;
    void zoomToFindInPageRect(const WebRect&);
    void advanceFocus(bool reverse) override;
    double zoomLevel() override;
    double setZoomLevel(double) override;
    void zoomLimitsChanged(double minimumZoomLevel, double maximumZoomLevel) override;
    float textZoomFactor() override;
    float setTextZoomFactor(float) override;
    bool zoomToMultipleTargetsRect(const WebRect&) override;
    float pageScaleFactor() const override;
    void setDefaultPageScaleLimits(float minScale, float maxScale) override;
    void setInitialPageScaleOverride(float) override;
    void setMaximumLegibleScale(float) override;
    void setPageScaleFactor(float) override;
    void setPinchViewportOffset(const WebFloatPoint&) override;
    WebFloatPoint pinchViewportOffset() const override;
    void resetScrollAndScaleState() override;
    void setIgnoreViewportTagScaleLimits(bool) override;
    WebSize contentsPreferredMinimumSize() override;
    void setDisplayMode(WebDisplayMode) override;

    float deviceScaleFactor() const override;
    void setDeviceScaleFactor(float) override;
    void setDeviceColorProfile(const WebVector<char>&) override;
    void resetDeviceColorProfile() override;

    void enableAutoResizeMode(
        const WebSize& minSize,
        const WebSize& maxSize) override;
    void disableAutoResizeMode() override;
    void performMediaPlayerAction(
        const WebMediaPlayerAction& action,
        const WebPoint& location) override;
    void performPluginAction(
        const WebPluginAction&,
        const WebPoint&) override;
    WebHitTestResult hitTestResultAt(const WebPoint&) override;
    WebHitTestResult hitTestResultForTap(const WebPoint&, const WebSize&) override;
    void copyImageAt(const WebPoint&) override;
    void saveImageAt(const WebPoint&) override;
    void dragSourceEndedAt(
        const WebPoint& clientPoint,
        const WebPoint& screenPoint,
        WebDragOperation) override;
    void dragSourceSystemDragEnded() override;
    WebDragOperation dragTargetDragEnter(
        const WebDragData&,
        const WebPoint& clientPoint,
        const WebPoint& screenPoint,
        WebDragOperationsMask operationsAllowed,
        int modifiers) override;
    WebDragOperation dragTargetDragOver(
        const WebPoint& clientPoint,
        const WebPoint& screenPoint,
        WebDragOperationsMask operationsAllowed,
        int modifiers) override;
    void dragTargetDragLeave() override;
    void dragTargetDrop(
        const WebPoint& clientPoint,
        const WebPoint& screenPoint,
        int modifiers) override;
    void spellingMarkers(WebVector<uint32_t>* markers) override;
    void removeSpellingMarkersUnderWords(const WebVector<WebString>& words) override;
    unsigned long createUniqueIdentifierForRequest() override;
    void enableDeviceEmulation(const WebDeviceEmulationParams&) override;
    void disableDeviceEmulation() override;
#ifdef MINIBLINK_NOT_IMPLEMENTED
    WebAXObject accessibilityObject() override;
#endif // MINIBLINK_NOT_IMPLEMENTED
    void setSelectionColors(unsigned activeBackgroundColor,
                                    unsigned activeForegroundColor,
                                    unsigned inactiveBackgroundColor,
                                    unsigned inactiveForegroundColor) override;
    void performCustomContextMenuAction(unsigned action) override;
    void showContextMenu() override;
    void extractSmartClipData(WebRect, WebString&, WebString&, WebRect&) override;
    void hidePopups() override;
    void addPageOverlay(WebPageOverlay*, int /* zOrder */) override;
    void removePageOverlay(WebPageOverlay*) override;
    void transferActiveWheelFlingAnimation(const WebActiveWheelFlingParameters&) override;
    bool endActiveFlingAnimation() override;
    void setShowPaintRects(bool) override;
    void setShowDebugBorders(bool);
    void setShowFPSCounter(bool) override;
    void setContinuousPaintingEnabled(bool) override;
    void setShowScrollBottleneckRects(bool) override;
    void acceptLanguagesChanged() override;

    // WebViewImpl
    void enableViewport();
    void disableViewport();

    float defaultMinimumPageScaleFactor() const;
    float defaultMaximumPageScaleFactor() const;
    float minimumPageScaleFactor() const;
    float maximumPageScaleFactor() const;
    float clampPageScaleFactorToLimits(float) const;

    HitTestResult coreHitTestResultAt(const WebPoint&);
    void suppressInvalidations(bool enable);
    void invalidateRect(const IntRect&);

    void setIgnoreInputEvents(bool newValue);
    void setBackgroundColorOverride(WebColor);
    void setZoomFactorOverride(float);
    void updateShowFPSCounterAndContinuousPainting();
    void setCompositorDeviceScaleFactorOverride(float);
    void setRootLayerTransform(const WebSize& offset, float scale);

    Color baseBackgroundColor() const { return m_baseBackgroundColor; }

    WebColor backgroundColorOverride() const { return m_backgroundColorOverride; }

    PageOverlayList* pageOverlays() const { return m_pageOverlays.get(); }

    void setOverlayLayer(GraphicsLayer*);

    const WebPoint& lastMouseDownPoint() const
    {
        return m_lastMouseDownPoint;
    }

    Frame* focusedCoreFrame() const;

    // Returns the currently focused Element or null if no element has focus.
    Element* focusedElement() const;

    static WebViewImpl* fromPage(Page*);

    WebViewClient* client()
    {
        return m_client;
    }

    WebSpellCheckClient* spellCheckClient()
    {
        return m_spellCheckClient;
    }

    // Returns the page object associated with this view. This may be null when
    // the page is shutting down, but will be valid at all other times.
    Page* page() const
    {
        return m_page.get();
    }

    WebDevToolsAgentImpl* mainFrameDevToolsAgentImpl();

    InspectorOverlay* inspectorOverlay();

    DevToolsEmulator* devToolsEmulator() const
    {
        return m_devToolsEmulator.get();
    }

    // Returns the main frame associated with this view. This may be null when
    // the page is shutting down, but will be valid at all other times.
    WebLocalFrameImpl* mainFrameImpl();

    // FIXME: Temporary method to accommodate out-of-process frame ancestors;
    // will be removed when there can be multiple WebWidgets for a single page.
    WebLocalFrameImpl* localFrameRootTemporary() const;

    // Event related methods:
    void mouseContextMenu(const WebMouseEvent&);
    void mouseDoubleClick(const WebMouseEvent&);

    bool detectContentOnTouch(const GestureEventWithHitTestResults& targetedEvent);
    bool startPageScaleAnimation(const IntPoint& targetPosition, bool useAnchor, float newScale, double durationInSeconds);

    void hasTouchEventHandlers(bool);

    // WebGestureCurveTarget implementation for fling.
    bool scrollBy(const WebFloatSize& delta, const WebFloatSize& velocity) override;

    // Handles context menu events orignated via the the keyboard. These
    // include the VK_APPS virtual key and the Shift+F10 combine. Code is
    // based on the Webkit function bool WebView::handleContextMenuEvent(WPARAM
    // wParam, LPARAM lParam) in webkit\webkit\win\WebView.cpp. The only
    // significant change in this function is the code to convert from a
    // Keyboard event to the Right Mouse button down event.
    bool sendContextMenuEvent(const WebKeyboardEvent&);

    void showContextMenuAtPoint(float x, float y, PassRefPtrWillBeRawPtr<ContextMenuProvider>);

    void showContextMenuForElement(WebElement);

    // Notifies the WebView that a load has been committed. isNewNavigation
    // will be true if a new session history item should be created for that
    // load. isNavigationWithinPage will be true if the navigation does
    // not take the user away from the current page.
    void didCommitLoad(bool isNewNavigation, bool isNavigationWithinPage);

    void postLayoutResize(WebLocalFrameImpl* webframe);

    // Indicates two things:
    //   1) This view may have a new layout now.
    //   2) Calling layout() is a no-op.
    // After calling WebWidget::layout(), expect to get this notification
    // unless the view did not need a layout.
    void layoutUpdated(WebLocalFrameImpl*);

    void willInsertBody(WebLocalFrameImpl*);
    void didRemoveAllPendingStylesheet(WebLocalFrameImpl*);
    void didChangeContentsSize();
    void pageScaleFactorChanged();

    // Returns true if popup menus should be rendered by the browser, false if
    // they should be rendered by WebKit (which is the default).
    static bool useExternalPopupMenus();

    bool contextMenuAllowed() const
    {
        return m_contextMenuAllowed;
    }

    bool shouldAutoResize() const
    {
        return m_shouldAutoResize;
    }

    IntSize minAutoSize() const
    {
        return m_minAutoSize;
    }

    IntSize maxAutoSize() const
    {
        return m_maxAutoSize;
    }

    void updateMainFrameLayoutSize();
    void updatePageDefinedViewportConstraints(const ViewportDescription&);

    // Start a system drag and drop operation.
    void startDragging(
        LocalFrame*,
        const WebDragData& dragData,
        WebDragOperationsMask mask,
        const WebImage& dragImage,
        const WebPoint& dragImageOffset);

    PagePopup* openPagePopup(PagePopupClient*);
    void closePagePopup(PagePopup*);
    LocalDOMWindow* pagePopupWindow() const;

    // Returns the input event we're currently processing. This is used in some
    // cases where the WebCore DOM event doesn't have the information we need.
    static const WebInputEvent* currentInputEvent()
    {
        return m_currentInputEvent;
    }

    GraphicsLayer* rootGraphicsLayer();
    void setRootGraphicsLayer(GraphicsLayer*);
    GraphicsLayerFactory* graphicsLayerFactory() const;
    DeprecatedPaintLayerCompositor* compositor() const;
    void registerForAnimations(WebLayer*);
    void scheduleAnimation();
    void attachCompositorAnimationTimeline(WebCompositorAnimationTimeline*);
    void detachCompositorAnimationTimeline(WebCompositorAnimationTimeline*);

    void setVisibilityState(WebPageVisibilityState, bool) override;

    bool hasOpenedPopup() const { return m_pagePopup; }

    // Returns true if the event leads to scrolling.
    static bool mapKeyCodeForScroll(
        int keyCode,
        ScrollDirectionPhysical*,
        ScrollGranularity*);

    // Called by a full frame plugin inside this view to inform it that its
    // zoom level has been updated.  The plugin should only call this function
    // if the zoom change was triggered by the browser, it's only needed in case
    // a plugin can update its own zoom, say because of its own UI.
    void fullFramePluginZoomLevelChanged(double zoomLevel);

    void computeScaleAndScrollForBlockRect(const WebPoint& hitPoint, const WebRect& blockRect, float padding, float defaultScaleWhenAlreadyLegible, float& scale, WebPoint& scroll);
    Node* bestTapNode(const GestureEventWithHitTestResults& targetedTapEvent);
    void enableTapHighlightAtPoint(const GestureEventWithHitTestResults& targetedTapEvent);
    void enableTapHighlights(WillBeHeapVector<RawPtrWillBeMember<Node>>&);
    void computeScaleAndScrollForFocusedNode(Node* focusedNode, bool zoomInToLegibleScale, float& scale, IntPoint& scroll, bool& needAnimation);

    void animateDoubleTapZoom(const IntPoint&);

    void enableFakePageScaleAnimationForTesting(bool);
    bool fakeDoubleTapAnimationPendingForTesting() const { return m_doubleTapZoomPending; }
    IntPoint fakePageScaleAnimationTargetPositionForTesting() const { return m_fakePageScaleAnimationTargetPosition; }
    float fakePageScaleAnimationPageScaleForTesting() const { return m_fakePageScaleAnimationPageScaleFactor; }
    bool fakePageScaleAnimationUseAnchorForTesting() const { return m_fakePageScaleAnimationUseAnchor; }

    void enterFullScreenForElement(Element*);
    void exitFullScreenForElement(Element*);

    void clearCompositedSelection();
    void updateCompositedSelection(const WebSelection&);

    // Exposed for the purpose of overriding device metrics.
    void sendResizeEventAndRepaint();

    // Exposed for testing purposes.
    bool hasHorizontalScrollbar();
    bool hasVerticalScrollbar();

    // Pointer Lock calls allow a page to capture all mouse events and
    // disable the system cursor.
    bool requestPointerLock();
    void requestPointerUnlock();
    bool isPointerLocked();

    // Heuristic-based function for determining if we should disable workarounds
    // for viewing websites that are not optimized for mobile devices.
    bool shouldDisableDesktopWorkarounds();

    // Exposed for tests.
    unsigned numLinkHighlights() { return m_linkHighlights.size(); }
    LinkHighlight* linkHighlight(int i) { return m_linkHighlights[i].get(); }

    WebSettingsImpl* settingsImpl();

    // Returns the bounding box of the block type node touched by the WebPoint.
    WebRect computeBlockBound(const WebPoint&, bool ignoreClipping);

    // Exposed for tests.
    WebVector<WebCompositionUnderline> compositionUnderlines() const;

    WebLayerTreeView* layerTreeView() const { return m_layerTreeView; }

    bool matchesHeuristicsForGpuRasterizationForTesting() const { return m_matchesHeuristicsForGpuRasterization; }

    void setTopControlsHeight(float height, bool topControlsShrinkLayoutSize) override;
    void updateTopControlsState(WebTopControlsState constraint, WebTopControlsState current, bool animate) override;

    TopControls& topControls();
    // Called anytime top controls layout height or content offset have changed.
    void didUpdateTopControls();

    void forceNextWebGLContextCreationToFail() override;
    void forceNextDrawingBufferCreationToFail() override;

    IntSize mainFrameSize();
    WebDisplayMode displayMode() const { return m_displayMode; }

    PageScaleConstraintsSet& pageScaleConstraintsSet() const;

    FloatSize elasticOverscroll() const { return m_elasticOverscroll; }

private:
    void setPageScaleFactorAndLocation(float, const FloatPoint&);

    void scrollAndRescaleViewports(float scaleFactor, const IntPoint& mainFrameOrigin, const FloatPoint& pinchViewportOrigin);

    float maximumLegiblePageScale() const;
    void refreshPageScaleFactorAfterLayout();
    void resumeTreeViewCommits();
    IntSize contentsSize() const;

    void performResize();
    void resizeViewWhileAnchored(FrameView*);

    friend class WebView;  // So WebView::Create can call our constructor
    friend class WTF::RefCounted<WebViewImpl>;
    friend void setCurrentInputEventForTest(const WebInputEvent*);

    enum DragAction {
      DragEnter,
      DragOver
    };

    explicit WebViewImpl(WebViewClient*);
    ~WebViewImpl() override;

    WebTextInputType textInputType();
    int textInputFlags();

    WebString inputModeOfFocusedElement();

    // Returns true if the event was actually processed.
    bool keyEventDefault(const WebKeyboardEvent&);

    bool confirmComposition(const WebString& text, ConfirmCompositionBehavior);

    // Returns true if the view was scrolled.
    bool scrollViewWithKeyboard(int keyCode, int modifiers);

    void hideSelectPopup();

    HitTestResult hitTestResultForRootFramePos(const IntPoint&);
    HitTestResult hitTestResultForViewportPos(const IntPoint&);

    // Consolidate some common code between starting a drag over a target and
    // updating a drag over a target. If we're starting a drag, |isEntering|
    // should be true.
    WebDragOperation dragTargetDragEnterOrOver(const WebPoint& clientPoint,
                                               const WebPoint& screenPoint,
                                               DragAction,
                                               int modifiers);

    void configureAutoResizeMode();

    void initializeLayerTreeView();

    void setIsAcceleratedCompositingActive(bool);
    void doComposite();
    void reallocateRenderer();
    void updateLayerTreeViewport();
    void updateLayerTreeBackgroundColor();
    void updateRootLayerTransform();
    void updateLayerTreeDeviceScaleFactor();

    // Helper function: Widens the width of |source| by the specified margins
    // while keeping it smaller than page width.
    WebRect widenRectWithinPageBounds(const WebRect& source, int targetMargin, int minimumMargin);

    void pointerLockMouseEvent(const WebInputEvent&);

    // PageWidgetEventHandler functions
    void handleMouseLeave(LocalFrame&, const WebMouseEvent&) override;
    void handleMouseDown(LocalFrame&, const WebMouseEvent&) override;
    void handleMouseUp(LocalFrame&, const WebMouseEvent&) override;
    bool handleMouseWheel(LocalFrame&, const WebMouseWheelEvent&) override;
    bool handleGestureEvent(const WebGestureEvent&) override;
    bool handleKeyEvent(const WebKeyboardEvent&) override;
    bool handleCharEvent(const WebKeyboardEvent&) override;

    bool handleSyntheticWheelFromTouchpadPinchEvent(const WebGestureEvent&);

    WebPlugin* focusedPluginIfInputMethodSupported(LocalFrame*);

    void enablePopupMouseWheelEventListener();
    void disablePopupMouseWheelEventListener();

    void cancelPagePopup();

    WebViewClient* m_client; // Can be 0 (e.g. unittests, shared workers, etc.)
    WebSpellCheckClient* m_spellCheckClient;

    ChromeClientImpl m_chromeClientImpl;
    ContextMenuClientImpl m_contextMenuClientImpl;
    DragClientImpl m_dragClientImpl;
    EditorClientImpl m_editorClientImpl;
    SpellCheckerClientImpl m_spellCheckerClientImpl;
    StorageClientImpl m_storageClientImpl;

    WebSize m_size;
    // If true, automatically resize the layout view around its content.
    bool m_shouldAutoResize;
    // The lower bound on the size when auto-resizing.
    IntSize m_minAutoSize;
    // The upper bound on the size when auto-resizing.
    IntSize m_maxAutoSize;

    OwnPtrWillBePersistent<Page> m_page;

    // An object that can be used to manipulate m_page->settings() without linking
    // against WebCore. This is lazily allocated the first time GetWebSettings()
    // is called.
    OwnPtr<WebSettingsImpl> m_webSettings;

    // A copy of the web drop data object we received from the browser.
    Persistent<DataObject> m_currentDragData;

    // The point relative to the client area where the mouse was last pressed
    // down. This is used by the drag client to determine what was under the
    // mouse when the drag was initiated. We need to track this here in
    // WebViewImpl since DragClient::startDrag does not pass the position the
    // mouse was at when the drag was initiated, only the current point, which
    // can be misleading as it is usually not over the element the user actually
    // dragged by the time a drag is initiated.
    WebPoint m_lastMouseDownPoint;

    // Keeps track of the current zoom level. 0 means no zoom, positive numbers
    // mean zoom in, negative numbers mean zoom out.
    double m_zoomLevel;

    double m_minimumZoomLevel;

    double m_maximumZoomLevel;

    // This value, when multiplied by the font scale factor, gives the maximum
    // page scale that can result from automatic zooms.
    float m_maximumLegibleScale;

    // The scale moved to by the latest double tap zoom, if any.
    float m_doubleTapZoomPageScaleFactor;
    // Have we sent a double-tap zoom and not yet heard back the scale?
    bool m_doubleTapZoomPending;

    // Used for testing purposes.
    bool m_enableFakePageScaleAnimationForTesting;
    IntPoint m_fakePageScaleAnimationTargetPosition;
    float m_fakePageScaleAnimationPageScaleFactor;
    bool m_fakePageScaleAnimationUseAnchor;

    bool m_contextMenuAllowed;

    bool m_doingDragAndDrop;

    bool m_ignoreInputEvents;

    float m_compositorDeviceScaleFactorOverride;
    WebSize m_rootLayerOffset;
    float m_rootLayerScale;

    // Webkit expects keyPress events to be suppressed if the associated keyDown
    // event was handled. Safari implements this behavior by peeking out the
    // associated WM_CHAR event if the keydown was handled. We emulate
    // this behavior by setting this flag if the keyDown was handled.
    bool m_suppressNextKeypressEvent;

    // Represents whether or not this object should process incoming IME events.
    bool m_imeAcceptEvents;

    // The available drag operations (copy, move link...) allowed by the source.
    WebDragOperation m_operationsAllowed;

    // The current drag operation as negotiated by the source and destination.
    // When not equal to DragOperationNone, the drag data can be dropped onto the
    // current drop target in this WebView (the drop target can accept the drop).
    WebDragOperation m_dragOperation;

    // The popup associated with an input/select element.
    RefPtr<WebPagePopupImpl> m_pagePopup;

    OwnPtrWillBePersistent<InspectorOverlayImpl> m_inspectorOverlay;
    OwnPtr<DevToolsEmulator> m_devToolsEmulator;
    OwnPtr<PageOverlayList> m_pageOverlays;

    // Whether the webview is rendering transparently.
    bool m_isTransparent;

    // Whether the user can press tab to focus links.
    bool m_tabsToLinks;

    // If set, the (plugin) node which has mouse capture.
    RefPtrWillBePersistent<Node> m_mouseCaptureNode;
    RefPtr<UserGestureToken> m_mouseCaptureGestureToken;

    RefPtr<UserGestureToken> m_pointerLockGestureToken;

    IntRect m_rootLayerScrollDamage;
    WebLayerTreeView* m_layerTreeView;
    WebLayer* m_rootLayer;
    GraphicsLayer* m_rootGraphicsLayer;
    GraphicsLayer* m_rootTransformLayer;
    OwnPtr<GraphicsLayerFactory> m_graphicsLayerFactory;

    bool m_matchesHeuristicsForGpuRasterization;
    // If true, the graphics context is being restored.
    bool m_recreatingGraphicsContext;
    static const WebInputEvent* m_currentInputEvent;
#ifdef MINIBLINK_NOT_IMPLEMENTED
    MediaKeysClientImpl m_mediaKeysClientImpl;
#endif // MINIBLINK_NOT_IMPLEMENTED
    OwnPtr<WebActiveGestureAnimation> m_gestureAnimation;
    WebPoint m_positionOnFlingStart;
    WebPoint m_globalPositionOnFlingStart;
    int m_flingModifier;
    bool m_flingSourceDevice;
    Vector<OwnPtr<LinkHighlight>> m_linkHighlights;
    OwnPtrWillBePersistent<FullscreenController> m_fullscreenController;

    bool m_showFPSCounter;
    bool m_continuousPaintingEnabled;
    WebColor m_baseBackgroundColor;
    WebColor m_backgroundColorOverride;
    float m_zoomFactorOverride;

    bool m_userGestureObserved;
    WebDisplayMode m_displayMode;

    FloatSize m_elasticOverscroll;

    RefPtr<EventListener> m_popupMouseWheelEventListener;

    void setMainFrameTopContentInset(int offset) override;

};

DEFINE_TYPE_CASTS(WebViewImpl, WebWidget, widget, widget->isWebView(), widget.isWebView());
// We have no ways to check if the specified WebView is an instance of
// WebViewImpl because WebViewImpl is the only implementation of WebView.
DEFINE_TYPE_CASTS(WebViewImpl, WebView, webView, true, true);

} // namespace blink

#endif
