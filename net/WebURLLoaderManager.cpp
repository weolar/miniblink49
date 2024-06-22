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
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "content/browser/WebFrameClientImpl.h"
#include "content/browser/WebPage.h"
#include "net/WebURLLoaderInternal.h"
#include "net/DataURL.h"
#include "net/RequestExtraData.h"
#include "net/BlobResourceLoader.h"
#include "net/SharedMemoryDataConsumerHandle.h"
#include "net/FixedReceivedData.h"
#include "net/WebURLLoaderManagerUtil.h"
#include "net/WebURLLoaderManagerMainTask.h"
#include "net/FlattenHTTPBodyElement.h"
#include "net/WebURLLoaderManagerSetupInfo.h"
#include "net/WebURLLoaderManagerAsynTask.h"
#include "net/InitializeHandleInfo.h"
#include "net/HeaderVisitor.h"
#include "net/PageNetExtraData.h"
#include "net/DefaultFullPath.h"
#include "net/CurlCacheManager.h"
#include "net/DownloadFileBlobCache.h"
#include "net/cookies/WebCookieJarCurlImpl.h"
#include "net/cookies/CookieJarMgr.h"
#include "third_party/WebKit/Source/wtf/Threading.h"
#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/text/CString.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include <errno.h>
#include <stdio.h>

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
#include "wke/wkeNetHook.h"
#include "wke/wkeWebView.h"
#include "wke/wkeGlobalVar.h"
#endif
#include "wtf/RefCountedLeakCounter.h"

using namespace blink;

namespace net {

MainTaskRunner* MainTaskRunner::m_inst = nullptr;

void WebURLLoaderManager::initializeMultiHandleOnIoThread(int* waitCount)
{
    CURLM* curlMultiHandle = curl_multi_init();
    m_curlMultiHandles.insert(std::pair<DWORD, CURLM*>(::GetCurrentThreadId(), curlMultiHandle));
    atomicDecrement(waitCount);
}

CURLM* WebURLLoaderManager::getCurrentThreadCurlMultiHandle() const
{
    std::map<DWORD, CURLM*>::const_iterator it = m_curlMultiHandles.find(::GetCurrentThreadId());
    CURLM* handle = it->second;
    return handle;
}

static void setCookiePath(CURL* handle, std::string cookieJarPath)
{
    if (0 == cookieJarPath.size())
        return;

//     std::vector<char> cookieJar;
//     WTF::Utf8ToMByte(cookieJarPath.c_str(), cookieJarPath.size(), &cookieJar, CP_ACP);
//     if (0 == cookieJar.size())
//         return;
// 
//     cookieJar.push_back('\0');
// 
//     curl_easy_setopt(handle, CURLOPT_COOKIEJAR, &cookieJar[0]);
//     curl_easy_setopt(handle, CURLOPT_COOKIEFILE, &cookieJar[0]);

    // 统一用UTF8格式了
    curl_easy_setopt(handle, CURLOPT_COOKIEJAR, cookieJarPath.c_str());
    curl_easy_setopt(handle, CURLOPT_COOKIEFILE, cookieJarPath.c_str());
}

WebURLLoaderManager::WebURLLoaderManager(const char* cookieJarFullPath)
    : m_certificatePath(certificatePath())
    , m_runningJobs(0)
    , m_isShutdown(false)
    , m_newestJobId(1)
{
    content::BlinkPlatformImpl* platform = (content::BlinkPlatformImpl*)Platform::current();

    if (wke::g_diskCacheEnable && CurlCacheManager::getInstance()->cacheDirectory().isEmpty()) {
        String path = net::getDefaultLocalStorageFullPath();
        path.append(L"cache");
        CurlCacheManager::getInstance()->setCacheDirectory(path);
    }

    m_mainTasksBegin = nullptr;
    m_mainTasksEnd = nullptr;
    m_syncIoThread = nullptr;
    m_threads = platform->getIoThreads();
    
    for (size_t i = 0; i < m_threads.size(); ++i) {
        int waitCount = 1;
        m_threads[i]->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::initializeMultiHandleOnIoThread, this, &waitCount));
        while (waitCount) {
            ::Sleep(10);
        }
    }    

    curl_global_init(CURL_GLOBAL_ALL);
    //m_curlMultiHandle = curl_multi_init(); // 初始化curl批处理句柄

    initCookieSession(cookieJarFullPath);    
}

WebURLLoaderManager::~WebURLLoaderManager()
{
    for (std::map<DWORD, CURLM*>::iterator it = m_curlMultiHandles.begin(); it != m_curlMultiHandles.end(); ++it) {
        CURLM* handle = it->second;
        curl_multi_cleanup(handle);
    }
    curl_global_cleanup();
}

void WebURLLoaderManager::shutdown()
{
    m_isShutdown = true;

    //WTF::Locker<WTF::Mutex> locker(m_shutdownMutex);
    m_shutdownLock.waitForWrite();

    CurlCacheManager::getInstance()->shutdown();

    m_liveJobsMutex.lock();
    WTF::HashMap<int, JobHead*> liveJobs = m_liveJobs;
    m_liveJobs.clear();
    m_liveJobsMutex.unlock();

    MainTaskRunner::destroy();

    WTF::HashMap<int, JobHead*>::iterator it = liveJobs.begin();
    for (; it != liveJobs.end(); ++it) {
        JobHead* job = it->value;
        if (!job)
            continue;

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

    m_threads.clear();
}

void WebURLLoaderManager::initCookieSession(const char* cookieJarFullPath)
{
    // Curl saves both persistent cookies, and session cookies to the cookie file.
    // The session cookies should be deleted before starting a new session.

    //初始化共享curl句柄,用于共享cookies和dns等缓存
    m_shareCookieJar = CookieJarMgr::getInst()->createOrGet(cookieJarFullPath);

    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_SHARE, m_shareCookieJar->getCurlShareHandle());

    std::string cookieJarPathString = m_shareCookieJar->getCookieJarFullPath();
    setCookiePath(handle, cookieJarPathString);

    curl_easy_setopt(handle, CURLOPT_COOKIESESSION, 1);
    curl_easy_cleanup(handle);
}

CURLSH* WebURLLoaderManager::getCurlShareHandle() const
{
    return m_shareCookieJar->getCurlShareHandle();
}

WebCookieJarImpl* WebURLLoaderManager::getShareCookieJar() const
{
    return m_shareCookieJar;
}

WebURLLoaderManager* WebURLLoaderManager::m_sharedInstance = nullptr;

static std::string getDefaultCookiesFullpath()
{
    std::vector<wchar_t> path;
    path.resize(MAX_PATH + 1);
    memset(&path[0], 0, sizeof(wchar_t) * (MAX_PATH + 1));
    ::GetModuleFileNameW(nullptr, &path[0], MAX_PATH);
    ::PathRemoveFileSpecW(&path[0]);
    ::PathAppendW(&path[0], L"cookies.dat");

    std::vector<char> pathStrA;
    WTF::WCharToMByte(&path[0], wcslen(&path[0]), &pathStrA, CP_UTF8);

    return std::string(&pathStrA[0], pathStrA.size());
}

WebURLLoaderManager* WebURLLoaderManager::sharedInstance()
{
    if (!m_sharedInstance) {
        std::string defaultCookiesFullpath = getDefaultCookiesFullpath();
        m_sharedInstance = new WebURLLoaderManager(defaultCookiesFullpath.c_str());
    }
    if (m_sharedInstance->isShutdown())
        return nullptr;
    return m_sharedInstance;
}

void WebURLLoaderManager::setCookieJarFullPath(const char* path)
{
    if (!m_sharedInstance) {
        m_sharedInstance = new WebURLLoaderManager(path);
    } else {
        WTF::Mutex* mutex = sharedResourceMutex(CURL_LOCK_DATA_COOKIE);
        WTF::Locker<WTF::Mutex> locker(*mutex);

        WebCookieJarImpl* cookieJar = CookieJarMgr::getInst()->createOrGet(path);
        m_sharedInstance->m_shareCookieJar = cookieJar;
    }
    ASSERT(m_sharedInstance->m_shareCookieJar);
}

