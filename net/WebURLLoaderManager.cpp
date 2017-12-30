/*
 * Copyright (C) 2004, 2006 Apple Inc.  All rights reserved.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 * Copyright (C) 2007 Holger Hans Peter Freyther
 * Copyright (C) 2008 Collabora Ltd.
 * Copyright (C) 2008 Nuanti Ltd.
 * Copyright (C) 2009 Appcelerator Inc.
 * Copyright (C) 2009 Brent Fulgham <bfulgham@webkit.org>
 * Copyright (C) 2013 Peter Gal <galpeter@inf.u-szeged.hu>, University of Szeged
 * Copyright (C) 2013 Alex Christensen <achristensen@webkit.org>
 * Copyright (C) 2013 University of Szeged
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

#if USING_VC6RT == 1
#define PURE = 0
#endif

#include <shlobj.h>
#include <shlwapi.h>

#include "config.h"
#include "net/WebURLLoaderManager.h"

#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include "third_party/WebKit/public/platform/WebURLError.h"
#include "third_party/WebKit/public/platform/WebHTTPHeaderVisitor.h"
#include "third_party/WebKit/public/platform/WebScheduler.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/platform/network/HTTPParsers.h"
#include "third_party/WebKit/Source/platform/MIMETypeRegistry.h"
#include "third_party/WebKit/Source/web/WebLocalFrameImpl.h"

#include "content/web_impl_win/WebBlobRegistryImpl.h"
#include "content/web_impl_win/WebCookieJarCurlImpl.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "content/browser/WebFrameClientImpl.h"
#include "content/browser/WebPage.h"
#include "net/WebURLLoaderInternal.h"
#include "net/DataURL.h"
#include "net/RequestExtraData.h"
#include "net/BlobResourceLoader.h"
#include "net/SharedMemoryDataConsumerHandle.h"
#include "net/FixedReceivedData.h"
#include "third_party/WebKit/Source/wtf/Threading.h"
#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/text/CString.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include <errno.h>
#include <stdio.h>

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
#include "wke/wkeWebView.h"
#endif
#include "wtf/RefCountedLeakCounter.h"

using namespace blink;

extern WKE_FILE_OPEN g_pfnOpen;
extern WKE_FILE_CLOSE g_pfnClose;

namespace net {

const int selectTimeoutMS = 5;
const double pollTimeSeconds = 0.05;
const int maxRunningJobs = 5;

static const bool ignoreSSLErrors = true; //  ("WEBKIT_IGNORE_SSL_ERRORS");

static const int kAllowedProtocols = CURLPROTO_FILE | CURLPROTO_FTP | CURLPROTO_FTPS | CURLPROTO_HTTP | CURLPROTO_HTTPS;

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, webURLLoaderInternalCounter, ("WebURLLoaderInternal"));
#endif

static CString certificatePath()
{
#if 0 
    char* envPath = getenv("CURL_CA_BUNDLE_PATH");
    if (envPath)
        return envPath;
#endif
    return CString();
}

static char* g_cookieJarPath = nullptr;

void setCookieJarFullPath(const WCHAR* path)
{
    if (!path)
        return;

    std::vector<char> jarPathA;
    WTF::WCharToMByte(path, wcslen(path), &jarPathA, CP_ACP);
    if (0 == jarPathA.size())
        return;

    if (g_cookieJarPath)
        free(g_cookieJarPath);

    const int pathLen = (MAX_PATH + 1) * sizeof(char) * 5;
    g_cookieJarPath = (char*)malloc(pathLen);
    memset(g_cookieJarPath, 0, pathLen);

    strncpy(g_cookieJarPath, &jarPathA[0], jarPathA.size());
}

void setCookieJarPath(const WCHAR* path)
{
    if (!path || !::PathIsDirectoryW(path))
        return;

    Vector<WCHAR> jarPath;
    jarPath.resize(MAX_PATH + 1);
    wcscpy(jarPath.data(), path);
    ::PathAppendW(jarPath.data(), L"cookies.dat");

    setCookieJarFullPath(jarPath.data());
}

static char* cookieJarPath()
{
    if (g_cookieJarPath)
        return g_cookieJarPath;
    return fastStrDup("cookies.dat");
}

#if ENABLE(WEB_TIMING)
static int milisecondsSinceRequest(double requestTime)
{
    return static_cast<int>((monotonicallyIncreasingTime() - requestTime) * 1000.0);
}

static void calculateWebTimingInformations(ResourceHandleInternal* job)
{
    double startTransfertTime = 0;
    double preTransferTime = 0;
    double dnslookupTime = 0;
    double connectTime = 0;
    double appConnectTime = 0;

    curl_easy_getinfo(job->m_handle, CURLINFO_NAMELOOKUP_TIME, &dnslookupTime);
    curl_easy_getinfo(job->m_handle, CURLINFO_CONNECT_TIME, &connectTime);
    curl_easy_getinfo(job->m_handle, CURLINFO_APPCONNECT_TIME, &appConnectTime);
    curl_easy_getinfo(job->m_handle, CURLINFO_STARTTRANSFER_TIME, &startTransfertTime);
    curl_easy_getinfo(job->m_handle, CURLINFO_PRETRANSFER_TIME, &preTransferTime);

    job->m_response.resourceLoadTiming().domainLookupStart = 0;
    job->m_response.resourceLoadTiming().domainLookupEnd = static_cast<int>(dnslookupTime * 1000);

    job->m_response.resourceLoadTiming().connectStart = static_cast<int>(dnslookupTime * 1000);
    job->m_response.resourceLoadTiming().connectEnd = static_cast<int>(connectTime * 1000);

    job->m_response.resourceLoadTiming().requestStart = static_cast<int>(connectTime * 1000);
    job->m_response.resourceLoadTiming().responseStart = static_cast<int>(preTransferTime * 1000);

    if (appConnectTime)
        job->m_response.resourceLoadTiming().secureConnectionStart = static_cast<int>(connectTime * 1000);
}
#endif

// libcurl does not implement its own thread synchronization primitives.
// these two functions provide mutexes for cookies, and for the global DNS
// cache.
static Mutex* sharedResourceMutex(curl_lock_data data)
{
    DEFINE_STATIC_LOCAL(Mutex, cookieMutex, ());
    DEFINE_STATIC_LOCAL(Mutex, dnsMutex, ());
    DEFINE_STATIC_LOCAL(Mutex, shareMutex, ());

    switch (data) {
    case CURL_LOCK_DATA_COOKIE:
        return &cookieMutex;
    case CURL_LOCK_DATA_DNS:
        return &dnsMutex;
    case CURL_LOCK_DATA_SHARE:
        return &shareMutex;
    default:
        ASSERT_NOT_REACHED();
        return NULL;
    }
}

static void curl_lock_callback(CURL* /* handle */, curl_lock_data data, curl_lock_access /* access */, void* /* userPtr */)
{
    if (Mutex* mutex = sharedResourceMutex(data))
        mutex->lock();
}

static void curl_unlock_callback(CURL* /* handle */, curl_lock_data data, void* /* userPtr */)
{
    if (Mutex* mutex = sharedResourceMutex(data))
        mutex->unlock();
}

inline static bool isHttpInfo(int statusCode)
{
    return 100 <= statusCode && statusCode < 200;
}

inline static bool isHttpRedirect(int statusCode)
{
    return 300 <= statusCode && statusCode < 400 && statusCode != 304;
}

inline static bool isHttpAuthentication(int statusCode)
{
    return statusCode == 401;
}

inline static bool isHttpNotModified(int statusCode)
{
    return statusCode == 304;
}

inline static bool isAppendableHeader(const String &key)
{
    static const char* appendableHeaders[] = {
        "access-control-allow-headers",
        "access-control-allow-methods",
        "access-control-allow-origin",
        "access-control-expose-headers",
        "allow",
        "cache-control",
        "connection",
        "content-encoding",
        "content-language",
        "if-match",
        "if-none-match",
        "keep-alive",
        "pragma",
        "proxy-authenticate",
        "public",
        "server",
        "set-cookie",
        "te",
        "trailer",
        "transfer-encoding",
        "upgrade",
        "user-agent",
        "vary",
        "via",
        "warning",
        "www-authenticate",
        0
    };

    // Custom headers start with 'X-', and need no further checking.
    if (key.startsWith("x-", WTF::TextCaseInsensitive))
        return true;

    for (unsigned i = 0; appendableHeaders[i]; ++i)
        if (equalIgnoringCase(key, appendableHeaders[i]))
            return true;

    return false;
}

