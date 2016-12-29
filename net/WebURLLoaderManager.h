/*
 * Copyright (C) 2006 Apple Inc.  All rights reserved.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef net_WebURLLoaderManager_h
#define net_WebURLLoaderManager_h

#define CURL_STATICLIB  
#define HTTP_ONLY 

#include "third_party/libcurl/include/curl/curl.h"
#include "third_party/WebKit/Source/platform/Timer.h"

#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/text/CString.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/Threading.h"
#include "third_party/WebKit/Source/platform/Timer.h"

namespace blink {
class WebURLRequest;
}

namespace net {

class WebURLLoaderInternal;

class WebURLLoaderManager {
public:
    enum ProxyType {
        HTTP = CURLPROXY_HTTP,
        Socks4 = CURLPROXY_SOCKS4,
        Socks4A = CURLPROXY_SOCKS4A,
        Socks5 = CURLPROXY_SOCKS5,
        Socks5Hostname = CURLPROXY_SOCKS5_HOSTNAME
    };
    static WebURLLoaderManager* sharedInstance();
    void add(WebURLLoaderInternal*);
    void cancel(WebURLLoaderInternal*);

    CURLSH* getCurlShareHandle() const;

    void setCookieJarFileName(const char* cookieJarFileName);
    const char* getCookieJarFileName() const;

    void dispatchSynchronousJob(WebURLLoaderInternal*);

    void setupPOST(WebURLLoaderInternal*, struct curl_slist**);
    void setupPUT(WebURLLoaderInternal*, struct curl_slist**);

    void setProxyInfo(const String& host,
                      unsigned long port,
                      ProxyType type,
                      const String& username,
                      const String& password);

private:
    WebURLLoaderManager();
    ~WebURLLoaderManager();
    void downloadTimerCallback(blink::Timer<WebURLLoaderManager>* timer);
    void removeFromCurl(WebURLLoaderInternal*);
    bool removeScheduledJob(WebURLLoaderInternal*);
    void startJob(WebURLLoaderInternal*);
    bool startScheduledJobs();
    void applyAuthenticationToRequest(WebURLLoaderInternal*, blink::WebURLRequest*);

    void initializeHandle(WebURLLoaderInternal*);

    void initCookieSession();

    blink::Timer<WebURLLoaderManager> m_downloadTimer;
    CURLM* m_curlMultiHandle;
    CURLSH* m_curlShareHandle;
    char* m_cookieJarFileName;
    char m_curlErrorBuffer[CURL_ERROR_SIZE];
    Vector<WebURLLoaderInternal*> m_resourceHandleList;
    const CString m_certificatePath;
    int m_runningJobs;
    
    String m_proxy;
    ProxyType m_proxyType;
};

}

#endif // net_WebURLLoaderManager_h