// https://rbt.guorenpcic.com/api/grecar/print/transcriptPrintNew?policyNo=6191515000518004167&loginComCode=151515HH&userCode=W1500227 pdf没有mime，强行设置
static void parseMimeIfNeeded(WebURLLoaderManager* manager, WebURLLoaderInternal* job, const char* data, int dataLengt)
{
    String mime = job->m_response.mimeType();
    if (!job->m_needParseMime || (!mime.isNull() && !mime.isEmpty()))
        return;
    job->m_needParseMime = false;

    if (dataLengt > 6 && 0 == memcmp(data, "%PDF-", 5)) {
        job->m_response.setMIMEType(blink::WebString::fromUTF8("application/pdf"));
        //manager->handleDidReceiveResponse(job);
    }
}

void WebURLLoaderManager::handleDidSentData(WebURLLoaderInternal* job, unsigned long long bytesSent, unsigned long long totalBytesToBeSent)
{
    blink::WebURLLoaderClient* client = job->client();
    WebURLLoaderImplCurl* loader = job->loader();
    client->didSendData(loader, bytesSent, totalBytesToBeSent); // weolar
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
        if (WebURLLoaderInternal::kCacheForDownloadYes != job->m_cacheForDownloadOpt) {
            parseMimeIfNeeded(this, job, data, dataLength);

            client->didReceiveData(loader, data, dataLength, encodedDataLength);
            CurlCacheManager::getInstance()->didReceiveData(*job, data, dataLength);
        }
        return;
    }

    DownloadFileBlobCache::inst()->appendDataToBlobCache(client, loader, String(job->m_url), data, dataLength, encodedDataLength);
}

DownloadFileBlobCache* DownloadFileBlobCache::m_inst = nullptr;

DownloadFileBlobCache* DownloadFileBlobCache::inst()
{
    if (m_inst)
        return m_inst;
    m_inst = new DownloadFileBlobCache();
    return m_inst;
}

void DownloadFileBlobCache::shutdown()
{
    ASSERT(m_blobCaches.size() == 0);
}

// 有两种情况会走到这里，一个是xhr加载一个blob的注册url，在BlobResourceLoader里加载，一个是正常的XHR。共同点是xhr.responseType = "blob";
void DownloadFileBlobCache::appendDataToBlobCache(
    blink::WebURLLoaderClient* client,
    blink::WebURLLoader* loader,
    const String& url,
    const char* data,
    int dataLength,
    int encodedDataLength
    )
{
    BlobCache::iterator it = m_blobCaches.find(url);
    if (it == m_blobCaches.end()) {
        DebugBreak();
        return;
    }

    BlobTempFileInfo* tempFile = it->value;
    Vector<char>& tempFileData = tempFile->data;
    tempFileData.append(data, dataLength);

    client->didDownloadData(loader, dataLength, encodedDataLength);
}

BlobTempFileInfo* DownloadFileBlobCache::getBlobTempFileInfoByTempFilePath(const String& path)
{
    BlobCache::iterator it = m_blobCaches.begin();
    for (; it != m_blobCaches.end(); ++it) {
        BlobTempFileInfo* tempFileInfo = it->value;
        if (tempFileInfo->tempUrl != path)
            continue;

        return tempFileInfo;
    }

    return nullptr;
}

String DownloadFileBlobCache::createBlobTempFileInfoByUrlIfNeeded(const String& url)
{
    BlobCache::iterator it = m_blobCaches.find(url);
    if (it != m_blobCaches.end())
        return it->value->tempUrl;

    static int s_id = 0;
    ++s_id;
    String tempPath = String::format("file:///c:/miniblink_blob_download_%d", s_id);

    BlobTempFileInfo* blobTempFileInfo = new BlobTempFileInfo();
    blobTempFileInfo->tempUrl = tempPath;
    blobTempFileInfo->url = url;

    m_blobCaches.set(url, blobTempFileInfo);

    return tempPath;
}

void DownloadFileBlobCache::removeBlobTempFileInfo(const String& url)
{
    if (url.isEmpty())
        return;

    BlobCache::iterator it = m_blobCaches.find(url);
    if (it == m_blobCaches.end())
        DebugBreak();
    m_blobCaches.remove(it);
}

String WebURLLoaderManager::handleHeaderForBlobOnMainThread(WebURLLoaderInternal* job, size_t totalSize)
{
    String url(job->m_url);
    String result = DownloadFileBlobCache::inst()->createBlobTempFileInfoByUrlIfNeeded(url);
    //job->m_loader->setBlobDownloadFile(url);
    return result;
}

static void setBlobDataLengthByTempPath(WebURLLoaderInternal* job)
{
    if (!job->firstRequest()->downloadToFile())
        return;

    content::WebBlobRegistryImpl* blolReg = (content::WebBlobRegistryImpl*)blink::Platform::current()->blobRegistry();
    blolReg->setBlobDataLengthByTempPath(job->m_response.downloadFilePath(), job->m_dataLength);
}

#if ENABLE_WKE == 1
static void dispatchWkeLoadUrlFinishCallback(WebURLLoaderInternal* job, int64_t totalEncodedDataLength)
{
    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    if (!requestExtraData)
        return;

    WebPage* page = requestExtraData->page;
    if (!page)
        return;

    wkeLoadUrlFinishCallback loadUrlFinishCallback = page->wkeHandler().loadUrlFinishCallback;
    void* loadUrlFinishCallbackParam = page->wkeHandler().loadUrlFinishCallbackParam;
    Vector<char> urlBuf = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);
    if (loadUrlFinishCallback)
        loadUrlFinishCallback(page->wkeWebView(), loadUrlFinishCallbackParam, urlBuf.data(), job, (int)totalEncodedDataLength);
}
#endif

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
    if (WebURLLoaderInternal::kCacheForDownloadYes != job->m_cacheForDownloadOpt) {
        handleDidSentData(job, job->m_totalBytesToBeSent, job->m_totalBytesToBeSent);

#if ENABLE_WKE == 1
        dispatchWkeLoadUrlFinishCallback(job, totalEncodedDataLength);
#endif

        job->client()->didFinishLoading(job->loader(), finishTime, totalEncodedDataLength);
        CurlCacheManager::getInstance()->didFinishLoading(*job);
    }
}

static void handleExternalProtocol(const KURL& url)
{
    String urlProtocol = url.protocol();
    if (WTF::kNotFound == urlProtocol.find("tencent") && WTF::kNotFound == urlProtocol.find("xunlei"))
        return;

    curl_version_info_data* curlVer = curl_version_info(CURLVERSION_FIRST);

    for (int i = 0; curlVer->protocols[i]; ++i) {
        const char* protocol = curlVer->protocols[i];
        if (!equalIgnoringCase(urlProtocol, protocol))
            continue;
        return;
    }
    ShellExecuteA(NULL, NULL, url.getUTF8String().utf8().data(), NULL, NULL, SW_SHOWNORMAL);
}

void WebURLLoaderManager::handleDidFail(WebURLLoaderInternal* job, const blink::WebURLError& error)
{
    if (job->m_bodyStreamWriter) {
        job->m_bodyStreamWriter->fail();
        delete job->m_bodyStreamWriter;
        job->m_bodyStreamWriter = nullptr;
    }
    KURL url = job->firstRequest()->url();

    handleExternalProtocol(url);

    setBlobDataLengthByTempPath(job);
    if (WebURLLoaderInternal::kCacheForDownloadYes != job->m_cacheForDownloadOpt) {
        job->client()->didFail(job->loader(), error);
        CurlCacheManager::getInstance()->didFail(*job);
    }
    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    if (!requestExtraData)
        return;

    WebPage* page = requestExtraData->page;
    if (!page)
        return;	

#if ENABLE_WKE == 1
    wkeLoadUrlFailCallback loadUrlFailCallback = page->wkeHandler().loadUrlFailCallback;
    void* loadUrlFailCallbackParam = page->wkeHandler().loadUrlFailCallbackParam;
    Vector<char> urlBuf = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);
    if (loadUrlFailCallback)
        loadUrlFailCallback(page->wkeWebView(), loadUrlFailCallbackParam, urlBuf.data(), job);
#endif
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

    if (job->client() && WebURLLoaderInternal::kCacheForDownloadYes != job->m_cacheForDownloadOpt) {
        // TODO(yhirano): Set |stale_copy_in_cache| appropriately if possible.
        WebURLError error;
        error.domain = WebString(String(job->m_url));
        error.reason = -3;
        error.localizedDescription = WebString::fromUTF8("cancelBodyStreaming");
        job->client()->didFail(job->loader(), error);
        CurlCacheManager::getInstance()->didFail(*job);
    }

    // Notify the browser process that the request is canceled.
    WebURLLoaderManager::sharedInstance()->cancel(jobId);
}

