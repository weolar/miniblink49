/*
 * Copyright (C) 2009, 2012 Google Inc. All rights reserved.
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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

#ifndef FrameLoaderClientImpl_h
#define FrameLoaderClientImpl_h

#include "core/loader/FrameLoaderClient.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class WebLocalFrameImpl;
class WebPluginLoadObserver;

class FrameLoaderClientImpl final : public FrameLoaderClient {
public:
    explicit FrameLoaderClientImpl(WebLocalFrameImpl* webFrame);
    ~FrameLoaderClientImpl() override;

    WebLocalFrameImpl* webFrame() const { return m_webFrame; }

    // FrameLoaderClient ----------------------------------------------

    void didCreateNewDocument() override;
    // Notifies the WebView delegate that the JS window object has been cleared,
    // giving it a chance to bind native objects to the window before script
    // parsing begins.
    void dispatchDidClearWindowObjectInMainWorld() override;
    void documentElementAvailable() override;

    void didCreateScriptContext(v8::Local<v8::Context>, int extensionGroup, int worldId) override;
    void willReleaseScriptContext(v8::Local<v8::Context>, int worldId) override;

    // Returns true if we should allow the given V8 extension to be added to
    // the script context at the currently loading page and given extension group.
    bool allowScriptExtension(const String& extensionName, int extensionGroup, int worldId) override;

    bool hasWebView() const override;
    bool inShadowTree() const override;
    Frame* opener() const override;
    void setOpener(Frame*) override;
    Frame* parent() const override;
    Frame* top() const override;
    Frame* previousSibling() const override;
    Frame* nextSibling() const override;
    Frame* firstChild() const override;
    Frame* lastChild() const override;
    void willBeDetached() override;
    void detached(FrameDetachType) override;
    void dispatchWillSendRequest(DocumentLoader*, unsigned long identifier, ResourceRequest&, const ResourceResponse& redirectResponse) override;
    void dispatchDidReceiveResponse(DocumentLoader*, unsigned long identifier, const ResourceResponse&) override;
    void dispatchDidChangeResourcePriority(unsigned long identifier, ResourceLoadPriority, int intraPriorityValue) override;
    void dispatchDidFinishLoading(DocumentLoader*, unsigned long identifier) override;
    void dispatchDidLoadResourceFromMemoryCache(const ResourceRequest&, const ResourceResponse&) override;
    void dispatchDidHandleOnloadEvents() override;
    void dispatchDidReceiveServerRedirectForProvisionalLoad() override;
    void dispatchDidNavigateWithinPage(HistoryItem*, HistoryCommitType) override;
    void dispatchWillClose() override;
    void dispatchDidStartProvisionalLoad(double triggeringEventTime) override;
    void dispatchDidReceiveTitle(const String&) override;
    void dispatchDidChangeIcons(IconType) override;
    void dispatchDidCommitLoad(HistoryItem*, HistoryCommitType) override;
    void dispatchDidFailProvisionalLoad(const ResourceError&, HistoryCommitType) override;
    void dispatchDidFailLoad(const ResourceError&, HistoryCommitType) override;
    void dispatchDidFinishDocumentLoad() override;
    void dispatchDidFinishLoad() override;
    void dispatchDidFirstVisuallyNonEmptyLayout() override;

    void dispatchDidChangeThemeColor() override;
    NavigationPolicy decidePolicyForNavigation(const ResourceRequest&, DocumentLoader*, NavigationPolicy) override;
    void dispatchWillRequestResource(FetchRequest*) override;
    void dispatchWillSendSubmitEvent(HTMLFormElement*) override;
    void dispatchWillSubmitForm(HTMLFormElement*) override;
    void didStartLoading(LoadStartType) override;
    void didStopLoading() override;
    void progressEstimateChanged(double progressEstimate) override;
    void loadURLExternally(const ResourceRequest&, NavigationPolicy, const String& suggestedName = String()) override;
    bool navigateBackForward(int offset) const override;
    void didAccessInitialDocument() override;
    void didDisplayInsecureContent() override;
    void didRunInsecureContent(SecurityOrigin*, const KURL& insecureURL) override;
    void didDetectXSS(const KURL&, bool didBlockEntirePage) override;
    void didDispatchPingLoader(const KURL&) override;
    void selectorMatchChanged(const Vector<String>& addedSelectors, const Vector<String>& removedSelectors) override;
    PassRefPtrWillBeRawPtr<DocumentLoader> createDocumentLoader(LocalFrame*, const ResourceRequest&, const SubstituteData&) override;
    WTF::String userAgent(const KURL&) override;
    WTF::String doNotTrackValue() override;
    void transitionToCommittedForNewPage() override;
    PassRefPtrWillBeRawPtr<LocalFrame> createFrame(const FrameLoadRequest&, const WTF::AtomicString& name, HTMLFrameOwnerElement*) override;
    virtual bool canCreatePluginWithoutRenderer(const String& mimeType) const;
    PassOwnPtrWillBeRawPtr<PluginPlaceholder> createPluginPlaceholder(
        Document&, const KURL&,
        const Vector<String>& paramNames, const Vector<String>& paramValues,
        const String& mimeType, bool loadManually) override;
    PassRefPtrWillBeRawPtr<Widget> createPlugin(
        HTMLPlugInElement*, const KURL&,
        const Vector<WTF::String>&, const Vector<WTF::String>&,
        const WTF::String&, bool loadManually, DetachedPluginPolicy) override;
    PassRefPtrWillBeRawPtr<Widget> createJavaAppletWidget(
        HTMLAppletElement*,
        const KURL& /* base_url */,
        const Vector<WTF::String>& paramNames,
        const Vector<WTF::String>& paramValues) override;
    ObjectContentType objectContentType(
        const KURL&, const WTF::String& mimeType, bool shouldPreferPlugInsForImages) override;
    void didChangeScrollOffset() override;
    void didUpdateCurrentHistoryItem() override;
    void didRemoveAllPendingStylesheet() override;
    bool allowScript(bool enabledPerSettings) override;
    bool allowScriptFromSource(bool enabledPerSettings, const KURL& scriptURL) override;
    bool allowPlugins(bool enabledPerSettings) override;
    bool allowImage(bool enabledPerSettings, const KURL& imageURL) override;
    bool allowMedia(const KURL& mediaURL) override;
    bool allowDisplayingInsecureContent(bool enabledPerSettings, SecurityOrigin*, const KURL&) override;
    bool allowRunningInsecureContent(bool enabledPerSettings, SecurityOrigin*, const KURL&) override;
    void didNotAllowScript() override;
    void didNotAllowPlugins() override;

    WebCookieJar* cookieJar() const override;
    bool willCheckAndDispatchMessageEvent(SecurityOrigin* target, MessageEvent*, LocalFrame* sourceFrame) const override;
    void didChangeName(const String&) override;
    void didChangeSandboxFlags(Frame* childFrame, SandboxFlags) override;

    void dispatchWillOpenWebSocket(WebSocketHandle*) override;

    void dispatchWillStartUsingPeerConnectionHandler(WebRTCPeerConnectionHandler*) override;

    void didRequestAutocomplete(HTMLFormElement*) override;

    bool allowWebGL(bool enabledPerSettings) override;
    void didLoseWebGLContext(int arbRobustnessContextLostReason) override;

    void dispatchWillInsertBody() override;

    v8::Local<v8::Value> createTestInterface(const AtomicString& name) override;

    PassOwnPtr<WebServiceWorkerProvider> createServiceWorkerProvider() override;
    bool isControlledByServiceWorker(DocumentLoader&) override;
    int64_t serviceWorkerID(DocumentLoader&) override;
    SharedWorkerRepositoryClient* sharedWorkerRepositoryClient() override;

    PassOwnPtr<WebApplicationCacheHost> createApplicationCacheHost(WebApplicationCacheHostClient*) override;

    void didStopAllLoaders() override;

    void dispatchDidChangeManifest() override;

    void dispatchDidChangeDefaultPresentation() override;

    unsigned backForwardLength() override;

    void suddenTerminationDisablerChanged(bool present, SuddenTerminationDisablerType) override;
private:
    bool isFrameLoaderClientImpl() const override { return true; }

    PassOwnPtr<WebPluginLoadObserver> pluginLoadObserver(DocumentLoader*);

    // The WebFrame that owns this object and manages its lifetime. Therefore,
    // the web frame object is guaranteed to exist.
    WebLocalFrameImpl* m_webFrame;
};

DEFINE_TYPE_CASTS(FrameLoaderClientImpl, FrameLoaderClient, client, client->isFrameLoaderClientImpl(), client.isFrameLoaderClientImpl());

} // namespace blink

#endif
