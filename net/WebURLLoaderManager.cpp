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
#include "net/WebURLLoaderManagerUtil.h"
#include "net/WebURLLoaderManagerMainTask.h"
#include "net/FlattenHTTPBodyElement.h"
#include "net/WebURLLoaderManagerSetupInfo.h"
#include "net/WebURLLoaderManagerAsynTask.h"
#include "net/InitializeHandleInfo.h"
#include "net/HeaderVisitor.h"
#include "wke/wkeNetHook.h"
#include "third_party/WebKit/Source/wtf/Threading.h"
#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/text/CString.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include <errno.h>
#include <stdio.h>

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
#include "wke/wkeWebView.h"
extern bool g_isDecodeUrlRequest;
#endif
#include "wtf/RefCountedLeakCounter.h"

using namespace blink;

extern WKE_FILE_OPEN g_pfnOpen;
extern WKE_FILE_CLOSE g_pfnClose;

namespace net {

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
        free(m_cookieJarFileName);
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
    if (!job || job->isCancelled())
        return 0;

    // We should never be called when deferred loading is activated.
    ASSERT(!job->m_defersLoading);

    size_t totalSize = size * nmemb;
    if (checkIsProxyHead(job, ptr, totalSize))
        return totalSize;

    WebURLLoaderManagerMainTask::pushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kHeaderCallback, ptr, size, nmemb, totalSize);
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

    if (!job->m_formDataStream->hasMoreElements())
        return 0;

    size_t sent = job->m_formDataStream->read(ptr, size, nmemb);

    // Something went wrong so cancel the job.
    if (!sent && job->loader())
        job->loader()->cancel();

    return sent;

