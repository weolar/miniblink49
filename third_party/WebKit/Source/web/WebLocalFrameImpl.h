/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef WebLocalFrameImpl_h
#define WebLocalFrameImpl_h

#include "core/frame/LocalFrame.h"
#include "platform/geometry/FloatRect.h"
#include "public/platform/WebFileSystemType.h"
#include "public/web/WebLocalFrame.h"
#include "web/FrameLoaderClientImpl.h"
#include "web/UserMediaClientImpl.h"
#include "wtf/Compiler.h"
#include "wtf/OwnPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ChromePrintContext;
class GeolocationClientProxy;
class InspectorOverlay;
class IntSize;
class KURL;
class Range;
class SharedWorkerRepositoryClientImpl;
class TextFinder;
class WebAutofillClient;
class WebDataSourceImpl;
class WebDevToolsAgentImpl;
class WebDevToolsFrontendImpl;
class WebFrameClient;
class WebFrameWidgetImpl;
class WebNode;
class WebPerformance;
class WebPlugin;
class WebPluginContainerImpl;
class WebScriptExecutionCallback;
class WebSuspendableTask;
class WebView;
class WebViewImpl;
struct FrameLoadRequest;
struct WebPrintParams;

template <typename T> class WebVector;

// Implementation of WebFrame, note that this is a reference counted object.
class WebLocalFrameImpl final : public RefCountedWillBeGarbageCollectedFinalized<WebLocalFrameImpl>, public WebLocalFrame {
public:
    // WebFrame methods:
    bool isWebLocalFrame() const override;
    WebLocalFrame* toWebLocalFrame() override;
    bool isWebRemoteFrame() const override;
    WebRemoteFrame* toWebRemoteFrame() override;
    void close() override;
    WebString uniqueName() const override;
    WebString assignedName() const override;
    void setName(const WebString&) override;
    WebVector<WebIconURL> iconURLs(int iconTypesMask) const override;
    void setRemoteWebLayer(WebLayer*) override;
    void setContentSettingsClient(WebContentSettingsClient*) override;
    void setSharedWorkerRepositoryClient(WebSharedWorkerRepositoryClient*) override;
    WebSize scrollOffset() const override;
    void setScrollOffset(const WebSize&) override;
    WebSize minimumScrollOffset() const override;
    WebSize maximumScrollOffset() const override;
    WebSize contentsSize() const override;
    bool hasVisibleContent() const override;
    WebRect visibleContentRect() const override;
    bool hasHorizontalScrollbar() const override;
    bool hasVerticalScrollbar() const override;
    WebView* view() const override;
    void setOpener(WebFrame*) override;
    WebDocument document() const override;
#ifdef MINIBLINK_NOT_IMPLEMENTED
    WebPerformance performance() const override;
#endif // MINIBLINK_NOT_IMPLEMENTED
    bool dispatchBeforeUnloadEvent() override;
    void dispatchUnloadEvent() override;
    NPObject* windowObject() const override;
    void bindToWindowObject(const WebString& name, NPObject*) override;
    void bindToWindowObject(const WebString& name, NPObject*, void*) override;
    void executeScript(const WebScriptSource&) override;
    void executeScriptInIsolatedWorld(
        int worldID, const WebScriptSource* sources, unsigned numSources,
        int extensionGroup) override;
    void setIsolatedWorldSecurityOrigin(int worldID, const WebSecurityOrigin&) override;
    void setIsolatedWorldContentSecurityPolicy(int worldID, const WebString&) override;
    void setIsolatedWorldHumanReadableName(int worldID, const WebString&) override;
    void addMessageToConsole(const WebConsoleMessage&) override;
    void collectGarbage() override;
    bool checkIfRunInsecureContent(const WebURL&) const override;
    v8::Local<v8::Value> executeScriptAndReturnValue(
        const WebScriptSource&) override;
    void requestExecuteScriptAndReturnValue(
        const WebScriptSource&, bool userGesture, WebScriptExecutionCallback*) override;
    void executeScriptInIsolatedWorld(
        int worldID, const WebScriptSource* sourcesIn, unsigned numSources,
        int extensionGroup, WebVector<v8::Local<v8::Value>>* results) override;
    void requestExecuteScriptInIsolatedWorld(
        int worldID, const WebScriptSource* sourceIn, unsigned numSources,
        int extensionGroup, bool userGesture, WebScriptExecutionCallback*) override;
    v8::Local<v8::Value> callFunctionEvenIfScriptDisabled(
        v8::Local<v8::Function>,
        v8::Local<v8::Value>,
        int argc,
        v8::Local<v8::Value> argv[]) override;
    v8::Local<v8::Context> mainWorldScriptContext() const override;
    void reload(bool ignoreCache) override;
    void reloadWithOverrideURL(const WebURL& overrideUrl, bool ignoreCache) override;
    void reloadImage(const WebNode&) override;
    void loadRequest(const WebURLRequest&) override;
    void loadHistoryItem(const WebHistoryItem&, WebHistoryLoadType, WebURLRequest::CachePolicy) override;
    void loadData(
        const WebData&, const WebString& mimeType, const WebString& textEncoding,
        const WebURL& baseURL, const WebURL& unreachableURL, bool replace) override;
    void loadHTMLString(
        const WebData& html, const WebURL& baseURL, const WebURL& unreachableURL,
        bool replace) override;
    void stopLoading() override;
    WebDataSource* provisionalDataSource() const override;
    WebDataSource* dataSource() const override;
    void enableViewSourceMode(bool enable) override;
    bool isViewSourceModeEnabled() const override;
    void setReferrerForRequest(WebURLRequest&, const WebURL& referrer) override;
    void dispatchWillSendRequest(WebURLRequest&) override;
    WebURLLoader* createAssociatedURLLoader(const WebURLLoaderOptions&) override;
    unsigned unloadListenerCount() const override;
    void replaceSelection(const WebString&) override;
    void insertText(const WebString&) override;
    void setMarkedText(const WebString&, unsigned location, unsigned length) override;
    void unmarkText() override;
    bool hasMarkedText() const override;
    WebRange markedRange() const override;
    bool firstRectForCharacterRange(unsigned location, unsigned length, WebRect&) const override;
    size_t characterIndexForPoint(const WebPoint&) const override;
    bool executeCommand(const WebString&, const WebNode& = WebNode()) override;
    bool executeCommand(const WebString&, const WebString& value, const WebNode& = WebNode()) override;
    bool isCommandEnabled(const WebString&) const override;
    void enableContinuousSpellChecking(bool) override;
    bool isContinuousSpellCheckingEnabled() const override;
    void requestTextChecking(const WebElement&) override;
    void replaceMisspelledRange(const WebString&) override;
    void removeSpellingMarkers() override;
    bool hasSelection() const override;
    WebRange selectionRange() const override;
    WebString selectionAsText() const override;
    WebString selectionAsMarkup() const override;
    bool selectWordAroundCaret() override;
    void selectRange(const WebPoint& base, const WebPoint& extent) override;
    void selectRange(const WebRange&) override;
    void moveRangeSelectionExtent(const WebPoint&) override;
    void moveRangeSelection(const WebPoint& base, const WebPoint& extent, WebFrame::TextGranularity = CharacterGranularity) override;
    void moveCaretSelection(const WebPoint&) override;
    bool setEditableSelectionOffsets(int start, int end) override;
    bool setCompositionFromExistingText(int compositionStart, int compositionEnd, const WebVector<WebCompositionUnderline>& underlines) override;
    void extendSelectionAndDelete(int before, int after) override;
    void setCaretVisible(bool) override;
    int printBegin(const WebPrintParams&, const WebNode& constrainToNode) override;
    float printPage(int pageToPrint, WebCanvas*) override;
    float getPrintPageShrink(int page) override;
    void printEnd() override;
    bool isPrintScalingDisabledForPlugin(const WebNode&) override;
    bool getPrintPresetOptionsForPlugin(const WebNode&, WebPrintPresetOptions*) override;
    bool hasCustomPageSizeStyle(int pageIndex) override;
    bool isPageBoxVisible(int pageIndex) override;
    void pageSizeAndMarginsInPixels(
        int pageIndex,
        WebSize& pageSize,
        int& marginTop,
        int& marginRight,
        int& marginBottom,
        int& marginLeft) override;
    WebString pageProperty(const WebString& propertyName, int pageIndex) override;
    void printPagesWithBoundaries(WebCanvas*, const WebSize&) override;
    bool find(
        int identifier, const WebString& searchText, const WebFindOptions&,
        bool wrapWithinFrame, WebRect* selectionRect) override;
    void stopFinding(bool clearSelection) override;
    void scopeStringMatches(
        int identifier, const WebString& searchText, const WebFindOptions&,
        bool reset) override;
    void cancelPendingScopingEffort() override;
    void increaseMatchCount(int count, int identifier) override;
    void resetMatchCount() override;
    int findMatchMarkersVersion() const override;
    WebFloatRect activeFindMatchRect() override;
    void findMatchRects(WebVector<WebFloatRect>&) override;
    int selectNearestFindMatch(const WebFloatPoint&, WebRect* selectionRect) override;
    void setTickmarks(const WebVector<WebRect>&) override;

