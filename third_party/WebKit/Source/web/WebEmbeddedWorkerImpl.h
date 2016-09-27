/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef WebEmbeddedWorkerImpl_h
#define WebEmbeddedWorkerImpl_h

#include "core/workers/WorkerLoaderProxy.h"

#include "public/web/WebContentSecurityPolicy.h"
#include "public/web/WebDevToolsAgentClient.h"
#include "public/web/WebEmbeddedWorker.h"
#include "public/web/WebEmbeddedWorkerStartData.h"
#include "public/web/WebFrameClient.h"

namespace blink {

class ServiceWorkerGlobalScopeProxy;
class WebLocalFrameImpl;
class WebServiceWorkerNetworkProvider;
class WebView;
class WorkerInspectorProxy;
class WorkerScriptLoader;
class WorkerThread;

class WebEmbeddedWorkerImpl final
    : public WebEmbeddedWorker
    , public WebFrameClient
    , public WebDevToolsAgentClient
    , private WorkerLoaderProxyProvider {
    WTF_MAKE_NONCOPYABLE(WebEmbeddedWorkerImpl);
public:
    WebEmbeddedWorkerImpl(PassOwnPtr<WebServiceWorkerContextClient>, PassOwnPtr<WebWorkerContentSettingsClientProxy>);
    ~WebEmbeddedWorkerImpl() override;

    // Terminate all WebEmbeddedWorkerImpl for testing purposes.
    // Note that this only schedules termination and
    // does not synchronously wait for it to complete.
    static void terminateAll();

    // WebEmbeddedWorker overrides.
    void startWorkerContext(const WebEmbeddedWorkerStartData&) override;
    void resumeAfterDownload() override;
    void terminateWorkerContext() override;
    void attachDevTools(const WebString& hostId) override;
    void reattachDevTools(const WebString& hostId, const WebString& savedState) override;
    void detachDevTools() override;
    void dispatchDevToolsMessage(const WebString&) override;

    void postMessageToPageInspector(const WTF::String&);

private:
    void prepareShadowPageForLoader();
    void loadShadowPage();

    // WebFrameClient overrides.
    void willSendRequest(
        WebLocalFrame*, unsigned identifier, WebURLRequest&,
        const WebURLResponse& redirectResponse) override;
    void didFinishDocumentLoad(WebLocalFrame*) override;

    // WebDevToolsAgentClient overrides.
    void sendProtocolMessage(int callId, const WebString&, const WebString&) override;
    void resumeStartup() override;

    void onScriptLoaderFinished();
    void startWorkerThread();

    // WorkerLoaderProxyProvider
    void postTaskToLoader(PassOwnPtr<ExecutionContextTask>) override;
    bool postTaskToWorkerGlobalScope(PassOwnPtr<ExecutionContextTask>) override;

    WebEmbeddedWorkerStartData m_workerStartData;

    OwnPtr<WebServiceWorkerContextClient> m_workerContextClient;

    // This is kept until startWorkerContext is called, and then passed on
    // to WorkerContext.
    OwnPtr<WebWorkerContentSettingsClientProxy> m_contentSettingsClient;

    // We retain ownership of this one which is for use on the
    // main thread only.
    OwnPtr<WebServiceWorkerNetworkProvider> m_networkProvider;

    // Kept around only while main script loading is ongoing.
    OwnPtr<WorkerScriptLoader> m_mainScriptLoader;

    RefPtr<WorkerThread> m_workerThread;
    RefPtr<WorkerLoaderProxy> m_loaderProxy;
    OwnPtr<ServiceWorkerGlobalScopeProxy> m_workerGlobalScopeProxy;
    OwnPtr<WorkerInspectorProxy> m_workerInspectorProxy;

    // 'shadow page' - created to proxy loading requests from the worker.
    // Both WebView and WebFrame objects are close()'ed (where they're
    // deref'ed) when this EmbeddedWorkerImpl is destructed, therefore they
    // are guaranteed to exist while this object is around.
    WebView* m_webView;
    WebLocalFrameImpl* m_mainFrame;

    bool m_loadingShadowPage;
    bool m_askedToTerminate;

    enum WaitingForDebuggerState {
        WaitingForDebuggerBeforeLoadingScript,
        WaitingForDebuggerAfterScriptLoaded,
        NotWaitingForDebugger
    };

    enum {
        DontPauseAfterDownload,
        DoPauseAfterDownload,
        IsPausedAfterDownload
    } m_pauseAfterDownloadState;

    WaitingForDebuggerState m_waitingForDebuggerState;
};

} // namespace blink

#endif // WebEmbeddedWorkerImpl_h