//     size_t sentSize = job->m_postBytes.size() - job->m_postBytesReadOffset;
//     if (0 == sentSize)
//         return 0;
// 
//     if (size * nmemb <= sentSize)
//         sentSize = size * nmemb;
// 
//     memcpy(ptr, job->m_postBytes.data() + job->m_postBytesReadOffset, sentSize);
//     job->m_postBytesReadOffset += sentSize;
//     ASSERT(job->m_postBytesReadOffset <= job->m_postBytes.size());
// 
//     return sentSize;
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
                WebURLLoaderManagerMainTask::pushTask(jobId, WebURLLoaderManagerMainTask::TaskType::kHandleLocalReceiveResponse, nullptr, 0, 0, 0);
                if (job->isCancelled()) {
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

static void setupFormDataOnIoThread(WebURLLoaderInternal* job, SetupDataInfo* info)
{
    if (info) {
        if (info->islongLong)
            curl_easy_setopt(job->m_handle, info->sizeOption, (long long)info->size);
        else
            curl_easy_setopt(job->m_handle, info->sizeOption, (int)info->size);
    }

    ASSERT(!job->m_formDataStream);
    job->m_formDataStream = new FlattenHTTPBodyElementStream(info->flattenElements);
    curl_easy_setopt(job->m_handle, CURLOPT_READFUNCTION, readCallbackOnIoThread);
    curl_easy_setopt(job->m_handle, CURLOPT_READDATA, job->m_id);

    curl_easy_setopt(job->m_handle, CURLOPT_IOCTLFUNCTION, ioctlCallbackOnIoThread);
    curl_easy_setopt(job->m_handle, CURLOPT_IOCTLDATA, job->m_id);
}

static void flattenHTTPBodyBlobElement(const WebString& blobUUID, curl_off_t* size, WTF::Vector<FlattenHTTPBodyElement*>* flattenElements)
{
    FlattenHTTPBodyElement* flattenElement = nullptr;
    WebBlobRegistryImpl* blobReg = (WebBlobRegistryImpl*)blink::Platform::current()->blobRegistry();
    net::BlobDataWrap* blobData = blobReg->getBlobDataFromUUID(blobUUID);
    if (!blobData)
        return;

    long long fileSizeResult = 0;
    const Vector<blink::WebBlobData::Item*>& items = blobData->items();
    for (size_t i = 0; i < items.size(); ++i) {
        blink::WebBlobData::Item* item = items[i];
        if (blink::WebBlobData::Item::TypeData == item->type) {
            flattenElement = new FlattenHTTPBodyElement();
            flattenElement->type = WebHTTPBody::Element::TypeData;
            flattenElement->data.append(item->data.data(), item->data.size());
            flattenElements->append(flattenElement);
            *size += item->data.size();
        } else if (blink::WebBlobData::Item::TypeFile == item->type) {
            if (getFileSize(item->filePath, fileSizeResult)) {
                *size += fileSizeResult;

                flattenElement = new FlattenHTTPBodyElement();
                flattenElement->type = WebHTTPBody::Element::TypeFile;
                Vector<UChar> filePath = WTF::ensureUTF16UChar(item->filePath, true);
                flattenElement->filePath = filePath.data();
                flattenElements->append(flattenElement);
            }
        } else if (blink::WebBlobData::Item::TypeFileSystemURL == item->type) {
            blink::KURL fileSystemURL = item->fileSystemURL;
            String fileSystemPath(fileSystemURL.getUTF8String());
            if (fileSystemPath.startsWith("file:///"))
                fileSystemPath.remove(0, sizeof("file:///") - 1);
            fileSystemPath.replace("/", "\\");
            if (!getFileSize(fileSystemPath, fileSizeResult))
                continue;

            *size += fileSizeResult;

            flattenElement = new FlattenHTTPBodyElement();
            flattenElement->type = WebHTTPBody::Element::TypeFile;
            Vector<UChar> filePath = WTF::ensureUTF16UChar(fileSystemPath, true);
            flattenElement->filePath = filePath.data();
            flattenElements->append(flattenElement);
        } else if (blink::WebBlobData::Item::TypeBlob == item->type) {
            flattenHTTPBodyBlobElement(item->blobUUID, size, flattenElements);
        }
    }
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
    SetupDataInfo* result = new SetupDataInfo();
    
    long long fileSizeResult = 0;
    for (size_t i = 0; i < httpBody.elementCount(); ++i) {
        blink::WebHTTPBody::Element element;
        if (!httpBody.elementAt(i, element))
            continue;

        FlattenHTTPBodyElement* flattenElement = nullptr;
        if (blink::WebHTTPBody::Element::TypeFile == element.type) {
            if (getFileSize(element.filePath, fileSizeResult)) {
                if (fileSizeResult > maxCurlOffT) {
                    // File size is too big for specifying it to cURL
                    chunkedTransfer = true;
                    break;
                }
                size += fileSizeResult;
            } else {
                chunkedTransfer = true;
                break;
            }

            flattenElement = new FlattenHTTPBodyElement();
            flattenElement->type = blink::WebHTTPBody::Element::TypeFile;
            Vector<UChar> filePath = WTF::ensureUTF16UChar(element.filePath, true);
            flattenElement->filePath = filePath.data();
            result->flattenElements.append(flattenElement);
        } else if (WebHTTPBody::Element::TypeData == element.type) {
            size += element.data.size();

            flattenElement = new FlattenHTTPBodyElement();
            flattenElement->type = blink::WebHTTPBody::Element::TypeData;
            flattenElement->data.append(element.data.data(), element.data.size());
            result->flattenElements.append(flattenElement);
        } else if (WebHTTPBody::Element::TypeBlob == element.type) {
            flattenHTTPBodyBlobElement(element.blobUUID, &size, &result->flattenElements);
        }
    }

    // dispatchPostBodyToWke(job, &result->flattenElements);

    // cURL guesses that we want chunked encoding as long as we specify the header
    if (chunkedTransfer)
        *headers = curl_slist_append(*headers, "Transfer-Encoding: chunked");
    else {
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
    *headers = curl_slist_append(*headers, "Expect:"); // Disable the Expect: 100 continue header

    size_t numElements = getFormElementsCount(job);
    if (!numElements)
        return nullptr;

    SetupPutInfo* result = new SetupPutInfo();
    result->data = setupFormDataOnMainThread(job, CURLOPT_INFILESIZE_LARGE, headers);
    return result;
}

static void setupPostOnIoThread(WebURLLoaderInternal* job, SetupPostInfo* info)
{
    curl_easy_setopt(job->m_handle, CURLOPT_POST, true);

    if (!info) {
        curl_easy_setopt(job->m_handle, CURLOPT_POSTFIELDSIZE, 0);
        return;
    }

    if (info->data && 1 == info->data->flattenElements.size()) {
        FlattenHTTPBodyElement* element = info->data->flattenElements[0];
        if (WebHTTPBody::Element::TypeData == element->type || WebHTTPBody::Element::TypeBlob == element->type) {
            curl_easy_setopt(job->m_handle, CURLOPT_POSTFIELDSIZE, element->data.size());
            curl_easy_setopt(job->m_handle, CURLOPT_COPYPOSTFIELDS, element->data.data());

            delete element;
            return;
        }
    }

    if (info->data)
        setupFormDataOnIoThread(job, info->data);
}

static SetupPostInfo* setupPostOnMainThread(WebURLLoaderInternal* job, struct curl_slist** headers)
{
    // *headers = curl_slist_append(*headers, "Expect:100-continue"); // Disable the Expect: 100 continue header

    size_t numElements = getFormElementsCount(job);
    if (!numElements)
        return nullptr;

    SetupPostInfo* result = new SetupPostInfo();
    result->data = setupFormDataOnMainThread(job, CURLOPT_POSTFIELDSIZE_LARGE, headers);
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
        if (!job || job->isCancelled())
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
    //if (WTF::kNotFound != url.find("nav_sprite_v") || WTF::kNotFound != url.find("products_sprites")) 
    {
        String outString = String::format("addAsynchronousJob : %d, %s\n", m_liveJobs.size(), WTF::ensureStringToUTF8(url, true).data());
        OutputDebugStringW(outString.charactersWithNullTermination().data());
    }
#endif

    if (g_isDecodeUrlRequest) {
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
        Platform::current()->currentThread()->scheduler()->postLoadingTask(FROM_HERE, new HandleDataURLTask(this, jobId)); // postLoadingTask
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

void WebURLLoaderManager::continueJob(WebURLLoaderInternal* job)
{
    if (job->m_isWkeNetSetDataBeSetted) {
        Platform::current()->currentThread()->scheduler()->postLoadingTask(FROM_HERE, new WkeAsynTask(this, job->m_id));
        return;
    }
    m_thread->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::initializeHandleOnIoThread, this, job->m_id, job->m_initializeHandleInfo));
    m_thread->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::startOnIoThread, this, job->m_id));

    IoTask* task = new IoTask(this, m_thread, false);
    m_thread->postTask(FROM_HERE, task);
}