#if ENABLE_WKE == 1
static void dispatchWkeLoadUrlHeadersReceivedCallback(WebURLLoaderInternal* job)
{
    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    if (!requestExtraData)
        return;

    WebPage* page = requestExtraData->page;
    if (!page)
        return;

    wkeLoadUrlHeadersReceivedCallback loadUrlHeadersReceivedCallback = page->wkeHandler().loadUrlHeadersReceivedCallback;
    void* loadUrlHeadersReceivedCallbackParam = page->wkeHandler().loadUrlHeadersReceivedCallbackParam;
    Vector<char> urlBuf = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);
    if (loadUrlHeadersReceivedCallback)
        loadUrlHeadersReceivedCallback(page->wkeWebView(), loadUrlHeadersReceivedCallbackParam, urlBuf.data(), job);
}
#endif

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

    if (WebURLLoaderInternal::kCacheForDownloadYes != job->m_cacheForDownloadOpt) {
#if ENABLE_WKE == 1
        dispatchWkeLoadUrlHeadersReceivedCallback(job);
#endif
        job->client()->didReceiveResponse(job->loader(), response, readHandle);
    }
}

// called with data after all headers have been processed via headerCallbackOnIoThread
static size_t writeCallbackOnIoThread(void* ptr, size_t size, size_t nmemb, void* data)
{
    int jobId = (int)data;
    AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
    WebURLLoaderInternal* job = autoLockJob.lock();
    if (!job || job->isCancelled())
        return 0;

    // We should never be called when deferred loading is activated.
    ASSERT(!job->m_defersLoading);

    size_t totalSize = size * nmemb;
    // this shouldn't be necessary but apparently is. CURL writes the data
    // of html page even if it is a redirect that was handled internally
    // can be observed e.g. on gmail.com
    long httpCode = 0;
    CURLcode err = curl_easy_getinfo(job->m_handle, !job->m_isProxyConnect ? CURLINFO_RESPONSE_CODE : CURLINFO_HTTP_CONNECTCODE, &httpCode);
    if (CURLE_OK == err && httpCode >= 300 && httpCode < 400)
        return totalSize;

    WebURLLoaderManagerMainTask::createAndPushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kWriteCallback, ptr, size, nmemb, totalSize);
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
    if (!job || job->isCancelled())
        return 0;

    // We should never be called when deferred loading is activated.
    ASSERT(!job->m_defersLoading);

    size_t totalSize = size * nmemb;
    if (checkIsProxyHead(job, ptr, totalSize))
        return totalSize;

    if (0 == size * nmemb)
        ASSERT(false);

    job->m_hasCallResponse = true;
    WebURLLoaderManagerMainTask::createAndPushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kHeaderCallback, ptr, size, nmemb, totalSize);
    return totalSize;
}

static curlioerr ioctlCallbackOnIoThread(CURL* handle, int cmd, void* data)
{
    int jobId = (int)data;
    AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
    WebURLLoaderInternal* job = autoLockJob.lock();
    if (!job || job->isCancelled())
        return CURLIOE_UNKNOWNCMD;

    if (cmd == CURLIOCMD_RESTARTREAD) {
        job->m_formDataStream->reset();

        return CURLIOE_OK;
    }
    return CURLIOE_UNKNOWNCMD;
}

size_t readCallbackOnIoThread(void* ptr, size_t size, size_t nmemb, void* data)
{
    int jobId = (int)data;
    AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
    WebURLLoaderInternal* job = autoLockJob.lock();
    if (!job || job->isCancelled())
        return 0;

    // We should never be called when deferred loading is activated.
    ASSERT(!job->m_defersLoading);

    if (!size || !nmemb)
        return 0;

    if (!job->m_formDataStream->hasMoreElements()) {
        delete job->m_formDataStream;
        job->m_formDataStream = nullptr;
        return 0;
    }

    size_t sent = job->m_formDataStream->read(ptr, size, nmemb, nullptr);

    // Something went wrong so cancel the job.
    if (!sent && WebURLLoaderInternal::kCacheForDownloadYes != job->m_cacheForDownloadOpt)
        WebURLLoaderManager::sharedInstance()->doCancel(job, kNormalCancelled);

    WebURLLoaderManagerMainTask::createAndPushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kDidSentData, nullptr, size, nmemb, 0);

    return sent;
}