WebURLLoaderManager::WebURLLoaderManager()
    : m_cookieJarFileName(cookieJarPath())
    , m_certificatePath(certificatePath())
    , m_runningJobs(0)
    , m_isShutdown(false)
    , m_newestJobId(1)
{
    content::BlinkPlatformImpl* platform = (content::BlinkPlatformImpl*)Platform::current();
    m_thread = platform->ioThread();

    curl_global_init(CURL_GLOBAL_ALL);
    //初始化curl批处理句柄
    m_curlMultiHandle = curl_multi_init();
    //初始化共享curl句柄,用于共享cookies和dns等缓存
    m_curlShareHandle = curl_share_init();
    curl_share_setopt(m_curlShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
    curl_share_setopt(m_curlShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
    curl_share_setopt(m_curlShareHandle, CURLSHOPT_LOCKFUNC, curl_lock_callback);
    curl_share_setopt(m_curlShareHandle, CURLSHOPT_UNLOCKFUNC, curl_unlock_callback);

    initCookieSession();
}

WebURLLoaderManager::~WebURLLoaderManager()
{
    curl_multi_cleanup(m_curlMultiHandle);
    curl_share_cleanup(m_curlShareHandle);
    if (m_cookieJarFileName)
        fastFree(m_cookieJarFileName);
    curl_global_cleanup();
}

void WebURLLoaderManager::shutdown()
{
    m_isShutdown = true;

    m_liveJobsMutex.lock();
    WTF::HashMap<int, WebURLLoaderInternal*> liveJobs = m_liveJobs;
    m_liveJobs.clear();
    m_liveJobsMutex.unlock();

    WTF::HashMap<int, WebURLLoaderInternal*>::iterator it = liveJobs.begin();
    for (; it != liveJobs.end(); ++it) {
        WebURLLoaderInternal* job = it->value;

        while (true) {
            m_liveJobsMutex.lock();
            if (0 < job->getRefCount()) {
                m_liveJobsMutex.unlock();
                ::Sleep(20);
                continue;
            }
            delete job;
            m_liveJobsMutex.unlock();
            break;
        }
    }

    // delete m_thread;
    m_thread = nullptr;
}

void WebURLLoaderManager::initCookieSession()
{
    // Curl saves both persistent cookies, and session cookies to the cookie file.
    // The session cookies should be deleted before starting a new session.

    CURL* curl = curl_easy_init();

    if (!curl)
        return;

    curl_easy_setopt(curl, CURLOPT_SHARE, m_curlShareHandle);

    if (m_cookieJarFileName) {
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, m_cookieJarFileName);
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, m_cookieJarFileName);
    }

    curl_easy_setopt(curl, CURLOPT_COOKIESESSION, 1);

    curl_easy_cleanup(curl);
}

CURLSH* WebURLLoaderManager::getCurlShareHandle() const
{
    return m_curlShareHandle;
}

void WebURLLoaderManager::setCookieJarFileName(const char* cookieJarFileName)
{
    m_cookieJarFileName = fastStrDup(cookieJarFileName);
}

const char* WebURLLoaderManager::getCookieJarFileName() const
{
    return m_cookieJarFileName;
}

WebURLLoaderManager* WebURLLoaderManager::sharedInstance()
{
    static WebURLLoaderManager* sharedInstance = 0;
    if (!sharedInstance)
        sharedInstance = new WebURLLoaderManager();
    if (sharedInstance->isShutdown())
        return nullptr;
    return sharedInstance;
}

void WebURLLoaderManager::didReceiveDataOrDownload(WebURLLoaderInternal* job, const char* data, int dataLength, int encodedDataLength)
{
    blink::WebURLLoaderClient* client = job->client();
    WebURLLoaderImplCurl* loader = job->loader();

    job->m_dataLength += dataLength;

    if (job->firstRequest()->useStreamOnResponse()) {
        // We don't support ftp_listening_delegate_ and multipart_delegate_ for now.
        // TODO(yhirano): Support ftp listening and multipart.
        job->m_bodyStreamWriter->addData(adoptPtr(new FixedReceivedData(data, dataLength, encodedDataLength)));
    }

    if (!job->firstRequest()->downloadToFile()) {
        client->didReceiveData(loader, data, dataLength, encodedDataLength);
        return;
    }

    WTF::HashMap<String, BlobTempFileInfo*>::iterator it = m_blobCache.find(String(job->m_url));
    if (it == m_blobCache.end()) {
        DebugBreak();
        return;
    }

    BlobTempFileInfo* tempFile = it->value;
    Vector<char>& tempFileData = tempFile->data;
    tempFileData.append(data, dataLength);

    client->didDownloadData(loader, dataLength, encodedDataLength);
}

BlobTempFileInfo* WebURLLoaderManager::getBlobTempFileInfoByTempFilePath(const String& path)
{
    WTF::HashMap<String, BlobTempFileInfo*>::iterator it = m_blobCache.begin();
    for (; it != m_blobCache.end(); ++it) {
        BlobTempFileInfo* tempFileInfo = it->value;
        if (tempFileInfo->tempUrl != path)
            continue;

        return tempFileInfo;
    }

    return nullptr;
}

String WebURLLoaderManager::handleHeaderForBlobOnMainThread(WebURLLoaderInternal* job, size_t totalSize)
{
    String tempPath = String::format("file:///c:/miniblink_blob_download_%d", GetTickCount());

    WTF::HashMap<String, BlobTempFileInfo*>::iterator it = m_blobCache.find(String(job->m_url));
    if (it != m_blobCache.end())
        return it->value->tempUrl;

    BlobTempFileInfo* tempFileInfo = new BlobTempFileInfo();
    tempFileInfo->tempUrl = tempPath;
    tempFileInfo->refCount = 0;

    m_blobCache.set(String(job->m_url), tempFileInfo);

    return tempPath;
}

static void setBlobDataLengthByTempPath(WebURLLoaderInternal* job)
{
    if (!job->firstRequest()->downloadToFile())
        return;

    content::WebBlobRegistryImpl* blolReg = (content::WebBlobRegistryImpl*)blink::Platform::current()->blobRegistry();
    blolReg->setBlobDataLengthByTempPath(job->m_response.downloadFilePath(), job->m_dataLength);
}

void WebURLLoaderManager::handleDidFinishLoading(WebURLLoaderInternal* job, double finishTime, int64_t totalEncodedDataLength)
{
    if (job->m_bodyStreamWriter) {
        delete job->m_bodyStreamWriter;
        job->m_bodyStreamWriter = nullptr;
    }

    KURL url = job->firstRequest()->url();

//     String outString = String::format("handleDidFinishLoading:%s\n", WTF::ensureStringToUTF8(url.string(), true).data());
//     OutputDebugStringW(outString.charactersWithNullTermination().data());

    setBlobDataLengthByTempPath(job);
    job->client()->didFinishLoading(job->loader(), finishTime, totalEncodedDataLength);
}

void WebURLLoaderManager::handleDidFail(WebURLLoaderInternal* job, const blink::WebURLError& error)
{
    if (job->m_bodyStreamWriter) {
        job->m_bodyStreamWriter->fail();
        delete job->m_bodyStreamWriter;
        job->m_bodyStreamWriter = nullptr;
    }
    KURL url = job->firstRequest()->url();

//     String outString = String::format("handleDidFail on ui Thread:%d %s\n", error.reason, WTF::ensureStringToUTF8(url.string(), true).data());
//     OutputDebugStringW(outString.charactersWithNullTermination().data());

    setBlobDataLengthByTempPath(job);
    job->client()->didFail(job->loader(), error);
}

static void cancelBodyStreaming(int jobId)
{
    AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
    WebURLLoaderInternal* job = autoLockJob.lock();
    if (!job)
        return;

     if (job->m_bodyStreamWriter) {
        job->m_bodyStreamWriter->fail();
        delete job->m_bodyStreamWriter;
        job->m_bodyStreamWriter = nullptr;
    }

    if (job->client()) {
        // TODO(yhirano): Set |stale_copy_in_cache| appropriately if possible.
        WebURLError error;
        error.domain = WebString(String(job->m_url));
        error.reason = -3;
        error.localizedDescription = WebString::fromUTF8("cancelBodyStreaming");
        job->client()->didFail(job->loader(), error);
    }

    // Notify the browser process that the request is canceled.
    WebURLLoaderManager::sharedInstance()->cancel(jobId);
}

void WebURLLoaderManager::handleDidReceiveResponse(WebURLLoaderInternal* job)
{
    const blink::WebURLResponse& response = job->m_response;
    WebDataConsumerHandle* readHandle = nullptr;
    if (job->firstRequest() && job->firstRequest()->useStreamOnResponse()) {
        SharedMemoryDataConsumerHandle::BackpressureMode mode = SharedMemoryDataConsumerHandle::kDoNotApplyBackpressure;

        String cacheControl = response.httpHeaderField("Cache-Control");
        if (cacheControl == "no-store")
            mode = SharedMemoryDataConsumerHandle::kApplyBackpressure;

        readHandle = new SharedMemoryDataConsumerHandle(mode, WTF::bind(&cancelBodyStreaming, job->m_id), &job->m_bodyStreamWriter);

        // Here |body_stream_writer_| has an indirect reference to |this| and that
        // creates a reference cycle, but it is not a problem because the cycle
        // will break if one of the following happens:
        //  1) The body data transfer is done (with or without an error).
        //  2) |readHandle| (and its reader) is detached.

        // The client takes |readHandle|'s ownership.
        // TODO(yhirano): Support ftp listening and multipart
    }

    job->client()->didReceiveResponse(job->loader(), response, readHandle);
}


// 回调回main线程的task
class WebURLLoaderManagerMainTask : public WebThread::Task {
public:
    enum TaskType {
        kWriteCallback,
        kHeaderCallback,
        kDidFinishLoading,
        kRemoveFromCurl,
        kDidCancel,
        kHandleLocalReceiveResponse,
        kContentEnded,
        kDidFail,
        kHandleHookRequest,
    };

    struct Args {
        void* ptr;
        size_t size;
        size_t nmemb;
        long httpCode;
        double contentLength;
        char* hdr;
        WebURLError* resourceError;

        ~Args()
        {
            free(ptr);
            free(hdr);
            delete resourceError;
        }

        static Args* build(void* ptr, size_t size, size_t nmemb, size_t totalSize, CURL* handle, bool isProxy)
        {
            Args* args = new Args();
            args->size = size;
            args->nmemb = nmemb;
            args->ptr = malloc(totalSize);
            args->resourceError = new WebURLError();
            memcpy(args->ptr, ptr, totalSize);

            curl_easy_getinfo(handle, !isProxy ? CURLINFO_RESPONSE_CODE : CURLINFO_HTTP_CONNECTCODE, &args->httpCode);
            if (isProxy && 0 == args->httpCode)
                args->httpCode = 200;

            double contentLength = 0;
            curl_easy_getinfo(handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &args->contentLength);

            const char* hdr = nullptr;
            args->hdr = nullptr;
            int hdrLen = 0;
            curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &hdr);
            if (hdr)
                hdrLen = strlen(hdr);

