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

#include "net/CancelledReason.h"
#include "net/ProxyType.h"

#include "third_party/libcurl/include/curl/curl.h"
#include "third_party/WebKit/Source/platform/Timer.h"

#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/text/CString.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/Threading.h"
#include "third_party/WebKit/Source/platform/Timer.h"

namespace blink {
class WebURLRequest;
class WebURLResponse;
struct WebURLError;
}

namespace net {

class WebURLLoaderInternal;
class WebURLLoaderManager;
struct BlobTempFileInfo;
struct InitializeHandleInfo;

class AutoLockJob {
public:
    AutoLockJob(WebURLLoaderManager* manager, int jobId);
    WebURLLoaderInternal* lock();
    ~AutoLockJob();

    void setNotDerefForDelete();

private:
    WebURLLoaderManager* m_manager;
    int m_jobId;
    bool m_isNotDerefForDelete;
};

class WebURLLoaderManager {
public:
    class IoTask;
    class MainTask;
    static WebURLLoaderManager* sharedInstance();
    int addAsynchronousJob(WebURLLoaderInternal*);
    void cancel(int jobId);
    void cancelAll();

    WebURLLoaderInternal* checkJob(int jobId);
    void removeLiveJobs(int jobId);
    int addLiveJobs(WebURLLoaderInternal* job);

    CURLSH* getCurlShareHandle() const;

    const char* getCookieJarFileName() const;

    void dispatchSynchronousJob(WebURLLoaderInternal*);

    void setProxyInfo(const String& host,
                      unsigned long port,
                      ProxyType type,
                      const String& username,
                      const String& password);
    void shutdown();

    bool isShutdown() const { return m_isShutdown; }

    String handleHeaderForBlobOnMainThread(WebURLLoaderInternal* job, size_t totalSize);
    BlobTempFileInfo* getBlobTempFileInfoByTempFilePath(const String& path);
    void didReceiveDataOrDownload(WebURLLoaderInternal* job, const char* data, int dataLength, int encodedDataLength);
    void handleDidFinishLoading(WebURLLoaderInternal* job, double finishTime, int64_t totalEncodedDataLength);
    void handleDidFail(WebURLLoaderInternal* job, const blink::WebURLError& error);
    void handleDidReceiveResponse(WebURLLoaderInternal* job);

    void continueJob(WebURLLoaderInternal* job);
    void cancelWithHookRedirect(WebURLLoaderInternal* job);

private:
    WebURLLoaderManager();
    ~WebURLLoaderManager();

    void doCancel(WebURLLoaderInternal* job, CancelledReason cancelledReason);
    
    void setupPOST(WebURLLoaderInternal*, struct curl_slist**);
    void setupPUT(WebURLLoaderInternal*, struct curl_slist**);

    bool downloadOnIoThread();
    void removeFromCurlOnIoThread(int jobId);

    void applyAuthenticationToRequest(WebURLLoaderInternal*, blink::WebURLRequest*);

    int initializeHandleOnMainThread(WebURLLoaderInternal* job);
    void initializeHandleOnIoThread(int jobId, InitializeHandleInfo* info);
    InitializeHandleInfo* preInitializeHandleOnMainThread(WebURLLoaderInternal* job);
    void startOnIoThread(int jobId);
    void timeoutOnMainThread(int jobId);

    void dispatchSynchronousJobOnIoThread(WebURLLoaderInternal* job, InitializeHandleInfo* info, CURLcode* ret, int* isCallFinish);

    void initCookieSession();

    Vector<WebURLLoaderInternal*> m_resourceHandleList;
    CURLM* m_curlMultiHandle;
    CURLSH* m_curlShareHandle;
    char* m_cookieJarFileName;
    char m_curlErrorBuffer[CURL_ERROR_SIZE];
    const CString m_certificatePath;
    int m_runningJobs;
    blink::WebThread* m_thread;
    String m_proxy;
    ProxyType m_proxyType;
    bool m_isShutdown;

    friend class WebURLLoaderManagerMainTask;
    WTF::Mutex m_liveJobsMutex;
    WTF::HashMap<int, WebURLLoaderInternal*> m_liveJobs;
    int m_newestJobId;
    
    WTF::HashMap<String, BlobTempFileInfo*> m_blobCache; // real url -> <temp, data>
};

}

#endif // net_WebURLLoaderManager_h
