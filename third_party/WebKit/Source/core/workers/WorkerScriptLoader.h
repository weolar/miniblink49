/*
 * Copyright (C) 2009 Apple Inc. All Rights Reserved.
 * Copyright (C) 2009, 2011 Google Inc. All Rights Reserved.
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
 *
 */

#ifndef WorkerScriptLoader_h
#define WorkerScriptLoader_h

#include "core/CoreExport.h"
#include "core/loader/ThreadableLoader.h"
#include "core/loader/ThreadableLoaderClient.h"
#include "platform/network/ResourceRequest.h"
#include "platform/weborigin/KURL.h"
#include "public/platform/WebURLRequest.h"
#include "wtf/FastAllocBase.h"
#include "wtf/Functional.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

class ContentSecurityPolicy;
class ResourceRequest;
class ResourceResponse;
class ExecutionContext;
class TextResourceDecoder;

class CORE_EXPORT WorkerScriptLoader final : public ThreadableLoaderClient {
    WTF_MAKE_FAST_ALLOCATED(WorkerScriptLoader);
public:
    WorkerScriptLoader();
    ~WorkerScriptLoader() override;

    void loadSynchronously(ExecutionContext&, const KURL&, CrossOriginRequestPolicy);
    // TODO: |finishedCallback| is not currently guaranteed to be invoked if
    // used from worker context and the worker shuts down in the middle of an
    // operation. This will cause leaks when we support nested workers.
    // Note that callbacks could be invoked before loadAsynchronously() returns.
    void loadAsynchronously(ExecutionContext&, const KURL&, CrossOriginRequestPolicy, PassOwnPtr<Closure> responseCallback, PassOwnPtr<Closure> finishedCallback);

    // This will immediately invoke |finishedCallback| if loadAsynchronously()
    // is in progress.
    void cancel();

    String script();
    const KURL& url() const { return m_url; }
    const KURL& responseURL() const;
    bool failed() const { return m_failed; }
    unsigned long identifier() const { return m_identifier; }
    long long appCacheID() const { return m_appCacheID; }

    PassOwnPtr<Vector<char>> releaseCachedMetadata() { return m_cachedMetadata.release(); }
    const Vector<char>* cachedMetadata() const { return m_cachedMetadata.get(); }

    PassRefPtr<ContentSecurityPolicy> contentSecurityPolicy() { return m_contentSecurityPolicy; }
    PassRefPtr<ContentSecurityPolicy> releaseContentSecurityPolicy() { return m_contentSecurityPolicy.release(); }

    // ThreadableLoaderClient
    void didReceiveResponse(unsigned long /*identifier*/, const ResourceResponse&, PassOwnPtr<WebDataConsumerHandle>) override;
    void didReceiveData(const char* data, unsigned dataLength) override;
    void didReceiveCachedMetadata(const char*, int /*dataLength*/) override;
    void didFinishLoading(unsigned long identifier, double) override;
    void didFail(const ResourceError&) override;
    void didFailRedirectCheck() override;

    void setRequestContext(WebURLRequest::RequestContext requestContext) { m_requestContext = requestContext; }

private:
    PassOwnPtr<ResourceRequest> createResourceRequest();
    void notifyError();
    void notifyFinished();

    void processContentSecurityPolicy(const ResourceResponse&);

    // Callbacks for loadAsynchronously().
    OwnPtr<Closure> m_responseCallback;
    OwnPtr<Closure> m_finishedCallback;

    RefPtr<ThreadableLoader> m_threadableLoader;
    String m_responseEncoding;
    OwnPtr<TextResourceDecoder> m_decoder;
    StringBuilder m_script;
    KURL m_url;
    KURL m_responseURL;
    bool m_failed;
    bool m_needToCancel;
    unsigned long m_identifier;
    long long m_appCacheID;
    OwnPtr<Vector<char>> m_cachedMetadata;
    WebURLRequest::RequestContext m_requestContext;
    RefPtr<ContentSecurityPolicy> m_contentSecurityPolicy;
};

} // namespace blink

#endif // WorkerScriptLoader_h