            args->hdr = (char*)malloc(hdrLen + 1);
            strncpy(args->hdr, hdr, hdrLen);
            args->hdr[hdrLen] = '\0';
            return args;
        }
    };

    virtual ~WebURLLoaderManagerMainTask() override
    {
        delete m_args;
    }

    void release()
    {
        AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), m_jobId);
        autoLockJob.setNotDerefForDelete();
        WebURLLoaderInternal* job = autoLockJob.lock();
        if (!job)
            return;

        WTF::Mutex& liveJobsMutex = WebURLLoaderManager::sharedInstance()->m_liveJobsMutex;
        while (true) {
            liveJobsMutex.lock();
            if (2 < job->getRefCount()) {
                liveJobsMutex.unlock();
                ::Sleep(20);
                continue;
            }

            job->m_handle = nullptr;
            WebURLLoaderManager::sharedInstance()->removeLiveJobs(m_jobId);
            delete job;

            liveJobsMutex.unlock();
            break;
        }
    }

    virtual void run() override
    {
        AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), m_jobId);
        WebURLLoaderInternal* job = autoLockJob.lock();
        if (!job)
            return;

        if (kRemoveFromCurl == m_type || kDidCancel == m_type) {
            autoLockJob.setNotDerefForDelete();
            release();
            return;
        }

        if (WebURLLoaderManager::sharedInstance()->isShutdown() || job->m_cancelled)
            return;

        switch (m_type) {
        case kWriteCallback:
            handleWriteCallbackOnMainThread(m_args, job);
            break;
        case kHeaderCallback:
            handleHeaderCallbackOnMainThread(m_args, job);
            break;
        case kDidFinishLoading:
            if (job->m_hookBuf)
                WebURLLoaderManager::sharedInstance()->didReceiveDataOrDownload(job, static_cast<char*>(job->m_hookBuf), job->m_hookLength, 0);
            WebURLLoaderManager::sharedInstance()->handleDidFinishLoading(job, 0, 0);
            break;
        case kRemoveFromCurl:
            break;
        case kDidCancel:
            break;
        case kHandleLocalReceiveResponse:
            handleLocalReceiveResponseOnMainThread(m_args, job);
            break;
        case kContentEnded:
            if (job->m_hookBuf)
                job->m_multipartHandle->contentReceived(static_cast<const char*>(job->m_hookBuf), job->m_hookLength);
            job->m_multipartHandle->contentEnded();
            break;
        case kDidFail:
            WebURLLoaderManager::sharedInstance()->handleDidFail(job, *(m_args->resourceError));
            break;
        case kHandleHookRequest:
            handleHookRequestOnMainThread(job);
            break;
        default:
            break;
        }
    }

    static Args* pushTask(int jobId, TaskType type, void* ptr, size_t size, size_t nmemb, size_t totalSize)
    {
        AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
        WebURLLoaderInternal* job = autoLockJob.lock();
        if (!job)
            return nullptr;

        Args* args = Args::build(ptr, size, nmemb, totalSize, job->m_handle, job->m_isProxy);
        WebURLLoaderManagerMainTask* task = new WebURLLoaderManagerMainTask(jobId, type, args);

        if (job->m_isSynchronous)
            job->m_syncTasks.append(task);
        else
            Platform::current()->mainThread()->scheduler()->postLoadingTask(FROM_HERE, task); // postLoadingTask
        return args;
    }

    static WebURLLoaderManagerMainTask* createTask(int jobId, TaskType type, void* ptr, size_t size, size_t nmemb, size_t totalSize)
    {
        AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
        WebURLLoaderInternal* job = autoLockJob.lock();
        if (!job)
            return nullptr;
        Args* args = Args::build(ptr, size, nmemb, totalSize, job->m_handle, job->m_isProxy);
        WebURLLoaderManagerMainTask* task = new WebURLLoaderManagerMainTask(jobId, type, args);
        return task;
    }

    static size_t handleWriteCallbackOnMainThread(WebURLLoaderManagerMainTask::Args* args, WebURLLoaderInternal* job);
    static size_t handleHeaderCallbackOnMainThread(WebURLLoaderManagerMainTask::Args* args, WebURLLoaderInternal* job);
    static void handleLocalReceiveResponseOnMainThread(WebURLLoaderManagerMainTask::Args* args, WebURLLoaderInternal* job);
    static void handleHookRequestOnMainThread(WebURLLoaderInternal* job);

private:
    int m_jobId;
    TaskType m_type;
    Args* m_args;

    WebURLLoaderManagerMainTask(int jobId, TaskType type, Args* args)
        : m_jobId(jobId)
        , m_type(type)
        , m_args(args)
    {
    }
};

static bool setHttpResponseDataToJobWhenDidReceiveResponseOnMainThread(WebURLLoaderInternal* job, WebURLLoaderManagerMainTask::Args* args)
{
    WebURLLoaderClient* client = job->client();
    size_t size = args->size;
    size_t nmemb = args->nmemb;
    size_t totalSize = size * nmemb;

    if (isHttpInfo(args->httpCode)) {
        // Just return when receiving http info, e.g. HTTP/1.1 100 Continue.
        // If not, the request might be cancelled, because the MIME type will be empty for this response.
        return false;
    }

    if (job->firstRequest()->downloadToFile()) {
        String tempPath = WebURLLoaderManager::sharedInstance()->handleHeaderForBlobOnMainThread(job, totalSize);
        job->m_response.setDownloadFilePath(tempPath);
    }

    AtomicString contentType = job->m_response.httpHeaderField(WebString::fromUTF8("Content-Type"));

    job->m_response.setExpectedContentLength(static_cast<long long int>(args->contentLength));
    job->m_response.setURL(KURL(ParsedURLString, args->hdr));
    job->m_response.setHTTPStatusCode(args->httpCode);
    job->m_response.setMIMEType(extractMIMETypeFromMediaType(contentType).lower());
    job->m_response.setTextEncodingName(extractCharsetFromMediaType(contentType));
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (equalIgnoringCase(contentType, "application/octet-stream") ||
        contentDispositionType(job->m_response.httpHeaderField("Content-Disposition")) == ContentDispositionAttachment) {
        RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
        WebPage* page = requestExtraData->page;
        if (page->wkeHandler().downloadCallback) {
            Vector<char> urlBuf = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);
            if (page->wkeHandler().downloadCallback(page->wkeWebView(), page->wkeHandler().downloadCallbackParam, urlBuf.data())) {
                blink::WebLocalFrame* frame = requestExtraData->frame;
                frame->stopLoading();
                return true;
            }
        }
    }
#endif
    if (equalIgnoringCase((String)(job->m_response.mimeType()), "multipart/x-mixed-replace")) {
        String boundary;
        bool parsed = MultipartHandle::extractBoundary(job->m_response.httpHeaderField(WebString::fromUTF8("Content-Type")), boundary);
        if (parsed)
            job->m_multipartHandle = adoptPtr(new MultipartHandle(job, boundary));
    }

    // HTTP redirection
    if (isHttpRedirect(args->httpCode)) {
        String location = job->m_response.httpHeaderField(WebString::fromUTF8("location"));
        if (!location.isEmpty()) {
            //重定向
            KURL newURL = KURL((KURL)(job->firstRequest()->url()), location);
            blink::WebURLRequest* redirectedRequest = new blink::WebURLRequest(*job->firstRequest());
            redirectedRequest->setURL(newURL);
            if (client && job->loader())
                client->willSendRequest(job->loader(), *redirectedRequest, job->m_response);
#if 0
            String outString = String::format("redirection:%p, %s\n", job, job->m_response.url().string().utf8().c_str());
            OutputDebugStringW(outString.charactersWithNullTermination().data());
#endif
            job->m_response.initialize();

            delete job->m_firstRequest;
            job->m_firstRequest = redirectedRequest;
            return false;
        }
    } else if (isHttpAuthentication(args->httpCode)) {

    }

    return true;
}

static void setResponseDataToJobWhenDidReceiveResponseOnMainThread(WebURLLoaderInternal* job, WebURLLoaderManagerMainTask::Args* args)
{
    KURL url = job->firstRequest()->url();
    bool needSetResponseFired = true;

    if (url.protocolIsInHTTPFamily())
        needSetResponseFired = setHttpResponseDataToJobWhenDidReceiveResponseOnMainThread(job, args);
    
    if (needSetResponseFired && !job->m_cancelled) {
        if (job->client() && job->loader())
            WebURLLoaderManager::sharedInstance()->handleDidReceiveResponse(job);
        job->setResponseFired(true);
    }
}

void WebURLLoaderManagerMainTask::handleLocalReceiveResponseOnMainThread(WebURLLoaderManagerMainTask::Args* args, WebURLLoaderInternal* job)
{
    if (job->responseFired())
        return;

    // since the code in headerCallbackOnIoThread will not have run for local files
    // the code to set the KURL and fire didReceiveResponse is never run,
    // which means the ResourceLoader's response does not contain the KURL.
    // Run the code here for local files to resolve the issue.
    // TODO: See if there is a better approach for handling this.
    job->m_response.setURL(KURL(ParsedURLString, args->hdr));

    setResponseDataToJobWhenDidReceiveResponseOnMainThread(job, args);

//     if (job->client() && job->loader() && !job->responseFired())
//         WebURLLoaderManager::sharedInstance()->handleDidReceiveResponse(job);
//     job->setResponseFired(true);
}