    void dispatchMessageEventWithOriginCheck(
        const WebSecurityOrigin& intendedTargetOrigin,
        const WebDOMEvent&) override;

    WebString contentAsText(size_t maxChars) const override;
    WebString contentAsMarkup() const override;
    WebString layoutTreeAsText(LayoutAsTextControls toShow = LayoutAsTextNormal) const override;

    void drawInCanvas(const WebRect& rect, const WebString& customCSS, WebCanvas* canvas) const override;

    WebString markerTextForListItem(const WebElement&) const override;
    WebRect selectionBoundsRect() const override;

    bool selectionStartHasSpellingMarkerFor(int from, int length) const override;
    WebString layerTreeAsText(bool showDebugInfo = false) const override;

    void registerTestInterface(const WebString& name, WebTestInterfaceFactory*) override;

    // Creates a test interface by name if available, returns an empty handle
    // for unknown names.
    v8::Local<v8::Value> createTestInterface(const AtomicString& name);

    // WebLocalFrame methods:
    void initializeToReplaceRemoteFrame(WebRemoteFrame*, const WebString& name, WebSandboxFlags) override;
    void setAutofillClient(WebAutofillClient*) override;
    WebAutofillClient* autofillClient() override;
    void setDevToolsAgentClient(WebDevToolsAgentClient*) override;
    WebDevToolsAgent* devToolsAgent() override;
    void sendPings(const WebNode& contextNode, const WebURL& destinationURL) override;
    WebURLRequest requestFromHistoryItem(const WebHistoryItem&, WebURLRequest::CachePolicy)
        const override;
    WebURLRequest requestForReload(WebFrameLoadType, const WebURL&) const override;
    void load(const WebURLRequest&, WebFrameLoadType, const WebHistoryItem&,
        WebHistoryLoadType) override;
    bool isLoading() const override;
    bool isResourceLoadInProgress() const override;
    void setCommittedFirstRealLoad() override;
    void sendOrientationChangeEvent() override;
    void willShowInstallBannerPrompt(int requestId, const WebVector<WebString>& platforms, WebAppBannerPromptReply*) override;
    WebSandboxFlags effectiveSandboxFlags() const override;
    void requestRunTask(WebSuspendableTask*) const override;

