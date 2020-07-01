/*
 * Copyright (C) 2011, 2012 Google Inc. All rights reserved.
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
#include "web/WebViewImpl.h"

#include "core/CSSValueKeywords.h"
#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/clipboard/DataObject.h"
#include "core/dom/Document.h"
#include "core/dom/DocumentMarkerController.h"
#include "core/dom/Fullscreen.h"
#include "core/dom/LayoutTreeBuilderTraversal.h"
#include "core/dom/Text.h"
#include "core/editing/Editor.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/HTMLInterchange.h"
#include "core/editing/InputMethodController.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/editing/markup.h"
#include "core/events/KeyboardEvent.h"
#include "core/events/UIEventWithKeyState.h"
#include "core/events/WheelEvent.h"
#include "core/fetch/UniqueIdentifier.h"
#include "core/frame/EventHandlerRegistry.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/PageScaleConstraintsSet.h"
#include "core/frame/PinchViewport.h"
#include "core/frame/RemoteFrame.h"
#include "core/frame/Settings.h"
#include "core/frame/SmartClip.h"
#include "core/frame/TopControls.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/HTMLPlugInElement.h"
#include "core/html/HTMLTextAreaElement.h"
#include "core/html/forms/PopupMenuClient.h"
#include "core/input/EventHandler.h"
#include "core/layout/LayoutPart.h"
#include "core/layout/LayoutView.h"
#include "core/layout/TextAutosizer.h"
#include "core/layout/compositing/DeprecatedPaintLayerCompositor.h"
#include "core/loader/DocumentLoader.h"
#include "core/loader/FrameLoadRequest.h"
#include "core/loader/FrameLoader.h"
#include "core/page/ContextMenuController.h"
#include "core/page/ContextMenuProvider.h"
#include "core/page/DragController.h"
#include "core/page/DragData.h"
#include "core/page/DragSession.h"
#include "core/page/FocusController.h"
#include "core/page/FrameTree.h"
#include "core/page/Page.h"
#include "core/page/PagePopupClient.h"
#include "core/page/PointerLockController.h"
#include "core/page/ScopedPageLoadDeferrer.h"
#include "core/page/TouchDisambiguation.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/timing/DOMWindowPerformance.h"
#include "core/timing/Performance.h"
#include "modules/accessibility/AXObject.h"
#include "modules/accessibility/AXObjectCacheImpl.h"
#include "modules/credentialmanager/CredentialManagerClient.h"
#include "modules/encryptedmedia/MediaKeysController.h"
#include "modules/storage/StorageNamespaceController.h"
#ifdef MINIBLINK_NOT_IMPLEMENTED
#include "modules/webgl/WebGLRenderingContext.h"
#endif // MINIBLINK_NOT_IMPLEMENTED
#include "platform/ContextMenu.h"
#include "platform/ContextMenuItem.h"
#include "platform/Cursor.h"
#include "platform/KeyboardCodes.h"
#include "platform/Logging.h"
#include "platform/NotImplemented.h"
#include "platform/PlatformGestureEvent.h"
#include "platform/PlatformKeyboardEvent.h"
#include "platform/PlatformMouseEvent.h"
#include "platform/PlatformWheelEvent.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "platform/UserGestureIndicator.h"
#include "platform/exported/WebActiveGestureAnimation.h"
#include "platform/fonts/FontCache.h"
#include "platform/graphics/Color.h"
#include "platform/graphics/FirstPaintInvalidationTracking.h"
#include "platform/graphics/Image.h"
#include "platform/graphics/ImageBuffer.h"
#include "platform/graphics/gpu/DrawingBuffer.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "platform/weborigin/SchemeRegistry.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCompositeAndReadbackAsyncCallback.h"
#include "public/platform/WebDragData.h"
#include "public/platform/WebFloatPoint.h"
#include "public/platform/WebGestureCurve.h"
#include "public/platform/WebImage.h"
#include "public/platform/WebLayerTreeView.h"
#include "public/platform/WebURLRequest.h"
#include "public/platform/WebVector.h"
#include "public/web/WebAXObject.h"
#include "public/web/WebActiveWheelFlingParameters.h"
#include "public/web/WebAutofillClient.h"
#include "public/web/WebBeginFrameArgs.h"
#include "public/web/WebElement.h"
#include "public/web/WebFrame.h"
#include "public/web/WebFrameClient.h"
#include "public/web/WebHitTestResult.h"
#include "public/web/WebInputElement.h"
#include "public/web/WebMediaPlayerAction.h"
#include "public/web/WebNode.h"
#include "public/web/WebPlugin.h"
#include "public/web/WebPluginAction.h"
#include "public/web/WebRange.h"
#include "public/web/WebSelection.h"
#include "public/web/WebTextInputInfo.h"
#include "public/web/WebViewClient.h"
#include "public/web/WebWindowFeatures.h"
#include "web/CompositionUnderlineVectorBuilder.h"
#include "web/ContextFeaturesClientImpl.h"
#include "web/DatabaseClientImpl.h"
#include "web/DevToolsEmulator.h"
#include "web/FullscreenController.h"
#include "web/GraphicsLayerFactoryChromium.h"
#include "web/InspectorOverlayImpl.h"
#include "web/LinkHighlight.h"
#include "web/PrerendererClientImpl.h"
#include "web/ResizeViewportAnchor.h"
#include "web/RotationViewportAnchor.h"
#include "web/SpeechRecognitionClientProxy.h"
#include "web/StorageQuotaClientImpl.h"
#include "web/ValidationMessageClientImpl.h"
#include "web/ViewportAnchor.h"
#include "web/WebDevToolsAgentImpl.h"
#include "web/WebInputEventConversion.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebPagePopupImpl.h"
#include "web/WebPluginContainerImpl.h"
#include "web/WebRemoteFrameImpl.h"
#include "web/WebSettingsImpl.h"
#include "web/WorkerGlobalScopeProxyProviderImpl.h"
#include "web/painting/ContinuousPainter.h"
#include "wtf/CurrentTime.h"
#include "wtf/RefPtr.h"
#include "wtf/TemporaryChange.h"

#if USE(DEFAULT_RENDER_THEME)
#include "core/layout/LayoutThemeDefault.h"
#endif

// Get rid of WTF's pow define so we can use std::pow.
#undef pow
#include <cmath> // for std::pow

// The following constants control parameters for automated scaling of webpages
// (such as due to a double tap gesture or find in page etc.). These are
// experimentally determined.
static const int touchPointPadding = 32;
static const int nonUserInitiatedPointPadding = 11;
static const float minScaleDifference = 0.01f;
static const float doubleTapZoomContentDefaultMargin = 5;
static const float doubleTapZoomContentMinimumMargin = 2;
static const double doubleTapZoomAnimationDurationInSeconds = 0.25;
static const float doubleTapZoomAlreadyLegibleRatio = 1.2f;

static const double multipleTargetsZoomAnimationDurationInSeconds = 0.25;
static const double findInPageAnimationDurationInSeconds = 0;

// Constants for viewport anchoring on resize.
static const float viewportAnchorCoordX = 0.5f;
static const float viewportAnchorCoordY = 0;

// Constants for zooming in on a focused text field.
static const double scrollAndScaleAnimationDurationInSeconds = 0.2;
static const int minReadableCaretHeight = 16;
static const int minReadableCaretHeightForTextArea = 13;
static const float minScaleChangeToTriggerZoom = 1.5f;
static const float leftBoxRatio = 0.3f;
static const int caretPadding = 10;

namespace blink {

// Change the text zoom level by kTextSizeMultiplierRatio each time the user
// zooms text in or out (ie., change by 20%).  The min and max values limit
// text zoom to half and 3x the original text size.  These three values match
// those in Apple's port in WebKit/WebKit/WebView/WebView.mm
const double WebView::textSizeMultiplierRatio = 1.2;
const double WebView::minTextSizeMultiplier = 0.5;
const double WebView::maxTextSizeMultiplier = 3.0;

// Used to defer all page activity in cases where the embedder wishes to run
// a nested event loop. Using a stack enables nesting of message loop invocations.
static WillBeHeapVector<RawPtrWillBeMember<ScopedPageLoadDeferrer>>& pageLoadDeferrerStack()
{
    DEFINE_STATIC_LOCAL(OwnPtrWillBePersistent<WillBeHeapVector<RawPtrWillBeMember<ScopedPageLoadDeferrer>>>, deferrerStack, (adoptPtrWillBeNoop(new WillBeHeapVector<RawPtrWillBeMember<ScopedPageLoadDeferrer>>())));
    return *deferrerStack;
}

// Ensure that the WebDragOperation enum values stay in sync with the original
// DragOperation constants.
#define STATIC_ASSERT_MATCHING_ENUM(coreName) \
    static_assert(int(coreName) == int(Web##coreName), "DragOperation and WebDragOperation enum mismatch: " #coreName)
STATIC_ASSERT_MATCHING_ENUM(DragOperationNone);
STATIC_ASSERT_MATCHING_ENUM(DragOperationCopy);
STATIC_ASSERT_MATCHING_ENUM(DragOperationLink);
STATIC_ASSERT_MATCHING_ENUM(DragOperationGeneric);
STATIC_ASSERT_MATCHING_ENUM(DragOperationPrivate);
STATIC_ASSERT_MATCHING_ENUM(DragOperationMove);
STATIC_ASSERT_MATCHING_ENUM(DragOperationDelete);
STATIC_ASSERT_MATCHING_ENUM(DragOperationEvery);

static bool shouldUseExternalPopupMenus = false;

namespace {

class UserGestureNotifier {
public:
    // If a UserGestureIndicator is created for a user gesture since the last
    // page load and *userGestureObserved is false, the UserGestureNotifier
    // will notify the client and set *userGestureObserved to true.
    UserGestureNotifier(WebAutofillClient*, bool* userGestureObserved);
    ~UserGestureNotifier();

private:
    WebAutofillClient* const m_client;
    bool* const m_userGestureObserved;
};

UserGestureNotifier::UserGestureNotifier(WebAutofillClient* client, bool* userGestureObserved)
    : m_client(client)
    , m_userGestureObserved(userGestureObserved)
{
    ASSERT(m_userGestureObserved);
}

UserGestureNotifier::~UserGestureNotifier()
{
    if (!*m_userGestureObserved && UserGestureIndicator::processedUserGestureSinceLoad()) {
        *m_userGestureObserved = true;
        if (m_client)
            m_client->firstUserGestureObserved();
    }
}

class EmptyEventListener final : public EventListener {
public:
    static PassRefPtr<EmptyEventListener> create()
    {
        return adoptRef(new EmptyEventListener());
    }

    bool operator==(const EventListener& other) override
    {
        return this == &other;
    }

private:
    EmptyEventListener()
        : EventListener(CPPEventListenerType)
    {
    }

    void handleEvent(ExecutionContext* executionContext, Event*) override
    {
    }
};

} // namespace

// WebView ----------------------------------------------------------------

WebView* WebView::create(WebViewClient* client)
{
    // Pass the WebViewImpl's self-reference to the caller.
    return WebViewImpl::create(client);
}

WebViewImpl* WebViewImpl::create(WebViewClient* client)
{
    // Pass the WebViewImpl's self-reference to the caller.
    return adoptRef(new WebViewImpl(client)).leakRef();
}

void WebView::setUseExternalPopupMenus(bool useExternalPopupMenus)
{
    shouldUseExternalPopupMenus = useExternalPopupMenus;
}

void WebView::updateVisitedLinkState(unsigned long long linkHash)
{
    Page::visitedStateChanged(linkHash);
}

void WebView::resetVisitedLinkState()
{
    Page::allVisitedStateChanged();
}

void WebView::willEnterModalLoop()
{
    pageLoadDeferrerStack().append(new ScopedPageLoadDeferrer());
}

void WebView::didExitModalLoop()
{
    ASSERT(pageLoadDeferrerStack().size());

    ScopedPageLoadDeferrer* deferrer = pageLoadDeferrerStack().last();
#if ENABLE(OILPAN)
    deferrer->dispose();
#else
    delete deferrer;
#endif
    pageLoadDeferrerStack().removeLast();
}

void WebViewImpl::setMainFrame(WebFrame* frame)
{
    if (frame->isWebLocalFrame()) {
        WebLocalFrameImpl* localFrame = toWebLocalFrameImpl(frame);
        localFrame->initializeCoreFrame(&page()->frameHost(), 0, nullAtom, nullAtom);
        // Composited WebViews want repaints outside the frame visible rect.
        localFrame->frame()->view()->setClipsRepaints(!m_layerTreeView);
    } else {
        toWebRemoteFrameImpl(frame)->initializeCoreFrame(&page()->frameHost(), 0, nullAtom);
    }
}

void WebViewImpl::setCredentialManagerClient(WebCredentialManagerClient* webCredentialManagerClient)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    ASSERT(m_page);
    provideCredentialManagerClientTo(*m_page, new CredentialManagerClient(webCredentialManagerClient));
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
}

void WebViewImpl::setPrerendererClient(WebPrerendererClient* prerendererClient)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    ASSERT(m_page);
    providePrerendererClientTo(*m_page, new PrerendererClientImpl(prerendererClient));
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
}

void WebViewImpl::setSpellCheckClient(WebSpellCheckClient* spellCheckClient)
{
    m_spellCheckClient = spellCheckClient;
}

// static
HashSet<WebViewImpl*>& WebViewImpl::allInstances()
{
    DEFINE_STATIC_LOCAL(HashSet<WebViewImpl*>, allInstances, ());
    return allInstances;
}

WebViewImpl::WebViewImpl(WebViewClient* client)
    : m_client(client)
    , m_spellCheckClient(0)
    , m_chromeClientImpl(this)
    , m_contextMenuClientImpl(this)
    , m_dragClientImpl(this)
    , m_editorClientImpl(this)
    , m_spellCheckerClientImpl(this)
    , m_storageClientImpl(this)
    , m_shouldAutoResize(false)
    , m_zoomLevel(0)
    , m_minimumZoomLevel(zoomFactorToZoomLevel(minTextSizeMultiplier))
    , m_maximumZoomLevel(zoomFactorToZoomLevel(maxTextSizeMultiplier))
    , m_maximumLegibleScale(1)
    , m_doubleTapZoomPageScaleFactor(0)
    , m_doubleTapZoomPending(false)
    , m_enableFakePageScaleAnimationForTesting(false)
    , m_fakePageScaleAnimationPageScaleFactor(0)
    , m_fakePageScaleAnimationUseAnchor(false)
    , m_contextMenuAllowed(false)
    , m_doingDragAndDrop(false)
    , m_ignoreInputEvents(false)
    , m_compositorDeviceScaleFactorOverride(0)
    , m_rootLayerScale(1)
    , m_suppressNextKeypressEvent(false)
    , m_imeAcceptEvents(true)
    , m_operationsAllowed(WebDragOperationNone)
    , m_dragOperation(WebDragOperationNone)
    , m_devToolsEmulator(nullptr)
    , m_isTransparent(false)
    , m_tabsToLinks(false)
    , m_layerTreeView(0)
    , m_rootLayer(0)
    , m_rootGraphicsLayer(0)
    , m_rootTransformLayer(0)
    , m_graphicsLayerFactory(adoptPtr(new GraphicsLayerFactoryChromium(this)))    
    , m_matchesHeuristicsForGpuRasterization(false)
    , m_recreatingGraphicsContext(false)
    , m_flingModifier(0)
    , m_flingSourceDevice(false)
    , m_fullscreenController(FullscreenController::create(this))
    , m_showFPSCounter(false)
    , m_continuousPaintingEnabled(false)
    , m_baseBackgroundColor(Color::white)
    , m_backgroundColorOverride(Color::transparent)
    , m_zoomFactorOverride(0)
    , m_userGestureObserved(false)
    , m_displayMode(WebDisplayModeBrowser)
    , m_elasticOverscroll(FloatSize())
{
    Page::PageClients pageClients;
    pageClients.chromeClient = &m_chromeClientImpl;
    pageClients.contextMenuClient = &m_contextMenuClientImpl;
    pageClients.editorClient = &m_editorClientImpl;
    pageClients.dragClient = &m_dragClientImpl;
    pageClients.spellCheckerClient = &m_spellCheckerClientImpl;

    m_page = adoptPtrWillBeNoop(new Page(pageClients));

#ifdef MINIBLINK_NOT_IMPLEMENTED
    MediaKeysController::provideMediaKeysTo(*m_page, &m_mediaKeysClientImpl);
    provideSpeechRecognitionTo(*m_page, SpeechRecognitionClientProxy::create(client ? client->speechRecognizer() : 0));
    provideDatabaseClientTo(*m_page, DatabaseClientImpl::create());
#endif // MINIBLINK_NOT_IMPLEMENTED
    provideWorkerGlobalScopeProxyProviderTo(*m_page, WorkerGlobalScopeProxyProviderImpl::create());

    provideContextFeaturesTo(*m_page, ContextFeaturesClientImpl::create());
    provideStorageQuotaClientTo(*m_page, StorageQuotaClientImpl::create());
    StorageNamespaceController::provideStorageNamespaceTo(*m_page, &m_storageClientImpl);
    m_page->setValidationMessageClient(ValidationMessageClientImpl::create(*this));

    m_page->makeOrdinary();

    if (m_client) {
        setDeviceScaleFactor(m_client->screenInfo().deviceScaleFactor);
        setVisibilityState(m_client->visibilityState(), true);
    }

    initializeLayerTreeView();

    m_devToolsEmulator = adoptPtr(new DevToolsEmulator(this));

    allInstances().add(this);
}

WebViewImpl::~WebViewImpl()
{
    ASSERT(!m_page);
}

WebDevToolsAgentImpl* WebViewImpl::mainFrameDevToolsAgentImpl()
{
    WebLocalFrameImpl* mainFrame = mainFrameImpl();
    return mainFrame ? mainFrame->devToolsAgentImpl() : nullptr;
}

InspectorOverlay* WebViewImpl::inspectorOverlay()
{
    if (!m_inspectorOverlay)
        m_inspectorOverlay = InspectorOverlayImpl::create(this);
    return m_inspectorOverlay.get();
}

WebLocalFrameImpl* WebViewImpl::mainFrameImpl()
{
    return m_page && m_page->mainFrame() && m_page->mainFrame()->isLocalFrame() ? WebLocalFrameImpl::fromFrame(m_page->deprecatedLocalMainFrame()) : 0;
}

bool WebViewImpl::tabKeyCyclesThroughElements() const
{
    ASSERT(m_page);
    return m_page->tabKeyCyclesThroughElements();
}

void WebViewImpl::setTabKeyCyclesThroughElements(bool value)
{
    if (m_page)
        m_page->setTabKeyCyclesThroughElements(value);
}

void WebViewImpl::handleMouseLeave(LocalFrame& mainFrame, const WebMouseEvent& event)
{
    m_client->setMouseOverURL(WebURL());
    PageWidgetEventHandler::handleMouseLeave(mainFrame, event);
}

void WebViewImpl::handleMouseDown(LocalFrame& mainFrame, const WebMouseEvent& event)
{
    // If there is a popup open, close it as the user is clicking on the page
    // (outside of the popup). We also save it so we can prevent a click on an
    // element from immediately reopening the same popup.
    RefPtr<WebPagePopupImpl> pagePopup;
    if (event.button == WebMouseEvent::ButtonLeft) {
        pagePopup = m_pagePopup;
        hidePopups();
        ASSERT(!m_pagePopup);
    }

    m_lastMouseDownPoint = WebPoint(event.x, event.y);

    // Take capture on a mouse down on a plugin so we can send it mouse events.
    // If the hit node is a plugin but a scrollbar is over it don't start mouse
    // capture because it will interfere with the scrollbar receiving events.
    IntPoint point(event.x, event.y);
    if (event.button == WebMouseEvent::ButtonLeft && m_page->mainFrame()->isLocalFrame()) {
        point = m_page->deprecatedLocalMainFrame()->view()->rootFrameToContents(point);
        HitTestResult result(m_page->deprecatedLocalMainFrame()->eventHandler().hitTestResultAtPoint(point));
        result.setToShadowHostIfInUserAgentShadowRoot();
        Node* hitNode = result.innerNodeOrImageMapImage();

        if (!result.scrollbar() && hitNode && hitNode->layoutObject() && hitNode->layoutObject()->isEmbeddedObject()) {
            m_mouseCaptureNode = hitNode;
            TRACE_EVENT_ASYNC_BEGIN0("input", "capturing mouse", this);
        }
    }

    PageWidgetEventHandler::handleMouseDown(mainFrame, event);

    if (event.button == WebMouseEvent::ButtonLeft && m_mouseCaptureNode)
        m_mouseCaptureGestureToken = mainFrame.eventHandler().takeLastMouseDownGestureToken();

    if (m_pagePopup && pagePopup && m_pagePopup->hasSamePopupClient(pagePopup.get())) {
        // That click triggered a page popup that is the same as the one we just closed.
        // It needs to be closed.
        cancelPagePopup();
    }

    // Dispatch the contextmenu event regardless of if the click was swallowed.
    if (!page()->settings().showContextMenuOnMouseUp()) {
#if OS(MACOSX)
        if (event.button == WebMouseEvent::ButtonRight
            || (event.button == WebMouseEvent::ButtonLeft
                && event.modifiers & WebMouseEvent::ControlKey))
            mouseContextMenu(event);
#else
        if (event.button == WebMouseEvent::ButtonRight)
            mouseContextMenu(event);
#endif
    }
}

void WebViewImpl::setDisplayMode(WebDisplayMode mode)
{
    m_displayMode = mode;
    if (!mainFrameImpl() || !mainFrameImpl()->frameView())
        return;

    mainFrameImpl()->frameView()->setDisplayMode(mode);
}

void WebViewImpl::mouseContextMenu(const WebMouseEvent& event)
{
    if (!mainFrameImpl() || !mainFrameImpl()->frameView())
        return;

    m_page->contextMenuController().clearContextMenu();

    PlatformMouseEventBuilder pme(mainFrameImpl()->frameView(), event);

    // Find the right target frame. See issue 1186900.
    HitTestResult result = hitTestResultForRootFramePos(pme.position());
    Frame* targetFrame;
    if (result.innerNodeOrImageMapImage())
        targetFrame = result.innerNodeOrImageMapImage()->document().frame();
    else
        targetFrame = m_page->focusController().focusedOrMainFrame();

    if (!targetFrame->isLocalFrame())
        return;

    LocalFrame* targetLocalFrame = toLocalFrame(targetFrame);

#if OS(WIN)
    targetLocalFrame->view()->setCursor(pointerCursor());
#endif

    m_contextMenuAllowed = true;
    targetLocalFrame->eventHandler().sendContextMenuEvent(pme, nullptr);
    m_contextMenuAllowed = false;
    // Actually showing the context menu is handled by the ContextMenuClient
    // implementation...
}

void WebViewImpl::handleMouseUp(LocalFrame& mainFrame, const WebMouseEvent& event)
{
    PageWidgetEventHandler::handleMouseUp(mainFrame, event);

    if (page()->settings().showContextMenuOnMouseUp()) {
        // Dispatch the contextmenu event regardless of if the click was swallowed.
        // On Mac/Linux, we handle it on mouse down, not up.
        if (event.button == WebMouseEvent::ButtonRight)
            mouseContextMenu(event);
    }
}

bool WebViewImpl::handleMouseWheel(LocalFrame& mainFrame, const WebMouseWheelEvent& event)
{
    hidePopups();
    return PageWidgetEventHandler::handleMouseWheel(mainFrame, event);
}

bool WebViewImpl::scrollBy(const WebFloatSize& delta, const WebFloatSize& velocity)
{
    if (m_flingSourceDevice == WebGestureDeviceTouchpad) {
        WebMouseWheelEvent syntheticWheel;
        const float tickDivisor = WheelEvent::TickMultiplier;

        syntheticWheel.deltaX = delta.width;
        syntheticWheel.deltaY = delta.height;
        syntheticWheel.wheelTicksX = delta.width / tickDivisor;
        syntheticWheel.wheelTicksY = delta.height / tickDivisor;
        syntheticWheel.hasPreciseScrollingDeltas = true;
        syntheticWheel.x = m_positionOnFlingStart.x;
        syntheticWheel.y = m_positionOnFlingStart.y;
        syntheticWheel.globalX = m_globalPositionOnFlingStart.x;
        syntheticWheel.globalY = m_globalPositionOnFlingStart.y;
        syntheticWheel.modifiers = m_flingModifier;

        if (m_page && m_page->mainFrame() && m_page->mainFrame()->isLocalFrame() && m_page->deprecatedLocalMainFrame()->view())
            return handleMouseWheel(*m_page->deprecatedLocalMainFrame(), syntheticWheel);
    } else {
        WebGestureEvent syntheticGestureEvent;

        syntheticGestureEvent.type = WebInputEvent::GestureScrollUpdate;
        syntheticGestureEvent.data.scrollUpdate.preventPropagation = true;
        syntheticGestureEvent.data.scrollUpdate.deltaX = delta.width;
        syntheticGestureEvent.data.scrollUpdate.deltaY = delta.height;
        syntheticGestureEvent.data.scrollUpdate.velocityX = velocity.width;
        syntheticGestureEvent.data.scrollUpdate.velocityY = velocity.height;
        syntheticGestureEvent.x = m_positionOnFlingStart.x;
        syntheticGestureEvent.y = m_positionOnFlingStart.y;
        syntheticGestureEvent.globalX = m_globalPositionOnFlingStart.x;
        syntheticGestureEvent.globalY = m_globalPositionOnFlingStart.y;
        syntheticGestureEvent.modifiers = m_flingModifier;
        syntheticGestureEvent.sourceDevice = WebGestureDeviceTouchscreen;
        syntheticGestureEvent.data.scrollUpdate.inertial = true;

        if (m_page && m_page->mainFrame() && m_page->mainFrame()->isLocalFrame() && m_page->deprecatedLocalMainFrame()->view())
            return handleGestureEvent(syntheticGestureEvent);
    }
    return false;
}

bool WebViewImpl::handleGestureEvent(const WebGestureEvent& event)
{
    if (!m_client)
        return false;

    bool eventSwallowed = false;
    bool eventCancelled = false; // for disambiguation

    // Special handling for slow-path fling gestures.
    switch (event.type) {
    case WebInputEvent::GestureFlingStart: {
        if (mainFrameImpl()->frame()->eventHandler().isScrollbarHandlingGestures())
            break;
        endActiveFlingAnimation();
        m_client->cancelScheduledContentIntents();
        m_positionOnFlingStart = WebPoint(event.x, event.y);
        m_globalPositionOnFlingStart = WebPoint(event.globalX, event.globalY);
        m_flingModifier = event.modifiers;
        m_flingSourceDevice = event.sourceDevice;
        OwnPtr<WebGestureCurve> flingCurve = adoptPtr(Platform::current()->createFlingAnimationCurve(event.sourceDevice, WebFloatPoint(event.data.flingStart.velocityX, event.data.flingStart.velocityY), WebSize()));
        ASSERT(flingCurve);
        m_gestureAnimation = WebActiveGestureAnimation::createAtAnimationStart(flingCurve.release(), this);
        scheduleAnimation();
        eventSwallowed = true;

        m_client->didHandleGestureEvent(event, eventCancelled);
        return eventSwallowed;
    }
    case WebInputEvent::GestureFlingCancel:
        if (endActiveFlingAnimation())
            eventSwallowed = true;

        m_client->didHandleGestureEvent(event, eventCancelled);
        return eventSwallowed;
    default:
        break;
    }

    PlatformGestureEventBuilder platformEvent(mainFrameImpl()->frameView(), event);

    // Special handling for double tap and scroll events as we don't want to
    // hit test for them.
    switch (event.type) {
    case WebInputEvent::GestureDoubleTap:
        if (m_webSettings->doubleTapToZoomEnabled() && minimumPageScaleFactor() != maximumPageScaleFactor()) {
            m_client->cancelScheduledContentIntents();
            animateDoubleTapZoom(platformEvent.position());
        }
        // GestureDoubleTap is currently only used by Android for zooming. For WebCore,
        // GestureTap with tap count = 2 is used instead. So we drop GestureDoubleTap here.
        eventSwallowed = true;
        m_client->didHandleGestureEvent(event, eventCancelled);
        return eventSwallowed;
    case WebInputEvent::GestureScrollBegin:
        m_client->cancelScheduledContentIntents();
    case WebInputEvent::GestureScrollEnd:
    case WebInputEvent::GestureScrollUpdate:
    case WebInputEvent::GestureFlingStart:
        // Scrolling-related gesture events invoke EventHandler recursively for each frame down
        // the chain, doing a single-frame hit-test per frame. This matches handleWheelEvent.
        // Perhaps we could simplify things by rewriting scroll handling to work inner frame
        // out, and then unify with other gesture events.
        eventSwallowed = mainFrameImpl()->frame()->eventHandler().handleGestureScrollEvent(platformEvent);
        m_client->didHandleGestureEvent(event, eventCancelled);
        return eventSwallowed;
    case WebInputEvent::GesturePinchBegin:
    case WebInputEvent::GesturePinchEnd:
    case WebInputEvent::GesturePinchUpdate:
        return false;
    default:
        break;
    }

    // Hit test across all frames and do touch adjustment as necessary for the event type.
    GestureEventWithHitTestResults targetedEvent =
        m_page->deprecatedLocalMainFrame()->eventHandler().targetGestureEvent(platformEvent);

    // Handle link highlighting outside the main switch to avoid getting lost in the
    // complicated set of cases handled below.
    switch (event.type) {
    case WebInputEvent::GestureShowPress:
        // Queue a highlight animation, then hand off to regular handler.
        enableTapHighlightAtPoint(targetedEvent);
        break;
    case WebInputEvent::GestureTapCancel:
    case WebInputEvent::GestureTap:
    case WebInputEvent::GestureLongPress:
        for (size_t i = 0; i < m_linkHighlights.size(); ++i)
            m_linkHighlights[i]->startHighlightAnimationIfNeeded();
        break;
    default:
        break;
    }

    switch (event.type) {
    case WebInputEvent::GestureTap: {
        m_client->cancelScheduledContentIntents();
        if (detectContentOnTouch(targetedEvent)) {
            eventSwallowed = true;
            break;
        }

        // Don't trigger a disambiguation popup on sites designed for mobile devices.
        // Instead, assume that the page has been designed with big enough buttons and links.
        // Don't trigger a disambiguation popup when screencasting, since it's implemented outside of
        // compositor pipeline and is not being screencasted itself. This leads to bad user experience.
        WebDevToolsAgentImpl* devTools = mainFrameDevToolsAgentImpl();
        bool screencastEnabled = devTools && devTools->screencastEnabled();
        if (event.data.tap.width > 0 && !shouldDisableDesktopWorkarounds() && !screencastEnabled) {
            IntRect boundingBox(page()->frameHost().pinchViewport().viewportToRootFrame(IntRect(
                event.x - event.data.tap.width / 2,
                event.y - event.data.tap.height / 2,
                event.data.tap.width,
                event.data.tap.height)));

            // FIXME: We shouldn't pass details of the PinchViewport offset to render_view_impl.
            WebSize pinchViewportOffset = flooredIntSize(page()->frameHost().pinchViewport().location());

            if (m_webSettings->multiTargetTapNotificationEnabled()) {
                Vector<IntRect> goodTargets;
                WillBeHeapVector<RawPtrWillBeMember<Node>> highlightNodes;
                findGoodTouchTargets(boundingBox, mainFrameImpl()->frame(), goodTargets, highlightNodes);
                // FIXME: replace touch adjustment code when numberOfGoodTargets == 1?
                // Single candidate case is currently handled by: https://bugs.webkit.org/show_bug.cgi?id=85101
                if (goodTargets.size() >= 2 && m_client
                    && m_client->didTapMultipleTargets(pinchViewportOffset, boundingBox, goodTargets)) {

                    enableTapHighlights(highlightNodes);
                    for (size_t i = 0; i < m_linkHighlights.size(); ++i)
                        m_linkHighlights[i]->startHighlightAnimationIfNeeded();
                    eventSwallowed = true;
                    eventCancelled = true;
                    break;
                }
            }
        }

        eventSwallowed = mainFrameImpl()->frame()->eventHandler().handleGestureEvent(targetedEvent);
        break;
    }
    case WebInputEvent::GestureTwoFingerTap:
    case WebInputEvent::GestureLongPress:
    case WebInputEvent::GestureLongTap: {
        if (!mainFrameImpl() || !mainFrameImpl()->frameView())
            break;

        m_client->cancelScheduledContentIntents();
        m_page->contextMenuController().clearContextMenu();
        m_contextMenuAllowed = true;
        eventSwallowed = mainFrameImpl()->frame()->eventHandler().handleGestureEvent(targetedEvent);
        m_contextMenuAllowed = false;

        break;
    }
    case WebInputEvent::GestureShowPress:
        m_client->cancelScheduledContentIntents();
    case WebInputEvent::GestureTapDown:
    case WebInputEvent::GestureTapCancel:
    case WebInputEvent::GestureTapUnconfirmed: {
        eventSwallowed = mainFrameImpl()->frame()->eventHandler().handleGestureEvent(targetedEvent);
        break;
    }
    default:
        ASSERT_NOT_REACHED();
    }
    m_client->didHandleGestureEvent(event, eventCancelled);
    return eventSwallowed;
}

bool WebViewImpl::handleSyntheticWheelFromTouchpadPinchEvent(const WebGestureEvent& pinchEvent)
{
    ASSERT(pinchEvent.type == WebInputEvent::GesturePinchUpdate);

    // Touchscreen pinch events should not reach Blink.
    ASSERT(pinchEvent.sourceDevice == WebGestureDeviceTouchpad);

    // For pinch gesture events, match typical trackpad behavior on Windows by sending fake
    // wheel events with the ctrl modifier set when we see trackpad pinch gestures.  Ideally
    // we'd someday get a platform 'pinch' event and send that instead.
    WebMouseWheelEvent wheelEvent;
    wheelEvent.type = WebInputEvent::MouseWheel;
    wheelEvent.timeStampSeconds = pinchEvent.timeStampSeconds;
    wheelEvent.windowX = wheelEvent.x = pinchEvent.x;
    wheelEvent.windowY = wheelEvent.y = pinchEvent.y;
    wheelEvent.globalX = pinchEvent.globalX;
    wheelEvent.globalY = pinchEvent.globalY;
    wheelEvent.modifiers =
        pinchEvent.modifiers | WebInputEvent::ControlKey;
    wheelEvent.deltaX = 0;

    // The function to convert scales to deltaY values is designed to be
    // compatible with websites existing use of wheel events, and with existing
    // Windows trackpad behavior.  In particular, we want:
    //  - deltas should accumulate via addition: f(s1*s2)==f(s1)+f(s2)
    //  - deltas should invert via negation: f(1/s) == -f(s)
    //  - zoom in should be positive: f(s) > 0 iff s > 1
    //  - magnitude roughly matches wheels: f(2) > 25 && f(2) < 100
    //  - a formula that's relatively easy to use from JavaScript
    // Note that 'wheel' event deltaY values have their sign inverted.  So to
    // convert a wheel deltaY back to a scale use Math.exp(-deltaY/100).
    ASSERT(pinchEvent.data.pinchUpdate.scale > 0);
    wheelEvent.deltaY = 100.0f * log(pinchEvent.data.pinchUpdate.scale);
    wheelEvent.hasPreciseScrollingDeltas = true;
    wheelEvent.wheelTicksX = 0;
    wheelEvent.wheelTicksY =
        pinchEvent.data.pinchUpdate.scale > 1 ? 1 : -1;
    wheelEvent.canScroll = false;

    return handleInputEvent(wheelEvent);
}

void WebViewImpl::transferActiveWheelFlingAnimation(const WebActiveWheelFlingParameters& parameters)
{
    TRACE_EVENT0("blink", "WebViewImpl::transferActiveWheelFlingAnimation");
    ASSERT(!m_gestureAnimation);
    m_positionOnFlingStart = parameters.point;
    m_globalPositionOnFlingStart = parameters.globalPoint;
    m_flingModifier = parameters.modifiers;
    OwnPtr<WebGestureCurve> curve = adoptPtr(Platform::current()->createFlingAnimationCurve(parameters.sourceDevice, WebFloatPoint(parameters.delta), parameters.cumulativeScroll));
    ASSERT(curve);
    m_gestureAnimation = WebActiveGestureAnimation::createWithTimeOffset(curve.release(), this, parameters.startTime);
    scheduleAnimation();
}

bool WebViewImpl::endActiveFlingAnimation()
{
    if (m_gestureAnimation) {
        m_gestureAnimation.clear();
        if (m_layerTreeView)
            m_layerTreeView->didStopFlinging();
        return true;
    }
    return false;
}

bool WebViewImpl::startPageScaleAnimation(const IntPoint& targetPosition, bool useAnchor, float newScale, double durationInSeconds)
{
    PinchViewport& pinchViewport = page()->frameHost().pinchViewport();
    WebPoint clampedPoint = targetPosition;
    if (!useAnchor) {
        clampedPoint = pinchViewport.clampDocumentOffsetAtScale(targetPosition, newScale);
        if (!durationInSeconds) {
            setPageScaleFactor(newScale);

            FrameView* view = mainFrameImpl()->frameView();
            if (view && view->scrollableArea())
                view->scrollableArea()->setScrollPosition(DoublePoint(clampedPoint.x, clampedPoint.y), ProgrammaticScroll);

            return false;
        }
    }
    if (useAnchor && newScale == pageScaleFactor())
        return false;

    if (m_enableFakePageScaleAnimationForTesting) {
        m_fakePageScaleAnimationTargetPosition = targetPosition;
        m_fakePageScaleAnimationUseAnchor = useAnchor;
        m_fakePageScaleAnimationPageScaleFactor = newScale;
    } else {
        if (!m_layerTreeView)
            return false;
        m_layerTreeView->startPageScaleAnimation(targetPosition, useAnchor, newScale, durationInSeconds);
    }
    return true;
}

void WebViewImpl::enableFakePageScaleAnimationForTesting(bool enable)
{
    m_enableFakePageScaleAnimationForTesting = enable;
}

void WebViewImpl::setShowFPSCounter(bool show)
{
    if (m_layerTreeView) {
        TRACE_EVENT0("blink", "WebViewImpl::setShowFPSCounter");
        // FIXME: allow emulation, fps counter and continuous painting at the same time: crbug.com/299837.
        m_layerTreeView->setShowFPSCounter(show && !m_devToolsEmulator->deviceEmulationEnabled());
    }
    m_showFPSCounter = show;
}

void WebViewImpl::setShowPaintRects(bool show)
{
    if (m_layerTreeView) {
        TRACE_EVENT0("blink", "WebViewImpl::setShowPaintRects");
        m_layerTreeView->setShowPaintRects(show);
    }
    setFirstPaintInvalidationTrackingEnabledForShowPaintRects(show);
}

void WebViewImpl::setShowDebugBorders(bool show)
{
    if (m_layerTreeView)
        m_layerTreeView->setShowDebugBorders(show);
}

void WebViewImpl::setContinuousPaintingEnabled(bool enabled)
{
    if (m_layerTreeView) {
        TRACE_EVENT0("blink", "WebViewImpl::setContinuousPaintingEnabled");
        // FIXME: allow emulation, fps counter and continuous painting at the same time: crbug.com/299837.
        m_layerTreeView->setContinuousPaintingEnabled(enabled && !m_devToolsEmulator->deviceEmulationEnabled());
    }
    m_continuousPaintingEnabled = enabled;
    if (m_client)
        m_client->scheduleAnimation();
}

void WebViewImpl::updateShowFPSCounterAndContinuousPainting()
{
    if (m_layerTreeView) {
        // FIXME: allow emulation, fps counter and continuous painting at the same time: crbug.com/299837.
        m_layerTreeView->setContinuousPaintingEnabled(m_continuousPaintingEnabled && !m_devToolsEmulator->deviceEmulationEnabled());
        m_layerTreeView->setShowFPSCounter(m_showFPSCounter && !m_devToolsEmulator->deviceEmulationEnabled());
    }
}

void WebViewImpl::setShowScrollBottleneckRects(bool show)
{
    if (m_layerTreeView)
        m_layerTreeView->setShowScrollBottleneckRects(show);
}

void WebViewImpl::acceptLanguagesChanged()
{
    if (!page())
        return;

    page()->acceptLanguagesChanged();
}

bool WebViewImpl::handleKeyEvent(const WebKeyboardEvent& event)
{
    ASSERT((event.type == WebInputEvent::RawKeyDown)
        || (event.type == WebInputEvent::KeyDown)
        || (event.type == WebInputEvent::KeyUp));

    // Halt an in-progress fling on a key event.
    endActiveFlingAnimation();

    // Please refer to the comments explaining the m_suppressNextKeypressEvent
    // member.
    // The m_suppressNextKeypressEvent is set if the KeyDown is handled by
    // Webkit. A keyDown event is typically associated with a keyPress(char)
    // event and a keyUp event. We reset this flag here as this is a new keyDown
    // event.
    m_suppressNextKeypressEvent = false;

    // If there is a popup, it should be the one processing the event, not the
    // page.
    if (m_pagePopup) {
        m_pagePopup->handleKeyEvent(PlatformKeyboardEventBuilder(event));
        // We need to ignore the next Char event after this otherwise pressing
        // enter when selecting an item in the popup will go to the page.
        if (WebInputEvent::RawKeyDown == event.type)
            m_suppressNextKeypressEvent = true;
        return true;
    }

    RefPtrWillBeRawPtr<Frame> focusedFrame = focusedCoreFrame();
    if (focusedFrame && focusedFrame->isRemoteFrame()) {
        WebRemoteFrameImpl* webFrame = WebRemoteFrameImpl::fromFrame(*toRemoteFrame(focusedFrame.get()));
        webFrame->client()->forwardInputEvent(&event);
        return true;
    }

    if (!focusedFrame || !focusedFrame->isLocalFrame())
        return false;

    LocalFrame* frame = toLocalFrame(focusedFrame.get());

    PlatformKeyboardEventBuilder evt(event);

    if (frame->eventHandler().keyEvent(evt)) {
        if (WebInputEvent::RawKeyDown == event.type) {
            // Suppress the next keypress event unless the focused node is a plugin node.
            // (Flash needs these keypress events to handle non-US keyboards.)
            Element* element = focusedElement();
            if (element && element->layoutObject() && element->layoutObject()->isEmbeddedObject()) {
                if (event.windowsKeyCode == VKEY_TAB) {
                    // If the plugin supports keyboard focus then we should not send a tab keypress event.
                    Widget* widget = toLayoutPart(element->layoutObject())->widget();
                    if (widget && widget->isPluginContainer()) {
                        WebPluginContainerImpl* plugin = toWebPluginContainerImpl(widget);
                        if (plugin && plugin->supportsKeyboardFocus())
                            m_suppressNextKeypressEvent = true;
                    }
                }
            } else {
                m_suppressNextKeypressEvent = true;
            }
        }
        return true;
    }

#if !OS(MACOSX)
    const WebInputEvent::Type contextMenuTriggeringEventType =
#if OS(WIN)
        WebInputEvent::KeyUp;
#else
        WebInputEvent::RawKeyDown;
#endif

    bool isUnmodifiedMenuKey = !(event.modifiers & WebInputEvent::InputModifiers) && event.windowsKeyCode == VKEY_APPS;
    bool isShiftF10 = event.modifiers == WebInputEvent::ShiftKey && event.windowsKeyCode == VKEY_F10;
    if ((isUnmodifiedMenuKey || isShiftF10) && event.type == contextMenuTriggeringEventType) {
        sendContextMenuEvent(event);
        return true;
    }
#endif // !OS(MACOSX)

    return keyEventDefault(event);
}

bool WebViewImpl::handleCharEvent(const WebKeyboardEvent& event)
{
    ASSERT(event.type == WebInputEvent::Char);

    // Please refer to the comments explaining the m_suppressNextKeypressEvent
    // member.  The m_suppressNextKeypressEvent is set if the KeyDown is
    // handled by Webkit. A keyDown event is typically associated with a
    // keyPress(char) event and a keyUp event. We reset this flag here as it
    // only applies to the current keyPress event.
    bool suppress = m_suppressNextKeypressEvent;
    m_suppressNextKeypressEvent = false;

    // If there is a popup, it should be the one processing the event, not the
    // page.
    if (m_pagePopup)
        return m_pagePopup->handleKeyEvent(PlatformKeyboardEventBuilder(event));

    LocalFrame* frame = toLocalFrame(focusedCoreFrame());
    if (!frame)
        return suppress;

    EventHandler& handler = frame->eventHandler();

    PlatformKeyboardEventBuilder evt(event);
    if (!evt.isCharacterKey())
        return true;

    // Accesskeys are triggered by char events and can't be suppressed.
    if (handler.handleAccessKey(evt))
        return true;

    // Safari 3.1 does not pass off windows system key messages (WM_SYSCHAR) to
    // the eventHandler::keyEvent. We mimic this behavior on all platforms since
    // for now we are converting other platform's key events to windows key
    // events.
    if (evt.isSystemKey())
        return false;

    if (!suppress && !handler.keyEvent(evt))
        return keyEventDefault(event);

    return true;
}

WebRect WebViewImpl::computeBlockBound(const WebPoint& pointInRootFrame, bool ignoreClipping)
{
    if (!mainFrameImpl())
        return WebRect();

    // Use the point-based hit test to find the node.
    IntPoint point = mainFrameImpl()->frameView()->rootFrameToContents(IntPoint(pointInRootFrame.x, pointInRootFrame.y));
    HitTestRequest::HitTestRequestType hitType = HitTestRequest::ReadOnly | HitTestRequest::Active | (ignoreClipping ? HitTestRequest::IgnoreClipping : 0);
    HitTestResult result = mainFrameImpl()->frame()->eventHandler().hitTestResultAtPoint(point, hitType);
    result.setToShadowHostIfInUserAgentShadowRoot();

    Node* node = result.innerNodeOrImageMapImage();
    if (!node)
        return WebRect();

    // Find the block type node based on the hit node.
    // FIXME: This wants to walk composed tree with LayoutTreeBuilderTraversal::parent().
    while (node && (!node->layoutObject() || node->layoutObject()->isInline()))
        node = LayoutTreeBuilderTraversal::parent(*node);

    // Return the bounding box in the root frame's coordinate space.
    if (node) {
        IntRect pointInRootFrame = node->Node::pixelSnappedBoundingBox();
        LocalFrame* frame = node->document().frame();
        return frame->view()->contentsToRootFrame(pointInRootFrame);
    }
    return WebRect();
}

WebRect WebViewImpl::widenRectWithinPageBounds(const WebRect& source, int targetMargin, int minimumMargin)
{
    WebSize maxSize;
    if (mainFrame())
        maxSize = mainFrame()->contentsSize();
    IntSize scrollOffset;
    if (mainFrame())
        scrollOffset = mainFrame()->scrollOffset();
    int leftMargin = targetMargin;
    int rightMargin = targetMargin;

    const int absoluteSourceX = source.x + scrollOffset.width();
    if (leftMargin > absoluteSourceX) {
        leftMargin = absoluteSourceX;
        rightMargin = std::max(leftMargin, minimumMargin);
    }

    const int maximumRightMargin = maxSize.width - (source.width + absoluteSourceX);
    if (rightMargin > maximumRightMargin) {
        rightMargin = maximumRightMargin;
        leftMargin = std::min(leftMargin, std::max(rightMargin, minimumMargin));
    }

    const int newWidth = source.width + leftMargin + rightMargin;
    const int newX = source.x - leftMargin;

    ASSERT(newWidth >= 0);
    ASSERT(scrollOffset.width() + newX + newWidth <= maxSize.width);

    return WebRect(newX, source.y, newWidth, source.height);
}

float WebViewImpl::maximumLegiblePageScale() const
{
    // Pages should be as legible as on desktop when at dpi scale, so no
    // need to zoom in further when automatically determining zoom level
    // (after double tap, find in page, etc), though the user should still
    // be allowed to manually pinch zoom in further if they desire.
    if (page())
        return m_maximumLegibleScale * page()->settings().accessibilityFontScaleFactor();
    return m_maximumLegibleScale;
}

void WebViewImpl::computeScaleAndScrollForBlockRect(const WebPoint& hitPointInRootFrame, const WebRect& blockRectInRootFrame, float padding, float defaultScaleWhenAlreadyLegible, float& scale, WebPoint& scroll)
{
    scale = pageScaleFactor();
    scroll.x = scroll.y = 0;

    WebRect rect = blockRectInRootFrame;

    if (!rect.isEmpty()) {
        float defaultMargin = doubleTapZoomContentDefaultMargin;
        float minimumMargin = doubleTapZoomContentMinimumMargin;
        // We want the margins to have the same physical size, which means we
        // need to express them in post-scale size. To do that we'd need to know
        // the scale we're scaling to, but that depends on the margins. Instead
        // we express them as a fraction of the target rectangle: this will be
        // correct if we end up fully zooming to it, and won't matter if we
        // don't.
        rect = widenRectWithinPageBounds(rect,
                static_cast<int>(defaultMargin * rect.width / m_size.width),
                static_cast<int>(minimumMargin * rect.width / m_size.width));
        // Fit block to screen, respecting limits.
        scale = static_cast<float>(m_size.width) / rect.width;
        scale = std::min(scale, maximumLegiblePageScale());
        if (pageScaleFactor() < defaultScaleWhenAlreadyLegible)
            scale = std::max(scale, defaultScaleWhenAlreadyLegible);
        scale = clampPageScaleFactorToLimits(scale);
    }

    // FIXME: If this is being called for auto zoom during find in page,
    // then if the user manually zooms in it'd be nice to preserve the
    // relative increase in zoom they caused (if they zoom out then it's ok
    // to zoom them back in again). This isn't compatible with our current
    // double-tap zoom strategy (fitting the containing block to the screen)
    // though.

    float screenWidth = m_size.width / scale;
    float screenHeight = m_size.height / scale;

    // Scroll to vertically align the block.
    if (rect.height < screenHeight) {
        // Vertically center short blocks.
        rect.y -= 0.5 * (screenHeight - rect.height);
    } else {
        // Ensure position we're zooming to (+ padding) isn't off the bottom of
        // the screen.
        rect.y = std::max<float>(rect.y, hitPointInRootFrame.y + padding - screenHeight);
    } // Otherwise top align the block.

    // Do the same thing for horizontal alignment.
    if (rect.width < screenWidth)
        rect.x -= 0.5 * (screenWidth - rect.width);
    else
        rect.x = std::max<float>(rect.x, hitPointInRootFrame.x + padding - screenWidth);
    scroll.x = rect.x;
    scroll.y = rect.y;

    scale = clampPageScaleFactorToLimits(scale);
    scroll = mainFrameImpl()->frameView()->rootFrameToContents(scroll);
    scroll = page()->frameHost().pinchViewport().clampDocumentOffsetAtScale(scroll, scale);
}

static Node* findCursorDefiningAncestor(Node* node, LocalFrame* frame)
{
    // Go up the tree to find the node that defines a mouse cursor style
    while (node) {
        if (node->layoutObject()) {
            ECursor cursor = node->layoutObject()->style()->cursor();
            if (cursor != CURSOR_AUTO || frame->eventHandler().useHandCursor(node, node->isLink()))
                break;
        }
        node = LayoutTreeBuilderTraversal::parent(*node);
    }

    return node;
}

static bool showsHandCursor(Node* node, LocalFrame* frame)
{
    if (!node || !node->layoutObject())
        return false;

    ECursor cursor = node->layoutObject()->style()->cursor();
    return cursor == CURSOR_POINTER
        || (cursor == CURSOR_AUTO && frame->eventHandler().useHandCursor(node, node->isLink()));
}

Node* WebViewImpl::bestTapNode(const GestureEventWithHitTestResults& targetedTapEvent)
{
    TRACE_EVENT0("input", "WebViewImpl::bestTapNode");

    if (!m_page || !m_page->mainFrame())
        return 0;

    Node* bestTouchNode = targetedTapEvent.hitTestResult().innerNode();
    if (!bestTouchNode)
        return nullptr;

    // We might hit something like an image map that has no layoutObject on it
    // Walk up the tree until we have a node with an attached layoutObject
    while (!bestTouchNode->layoutObject()) {
        bestTouchNode = LayoutTreeBuilderTraversal::parent(*bestTouchNode);
        if (!bestTouchNode)
            return nullptr;
    }

    // Editable nodes should not be highlighted (e.g., <input>)
    if (bestTouchNode->hasEditableStyle())
        return nullptr;

    Node* cursorDefiningAncestor =
        findCursorDefiningAncestor(bestTouchNode, m_page->deprecatedLocalMainFrame());
    // We show a highlight on tap only when the current node shows a hand cursor
    if (!cursorDefiningAncestor || !showsHandCursor(cursorDefiningAncestor, m_page->deprecatedLocalMainFrame())) {
        return 0;
    }

    // We should pick the largest enclosing node with hand cursor set. We do this by first jumping
    // up to cursorDefiningAncestor (which is already known to have hand cursor set). Then we locate
    // the next cursor-defining ancestor up in the the tree and repeat the jumps as long as the node
    // has hand cursor set.
    do {
        bestTouchNode = cursorDefiningAncestor;
        cursorDefiningAncestor = findCursorDefiningAncestor(LayoutTreeBuilderTraversal::parent(*bestTouchNode),
            m_page->deprecatedLocalMainFrame());
    } while (cursorDefiningAncestor && showsHandCursor(cursorDefiningAncestor, m_page->deprecatedLocalMainFrame()));

    return bestTouchNode;
}

void WebViewImpl::enableTapHighlightAtPoint(const GestureEventWithHitTestResults& targetedTapEvent)
{
    Node* touchNode = bestTapNode(targetedTapEvent);

    WillBeHeapVector<RawPtrWillBeMember<Node>> highlightNodes;
    highlightNodes.append(touchNode);

    enableTapHighlights(highlightNodes);
}

void WebViewImpl::enableTapHighlights(WillBeHeapVector<RawPtrWillBeMember<Node>>& highlightNodes)
{
    if (highlightNodes.isEmpty())
        return;

    // Always clear any existing highlight when this is invoked, even if we
    // don't get a new target to highlight.
    m_linkHighlights.clear();

    // LinkHighlight reads out layout and compositing state, so we need to make sure that's all up to date.
    layout();

    for (size_t i = 0; i < highlightNodes.size(); ++i) {
        Node* node = highlightNodes[i];

        if (!node || !node->layoutObject())
            continue;

        Color highlightColor = node->layoutObject()->style()->tapHighlightColor();
        // Safari documentation for -webkit-tap-highlight-color says if the specified color has 0 alpha,
        // then tap highlighting is disabled.
        // http://developer.apple.com/library/safari/#documentation/appleapplications/reference/safaricssref/articles/standardcssproperties.html
        if (!highlightColor.alpha())
            continue;

        m_linkHighlights.append(LinkHighlight::create(node, this));
    }
}

void WebViewImpl::animateDoubleTapZoom(const IntPoint& pointInRootFrame)
{
    if (!mainFrameImpl())
        return;

    WebRect blockBounds = computeBlockBound(pointInRootFrame, false);
    float scale;
    WebPoint scroll;

    computeScaleAndScrollForBlockRect(pointInRootFrame, blockBounds, touchPointPadding, minimumPageScaleFactor() * doubleTapZoomAlreadyLegibleRatio, scale, scroll);

    bool stillAtPreviousDoubleTapScale = (pageScaleFactor() == m_doubleTapZoomPageScaleFactor
        && m_doubleTapZoomPageScaleFactor != minimumPageScaleFactor())
        || m_doubleTapZoomPending;

    bool scaleUnchanged = fabs(pageScaleFactor() - scale) < minScaleDifference;
    bool shouldZoomOut = blockBounds.isEmpty() || scaleUnchanged || stillAtPreviousDoubleTapScale;

    bool isAnimating;

    if (shouldZoomOut) {
        scale = minimumPageScaleFactor();
        isAnimating = startPageScaleAnimation(mainFrameImpl()->frameView()->rootFrameToContents(pointInRootFrame), true, scale, doubleTapZoomAnimationDurationInSeconds);
    } else {
        isAnimating = startPageScaleAnimation(scroll, false, scale, doubleTapZoomAnimationDurationInSeconds);
    }

    if (isAnimating) {
        m_doubleTapZoomPageScaleFactor = scale;
        m_doubleTapZoomPending = true;
    }
}

void WebViewImpl::zoomToFindInPageRect(const WebRect& rectInRootFrame)
{
    if (!mainFrameImpl())
        return;

    WebRect blockBounds = computeBlockBound(WebPoint(rectInRootFrame.x + rectInRootFrame.width / 2, rectInRootFrame.y + rectInRootFrame.height / 2), true);

    if (blockBounds.isEmpty()) {
        // Keep current scale (no need to scroll as x,y will normally already
        // be visible). FIXME: Revisit this if it isn't always true.
        return;
    }

    float scale;
    WebPoint scroll;

    computeScaleAndScrollForBlockRect(WebPoint(rectInRootFrame.x, rectInRootFrame.y), blockBounds, nonUserInitiatedPointPadding, minimumPageScaleFactor(), scale, scroll);

    startPageScaleAnimation(scroll, false, scale, findInPageAnimationDurationInSeconds);
}

bool WebViewImpl::zoomToMultipleTargetsRect(const WebRect& rectInRootFrame)
{
    if (!mainFrameImpl())
        return false;

    float scale;
    WebPoint scroll;

    computeScaleAndScrollForBlockRect(WebPoint(rectInRootFrame.x, rectInRootFrame.y), rectInRootFrame, nonUserInitiatedPointPadding, minimumPageScaleFactor(), scale, scroll);

    if (scale <= pageScaleFactor())
        return false;

    startPageScaleAnimation(scroll, false, scale, multipleTargetsZoomAnimationDurationInSeconds);
    return true;
}

void WebViewImpl::hasTouchEventHandlers(bool hasTouchHandlers)
{
    if (m_client)
        m_client->hasTouchEventHandlers(hasTouchHandlers);
}

bool WebViewImpl::hasTouchEventHandlersAt(const WebPoint& point)
{
    // FIXME: Implement this. Note that the point must be divided by pageScaleFactor.
    return true;
}

#if !OS(MACOSX)
// Mac has no way to open a context menu based on a keyboard event.
bool WebViewImpl::sendContextMenuEvent(const WebKeyboardEvent& event)
{
    // The contextMenuController() holds onto the last context menu that was
    // popped up on the page until a new one is created. We need to clear
    // this menu before propagating the event through the DOM so that we can
    // detect if we create a new menu for this event, since we won't create
    // a new menu if the DOM swallows the event and the defaultEventHandler does
    // not run.
    page()->contextMenuController().clearContextMenu();

    m_contextMenuAllowed = true;
    Frame* focusedFrame = page()->focusController().focusedOrMainFrame();
    if (!focusedFrame->isLocalFrame())
        return false;
    bool handled = toLocalFrame(focusedFrame)->eventHandler().sendContextMenuEventForKey(nullptr);
    m_contextMenuAllowed = false;
    return handled;
}
#endif

void WebViewImpl::showContextMenuAtPoint(float x, float y, PassRefPtrWillBeRawPtr<ContextMenuProvider> menuProvider)
{
    if (!page()->mainFrame()->isLocalFrame())
        return;
    m_contextMenuAllowed = true;
    page()->contextMenuController().clearContextMenu();
    page()->contextMenuController().showContextMenuAtPoint(page()->deprecatedLocalMainFrame(), x, y, menuProvider);
    m_contextMenuAllowed = false;
}

void WebViewImpl::showContextMenuForElement(WebElement element)
{
    if (!page())
        return;

    page()->contextMenuController().clearContextMenu();
    m_contextMenuAllowed = true;
    if (LocalFrame* focusedFrame = toLocalFrame(page()->focusController().focusedOrMainFrame()))
        focusedFrame->eventHandler().sendContextMenuEventForKey(element.unwrap<Element>());
    m_contextMenuAllowed = false;
}

bool WebViewImpl::keyEventDefault(const WebKeyboardEvent& event)
{
    LocalFrame* frame = toLocalFrame(focusedCoreFrame());
    if (!frame)
        return false;

    switch (event.type) {
    case WebInputEvent::Char:
        if (event.windowsKeyCode == VKEY_SPACE) {
            int keyCode = ((event.modifiers & WebInputEvent::ShiftKey) ? VKEY_PRIOR : VKEY_NEXT);
            return scrollViewWithKeyboard(keyCode, event.modifiers);
        }
        break;
    case WebInputEvent::RawKeyDown:
        if (event.modifiers == WebInputEvent::ControlKey) {
            switch (event.windowsKeyCode) {
#if !OS(MACOSX)
            case 'A':
                focusedFrame()->executeCommand(WebString::fromUTF8("SelectAll"));
                return true;
            case VKEY_INSERT:
            case 'C':
                focusedFrame()->executeCommand(WebString::fromUTF8("Copy"));
                return true;
#endif
            // Match FF behavior in the sense that Ctrl+home/end are the only Ctrl
            // key combinations which affect scrolling. Safari is buggy in the
            // sense that it scrolls the page for all Ctrl+scrolling key
            // combinations. For e.g. Ctrl+pgup/pgdn/up/down, etc.
            case VKEY_HOME:
            case VKEY_END:
                break;
            default:
                return false;
            }
        }
        if (!event.isSystemKey && !(event.modifiers & WebInputEvent::ShiftKey))
            return scrollViewWithKeyboard(event.windowsKeyCode, event.modifiers);
        break;
    default:
        break;
    }
    return false;
}

bool WebViewImpl::scrollViewWithKeyboard(int keyCode, int modifiers)
{
    ScrollDirectionPhysical scrollDirectionPhysical;
    ScrollGranularity scrollGranularity;
#if OS(MACOSX)
    // Control-Up/Down should be PageUp/Down on Mac.
    if (modifiers & WebMouseEvent::ControlKey) {
      if (keyCode == VKEY_UP)
        keyCode = VKEY_PRIOR;
      else if (keyCode == VKEY_DOWN)
        keyCode = VKEY_NEXT;
    }
#endif
    if (!mapKeyCodeForScroll(keyCode, &scrollDirectionPhysical, &scrollGranularity))
        return false;

    if (LocalFrame* frame = toLocalFrame(focusedCoreFrame()))
        return frame->eventHandler().bubblingScroll(toScrollDirection(scrollDirectionPhysical), scrollGranularity);
    return false;
}

bool WebViewImpl::mapKeyCodeForScroll(
    int keyCode,
    ScrollDirectionPhysical* scrollDirection,
    ScrollGranularity* scrollGranularity)
{
    switch (keyCode) {
    case VKEY_LEFT:
        *scrollDirection = ScrollLeft;
        *scrollGranularity = ScrollByLine;
        break;
    case VKEY_RIGHT:
        *scrollDirection = ScrollRight;
        *scrollGranularity = ScrollByLine;
        break;
    case VKEY_UP:
        *scrollDirection = ScrollUp;
        *scrollGranularity = ScrollByLine;
        break;
    case VKEY_DOWN:
        *scrollDirection = ScrollDown;
        *scrollGranularity = ScrollByLine;
        break;
    case VKEY_HOME:
        *scrollDirection = ScrollUp;
        *scrollGranularity = ScrollByDocument;
        break;
    case VKEY_END:
        *scrollDirection = ScrollDown;
        *scrollGranularity = ScrollByDocument;
        break;
    case VKEY_PRIOR:  // page up
        *scrollDirection = ScrollUp;
        *scrollGranularity = ScrollByPage;
        break;
    case VKEY_NEXT:  // page down
        *scrollDirection = ScrollDown;
        *scrollGranularity = ScrollByPage;
        break;
    default:
        return false;
    }

    return true;
}

PagePopup* WebViewImpl::openPagePopup(PagePopupClient* client)
{
    ASSERT(client);
    if (hasOpenedPopup())
        hidePopups();
    ASSERT(!m_pagePopup);

    WebWidget* popupWidget = m_client->createPopupMenu(WebPopupTypePage);
    ASSERT(popupWidget);
    m_pagePopup = toWebPagePopupImpl(popupWidget);
    if (!m_pagePopup->initialize(this, client)) {
        m_pagePopup->closePopup();
        m_pagePopup = nullptr;
    }
    enablePopupMouseWheelEventListener();
    return m_pagePopup.get();
}

void WebViewImpl::closePagePopup(PagePopup* popup)
{
    ASSERT(popup);
    WebPagePopupImpl* popupImpl = toWebPagePopupImpl(popup);
    ASSERT(m_pagePopup.get() == popupImpl);
    if (m_pagePopup.get() != popupImpl)
        return;
    m_pagePopup->closePopup();
    m_pagePopup = nullptr;
    disablePopupMouseWheelEventListener();
}

void WebViewImpl::cancelPagePopup()
{
    if (m_pagePopup)
        m_pagePopup->cancel();
}

void WebViewImpl::enablePopupMouseWheelEventListener()
{
    ASSERT(!m_popupMouseWheelEventListener);
    ASSERT(mainFrameImpl()->frame()->document());
    // We register an empty event listener, EmptyEventListener, so that mouse
    // wheel events get sent to the WebView.
    m_popupMouseWheelEventListener = EmptyEventListener::create();
    mainFrameImpl()->frame()->document()->addEventListener(EventTypeNames::mousewheel, m_popupMouseWheelEventListener, false);
}

void WebViewImpl::disablePopupMouseWheelEventListener()
{
    ASSERT(m_popupMouseWheelEventListener);
    ASSERT(mainFrameImpl()->frame()->document());
    // Document may have already removed the event listener, for instance, due
    // to a navigation, but remove it anyway.
    mainFrameImpl()->frame()->document()->removeEventListener(EventTypeNames::mousewheel, m_popupMouseWheelEventListener.release(), false);
}

LocalDOMWindow* WebViewImpl::pagePopupWindow() const
{
    return m_pagePopup ? m_pagePopup->window() : nullptr;
}

Frame* WebViewImpl::focusedCoreFrame() const
{
    return m_page ? m_page->focusController().focusedOrMainFrame() : 0;
}

WebViewImpl* WebViewImpl::fromPage(Page* page)
{
    if (!page)
        return 0;
    return static_cast<WebViewImpl*>(page->chromeClient().webView());
}

// WebWidget ------------------------------------------------------------------

void WebViewImpl::close()
{
    WebDevToolsAgentImpl::webViewImplClosed(this);
    ASSERT(allInstances().contains(this));
    allInstances().remove(this);

    if (m_page) {
        // Initiate shutdown for the entire frameset.  This will cause a lot of
        // notifications to be sent.
        m_page->willBeDestroyed();
        m_page.clear();
    }

    // Reset the delegate to prevent notifications being sent as we're being
    // deleted.
    m_client = nullptr;

    deref();  // Balances ref() acquired in WebView::create
}

void WebViewImpl::willStartLiveResize()
{
    if (mainFrameImpl() && mainFrameImpl()->frameView())
        mainFrameImpl()->frameView()->willStartLiveResize();
}

WebSize WebViewImpl::size()
{
    return m_size;
}

void WebViewImpl::resizePinchViewport(const WebSize& newSize)
{
    page()->frameHost().pinchViewport().setSize(newSize);
    page()->frameHost().pinchViewport().clampToBoundaries();
}

void WebViewImpl::performResize()
{
    pageScaleConstraintsSet().didChangeViewSize(m_size);

    updatePageDefinedViewportConstraints(mainFrameImpl()->frame()->document()->viewportDescription());
    updateMainFrameLayoutSize();

    page()->frameHost().pinchViewport().setSize(m_size);

    if (mainFrameImpl()->frameView()) {
        if (!mainFrameImpl()->frameView()->needsLayout())
            postLayoutResize(mainFrameImpl());
    }

    // When device emulation is enabled, device size values may change - they are
    // usually set equal to the view size. These values are not considered viewport-dependent
    // (see MediaQueryExp::isViewportDependent), since they are only viewport-dependent in emulation mode,
    // and thus will not be invalidated in |FrameView::performPreLayoutTasks|.
    // Therefore we should force explicit media queries invalidation here.
    if (m_devToolsEmulator->deviceEmulationEnabled()) {
        if (Document* document = mainFrameImpl()->frame()->document()) {
            document->styleResolverChanged();
            document->mediaQueryAffectingValueChanged();
        }
    }
}

void WebViewImpl::setTopControlsHeight(float height, bool topControlsShrinkLayoutSize)
{
    topControls().setHeight(height, topControlsShrinkLayoutSize);
}

void WebViewImpl::updateTopControlsState(WebTopControlsState constraint, WebTopControlsState current, bool animate)
{
    topControls().updateConstraints(constraint);
    if (m_layerTreeView)
        m_layerTreeView->updateTopControlsState(constraint, current, animate);
}

void WebViewImpl::didUpdateTopControls()
{
    if (m_layerTreeView) {
        m_layerTreeView->setTopControlsShownRatio(topControls().shownRatio());
        m_layerTreeView->setTopControlsHeight(topControls().height(), topControls().shrinkViewport());
    }

    WebLocalFrameImpl* mainFrame = mainFrameImpl();
    if (!mainFrame)
        return;

    FrameView* view = mainFrame->frameView();
    if (!view)
        return;

    float topControlsViewportAdjustment = topControls().layoutHeight() - topControls().contentOffset();

    PinchViewport& pinchViewport = page()->frameHost().pinchViewport();

    if (pinchViewport.visibleRect().isEmpty())
        return;

    pinchViewport.setTopControlsAdjustment(topControlsViewportAdjustment);

    // On ChromeOS the pinch viewport can change size independent of the layout viewport due to the
    // on screen keyboard so we should only set the FrameView adjustment on Android.
    if (settings() && settings()->mainFrameResizesAreOrientationChanges()) {
        // Shrink the FrameView by the amount that will maintain the aspect-ratio with the PinchViewport.
        float aspectRatio = pinchViewport.visibleRect().width() / pinchViewport.visibleRect().height();
        float newHeight = view->visibleContentSize(ExcludeScrollbars).width() / aspectRatio;
        float adjustment = newHeight - view->visibleContentSize(ExcludeScrollbars).height();
        view->setTopControlsViewportAdjustment(adjustment);
    }
}

TopControls& WebViewImpl::topControls()
{
    return page()->frameHost().topControls();
}

void WebViewImpl::resizeViewWhileAnchored(FrameView* view)
{
    // FIXME: TextAutosizer does not yet support out-of-process frames.
    if (mainFrameImpl() && mainFrameImpl()->frame()->isLocalFrame()) {
        // Avoids unnecessary invalidations while various bits of state in TextAutosizer are updated.
        TextAutosizer::DeferUpdatePageInfo deferUpdatePageInfo(page());
        performResize();
    } else {
        performResize();
    }

    m_fullscreenController->updateSize();

    // Relayout immediately to recalculate the minimum scale limit for rotation anchoring.
    if (view->needsLayout())
        view->layout();
}

void WebViewImpl::resize(const WebSize& newSize)
{
    if (m_shouldAutoResize || m_size == newSize)
        return;

    WebLocalFrameImpl* mainFrame = mainFrameImpl();
    if (!mainFrame)
        return;

    FrameView* view = mainFrame->frameView();
    if (!view)
        return;

    PinchViewport& pinchViewport = page()->frameHost().pinchViewport();

    bool isRotation = settings()->mainFrameResizesAreOrientationChanges()
        && m_size.width && contentsSize().width() && newSize.width != m_size.width && !m_fullscreenController->isFullscreen();
    m_size = newSize;

    FloatSize viewportAnchorCoords(viewportAnchorCoordX, viewportAnchorCoordY);
    if (isRotation) {
        RotationViewportAnchor anchor(*view, pinchViewport, viewportAnchorCoords, pageScaleConstraintsSet());
        resizeViewWhileAnchored(view);
    } else {
        ResizeViewportAnchor anchor(*view, pinchViewport);
        resizeViewWhileAnchored(view);
    }
    sendResizeEventAndRepaint();
}

void WebViewImpl::willEndLiveResize()
{
    if (mainFrameImpl() && mainFrameImpl()->frameView())
        mainFrameImpl()->frameView()->willEndLiveResize();
}

void WebViewImpl::didEnterFullScreen()
{
    m_fullscreenController->didEnterFullScreen();
}

void WebViewImpl::didExitFullScreen()
{
    m_fullscreenController->didExitFullScreen();
}

void WebViewImpl::beginFrame(const WebBeginFrameArgs& frameTime)
{
    TRACE_EVENT0("blink", "WebViewImpl::beginFrame");

    WebBeginFrameArgs validFrameTime(frameTime);
    if (!validFrameTime.lastFrameTimeMonotonic)
        validFrameTime.lastFrameTimeMonotonic = monotonicallyIncreasingTime();

    // Create synthetic wheel events as necessary for fling.
    if (m_gestureAnimation) {
        if (m_gestureAnimation->animate(validFrameTime.lastFrameTimeMonotonic))
            scheduleAnimation();
        else {
            endActiveFlingAnimation();

            PlatformGestureEvent endScrollEvent(PlatformEvent::GestureScrollEnd,
                m_positionOnFlingStart, m_globalPositionOnFlingStart,
                IntSize(), 0, false, false, false, false);
            endScrollEvent.setScrollGestureData(0, 0, 0, 0, true, false);

            mainFrameImpl()->frame()->eventHandler().handleGestureScrollEnd(endScrollEvent);
        }
    }

    if (!m_page)
        return;

    // FIXME: This should probably be using the local root?
    if (m_page->mainFrame()->isLocalFrame())
        PageWidgetDelegate::animate(*m_page, validFrameTime.lastFrameTimeMonotonic, *m_page->deprecatedLocalMainFrame());

    if (m_continuousPaintingEnabled) {
        ContinuousPainter::setNeedsDisplayRecursive(m_rootGraphicsLayer, m_pageOverlays.get());
        m_client->scheduleAnimation();
    }
}

void WebViewImpl::setNeedsLayoutAndFullPaintInvalidation()
{
    if (!mainFrameImpl() || !mainFrameImpl()->frame() || !mainFrameImpl()->frame()->view())
        return;

    FrameView* view = mainFrameImpl()->frame()->view();
    view->setNeedsLayout();
    view->setNeedsFullPaintInvalidation();
}

void WebViewImpl::layout()
{
    TRACE_EVENT0("blink", "WebViewImpl::layout");
    if (!mainFrameImpl())
        return;

    PageWidgetDelegate::layout(*m_page, *mainFrameImpl()->frame());
    updateLayerTreeBackgroundColor();

    for (size_t i = 0; i < m_linkHighlights.size(); ++i)
        m_linkHighlights[i]->updateGeometry();
}

void WebViewImpl::paint(WebCanvas* canvas, const WebRect& rect)
{
    // This should only be used when compositing is not being used for this
    // WebView, and it is painting into the recording of its parent.
    ASSERT(!isAcceleratedCompositingActive());

    double paintStart = currentTime();
    PageWidgetDelegate::paint(*m_page, pageOverlays(), canvas, rect, *m_page->deprecatedLocalMainFrame());
    double paintEnd = currentTime();
    double pixelsPerSec = (rect.width * rect.height) / (paintEnd - paintStart);
    Platform::current()->histogramCustomCounts("Renderer4.SoftwarePaintDurationMS", (paintEnd - paintStart) * 1000, 0, 120, 30);
    Platform::current()->histogramCustomCounts("Renderer4.SoftwarePaintMegapixPerSecond", pixelsPerSec / 1000000, 10, 210, 30);
}

#if OS(ANDROID)
void WebViewImpl::paintCompositedDeprecated(WebCanvas* canvas, const WebRect& rect)
{
    // Note: This method exists on OS(ANDROID) and will hopefully be
    //       removed once the link disambiguation feature renders using
    //       the compositor.
    ASSERT(isAcceleratedCompositingActive());

    FrameView* view = page()->deprecatedLocalMainFrame()->view();
    PaintBehavior oldPaintBehavior = view->paintBehavior();
    view->setPaintBehavior(oldPaintBehavior | PaintBehaviorFlattenCompositingLayers);

    PageWidgetDelegate::paint(*m_page, pageOverlays(), canvas, rect, *m_page->deprecatedLocalMainFrame());

    view->setPaintBehavior(oldPaintBehavior);
}
#endif

void WebViewImpl::layoutAndPaintAsync(WebLayoutAndPaintAsyncCallback* callback)
{
    m_layerTreeView->layoutAndPaintAsync(callback);
}

void WebViewImpl::compositeAndReadbackAsync(WebCompositeAndReadbackAsyncCallback* callback)
{
    m_layerTreeView->compositeAndReadbackAsync(callback);
}

void WebViewImpl::themeChanged()
{
    if (!page())
        return;
    if (!page()->mainFrame()->isLocalFrame())
        return;
    FrameView* view = page()->deprecatedLocalMainFrame()->view();

    WebRect damagedRect(0, 0, m_size.width, m_size.height);
    view->invalidateRect(damagedRect);
}

void WebViewImpl::enterFullScreenForElement(Element* element)
{
    m_fullscreenController->enterFullScreenForElement(element);
}

void WebViewImpl::exitFullScreenForElement(Element* element)
{
    m_fullscreenController->exitFullScreenForElement(element);
}

void WebViewImpl::clearCompositedSelection()
{
    if (m_layerTreeView)
        m_layerTreeView->clearSelection();
}

void WebViewImpl::updateCompositedSelection(const WebSelection& selection)
{
    if (m_layerTreeView) {
        m_layerTreeView->registerSelection(selection);
        // TODO(jdduke): Remove this overload when downstream consumers have been updated, crbug.com/466672.
        m_layerTreeView->registerSelection(selection.start(), selection.end());
    }
}

bool WebViewImpl::hasHorizontalScrollbar()
{
    return mainFrameImpl()->frameView()->horizontalScrollbar();
}

bool WebViewImpl::hasVerticalScrollbar()
{
    return mainFrameImpl()->frameView()->verticalScrollbar();
}

const WebInputEvent* WebViewImpl::m_currentInputEvent = nullptr;

// FIXME: autogenerate this kind of code, and use it throughout Blink rather than
// the one-offs for subsets of these values.
static String inputTypeToName(WebInputEvent::Type type)
{
    switch (type) {
    case WebInputEvent::MouseDown:
        return EventTypeNames::mousedown;
    case WebInputEvent::MouseUp:
        return EventTypeNames::mouseup;
    case WebInputEvent::MouseMove:
        return EventTypeNames::mousemove;
    case WebInputEvent::MouseEnter:
        return EventTypeNames::mouseenter;
    case WebInputEvent::MouseLeave:
        return EventTypeNames::mouseleave;
    case WebInputEvent::ContextMenu:
        return EventTypeNames::contextmenu;
    case WebInputEvent::MouseWheel:
        return EventTypeNames::mousewheel;
    case WebInputEvent::KeyDown:
        return EventTypeNames::keydown;
    case WebInputEvent::KeyUp:
        return EventTypeNames::keyup;
    case WebInputEvent::GestureScrollBegin:
        return EventTypeNames::gesturescrollstart;
    case WebInputEvent::GestureScrollEnd:
        return EventTypeNames::gesturescrollend;
    case WebInputEvent::GestureScrollUpdate:
        return EventTypeNames::gesturescrollupdate;
    case WebInputEvent::GestureTapDown:
        return EventTypeNames::gesturetapdown;
    case WebInputEvent::GestureShowPress:
        return EventTypeNames::gestureshowpress;
    case WebInputEvent::GestureTap:
        return EventTypeNames::gesturetap;
    case WebInputEvent::GestureTapUnconfirmed:
        return EventTypeNames::gesturetapunconfirmed;
    case WebInputEvent::TouchStart:
        return EventTypeNames::touchstart;
    case WebInputEvent::TouchMove:
        return EventTypeNames::touchmove;
    case WebInputEvent::TouchEnd:
        return EventTypeNames::touchend;
    case WebInputEvent::TouchCancel:
        return EventTypeNames::touchcancel;
    default:
        return String("unknown");
    }
}

bool WebViewImpl::handleInputEvent(const WebInputEvent& inputEvent)
{
    // TODO(dcheng): The fact that this is getting called when there is no local
    // main frame is problematic and probably indicates a bug in the input event
    // routing code.
    if (!mainFrameImpl())
        return false;

    WebAutofillClient* autofillClient = mainFrameImpl()->autofillClient();
    UserGestureNotifier notifier(autofillClient, &m_userGestureObserved);
    // On the first input event since page load, |notifier| instructs the
    // autofill client to unblock values of password input fields of any forms
    // on the page. There is a single input event, GestureTap, which can both
    // be the first event after page load, and cause a form submission. In that
    // case, the form submission happens before the autofill client is told
    // to unblock the password values, and so the password values are not
    // submitted. To avoid that, GestureTap is handled explicitly:
    if (inputEvent.type == WebInputEvent::GestureTap && autofillClient) {
        m_userGestureObserved = true;
        autofillClient->firstUserGestureObserved();
    }

    TRACE_EVENT1("input", "WebViewImpl::handleInputEvent", "type", inputTypeToName(inputEvent.type).ascii());
    // If we've started a drag and drop operation, ignore input events until
    // we're done.
    if (m_doingDragAndDrop)
        return true;

    if (m_devToolsEmulator->handleInputEvent(inputEvent))
        return true;

    WebDevToolsAgentImpl* devTools = mainFrameDevToolsAgentImpl();
    if (devTools && devTools->handleInputEvent(inputEvent))
        return true;

    if (m_inspectorOverlay && m_inspectorOverlay->handleInputEvent(inputEvent))
        return true;

    // Report the event to be NOT processed by WebKit, so that the browser can handle it appropriately.
    if (m_ignoreInputEvents)
        return false;

    TemporaryChange<const WebInputEvent*> currentEventChange(m_currentInputEvent, &inputEvent);
    UIEventWithKeyState::clearNewTabModifierSetFromIsolatedWorld();

    if (isPointerLocked() && WebInputEvent::isMouseEventType(inputEvent.type)) {
        pointerLockMouseEvent(inputEvent);
        return true;
    }

    if (m_mouseCaptureNode && WebInputEvent::isMouseEventType(inputEvent.type)) {
        TRACE_EVENT1("input", "captured mouse event", "type", inputEvent.type);
        // Save m_mouseCaptureNode since mouseCaptureLost() will clear it.
        RefPtrWillBeRawPtr<Node> node = m_mouseCaptureNode;

        // Not all platforms call mouseCaptureLost() directly.
        if (inputEvent.type == WebInputEvent::MouseUp)
            mouseCaptureLost();

        OwnPtr<UserGestureIndicator> gestureIndicator;

        AtomicString eventType;
        switch (inputEvent.type) {
        case WebInputEvent::MouseMove:
            eventType = EventTypeNames::mousemove;
            break;
        case WebInputEvent::MouseLeave:
            eventType = EventTypeNames::mouseout;
            break;
        case WebInputEvent::MouseDown:
            eventType = EventTypeNames::mousedown;
            gestureIndicator = adoptPtr(new UserGestureIndicator(DefinitelyProcessingNewUserGesture));
            m_mouseCaptureGestureToken = gestureIndicator->currentToken();
            break;
        case WebInputEvent::MouseUp:
            eventType = EventTypeNames::mouseup;
            gestureIndicator = adoptPtr(new UserGestureIndicator(m_mouseCaptureGestureToken.release()));
            break;
        default:
            ASSERT_NOT_REACHED();
        }

        node->dispatchMouseEvent(
            PlatformMouseEventBuilder(mainFrameImpl()->frameView(), static_cast<const WebMouseEvent&>(inputEvent)),
            eventType, static_cast<const WebMouseEvent&>(inputEvent).clickCount);
        return true;
    }

    // FIXME: This should take in the intended frame, not the local frame root.
    if (PageWidgetDelegate::handleInputEvent(*this, inputEvent, mainFrameImpl()->frame()))
        return true;

    // Unhandled touchpad gesture pinch events synthesize mouse wheel events.
    if (inputEvent.type == WebInputEvent::GesturePinchUpdate) {
        const WebGestureEvent& pinchEvent = static_cast<const WebGestureEvent&>(inputEvent);

        // First, synthesize a Windows-like wheel event to send to any handlers that may exist.
        if (handleSyntheticWheelFromTouchpadPinchEvent(pinchEvent))
            return true;

        if (pinchEvent.data.pinchUpdate.zoomDisabled)
            return false;

        if (page()->frameHost().pinchViewport().magnifyScaleAroundAnchor(pinchEvent.data.pinchUpdate.scale, FloatPoint(pinchEvent.x, pinchEvent.y)))
            return true;
    }

    return false;
}

void WebViewImpl::setCursorVisibilityState(bool isVisible)
{
    if (m_page)
        m_page->setIsCursorVisible(isVisible);
}

void WebViewImpl::mouseCaptureLost()
{
    TRACE_EVENT_ASYNC_END0("input", "capturing mouse", this);
    m_mouseCaptureNode = nullptr;
}

void WebViewImpl::setFocus(bool enable)
{
    m_page->focusController().setFocused(enable);
    if (enable) {
        m_page->focusController().setActive(true);
        RefPtrWillBeRawPtr<Frame> focusedFrame = m_page->focusController().focusedFrame();
        if (focusedFrame && focusedFrame->isLocalFrame()) {
            LocalFrame* localFrame = toLocalFrame(focusedFrame.get());
            Element* element = localFrame->document()->focusedElement();
            if (element && localFrame->selection().selection().isNone()) {
                // If the selection was cleared while the WebView was not
                // focused, then the focus element shows with a focus ring but
                // no caret and does respond to keyboard inputs.
                if (element->isTextFormControl()) {
                    element->updateFocusAppearance(true);
                } else if (element->isContentEditable()) {
                    // updateFocusAppearance() selects all the text of
                    // contentseditable DIVs. So we set the selection explicitly
                    // instead. Note that this has the side effect of moving the
                    // caret back to the beginning of the text.
                    Position position(element, 0);
                    localFrame->selection().setSelection(VisibleSelection(position, SEL_DEFAULT_AFFINITY));
                }
            }
        }
        m_imeAcceptEvents = true;
    } else {
        hidePopups();

        // Clear focus on the currently focused frame if any.
        if (!m_page)
            return;

        LocalFrame* frame = m_page->mainFrame() && m_page->mainFrame()->isLocalFrame()
            ? m_page->deprecatedLocalMainFrame() : 0;
        if (!frame)
            return;

        RefPtrWillBeRawPtr<Frame> focusedFrame = m_page->focusController().focusedFrame();
        if (focusedFrame && focusedFrame->isLocalFrame()) {
            // Finish an ongoing composition to delete the composition node.
            if (toLocalFrame(focusedFrame.get())->inputMethodController().hasComposition()) {
                WebAutofillClient* autofillClient = WebLocalFrameImpl::fromFrame(toLocalFrame(focusedFrame.get()))->autofillClient();

                if (autofillClient)
                    autofillClient->setIgnoreTextChanges(true);

                toLocalFrame(focusedFrame.get())->inputMethodController().confirmComposition();

                if (autofillClient)
                    autofillClient->setIgnoreTextChanges(false);
            }
            m_imeAcceptEvents = false;
        }
    }
}

bool WebViewImpl::setComposition(
    const WebString& text,
    const WebVector<WebCompositionUnderline>& underlines,
    int selectionStart,
    int selectionEnd)
{
    LocalFrame* focused = toLocalFrame(focusedCoreFrame());
    if (!focused || !m_imeAcceptEvents)
        return false;

    if (WebPlugin* plugin = focusedPluginIfInputMethodSupported(focused))
        return plugin->setComposition(text, underlines, selectionStart, selectionEnd);

    // The input focus has been moved to another WebWidget object.
    // We should use this |editor| object only to complete the ongoing
    // composition.
    InputMethodController& inputMethodController = focused->inputMethodController();
    if (!focused->editor().canEdit() && !inputMethodController.hasComposition())
        return false;

    // We should verify the parent node of this IME composition node are
    // editable because JavaScript may delete a parent node of the composition
    // node. In this case, WebKit crashes while deleting texts from the parent
    // node, which doesn't exist any longer.
    RefPtrWillBeRawPtr<Range> range = inputMethodController.compositionRange();
    if (range) {
        Node* node = range->startContainer();
        if (!node || !node->isContentEditable())
            return false;
    }

    // A keypress event is canceled. If an ongoing composition exists, then the
    // keydown event should have arisen from a handled key (e.g., backspace).
    // In this case we ignore the cancellation and continue; otherwise (no
    // ongoing composition) we exit and signal success only for attempts to
    // clear the composition.
    if (m_suppressNextKeypressEvent && !inputMethodController.hasComposition())
        return text.isEmpty();

    UserGestureIndicator gestureIndicator(DefinitelyProcessingNewUserGesture);

    // When the range of composition underlines overlap with the range between
    // selectionStart and selectionEnd, WebKit somehow won't paint the selection
    // at all (see InlineTextBox::paint() function in InlineTextBox.cpp).
    // But the selection range actually takes effect.
    inputMethodController.setComposition(String(text),
                           CompositionUnderlineVectorBuilder(underlines),
                           selectionStart, selectionEnd);

    return text.isEmpty() || inputMethodController.hasComposition();
}

bool WebViewImpl::confirmComposition()
{
    return confirmComposition(DoNotKeepSelection);
}

bool WebViewImpl::confirmComposition(ConfirmCompositionBehavior selectionBehavior)
{
    return confirmComposition(WebString(), selectionBehavior);
}

bool WebViewImpl::confirmComposition(const WebString& text)
{
    return confirmComposition(text, DoNotKeepSelection);
}

bool WebViewImpl::confirmComposition(const WebString& text, ConfirmCompositionBehavior selectionBehavior)
{
    LocalFrame* focused = toLocalFrame(focusedCoreFrame());
    if (!focused || !m_imeAcceptEvents)
        return false;

    if (WebPlugin* plugin = focusedPluginIfInputMethodSupported(focused))
        return plugin->confirmComposition(text, selectionBehavior);

    return focused->inputMethodController().confirmCompositionOrInsertText(text, selectionBehavior == KeepSelection ? InputMethodController::KeepSelection : InputMethodController::DoNotKeepSelection);
}

bool WebViewImpl::compositionRange(size_t* location, size_t* length)
{
    // FIXME: Long term, the focused frame should be a local frame. For now,
    // return early to avoid crashes.
    Frame* frame = focusedCoreFrame();
    if (!frame || frame->isRemoteFrame())
        return false;

    LocalFrame* focused = toLocalFrame(frame);
    if (!focused || !m_imeAcceptEvents)
        return false;

    RefPtrWillBeRawPtr<Range> range = focused->inputMethodController().compositionRange();
    if (!range)
        return false;

    Element* editable = focused->selection().rootEditableElementOrDocumentElement();
    ASSERT(editable);
    PlainTextRange plainTextRange(PlainTextRange::create(*editable, *range.get()));
    if (plainTextRange.isNull())
        return false;
    *location = plainTextRange.start();
    *length = plainTextRange.length();
    return true;
}

WebTextInputInfo WebViewImpl::textInputInfo()
{
    WebTextInputInfo info;

    Frame* focusedFrame = focusedCoreFrame();
    if (!focusedFrame->isLocalFrame())
        return info;

    LocalFrame* focused = toLocalFrame(focusedFrame);
    if (!focused)
        return info;

    FrameSelection& selection = focused->selection();
    Element* element = selection.selection().rootEditableElement();
    if (!element)
        return info;

    info.inputMode = inputModeOfFocusedElement();

    info.type = textInputType();
    info.flags = textInputFlags();
    if (info.type == WebTextInputTypeNone)
        return info;

    if (!focused->editor().canEdit())
        return info;

    // Emits an object replacement character for each replaced element so that
    // it is exposed to IME and thus could be deleted by IME on android.
    info.value = plainText(Position(element, PositionAnchorType::BeforeChildren), Position(element, PositionAnchorType::AfterChildren), TextIteratorEmitsObjectReplacementCharacter);

    if (info.value.isEmpty())
        return info;

    if (RefPtrWillBeRawPtr<Range> range = selection.selection().firstRange()) {
        PlainTextRange plainTextRange(PlainTextRange::create(*element, *range.get()));
        if (plainTextRange.isNotNull()) {
            info.selectionStart = plainTextRange.start();
            info.selectionEnd = plainTextRange.end();
        }
    }

    if (RefPtrWillBeRawPtr<Range> range = focused->inputMethodController().compositionRange()) {
        PlainTextRange plainTextRange(PlainTextRange::create(*element, *range.get()));
        if (plainTextRange.isNotNull()) {
            info.compositionStart = plainTextRange.start();
            info.compositionEnd = plainTextRange.end();
        }
    }

    return info;
}

WebTextInputType WebViewImpl::textInputType()
{
    Element* element = focusedElement();
    if (!element)
        return WebTextInputTypeNone;

    if (isHTMLInputElement(*element)) {
        HTMLInputElement& input = toHTMLInputElement(*element);
        const AtomicString& type = input.type();

        if (input.isDisabledOrReadOnly())
            return WebTextInputTypeNone;

        if (type == InputTypeNames::password)
            return WebTextInputTypePassword;
        if (type == InputTypeNames::search)
            return WebTextInputTypeSearch;
        if (type == InputTypeNames::email)
            return WebTextInputTypeEmail;
        if (type == InputTypeNames::number)
            return WebTextInputTypeNumber;
        if (type == InputTypeNames::tel)
            return WebTextInputTypeTelephone;
        if (type == InputTypeNames::url)
            return WebTextInputTypeURL;
        if (type == InputTypeNames::date)
            return WebTextInputTypeDate;
        if (type == InputTypeNames::datetime_local)
            return WebTextInputTypeDateTimeLocal;
        if (type == InputTypeNames::month)
            return WebTextInputTypeMonth;
        if (type == InputTypeNames::time)
            return WebTextInputTypeTime;
        if (type == InputTypeNames::week)
            return WebTextInputTypeWeek;
        if (type == InputTypeNames::text)
            return WebTextInputTypeText;

        return WebTextInputTypeNone;
    }

    if (isHTMLTextAreaElement(*element)) {
        if (toHTMLTextAreaElement(*element).isDisabledOrReadOnly())
            return WebTextInputTypeNone;
        return WebTextInputTypeTextArea;
    }

#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
    if (element->isHTMLElement()) {
        if (toHTMLElement(element)->isDateTimeFieldElement())
            return WebTextInputTypeDateTimeField;
    }
#endif

    if (element->isContentEditable(Node::UserSelectAllIsAlwaysNonEditable))
        return WebTextInputTypeContentEditable;

    return WebTextInputTypeNone;
}

int WebViewImpl::textInputFlags()
{
    Element* element = focusedElement();
    if (!element)
        return WebTextInputFlagNone;

    DEFINE_STATIC_LOCAL(AtomicString, autocompleteString, ("autocomplete", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(AtomicString, autocorrectString, ("autocorrect", AtomicString::ConstructFromLiteral));
    int flags = 0;

    const AtomicString& autocomplete = element->getAttribute(autocompleteString);
    if (autocomplete == "on")
        flags |= WebTextInputFlagAutocompleteOn;
    else if (autocomplete == "off")
        flags |= WebTextInputFlagAutocompleteOff;

    const AtomicString& autocorrect = element->getAttribute(autocorrectString);
    if (autocorrect == "on")
        flags |= WebTextInputFlagAutocorrectOn;
    else if (autocorrect == "off")
        flags |= WebTextInputFlagAutocorrectOff;

    SpellcheckAttributeState spellcheck = element->spellcheckAttributeState();
    if (spellcheck == SpellcheckAttributeTrue)
        flags |= WebTextInputFlagSpellcheckOn;
    else if (spellcheck == SpellcheckAttributeFalse)
        flags |= WebTextInputFlagSpellcheckOff;

    if (isHTMLTextFormControlElement(element)) {
        HTMLTextFormControlElement* formElement = static_cast<HTMLTextFormControlElement*>(element);
        if (formElement->supportsAutocapitalize()) {
            DEFINE_STATIC_LOCAL(const AtomicString, none, ("none", AtomicString::ConstructFromLiteral));
            DEFINE_STATIC_LOCAL(const AtomicString, characters, ("characters", AtomicString::ConstructFromLiteral));
            DEFINE_STATIC_LOCAL(const AtomicString, words, ("words", AtomicString::ConstructFromLiteral));
            DEFINE_STATIC_LOCAL(const AtomicString, sentences, ("sentences", AtomicString::ConstructFromLiteral));

            const AtomicString& autocapitalize = formElement->autocapitalize();
            if (autocapitalize == none)
                flags |= WebTextInputFlagAutocapitalizeNone;
            else if (autocapitalize == characters)
                flags |= WebTextInputFlagAutocapitalizeCharacters;
            else if (autocapitalize == words)
                flags |= WebTextInputFlagAutocapitalizeWords;
            else if (autocapitalize == sentences)
                flags |= WebTextInputFlagAutocapitalizeSentences;
            else
                ASSERT_NOT_REACHED();
        }
    }

    return flags;
}

WebString WebViewImpl::inputModeOfFocusedElement()
{
    if (!RuntimeEnabledFeatures::inputModeAttributeEnabled())
        return WebString();

    Element* element = focusedElement();
    if (!element)
        return WebString();

    if (isHTMLInputElement(*element)) {
        const HTMLInputElement& input = toHTMLInputElement(*element);
        if (input.supportsInputModeAttribute())
            return input.fastGetAttribute(HTMLNames::inputmodeAttr).lower();
        return WebString();
    }
    if (isHTMLTextAreaElement(*element)) {
        const HTMLTextAreaElement& textarea = toHTMLTextAreaElement(*element);
        return textarea.fastGetAttribute(HTMLNames::inputmodeAttr).lower();
    }

    return WebString();
}

bool WebViewImpl::selectionBounds(WebRect& anchor, WebRect& focus) const
{
    const Frame* frame = focusedCoreFrame();
    if (!frame || !frame->isLocalFrame())
        return false;

    const LocalFrame* localFrame = toLocalFrame(frame);
    if (!localFrame)
        return false;
    FrameSelection& selection = localFrame->selection();

    if (selection.isCaret()) {
        anchor = focus = selection.absoluteCaretBounds();
    } else {
        RefPtrWillBeRawPtr<Range> selectedRange = selection.toNormalizedRange();
        if (!selectedRange)
            return false;

        RefPtrWillBeRawPtr<Range> range(Range::create(selectedRange->startContainer()->document(),
            selectedRange->startContainer(),
            selectedRange->startOffset(),
            selectedRange->startContainer(),
            selectedRange->startOffset()));
        anchor = localFrame->editor().firstRectForRange(range.get());

        range = Range::create(selectedRange->endContainer()->document(),
            selectedRange->endContainer(),
            selectedRange->endOffset(),
            selectedRange->endContainer(),
            selectedRange->endOffset());
        focus = localFrame->editor().firstRectForRange(range.get());
    }

    anchor = localFrame->view()->contentsToViewport(anchor);
    focus = localFrame->view()->contentsToViewport(focus);

    if (!selection.selection().isBaseFirst())
        std::swap(anchor, focus);
    return true;
}

WebPlugin* WebViewImpl::focusedPluginIfInputMethodSupported(LocalFrame* frame)
{
    WebPluginContainerImpl* container = WebLocalFrameImpl::pluginContainerFromNode(frame, WebNode(focusedElement()));
    if (container && container->supportsInputMethod())
        return container->plugin();
    return 0;
}

bool WebViewImpl::selectionTextDirection(WebTextDirection& start, WebTextDirection& end) const
{
    const Frame* frame = focusedCoreFrame();
    if (!frame || frame->isRemoteFrame())
        return false;
    FrameSelection& selection = toLocalFrame(frame)->selection();
    if (!selection.toNormalizedRange())
        return false;
    start = toWebTextDirection(selection.start().primaryDirection());
    end = toWebTextDirection(selection.end().primaryDirection());
    return true;
}

bool WebViewImpl::isSelectionAnchorFirst() const
{
    const Frame* frame = focusedCoreFrame();
    if (!frame || frame->isRemoteFrame())
        return false;
    return toLocalFrame(frame)->selection().selection().isBaseFirst();
}

WebVector<WebCompositionUnderline> WebViewImpl::compositionUnderlines() const
{
    const LocalFrame* focused = toLocalFrame(focusedCoreFrame());
    if (!focused)
        return WebVector<WebCompositionUnderline>();
    const Vector<CompositionUnderline>& underlines = focused->inputMethodController().customCompositionUnderlines();
    WebVector<WebCompositionUnderline> results(underlines.size());
    for (size_t index = 0; index < underlines.size(); ++index) {
        CompositionUnderline underline = underlines[index];
        results[index] = WebCompositionUnderline(underline.startOffset, underline.endOffset, static_cast<WebColor>(underline.color.rgb()), underline.thick, static_cast<WebColor>(underline.backgroundColor.rgb()));
    }
    return results;
}

WebColor WebViewImpl::backgroundColor() const
{
    if (isTransparent())
        return Color::transparent;
    if (!m_page)
        return m_baseBackgroundColor;
    if (!m_page->mainFrame())
        return m_baseBackgroundColor;
    if (!m_page->mainFrame()->isLocalFrame())
        return m_baseBackgroundColor;
    FrameView* view = m_page->deprecatedLocalMainFrame()->view();
    return view->documentBackgroundColor().rgb();
}

WebPagePopup* WebViewImpl::pagePopup() const
{
    return m_pagePopup.get();
}

bool WebViewImpl::caretOrSelectionRange(size_t* location, size_t* length)
{
    const LocalFrame* focused = toLocalFrame(focusedCoreFrame());
    if (!focused)
        return false;

    PlainTextRange selectionOffsets = focused->inputMethodController().getSelectionOffsets();
    if (selectionOffsets.isNull())
        return false;

    *location = selectionOffsets.start();
    *length = selectionOffsets.length();
    return true;
}

void WebViewImpl::setTextDirection(WebTextDirection direction)
{
    // The Editor::setBaseWritingDirection() function checks if we can change
    // the text direction of the selected node and updates its DOM "dir"
    // attribute and its CSS "direction" property.
    // So, we just call the function as Safari does.
    const LocalFrame* focused = toLocalFrame(focusedCoreFrame());
    if (!focused)
        return;

    Editor& editor = focused->editor();
    if (!editor.canEdit())
        return;

    switch (direction) {
    case WebTextDirectionDefault:
        editor.setBaseWritingDirection(NaturalWritingDirection);
        break;

    case WebTextDirectionLeftToRight:
        editor.setBaseWritingDirection(LeftToRightWritingDirection);
        break;

    case WebTextDirectionRightToLeft:
        editor.setBaseWritingDirection(RightToLeftWritingDirection);
        break;

    default:
        notImplemented();
        break;
    }
}

bool WebViewImpl::isAcceleratedCompositingActive() const
{
    return m_rootLayer;
}

void WebViewImpl::willCloseLayerTreeView()
{
    setRootGraphicsLayer(nullptr);
    m_layerTreeView = 0;
}

void WebViewImpl::didAcquirePointerLock()
{
    if (page())
        page()->pointerLockController().didAcquirePointerLock();
}

void WebViewImpl::didNotAcquirePointerLock()
{
    if (page())
        page()->pointerLockController().didNotAcquirePointerLock();
}

void WebViewImpl::didLosePointerLock()
{
    m_pointerLockGestureToken.clear();
    if (page())
        page()->pointerLockController().didLosePointerLock();
}

void WebViewImpl::didChangeWindowResizerRect()
{
    if (mainFrameImpl()->frameView())
        mainFrameImpl()->frameView()->windowResizerRectChanged();
}

// WebView --------------------------------------------------------------------

WebSettingsImpl* WebViewImpl::settingsImpl()
{
    if (!m_webSettings)
        m_webSettings = adoptPtr(new WebSettingsImpl(&m_page->settings(), m_devToolsEmulator.get()));
    ASSERT(m_webSettings);
    return m_webSettings.get();
}

WebSettings* WebViewImpl::settings()
{
    return settingsImpl();
}

WebString WebViewImpl::pageEncoding() const
{
    if (!m_page)
        return WebString();

    if (!m_page->mainFrame()->isLocalFrame())
        return WebString();

    // FIXME: Is this check needed?
    if (!m_page->deprecatedLocalMainFrame()->document()->loader())
        return WebString();

    return m_page->deprecatedLocalMainFrame()->document()->encodingName();
}

void WebViewImpl::setPageEncoding(const WebString& encodingName)
{
    if (!m_page)
        return;

    // Only change override encoding, don't change default encoding.
    // Note that the new encoding must be 0 if it isn't supposed to be set.
    AtomicString newEncodingName;
    if (!encodingName.isEmpty())
        newEncodingName = encodingName;
    m_page->frameHost().setOverrideEncoding(newEncodingName);

    if (m_page->mainFrame()->isLocalFrame()) {
        if (!m_page->deprecatedLocalMainFrame()->loader().currentItem())
            return;
        FrameLoadRequest request = FrameLoadRequest(
            nullptr,
            m_page->deprecatedLocalMainFrame()->loader().resourceRequestForReload(
                FrameLoadTypeReload, KURL(), ClientRedirect));
        request.setClientRedirect(ClientRedirect);
        m_page->deprecatedLocalMainFrame()->loader().load(request, FrameLoadTypeReload);
    }
}

WebFrame* WebViewImpl::mainFrame()
{
    return WebFrame::fromFrame(m_page ? m_page->mainFrame() : 0);
}

WebFrame* WebViewImpl::findFrameByName(
    const WebString& name, WebFrame* relativeToFrame)
{
    // FIXME: Either this should only deal with WebLocalFrames or it should move to WebFrame.
    if (!relativeToFrame)
        relativeToFrame = mainFrame();
    Frame* frame = toWebLocalFrameImpl(relativeToFrame)->frame();
    frame = frame->tree().find(name);
    if (!frame || !frame->isLocalFrame())
        return 0;
    return WebLocalFrameImpl::fromFrame(toLocalFrame(frame));
}

WebFrame* WebViewImpl::focusedFrame()
{
    return WebFrame::fromFrame(focusedCoreFrame());
}

void WebViewImpl::setFocusedFrame(WebFrame* frame)
{
    if (!frame) {
        // Clears the focused frame if any.
        Frame* focusedFrame = focusedCoreFrame();
        if (focusedFrame && focusedFrame->isLocalFrame())
            toLocalFrame(focusedFrame)->selection().setFocused(false);
        return;
    }
    LocalFrame* coreFrame = toWebLocalFrameImpl(frame)->frame();
    coreFrame->page()->focusController().setFocusedFrame(coreFrame);
}

void WebViewImpl::setInitialFocus(bool reverse)
{
    if (!m_page)
        return;
    Frame* frame = page()->focusController().focusedOrMainFrame();
    if (frame->isLocalFrame()) {
        if (Document* document = toLocalFrame(frame)->document())
            document->setFocusedElement(nullptr);
    }
    page()->focusController().setInitialFocus(reverse ? WebFocusTypeBackward : WebFocusTypeForward);
}

void WebViewImpl::clearFocusedElement()
{
    RefPtrWillBeRawPtr<Frame> frame = focusedCoreFrame();
    if (!frame || !frame->isLocalFrame())
        return;

    LocalFrame* localFrame = toLocalFrame(frame.get());

    RefPtrWillBeRawPtr<Document> document = localFrame->document();
    if (!document)
        return;

    RefPtrWillBeRawPtr<Element> oldFocusedElement = document->focusedElement();

    // Clear the focused node.
    document->setFocusedElement(nullptr);

    if (!oldFocusedElement)
        return;

    // If a text field has focus, we need to make sure the selection controller
    // knows to remove selection from it. Otherwise, the text field is still
    // processing keyboard events even though focus has been moved to the page and
    // keystrokes get eaten as a result.
    if (oldFocusedElement->isContentEditable() || oldFocusedElement->isTextFormControl())
        localFrame->selection().clear();
}

bool WebViewImpl::scrollFocusedNodeIntoRect(const WebRect& rectInViewport)
{
    LocalFrame* frame = page()->mainFrame() && page()->mainFrame()->isLocalFrame()
        ? page()->deprecatedLocalMainFrame() : 0;
    Element* element = focusedElement();
    if (!frame || !frame->view() || !element)
        return false;

    bool zoomInToLegibleScale = m_webSettings->autoZoomFocusedNodeToLegibleScale() && !shouldDisableDesktopWorkarounds();

    float scale;
    IntPoint scroll;
    bool needAnimation;
    computeScaleAndScrollForFocusedNode(element, zoomInToLegibleScale, scale, scroll, needAnimation);
    if (needAnimation)
        return startPageScaleAnimation(scroll, false, scale, scrollAndScaleAnimationDurationInSeconds);

    return false;
}

void WebViewImpl::computeScaleAndScrollForFocusedNode(Node* focusedNode, bool zoomInToLegibleScale, float& newScale, IntPoint& newScroll, bool& needAnimation)
{
    focusedNode->document().updateLayoutIgnorePendingStylesheets();

    PinchViewport& pinchViewport = page()->frameHost().pinchViewport();

    WebRect caretInViewport, unusedEnd;
    selectionBounds(caretInViewport, unusedEnd);

    // 'caretInDocument' is rect encompassing the blinking cursor relative to the root document.
    IntRect caretInDocument = mainFrameImpl()->frameView()->frameToContents(pinchViewport.viewportToRootFrame(caretInViewport));
    IntRect textboxRectInDocument = mainFrameImpl()->frameView()->frameToContents(
        focusedNode->document().view()->contentsToRootFrame(pixelSnappedIntRect(focusedNode->Node::boundingBox())));

    if (!zoomInToLegibleScale) {
        newScale = pageScaleFactor();
    } else {
        // Pick a scale which is reasonably readable. This is the scale at which
        // the caret height will become minReadableCaretHeightForNode (adjusted
        // for dpi and font scale factor).
        const int minReadableCaretHeightForNode = textboxRectInDocument.height() >= 2 * caretInDocument.height() ? minReadableCaretHeightForTextArea : minReadableCaretHeight;
        newScale = clampPageScaleFactorToLimits(maximumLegiblePageScale() * minReadableCaretHeightForNode / caretInDocument.height());
        newScale = std::max(newScale, pageScaleFactor());
    }
    const float deltaScale = newScale / pageScaleFactor();

    needAnimation = false;

    // If we are at less than the target zoom level, zoom in.
    if (deltaScale > minScaleChangeToTriggerZoom)
        needAnimation = true;
    else
        newScale = pageScaleFactor();

    // If the caret is offscreen, then animate.
    if (!pinchViewport.visibleRectInDocument().contains(caretInDocument))
        needAnimation = true;

    // If the box is partially offscreen and it's possible to bring it fully
    // onscreen, then animate.
    if (pinchViewport.visibleRect().width() >= textboxRectInDocument.width()
        && pinchViewport.visibleRect().height() >= textboxRectInDocument.height()
        && !pinchViewport.visibleRectInDocument().contains(textboxRectInDocument))
        needAnimation = true;

    if (!needAnimation)
        return;

    FloatSize targetViewportSize = pinchViewport.size();
    targetViewportSize.scale(1 / newScale);

    if (textboxRectInDocument.width() <= targetViewportSize.width()) {
        // Field is narrower than screen. Try to leave padding on left so field's
        // label is visible, but it's more important to ensure entire field is
        // onscreen.
        int idealLeftPadding = targetViewportSize.width() * leftBoxRatio;
        int maxLeftPaddingKeepingBoxOnscreen = targetViewportSize.width() - textboxRectInDocument.width();
        newScroll.setX(textboxRectInDocument.x() - std::min<int>(idealLeftPadding, maxLeftPaddingKeepingBoxOnscreen));
    } else {
        // Field is wider than screen. Try to left-align field, unless caret would
        // be offscreen, in which case right-align the caret.
        newScroll.setX(std::max<int>(textboxRectInDocument.x(), caretInDocument.x() + caretInDocument.width() + caretPadding - targetViewportSize.width()));
    }
    if (textboxRectInDocument.height() <= targetViewportSize.height()) {
        // Field is shorter than screen. Vertically center it.
        newScroll.setY(textboxRectInDocument.y() - (targetViewportSize.height() - textboxRectInDocument.height()) / 2);
    } else {
        // Field is taller than screen. Try to top align field, unless caret would
        // be offscreen, in which case bottom-align the caret.
        newScroll.setY(std::max<int>(textboxRectInDocument.y(), caretInDocument.y() + caretInDocument.height() + caretPadding - targetViewportSize.height()));
    }
}

void WebViewImpl::advanceFocus(bool reverse)
{
    page()->focusController().advanceFocus(reverse ? WebFocusTypeBackward : WebFocusTypeForward);
}

double WebViewImpl::zoomLevel()
{
    return m_zoomLevel;
}

double WebViewImpl::setZoomLevel(double zoomLevel)
{
    if (zoomLevel < m_minimumZoomLevel)
        m_zoomLevel = m_minimumZoomLevel;
    else if (zoomLevel > m_maximumZoomLevel)
        m_zoomLevel = m_maximumZoomLevel;
    else
        m_zoomLevel = zoomLevel;

    LocalFrame* frame = mainFrameImpl()->frame();
    WebPluginContainerImpl* pluginContainer = WebLocalFrameImpl::pluginContainerFromFrame(frame);
    if (pluginContainer)
        pluginContainer->plugin()->setZoomLevel(m_zoomLevel, false);
    else {
        float zoomFactor = m_zoomFactorOverride ? m_zoomFactorOverride : static_cast<float>(zoomLevelToZoomFactor(m_zoomLevel));
        frame->setPageZoomFactor(zoomFactor);
    }

    return m_zoomLevel;
}

void WebViewImpl::zoomLimitsChanged(double minimumZoomLevel,
                                    double maximumZoomLevel)
{
    m_minimumZoomLevel = minimumZoomLevel;
    m_maximumZoomLevel = maximumZoomLevel;
    m_client->zoomLimitsChanged(m_minimumZoomLevel, m_maximumZoomLevel);
}

float WebViewImpl::textZoomFactor()
{
    return mainFrameImpl()->frame()->textZoomFactor();
}

float WebViewImpl::setTextZoomFactor(float textZoomFactor)
{
    LocalFrame* frame = mainFrameImpl()->frame();
    if (WebLocalFrameImpl::pluginContainerFromFrame(frame))
        return 1;

    frame->setTextZoomFactor(textZoomFactor);

    return textZoomFactor;
}

void WebViewImpl::fullFramePluginZoomLevelChanged(double zoomLevel)
{
    if (zoomLevel == m_zoomLevel)
        return;

    m_zoomLevel = std::max(std::min(zoomLevel, m_maximumZoomLevel), m_minimumZoomLevel);
    m_client->zoomLevelChanged();
}

double WebView::zoomLevelToZoomFactor(double zoomLevel)
{
    return pow(textSizeMultiplierRatio, zoomLevel);
}

double WebView::zoomFactorToZoomLevel(double factor)
{
    // Since factor = 1.2^level, level = log(factor) / log(1.2)
    return log(factor) / log(textSizeMultiplierRatio);
}

float WebViewImpl::pageScaleFactor() const
{
    if (!page())
        return 1;

    return page()->frameHost().pinchViewport().scale();
}

float WebViewImpl::clampPageScaleFactorToLimits(float scaleFactor) const
{
    return pageScaleConstraintsSet().finalConstraints().clampToConstraints(scaleFactor);
}

void WebViewImpl::setPinchViewportOffset(const WebFloatPoint& offset)
{
    ASSERT(page());
    page()->frameHost().pinchViewport().setLocation(offset);
}

WebFloatPoint WebViewImpl::pinchViewportOffset() const
{
    ASSERT(page());
    return page()->frameHost().pinchViewport().visibleRect().location();
}

void WebViewImpl::scrollAndRescaleViewports(float scaleFactor,
    const IntPoint& mainFrameOrigin,
    const FloatPoint& pinchViewportOrigin)
{
    if (!page())
        return;

    if (!mainFrameImpl())
        return;

    FrameView * view = mainFrameImpl()->frameView();
    if (!view)
        return;

    // Order is important: pinch viewport location is clamped based on
    // main frame scroll position and pinch viewport scale.

    view->setScrollPosition(mainFrameOrigin, ProgrammaticScroll);

    setPageScaleFactor(scaleFactor);

    page()->frameHost().pinchViewport().setLocation(pinchViewportOrigin);
}

void WebViewImpl::setPageScaleFactorAndLocation(float scaleFactor, const FloatPoint& location)
{
    ASSERT(page());

    page()->frameHost().pinchViewport().setScaleAndLocation(
        clampPageScaleFactorToLimits(scaleFactor),
        location);
}

void WebViewImpl::setPageScaleFactor(float scaleFactor)
{
    ASSERT(page());

    scaleFactor = clampPageScaleFactorToLimits(scaleFactor);
    if (scaleFactor == pageScaleFactor())
        return;

    page()->frameHost().pinchViewport().setScale(scaleFactor);
}

float WebViewImpl::deviceScaleFactor() const
{
    if (!page())
        return 1;

    return page()->deviceScaleFactor();
}

void WebViewImpl::setDeviceScaleFactor(float scaleFactor)
{
    if (!page())
        return;

    page()->setDeviceScaleFactor(scaleFactor);

    if (m_layerTreeView)
        updateLayerTreeDeviceScaleFactor();
}

void WebViewImpl::setDeviceColorProfile(const WebVector<char>& colorProfile)
{
    if (!page())
        return;

    Vector<char> deviceProfile;
    deviceProfile.append(colorProfile.data(), colorProfile.size());

    page()->setDeviceColorProfile(deviceProfile);
}

void WebViewImpl::resetDeviceColorProfile()
{
    if (!page())
        return;

    page()->resetDeviceColorProfile();
}

void WebViewImpl::enableAutoResizeMode(const WebSize& minSize, const WebSize& maxSize)
{
    m_shouldAutoResize = true;
    m_minAutoSize = minSize;
    m_maxAutoSize = maxSize;
    configureAutoResizeMode();
}

void WebViewImpl::disableAutoResizeMode()
{
    m_shouldAutoResize = false;
    configureAutoResizeMode();
}

void WebViewImpl::setDefaultPageScaleLimits(float minScale, float maxScale)
{
    return page()->frameHost().setDefaultPageScaleLimits(minScale, maxScale);
}

void WebViewImpl::setInitialPageScaleOverride(float initialPageScaleFactorOverride)
{
    PageScaleConstraints constraints = pageScaleConstraintsSet().userAgentConstraints();
    constraints.initialScale = initialPageScaleFactorOverride;

    if (constraints == pageScaleConstraintsSet().userAgentConstraints())
        return;

    pageScaleConstraintsSet().setNeedsReset(true);
    page()->frameHost().setUserAgentPageScaleConstraints(constraints);
}

void WebViewImpl::setMaximumLegibleScale(float maximumLegibleScale)
{
    m_maximumLegibleScale = maximumLegibleScale;
}

void WebViewImpl::setIgnoreViewportTagScaleLimits(bool ignore)
{
    PageScaleConstraints constraints = pageScaleConstraintsSet().userAgentConstraints();
    if (ignore) {
        constraints.minimumScale = pageScaleConstraintsSet().defaultConstraints().minimumScale;
        constraints.maximumScale = pageScaleConstraintsSet().defaultConstraints().maximumScale;
    } else {
        constraints.minimumScale = -1;
        constraints.maximumScale = -1;
    }
    page()->frameHost().setUserAgentPageScaleConstraints(constraints);
}

IntSize WebViewImpl::mainFrameSize()
{
    return pageScaleConstraintsSet().mainFrameSize();
}

PageScaleConstraintsSet& WebViewImpl::pageScaleConstraintsSet() const
{
    return page()->frameHost().pageScaleConstraintsSet();
}

void WebViewImpl::refreshPageScaleFactorAfterLayout()
{
    if (!mainFrame() || !page() || !page()->mainFrame() || !page()->mainFrame()->isLocalFrame() || !page()->deprecatedLocalMainFrame()->view())
        return;
    FrameView* view = page()->deprecatedLocalMainFrame()->view();

    updatePageDefinedViewportConstraints(mainFrameImpl()->frame()->document()->viewportDescription());
    pageScaleConstraintsSet().computeFinalConstraints();

    int verticalScrollbarWidth = 0;
    if (view->verticalScrollbar() && !view->verticalScrollbar()->isOverlayScrollbar())
        verticalScrollbarWidth = view->verticalScrollbar()->width();
    pageScaleConstraintsSet().adjustFinalConstraintsToContentsSize(contentsSize(), verticalScrollbarWidth, settings()->shrinksViewportContentToFit());

    float newPageScaleFactor = pageScaleFactor();
    if (pageScaleConstraintsSet().needsReset() && pageScaleConstraintsSet().finalConstraints().initialScale != -1) {
        newPageScaleFactor = pageScaleConstraintsSet().finalConstraints().initialScale;
        pageScaleConstraintsSet().setNeedsReset(false);
    }
    setPageScaleFactor(newPageScaleFactor);

    updateLayerTreeViewport();
}

void WebViewImpl::updatePageDefinedViewportConstraints(const ViewportDescription& description)
{
    // If we're not reading the viewport meta tag, allow GPU rasterization.
    if (!settingsImpl()->viewportMetaEnabled()) {
        m_matchesHeuristicsForGpuRasterization = true;
        if (m_layerTreeView)
            m_layerTreeView->heuristicsForGpuRasterizationUpdated(m_matchesHeuristicsForGpuRasterization);
    }

    if (!settings()->viewportEnabled() || !page() || (!m_size.width && !m_size.height) || !page()->mainFrame()->isLocalFrame())
        return;

    Document* document = page()->deprecatedLocalMainFrame()->document();

    m_matchesHeuristicsForGpuRasterization = description.matchesHeuristicsForGpuRasterization();
    if (m_layerTreeView)
        m_layerTreeView->heuristicsForGpuRasterizationUpdated(m_matchesHeuristicsForGpuRasterization);

    Length defaultMinWidth = document->viewportDefaultMinWidth();
    if (defaultMinWidth.isAuto())
        defaultMinWidth = Length(ExtendToZoom);

    ViewportDescription adjustedDescription = description;
    if (settingsImpl()->viewportMetaLayoutSizeQuirk() && adjustedDescription.type == ViewportDescription::ViewportMeta) {
        const int legacyWidthSnappingMagicNumber = 320;
        if (adjustedDescription.maxWidth.isFixed() && adjustedDescription.maxWidth.value() <= legacyWidthSnappingMagicNumber)
            adjustedDescription.maxWidth = Length(DeviceWidth);
        if (adjustedDescription.maxHeight.isFixed() && adjustedDescription.maxHeight.value() <= m_size.height)
            adjustedDescription.maxHeight = Length(DeviceHeight);
        adjustedDescription.minWidth = adjustedDescription.maxWidth;
        adjustedDescription.minHeight = adjustedDescription.maxHeight;
    }

    float oldInitialScale = pageScaleConstraintsSet().pageDefinedConstraints().initialScale;
    pageScaleConstraintsSet().updatePageDefinedConstraints(adjustedDescription, defaultMinWidth);

    if (settingsImpl()->clobberUserAgentInitialScaleQuirk()
        && pageScaleConstraintsSet().userAgentConstraints().initialScale != -1
        && pageScaleConstraintsSet().userAgentConstraints().initialScale * deviceScaleFactor() <= 1) {
        if (description.maxWidth == Length(DeviceWidth)
            || (description.maxWidth.type() == Auto && pageScaleConstraintsSet().pageDefinedConstraints().initialScale == 1.0f))
            setInitialPageScaleOverride(-1);
    }

    pageScaleConstraintsSet().adjustForAndroidWebViewQuirks(adjustedDescription, defaultMinWidth.intValue(), deviceScaleFactor(), settingsImpl()->supportDeprecatedTargetDensityDPI(), page()->settings().wideViewportQuirkEnabled(), page()->settings().useWideViewport(), page()->settings().loadWithOverviewMode(), settingsImpl()->viewportMetaNonUserScalableQuirk());
    float newInitialScale = pageScaleConstraintsSet().pageDefinedConstraints().initialScale;
    if (oldInitialScale != newInitialScale && newInitialScale != -1) {
        pageScaleConstraintsSet().setNeedsReset(true);
        if (mainFrameImpl() && mainFrameImpl()->frameView())
            mainFrameImpl()->frameView()->setNeedsLayout();
    }

    updateMainFrameLayoutSize();

    if (LocalFrame* frame = page()->deprecatedLocalMainFrame()) {
        if (TextAutosizer* textAutosizer = frame->document()->textAutosizer())
            textAutosizer->updatePageInfoInAllFrames();
    }
}

void WebViewImpl::updateMainFrameLayoutSize()
{
    if (m_shouldAutoResize || !mainFrameImpl())
        return;

    RefPtrWillBeRawPtr<FrameView> view = mainFrameImpl()->frameView();
    if (!view)
        return;

    WebSize layoutSize = m_size;

    if (settings()->viewportEnabled())
        layoutSize = pageScaleConstraintsSet().layoutSize();

    if (page()->settings().forceZeroLayoutHeight())
        layoutSize.height = 0;

    view->setLayoutSize(layoutSize);

    // Resizing marks the frame as needsLayout. Inform clients so that they
    // will perform the layout. Widgets held by WebPluginContainerImpl do not otherwise
    // see this resize layout invalidation.
    if (client())
        client()->didUpdateLayoutSize(layoutSize);
}

IntSize WebViewImpl::contentsSize() const
{
    if (!page()->mainFrame()->isLocalFrame())
        return IntSize();
    LayoutView* root = page()->deprecatedLocalMainFrame()->contentLayoutObject();
    if (!root)
        return IntSize();
    return root->documentRect().size();
}

WebSize WebViewImpl::contentsPreferredMinimumSize()
{
    Document* document = m_page->mainFrame()->isLocalFrame() ? m_page->deprecatedLocalMainFrame()->document() : 0;
    if (!document || !document->layoutView() || !document->documentElement())
        return WebSize();

    layout();
    int widthScaled = document->layoutView()->minPreferredLogicalWidth().round(); // Already accounts for zoom.
    int heightScaled = static_cast<int>((document->documentElement()->scrollHeight() * zoomLevelToZoomFactor(zoomLevel())) + 0.5); // +0.5 to round rather than truncating
    return IntSize(widthScaled, heightScaled);
}

void WebViewImpl::enableViewport()
{
    settings()->setViewportEnabled(true);
}

void WebViewImpl::disableViewport()
{
    settings()->setViewportEnabled(false);
    pageScaleConstraintsSet().clearPageDefinedConstraints();
    updateMainFrameLayoutSize();
}

float WebViewImpl::defaultMinimumPageScaleFactor() const
{
    return pageScaleConstraintsSet().defaultConstraints().minimumScale;
}

float WebViewImpl::defaultMaximumPageScaleFactor() const
{
    return pageScaleConstraintsSet().defaultConstraints().maximumScale;
}

float WebViewImpl::minimumPageScaleFactor() const
{
    return pageScaleConstraintsSet().finalConstraints().minimumScale;
}

float WebViewImpl::maximumPageScaleFactor() const
{
    return pageScaleConstraintsSet().finalConstraints().maximumScale;
}

void WebViewImpl::resetScrollAndScaleState()
{
    page()->frameHost().pinchViewport().reset();

    if (!page()->mainFrame()->isLocalFrame())
        return;

    if (FrameView* frameView = toLocalFrame(page()->mainFrame())->view()) {
        ScrollableArea* scrollableArea = frameView->layoutViewportScrollableArea();

        if (scrollableArea->scrollPositionDouble() != DoublePoint::zero())
            scrollableArea->setScrollPosition(DoublePoint::zero(), ProgrammaticScroll);
    }

    pageScaleConstraintsSet().setNeedsReset(true);

    // Clobber saved scales and scroll offsets.
    if (FrameView* view = page()->deprecatedLocalMainFrame()->document()->view())
        view->cacheCurrentScrollPosition();
}

void WebViewImpl::performMediaPlayerAction(const WebMediaPlayerAction& action,
                                           const WebPoint& location)
{
    HitTestResult result = hitTestResultForViewportPos(location);
    RefPtrWillBeRawPtr<Node> node = result.innerNode();
    if (!isHTMLVideoElement(*node) && !isHTMLAudioElement(*node))
        return;

    RefPtrWillBeRawPtr<HTMLMediaElement> mediaElement = static_pointer_cast<HTMLMediaElement>(node);
    switch (action.type) {
    case WebMediaPlayerAction::Play:
        if (action.enable)
            mediaElement->play();
        else
            mediaElement->pause();
        break;
    case WebMediaPlayerAction::Mute:
        mediaElement->setMuted(action.enable);
        break;
    case WebMediaPlayerAction::Loop:
        mediaElement->setLoop(action.enable);
        break;
    case WebMediaPlayerAction::Controls:
        mediaElement->setBooleanAttribute(HTMLNames::controlsAttr, action.enable);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

void WebViewImpl::performPluginAction(const WebPluginAction& action,
                                      const WebPoint& location)
{
    // FIXME: Location is probably in viewport coordinates
    HitTestResult result = hitTestResultForRootFramePos(location);
    RefPtrWillBeRawPtr<Node> node = result.innerNode();
    if (!isHTMLObjectElement(*node) && !isHTMLEmbedElement(*node))
        return;

    LayoutObject* object = node->layoutObject();
    if (object && object->isLayoutPart()) {
        Widget* widget = toLayoutPart(object)->widget();
        if (widget && widget->isPluginContainer()) {
            WebPluginContainerImpl* plugin = toWebPluginContainerImpl(widget);
            switch (action.type) {
            case WebPluginAction::Rotate90Clockwise:
                plugin->plugin()->rotateView(WebPlugin::RotationType90Clockwise);
                break;
            case WebPluginAction::Rotate90Counterclockwise:
                plugin->plugin()->rotateView(WebPlugin::RotationType90Counterclockwise);
                break;
            default:
                ASSERT_NOT_REACHED();
            }
        }
    }
}

WebHitTestResult WebViewImpl::hitTestResultAt(const WebPoint& point)
{
    return coreHitTestResultAt(point);
}

HitTestResult WebViewImpl::coreHitTestResultAt(const WebPoint& pointInViewport)
{
    FrameView* view = mainFrameImpl()->frameView();
    IntPoint pointInRootFrame = view->contentsToFrame(view->viewportToContents(pointInViewport));
    return hitTestResultForRootFramePos(pointInRootFrame);
}

void WebViewImpl::copyImageAt(const WebPoint& point)
{
    if (!m_page)
        return;

    HitTestResult result = hitTestResultForViewportPos(point);
    if (!isHTMLCanvasElement(result.innerNodeOrImageMapImage()) && result.absoluteImageURL().isEmpty()) {
        // There isn't actually an image at these coordinates.  Might be because
        // the window scrolled while the context menu was open or because the page
        // changed itself between when we thought there was an image here and when
        // we actually tried to retreive the image.
        //
        // FIXME: implement a cache of the most recent HitTestResult to avoid having
        //        to do two hit tests.
        return;
    }

    m_page->deprecatedLocalMainFrame()->editor().copyImage(result);
}

void WebViewImpl::saveImageAt(const WebPoint& point)
{
    if (!m_client)
        return;

    Node* node = hitTestResultForViewportPos(point).innerNodeOrImageMapImage();
    if (!node || !(isHTMLCanvasElement(*node) || isHTMLImageElement(*node)))
        return;

    String url = toElement(*node).imageSourceURL();
    if (!KURL(KURL(), url).protocolIsData())
        return;

    m_client->saveImageFromDataURL(url);
}

void WebViewImpl::dragSourceEndedAt(
    const WebPoint& clientPoint,
    const WebPoint& screenPoint,
    WebDragOperation operation)
{
    PlatformMouseEvent pme(clientPoint, screenPoint, LeftButton, PlatformEvent::MouseMoved,
        0, false, false, false, false, PlatformMouseEvent::RealOrIndistinguishable, 0);
    m_page->deprecatedLocalMainFrame()->eventHandler().dragSourceEndedAt(pme,
        static_cast<DragOperation>(operation));
}

void WebViewImpl::dragSourceSystemDragEnded()
{
    // It's possible for us to get this callback while not doing a drag if
    // it's from a previous page that got unloaded.
    if (m_doingDragAndDrop) {
        m_page->dragController().dragEnded();
        m_doingDragAndDrop = false;
    }
}

WebDragOperation WebViewImpl::dragTargetDragEnter(
    const WebDragData& webDragData,
    const WebPoint& clientPoint,
    const WebPoint& screenPoint,
    WebDragOperationsMask operationsAllowed,
    int modifiers)
{
    ASSERT(!m_currentDragData);

    m_currentDragData = DataObject::create(webDragData);
    m_operationsAllowed = operationsAllowed;

    return dragTargetDragEnterOrOver(clientPoint, screenPoint, DragEnter, modifiers);
}

WebDragOperation WebViewImpl::dragTargetDragOver(
    const WebPoint& clientPoint,
    const WebPoint& screenPoint,
    WebDragOperationsMask operationsAllowed,
    int modifiers)
{
    m_operationsAllowed = operationsAllowed;

    return dragTargetDragEnterOrOver(clientPoint, screenPoint, DragOver, modifiers);
}

void WebViewImpl::dragTargetDragLeave()
{
    ASSERT(m_currentDragData);

    DragData dragData(
        m_currentDragData.get(),
        IntPoint(),
        IntPoint(),
        static_cast<DragOperation>(m_operationsAllowed));

    m_page->dragController().dragExited(&dragData);

    // FIXME: why is the drag scroll timer not stopped here?

    m_dragOperation = WebDragOperationNone;
    m_currentDragData = nullptr;
}

void WebViewImpl::dragTargetDrop(const WebPoint& clientPoint,
                                 const WebPoint& screenPoint,
                                 int modifiers)
{
    ASSERT(m_currentDragData);

    WebAutofillClient* autofillClient = mainFrameImpl() ? mainFrameImpl()->autofillClient() : 0;
    UserGestureNotifier notifier(autofillClient, &m_userGestureObserved);

    // If this webview transitions from the "drop accepting" state to the "not
    // accepting" state, then our IPC message reply indicating that may be in-
    // flight, or else delayed by javascript processing in this webview.  If a
    // drop happens before our IPC reply has reached the browser process, then
    // the browser forwards the drop to this webview.  So only allow a drop to
    // proceed if our webview m_dragOperation state is not DragOperationNone.

    if (m_dragOperation == WebDragOperationNone) { // IPC RACE CONDITION: do not allow this drop.
        dragTargetDragLeave();
        return;
    }

    m_currentDragData->setModifiers(toPlatformMouseEventModifiers(modifiers));
    DragData dragData(
        m_currentDragData.get(),
        clientPoint,
        screenPoint,
        static_cast<DragOperation>(m_operationsAllowed));

    UserGestureIndicator gesture(DefinitelyProcessingNewUserGesture);
    m_page->dragController().performDrag(&dragData);

    m_dragOperation = WebDragOperationNone;
    m_currentDragData = nullptr;
}

void WebViewImpl::spellingMarkers(WebVector<uint32_t>* markers)
{
    Vector<uint32_t> result;
    for (Frame* frame = m_page->mainFrame(); frame; frame = frame->tree().traverseNext()) {
        if (!frame->isLocalFrame())
            continue;
        const DocumentMarkerVector& documentMarkers = toLocalFrame(frame)->document()->markers().markers();
        for (size_t i = 0; i < documentMarkers.size(); ++i)
            result.append(documentMarkers[i]->hash());
    }
    markers->assign(result);
}

void WebViewImpl::removeSpellingMarkersUnderWords(const WebVector<WebString>& words)
{
    Vector<String> convertedWords;
    convertedWords.append(words.data(), words.size());

    for (Frame* frame = m_page->mainFrame(); frame; frame = frame->tree().traverseNext()) {
        if (frame->isLocalFrame())
            toLocalFrame(frame)->removeSpellingMarkersUnderWords(convertedWords);
    }
}

WebDragOperation WebViewImpl::dragTargetDragEnterOrOver(const WebPoint& clientPoint, const WebPoint& screenPoint, DragAction dragAction, int modifiers)
{
    ASSERT(m_currentDragData);

    m_currentDragData->setModifiers(toPlatformMouseEventModifiers(modifiers));
    DragData dragData(
        m_currentDragData.get(),
        clientPoint,
        screenPoint,
        static_cast<DragOperation>(m_operationsAllowed));

    DragSession dragSession;
    if (dragAction == DragEnter)
        dragSession = m_page->dragController().dragEntered(&dragData);
    else
        dragSession = m_page->dragController().dragUpdated(&dragData);

    DragOperation dropEffect = dragSession.operation;

    // Mask the drop effect operation against the drag source's allowed operations.
    if (!(dropEffect & dragData.draggingSourceOperationMask()))
        dropEffect = DragOperationNone;

     m_dragOperation = static_cast<WebDragOperation>(dropEffect);

    return m_dragOperation;
}

void WebViewImpl::sendResizeEventAndRepaint()
{
    // FIXME: This is wrong. The FrameView is responsible sending a resizeEvent
    // as part of layout. Layout is also responsible for sending invalidations
    // to the embedder. This method and all callers may be wrong. -- eseidel.
    if (mainFrameImpl()->frameView()) {
        // Enqueues the resize event.
        mainFrameImpl()->frame()->document()->enqueueResizeEvent();
    }

    if (m_client) {
        if (m_layerTreeView) {
            updateLayerTreeViewport();
        } else {
            WebRect damagedRect(0, 0, m_size.width, m_size.height);
            m_client->didInvalidateRect(damagedRect);
        }
    }
    if (m_pageOverlays)
        m_pageOverlays->update();

    m_devToolsEmulator->viewportChanged();
}

void WebViewImpl::configureAutoResizeMode()
{
    if (!mainFrameImpl() || !mainFrameImpl()->frame() || !mainFrameImpl()->frame()->view())
        return;

    if (m_shouldAutoResize)
        mainFrameImpl()->frame()->view()->enableAutoSizeMode(m_minAutoSize, m_maxAutoSize);
    else
        mainFrameImpl()->frame()->view()->disableAutoSizeMode();
}

unsigned long WebViewImpl::createUniqueIdentifierForRequest()
{
    return createUniqueIdentifier();
}

void WebViewImpl::setCompositorDeviceScaleFactorOverride(float deviceScaleFactor)
{
    if (m_compositorDeviceScaleFactorOverride == deviceScaleFactor)
        return;
    m_compositorDeviceScaleFactorOverride = deviceScaleFactor;
    if (page() && m_layerTreeView)
        updateLayerTreeDeviceScaleFactor();
}

void WebViewImpl::setRootLayerTransform(const WebSize& rootLayerOffset, float rootLayerScale)
{
    if (m_rootLayerScale == rootLayerScale && m_rootLayerOffset == rootLayerOffset)
        return;
    m_rootLayerScale = rootLayerScale;
    m_rootLayerOffset = rootLayerOffset;
    if (mainFrameImpl())
        mainFrameImpl()->setInputEventsTransformForEmulation(m_rootLayerOffset, m_rootLayerScale);
    updateRootLayerTransform();
}

void WebViewImpl::enableDeviceEmulation(const WebDeviceEmulationParams& params)
{
    m_devToolsEmulator->enableDeviceEmulation(params);
}

void WebViewImpl::disableDeviceEmulation()
{
    m_devToolsEmulator->disableDeviceEmulation();
}

#ifdef MINIBLINK_NOT_IMPLEMENTED
WebAXObject WebViewImpl::accessibilityObject()
{
    if (!mainFrameImpl())
        return WebAXObject();

    Document* document = mainFrameImpl()->frame()->document();
    return WebAXObject(toAXObjectCacheImpl(document->axObjectCache())->root());
}
#endif // MINIBLINK_NOT_IMPLEMENTED

void WebViewImpl::performCustomContextMenuAction(unsigned action)
{
    if (!m_page)
        return;
    ContextMenu* menu = m_page->contextMenuController().contextMenu();
    if (!menu)
        return;
    const ContextMenuItem* item = menu->itemWithAction(static_cast<ContextMenuAction>(ContextMenuItemBaseCustomTag + action));
    if (item)
        m_page->contextMenuController().contextMenuItemSelected(item);
    m_page->contextMenuController().clearContextMenu();
}

void WebViewImpl::showContextMenu()
{
    if (!page())
        return;

    page()->contextMenuController().clearContextMenu();
    m_contextMenuAllowed = true;
    if (LocalFrame* focusedFrame = toLocalFrame(page()->focusController().focusedOrMainFrame()))
        focusedFrame->eventHandler().sendContextMenuEventForKey(nullptr);
    m_contextMenuAllowed = false;
}

void WebViewImpl::extractSmartClipData(WebRect rectInViewport, WebString& clipText, WebString& clipHtml, WebRect& clipRectInViewport)
{
    LocalFrame* localFrame = toLocalFrame(focusedCoreFrame());
    if (!localFrame)
        return;
    SmartClipData clipData = SmartClip(localFrame).dataForRect(rectInViewport);
    clipText = clipData.clipData();
    clipRectInViewport = clipData.rectInViewport();

    WebLocalFrameImpl* frame = mainFrameImpl();
    if (!frame)
        return;
    WebPoint startPoint(rectInViewport.x, rectInViewport.y);
    WebPoint endPoint(rectInViewport.x + rectInViewport.width, rectInViewport.y + rectInViewport.height);
    VisiblePosition startVisiblePosition = frame->visiblePositionForViewportPoint(startPoint);
    VisiblePosition endVisiblePosition = frame->visiblePositionForViewportPoint(endPoint);

    Position startPosition = startVisiblePosition.deepEquivalent();
    Position endPosition = endVisiblePosition.deepEquivalent();

    // document() will return null if -webkit-user-select is set to none.
    if (!startPosition.document() || !endPosition.document())
        return;

    clipHtml = createMarkup(startPosition, endPosition, AnnotateForInterchange, ConvertBlocksToInlines::NotConvert, ResolveNonLocalURLs);
}

void WebViewImpl::hidePopups()
{
    cancelPagePopup();
}

void WebViewImpl::setIsTransparent(bool isTransparent)
{
    // Set any existing frames to be transparent.
    Frame* frame = m_page->mainFrame();
    while (frame) {
        if (frame->isLocalFrame())
            toLocalFrame(frame)->view()->setTransparent(isTransparent);
        frame = frame->tree().traverseNext();
    }

    // Future frames check this to know whether to be transparent.
    m_isTransparent = isTransparent;

    if (m_layerTreeView)
        m_layerTreeView->setHasTransparentBackground(this->isTransparent());
}

bool WebViewImpl::isTransparent() const
{
    return m_isTransparent;
}

void WebViewImpl::setBaseBackgroundColor(WebColor color)
{
    layout();

    if (m_baseBackgroundColor == color)
        return;

    m_baseBackgroundColor = color;

    if (m_page->mainFrame() && m_page->mainFrame()->isLocalFrame())
        m_page->deprecatedLocalMainFrame()->view()->setBaseBackgroundColor(color);

    updateLayerTreeBackgroundColor();
}

void WebViewImpl::setIsActive(bool active)
{
    if (page())
        page()->focusController().setActive(active);
}

bool WebViewImpl::isActive() const
{
    return page() ? page()->focusController().isActive() : false;
}

void WebViewImpl::setDomainRelaxationForbidden(bool forbidden, const WebString& scheme)
{
    SchemeRegistry::setDomainRelaxationForbiddenForURLScheme(forbidden, String(scheme));
}

void WebViewImpl::setWindowFeatures(const WebWindowFeatures& features)
{
    m_page->chromeClient().setWindowFeatures(features);
}

void WebViewImpl::setOpenedByDOM()
{
    m_page->setOpenedByDOM();
}

void WebViewImpl::setSelectionColors(unsigned activeBackgroundColor,
                                     unsigned activeForegroundColor,
                                     unsigned inactiveBackgroundColor,
                                     unsigned inactiveForegroundColor) {
#if USE(DEFAULT_RENDER_THEME)
    LayoutThemeDefault::setSelectionColors(activeBackgroundColor, activeForegroundColor, inactiveBackgroundColor, inactiveForegroundColor);
    LayoutTheme::theme().platformColorsDidChange();
#endif
}

void WebViewImpl::didCommitLoad(bool isNewNavigation, bool isNavigationWithinPage)
{
    if (isNewNavigation && !isNavigationWithinPage)
        pageScaleConstraintsSet().setNeedsReset(true);

    // Give the pinch viewport's scroll layer its initial size.
    page()->frameHost().pinchViewport().mainFrameDidChangeSize();

    // Make sure link highlight from previous page is cleared.
    m_linkHighlights.clear();
    endActiveFlingAnimation();
    m_userGestureObserved = false;
}

void WebViewImpl::willInsertBody(WebLocalFrameImpl* webframe)
{
    if (webframe != mainFrameImpl())
        return;

    if (!m_page->mainFrame()->isLocalFrame())
        return;

    // If we get to the <body> tag and we have no pending stylesheet and import load, we
    // can be fairly confident we'll have something sensible to paint soon and
    // can turn off deferred commits.
    if (m_page->deprecatedLocalMainFrame()->document()->isRenderingReady())
        resumeTreeViewCommits();
}

void WebViewImpl::didRemoveAllPendingStylesheet(WebLocalFrameImpl* webframe)
{
    if (webframe != mainFrameImpl())
        return;

    // If we have no more stylesheets to load and we're past the body tag,
    // we should have something to paint and should start as soon as possible.
    if (m_page->deprecatedLocalMainFrame()->document()->body())
        resumeTreeViewCommits();
}

void WebViewImpl::resumeTreeViewCommits()
{
    if (m_layerTreeView)
        m_layerTreeView->setDeferCommits(false);
}

void WebViewImpl::postLayoutResize(WebLocalFrameImpl* webframe)
{
    FrameView* view = webframe->frame()->view();
    if (webframe == mainFrame())
        view->resize(mainFrameSize());
    else
        view->resize(webframe->frameView()->size());
}

void WebViewImpl::layoutUpdated(WebLocalFrameImpl* webframe)
{
    if (!m_client || !webframe->frame()->isLocalRoot())
        return;

    // If we finished a layout while in deferred commit mode,
    // that means it's time to start producing frames again so un-defer.
    resumeTreeViewCommits();

    if (m_shouldAutoResize && webframe->frame() && webframe->frame()->view()) {
        WebSize frameSize = webframe->frame()->view()->frameRect().size();
        if (frameSize != m_size) {
            m_size = frameSize;

            page()->frameHost().pinchViewport().setSize(m_size);
            pageScaleConstraintsSet().didChangeViewSize(m_size);

            m_client->didAutoResize(m_size);
            sendResizeEventAndRepaint();
        }
    }

    if (pageScaleConstraintsSet().constraintsDirty())
        refreshPageScaleFactorAfterLayout();

    FrameView* view = webframe->frame()->view();

    postLayoutResize(webframe);

    // Relayout immediately to avoid violating the rule that needsLayout()
    // isn't set at the end of a layout.
    if (view->needsLayout())
        view->layout();

    // In case we didn't have a size when the top controls were updated.
    didUpdateTopControls();

    m_client->didUpdateLayout();
}

void WebViewImpl::didChangeContentsSize()
{
    pageScaleConstraintsSet().didChangeContentsSize(contentsSize(), pageScaleFactor());
}

void WebViewImpl::pageScaleFactorChanged()
{
    pageScaleConstraintsSet().setNeedsReset(false);
    updateLayerTreeViewport();
    if (m_inspectorOverlay)
        m_inspectorOverlay->update();
#ifdef MINIBLINK_NOT_IMPLEMENTED
    m_devToolsEmulator->viewportChanged();
#endif // MINIBLINK_NOT_IMPLEMENTED
    m_client->pageScaleFactorChanged();
}

bool WebViewImpl::useExternalPopupMenus()
{
    return shouldUseExternalPopupMenus;
}

void WebViewImpl::startDragging(LocalFrame* frame,
                                const WebDragData& dragData,
                                WebDragOperationsMask mask,
                                const WebImage& dragImage,
                                const WebPoint& dragImageOffset)
{
    if (!m_client)
        return;
    ASSERT(!m_doingDragAndDrop);
    m_doingDragAndDrop = true;
    m_client->startDragging(WebLocalFrameImpl::fromFrame(frame), dragData, mask, dragImage, dragImageOffset);
}

void WebViewImpl::setIgnoreInputEvents(bool newValue)
{
    ASSERT(m_ignoreInputEvents != newValue);
    m_ignoreInputEvents = newValue;
}

void WebViewImpl::setBackgroundColorOverride(WebColor color)
{
    m_backgroundColorOverride = color;
    updateLayerTreeBackgroundColor();
}

void WebViewImpl::setZoomFactorOverride(float zoomFactor)
{
    m_zoomFactorOverride = zoomFactor;
    setZoomLevel(zoomLevel());
}

void WebViewImpl::addPageOverlay(WebPageOverlay* overlay, int zOrder)
{
    if (!m_pageOverlays)
        m_pageOverlays = PageOverlayList::create(this);

    m_pageOverlays->add(overlay, zOrder);
}

void WebViewImpl::removePageOverlay(WebPageOverlay* overlay)
{
    if (m_pageOverlays && m_pageOverlays->remove(overlay) && m_pageOverlays->empty())
        m_pageOverlays = nullptr;
}

void WebViewImpl::setOverlayLayer(GraphicsLayer* layer)
{
    if (!m_rootGraphicsLayer)
        return;

    if (!m_page->mainFrame()->isLocalFrame())
        return;

    m_page->frameHost().pinchViewport().containerLayer()->addChild(layer);
}

Element* WebViewImpl::focusedElement() const
{
    Frame* frame = m_page->focusController().focusedFrame();
    if (!frame || !frame->isLocalFrame())
        return 0;

    Document* document = toLocalFrame(frame)->document();
    if (!document)
        return 0;

    return document->focusedElement();
}

HitTestResult WebViewImpl::hitTestResultForViewportPos(const IntPoint& posInViewport)
{
    IntPoint rootFramePoint(m_page->frameHost().pinchViewport().viewportToRootFrame(posInViewport));
    return hitTestResultForRootFramePos(rootFramePoint);
}

HitTestResult WebViewImpl::hitTestResultForRootFramePos(const IntPoint& posInRootFrame)
{
    if (!m_page->mainFrame()->isLocalFrame())
        return HitTestResult();
    IntPoint docPoint(m_page->deprecatedLocalMainFrame()->view()->rootFrameToContents(posInRootFrame));
    HitTestResult result = m_page->deprecatedLocalMainFrame()->eventHandler().hitTestResultAtPoint(docPoint, HitTestRequest::ReadOnly | HitTestRequest::Active);
    result.setToShadowHostIfInUserAgentShadowRoot();
    return result;
}

WebHitTestResult WebViewImpl::hitTestResultForTap(const WebPoint& tapPointWindowPos, const WebSize& tapArea)
{
    if (!m_page->mainFrame()->isLocalFrame())
        return HitTestResult();

    WebGestureEvent tapEvent;
    tapEvent.x = tapPointWindowPos.x;
    tapEvent.y = tapPointWindowPos.y;
    tapEvent.type = WebInputEvent::GestureTap;
    tapEvent.data.tap.tapCount = 1;
    tapEvent.data.tap.width = tapArea.width;
    tapEvent.data.tap.height = tapArea.height;

    PlatformGestureEventBuilder platformEvent(mainFrameImpl()->frameView(), tapEvent);

    HitTestResult result = m_page->deprecatedLocalMainFrame()->eventHandler().hitTestResultForGestureEvent(platformEvent, HitTestRequest::ReadOnly | HitTestRequest::Active).hitTestResult();

    result.setToShadowHostIfInUserAgentShadowRoot();
    return result;
}

void WebViewImpl::setTabsToLinks(bool enable)
{
    m_tabsToLinks = enable;
}

bool WebViewImpl::tabsToLinks() const
{
    return m_tabsToLinks;
}

void WebViewImpl::suppressInvalidations(bool enable)
{
    if (m_client)
        m_client->suppressCompositorScheduling(enable);
}

void WebViewImpl::setRootGraphicsLayer(GraphicsLayer* layer)
{
    if (!m_layerTreeView)
        return;

    suppressInvalidations(true);

    PinchViewport& pinchViewport = page()->frameHost().pinchViewport();
    pinchViewport.attachToLayerTree(layer, graphicsLayerFactory());
    if (layer) {
        m_rootGraphicsLayer = pinchViewport.rootGraphicsLayer();
        m_rootLayer = pinchViewport.rootGraphicsLayer()->platformLayer();
        m_rootTransformLayer = pinchViewport.rootGraphicsLayer();
        updateRootLayerTransform();
        m_layerTreeView->setRootLayer(*m_rootLayer);
        // We register viewport layers here since there may not be a layer
        // tree view prior to this point.
        page()->frameHost().pinchViewport().registerLayersWithTreeView(m_layerTreeView);
        if (m_pageOverlays)
            m_pageOverlays->update();

        // TODO(enne): Work around page visibility changes not being
        // propogated to the WebView in some circumstances.  This needs to
        // be refreshed here when setting a new root layer to avoid being
        // stuck in a presumed incorrectly invisible state.
        bool visible = page()->visibilityState() == PageVisibilityStateVisible;
        m_layerTreeView->setVisible(visible);
    } else {
        m_rootGraphicsLayer = nullptr;
        m_rootLayer = nullptr;
        m_rootTransformLayer = nullptr;
        // This means that we're transitioning to a new page. Suppress
        // commits until Blink generates invalidations so we don't
        // attempt to paint too early in the next page load.
        m_layerTreeView->setDeferCommits(true);
        m_layerTreeView->clearRootLayer();
        page()->frameHost().pinchViewport().clearLayersForTreeView(m_layerTreeView);
    }

    suppressInvalidations(false);
}

void WebViewImpl::invalidateRect(const IntRect& rect)
{
    if (m_layerTreeView)
        updateLayerTreeViewport();
    else if (m_client)
        m_client->didInvalidateRect(rect);
}

GraphicsLayerFactory* WebViewImpl::graphicsLayerFactory() const
{
    return m_graphicsLayerFactory.get();
}

DeprecatedPaintLayerCompositor* WebViewImpl::compositor() const
{
    if (!page() || !page()->mainFrame() || !page()->mainFrame()->isLocalFrame())
        return 0;

    if (!page()->deprecatedLocalMainFrame()->document() || !page()->deprecatedLocalMainFrame()->document()->layoutView())
        return 0;

    return page()->deprecatedLocalMainFrame()->document()->layoutView()->compositor();
}

void WebViewImpl::registerForAnimations(WebLayer* layer)
{
    if (m_layerTreeView)
        m_layerTreeView->registerForAnimations(layer);
}

GraphicsLayer* WebViewImpl::rootGraphicsLayer()
{
    return m_rootGraphicsLayer;
}

void WebViewImpl::scheduleAnimation()
{
    if (m_layerTreeView) {
        m_layerTreeView->setNeedsAnimate();
        return;
    }
    if (m_client)
        m_client->scheduleAnimation();
}

void WebViewImpl::attachCompositorAnimationTimeline(WebCompositorAnimationTimeline* timeline)
{
    if (m_layerTreeView)
        m_layerTreeView->attachCompositorAnimationTimeline(timeline);
}

void WebViewImpl::detachCompositorAnimationTimeline(WebCompositorAnimationTimeline* timeline)
{
    if (m_layerTreeView)
        m_layerTreeView->detachCompositorAnimationTimeline(timeline);
}

void WebViewImpl::initializeLayerTreeView()
{
    if (m_client) {
        m_client->initializeLayerTreeView();
        m_layerTreeView = m_client->layerTreeView();
    }

    if (WebDevToolsAgentImpl* devTools = mainFrameDevToolsAgentImpl())
        devTools->layerTreeViewChanged(m_layerTreeView);

    m_page->settings().setAcceleratedCompositingEnabled(m_layerTreeView != 0);

    if (!m_layerTreeView) // weolar
        return;

    // FIXME: only unittests, click to play, Android priting, and printing (for headers and footers)
    // make this assert necessary. We should make them not hit this code and then delete allowsBrokenNullLayerTreeView.
    ASSERT(m_layerTreeView || !m_client || m_client->allowsBrokenNullLayerTreeView());
}

void WebViewImpl::applyViewportDeltas(
    const WebFloatSize& pinchViewportDelta,
    const WebFloatSize& layoutViewportDelta,
    const WebFloatSize& elasticOverscrollDelta,
    float pageScaleDelta,
    float topControlsShownRatioDelta)
{
    if (!mainFrameImpl())
        return;
    FrameView* frameView = mainFrameImpl()->frameView();
    if (!frameView)
        return;

    topControls().setShownRatio(topControls().shownRatio() + topControlsShownRatioDelta);

    FloatPoint pinchViewportOffset = page()->frameHost().pinchViewport().visibleRect().location();
    pinchViewportOffset.move(pinchViewportDelta.width, pinchViewportDelta.height);
    setPageScaleFactorAndLocation(pageScaleFactor() * pageScaleDelta, pinchViewportOffset);

    if (pageScaleDelta != 1)
        m_doubleTapZoomPending = false;

    m_elasticOverscroll += elasticOverscrollDelta;
    frameView->didUpdateElasticOverscroll();

    ScrollableArea* layoutViewport = frameView->layoutViewportScrollableArea();

    DoublePoint layoutViewportPosition = layoutViewport->scrollPositionDouble()
        + DoubleSize(layoutViewportDelta.width, layoutViewportDelta.height);

    if (layoutViewport->scrollPositionDouble() != layoutViewportPosition) {
        layoutViewport->setScrollPosition(layoutViewportPosition, CompositorScroll);
        frameView->setWasScrolledByUser(true);
    }
}

void WebViewImpl::recordFrameTimingEvent(FrameTimingEventType eventType, int64_t FrameId, const WebVector<WebFrameTimingEvent>& events)
{
    Frame* frame = m_page ? m_page->mainFrame() : 0;

    while (frame && frame->frameID() != FrameId) {
        frame = frame->tree().traverseNext();
    }

    if (!frame || !frame->domWindow() || !frame->domWindow()->document())
        return; // Can't find frame, it may have been cleaned up from the DOM.

#ifdef MINIBLINK_NOT_IMPLEMENTED
    blink::DOMWindow* domWindow = frame->domWindow();
    blink::Performance* performance = DOMWindowPerformance::performance(*domWindow);
    for (size_t i = 0; i < events.size(); ++i) {
        if (eventType == CompositeEvent)
            performance->addCompositeTiming(domWindow->document(), events[i].sourceFrame, events[i].startTime);
        else if (eventType == RenderEvent)
            performance->addRenderTiming(domWindow->document(), events[i].sourceFrame, events[i].startTime, events[i].finishTime);
    }
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void WebViewImpl::updateLayerTreeViewport()
{
    if (!page() || !m_layerTreeView)
        return;

    m_layerTreeView->setPageScaleFactorAndLimits(pageScaleFactor(), minimumPageScaleFactor(), maximumPageScaleFactor());
}

void WebViewImpl::updateLayerTreeBackgroundColor()
{
    if (!m_layerTreeView)
        return;

    m_layerTreeView->setBackgroundColor(alphaChannel(m_backgroundColorOverride) ? m_backgroundColorOverride : backgroundColor());
}

void WebViewImpl::updateLayerTreeDeviceScaleFactor()
{
    ASSERT(page());
    ASSERT(m_layerTreeView);

    float deviceScaleFactor = m_compositorDeviceScaleFactorOverride ? m_compositorDeviceScaleFactorOverride : page()->deviceScaleFactor();
    m_layerTreeView->setDeviceScaleFactor(deviceScaleFactor);
}

void WebViewImpl::updateRootLayerTransform()
{
    // If we don't have a root graphics layer, we won't bother trying to find
    // or update the transform layer.
    if (!m_rootGraphicsLayer)
        return;

    if (m_rootTransformLayer) {
        TransformationMatrix transform;
        transform.translate(m_rootLayerOffset.width, m_rootLayerOffset.height);
        transform = transform.scale(m_rootLayerScale);
        m_rootTransformLayer->setTransform(transform);
    }
}

bool WebViewImpl::detectContentOnTouch(const GestureEventWithHitTestResults& targetedEvent)
{
    if (!m_page->mainFrame()->isLocalFrame())
        return false;

    // Need a local copy of the hit test as setToShadowHostIfInUserAgentShadowRoot() will modify it.
    HitTestResult touchHit = targetedEvent.hitTestResult();
    touchHit.setToShadowHostIfInUserAgentShadowRoot();

    if (touchHit.isContentEditable())
        return false;

    Node* node = touchHit.innerNode();
    if (!node || !node->isTextNode())
        return false;

    // Ignore when tapping on links or nodes listening to click events, unless the click event is on the
    // body element, in which case it's unlikely that the original node itself was intended to be clickable.
    for (; node && !isHTMLBodyElement(*node); node = LayoutTreeBuilderTraversal::parent(*node)) {
        if (node->isLink() || node->willRespondToTouchEvents() || node->willRespondToMouseClickEvents())
            return false;
    }

    WebContentDetectionResult content = m_client->detectContentAround(touchHit);
    if (!content.isValid())
        return false;

    m_client->scheduleContentIntent(content.intent());
    return true;
}

void WebViewImpl::setVisibilityState(WebPageVisibilityState visibilityState,
                                     bool isInitialState) {
    ASSERT(visibilityState == WebPageVisibilityStateVisible || visibilityState == WebPageVisibilityStateHidden || visibilityState == WebPageVisibilityStatePrerender);

    if (page())
        m_page->setVisibilityState(static_cast<PageVisibilityState>(static_cast<int>(visibilityState)), isInitialState);

    if (m_layerTreeView) {
        bool visible = visibilityState == WebPageVisibilityStateVisible;
        m_layerTreeView->setVisible(visible);
    }
}

bool WebViewImpl::requestPointerLock()
{
    return m_client && m_client->requestPointerLock();
}

void WebViewImpl::requestPointerUnlock()
{
    if (m_client)
        m_client->requestPointerUnlock();
}

bool WebViewImpl::isPointerLocked()
{
    return m_client && m_client->isPointerLocked();
}

void WebViewImpl::pointerLockMouseEvent(const WebInputEvent& event)
{
    OwnPtr<UserGestureIndicator> gestureIndicator;
    AtomicString eventType;
    switch (event.type) {
    case WebInputEvent::MouseDown:
        eventType = EventTypeNames::mousedown;
        gestureIndicator = adoptPtr(new UserGestureIndicator(DefinitelyProcessingNewUserGesture));
        m_pointerLockGestureToken = gestureIndicator->currentToken();
        break;
    case WebInputEvent::MouseUp:
        eventType = EventTypeNames::mouseup;
        gestureIndicator = adoptPtr(new UserGestureIndicator(m_pointerLockGestureToken.release()));
        break;
    case WebInputEvent::MouseMove:
        eventType = EventTypeNames::mousemove;
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    const WebMouseEvent& mouseEvent = static_cast<const WebMouseEvent&>(event);

    if (page())
        page()->pointerLockController().dispatchLockedMouseEvent(
            PlatformMouseEventBuilder(mainFrameImpl()->frameView(), mouseEvent),
            eventType);
}

bool WebViewImpl::shouldDisableDesktopWorkarounds()
{
    if (!settings()->viewportEnabled())
        return false;

    // A document is considered adapted to small screen UAs if one of these holds:
    // 1. The author specified viewport has a constrained width that is equal to
    //    the initial viewport width.
    // 2. The author has disabled viewport zoom.

    const PageScaleConstraints& constraints = pageScaleConstraintsSet().pageDefinedConstraints();

    if (!mainFrameImpl() || !mainFrameImpl()->frameView())
        return false;

    return mainFrameImpl()->frameView()->layoutSize().width() == m_size.width
        || (constraints.minimumScale == constraints.maximumScale && constraints.minimumScale != -1);
}

void WebViewImpl::forceNextWebGLContextCreationToFail()
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    WebGLRenderingContext::forceNextWebGLContextCreationToFail();
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
}

void WebViewImpl::forceNextDrawingBufferCreationToFail()
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    DrawingBuffer::forceNextDrawingBufferCreationToFail();
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
}

void WebViewImpl::setMainFrameTopContentInset(int offset) {
  if (mainFrameImpl())
    mainFrameImpl()->setTopContentInset(offset);
  if (m_layerTreeView)
    m_layerTreeView->SetHudLayerTopInset(offset);
}

} // namespace blink
