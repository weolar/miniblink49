/*
 * Copyright (C) 2004, 2006 Apple Computer, Inc.  All rights reserved.
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
 */

#ifndef WebURLLoaderInternal_h
#define WebURLLoaderInternal_h

#include "content/web_impl_win/WebURLLoaderImplCurl.h"

#include "net/MultipartHandle.h"
#include "third_party/WebKit/public/platform/WebURLLoader.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"
#include "third_party/WebKit/public/platform/WebURLError.h"
#include "third_party/WebKit/Source/platform/Timer.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/wtf/Noncopyable.h"
#include "third_party/WebKit/Source/wtf/FastAllocBase.h"
#include "third_party/WebKit/public/platform/WebURLLoaderClient.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"

#include <windows.h>
#include <memory>

#define CURL_STATICLIB 
#include "third_party/libcurl/include/curl/curl.h"

// The allocations and releases in WebURLLoaderInternal are
// Cocoa-exception-free (either simple Foundation classes or
// WebCoreResourceLoaderImp which avoids doing work in dealloc).

namespace blink {
class WebURLLoaderClient;
}

namespace content {
class WebURLLoaderImplCurl;
}

using namespace blink;
using namespace content;

namespace net {
    
class WebURLLoaderInternal {
    //WTF_MAKE_NONCOPYABLE(WebURLLoaderInternal); WTF_MAKE_FAST_ALLOCATED;
public:
	WebURLLoaderInternal(WebURLLoaderImplCurl* loader, const WebURLRequest& request, WebURLLoaderClient* client, bool defersLoading, bool shouldContentSniff)
		: m_ref(0)
		, m_client(client)
		, m_lastHTTPMethod(request.httpMethod())
		, status(0)
		, m_defersLoading(defersLoading)
		, m_shouldContentSniff(shouldContentSniff)
		, m_responseFired(false)
		, m_handle(0)
		, m_url(0)
		, m_customHeaders(0)
		, m_cancelled(false)
		//, m_formDataStream(loader)
		, m_scheduledFailureType(NoFailure)
		, m_loader(loader)
		, m_failureTimer(this, &WebURLLoaderInternal::fireFailure)
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
		, m_hookbuf(0)
		, m_hooklen(0)
		, m_hookRequest(false)
#endif
    {
        m_firstRequest = new blink::WebURLRequest(request);
        KURL url = (KURL)m_firstRequest->url();
        m_user = url.user();
        m_pass = url.pass();

        m_response.initialize();
    }

    ~WebURLLoaderInternal()
    {
        delete m_firstRequest;

        fastFree(m_url);
        if (m_customHeaders)
            curl_slist_free_all(m_customHeaders);
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
		if (m_hookbuf)
			free(m_hookbuf);
#endif
    }

    void ref() { ++m_ref; }
    void deref()
    {
        --m_ref;
        if (0 >= m_ref) {
            delete this;
        }
    }

    void fireFailure(blink::Timer<WebURLLoaderInternal>*)
    {
        if (!client())
            return;

        switch (m_scheduledFailureType) {
        case net::WebURLLoaderInternal::NoFailure:
            ASSERT_NOT_REACHED();
            return;
        case net::WebURLLoaderInternal::BlockedFailure:
            m_scheduledFailureType = net::WebURLLoaderInternal::NoFailure;
            //client()->wasBlocked(this);
            return;
        case net::WebURLLoaderInternal::InvalidURLFailure:
            m_scheduledFailureType = net::WebURLLoaderInternal::NoFailure;
            //client()->cannotShowURL(this);

            blink::WebURLError error;
            error.domain = firstRequest()->url().string();
            error.localizedDescription = blink::WebString::fromUTF8("Cannot show DataUR\n");
            if (client() && loader())
                client()->didFail(loader(), error);
            return;
        }

        ASSERT_NOT_REACHED();
    }

    WebURLLoaderClient* client() { return m_client; }
    WebURLLoaderClient* m_client;

    void setResponseFired(bool responseFired) { m_responseFired = responseFired; };
    bool responseFired() { return m_responseFired; }
    bool m_responseFired;

    int m_ref;
    String m_lastHTTPMethod;

    // Suggested credentials for the current redirection step.
    String m_user;
    String m_pass;

    //Credential m_initialCredential;

    int status;

    bool m_defersLoading;
    bool m_shouldContentSniff;

    CURL* m_handle;
    char* m_url;
    struct curl_slist* m_customHeaders;
    WebURLResponse m_response;
    OwnPtr<MultipartHandle> m_multipartHandle;
    bool m_cancelled;

    //FormDataStream m_formDataStream;
    Vector<char> m_postBytes;

    enum FailureType {
        NoFailure,
        BlockedFailure,
        InvalidURLFailure
    };

    FailureType m_scheduledFailureType;
    Timer<WebURLLoaderInternal> m_failureTimer;

    //////////////////////////////////////////////////////////////////////////
    WebURLLoaderImplCurl* loader() { return m_loader; }
    void setLoader(WebURLLoaderImplCurl* loader) { m_loader = loader; }

    blink::WebURLRequest* firstRequest() { return m_firstRequest; }

    WebURLLoaderImplCurl* m_loader;

    blink::WebURLRequest* m_firstRequest;

    String m_debugPath;

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
	bool m_hookRequest;
	void *m_hookbuf;
	int m_hooklen;
#endif
};

} // namespace net

#endif // WebURLLoaderInternal_h