bool WebURLLoaderManager::downloadOnIoThread()
{
    if (m_isShutdown)
        return false;

    CURLM* curlMultiHandle = getCurrentThreadCurlMultiHandle();

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
        curl_multi_fdset(curlMultiHandle, &fdread, &fdwrite, &fdexcep, &maxfd);
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
    while (curl_multi_perform(curlMultiHandle, &runningHandles) == CURLM_CALL_MULTI_PERFORM) {}

    // check the curl messages indicating completed transfers
    // and free their resources
    while (true) {
        int messagesInQueue;
        CURLMsg* msg = curl_multi_info_read(curlMultiHandle, &messagesInQueue);
        if (!msg)
            break;

        // find the node which has same job->m_handle as completed transfer
        CURL* handle = msg->easy_handle;
        ASSERT(handle);
        char* info = NULL;
        CURLcode err = curl_easy_getinfo(handle, CURLINFO_PRIVATE, &info);
        int jobId = (int)info;
        ASSERT_UNUSED(err, CURLE_OK == err);
        ASSERT(jobId > 0);
        AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
        WebURLLoaderInternal* job = autoLockJob.lock();
        if (!job)
            continue;

        ASSERT(job->m_handle == handle);
        if (job->isCancelled()) {
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
                WebURLLoaderManagerMainTask::createAndPushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kHandleLocalReceiveResponse, nullptr, 0, 0, 0);
                if (job->isCancelled()) {
                    removeFromCurlOnIoThread(jobId);
                    continue;
                }
            }

            if (job->m_isHookRequest)
                WebURLLoaderManagerMainTask::createAndPushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kHandleHookRequest, nullptr, 0, 0, 0);

            if (job->m_multipartHandle) {
                WebURLLoaderManagerMainTask::createAndPushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kContentEnded, nullptr, 0, 0, 0);
            } else if (job->client() && job->loader()) {
                WebURLLoaderManagerMainTask::createAndPushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kDidFinishLoading, nullptr, 0, 0, 0);
            }
        } else {
            char* url = nullptr;
            curl_easy_getinfo(job->m_handle, CURLINFO_EFFECTIVE_URL, &url);
            if (!url)
                url = "url is empty";
            if (job->client() && job->loader()) {
                net::URLError* resourceError = new net::URLError();
                resourceError->reason = msg->data.result;
                resourceError->domain = url;
                resourceError->unreachableURL = url;
                resourceError->localizedDescription = curl_easy_strerror(msg->data.result);

                char* output = (char*)malloc(0x300 + strlen(url));
                sprintf(output, "kDidFail on io Thread:%d, %s\n", msg->data.result, url);          
                OutputDebugStringA(output);
                free(output);

                WebURLLoaderManagerMainTask::createAndPushResErrTask(jobId, resourceError);
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

        m_proxy = host;
//        m_proxy = userPass + host + ":" + String::number(port);
//         if (!m_proxy.startsWith("https://") && !m_proxy.startsWith("http://"))
//             m_proxy.insert("http://", 0);
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

        if (/*!job->m_isWkeNetSetDataBeSetted && !job->m_isBlackList && !job->m_isDataUrl*/job->m_handle) {
            //ASSERT(job->m_handle);

            if (job->m_handle) {
                curl_multi_remove_handle(getCurrentThreadCurlMultiHandle(), job->m_handle);
                curl_easy_cleanup(job->m_handle);
            }
            job->m_handle = nullptr;
            WebURLLoaderManagerMainTask* task = WebURLLoaderManagerMainTask::createTask(jobId, WebURLLoaderManagerMainTask::TaskType::kRemoveFromCurl, nullptr, 0, 0, 0);
//             if (task) // 退出时候，这里可能为null
//                 Platform::current()->mainThread()->scheduler()->postLoadingTask(FROM_HERE, task); // postLoadingTask
            WebURLLoaderManagerMainTask::pushTask(nullptr, task);
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

static void setupFormDataOnIoThread(WebURLLoaderInternal* job, SetupDataInfo* info)
{
    if (info && !info->chunkedTransfer) {
        if (info->islongLong)
            curl_easy_setopt(job->m_handle, info->sizeOption, (long long)info->size);
        else
            curl_easy_setopt(job->m_handle, info->sizeOption, (int)info->size);
    }

    ASSERT(!job->m_formDataStream);
    job->m_formDataStream = new FlattenHTTPBodyElementStream(info->flattenElements, info->size);
    curl_easy_setopt(job->m_handle, CURLOPT_READFUNCTION, readCallbackOnIoThread);
    curl_easy_setopt(job->m_handle, CURLOPT_READDATA, job->m_id);

    curl_easy_setopt(job->m_handle, CURLOPT_IOCTLFUNCTION, ioctlCallbackOnIoThread);
    curl_easy_setopt(job->m_handle, CURLOPT_IOCTLDATA, job->m_id);
}

static void dispatchPostBodyToWke(WebURLLoaderInternal* job, WTF::Vector<FlattenHTTPBodyElement*>* flattenElements)
{
//     RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
//     if (!requestExtraData)
//         return;
// 
//     WebPage* page = requestExtraData->page;
//     if (!page->wkeHandler().otherLoadCallback)
//         return;
// 
//     wkeTempCallbackInfo* tempInfo = wkeGetTempCallbackInfo(page->wkeWebView());
//     Vector<char> urlBuf = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);
//     tempInfo->url = urlBuf.data();
// 
//     tempInfo->postBody = wke::flattenHTTPBodyElementToWke(*flattenElements);
//     
//     page->wkeHandler().otherLoadCallback(page->wkeWebView(), page->wkeHandler().otherLoadCallbackParam, WKE_DID_POST_REQUEST, tempInfo);
//     if (tempInfo->postBody->isDirty)
//         wke::wkeflattenElementToBlink(*tempInfo->postBody, flattenElements);
//     else
//         wkeNetFreePostBodyElements(tempInfo->postBody);
}

static SetupDataInfo* setupFormDataOnMainThread(WebURLLoaderInternal* job, CURLoption sizeOption, struct curl_slist** headers)
{
    WebHTTPBody httpBody = job->firstRequest()->httpBody();

    // The size of a curl_off_t could be different in WebKit and in cURL depending on
    // compilation flags of both.
    static int expectedSizeOfCurlOffT = 0;
    if (!expectedSizeOfCurlOffT) {
        curl_version_info_data* infoData = curl_version_info(CURLVERSION_NOW);
        if (infoData->features & CURL_VERSION_LARGEFILE)
            expectedSizeOfCurlOffT = sizeof(long long);
        else
            expectedSizeOfCurlOffT = sizeof(int);

        expectedSizeOfCurlOffT = sizeof(int); // weolar
    }
    
    static const long long maxCurlOffT = (1LL << (expectedSizeOfCurlOffT * 8 - 1)) - 1;
    // Obtain the total size of the form data
    curl_off_t size = 0;
    SetupDataInfo* result = new SetupDataInfo();
    result->chunkedTransfer = false;

    long long fileSizeResult = 0;
    for (size_t i = 0; i < httpBody.elementCount(); ++i) {
        blink::WebHTTPBody::Element element;
        if (!httpBody.elementAt(i, element))
            continue;

        FlattenHTTPBodyElement* flattenElement = nullptr;
        if (blink::WebHTTPBody::Element::TypeFile == element.type) {
            if (getFileSize((String)(element.filePath), fileSizeResult)) {
                if (fileSizeResult > maxCurlOffT) {
                    // File size is too big for specifying it to cURL
                    result->chunkedTransfer = true;
                    break;
                }
                
            } else {
                result->chunkedTransfer = true;
                break;
            }

            long long offset = element.fileStart;
            long long length = element.fileLength;
            FlattenHTTPBodyElementStream::clampSliceOffsets(fileSizeResult, &offset, &length);
            size += length;

            flattenElement = new FlattenHTTPBodyElement();
            flattenElement->type = FlattenHTTPBodyElement::Type::TypeFile;
            Vector<UChar> filePath = WTF::ensureUTF16UChar(element.filePath, true);
            flattenElement->filePath = filePath.data();
            flattenElement->fileStart = offset;
            flattenElement->fileLength = length;
            result->flattenElements.append(flattenElement);
        } else if (WebHTTPBody::Element::TypeData == element.type) {
            size += element.data.size();

            flattenElement = new FlattenHTTPBodyElement();
            flattenElement->type = FlattenHTTPBodyElement::Type::TypeData;
            flattenElement->data.append(element.data.data(), element.data.size());
            result->flattenElements.append(flattenElement);
        } else if (WebHTTPBody::Element::TypeBlob == element.type) {
            FlattenHTTPBodyElementStream::flatten(element.blobUUID, &size, &result->flattenElements, 0, -1, true, 1);
        }
    }
    
    // dispatchPostBodyToWke(job, &result->flattenElements);

    // cURL guesses that we want chunked encoding as long as we specify the header
    if (result->chunkedTransfer)
        *headers = curl_slist_append(*headers, "Transfer-Encoding: chunked");
    else {
        result->sizeOption = sizeOption;
        result->size = size;
        result->islongLong = (sizeof(long long) == expectedSizeOfCurlOffT);
    }

    job->m_totalBytesToBeSent = size;

    return result;
}

static void setupPutOnIoThread(WebURLLoaderInternal* job, SetupPutInfo* info)
{
    if (!info)
        return;

    curl_easy_setopt(job->m_handle, CURLOPT_UPLOAD, TRUE); // CURLOPT_PUT
    curl_easy_setopt(job->m_handle, CURLOPT_INFILESIZE, 0);

    if (info->data)
        setupFormDataOnIoThread(job, info->data);
}

static SetupPutInfo* setupPutOnMainThread(WebURLLoaderInternal* job, struct curl_slist** headers)
{
    *headers = curl_slist_append(*headers, "Expect:"); // Disable the Expect: 100 continue header

    size_t numElements = getFormElementsCount(job);
    if (!numElements)
        return nullptr;

    SetupPutInfo* result = new SetupPutInfo();
    result->data = setupFormDataOnMainThread(job, CURLOPT_INFILESIZE, headers);
    return result;
}

static void setupPostOrPutOnIoThread(WebURLLoaderInternal* job, bool isPost, SetupDataInfo* data)
{
    if (isPost) {
        curl_easy_setopt(job->m_handle, CURLOPT_POST, TRUE);
    } else {
        curl_easy_setopt(job->m_handle, CURLOPT_POST, FALSE);
        curl_easy_setopt(job->m_handle, CURLOPT_UPLOAD, TRUE);
        curl_easy_setopt(job->m_handle, CURLOPT_PUT, TRUE);
    }

    if (!data) {
        curl_easy_setopt(job->m_handle, CURLOPT_POSTFIELDSIZE, 0);
        return;
    }

    if (isPost && data && 1 == data->flattenElements.size()) {
        FlattenHTTPBodyElement* element = data->flattenElements[0];
        if (WebHTTPBody::Element::TypeData == element->type || WebHTTPBody::Element::TypeBlob == element->type) {
            curl_easy_setopt(job->m_handle, CURLOPT_POSTFIELDSIZE, element->data.size());
            curl_easy_setopt(job->m_handle, CURLOPT_COPYPOSTFIELDS, element->data.data());

            InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&job->m_sentDataBytes), static_cast<long>(element->data.size()));

            delete element;
            return;
        }
    }

    if (data)
        setupFormDataOnIoThread(job, data);
}

static SetupPostInfo* setupPostOnMainThread(WebURLLoaderInternal* job, struct curl_slist** headers)
{
    // *headers = curl_slist_append(*headers, "Expect:100-continue"); // Disable the Expect: 100 continue header
    *headers = curl_slist_append(*headers, "Expect:"); // https://blog.csdn.net/pzqingchong/article/details/70196092
    size_t numElements = getFormElementsCount(job);
    if (!numElements)
        return nullptr;

    SetupPostInfo* result = new SetupPostInfo();
    result->data = setupFormDataOnMainThread(job, CURLOPT_POSTFIELDSIZE, headers); // CURLOPT_POSTFIELDSIZE_LARGE
    return result;
}

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

    JobHead* jobHead = m_manager->checkJob(m_jobId);
    if (!jobHead)
        return nullptr;
    if (JobHead::kLoaderInternal != jobHead->getType())
        return nullptr;

    jobHead->ref();
    WebURLLoaderInternal* job = (WebURLLoaderInternal*)jobHead;
    return job;
}