// called with data after all headers have been processed via headerCallbackOnIoThread
size_t WebURLLoaderManagerMainTask::handleWriteCallbackOnMainThread(WebURLLoaderManagerMainTask::Args* args, WebURLLoaderInternal* job)
{
    void* ptr = args->ptr;
    size_t size = args->size;
    size_t nmemb = args->nmemb;

    size_t totalSize = size * nmemb;

    if (!job->responseFired()) {
        handleLocalReceiveResponseOnMainThread(args, job);
        if (job->m_cancelled)
            return 0;
    }

    if (job->m_isHookRequest) {
        if (!job->m_hookBuf) {
            job->m_hookBuf = malloc(totalSize);
        } else {
            job->m_hookBuf = realloc(job->m_hookBuf, job->m_hookLength + totalSize);
        }
        memcpy(((char *)job->m_hookBuf + job->m_hookLength), ptr, totalSize);
        job->m_hookLength += totalSize;
        return totalSize;
    }

    if (job->m_multipartHandle) {
        job->m_multipartHandle->contentReceived(static_cast<const char*>(ptr), totalSize);
    } else if (job->client() && job->loader()) {
        WebURLLoaderManager::sharedInstance()->didReceiveDataOrDownload(job, static_cast<char*>(ptr), totalSize, 0);
    }
    return totalSize;
}

size_t WebURLLoaderManagerMainTask::handleHeaderCallbackOnMainThread(WebURLLoaderManagerMainTask::Args* args, WebURLLoaderInternal* job)
{
    if (job->m_cancelled)
        return 0;

    // We should never be called when deferred loading is activated.
    ASSERT(!job->m_defersLoading);

    size_t totalSize = args->size * args->nmemb;
    WebURLLoaderClient* client = job->client();

    String header(static_cast<const char*>(args->ptr), totalSize);

    String url = job->firstRequest()->url().string();

    /*
    * a) We can finish and send the ResourceResponse
    * b) We will add the current header to the HTTPHeaderMap of the ResourceResponse
    *
    * The HTTP standard requires to use \r\n but for compatibility it recommends to
    * accept also \n.
    */
    if (header == String("\r\n") || header == String("\n")) {       
        setResponseDataToJobWhenDidReceiveResponseOnMainThread(job, args);
        return totalSize;
    } else {
        int splitPos = header.find(":");
        if (splitPos != -1) {
            String key = header.left(splitPos).stripWhiteSpace();
            String value = header.substring(splitPos + 1).stripWhiteSpace();

            if (isAppendableHeader(key))
                job->m_response.addHTTPHeaderField(key, value);
            else
                job->m_response.setHTTPHeaderField(key, value);
        } else if (header.startsWith("HTTP", WTF::TextCaseInsensitive)) {
            // This is the first line of the response.
            // Extract the http status text from this.
            //
            // If the FOLLOWLOCATION option is enabled for the curl handle then
            // curl will follow the redirections internally. Thus this header callback
            // will be called more than one time with the line starting "HTTP" for one job.
            String httpCodeString = String::number(args->httpCode);
            if (job->m_isProxy && 0 == args->httpCode)
                httpCodeString = "200";
            int statusCodePos = header.find(httpCodeString);

            if (statusCodePos != -1) {
                // The status text is after the status code.
                String status = header.substring(statusCodePos + httpCodeString.length());
                job->m_response.setHTTPStatusText(status.stripWhiteSpace());
            }
        }
    }

    return totalSize;
}

void WebURLLoaderManagerMainTask::handleHookRequestOnMainThread(WebURLLoaderInternal* job)
{
    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    content::WebPage* page = requestExtraData->page;
    if (page->wkeHandler().loadUrlEndCallback) {
        Vector<char> urlBuf = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);
        page->wkeHandler().loadUrlEndCallback(page->wkeWebView(), page->wkeHandler().loadUrlEndCallbackParam,
            urlBuf.data(), job,
            job->m_hookBuf, job->m_hookLength);
    }
}

// called with data after all headers have been processed via headerCallbackOnIoThread
static size_t writeCallbackOnIoThread(void* ptr, size_t size, size_t nmemb, void* data)
{
    int jobId = (int)data;
    AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
    WebURLLoaderInternal* job = autoLockJob.lock();
    if (!job)
        return 0;

    if (job->m_cancelled)
        return 0;

    // We should never be called when deferred loading is activated.
    ASSERT(!job->m_defersLoading);

    size_t totalSize = size * nmemb;
    // this shouldn't be necessary but apparently is. CURL writes the data
    // of html page even if it is a redirect that was handled internally
    // can be observed e.g. on gmail.com
    long httpCode = 0;
    CURLcode err = curl_easy_getinfo(job->m_handle, !job->m_isProxy ? CURLINFO_RESPONSE_CODE : CURLINFO_HTTP_CONNECTCODE, &httpCode);
    if (CURLE_OK == err && httpCode >= 300 && httpCode < 400)
        return totalSize;

    WebURLLoaderManagerMainTask::pushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kWriteCallback, ptr, size, nmemb, totalSize);
    return totalSize;
}

static bool checkIsProxyHead(WebURLLoaderInternal* job, char* ptr, size_t size)
{
    if (0 == size || !job->m_isProxy)
        return false;

    Vector<char> buffer;
    buffer.resize(size + 1);
    memset(buffer.data(), 0, buffer.size());
    memcpy(buffer.data(), ptr, size);

    if ((2 == size && buffer[0] == '\r' && buffer[1] == '\n') || (1 == size && buffer[0] == '\n')) {
        job->m_isProxyHeadRequest = false;
        return true;
    }

    if (job->m_isProxyHeadRequest)
        return true;

    job->m_isProxyHeadRequest = (0 != strstr(buffer.data(), "Connection Established"));
    return job->m_isProxyHeadRequest;
}

static size_t headerCallbackOnIoThread(char* ptr, size_t size, size_t nmemb, void* data)
{
    int jobId = (int)data;
    AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
    WebURLLoaderInternal* job = autoLockJob.lock();
    if (!job || job->m_cancelled)
        return 0;

    // We should never be called when deferred loading is activated.
    ASSERT(!job->m_defersLoading);

    size_t totalSize = size * nmemb;
    if (checkIsProxyHead(job, ptr, totalSize))
        return totalSize;

    WebURLLoaderManagerMainTask::pushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kHeaderCallback, ptr, size, nmemb, totalSize);
    return totalSize;
}

size_t readCallbackOnIoThread(void* ptr, size_t size, size_t nmemb, void* data)
{
    int jobId = (int)data;
    AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
    WebURLLoaderInternal* job = autoLockJob.lock();
    if (!job || job->m_cancelled)
        return 0;

    // We should never be called when deferred loading is activated.
    ASSERT(!job->m_defersLoading);

    if (!size || !nmemb)
        return 0;

    //     if (!job->m_formDataStream.hasMoreElements())
    //         return 0;
    // 
    //     size_t sent = job->m_formDataStream.read(ptr, size, nmemb);
    // 
    //     // Something went wrong so cancel the job.
    //     if (!sent && job->loader())
    //         job->loader()->cancel();
    // 
    //     return sent;

    size_t sentSize = job->m_postBytes.size() - job->m_postBytesReadOffset;
    if (0 == sentSize)
        return 0;

    if (size * nmemb <= sentSize)
        sentSize = size * nmemb;

    memcpy(ptr, job->m_postBytes.data() + job->m_postBytesReadOffset, sentSize);
    job->m_postBytesReadOffset += sentSize;
    ASSERT(job->m_postBytesReadOffset <= job->m_postBytes.size());

    return sentSize;
}

bool WebURLLoaderManager::downloadOnIoThread()
{
    if (m_isShutdown)
        return false;

    fd_set fdread;
    fd_set fdwrite;
    fd_set fdexcep;
    int maxfd = 0;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = selectTimeoutMS * 1000;       // select waits microseconds
                                                    // Retry 'select' if it was interrupted by a process signal.
    int rc = 0;
    do {
        FD_ZERO(&fdread);
        FD_ZERO(&fdwrite);
        FD_ZERO(&fdexcep);
        curl_multi_fdset(m_curlMultiHandle, &fdread, &fdwrite, &fdexcep, &maxfd);
        // When the 3 file descriptors are empty, winsock will return -1
        // and bail out, stopping the file download. So make sure we
        // have valid file descriptors before calling select.
        if (maxfd >= 0)
            rc = ::select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
    } while (rc == -1 && errno == EINTR);

    if (-1 == rc) {
#ifndef NDEBUG
        perror("bad: select() returned -1: ");
#endif
        return false;
    }

    int runningHandles = 0;
    while (curl_multi_perform(m_curlMultiHandle, &runningHandles) == CURLM_CALL_MULTI_PERFORM) {}

    // check the curl messages indicating completed transfers
    // and free their resources
    while (true) {
        int messagesInQueue;
        CURLMsg* msg = curl_multi_info_read(m_curlMultiHandle, &messagesInQueue);
        if (!msg)
            break;

        // find the node which has same job->m_handle as completed transfer
        CURL* handle = msg->easy_handle;
        ASSERT(handle);
        int jobId = 0;
        CURLcode err = curl_easy_getinfo(handle, CURLINFO_PRIVATE, &jobId);
        ASSERT_UNUSED(err, CURLE_OK == err);
        ASSERT(jobId > 0);
        AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
        WebURLLoaderInternal* job = autoLockJob.lock();
        if (!job)
            continue;

        ASSERT(job->m_handle == handle);
        if (job->m_cancelled) {
            removeFromCurlOnIoThread(jobId);
            continue;
        }

        if (CURLMSG_DONE != msg->msg)
            continue;

        if (CURLE_OK == msg->data.result) {
#if ENABLE(WEB_TIMING)
            calculateWebTimingInformations(job);
#endif
            if (!job->responseFired()) {
                WebURLLoaderManagerMainTask::pushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kHandleLocalReceiveResponse, nullptr, 0, 0, 0);
                if (job->m_cancelled) {
                    removeFromCurlOnIoThread(jobId);
                    continue;
                }
            }

            if (job->m_isHookRequest)
                WebURLLoaderManagerMainTask::pushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kHandleHookRequest, nullptr, 0, 0, 0);

            if (job->m_multipartHandle) {
                WebURLLoaderManagerMainTask::pushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kContentEnded, nullptr, 0, 0, 0);
            } else if (job->client() && job->loader()) {
                WebURLLoaderManagerMainTask::pushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kDidFinishLoading, nullptr, 0, 0, 0);
            }
        } else {
            char* url = 0;
            curl_easy_getinfo(job->m_handle, CURLINFO_EFFECTIVE_URL, &url);
            if (job->client() && job->loader()) {

                WebURLLoaderManagerMainTask::Args* args = WebURLLoaderManagerMainTask::pushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kDidFail, nullptr, 0, 0, 0);
                args->resourceError->reason = msg->data.result;
                args->resourceError->domain = WebString::fromLatin1(url);
                args->resourceError->localizedDescription = WebString::fromLatin1(curl_easy_strerror(msg->data.result));

                String outString = String::format("kDidFail on io Thread:%d, %s\n", msg->data.result, url);
                OutputDebugStringW(outString.charactersWithNullTermination().data());
            }
        }
        
        removeFromCurlOnIoThread(jobId);
    }

    return (runningHandles > 0); // 如果还有请求未处理则返回true,下个timer继续处理
}