void WebURLLoaderManager::cancelWithHookRedirect(WebURLLoaderInternal* job)
{
    doCancel(job, kHookRedirectCancelled);
}

void WebURLLoaderManager::doCancel(WebURLLoaderInternal* job, CancelledReason cancelledReason)
{
    WTF::Locker<WTF::Mutex> locker(job->m_destroingMutex);
    bool cancelled = job->isCancelled();

    RELEASE_ASSERT(kNoCancelled != cancelledReason);
    //RELEASE_ASSERT(!(kHookRedirectCancelled == job->m_cancelledReason && kNormalCancelled == cancelledReason));

    if (!(kHookRedirectCancelled == job->m_cancelledReason && kHookRedirectCancelled != cancelledReason)) {
        job->m_cancelledReason = cancelledReason;
    }

    if (WebURLLoaderInternal::kDestroying != job->m_state && !cancelled)
        m_thread->postTask(FROM_HERE, WTF::bind(&WebURLLoaderManager::removeFromCurlOnIoThread, this, job->m_id));
}

void WebURLLoaderManager::cancel(int jobId)
{
    ASSERT(WTF::isMainThread());

    AutoLockJob autoLockJob(this, jobId);
    WebURLLoaderInternal* job = autoLockJob.lock();
    if (!job)
        return;

    doCancel(job, kNormalCancelled);
}