JobHead* AutoLockJob::lockJobHead()
{
    if (!m_manager)
        return nullptr;

    JobHead* jobHead = m_manager->checkJob(m_jobId);
    if (!jobHead)
        return nullptr;

    jobHead->ref();
    return jobHead;
}

void AutoLockJob::setNotDerefForDelete()
{
    m_isNotDerefForDelete = true;
}

AutoLockJob::~AutoLockJob()
{
    if (m_isNotDerefForDelete || !m_manager)
        return;
    JobHead* job = m_manager->checkJob(m_jobId);
    if (job)
        job->deref();
}

JobHead* WebURLLoaderManager::checkJob(int jobId)
{
    WTF::Locker<WTF::Mutex> locker(m_liveJobsMutex);
    WTF::HashMap<int, JobHead*>::iterator it = m_liveJobs.find(jobId);
    if (it == m_liveJobs.end())
        return nullptr;
    return it->value;
}

int WebURLLoaderManager::addLiveJobs(JobHead* job)
{
    RELEASE_ASSERT(WTF::isMainThread());
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
    RELEASE_ASSERT(WTF::isMainThread());
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
    HandleDataURLTask(WebURLLoaderManager* manager, int jobId, bool useStreamOnResponse)
    {
        m_manager = manager;
        m_jobId = jobId;
        m_useStreamOnResponse = useStreamOnResponse;
    }

    ~HandleDataURLTask() override
    {
    }

    virtual void run() override
    {
        JobHead* jobHead = m_manager->checkJob(m_jobId);
        if (!jobHead || JobHead::kLoaderInternal != jobHead->getType())
            return;

        WebURLLoaderInternal* job = (WebURLLoaderInternal*)jobHead;
        if (!job || job->isCancelled())
            return;

        KURL url = job->firstRequest()->url();
        handleDataURL(job->loader(), job->client(), url, m_useStreamOnResponse, false);
    }

private:
    WebURLLoaderManager* m_manager;
    int m_jobId;
    bool m_useStreamOnResponse;
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

    size_t questionMarkPos = url.find("?");
    if (WTF::kNotFound != questionMarkPos)
        url = url.substring(0, questionMarkPos);

    bool result = false;
    Vector<UChar> buf = WTF::ensureUTF16UChar(url, true);
    result = !::PathFileExistsW(buf.data());
    if (!result)
        return false;

    String outString = String::format("isLocalFileNotExist: %s\n", WTF::ensureStringToUTF8(url, true).data());
    OutputDebugStringW(outString.charactersWithNullTermination().data());

    return result;
}

int WebURLLoaderManager::addAsynchronousJob(WebURLLoaderInternal* job)
{
    ASSERT(WTF::isMainThread());

    KURL kurl = job->firstRequest()->url();
    String url = WTF::ensureStringToUTF8String(kurl.string());
#if 0
//     if (WTF::kNotFound != url.find("electron-ui/file:")) 
//         OutputDebugStringA("");
    
    String outString = String::format("addAsynchronousJob: %d, %s\n", m_liveJobs.size(), WTF::ensureStringToUTF8(url, true).data());
    OutputDebugStringW(outString.charactersWithNullTermination().data());
#endif

    if (wke::g_isDecodeUrlRequest && !kurl.protocolIsData()) {
        url = blink::decodeURLEscapeSequences(url);
        job->firstRequest()->setURL((blink::KURL(blink::ParsedURLString, url)));
    }

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
        bool useStreamOnResponse = job->firstRequest()->useStreamOnResponse();

        Platform::current()->currentThread()->scheduler()->postLoadingTask(FROM_HERE, new HandleDataURLTask(this, jobId, useStreamOnResponse)); // postLoadingTask
        return jobId;
    }

    jobId = initializeHandleOnMainThread(job);
    if (0 == jobId)
        return 0;

    if (job->m_isWkeNetSetDataBeSetted || job->m_isHoldJobToAsynCommit || job->m_isBlackList)
        return jobId;

    continueJob(job);
    return jobId;
}

blink::WebThread* WebURLLoaderManager::getSyncIoThread() // 单独给某些垃圾网页使用同步io开个线程
{
    if (m_syncIoThread)
        return m_syncIoThread;
    
    content::BlinkPlatformImpl* platform = (content::BlinkPlatformImpl*)Platform::current();
    m_syncIoThread = platform->createIoThread("SyncIoThread");

    int waitCount = 1;
    m_syncIoThread->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::initializeMultiHandleOnIoThread, this, &waitCount));
    while (waitCount) {
        ::Sleep(10);
    }
    return m_syncIoThread;
}

blink::WebThread* WebURLLoaderManager::getIoThread(IoThreadType type)
{
    if (kIoThreadTypeRes == type || m_threads.size() == 1)
        return m_threads[0];

    if (kIoThreadTypeSync == type)
        return getSyncIoThread();

    int index = rand() % (m_threads.size() - 1);
    return m_threads[index + 1];
}

void WebURLLoaderManager::continueJob(WebURLLoaderInternal* job)
{
    if (job->m_isWkeNetSetDataBeSetted) {
        Platform::current()->currentThread()->scheduler()->postLoadingTask(FROM_HERE, new HookAsynTask(this, job->m_id));
        return;
    }

    blink::WebThread* ioThread = job->m_ioThread;
    ioThread->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::initializeHandleOnIoThread, this, job->m_id, job->m_initializeHandleInfo));
    ioThread->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::startOnIoThread, this, job->m_id));

    IoTask* task = new IoTask(this, ioThread, false);
    ioThread->postTask(FROM_HERE, task);
}

void WebURLLoaderManager::cancelWithHookRedirect(WebURLLoaderInternal* job)
{
    doCancel(job, kHookRedirectCancelled);
}

bool WebURLLoaderManager::doCancel(JobHead* jobHead, CancelledReason cancelledReason)
{
    if (JobHead::kLoaderInternal != jobHead->getType()) {
        jobHead->cancel();
        return true;
    }

    WebURLLoaderInternal* job = (WebURLLoaderInternal*)jobHead;
    WTF::Locker<WTF::Mutex> locker(job->m_destroingMutex);
    bool cancelled = job->isCancelled();

    RELEASE_ASSERT(kNoCancelled != cancelledReason);
    //RELEASE_ASSERT(!(kHookRedirectCancelled == job->m_cancelledReason && kNormalCancelled == cancelledReason));

    if (!(kHookRedirectCancelled == job->m_cancelledReason && kHookRedirectCancelled != cancelledReason))
        job->m_cancelledReason = cancelledReason;

//     if (job->m_isWkeNetSetDataBeSetted && kHookRedirectCancelled != cancelledReason && ::IsDebuggerPresent()) {
//         RELEASE_ASSERT(!job->m_handle);
//     }

    if (job->m_handle && WebURLLoaderInternal::kDestroying != job->m_state && !cancelled)
        job->m_ioThread->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::removeFromCurlOnIoThread, this, job->m_id));

    return false;
}

void WebURLLoaderManager::cancel(int jobId)
{
    ASSERT(WTF::isMainThread());
    AutoLockJob autoLockJob(this, jobId);
    WebURLLoaderInternal* job = autoLockJob.lock();
    if (!job || WebURLLoaderInternal::kCacheForDownloadYes == job->m_cacheForDownloadOpt)
        return;

    doCancel(job, kNormalCancelled);
}

void WebURLLoaderManager::cancelAllJobsOfWebview(int webviewId)
{
    //WTF::Locker<WTF::Mutex> locker(m_liveJobsMutex);
    m_liveJobsMutex.lock();

    int jobId = -1;
    WTF::HashSet<int> removedSet;
    WTF::HashMap<int, JobHead*>::iterator it = m_liveJobs.begin();
    for (; it != m_liveJobs.end(); ++it) {
        JobHead* jobHead = it->value;
        jobId = it->key;

        if (-1 != webviewId && JobHead::kLoaderInternal == jobHead->getType()) {
            WebURLLoaderInternal* job = (WebURLLoaderInternal*)jobHead;
            if (job->m_webviewId != webviewId)
                continue;
        }

        m_liveJobsMutex.unlock();
        if (doCancel(jobHead, kNormalCancelled))
            removedSet.add(jobId);
        m_liveJobsMutex.lock();
    }

    WTF::HashSet<int>::iterator itor = removedSet.begin();
    for (; itor != removedSet.end(); ++itor) {
        jobId = *itor;
        m_liveJobs.remove(jobId);
    }
    m_liveJobsMutex.unlock();
}