void WebURLLoaderManager::setProxyInfo(const String& host, unsigned long port, ProxyType type, const String& username, const String& password)
{
    m_proxyType = type;

    if (!host.length()) {
        m_proxy = emptyString();
    } else {
        String userPass;
        if (username.length() || password.length())
            userPass = username + ":" + password + "@";

        m_proxy = userPass + host + ":" + String::number(port);
        if (!m_proxy.startsWith("https://") && !m_proxy.startsWith("http://"))
            m_proxy.insert("http://", 0);
    }
}

void WebURLLoaderManager::removeFromCurlOnIoThread(int jobId)
{
    AutoLockJob autoLockJob(this, jobId);
    WebURLLoaderInternal* job = autoLockJob.lock();
    if (!job)
        return;

    WTF::Locker<WTF::Mutex> locker(job->m_destroingMutex);

    WebURLLoaderInternal::State state = job->m_state;
    ASSERT(WebURLLoaderInternal::kDestroyed != state);

    job->m_state = WebURLLoaderInternal::kDestroying;
    if (WebURLLoaderInternal::kNormal == state) {
        m_runningJobs--;

        if (!job->m_isWkeNetSetDataBeSetted && !job->m_isBlackList && !job->m_isDataUrl) {
            ASSERT(job->m_handle);
            WebURLLoaderManagerMainTask* task = WebURLLoaderManagerMainTask::createTask(jobId, WebURLLoaderManagerMainTask::TaskType::kRemoveFromCurl, nullptr, 0, 0, 0);
                        
            if (job->m_handle) {
                curl_multi_remove_handle(m_curlMultiHandle, job->m_handle);
                curl_easy_cleanup(job->m_handle);
            }
            job->m_handle = nullptr;
            Platform::current()->mainThread()->scheduler()->postLoadingTask(FROM_HERE, task); // postLoadingTask
        }
    }
}

static inline size_t getFormElementsCount(WebURLLoaderInternal* job)
{
    WebHTTPBody httpBody = job->firstRequest()->httpBody();
    if (httpBody.isNull())
        return 0;

    // Resolve the blob elements so the formData can correctly report it's size.
    //formData = formData->resolveBlobReferences();
    //job->firstRequest().setHTTPBody(httpBody);

    return httpBody.elementCount();
}

struct SetupDataInfo {
    CURLoption sizeOption;
    curl_off_t size;
    bool islongLong;
};

struct SetupInfoBase {
    SetupDataInfo* data;

    SetupInfoBase() { data = nullptr; }

    ~SetupInfoBase()
    {
        if (data)
            delete data;
    }
};

struct SetupPutInfo : public SetupInfoBase {
};

struct SetupPostInfo : public SetupInfoBase {
};

struct SetupHttpMethodInfo {
    SetupHttpMethodInfo()
    {
        put = nullptr;
        post = nullptr;
    }

    ~SetupHttpMethodInfo()
    {
        if (put)
            delete put;
        if (post)
            delete post;
    }
    SetupPutInfo* put;
    SetupPostInfo* post;
};

static void setupFormDataOnIoThread(WebURLLoaderInternal* job, SetupDataInfo* info)
{
    if (info) {
        if (info->islongLong)
            curl_easy_setopt(job->m_handle, info->sizeOption, (long long)info->size);
        else
            curl_easy_setopt(job->m_handle, info->sizeOption, (int)info->size);
    }

    curl_easy_setopt(job->m_handle, CURLOPT_READFUNCTION, readCallbackOnIoThread);
    curl_easy_setopt(job->m_handle, CURLOPT_READDATA, job);
}

SetupDataInfo* setupFormDataOnMainThread(WebURLLoaderInternal* job, CURLoption sizeOption, struct curl_slist** headers)
{
    WebHTTPBody httpBody = job->firstRequest()->httpBody();

    // The size of a curl_off_t could be different in WebKit and in cURL depending on
    // compilation flags of both.
    static int expectedSizeOfCurlOffT = 0;
    if (!expectedSizeOfCurlOffT) {
        curl_version_info_data *infoData = curl_version_info(CURLVERSION_NOW);
        if (infoData->features & CURL_VERSION_LARGEFILE)
            expectedSizeOfCurlOffT = sizeof(long long);
        else
            expectedSizeOfCurlOffT = sizeof(int);
    }

    static const long long maxCurlOffT = (1LL << (expectedSizeOfCurlOffT * 8 - 1)) - 1;
    // Obtain the total size of the form data
    curl_off_t size = 0;
    bool chunkedTransfer = false;
    for (size_t i = 0; i < httpBody.elementCount(); ++i) {
        WebHTTPBody::Element element;
        if (!httpBody.elementAt(i, element))
            continue;

        if (WebHTTPBody::Element::TypeFile == element.type) {
            // long long fileSizeResult;
            //             if (getFileSize(element.m_filename, fileSizeResult)) {
            //                 if (fileSizeResult > maxCurlOffT) {
            //                     // File size is too big for specifying it to cURL
            //                     chunkedTransfer = true;
            //                     break;
            //                 }
            //                 size += fileSizeResult;
            //             } else {
            //                 chunkedTransfer = true;
            //                 break;
            //             }
        } else
            size += element.data.size();
    }

    SetupDataInfo* result = nullptr;
    // cURL guesses that we want chunked encoding as long as we specify the header
    if (chunkedTransfer)
        *headers = curl_slist_append(*headers, "Transfer-Encoding: chunked");
    else {
        result = new SetupDataInfo();
        result->sizeOption = sizeOption;
        result->size = size;
        result->islongLong = (sizeof(long long) == expectedSizeOfCurlOffT);
    }

    return result;
}

static void setupPutOnIoThread(WebURLLoaderInternal* job, SetupPutInfo* info)
{
    curl_easy_setopt(job->m_handle, CURLOPT_UPLOAD, TRUE);
    curl_easy_setopt(job->m_handle, CURLOPT_INFILESIZE, 0);

    if (!info)
        return;

    if (info->data)
        setupFormDataOnIoThread(job, info->data);
}

static SetupPutInfo* setupPutOnMainThread(WebURLLoaderInternal* job, struct curl_slist** headers)
{
    // Disable the Expect: 100 continue header
    *headers = curl_slist_append(*headers, "Expect:");

    size_t numElements = getFormElementsCount(job);
    if (!numElements)
        return nullptr;

    SetupPutInfo* result = new SetupPutInfo();
    result->data = setupFormDataOnMainThread(job, CURLOPT_INFILESIZE_LARGE, headers);
    return result;
}

static void flattenHttpBody(const WebHTTPBody& httpBody, WTF::Vector<char>* data)
{
    for (size_t i = 0; i < httpBody.elementCount(); ++i) {
        WebHTTPBody::Element element;
        if (!httpBody.elementAt(i, element))
            continue;

        if (WebHTTPBody::Element::TypeData == element.type) {
            data->append(element.data.data(), element.data.size());
        } else if (WebHTTPBody::Element::TypeBlob == element.type) {
            WebBlobRegistryImpl* blobReg = (WebBlobRegistryImpl*)blink::Platform::current()->blobRegistry();
            net::BlobDataWrap* blobData = blobReg->getBlobDataFromUUID(element.blobUUID);
            if (!blobData)
                continue;

            const Vector<blink::WebBlobData::Item*>& items = blobData->items();
            for (size_t i = 0; i < items.size(); ++i) {
                blink::WebBlobData::Item* item = items[i];
                data->append(item->data.data(), item->data.size());
            }
        }
    }
}

