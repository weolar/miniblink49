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
#include "net/SharedMemoryDataConsumerHandle.h"
#include "net/CancelledReason.h"
#include "net/PageNetExtraData.h"
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
    
class WebURLLoaderManagerMainTask;
class WebURLLoaderManager;
class FlattenHTTPBodyElementStream;
struct InitializeHandleInfo;

class JobHead {
public:
    enum Type {
        kLoaderInternal,
        kGetFaviconTask,
        kSetCookiesTask,
        kWkeCustomNetRequest,
    };
    virtual ~JobHead() {}
    virtual int getRefCount() const { return m_ref; }
    virtual void ref() { atomicIncrement(&m_ref); }
    virtual void deref() { atomicDecrement(&m_ref); }
    virtual Type getType() { return m_type; }
    virtual void cancel() {}
    int m_id;
    int m_ref;
    Type m_type;
};

class WebURLLoaderInternal : public JobHead {
public:
    WebURLLoaderInternal(WebURLLoaderImplCurl* loader, const WebURLRequest& request, WebURLLoaderClient* client, bool defersLoading, bool shouldContentSniff);
    virtual ~WebURLLoaderInternal() override;

    WebURLLoaderClient* client() { return m_client; }

    void setResponseFired(bool responseFired) { m_responseFired = responseFired; }
    bool responseFired() { return m_responseFired; }

    WebURLLoaderImplCurl* loader() { return m_loader; }
    void setLoader(WebURLLoaderImplCurl* loader) { m_loader = loader; }

    blink::WebURLRequest* firstRequest() { return m_firstRequest; }

    bool isCancelled() const { return kNoCancelled != m_cancelledReason; }

public:
    WebURLLoaderClient* m_client;
    bool m_isSynchronous;

    blink::WebURLRequest* m_firstRequest;

    String m_lastHTTPMethod;

    // Suggested credentials for the current redirection step.
    String m_user;
    String m_pass;

    //Credential m_initialCredential;

    int status;

    size_t m_dataLength;

    bool m_defersLoading;
    bool m_shouldContentSniff;

    CURL* m_handle;
    char* m_url; // 设置给curl的地址。和request可能不同，主要是fragment
    std::string m_effectiveUrl; // curl收到网络包后返回的最后有效地址，如果有重定向redirect，则可能和上面的变量不同
    struct curl_slist* m_customHeaders;
    WebURLResponse m_response;
    OwnPtr<MultipartHandle> m_multipartHandle;

    CancelledReason m_cancelledReason;

    FlattenHTTPBodyElementStream* m_formDataStream;

    enum FailureType {
        NoFailure,
        BlockedFailure,
        InvalidURLFailure
    };
    FailureType m_scheduledFailureType;

    bool m_responseFired;

    WebURLLoaderImplCurl* m_loader;
    WebURLLoaderManager* m_manager;

    WTF::Mutex m_destroingMutex;
    enum State {
        kNormal,
        kDestroying,
        kDestroyed,
    };
    State m_state;

    Vector<WebURLLoaderManagerMainTask*> m_syncTasks;

    SharedMemoryDataConsumerHandle::Writer* m_bodyStreamWriter;

    String m_debugPath;

    bool m_isBlackList;
    bool m_isDataUrl;
    bool m_isProxy;
    bool m_isProxyHeadRequest;

    InitializeHandleInfo* m_initializeHandleInfo;
    bool m_isHoldJobToAsynCommit;

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    int m_isHookRequest; // 1表示wke接口设置的，2表示内部指定要缓存，3表示既是内部指定，又被缓存了
    Vector<char>* m_hookBufForEndHook;
    Vector<char>* m_asynWkeNetSetData;
    bool m_isWkeNetSetDataBeSetted;
#endif
    RefPtr<PageNetExtraData> m_pageNetExtraData;
};

} // namespace net

#endif // WebURLLoaderInternal_h