    void willBeDetached();
    void willDetachParent();

    static WebLocalFrameImpl* create(WebTreeScopeType, WebFrameClient*);
    ~WebLocalFrameImpl() override;

    PassRefPtrWillBeRawPtr<LocalFrame> initializeCoreFrame(FrameHost*, FrameOwner*, const AtomicString& name, const AtomicString& fallbackName);

    PassRefPtrWillBeRawPtr<LocalFrame> createChildFrame(const FrameLoadRequest&, const AtomicString& name, HTMLFrameOwnerElement*);

    void didChangeContentsSize(const IntSize&);

    void createFrameView();

    static WebLocalFrameImpl* fromFrame(LocalFrame*);
    static WebLocalFrameImpl* fromFrame(LocalFrame&);
    static WebLocalFrameImpl* fromFrameOwnerElement(Element*);

    // If the frame hosts a PluginDocument, this method returns the WebPluginContainerImpl
    // that hosts the plugin.
    static WebPluginContainerImpl* pluginContainerFromFrame(LocalFrame*);

    // If the frame hosts a PluginDocument, this method returns the WebPluginContainerImpl
    // that hosts the plugin. If the provided node is a plugin, then it runs its
    // WebPluginContainerImpl.
    static WebPluginContainerImpl* pluginContainerFromNode(LocalFrame*, const WebNode&);