static void setupPostOnIoThread(WebURLLoaderInternal* job, SetupPostInfo* info)
{
    curl_easy_setopt(job->m_handle, CURLOPT_POST, true);

    if (!info) {
        curl_easy_setopt(job->m_handle, CURLOPT_POSTFIELDSIZE, 0);
        return;
    }

    if (0 != job->m_postBytes.size()) {
        curl_easy_setopt(job->m_handle, CURLOPT_POSTFIELDSIZE, job->m_postBytes.size());
        curl_easy_setopt(job->m_handle, CURLOPT_POSTFIELDS, job->m_postBytes.data());
    }

//     if (info->data)
//         setupFormDataOnIoThread(job, info->data);
}

static SetupPostInfo* setupPostOnMainThread(WebURLLoaderInternal* job, struct curl_slist** headers)
{
    size_t numElements = getFormElementsCount(job);
    if (!numElements)
        return nullptr;

    SetupPostInfo* result = new SetupPostInfo();

    // Do not stream for simple POST data
//     if (numElements == 1) {
//         flattenHttpBody(job->firstRequest()->httpBody(), &job->m_postBytes);
//         return result;
//     }

    flattenHttpBody(job->firstRequest()->httpBody(), &job->m_postBytes);
    result->data = setupFormDataOnMainThread(job, CURLOPT_POSTFIELDSIZE_LARGE, headers);
    return result;
}

class WebURLLoaderManager::IoTask : public WebThread::Task {
public:
    IoTask(WebURLLoaderManager* manager, blink::WebThread* thread, bool start)
        : m_manager(manager)
        , m_thread(thread)
        , m_start(start)
    {
    }

    ~IoTask() override
    {
    }

    virtual void run() override
    {
        if (!m_manager->downloadOnIoThread())
            return;

        IoTask* task = new IoTask(m_manager, m_thread, true);
        m_thread->postDelayedTask(FROM_HERE, task, 1);
    }

private:
    WebURLLoaderManager* m_manager;
    blink::WebThread* m_thread;
    bool m_start;
};

class WkeAsynTask : public WebThread::Task {
public:
    WkeAsynTask(WebURLLoaderManager* manager, int jobId)
    {
        m_manager = manager;
        m_jobId = jobId;
    }

    ~WkeAsynTask() override
    {
    }

    virtual void run() override
    {
        WebURLLoaderInternal* job = m_manager->checkJob(m_jobId);
        if (!job || job->m_cancelled)
            return;

        job->m_response.setURL(job->firstRequest()->url());
        job->client()->didReceiveResponse(job->loader(), job->m_response);
        if (job->m_asynWkeNetSetData && !job->m_cancelled) { // 可能在didReceiveResponse里被cancel
            WebURLLoaderManager::sharedInstance()->didReceiveDataOrDownload(job, static_cast<char*>(job->m_asynWkeNetSetData), job->m_asynWkeNetSetDataLength, 0);
            WebURLLoaderManager::sharedInstance()->handleDidFinishLoading(job, WTF::currentTime(), 0);
        }
    }

private:
    WebURLLoaderManager* m_manager;
    int m_jobId;
};

class BlackListCancelTask : public WebThread::Task {
public:
    BlackListCancelTask(WebURLLoaderManager* manager, int jobId)
    {
        m_manager = manager;
        m_jobId = jobId;
    }

    ~BlackListCancelTask() override
    {
    }

    static void cancel(WebURLLoaderInternal* job)
    {
        job->m_isBlackList = true;
        job->m_response.setURL(job->firstRequest()->url());
        job->client()->didReceiveResponse(job->loader(), job->m_response);
        if (!job->m_cancelled) { // 可能在didReceiveResponse里被cancel
            //WebURLLoaderManager::sharedInstance()->didReceiveDataOrDownload(job, static_cast<char*>(""), 0, 0);

            WebURLError error;
            error.domain = WebString(String(job->m_url));
            error.reason = -1;
            error.localizedDescription = WebString::fromUTF8("black list");
            WebURLLoaderManager::sharedInstance()->handleDidFail(job, error);
            RELEASE_ASSERT(job->m_cancelled);
        }
    }

    virtual void run() override
    {
        WebURLLoaderInternal* job = m_manager->checkJob(m_jobId);
        if (!job || job->m_cancelled)
            return;

        cancel(job);
    }

private:
    WebURLLoaderManager* m_manager;
    int m_jobId;
};

AutoLockJob::AutoLockJob(WebURLLoaderManager* manager, int jobId)
{
    m_manager = manager;
    m_jobId = jobId;
    m_isNotDerefForDelete = false;
}

WebURLLoaderInternal* AutoLockJob::lock()
{
    if (!m_manager)
        return nullptr;

    WebURLLoaderInternal* job = m_manager->checkJob(m_jobId);
    if (!job)
        return nullptr;

    job->ref();
    return job;
}

void AutoLockJob::setNotDerefForDelete()
{
    m_isNotDerefForDelete = true;
}

AutoLockJob::~AutoLockJob()
{
    if (m_isNotDerefForDelete || !m_manager)
        return;
    WebURLLoaderInternal* job = m_manager->checkJob(m_jobId);
    if (job)
        job->deref();
}

WebURLLoaderInternal* WebURLLoaderManager::checkJob(int jobId)
{
    WTF::Locker<WTF::Mutex> locker(m_liveJobsMutex);

    WTF::HashMap<int, WebURLLoaderInternal*>::iterator it = m_liveJobs.find(jobId);
    if (it == m_liveJobs.end())
        return nullptr;
    return it->value;
}

int WebURLLoaderManager::addLiveJobs(WebURLLoaderInternal* job)
{
    if (m_isShutdown)
        return 0;
    WTF::Locker<WTF::Mutex> locker(m_liveJobsMutex);

    int jobId = (++m_newestJobId);
    m_liveJobs.add(jobId, job);
    ASSERT(0 == job->m_id);
    job->m_id = jobId;
    return jobId;
}

void WebURLLoaderManager::removeLiveJobs(int jobId)
{
    if (m_isShutdown)
        return;
    WTF::Locker<WTF::Mutex> locker(m_liveJobsMutex);
    m_liveJobs.remove(jobId);
}

bool isBlackListUrl(const String& url)
{
    char* blackList[] = {
//         ".woff2",
        nullptr
    };

    const char* blackFile = nullptr;
    int i = 0;
    for (blackFile = blackList[0]; blackFile; blackFile = blackList[i]) {
        if (WTF::kNotFound != url.find(blackFile))
            return true;
        ++i;
    }

    return false;
}

class HandleDataURLTask : public WebThread::Task {
public:
    HandleDataURLTask(WebURLLoaderManager* manager, int jobId)
    {
        m_manager = manager;
        m_jobId = jobId;
    }

    ~HandleDataURLTask() override
    {
    }

    virtual void run() override
    {
        WebURLLoaderInternal* job = m_manager->checkJob(m_jobId);
        if (!job || job->m_cancelled)
            return;

        KURL url = job->firstRequest()->url();
        handleDataURL(job->loader(), job->client(), url);
    }

private:
    WebURLLoaderManager* m_manager;
    int m_jobId;
};

static bool isLocalFileNotExist(const char* urlTrim, WebURLLoaderInternal* job)
{
    // 有外部hook，则不走快速判断流程
    if (g_pfnOpen)
        return false;

    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    if (!requestExtraData)
        return false;

    WebPage* page = requestExtraData->page;
    if (!page->wkeHandler().loadUrlBeginCallback)
        return false;

    String url(urlTrim);
    if (url.startsWith("file:///"))
        url.remove(0, sizeof("file:///") - 1);
    url.replace("/", "\\");

    bool result = false;
    Vector<UChar> buf = WTF::ensureUTF16UChar(url, true);
    result = !::PathFileExistsW(buf.data());
    if (result) {
        String outString = String::format("isLocalFileNotExist: %s\n", WTF::ensureStringToUTF8(url, true).data());
        OutputDebugStringW(outString.charactersWithNullTermination().data());
    }
    return result;
}

int WebURLLoaderManager::addAsynchronousJob(WebURLLoaderInternal* job)
{
    ASSERT(WTF::isMainThread());

    KURL kurl = job->firstRequest()->url();
    String url = WTF::ensureStringToUTF8String(kurl.string());
#if 0
    String outString = String::format("addAsynchronousJob : %d, %s\n", m_liveJobs.size(), WTF::ensureStringToUTF8(url, true).data());
    OutputDebugStringW(outString.charactersWithNullTermination().data());

    if (WTF::kNotFound != url.find("ensearch=1"))
        OutputDebugStringA("ensearch=1!\n");
#endif

    String referer = job->firstRequest()->httpHeaderField(WebString::fromUTF8("referer"));
    job->m_manager = this;

    int jobId = 0;
    if (isBlackListUrl(url)) {
        jobId = addLiveJobs(job);
        Platform::current()->currentThread()->scheduler()->postLoadingTask(FROM_HERE, new BlackListCancelTask(this, jobId)); // postLoadingTask
        return jobId;
    }

    if (kurl.protocolIsData()) {
        jobId = addLiveJobs(job);
        job->m_isDataUrl = true;
        Platform::current()->currentThread()->scheduler()->postLoadingTask(FROM_HERE, new HandleDataURLTask(this, jobId)); // postLoadingTask
        return jobId;
    }

    jobId = startJobOnMainThread(job);
    if (0 == jobId)
        return 0;

    if (job->m_isWkeNetSetDataBeSetted) {
        Platform::current()->currentThread()->scheduler()->postLoadingTask(FROM_HERE, new WkeAsynTask(this, jobId)); // postLoadingTask
        return jobId;
    }

    IoTask* task = new IoTask(this, m_thread, false);
    m_thread->postTask(FROM_HERE, task);
    return jobId;
}

