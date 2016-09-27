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
 *
 */

#ifndef PingLoader_h
#define PingLoader_h

#include "core/CoreExport.h"
#include "core/fetch/ResourceLoaderOptions.h"
#include "core/page/PageLifecycleObserver.h"
#include "platform/Timer.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebURLLoaderClient.h"
#include "wtf/Noncopyable.h"
#include "wtf/RefPtr.h"

namespace blink {

class FormData;
class LocalFrame;
class KURL;
class ResourceRequest;

// Issue an asynchronous, one-directional request at some resources, ignoring
// any response. The request is made independent of any LocalFrame staying alive,
// and must only stay alive until the transmission has completed successfully
// (or not -- errors are not propagated back either.) Upon transmission, the
// the load is cancelled and the loader cancels itself.
//
// The ping loader is used by audit pings, beacon transmissions and image loads
// during page unloading.
//
class CORE_EXPORT PingLoader : public RefCountedWillBeRefCountedGarbageCollected<PingLoader>, public PageLifecycleObserver, private WebURLLoaderClient {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(PingLoader);
    WTF_MAKE_NONCOPYABLE(PingLoader);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(PingLoader);
public:
    ~PingLoader() override;

    enum ViolationReportType {
        ContentSecurityPolicyViolationReport,
        XSSAuditorViolationReport
    };

    static void loadImage(LocalFrame*, const KURL&);
    static void sendLinkAuditPing(LocalFrame*, const KURL& pingURL, const KURL& destinationURL);
    static void sendViolationReport(LocalFrame*, const KURL& reportURL, PassRefPtr<FormData> report, ViolationReportType);

    DECLARE_VIRTUAL_TRACE();

protected:
    PingLoader(LocalFrame*, ResourceRequest&, const FetchInitiatorInfo&, StoredCredentials);

    static void start(LocalFrame*, ResourceRequest&, const FetchInitiatorInfo&, StoredCredentials = AllowStoredCredentials);

    void dispose();

private:
    void didReceiveResponse(WebURLLoader*, const WebURLResponse&) override;
    void didReceiveData(WebURLLoader*, const char*, int, int) override;
    void didFinishLoading(WebURLLoader*, double, int64_t) override;
    void didFail(WebURLLoader*, const WebURLError&) override;

    void timeout(Timer<PingLoader>*);

    void didFailLoading(Page*);

    OwnPtr<WebURLLoader> m_loader;
    Timer<PingLoader> m_timeout;
    String m_url;
    unsigned long m_identifier;
};

} // namespace blink

#endif // PingLoader_h