    WebViewImpl* viewImpl() const;

    FrameView* frameView() const { return frame() ? frame()->view() : 0; }

    InspectorOverlay* inspectorOverlay();
    WebDevToolsAgentImpl* devToolsAgentImpl() const { return m_devToolsAgent.get(); }

    // Getters for the impls corresponding to Get(Provisional)DataSource. They
    // may return 0 if there is no corresponding data source.
    WebDataSourceImpl* dataSourceImpl() const;
    WebDataSourceImpl* provisionalDataSourceImpl() const;

    // Returns which frame has an active match. This function should only be
    // called on the main frame, as it is the only frame keeping track. Returned
    // value can be 0 if no frame has an active match.
    WebLocalFrameImpl* activeMatchFrame() const;

    // Returns the active match in the current frame. Could be a null range if
    // the local frame has no active match.
    Range* activeMatch() const;

    // When a Find operation ends, we want to set the selection to what was active
    // and set focus to the first focusable node we find (starting with the first
    // node in the matched range and going up the inheritance chain). If we find
    // nothing to focus we focus the first focusable node in the range. This
    // allows us to set focus to a link (when we find text inside a link), which
    // allows us to navigate by pressing Enter after closing the Find box.
    void setFindEndstateFocusAndSelection();

    void didFail(const ResourceError&, bool wasProvisional, HistoryCommitType);

    // Sets whether the WebLocalFrameImpl allows its document to be scrolled.
    // If the parameter is true, allow the document to be scrolled.
    // Otherwise, disallow scrolling.
    void setCanHaveScrollbars(bool) override;

    LocalFrame* frame() const { return m_frame.get(); }
    WebFrameClient* client() const { return m_client; }
    void setClient(WebFrameClient* client) { m_client = client; }

    WebContentSettingsClient* contentSettingsClient() { return m_contentSettingsClient; }
#ifdef MINIBLINK_NOT_IMPLEMENTED
    SharedWorkerRepositoryClientImpl* sharedWorkerRepositoryClient() const { return m_sharedWorkerRepositoryClient.get(); }
#else
    SharedWorkerRepositoryClientImpl* sharedWorkerRepositoryClient() const { return nullptr; }
#endif // MINIBLINK_NOT_IMPLEMENTED
    void setInputEventsTransformForEmulation(const IntSize&, float);

    static void selectWordAroundPosition(LocalFrame*, VisiblePosition);

    // Returns the text finder object if it already exists.
    // Otherwise creates it and then returns.
#ifdef MINIBLINK_NOT_IMPLEMENTED
    TextFinder& ensureTextFinder();
#endif // MINIBLINK_NOT_IMPLEMENTED