void WebURLLoaderManager::doCancel(int jobId, WebURLLoaderInternal* job)
{
    WTF::Locker<WTF::Mutex> locker(job->m_destroingMutex);
    bool cancelled = job->m_cancelled;
    job->m_cancelled = true;
    if (!cancelled && job->m_state != WebURLLoaderInternal::kDestroying)
        m_thread->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::removeFromCurlOnIoThread, this, jobId));
}

void WebURLLoaderManager::cancel(int jobId)
{
    ASSERT(WTF::isMainThread());

    AutoLockJob autoLockJob(this, jobId);
    WebURLLoaderInternal* job = autoLockJob.lock();
    if (!job)
        return;

    doCancel(jobId, job);
}

void WebURLLoaderManager::cancelAll()
{
    WTF::Locker<WTF::Mutex> locker(m_liveJobsMutex);

    WTF::HashMap<int, WebURLLoaderInternal*>::iterator it = m_liveJobs.begin();
    for (; it != m_liveJobs.end(); ++it) {
        WebURLLoaderInternal* job = it->value;
        int jobId = it->key;
        doCancel(jobId, job);
    }
}

void WebURLLoaderManager::dispatchSynchronousJob(WebURLLoaderInternal* job)
{
    ASSERT(WTF::isMainThread());
    job->m_manager = this;
    job->m_isSynchronous = true;

    int jobId = addLiveJobs(job);

    KURL url = job->firstRequest()->url();
    if (url.protocolIsData() && job->client()) {
        handleDataURL(job->loader(), job->client(), url);
        delete job;
        return;
    }

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    WebPage* page = requestExtraData->page;
    if (page->wkeHandler().loadUrlBeginCallback) {
        Vector<char> url = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);
        if (page->wkeHandler().loadUrlBeginCallback(page->wkeWebView(), page->wkeHandler().loadUrlBeginCallbackParam,
            url.data(), job)) {
            WebURLLoaderManager::sharedInstance()->handleDidFinishLoading(job, WTF::currentTime(), 0);
            delete job;
            return;
        }
    }
#endif

    // If defersLoading is true and we call curl_easy_perform
    // on a paused handle, libcURL would do the transfert anyway
    // and we would assert so force defersLoading to be false.
    job->m_defersLoading = false;

    InitializeHandleInfo* info = preInitializeHandleOnMainThread(job);
    m_thread->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::initializeHandleOnIoThread, this, jobId, info));

    int isCallFinish = 0;
    CURLcode ret = CURLE_OK;
    m_thread->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::dispatchSynchronousJobOnIoThread, this, job, info, &ret, &isCallFinish));
    while (!isCallFinish) { ::Sleep(50); }

    for (size_t i = 0; i < job->m_syncTasks.size(); ++i) {
        WebURLLoaderManagerMainTask* task = job->m_syncTasks[i];
        task->run();
        delete task;
    }

    if (ret != CURLE_OK) {
        if (job->client() && job->loader()) {
            WebURLError error;
            error.domain = WebString(String(job->m_url));
            error.reason = ret;
            error.localizedDescription = WebString(String(curl_easy_strerror(ret)));
            WebURLLoaderManager::sharedInstance()->handleDidFail(job, error);
        }
    } else {
        if (job->client() && job->loader())
            handleDidReceiveResponse(job);
    }

    removeLiveJobs(jobId);
    delete job;
}

void WebURLLoaderManager::dispatchSynchronousJobOnIoThread(WebURLLoaderInternal* job, InitializeHandleInfo* info, CURLcode* ret, int* isCallFinish)
{
    // curl_easy_perform blocks until the transfert is finished.
    *ret = curl_easy_perform(job->m_handle);
    curl_easy_cleanup(job->m_handle);

    *isCallFinish = true;
}

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
static bool dispatchWkeLoadUrlBegin(WebURLLoaderInternal* job)
{
    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    if (!requestExtraData)
        return false;

    WebPage* page = requestExtraData->page;
    if (!page->wkeHandler().loadUrlBeginCallback)
        return false;

    Vector<char> urlBuf = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);
    if (!page->wkeHandler().loadUrlBeginCallback(page->wkeWebView(),
        page->wkeHandler().loadUrlBeginCallbackParam,
        urlBuf.data(), job))
        return false;

    return true;
}
#endif

int WebURLLoaderManager::startJobOnMainThread(WebURLLoaderInternal* job)
{
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (dispatchWkeLoadUrlBegin(job))
        return addLiveJobs(job);
#endif

    return initializeHandleOnMainThread(job);
}

void WebURLLoaderManager::applyAuthenticationToRequest(WebURLLoaderInternal* handle, blink::WebURLRequest* request)
{
    // m_user/m_pass are credentials given manually, for instance, by the arguments passed to XMLHttpRequest.open().
    WebURLLoaderInternal* job = handle;
    // 
    //     if (handle->shouldUseCredentialStorage()) {
    //         if (job->m_user.isEmpty() && job->m_pass.isEmpty()) {
    //             // <rdar://problem/7174050> - For URLs that match the paths of those previously challenged for HTTP Basic authentication, 
    //             // try and reuse the credential preemptively, as allowed by RFC 2617.
    //             job->m_initialCredential = CredentialStorage::defaultCredentialStorage().get(request.url());
    //         } else {
    //             // If there is already a protection space known for the KURL, update stored credentials
    //             // before sending a request. This makes it possible to implement logout by sending an
    //             // XMLHttpRequest with known incorrect credentials, and aborting it immediately (so that
    //             // an authentication dialog doesn't pop up).
    //             CredentialStorage::defaultCredentialStorage().set(Credential(job->m_user, job->m_pass, CredentialPersistenceNone), request.url());
    //         }
    //     }
    // 
    //     String user = job->m_user;
    //     String password = job->m_pass;
    // 
    //     if (!job->m_initialCredential.isEmpty()) {
    //         user = job->m_initialCredential.user();
    //         password = job->m_initialCredential.password();
    //         curl_easy_setopt(job->m_handle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    //     }
    // 
    //     // It seems we need to set CURLOPT_USERPWD even if username and password is empty.
    //     // Otherwise cURL will not automatically continue with a new request after a 401 response.
    // 
    //     // curl CURLOPT_USERPWD expects username:password
    //     String userpass = user + ":" + password;
    //     curl_easy_setopt(job->m_handle, CURLOPT_USERPWD, userpass.utf8().data());

    curl_easy_setopt(job->m_handle, CURLOPT_USERPWD, ":");
}

class HeaderVisitor : public blink::WebHTTPHeaderVisitor {
public:
    explicit HeaderVisitor(curl_slist** headers) : m_headers(headers) {}

    virtual void visitHeader(const WebString& webName, const WebString& webValue) override
    {
        String value = webValue;
        String headerString(webName);
        if (value.isNull() || value.isEmpty())
            // Insert the ; to tell curl that this header has an empty value.
            headerString.append(";");
        else {
            headerString.append(": ");
            headerString.append(value);
        }
        CString headerLatin1 = headerString.latin1();
        *m_headers = curl_slist_append(*m_headers, headerLatin1.data());
    }

    curl_slist* headers() { return*m_headers; }
private:
    curl_slist** m_headers;
};

struct WebURLLoaderManager::InitializeHandleInfo {
    std::string url;
    std::string method;
    curl_slist* headers;
    std::string proxy;
    std::string wkeNetInterface;
    ProxyType proxyType;
    SetupHttpMethodInfo* methodInfo;

    InitializeHandleInfo()
    {
        methodInfo = nullptr;
    }

    ~InitializeHandleInfo()
    {
        if (methodInfo) {
            delete methodInfo;
        }
    }
};

WebURLLoaderManager::InitializeHandleInfo* WebURLLoaderManager::preInitializeHandleOnMainThread(WebURLLoaderInternal* job)
{
    InitializeHandleInfo* info = new InitializeHandleInfo();
    KURL url = job->firstRequest()->url();
    String urlString = url.string();

    // Remove any fragment part, otherwise curl will send it as part of the request.
    url.removeFragmentIdentifier();
    if (url.isLocalFile()) {
        // Remove any query part sent to a local file.
        if (!url.query().isEmpty()) {
            // By setting the query to a null string it'll be removed.
            url.setQuery(String());
            urlString = url.string();
        }

        // Determine the MIME type based on the path.
        job->m_response.setMIMEType(MIMETypeRegistry::getMIMETypeForPath(url));
    }

    info->url = WTF::ensureStringToUTF8(urlString, true).data();
    //ASSERT(info->url.size() == urlString.length());
#if 0
    String output = String::format("preInit: %d %s\n", urlString.length(), info->url.c_str());
    OutputDebugStringA(output.utf8().data());
#endif
    info->method = job->firstRequest()->httpMethod().utf8();

    curl_slist* headers = nullptr;
    HeaderVisitor visitor(&headers);
    job->firstRequest()->visitHTTPHeaderFields(&visitor);

    String method = job->firstRequest()->httpMethod();
    if ("GET" == method) {

    } else if ("POST" == method) {
        info->methodInfo = new SetupHttpMethodInfo();
        info->methodInfo->post = setupPostOnMainThread(job, &headers);
    } else if ("PUT" == method) {
        info->methodInfo = new SetupHttpMethodInfo();
        info->methodInfo->put = setupPutOnMainThread(job, &headers);
    } else if ("HEAD" == method) {

    } else {
        info->methodInfo = new SetupHttpMethodInfo();
        info->methodInfo->put = setupPutOnMainThread(job, &headers);
    }
    info->headers = headers;

    // Set proxy options if we have them.
    if (m_proxy.length()) {
        info->proxy = m_proxy.utf8().data();
        info->proxyType = m_proxyType;
    }

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    if (!requestExtraData) // 在退出时候js调用同步XHR请求，会导致ExtraData为0情况
        return info;

    WebPage* page = requestExtraData->page;
    if (!page->wkeWebView())
        return info;

    String wkeProxy = page->wkeWebView()->getProxy();
    if (0 != wkeProxy.length()) {
        info->proxy = wkeProxy.utf8().data();
        info->proxyType = page->wkeWebView()->getProxyType();
    }

    String wkeNetInterface = page->wkeWebView()->getNetInterface();
    if (0 != wkeNetInterface.length()) {
        info->wkeNetInterface = wkeNetInterface.utf8().data();
    }
#endif

    return info;
}