void WebURLLoaderManager::cancelAll()
{
    cancelAllJobsOfWebview(-1);
}

void WebURLLoaderManager::dispatchSynchronousJob(WebURLLoaderInternal* job)
{
    ASSERT(WTF::isMainThread());
    job->m_manager = this;
    job->m_isSynchronous = true;

    int jobId = addLiveJobs(job);

    KURL url = job->firstRequest()->url();
    if (url.protocolIsData() && job->client()) {
        handleDataURL(job->loader(), job->client(), url, job->firstRequest()->useStreamOnResponse(), true);
        removeLiveJobs(jobId);
        delete job;
        return;
    }

    Vector<char> urlBuf = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    InitializeHandleInfo* info = preInitializeHandleOnMainThread(job);
    job->m_initializeHandleInfo = info;

    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    WebPage* page = requestExtraData->page;
    wkeLoadUrlBeginCallback loadUrlBeginCallback = page->wkeHandler().loadUrlBeginCallback;
    if (loadUrlBeginCallback) {
        loadUrlBeginCallback(page->wkeWebView(), page->wkeHandler().loadUrlBeginCallbackParam, urlBuf.data(), job);
            
        // 同步请求，不考虑wkeNetHoldJobToAsynCommit
        if (job->m_asynWkeNetSetData && kNormalCancelled != job->m_cancelledReason) {
            size_t size = job->m_asynWkeNetSetData->size();
            if (job->firstRequest()->downloadToFile() && size > 0) {
                String tempPath = handleHeaderForBlobOnMainThread(job, size);
                job->m_response.setDownloadFilePath(tempPath);
            }
            job->m_response.setURL(url);
            job->m_response.setHTTPStatusCode(200);
            job->m_response.setHTTPStatusText(blink::WebString::fromUTF8("OK"));
            handleDidReceiveResponse(job);

            didReceiveDataOrDownload(job, job->m_asynWkeNetSetData->data(), job->m_asynWkeNetSetData->size(), 0);
            handleDidFinishLoading(job, WTF::currentTime(), 0);
            removeLiveJobs(jobId);
            delete job;
            return;
        }
    }
#endif

    // If defersLoading is true and we call curl_easy_perform
    // on a paused handle, libcURL would do the transfert anyway
    // and we would assert so force defersLoading to be false.
    job->m_defersLoading = false;
    job->m_ioThread->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::initializeHandleOnIoThread, this, jobId, info));

    int isCallFinish = 0;
    CURLcode ret = CURLE_OK;
    job->m_ioThread->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::dispatchSynchronousJobOnIoThread, this, job, info, &ret, &isCallFinish));
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

        void* hookBuf = job->m_hookBufForEndHook ? job->m_hookBufForEndHook->data() : nullptr;
        int hookLength = job->m_hookBufForEndHook ? job->m_hookBufForEndHook->size() : 0;

        wkeLoadUrlEndCallback loadUrlEndCallback = page->wkeHandler().loadUrlEndCallback;
        void* loadUrlEndCallbackParam = page->wkeHandler().loadUrlEndCallbackParam;
        if (1 == job->m_isHookRequest && loadUrlEndCallback)
            loadUrlEndCallback(page->wkeWebView(), loadUrlEndCallbackParam, urlBuf.data(), job, hookBuf, hookLength);

        if (job->m_hookBufForEndHook)
            didReceiveDataOrDownload(job, static_cast<char*>(job->m_hookBufForEndHook->data()), hookLength, 0);
        handleDidFinishLoading(job, WTF::currentTime(), 0);
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

static bool dispatchWkeLoadUrlBegin(WebURLLoaderInternal* job, InitializeHandleInfo* info)
{
    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    if (!requestExtraData)
        return false;

    WebPage* page = requestExtraData->page;
    if (!page)
        return false;

    Vector<char> urlBuf = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);
    void* param = page->wkeHandler().loadUrlBeginCallbackParam;
    bool b = false;

    wkeLoadUrlBeginCallback callback = page->wkeHandler().loadUrlBeginCallback;
    if (!callback)
        return false;

    callback(page->wkeWebView(), param, urlBuf.data(), job);

    return job->m_isWkeNetSetDataBeSetted || b;
}

#endif

void WebURLLoaderManager::applyAuthenticationToRequest(WebURLLoaderInternal* handle)
{
    // m_user/m_pass are credentials given manually, for instance, by the arguments passed to XMLHttpRequest.open().
    WebURLLoaderInternal* job = handle;

    String user = job->m_user;
    String password = job->m_pass;

//     if (!job->m_initialCredential.isEmpty()) {
//         user = job->m_initialCredential.user();
//         password = job->m_initialCredential.password();
//         curl_easy_setopt(job->m_handle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
//     }

    // It seems we need to set CURLOPT_USERPWD even if username and password is empty.
    // Otherwise cURL will not automatically continue with a new request after a 401 response.

    // curl CURLOPT_USERPWD expects username:password
    String userpass = user + ":" + password;
    //curl_easy_setopt(job->m_handle, CURLOPT_USERPWD, userpass.utf8().data());
    curl_easy_setopt(job->m_handle, CURLOPT_USERPWD, "customer-GM_AOGub0oW-zone-pig666-area-CA-session-w2H0CHW6xo-life-30:HGFoyZeVt");
}