    // Returns a hit-tested VisiblePosition for the given point
    VisiblePosition visiblePositionForViewportPoint(const WebPoint&);

    void setFrameWidget(WebFrameWidgetImpl*);
    WebFrameWidgetImpl* frameWidget() const;

    // DevTools front-end bindings.
    void setDevToolsFrontend(WebDevToolsFrontendImpl* frontend) { m_webDevToolsFrontend = frontend; }
    WebDevToolsFrontendImpl* devToolsFrontend() { return m_webDevToolsFrontend; }

#if ENABLE(OILPAN)
    DECLARE_TRACE();
#endif

private:
    friend class FrameLoaderClientImpl;

    WebLocalFrameImpl(WebTreeScopeType, WebFrameClient*);

    // Sets the local core frame and registers destruction observers.
    void setCoreFrame(PassRefPtrWillBeRawPtr<LocalFrame>);

    void loadJavaScriptURL(const KURL&);

    WebPlugin* focusedPluginIfInputMethodSupported();
    ScrollableArea* layoutViewportScrollableArea() const;

    FrameLoaderClientImpl m_frameLoaderClientImpl;

    // The embedder retains a reference to the WebCore LocalFrame while it is active in the DOM. This
    // reference is released when the frame is removed from the DOM or the entire page is closed.
    // FIXME: These will need to change to WebFrame when we introduce WebFrameProxy.
    RefPtrWillBeMember<LocalFrame> m_frame;

    OwnPtrWillBeMember<InspectorOverlay> m_inspectorOverlay;
    OwnPtrWillBeMember<WebDevToolsAgentImpl> m_devToolsAgent;

    // This is set if the frame is the root of a local frame tree, and requires a widget for layout.
    WebFrameWidgetImpl* m_frameWidget;

    WebFrameClient* m_client;
    WebAutofillClient* m_autofillClient;
    WebContentSettingsClient* m_contentSettingsClient;
    
#ifdef MINIBLINK_NOT_IMPLEMENTED
    OwnPtr<SharedWorkerRepositoryClientImpl> m_sharedWorkerRepositoryClient;
    // Will be initialized after first call to find() or scopeStringMatches().
    OwnPtrWillBeMember<TextFinder> m_textFinder;
#endif // MINIBLINK_NOT_IMPLEMENTED
    // Valid between calls to BeginPrint() and EndPrint(). Containts the print
    // information. Is used by PrintPage().
    OwnPtrWillBeMember<ChromePrintContext> m_printContext;

    // Stores the additional input events offset and scale when device metrics emulation is enabled.
    IntSize m_inputEventsOffsetForEmulation;
    float m_inputEventsScaleFactorForEmulation;

#ifdef MINIBLINK_NOT_IMPLEMENTED
    UserMediaClientImpl m_userMediaClientImpl;
    OwnPtrWillBeMember<GeolocationClientProxy> m_geolocationClientProxy;
#endif // MINIBLINK_NOT_IMPLEMENTED

    WebDevToolsFrontendImpl* m_webDevToolsFrontend;

    HashMap<AtomicString, OwnPtr<WebTestInterfaceFactory>> m_testInterfaces;

#if ENABLE(OILPAN)
    // Oilpan: to provide the guarantee of having the frame live until
    // close() is called, an instance keep a self-persistent. It is
    // cleared upon calling close(). This avoids having to assume that
    // an embedder's WebFrame references are all discovered via thread
    // state (stack, registers) should an Oilpan GC strike while we're
    // in the process of detaching.
    GC_PLUGIN_IGNORE("340522")
    Persistent<WebLocalFrameImpl> m_selfKeepAlive;
#endif

public:
    int topContentInset() const override;
    void setTopContentInset(int offset);
    int m_topContentInset = 0;
};

DEFINE_TYPE_CASTS(WebLocalFrameImpl, WebFrame, frame, frame->isWebLocalFrame(), frame.isWebLocalFrame());

} // namespace blink

#endif