void WebURLLoaderManager::initializeHandleOnIoThread(int jobId, InitializeHandleInfo* info)
{
    AutoLockJob autoLockJob(this, jobId);
    WebURLLoaderInternal* job = autoLockJob.lock();
    if (!job)
        return;
    job->m_handle = curl_easy_init();

    if (job->m_defersLoading) {
        CURLcode error = curl_easy_pause(job->m_handle, CURLPAUSE_ALL);
        // If we did not pause the handle, we would ASSERT in the
        // header callback. So just assert here.
        ASSERT_UNUSED(error, error == CURLE_OK);
    }
#ifndef NDEBUG
    if (getenv("DEBUG_CURL"))
        curl_easy_setopt(job->m_handle, CURLOPT_VERBOSE, 1);
#endif
    curl_easy_setopt(job->m_handle, CURLOPT_TIMEOUT, 30000);
    curl_easy_setopt(job->m_handle, CURLOPT_SSL_VERIFYPEER, false); // ignoreSSLErrors
    curl_easy_setopt(job->m_handle, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(job->m_handle, CURLOPT_PRIVATE, jobId);
    curl_easy_setopt(job->m_handle, CURLOPT_ERRORBUFFER, m_curlErrorBuffer);
    curl_easy_setopt(job->m_handle, CURLOPT_WRITEFUNCTION, writeCallbackOnIoThread);
    curl_easy_setopt(job->m_handle, CURLOPT_WRITEDATA, jobId);
    curl_easy_setopt(job->m_handle, CURLOPT_HEADERFUNCTION, headerCallbackOnIoThread);
    curl_easy_setopt(job->m_handle, CURLOPT_WRITEHEADER, jobId);
    curl_easy_setopt(job->m_handle, CURLOPT_AUTOREFERER, 1);
    curl_easy_setopt(job->m_handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(job->m_handle, CURLOPT_MAXREDIRS, 10);
    curl_easy_setopt(job->m_handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(job->m_handle, CURLOPT_SHARE, m_curlShareHandle);
    curl_easy_setopt(job->m_handle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5); // 5 minutes
    curl_easy_setopt(job->m_handle, CURLOPT_PROTOCOLS, kAllowedProtocols);
    curl_easy_setopt(job->m_handle, CURLOPT_REDIR_PROTOCOLS, kAllowedProtocols);

    //curl_easy_setopt(job->m_handle, CURLOPT_HTTPPROXYTUNNEL, true); // TODO

    if (!m_certificatePath.isNull())
        curl_easy_setopt(job->m_handle, CURLOPT_CAINFO, m_certificatePath.data());

    // enable gzip and deflate through Accept-Encoding:
    curl_easy_setopt(job->m_handle, CURLOPT_ENCODING, "");

    // url must remain valid through the request
    ASSERT(!job->m_url);

    // url is in ASCII so latin1() will only convert it to char* without character translation.
    job->m_url = fastStrDup(info->url.c_str());

    KURL url = job->firstRequest()->url();
    String urlString = job->m_url;
    //ASSERT(url.string() == urlString);

    curl_easy_setopt(job->m_handle, CURLOPT_URL, job->m_url);

    if (m_cookieJarFileName)
        curl_easy_setopt(job->m_handle, CURLOPT_COOKIEJAR, m_cookieJarFileName);

    if ("GET" == info->method) {
        curl_easy_setopt(job->m_handle, CURLOPT_HTTPGET, TRUE);
    } else if ("POST" == info->method) {
        setupPostOnIoThread(job, info->methodInfo->post);
    } else if ("PUT" == info->method) {
        setupPutOnIoThread(job, info->methodInfo->put);
    } else if ("HEAD" == info->method)
        curl_easy_setopt(job->m_handle, CURLOPT_NOBODY, TRUE);
    else {
        curl_easy_setopt(job->m_handle, CURLOPT_CUSTOMREQUEST, info->method.c_str());
    }

    if (info->headers) {
        curl_easy_setopt(job->m_handle, CURLOPT_HTTPHEADER, info->headers);
        job->m_customHeaders = info->headers;
    }

    curl_easy_setopt(job->m_handle, CURLOPT_USERPWD, ":");

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (info->proxy.size()) {
        job->m_isProxy = true;
        curl_easy_setopt(job->m_handle, CURLOPT_PROXY, info->proxy.c_str());
        curl_easy_setopt(job->m_handle, CURLOPT_PROXYTYPE, info->proxyType);
    }

    if (info->wkeNetInterface.size())
        curl_easy_setopt(job->m_handle, CURLOPT_INTERFACE, info->wkeNetInterface.c_str());
#endif

    delete info;
}

void WebURLLoaderManager::timeoutOnMainThread(int jobId)
{
    AutoLockJob autoLockJob(this, jobId);
    WebURLLoaderInternal* job = autoLockJob.lock();
    if (!job)
        return;

    OutputDebugStringW(L"timeoutOnMainThread:");
    KURL kUrl = job->firstRequest()->url();
    OutputDebugStringA(kUrl.string().utf8().data());
    OutputDebugStringW(L"\n");

    BlackListCancelTask::cancel(job);
    cancel(jobId);
}

int WebURLLoaderManager::initializeHandleOnMainThread(WebURLLoaderInternal* job)
{
    int jobId = addLiveJobs(job);

    InitializeHandleInfo* info = preInitializeHandleOnMainThread(job);
    KURL kurl = job->firstRequest()->url();
    if (kurl.isLocalFile() && isLocalFileNotExist(info->url.c_str(), job)) {
        Platform::current()->currentThread()->scheduler()->postLoadingTask(FROM_HERE, new BlackListCancelTask(this, jobId));
        return jobId;
    }

    m_thread->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::initializeHandleOnIoThread, this, jobId, info));
    m_thread->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::startOnIoThread, this, jobId));

    return jobId;
}

void WebURLLoaderManager::startOnIoThread(int jobId)
{
    AutoLockJob autoLockJob(this, jobId);
    WebURLLoaderInternal* job = autoLockJob.lock();
    if (!job)
        return;

    //Platform::current()->mainThread()->postDelayedTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::timeoutOnMainThread, this, jobId), 5000);

    m_runningJobs++;
    CURLMcode ret = curl_multi_add_handle(m_curlMultiHandle, job->m_handle);
    // don't call perform, because events must be async
    // timeout will occur and do curl_multi_perform
    if (ret && ret != CURLM_CALL_MULTI_PERFORM) {
#ifndef NDEBUG
        //         WTF::String outstr = String::format("Error %job starting job %s\n", ret, encodeWithURLEscapeSequences(job->firstRequest()->url().string()).latin1().data());
        //         OutputDebugStringW(outstr.charactersWithNullTermination().data());
#endif
        WTF::Locker<WTF::Mutex> locker(job->m_destroingMutex);
        job->m_cancelled = true;
        removeFromCurlOnIoThread(jobId);

        curl_easy_setopt(job->m_handle, CURLOPT_PRIVATE, nullptr);
        curl_easy_setopt(job->m_handle, CURLOPT_ERRORBUFFER, nullptr);
        curl_easy_setopt(job->m_handle, CURLOPT_WRITEDATA, nullptr);
        curl_easy_setopt(job->m_handle, CURLOPT_WRITEHEADER, nullptr);
        curl_easy_setopt(job->m_handle, CURLOPT_SHARE, nullptr);
    }
}

WebURLLoaderInternal::WebURLLoaderInternal(WebURLLoaderImplCurl* loader, const WebURLRequest& request, WebURLLoaderClient* client, bool defersLoading, bool shouldContentSniff)
    : m_ref(0)
    , m_id(0)
    , m_isSynchronous(false)
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
    , m_state(kNormal)
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    , m_hookBuf(nullptr)
    , m_hookLength(0)
    , m_isHookRequest(false)
    , m_asynWkeNetSetData(nullptr)
    , m_asynWkeNetSetDataLength(0)
    , m_isWkeNetSetDataBeSetted(false)
#endif
    , m_bodyStreamWriter(nullptr)
{
    m_firstRequest = new blink::WebURLRequest(request);
    KURL url = (KURL)m_firstRequest->url();
    m_user = url.user();
    m_pass = url.pass();

    m_response.initialize();

    m_dataLength = 0;
    m_isBlackList = false;
    m_isDataUrl = false;
    m_isProxy = false;
    m_isProxyHeadRequest = false;
    m_postBytesReadOffset = 0;

#ifndef NDEBUG
    webURLLoaderInternalCounter.increment();
#endif
}

WebURLLoaderInternal::~WebURLLoaderInternal()
{
    m_state = kDestroyed;
    delete m_firstRequest;

    fastFree(m_url);
    if (m_customHeaders)
        curl_slist_free_all(m_customHeaders);

    if (m_bodyStreamWriter) {
        delete m_bodyStreamWriter;
        m_bodyStreamWriter = nullptr;
    }

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (m_hookBuf)
        free(m_hookBuf);
    if (m_asynWkeNetSetData)
        free(m_asynWkeNetSetData);
#endif

#ifndef NDEBUG
    webURLLoaderInternalCounter.decrement();
#endif
}

} // namespace net
;