InitializeHandleInfo* WebURLLoaderManager::preInitializeHandleOnMainThread(WebURLLoaderInternal* job)
{
    InitializeHandleInfo* info = new InitializeHandleInfo();
    KURL url = job->firstRequest()->url();

    // Remove any fragment part, otherwise curl will send it as part of the request.
    url.removeFragmentIdentifier();
    String urlString = url.string();
    if (url.isLocalFile()) {
        // Remove any query part sent to a local file.
        if (!url.query().isEmpty()) {
            // By setting the query to a null string it'll be removed.
            url.setQuery(String());
            urlString = url.string();
        }

        size_t questionMaskPos = urlString.find('?');
        if (std::string::npos != questionMaskPos)
            urlString = urlString.substring(0, questionMaskPos);

        // Determine the MIME type based on the path.
        job->m_response.setMIMEType(MIMETypeRegistry::getMIMETypeForPath(urlString));
    }

    info->url = WTF::ensureStringToUTF8(urlString, true).data();
    info->method = job->firstRequest()->httpMethod().utf8();
    
#ifdef MINIBLINK_NO_MULTITHREAD_NET
    ASSERT(!job->m_url); // url must remain valid through the request
    job->m_url = fastStrDup(info->url.c_str()); // url is in ASCII so latin1() will only convert it to char* without character translation.
#endif

    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    if (requestExtraData) {
        WebPage* page = requestExtraData->page;
        if (page) {
            job->m_webviewId = page->getId();
        }
    }

    String contentType = job->firstRequest()->httpHeaderField("Content-Type");
    if (WTF::kNotFound != url.host().find("huobi.pro") && "POST" == info->method && job->firstRequest()->httpBody().isNull())
        job->firstRequest()->setHTTPHeaderField("Content-Type", ""); // 修复火币网登录不了的bug

    String range = job->firstRequest()->httpHeaderField("Range");
    if (range.startsWith("bytes="))
        range = range.substring(sizeof("bytes=") - 1);
    info->range = range.utf8().data();

    struct curl_slist* headers = 0;
    if (job->firstRequest()->toResourceRequest().httpHeaderFields().size() > 0) {
        HTTPHeaderMap customHeaders = job->firstRequest()->toResourceRequest().httpHeaderFields();

        if (CurlCacheManager::getInstance()->isCached(job)) {
            HTTPHeaderMap& requestHeaders = CurlCacheManager::getInstance()->requestHeaders(job);

            // append additional cache information
            HTTPHeaderMap::const_iterator it = requestHeaders.begin();
            HTTPHeaderMap::const_iterator end = requestHeaders.end();
            while (it != end) {
                customHeaders.set(it->key, it->value);
                ++it;
            }
        }

        HTTPHeaderMap::const_iterator end = customHeaders.end();
        for (HTTPHeaderMap::const_iterator it = customHeaders.begin(); it != end; ++it) {
            String key = it->key;
            String value = it->value;
            String headerString(key);
            if (value.isEmpty())
                // Insert the ; to tell curl that this header has an empty value.
                headerString.append(";");
            else {
                headerString.append(": ");
                headerString.append(value);
            }
            CString headerLatin1 = headerString.latin1();
            headers = curl_slist_append(headers, headerLatin1.data());
        }
    }

    if ("GET" == info->method) {

    } else if ("POST" == info->method) {
        info->methodInfo = new SetupHttpMethodInfo();
        info->methodInfo->post = setupPostOnMainThread(job, &headers);
    } else if ("PUT" == info->method) {
        info->methodInfo = new SetupHttpMethodInfo();
        info->methodInfo->put = setupPutOnMainThread(job, &headers);
    } else if ("HEAD" == info->method) {

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
    if (!requestExtraData) // 在退出时候js调用同步XHR请求，会导致ExtraData为0情况
        return info;

    WebPage* page = requestExtraData->page;
    if (!page->wkeWebView())
        return info;

//     String wkeProxy = page->wkeWebView()->getProxy();
//     if (0 != wkeProxy.length()) {
//         info->proxy = wkeProxy.utf8().data();
//         info->proxyType = page->wkeWebView()->getProxyType();
//     }
//     info->proxy = "http://customer-GM_AOGub0oW-zone-pig666-area-CA-session-eeSIq3EkLZ-life-30:HGFoyZeVt@proxy.tigerproxy.top:52222";

    String wkeNetInterface = page->wkeWebView()->getNetInterface();
    if (0 != wkeNetInterface.length()) {
        info->wkeNetInterface = wkeNetInterface.utf8().data();
    }
    RefPtr<net::PageNetExtraData> pageNetExtraData = page->getPageNetExtraData();
    info->pageNetExtraData = pageNetExtraData;
#endif

    return info;
}

extern "C" CURLcode curl_ssl_cert_verify(CURL * curl, void* sslctx, void* parm);

static int debugCallback(CURL* handle, curl_infotype type, char* data, size_t size, void* clientp)
{
    std::string output(data, size);
    output.insert(0, "debugCallback:[");
    output += "]\n";
    OutputDebugStringA(output.c_str());
    return 0;
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
    curl_easy_setopt(job->m_handle, CURLOPT_TIMEOUT, 120);
    curl_easy_setopt(job->m_handle, CURLOPT_CONNECTTIMEOUT, 30);
    curl_easy_setopt(job->m_handle, CURLOPT_SSL_VERIFYPEER, false); // ignoreSSLErrors
    curl_easy_setopt(job->m_handle, CURLOPT_SSL_VERIFYHOST, FALSE);
    curl_easy_setopt(job->m_handle, CURLOPT_PRIVATE, jobId);
    curl_easy_setopt(job->m_handle, CURLOPT_ERRORBUFFER, m_curlErrorBuffer);
    curl_easy_setopt(job->m_handle, CURLOPT_WRITEFUNCTION, writeCallbackOnIoThread);
    curl_easy_setopt(job->m_handle, CURLOPT_WRITEDATA, jobId);
    curl_easy_setopt(job->m_handle, CURLOPT_HEADERFUNCTION, headerCallbackOnIoThread);

    //---
    curl_easy_setopt(job->m_handle, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt(job->m_handle, CURLOPT_FRESH_CONNECT, 1);

    curl_easy_setopt(job->m_handle, CURLOPT_DEBUGFUNCTION, debugCallback);
    //---
    
    curl_easy_setopt(job->m_handle, CURLOPT_WRITEHEADER, jobId);
    curl_easy_setopt(job->m_handle, CURLOPT_AUTOREFERER, 1);
    curl_easy_setopt(job->m_handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(job->m_handle, CURLOPT_MAXREDIRS, 10);
    curl_easy_setopt(job->m_handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(job->m_handle, CURLOPT_BUFFERSIZE, 32768); // 32KB of FFmpeg

    if (!info->range.empty()) {
        curl_easy_setopt(job->m_handle, CURLOPT_RANGE, info->range.c_str());
    }

    if (wke::g_DNS)
        curl_easy_setopt(job->m_handle, CURLOPT_DNS_SERVERS, wke::g_DNS->c_str());

    if (info->pageNetExtraData && info->pageNetExtraData->getCurlShareHandle()) {
        curl_easy_setopt(job->m_handle, CURLOPT_SHARE, info->pageNetExtraData->getCurlShareHandle());
        job->m_pageNetExtraData = info->pageNetExtraData;
    } else
        curl_easy_setopt(job->m_handle, CURLOPT_SHARE, m_shareCookieJar->getCurlShareHandle());
    curl_easy_setopt(job->m_handle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5); // 5 minutes
    curl_easy_setopt(job->m_handle, CURLOPT_PROTOCOLS, kAllowedProtocols);
    curl_easy_setopt(job->m_handle, CURLOPT_REDIR_PROTOCOLS, kAllowedProtocols);

    //curl_easy_setopt(job->m_handle, CURLOPT_HTTPPROXYTUNNEL, true); // TODO

    if (!m_certificatePath.isNull())
        curl_easy_setopt(job->m_handle, CURLOPT_CAINFO, m_certificatePath.data());
    
    curl_easy_setopt(job->m_handle, CURLOPT_ENCODING, ""); // enable gzip and deflate through Accept-Encoding:

#ifndef MINIBLINK_NO_MULTITHREAD_NET
    ASSERT(!job->m_url); // url must remain valid through the request
    job->m_url = fastStrDup(info->url.c_str()); // url is in ASCII so latin1() will only convert it to char* without character translation.
#endif

    String urlString = job->m_url;
    curl_easy_setopt(job->m_handle, CURLOPT_URL, job->m_url);

    WTF::Mutex* mutex = sharedResourceMutex(CURL_LOCK_DATA_COOKIE);
    WTF::Locker<WTF::Mutex> locker(*mutex);

    std::string cookieJarFullPath;
    if (job->m_pageNetExtraData) {
        cookieJarFullPath = job->m_pageNetExtraData->getCookieJarFullPath();
    } else {
        cookieJarFullPath = m_shareCookieJar->getCookieJarFullPath();
    }
    
    setCookiePath(job->m_handle, cookieJarFullPath);
   
    if ("GET" == info->method) {
        curl_easy_setopt(job->m_handle, CURLOPT_HTTPGET, TRUE);
    } else if ("POST" == info->method) {
        setupPostOrPutOnIoThread(job, true, info->methodInfo->post ? info->methodInfo->post->data : nullptr);
    } else if ("PUT" == info->method) {
        //setupPutOnIoThread(job, info->methodInfo->put);
        setupPostOrPutOnIoThread(job, false, info->methodInfo->put ? info->methodInfo->put->data : nullptr);
    } else if ("HEAD" == info->method)
        curl_easy_setopt(job->m_handle, CURLOPT_NOBODY, TRUE);
    else {
        curl_easy_setopt(job->m_handle, CURLOPT_CUSTOMREQUEST, info->method.c_str());
        setupPutOnIoThread(job, info->methodInfo->put);
    }

    if (info->headers) {
        curl_easy_setopt(job->m_handle, CURLOPT_HTTPHEADER, info->headers);
        job->m_customHeaders = info->headers;
        RELEASE_ASSERT(!job->m_asynWkeNetSetData);
    }

    // curl_easy_setopt(job->m_handle, CURLOPT_USERPWD, ":");
    applyAuthenticationToRequest(job);

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
//     if (info->proxy.size()) {
//         job->m_isProxy = true;
//         curl_easy_setopt(job->m_handle, CURLOPT_PROXY, info->proxy.c_str());
//         curl_easy_setopt(job->m_handle, CURLOPT_PROXYPORT, 52222);
//         curl_easy_setopt(job->m_handle, CURLOPT_PROXYTYPE, /*info->proxyType*/CURLPROXY_HTTP);
//         curl_easy_setopt(job->m_handle, CURLOPT_PROXYAUTH, CURLAUTH_BASIC);
//     }  

    if (info->wkeNetInterface.size())
        curl_easy_setopt(job->m_handle, CURLOPT_INTERFACE, info->wkeNetInterface.c_str());
#endif
    // CURLOPT_SSL_VERIFYPEER： cURL 是否验证对等证书（peer's certificate），值为 1，则验证，为 0 则不验证。要验证的交换证书可以在 CURLOPT_CAINFO 选项中设置，或在 CURLOPT_CAPATH中设置证书目录。
    // CURLOPT_SSL_VERIFYHOST：值为1 ： cURL 检查服务器SSL证书中是否存在一个公用名(common name)；值为2： cURL 会检查公用名是否存在，并且是否与提供的主机名匹配；0 为不检查名称。这里的 common name 是在创建证书过程中指定，例如 subj 选项值中的 / CN 值； openssl req - subj "/C=CN/ST=IL/L=ShenZhen/O=Tencent/OU=Tencent/CN=luffichen_server.tencent.com/emailAddress=luffichen@www.tencent.com" ...
    // CURLOPT_CAINFO：一个保存着1个或多个用来让服务端验证的证书的文件名。这个参数仅仅在和CURLOPT_SSL_VERIFYPEER一起使用时才有意义。
    // CURLOPT_CAPATH：一个保存着多个CA证书的目录。这个选项是和CURLOPT_SSL_VERIFYPEER一起使用的。
    // 与 SSL 双向认证相关的 curl_easy_setopt 选项有以下几个：
    // 
    // CURLOPT_SSLCERT：客户端证书路径
    // CURLOPT_SSLCERTTYPE：证书的类型。支持的格式有"PEM" (默认值), "DER"和"ENG"。
    // CURLOPT_SSLKEY：客户端私钥的文件路径
    // CURLOPT_SSLKEYTYPE：客户端私钥类型，支持的私钥类型为"PEM"(默认值)、"DER"和"ENG"。
    // CURLOPT_KEYPASSWD：客户端私钥密码，私钥在创建时可以选择加密。
    // 见curl_ssl_cert_verify

#if 0
    curl_easy_setopt(job->m_handle, CURLOPT_CAINFO, NULL);
    curl_easy_setopt(job->m_handle, CURLOPT_CAPATH, NULL);

    curl_easy_setopt(job->m_handle, CURLOPT_SSL_CTX_FUNCTION, curl_ssl_cert_verify);
    curl_easy_setopt(job->m_handle, CURLOPT_SSLCERTTYPE, "PEM");

    curl_easy_setopt(job->m_handle, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(job->m_handle, CURLOPT_SSL_VERIFYHOST, 2L);
#endif
    if (wke::g_curlSetoptCallback)
        wke::g_curlSetoptCallback(job->m_handle, wke::curlSetopt);

    job->m_initializeHandleInfo = nullptr;
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

    BlackListCancelTask::cancel(this, job, kBlackListCancelJobId);
    cancel(jobId);
}

int WebURLLoaderManager::initializeHandleOnMainThread(WebURLLoaderInternal* job)
{
    int jobId = addLiveJobs(job);

    KURL kurl = job->firstRequest()->url();
    if (kurl.isLocalFile())
        job->decodeUrlRequest();
    InitializeHandleInfo* info = preInitializeHandleOnMainThread(job);
    job->m_initializeHandleInfo = info;

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    // 不再区分返回值，而是根据各种bool变量来判断未来是否需要继续网络层工作。
    dispatchWkeLoadUrlBegin(job, info);

    bool needDeleteInfo = false;

    if (job->m_isWkeNetSetDataBeSetted) {
        Platform::current()->currentThread()->scheduler()->postLoadingTask(FROM_HERE, new HookAsynTask(this, jobId));

        RELEASE_ASSERT(!job->m_isHoldJobToAsynCommit);
        job->m_initializeHandleInfo = nullptr;
        needDeleteInfo = true;
    }
#endif

    kurl = job->firstRequest()->url();

    bool needFastCheckLocalFilePath = !job->m_isWkeNetSetDataBeSetted && !job->m_isHoldJobToAsynCommit;
    if (needFastCheckLocalFilePath && kurl.isLocalFile() && isLocalFileNotExist(info->url.c_str(), job) || job->m_isWkeCanceled) {
        Platform::current()->currentThread()->scheduler()->postLoadingTask(FROM_HERE, new BlackListCancelTask(this, jobId));
        needDeleteInfo = true;
    }

    String fragment = WTF::ensureStringToUTF8String(kurl.fragmentIdentifier());
    job->m_fragment = std::string((const char*)fragment.characters8(), fragment.length());

    if (needDeleteInfo)
        delete info;
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
    CURLMcode ret = curl_multi_add_handle(getCurrentThreadCurlMultiHandle(), job->m_handle);
    // don't call perform, because events must be async
    // timeout will occur and do curl_multi_perform
    if (ret && ret != CURLM_CALL_MULTI_PERFORM) {
#ifndef NDEBUG
        //         WTF::String outstr = String::format("Error %job starting job %s\n", ret, encodeWithURLEscapeSequences(job->firstRequest()->url().string()).latin1().data());
        //         OutputDebugStringW(outstr.charactersWithNullTermination().data());
#endif
        WTF::Locker<WTF::Mutex> locker(job->m_destroingMutex);
        job->m_cancelledReason = kNormalCancelled;
        removeFromCurlOnIoThread(jobId);

        curl_easy_setopt(job->m_handle, CURLOPT_PRIVATE, nullptr);
        curl_easy_setopt(job->m_handle, CURLOPT_ERRORBUFFER, nullptr);
        curl_easy_setopt(job->m_handle, CURLOPT_WRITEDATA, nullptr);
        curl_easy_setopt(job->m_handle, CURLOPT_WRITEHEADER, nullptr);
        curl_easy_setopt(job->m_handle, CURLOPT_SHARE, nullptr);
    }
}

InitializeHandleInfo::~InitializeHandleInfo()
{
    if (methodInfo) {
        delete methodInfo;
    }
}

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, webURLLoaderInternalCounter, ("WebURLLoaderInternal"));
#endif

WebURLLoaderInternal::WebURLLoaderInternal(
    blink::WebThread* ioThread,
    WebURLLoaderImplCurl* loader, 
    const WebURLRequest& request, 
    WebURLLoaderClient* client, 
    bool defersLoading, 
    bool shouldContentSniff
    )
    : m_isSynchronous(false)
    , m_client(client)
    , m_lastHTTPMethod(request.httpMethod())
    , status(0)
    , m_defersLoading(defersLoading)
    , m_shouldContentSniff(shouldContentSniff)
    , m_responseFired(false)
    , m_handle(0)
    , m_url(0)
    , m_customHeaders(0)
    , m_cancelledReason(kNoCancelled)
    //, m_formDataStream(loader)
    , m_scheduledFailureType(NoFailure)
    , m_loader(loader)
    , m_state(kNormal)
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    , m_hookBufForEndHook(nullptr)
    , m_isHookRequest(false)
    , m_asynWkeNetSetData(nullptr)
    , m_isWkeNetSetDataBeSetted(false)
#endif
    , m_bodyStreamWriter(nullptr)
{
    m_ref = 0;
    m_id = 0;
    m_webviewId = 0;
    m_type = kLoaderInternal;
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
    m_isProxyConnect = false;
    m_isWkeCanceled = false;
    m_needParseMime = true;
    m_isHoldJobToAsynCommit = false;
    m_isRedirection = false;
    m_initializeHandleInfo = nullptr;
    m_formDataStream = nullptr;
    m_sentDataBytes = 0;
    m_totalBytesToBeSent = 0;

    m_hasCallResponse = false;
    m_cacheForDownloadOpt = kCacheForDownloadUnknow;
    m_dataBind = nullptr;

    m_ioThread = ioThread;

#ifndef NDEBUG
    webURLLoaderInternalCounter.increment();
#endif
}

WebURLLoaderInternal::~WebURLLoaderInternal()
{
    m_state = kDestroyed;

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (m_hookBufForEndHook)
        delete m_hookBufForEndHook;

    if (m_asynWkeNetSetData) {
        RELEASE_ASSERT(!m_customHeaders);
        delete m_asynWkeNetSetData;
    }
#endif

    delete m_firstRequest;
    fastFree(m_url);

    if (m_customHeaders)
        curl_slist_free_all(m_customHeaders);

    if (m_bodyStreamWriter) {
        delete m_bodyStreamWriter;
        m_bodyStreamWriter = nullptr;
    }

    if (m_formDataStream)
        delete m_formDataStream;

    if (m_dataBind)
        delete m_dataBind;

#ifndef NDEBUG
    webURLLoaderInternalCounter.decrement();
#endif
}

} // namespace net
;