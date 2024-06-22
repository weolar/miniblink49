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
#include <map>

namespace blink {
class WebURLRequest;
class WebURLResponse;
struct WebURLError;
class WebURLLoaderClient;
class WebURLLoader;
}

namespace net {

class JobHead;
class WebURLLoaderInternal;
class WebURLLoaderManager;
class WebCookieJarImpl;
struct BlobTempFileInfo;
struct InitializeHandleInfo;
struct MainTaskArgs;

class AutoLockJob {
public:
    AutoLockJob(WebURLLoaderManager* manager, int jobId);
    WebURLLoaderInternal* lock();
    JobHead* lockJobHead();
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
    static void setCookieJarFullPath(const char* path);
    int addAsynchronousJob(WebURLLoaderInternal*);
    void cancel(int jobId);
    void cancelAll();
    void cancelAllJobsOfWebview(int webviewId);

    JobHead* checkJob(int jobId);
    void removeLiveJobs(int jobId);
    int addLiveJobs(JobHead* job);

    CURLSH* getCurlShareHandle() const;

    //const char* getCookieJarFileName() const;

    void dispatchSynchronousJob(WebURLLoaderInternal*);

    void setProxyInfo(const String& host,
                      unsigned long port,
                      ProxyType type,
                      const String& username,
                      const String& password);

    void shutdown();
    bool isShutdown() const { return m_isShutdown; }
    
    void didReceiveDataOrDownload(WebURLLoaderInternal* job, const char* data, int dataLength, int encodedDataLength);
    void handleDidFinishLoading(WebURLLoaderInternal* job, double finishTime, int64_t totalEncodedDataLength);
    void handleDidFail(WebURLLoaderInternal* job, const blink::WebURLError& error);
    void handleDidReceiveResponse(WebURLLoaderInternal* job);
    void handleDidSentData(WebURLLoaderInternal* job, unsigned long long bytesSent, unsigned long long totalBytesToBeSent);

    void continueJob(WebURLLoaderInternal* job);
    void cancelWithHookRedirect(WebURLLoaderInternal* job);

    String handleHeaderForBlobOnMainThread(WebURLLoaderInternal* job, size_t totalSize);

    enum IoThreadType {
        kIoThreadTypeRes, // 图片等资源类型
        kIoThreadTypeSync, // 同步资源
        kIoThreadTypeOther,
    };

    blink::WebThread* getIoThread(IoThreadType type);
private:
    blink::WebThread* getSyncIoThread();
public:
    WebCookieJarImpl* getShareCookieJar() const;

    bool doCancel(JobHead* jobHeead, CancelledReason cancelledReason);

private:
    WebURLLoaderManager(const char* cookieJarFullPath);
    ~WebURLLoaderManager();

    void initializeMultiHandleOnIoThread(int* waitCount);    

    void setupPOST(WebURLLoaderInternal*, struct curl_slist**);
    void setupPUT(WebURLLoaderInternal*, struct curl_slist**);

    bool downloadOnIoThread();
    void removeFromCurlOnIoThread(int jobId);

    void applyAuthenticationToRequest(WebURLLoaderInternal* job);

    int initializeHandleOnMainThread(WebURLLoaderInternal* job);
    void initializeHandleOnIoThread(int jobId, InitializeHandleInfo* info);
    InitializeHandleInfo* preInitializeHandleOnMainThread(WebURLLoaderInternal* job);
    void startOnIoThread(int jobId);
    void timeoutOnMainThread(int jobId);

    void dispatchSynchronousJobOnIoThread(WebURLLoaderInternal* job, InitializeHandleInfo* info, CURLcode* ret, int* isCallFinish);

    void initCookieSession(const char* cookiePath);

    CURLM* getCurrentThreadCurlMultiHandle() const;

    static WebURLLoaderManager* m_sharedInstance;

    Vector<WebURLLoaderInternal*> m_resourceHandleList;
    //CURLM* m_curlMultiHandle;
    //CURLSH* m_curlShareHandle;
    //char* m_cookieJarFileName;
    std::map<DWORD, CURLM*> m_curlMultiHandles;
    WebCookieJarImpl* m_shareCookieJar;

    char m_curlErrorBuffer[CURL_ERROR_SIZE];
    const CString m_certificatePath;
    int m_runningJobs;
    WTF::Vector<blink::WebThread*> m_threads;
    blink::WebThread* m_syncIoThread;
    String m_proxy;
    ProxyType m_proxyType;
    bool m_isShutdown;

    friend class WebURLLoaderInternal;
    WTF::Mutex m_liveJobsMutex;
    WTF::HashMap<int, JobHead*> m_liveJobs;
    int m_newestJobId;
    
    friend class WebURLLoaderManagerMainTask;
    WTF::Mutex m_mainTasksMutex;
    WebURLLoaderManagerMainTask* m_mainTasksBegin;
    WebURLLoaderManagerMainTask* m_mainTasksEnd;

public:
    //WTF::Mutex m_shutdownMutex;
    class ShutdownRWLock {
    public:
        ShutdownRWLock()
        {
            m_readCount = 0;
            m_writeCount = 0;
        }

        bool read()
        {
            WTF::Locker<WTF::Mutex> locker(m_lock);
            if (m_writeCount > 0)
                return false;

            ++m_readCount;
            return true;
        }

        void releaseRead()
        {
            WTF::Locker<WTF::Mutex> locker(m_lock);
            --m_readCount;
        }

        void waitForWrite()
        {
            m_lock.lock();
            ++m_writeCount;
            m_lock.unlock();

            while (true) {
                m_lock.lock();
                if (m_readCount > 0) {
                    m_lock.unlock();
                    ::Sleep(10);
                } else {
                    m_lock.unlock();
                    break;
                }
            }
        }

    private:
        int m_readCount;
        int m_writeCount;
        WTF::Mutex m_lock;
    };

    class ShutdownReadLocker {
    public:
        ShutdownReadLocker(ShutdownRWLock* lock)
        {
            m_lock = lock;
        }

        bool lock()
        {
            bool b = m_lock->read();
            if (!b)
                m_lock = nullptr;
            return b;
        }

        ~ShutdownReadLocker()
        {
            if (m_lock)
                m_lock->releaseRead();
        }
    private:
        ShutdownRWLock* m_lock;
    };

    ShutdownRWLock m_shutdownLock;
};

}

#endif // net_WebURLLoaderManager_h