void WebURLLoaderManager::cancelAll()
{
    WTF::Locker<WTF::Mutex> locker(m_liveJobsMutex);

    WTF::HashMap<int, WebURLLoaderInternal*>::iterator it = m_liveJobs.begin();
    for (; it != m_liveJobs.end(); ++it) {
        WebURLLoaderInternal* job = it->value;
        int jobId = it->key;
        doCancel(job, kNormalCancelled);
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

    Vector<char> urlBuf = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    WebPage* page = requestExtraData->page;
    if (page->wkeHandler().loadUrlBeginCallback) {
        
        if (page->wkeHandler().loadUrlBeginCallback(
            page->wkeWebView(), 
            page->wkeHandler().loadUrlBeginCallbackParam,
            urlBuf.data(),
            job)) {
            if (job->m_asynWkeNetSetData && kNormalCancelled != job->m_cancelledReason)
                WebURLLoaderManager::sharedInstance()->didReceiveDataOrDownload(job, static_cast<char*>(job->m_asynWkeNetSetData), job->m_asynWkeNetSetDataLength, 0);

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

        wkeLoadUrlEndCallback loadUrlEndCallback = page->wkeHandler().loadUrlEndCallback;
        void* loadUrlEndCallbackParam = page->wkeHandler().loadUrlEndCallbackParam;
        if (job->m_isHookRequest && loadUrlEndCallback)
            loadUrlEndCallback(page->wkeWebView(), loadUrlEndCallbackParam, urlBuf.data(), job, job->m_hookBufForEndHook, job->m_hookLength);

        if (job->m_hookBufForEndHook)
            didReceiveDataOrDownload(job, static_cast<char*>(job->m_hookBufForEndHook), job->m_hookLength, 0);
        handleDidFinishLoading(job, 0, 0);
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

//  dispatchPostBodyToWke(job, &result->flattenElements);

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
static bool dispatchWkeLoadUrlBegin(WebURLLoaderInternal* job, InitializeHandleInfo* info)
{
    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    if (!requestExtraData)
        return false;

    WebPage* page = requestExtraData->page;
    if (!page->wkeHandler().loadUrlBeginCallback)
        return false;

    Vector<char> urlBuf = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);
    void* param = page->wkeHandler().loadUrlBeginCallbackParam;
    bool b = page->wkeHandler().loadUrlBeginCallback(page->wkeWebView(), param, urlBuf.data(), job);

    return job->m_isWkeNetSetDataBeSetted || b;
}
#endif

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
    curl_easy_setopt(job->m_handle, CURLOPT_USERPWD, userpass.utf8().data());
    //curl_easy_setopt(job->m_handle, CURLOPT_USERPWD, ":");
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

        // Determine the MIME type based on the path.
        job->m_response.setMIMEType(MIMETypeRegistry::getMIMETypeForPath(url));
    }

    info->url = WTF::ensureStringToUTF8(urlString, true).data();

    info->method = job->firstRequest()->httpMethod().utf8();

    String contentType = job->firstRequest()->httpHeaderField("Content-Type");
    if (WTF::kNotFound != url.host().find("huobi.pro") && "POST" == info->method && job->firstRequest()->httpBody().isNull())
        job->firstRequest()->setHTTPHeaderField("Content-Type", ""); // 修复火币网登录不了的bug

    curl_slist* headers = nullptr;
    HeaderVisitor visitor(&headers);
    job->firstRequest()->visitHTTPHeaderFields(&visitor);

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

    if (m_cookieJarFileName && '\0' != m_cookieJarFileName[0]) {
        curl_easy_setopt(job->m_handle, CURLOPT_COOKIEJAR, m_cookieJarFileName);
        curl_easy_setopt(job->m_handle, CURLOPT_COOKIEFILE, m_cookieJarFileName);
    }

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
        RELEASE_ASSERT(!job->m_asynWkeNetSetData);
    }

    // curl_easy_setopt(job->m_handle, CURLOPT_USERPWD, ":");

    applyAuthenticationToRequest(job, job->firstRequest());

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (info->proxy.size()) {
        job->m_isProxy = true;
        curl_easy_setopt(job->m_handle, CURLOPT_PROXY, info->proxy.c_str());
        curl_easy_setopt(job->m_handle, CURLOPT_PROXYTYPE, info->proxyType);
    }  

    if (info->wkeNetInterface.size())
        curl_easy_setopt(job->m_handle, CURLOPT_INTERFACE, info->wkeNetInterface.c_str());
#endif

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

    BlackListCancelTask::cancel(job, kBlackListCancelJobId);
    cancel(jobId);
}

