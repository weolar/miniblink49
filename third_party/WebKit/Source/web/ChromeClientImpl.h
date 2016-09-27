/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ChromeClientImpl_h
#define ChromeClientImpl_h

#include "core/page/ChromeClient.h"
#include "core/page/WindowFeatures.h"
#include "public/web/WebNavigationPolicy.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class PagePopup;
class PagePopupClient;
class WebViewImpl;
struct WebCursorInfo;

// Handles window-level notifications from core on behalf of a WebView.
class ChromeClientImpl final : public ChromeClient {
public:
    explicit ChromeClientImpl(WebViewImpl*);
    ~ChromeClientImpl() override;

    void* webView() const override;

    // ChromeClient methods:
    void chromeDestroyed() override;
    void setWindowRect(const IntRect&) override;
    IntRect windowRect() override;
    IntRect pageRect() override;
    void focus() override;
    bool canTakeFocus(WebFocusType) override;
    void takeFocus(WebFocusType) override;
    void focusedNodeChanged(Node* fromNode, Node* toNode) override;
    void focusedFrameChanged(LocalFrame*) override;
    Page* createWindow(
        LocalFrame*, const FrameLoadRequest&, const WindowFeatures&, NavigationPolicy, ShouldSendReferrer) override;
    void show(NavigationPolicy) override;
    void didOverscroll(const FloatSize&, const FloatSize&, const FloatPoint&, const FloatSize&) override;
    void setToolbarsVisible(bool) override;
    bool toolbarsVisible() override;
    void setStatusbarVisible(bool) override;
    bool statusbarVisible() override;
    void setScrollbarsVisible(bool) override;
    bool scrollbarsVisible() override;
    void setMenubarVisible(bool) override;
    bool menubarVisible() override;
    void setResizable(bool) override;
    bool shouldReportDetailedMessageForSource(LocalFrame&, const String&) override;
    void addMessageToConsole(
        LocalFrame*, MessageSource, MessageLevel,
        const String& message, unsigned lineNumber,
        const String& sourceID, const String& stackTrace) override;
    bool canOpenBeforeUnloadConfirmPanel() override;
    bool openBeforeUnloadConfirmPanelDelegate(LocalFrame*, const String&) override;
    void closeWindowSoon() override;
    bool openJavaScriptAlertDelegate(LocalFrame*, const String&) override;
    bool openJavaScriptConfirmDelegate(LocalFrame*, const String&) override;
    bool openJavaScriptPromptDelegate(
        LocalFrame*, const String& message,
        const String& defaultValue, String& result) override;
    void setStatusbarText(const String& message) override;
    bool tabsToLinks() override;
    IntRect windowResizerRect() const override;
    void invalidateRect(const IntRect&) override;
    void scheduleAnimation() override;
    void scheduleAnimationForFrame(LocalFrame* localRoot) override;
    IntRect viewportToScreen(const IntRect&) const override;
    WebScreenInfo screenInfo() const override;
    void contentsSizeChanged(LocalFrame*, const IntSize&) const override;
    void pageScaleFactorChanged() const override;
    float clampPageScaleFactorToLimits(float scale) const override;
    void layoutUpdated(LocalFrame*) const override;
    void showMouseOverURL(const HitTestResult&) override;
    void setToolTip(const String& tooltipText, TextDirection) override;
    void dispatchViewportPropertiesDidChange(const ViewportDescription&) const override;
    void printDelegate(LocalFrame*) override;
    void annotatedRegionsChanged() override;
    PassOwnPtrWillBeRawPtr<ColorChooser> openColorChooser(LocalFrame*, ColorChooserClient*, const Color&) override;
    PassRefPtr<DateTimeChooser> openDateTimeChooser(DateTimeChooserClient*, const DateTimeChooserParameters&) override;
    void openFileChooser(LocalFrame*, PassRefPtr<FileChooser>) override;
    void enumerateChosenDirectory(FileChooser*) override;
    void setCursor(const Cursor&) override;
    Cursor lastSetCursorForTesting() const override;
    void needTouchEvents(bool needTouchEvents) override;
    void setTouchAction(TouchAction) override;

    GraphicsLayerFactory* graphicsLayerFactory() const override;

    // Pass 0 as the GraphicsLayer to detatch the root layer.
    void attachRootGraphicsLayer(GraphicsLayer*, LocalFrame* localRoot) override;

    void attachCompositorAnimationTimeline(WebCompositorAnimationTimeline*, LocalFrame* localRoot) override;
    void detachCompositorAnimationTimeline(WebCompositorAnimationTimeline*, LocalFrame* localRoot) override;

    void enterFullScreenForElement(Element*) override;
    void exitFullScreenForElement(Element*) override;

    void clearCompositedSelection() override;
    void updateCompositedSelection(const CompositedSelection&) override;

    // ChromeClient methods:
    void postAccessibilityNotification(AXObject*, AXObjectCache::AXNotification) override;
    String acceptLanguages() override;

    // ChromeClientImpl:
    void setCursorForPlugin(const WebCursorInfo&);
    void setNewWindowNavigationPolicy(WebNavigationPolicy);

    bool hasOpenedPopup() const override;
    PassRefPtrWillBeRawPtr<PopupMenu> openPopupMenu(LocalFrame&, PopupMenuClient*) override;
    PagePopup* openPagePopup(PagePopupClient*);
    void closePagePopup(PagePopup*);
    DOMWindow* pagePopupWindowForTesting() const override;

    bool shouldOpenModalDialogDuringPageDismissal(const DialogType&, const String& dialogMessage, Document::PageDismissalType) const override;

    bool requestPointerLock() override;
    void requestPointerUnlock() override;

    // AutofillClient pass throughs:
    void didAssociateFormControls(const WillBeHeapVector<RefPtrWillBeMember<Element>>&, LocalFrame*) override;
    void handleKeyboardEventOnTextField(HTMLInputElement&, KeyboardEvent&) override;
    void didChangeValueInTextField(HTMLFormControlElement&) override;
    void didEndEditingOnTextField(HTMLInputElement&) override;
    void openTextDataListChooser(HTMLInputElement&) override;
    void textFieldDataListChanged(HTMLInputElement&) override;
    void xhrSucceeded(LocalFrame*) override;
    void ajaxSucceeded(LocalFrame*) override;

    void didCancelCompositionOnSelectionChange() override;
    void willSetInputMethodState() override;
    void didUpdateTextOfFocusedElementByNonUserInput() override;
    void showImeIfNeeded() override;

    void registerViewportLayers() const override;

    void showUnhandledTapUIIfNeeded(IntPoint, Node*, bool) override;
    void onMouseDown(Node*) override;
    void didUpdateTopControls() const override;

    FloatSize elasticOverscroll() const override;

private:
    bool isChromeClientImpl() const override { return true; }
    void registerPopupOpeningObserver(PopupOpeningObserver*) override;
    void unregisterPopupOpeningObserver(PopupOpeningObserver*) override;

    void notifyPopupOpeningObservers() const;
    void setCursor(const WebCursorInfo&);

    WebViewImpl* m_webView; // Weak pointer.
    WindowFeatures m_windowFeatures;
    Vector<PopupOpeningObserver*> m_popupOpeningObservers;
    Cursor m_lastSetMouseCursorForTesting;
};

DEFINE_TYPE_CASTS(ChromeClientImpl, ChromeClient, client, client->isChromeClientImpl(), client.isChromeClientImpl());

} // namespace blink

#endif