int WebURLLoaderManager::initializeHandleOnMainThread(WebURLLoaderInternal* job)
{
    int jobId = addLiveJobs(job);

    InitializeHandleInfo* info = preInitializeHandleOnMainThread(job);
    job->m_initializeHandleInfo = info;

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (dispatchWkeLoadUrlBegin(job, info)) {
        if (job->m_isWkeNetSetDataBeSetted)
            Platform::current()->currentThread()->scheduler()->postLoadingTask(FROM_HERE, new WkeAsynTask(this, jobId)); // postLoadingTask
        
        if (!job->m_isHoldJobToAsynCommit) {
            job->m_initializeHandleInfo = nullptr;
            delete info;
        }
        return jobId;
    }
#endif

    KURL kurl = job->firstRequest()->url();
    if (kurl.isLocalFile() && isLocalFileNotExist(info->url.c_str(), job)) {
        Platform::current()->currentThread()->scheduler()->postLoadingTask(FROM_HERE, new BlackListCancelTask(this, jobId));
        delete info;
        return jobId;
    }

    job->m_initializeHandleInfo = info;
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
        job->m_cancelledReason = kNormalCancelled;
        removeFromCurlOnIoThread(jobId);

        curl_easy_setopt(job->m_handle, CURLOPT_PRIVATE, nullptr);
        curl_easy_setopt(job->m_handle, CURLOPT_ERRORBUFFER, nullptr);
        curl_easy_setopt(job->m_handle, CURLOPT_WRITEDATA, nullptr);
        curl_easy_setopt(job->m_handle, CURLOPT_WRITEHEADER, nullptr);
        curl_easy_setopt(job->m_handle, CURLOPT_SHARE, nullptr);
    }
}

InitializeHandleInfo::~InitializeHandleInfo() {
    if (methodInfo) {
        delete methodInfo;
    }
}

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, webURLLoaderInternalCounter, ("WebURLLoaderInternal"));
#endif

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
    , m_cancelledReason(kNoCancelled)
    //, m_formDataStream(loader)
    , m_scheduledFailureType(NoFailure)
    , m_loader(loader)
    , m_state(kNormal)
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    , m_hookBufForEndHook(nullptr)
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
    m_isHoldJobToAsynCommit = false;
    m_initializeHandleInfo = nullptr;
    m_formDataStream = nullptr;

#ifndef NDEBUG
    webURLLoaderInternalCounter.increment();
#endif
}

WebURLLoaderInternal::~WebURLLoaderInternal()
{
    m_state = kDestroyed;
    delete m_firstRequest;

    fastFree(m_url);

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (m_hookBufForEndHook)
        free(m_hookBufForEndHook);

    if (m_asynWkeNetSetData) {
        RELEASE_ASSERT(!m_customHeaders);
        free(m_asynWkeNetSetData);
    }
#endif

    if (m_customHeaders)
        curl_slist_free_all(m_customHeaders);

    if (m_bodyStreamWriter) {
        delete m_bodyStreamWriter;
        m_bodyStreamWriter = nullptr;
    }

    if (m_formDataStream)
        delete m_formDataStream;

#ifndef NDEBUG
    webURLLoaderInternalCounter.decrement();
#endif
}

} // namespace net
;