
#ifndef net_WebURLLoaderManagerMainTask_h
#define net_WebURLLoaderManagerMainTask_h

#include "net/WebURLLoaderInternal.h"
#include "net/WebURLLoaderManagerUtil.h"
#include "net/WebURLLoaderManagerAsynTask.h"
#include "net/RequestExtraData.h"
#include "content/browser/WebPage.h"
#include "third_party/WebKit/Source/wtf/Threading.h"
#include "third_party/WebKit/Source/platform/network/HTTPParsers.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebScheduler.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "wke/wkeWebView.h"

void WKE_CALL_TYPE wkeDeleteWillSendRequestInfo(wkeWebView webWindow, wkeWillSendRequestInfo* info);

namespace net {

struct MainTaskArgs {
    void* ptr;
    size_t size;
    size_t nmemb;
    long httpCode;
    double contentLength;
    char* hdr;
    blink::WebURLError* resourceError;

    int ref;

    ~MainTaskArgs()
    {
        free(ptr);
        free(hdr);
        delete resourceError;
    }

    static MainTaskArgs* build(void* ptr, size_t size, size_t nmemb, size_t totalSize, CURL* handle, bool isProxyConnect)
    {
        MainTaskArgs* args = new MainTaskArgs();
        args->size = size;
        args->nmemb = nmemb;
        args->ptr = malloc(totalSize);
        args->resourceError = new blink::WebURLError();
        args->ref = 0;
        memcpy(args->ptr, ptr, totalSize);

        curl_easy_getinfo(handle, !isProxyConnect ? CURLINFO_RESPONSE_CODE : CURLINFO_HTTP_CONNECTCODE, &args->httpCode); // 只有使用了代理的Connect请求才需要特殊处理

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

class MainTaskRunner : public blink::WebThread::TaskObserver {
public:
    MainTaskRunner() : m_isDestroying(false) {}
    virtual ~MainTaskRunner() override {}
    virtual void willProcessTask() override;
    virtual void didProcessTask() override
    {
    }

    static void destroy();
    static void add(WebURLLoaderManagerMainTask* task)
    {
        if (!m_inst) {
            m_inst = new MainTaskRunner();
            blink::Platform::current()->mainThread()->addTaskObserver(m_inst);
        }
        
        m_inst->addTask(task);
    }

private:
    void destroyImpl();
    void addTask(WebURLLoaderManagerMainTask* task);

    static MainTaskRunner* m_inst;
    bool m_isDestroying;
    WTF::Mutex m_mutex;
    WTF::Vector<WebURLLoaderManagerMainTask*> m_list;
};

// 回调回main线程的task
class WebURLLoaderManagerMainTask : public blink::WebThread::Task {
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

        dispatchHook(job, m_type, m_args->ptr, m_args->size, m_args->nmemb);

        if (kRemoveFromCurl == m_type || kDidCancel == m_type) {
            autoLockJob.setNotDerefForDelete();
            release();
            return;
        }

        if (WebURLLoaderManager::sharedInstance()->isShutdown() || job->isCancelled())
            return;

        switch (m_type) {
        case kWriteCallback:
            handleWriteCallbackOnMainThread(m_args, job);
            break;
        case kHeaderCallback:
            handleHeaderCallbackOnMainThread(m_args, job);
            break;
        case kDidFinishLoading:
            if (job->m_hookBufForEndHook)
                WebURLLoaderManager::sharedInstance()->didReceiveDataOrDownload(job, job->m_hookBufForEndHook->data(), job->m_hookBufForEndHook->size(), 0);
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
            if (job->m_hookBufForEndHook)
                job->m_multipartHandle->contentReceived(job->m_hookBufForEndHook->data(), job->m_hookBufForEndHook->size());
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

    static void dispatchHook(WebURLLoaderInternal* job, TaskType type, void* ptr, size_t size, size_t nmemb)
    {
        if (kWriteCallback == type) {
            if (WebURLLoaderInternal::kCacheForDownloadNot != job->m_cacheForDownloadOpt) {
                job->m_dataCacheForDownload.append((char*)ptr, size * nmemb);
            }

            if (job->m_dataBind) {
                job->m_dataBind->recvCallback(job->m_dataBind->param, job, job->m_dataCacheForDownload.data(), job->m_dataCacheForDownload.size());
                job->m_dataCacheForDownload.clear();
            }
        } else if (kDidCancel == type) {
            if (job->m_dataBind) {
                job->m_dataBind->finishCallback(job->m_dataBind->param, job, WKE_LOADING_CANCELED);
                job->m_dataCacheForDownload.clear();
            }
        } else if (kDidFail == type) {
            if (job->m_dataBind) {
                job->m_dataBind->finishCallback(job->m_dataBind->param, job, WKE_LOADING_FAILED);
                job->m_dataCacheForDownload.clear();
            }
        } else if (kDidFinishLoading == type) {
            if (job->m_dataBind) {
                job->m_dataBind->finishCallback(job->m_dataBind->param, job, WKE_LOADING_SUCCEEDED);
                job->m_dataCacheForDownload.clear();
            }
        }
    }

    static MainTaskArgs* pushTask(int jobId, TaskType type, void* ptr, size_t size, size_t nmemb, size_t totalSize)
    {
        AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
        WebURLLoaderInternal* job = autoLockJob.lock();
        if (!job)
            return nullptr;

        MainTaskArgs* args = MainTaskArgs::build(ptr, size, nmemb, totalSize, job->m_handle, job->m_isProxyConnect);
        WebURLLoaderManagerMainTask* task = new WebURLLoaderManagerMainTask(jobId, type, args);

        if (job->m_isSynchronous)
            job->m_syncTasks.append(task);
        else {
            //blink::Platform::current()->mainThread()->scheduler()->postLoadingTask(FROM_HERE, task); // postLoadingTask
            MainTaskRunner::add(task);
        }
        return args;
    }

    static WebURLLoaderManagerMainTask* createTask(int jobId, TaskType type, void* ptr, size_t size, size_t nmemb, size_t totalSize)
    {
        AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
        WebURLLoaderInternal* job = autoLockJob.lock();
        if (!job)
            return nullptr;
        MainTaskArgs* args = MainTaskArgs::build(ptr, size, nmemb, totalSize, job->m_handle, job->m_isProxyConnect);
        WebURLLoaderManagerMainTask* task = new WebURLLoaderManagerMainTask(jobId, type, args);
        return task;
    }

    static size_t handleWriteCallbackOnMainThread(MainTaskArgs* args, WebURLLoaderInternal* job);
    static size_t handleHeaderCallbackOnMainThread(MainTaskArgs* args, WebURLLoaderInternal* job);
    static void handleLocalReceiveResponseOnMainThread(MainTaskArgs* args, WebURLLoaderInternal* job);
    static void handleHookRequestOnMainThread(WebURLLoaderInternal* job);

private:
    int m_jobId;
    TaskType m_type;
    MainTaskArgs* m_args;

    WebURLLoaderManagerMainTask(int jobId, TaskType type, MainTaskArgs* args)
        : m_jobId(jobId)
        , m_type(type)
        , m_args(args)
    {
    }
};

static void checkList(const WTF::Vector<WebURLLoaderManagerMainTask*>& tasks)
{
    for (size_t i = 0; i < tasks.size(); ++i) {
        WebURLLoaderManagerMainTask* task = tasks[i];
        RELEASE_ASSERT(!task);
    }
}

void MainTaskRunner::willProcessTask()
{
    for (size_t i = 0; ; ++i) {
        m_mutex.lock();
        if (i >= m_list.size()) {
            checkList(m_list);
            m_list.clear();
            m_mutex.unlock();
            break;
        }
        WebURLLoaderManagerMainTask* task = m_list[i];
        m_list[i] = nullptr;
        m_mutex.unlock();

        if (!task)
            continue;
        task->run();
        delete task;
    }
}

void MainTaskRunner::destroy()
{
    if (!m_inst)
        return;
    m_inst->destroyImpl();
}

void MainTaskRunner::destroyImpl()
{
    m_mutex.lock();
    m_isDestroying = true;

    size_t size = m_list.size();
    for (size_t i = 0; i < size; ++i) {
        WebURLLoaderManagerMainTask* task = m_list[i];
        delete task;
    }
    RELEASE_ASSERT(size == m_list.size());

    m_list.clear();
    m_mutex.unlock();
}

void MainTaskRunner::addTask(WebURLLoaderManagerMainTask* task)
{
    WTF::Locker<WTF::Mutex> locker(m_mutex);
    if (m_isDestroying) {
        delete task;
        return;
    }
    m_list.append(task);
}

wkeResourceType webURLRequestToResourceType(const blink::WebURLRequest& request)
{
    blink::WebURLRequest::RequestContext requestContext = request.requestContext();
    if (request.frameType() != blink::WebURLRequest::FrameTypeNone) {
        ASSERT(requestContext == blink::WebURLRequest::RequestContextForm ||
            requestContext == blink::WebURLRequest::RequestContextFrame ||
            requestContext == blink::WebURLRequest::RequestContextHyperlink ||
            requestContext == blink::WebURLRequest::RequestContextIframe ||
            requestContext == blink::WebURLRequest::RequestContextInternal ||
            requestContext == blink::WebURLRequest::RequestContextLocation);
        if (request.frameType() == WebURLRequest::FrameTypeTopLevel ||
            request.frameType() == WebURLRequest::FrameTypeAuxiliary) {
            return WKE_RESOURCE_TYPE_MAIN_FRAME;
        }
        if (request.frameType() == WebURLRequest::FrameTypeNested)
            return WKE_RESOURCE_TYPE_SUB_FRAME;
        DebugBreak();
        return WKE_RESOURCE_TYPE_SUB_RESOURCE;
    }

    switch (requestContext) {
        // Favicon
    case blink::WebURLRequest::RequestContextFavicon:
        return WKE_RESOURCE_TYPE_FAVICON;

        // Font
    case blink::WebURLRequest::RequestContextFont:
        return WKE_RESOURCE_TYPE_FONT_RESOURCE;

        // Image
    case blink::WebURLRequest::RequestContextImage:
    case blink::WebURLRequest::RequestContextImageSet:
        return WKE_RESOURCE_TYPE_IMAGE;

        // Media
    case blink::WebURLRequest::RequestContextAudio:
    case blink::WebURLRequest::RequestContextVideo:
        return WKE_RESOURCE_TYPE_MEDIA;

        // Object
    case blink::WebURLRequest::RequestContextEmbed:
    case blink::WebURLRequest::RequestContextObject:
        return WKE_RESOURCE_TYPE_OBJECT;

        // Ping
    case blink::WebURLRequest::RequestContextBeacon:
    case blink::WebURLRequest::RequestContextCSPReport:
    case blink::WebURLRequest::RequestContextPing:
        return WKE_RESOURCE_TYPE_PING;

        // Prefetch
    case blink::WebURLRequest::RequestContextPrefetch:
        return WKE_RESOURCE_TYPE_PREFETCH;

        // Script
    case blink::WebURLRequest::RequestContextImport:
    case blink::WebURLRequest::RequestContextScript:
        return WKE_RESOURCE_TYPE_SCRIPT;

        // Style
    case WebURLRequest::RequestContextXSLT:
    case WebURLRequest::RequestContextStyle:
        return WKE_RESOURCE_TYPE_STYLESHEET;

        // Subresource
    case blink::WebURLRequest::RequestContextDownload:
    case blink::WebURLRequest::RequestContextManifest:
    case blink::WebURLRequest::RequestContextSubresource:
    case blink::WebURLRequest::RequestContextPlugin:
        return WKE_RESOURCE_TYPE_SUB_RESOURCE;

        // TextTrack
    case blink::WebURLRequest::RequestContextTrack:
        return WKE_RESOURCE_TYPE_MEDIA;

        // Workers
    case blink::WebURLRequest::RequestContextServiceWorker:
        return WKE_RESOURCE_TYPE_SERVICE_WORKER;
    case blink::WebURLRequest::RequestContextSharedWorker:
        return WKE_RESOURCE_TYPE_SHARED_WORKER;
    case blink::WebURLRequest::RequestContextWorker:
        return WKE_RESOURCE_TYPE_WORKER;

        // Unspecified
    case blink::WebURLRequest::RequestContextInternal:
    case blink::WebURLRequest::RequestContextUnspecified:
        return WKE_RESOURCE_TYPE_SUB_RESOURCE;

        // XHR
    case WebURLRequest::RequestContextEventSource:
    case WebURLRequest::RequestContextFetch:
    case WebURLRequest::RequestContextXMLHttpRequest:
        return WKE_RESOURCE_TYPE_XHR;

        // These should be handled by the FrameType checks at the top of the
        // function.
    case blink::WebURLRequest::RequestContextForm:
    case blink::WebURLRequest::RequestContextHyperlink:
    case blink::WebURLRequest::RequestContextLocation:
    case blink::WebURLRequest::RequestContextFrame:
    case blink::WebURLRequest::RequestContextIframe:
        DebugBreak();
        return WKE_RESOURCE_TYPE_SUB_RESOURCE;

    default:
        DebugBreak();
        return WKE_RESOURCE_TYPE_SUB_RESOURCE;
    }
}

static bool isResourceTypeFrame(WebURLLoaderInternal* job)
{
    blink::WebURLRequest* request = job->firstRequest();
    if (!request)
        return false;
// 
//     blink::WebURLRequest::RequestContext requestContext = request->requestContext();
//     if (request->frameType() != blink::WebURLRequest::FrameTypeNone) {
//         ASSERT(requestContext == blink::WebURLRequest::RequestContextForm ||
//             requestContext == blink::WebURLRequest::RequestContextFrame ||
//             requestContext == blink::WebURLRequest::RequestContextHyperlink ||
//             requestContext == blink::WebURLRequest::RequestContextIframe ||
//             requestContext == blink::WebURLRequest::RequestContextInternal ||
//             requestContext == blink::WebURLRequest::RequestContextLocation);
// 
//         if (request->frameType() == blink::WebURLRequest::FrameTypeTopLevel || request->frameType() == blink::WebURLRequest::FrameTypeAuxiliary) {
//             return true; //  RESOURCE_TYPE_MAIN_FRAME;
//         }
//         if (request->frameType() == WebURLRequest::FrameTypeNested)
//             return true; //  RESOURCE_TYPE_SUB_FRAME
//         
//         ASSERT(false);
//         return false;
//     }
// 
//     return false;

    wkeResourceType type = webURLRequestToResourceType(*request);
    if (WKE_RESOURCE_TYPE_MAIN_FRAME == type || WKE_RESOURCE_TYPE_SUB_FRAME == type)
        return true;

    return false;
}

// content\public\common\resource_type.cc
// content\browser\loader\mime_type_resource_handler.cc
static bool isDownloadResponse(WebURLLoaderInternal* job, const AtomicString& contentType)
{
    if (!isResourceTypeFrame(job))
        return false;

    if (contentDispositionType(job->m_response.httpHeaderField("Content-Disposition")) == ContentDispositionAttachment)
        return true;

    if (contentType.isEmpty())
        return false;

    const char* disableDownloadMimes[] = {
        "text/css",
        "text/javascript",
        "text/plain",
        "text/html",
        "text/xml",
        "text/xsl",
        "image/png",
        "image/gif",
        "image/jpeg",
        "image/bmp",
        "image/webp",
        "image/x-icon",
        "image/svg+xml",
        "audio/ogg",
        "audio/midi",
        "audio/x-midi",
        "video/x-msvideo",
        "video/mpeg",
        "video/mp4",
        "video/x-ms-wmv",
        "font/woff2",
        "font/opentype",
        "application/xhtml+xml",
        "application/font-woff",
        "application/json",
        "application/xhtml+xml",
        "application/x-javascript",
        "application/javascript",
		"application/pdf",
        nullptr
    };
    for (int i = 0; ; ++i) {
        const char* type = disableDownloadMimes[i];
        if (!type)
            break;

        String contentMime = contentType.lower();
        
        if (contentMime.startsWith(type))
            return false;
    }

    return true;
}

#if ENABLE_WKE == 1

static bool dispatchDownloadToWke(WebPage* page, WebURLLoaderInternal* job, const utf8* url , const AtomicString& contentType, const String& downloadName)
{
    job->m_cacheForDownloadOpt = WebURLLoaderInternal::kCacheForDownloadYes;

    Vector<char> mimeBuf = WTF::ensureStringToUTF8((String)contentType, true);
    
    String contentDisposition;
    if (!downloadName.isNull() && !downloadName.isEmpty())
        contentDisposition = WTF::ensureStringToUTF8String(downloadName);
    else
        contentDisposition = job->m_response.httpHeaderField("Content-Disposition");
    Vector<char> contentDispositionBuf = WTF::ensureStringToUTF8(contentDisposition, true);
    
    wkeNetJobDataBind dataBind = { 0 };
    wkeDownload2Callback cb = page->wkeHandler().download2Callback;
    if (!cb) {
        job->m_cacheForDownloadOpt = WebURLLoaderInternal::kCacheForDownloadNot;
        return false;
    }

    size_t expectedContentLength = job->m_response.expectedContentLength();
    void* param = page->wkeHandler().download2CallbackParam;
    wkeDownloadOpt opt = cb(page->wkeWebView(), param, expectedContentLength, url, mimeBuf.data(), contentDispositionBuf.data(), job, &dataBind);
    if (kWkeDownloadOptCancel == opt) {
        job->m_cacheForDownloadOpt = WebURLLoaderInternal::kCacheForDownloadNot;
        return true;
    } else if (kWkeDownloadOptCacheData == opt) {
        job->m_dataBind = new wkeNetJobDataBind();
        *(job->m_dataBind) = dataBind;

        return false;
    }

    return true;
}

static bool dispatchResponseToWke(WebURLLoaderInternal* job, const AtomicString& contentType, bool isRedirect)
{
    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    if (!requestExtraData) { //没有的情况可能是客户端用导出接口发送的请求，也可能是即将关闭程序
        job->m_cacheForDownloadOpt = WebURLLoaderInternal::kCacheForDownloadNot;
        return false;
    }

    blink::WebLocalFrame* frame = requestExtraData->getFrame();
    if (!frame)
        return false;

    WebPage* page = requestExtraData->page;
    Vector<char> urlBuf = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);

    wkeTempCallbackInfo* temInfo = wkeGetTempCallbackInfo(nullptr);
    temInfo->size = sizeof(wkeTempCallbackInfo);
    temInfo->job = job;

    wkeWebFrameHandle frameHandle = wke::CWebView::frameIdTowkeWebFrameHandle(page, page->getFrameIdByBlinkFrame(frame));
    temInfo->frame = frameHandle;

    bool result = false;
    do {
        if (page->wkeHandler().netResponseCallback) {
            if (page->wkeHandler().netResponseCallback(page->wkeWebView(), page->wkeHandler().netResponseCallbackParam, urlBuf.data(), job)) {
                result = true;
                break;
            }
        }

        if (requestExtraData->isDownload() || (isDownloadResponse(job, contentType) && !isRedirect)) {
            if (dispatchDownloadToWke(page, job, urlBuf.data(), contentType, requestExtraData->getDownloadName())) {
                result = true;
                break;
            }

            if (page->wkeHandler().downloadCallback && page->wkeHandler().downloadCallback(page->wkeWebView(), page->wkeHandler().downloadCallbackParam, urlBuf.data())) {
                result = true;
                break;
            }
        } else {
            job->m_cacheForDownloadOpt = WebURLLoaderInternal::kCacheForDownloadNot;
            job->m_dataCacheForDownload.clear();
        }
    } while (false);

    if (result)
        frame->stopLoading();
    
    temInfo->size = 0;

    return result;
}

#endif

static void distpatchWkeWillSendRequest(WebURLLoaderInternal* job, const KURL* newURL, long httpCode)
{
    net::RequestExtraData* requestExtraData = (net::RequestExtraData*)job->firstRequest()->extraData();
    if (!requestExtraData)
        return;

    content::WebPage* page = requestExtraData->page;
    if (!page->wkeHandler().otherLoadCallback)
        return;

    Vector<UChar> url = WTF::ensureUTF16UChar(job->firstRequest()->url().string(), false);
    Vector<UChar> newUrl;
    if (newURL)
        newUrl = WTF::ensureUTF16UChar(newURL->getUTF8String(), false);
    Vector<UChar> method = WTF::ensureUTF16UChar(job->firstRequest()->httpMethod(), false);
    Vector<UChar> referrer = WTF::ensureUTF16UChar(job->firstRequest()->httpHeaderField(blink::WebString::fromUTF8("Referer")), false);
    
    wkeTempCallbackInfo* info = wkeGetTempCallbackInfo(page->wkeWebView());
    info->size = sizeof(wkeTempCallbackInfo);
    info->willSendRequestInfo = new wkeWillSendRequestInfo();
    info->willSendRequestInfo->url = wkeCreateStringW(url.data(), url.size());
    info->willSendRequestInfo->newUrl = newURL ? wkeCreateStringW(newUrl.data(), newUrl.size()) : nullptr;
    info->willSendRequestInfo->resourceType = webURLRequestToResourceType(*job->firstRequest());
    info->willSendRequestInfo->httpResponseCode = httpCode;
    info->willSendRequestInfo->method = wkeCreateStringW(method.data(), method.size());
    info->willSendRequestInfo->referrer = wkeCreateStringW(referrer.data(), referrer.size());
    info->willSendRequestInfo->headers = nullptr;

    page->wkeHandler().otherLoadCallback(page->wkeWebView(), page->wkeHandler().otherLoadCallbackParam,
        newURL ? WKE_DID_GET_REDIRECT_REQUEST : WKE_DID_GET_RESPONSE_DETAILS,
        info);

    wkeDeleteWillSendRequestInfo(page->wkeWebView(), info->willSendRequestInfo);
    info->willSendRequestInfo = nullptr;
    info->size = 0;
}

static void doRedirect(WebURLLoaderInternal* job, const String& location, MainTaskArgs* args, bool isRedirectByHttpCode)
{
    WebURLLoaderClient* client = job->client();
    KURL newURL = KURL((KURL)(job->firstRequest()->url()), location);

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    distpatchWkeWillSendRequest(job, &newURL, args->httpCode);

    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    if (!requestExtraData)
        return;
    WebPage* page = requestExtraData->page;
    wkeLoadUrlBeginCallback loadUrlBeginCallback = page->wkeHandler().loadUrlBeginCallback;
    void* param = page->wkeHandler().loadUrlBeginCallbackParam;

    if (loadUrlBeginCallback) {
        CString newURLBuf(newURL.getUTF8String().utf8());
        
        job->m_isRedirection = true;
        loadUrlBeginCallback(page->wkeWebView(), param, newURLBuf.data(), job);
        job->m_isRedirection = false;

        if (job->m_isWkeNetSetDataBeSetted) {
            if (job->m_customHeaders)
                curl_slist_free_all(job->m_customHeaders);
            job->m_customHeaders = nullptr;
            WebURLLoaderManager::sharedInstance()->cancelWithHookRedirect(job);
            Platform::current()->currentThread()->scheduler()->postLoadingTask(FROM_HERE, new HookAsynTask(WebURLLoaderManager::sharedInstance(), job->m_id, false));
            return;
        }
    }
#endif

    blink::WebURLRequest* redirectedRequest = new blink::WebURLRequest(*job->firstRequest());
    redirectedRequest->setURL(newURL);
    if (client && job->loader() && (WebURLLoaderInternal::kCacheForDownloadYes != job->m_cacheForDownloadOpt))
        client->willSendRequest(job->loader(), *redirectedRequest, job->m_response);

    if (isRedirectByHttpCode)
        job->m_response.initialize();

    job->resetFirstRequest(redirectedRequest);
}

static bool setHttpResponseDataToJobWhenDidReceiveResponseOnMainThread(WebURLLoaderInternal* job, MainTaskArgs* args)
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
    job->m_response.setMIMEType(extractMIMETypeFromMediaType(contentType).lower());

    String textEncodingName = extractCharsetFromMediaType(contentType);
//     if (textEncodingName.isNull() || textEncodingName.isEmpty())
//         textEncodingName = "utf-8";
    job->m_response.setTextEncodingName(textEncodingName);
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (dispatchResponseToWke(job, contentType, isHttpRedirect(args->httpCode)))
        return false;
#endif
    if (equalIgnoringCase((String)(job->m_response.mimeType()), "multipart/x-mixed-replace")) {
        String boundary;
        bool parsed = MultipartHandle::extractBoundary(job->m_response.httpHeaderField(WebString::fromUTF8("Content-Type")), boundary);
        if (parsed)
            job->m_multipartHandle = adoptPtr(new MultipartHandle(job, boundary));
    }

    if (job->m_effectiveUrl.empty())
        job->m_effectiveUrl = args->hdr;

    bool isRedirectByHttpCode = isHttpRedirect(args->httpCode);
    bool isRedirectByUrl = (!job->m_effectiveUrl.empty() && job->m_effectiveUrl != job->m_url); // 有时有代理时，url会变，但没有30x码

    job->m_effectiveUrl = args->hdr;

    // HTTP redirection 重定向
    if (isRedirectByHttpCode || isRedirectByUrl) {
        String location = job->m_response.httpHeaderField(WebString::fromUTF8("location"));
        String nonAuthoritativeReason = job->m_response.httpHeaderField(WebString::fromUTF8("Non-Authoritative-Reason"));
        
        if (isRedirectByHttpCode)
            OutputDebugStringA("isRedirectByHttpCode: ");

        if (isRedirectByUrl)
            OutputDebugStringA("isRedirectByUrl: ");

        // https://passport.liepin.com/account/v1/elogin#sfrom=click-pc_homepage-front_navigation-ecomphr_new
        // 可能没location，或者开启了HSTS强制要求跳转到HTTPS。不过发现改这里没用，只需要在curl里把http改成https处理即可
        // 见third_party\libcurl\src\url.c
        // 另外发现不能用这个m_effectiveUrl，因为https://zm12.sm-tc.cn/rec/person_entity?wd=shenma_query&title=%E7%BB%BF%E7%98%A6%EF%BC%9A%
        // E5%87%A0%E5%A4%A7%E9%AA%97%E4%BA%BA%E8%AF%AF%E5%8C%BA%EF%BC%8C%E8%AE%A9%E4%BD%A0%E7%9A%84%E7%98%A6%E8%BA%AB%
        // E4%B9%8B%E8%B7%AF%E6%9B%B4%E5%8A%A0%E6%9B%B2%E6%8A%98 这个网址会发生崩溃
//         if (location.isEmpty() || WTF::equalIgnoringCase(nonAuthoritativeReason, "HSTS"))
//             location = job->m_effectiveUrl.c_str();

        if (!location.isEmpty()) {
            Vector<char> locationBuffer = WTF::ensureStringToUTF8(location, false);
            locationBuffer.append('\n');
            locationBuffer.append('\0');
            OutputDebugStringA(locationBuffer.data());

            doRedirect(job, location, args, isRedirectByHttpCode);
            if (isRedirectByHttpCode)
                return false;
            return true;
        } else
            OutputDebugStringA("location.isEmpty\n");

    } else if (isHttpAuthentication(args->httpCode)) {

    } else
        distpatchWkeWillSendRequest(job, nullptr, args->httpCode);

#if 0
    if (/*8000 < args->contentLength &&*/ args->contentLength < 25000) {
        wkeNetHookRequest(job);
        job->m_isHookRequest |= 2;
    }
#endif
    return true;
}

static void setResponseDataToJobWhenDidReceiveResponseOnMainThread(WebURLLoaderInternal* job, MainTaskArgs* args)
{
    KURL url = job->firstRequest()->url();
    bool needSetResponseFired = true;

    job->m_response.setExpectedContentLength(static_cast<long long int>(args->contentLength));
    job->m_response.setURL(KURL(ParsedURLString, args->hdr));
    job->m_response.setHTTPStatusCode(args->httpCode);

    if (url.protocolIsInHTTPFamily())
        needSetResponseFired = setHttpResponseDataToJobWhenDidReceiveResponseOnMainThread(job, args);

    if (needSetResponseFired && !job->isCancelled()) {
        if (job->client() && job->loader())
            WebURLLoaderManager::sharedInstance()->handleDidReceiveResponse(job);
        job->setResponseFired(true);
    }
}

void WebURLLoaderManagerMainTask::handleLocalReceiveResponseOnMainThread(MainTaskArgs* args, WebURLLoaderInternal* job)
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
size_t WebURLLoaderManagerMainTask::handleWriteCallbackOnMainThread(MainTaskArgs* args, WebURLLoaderInternal* job)
{
    void* ptr = args->ptr;
    size_t size = args->size;
    size_t nmemb = args->nmemb;

    size_t totalSize = size * nmemb;

    if (!job->responseFired()) {
        handleLocalReceiveResponseOnMainThread(args, job);
        if (job->isCancelled())
            return 0;
    }

    if (job->m_isHookRequest) {
        if (!job->m_hookBufForEndHook)
            job->m_hookBufForEndHook = new Vector<char>();
        job->m_hookBufForEndHook->append((char*)ptr, totalSize);
        return totalSize;
    }

    if (job->m_multipartHandle) {
        job->m_multipartHandle->contentReceived(static_cast<const char*>(ptr), totalSize);
    } else if (job->client() && job->loader()) {
        WebURLLoaderManager::sharedInstance()->didReceiveDataOrDownload(job, static_cast<char*>(ptr), totalSize, 0);
    }
    return totalSize;
}

size_t WebURLLoaderManagerMainTask::handleHeaderCallbackOnMainThread(MainTaskArgs* args, WebURLLoaderInternal* job)
{
    if (job->isCancelled())
        return 0;

    // We should never be called when deferred loading is activated.
    ASSERT(!job->m_defersLoading);

    size_t totalSize = args->size * args->nmemb;

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
    if (1 != job->m_isHookRequest)
        return;

    KURL kurl = job->firstRequest()->url();
    String url = WTF::ensureStringToUTF8String(kurl.string());

    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    if (!requestExtraData)
        return;
    content::WebPage* page = requestExtraData->page;
    if (!page)
        return;

    Vector<char> urlBuf = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);
    void* loadUrlEndCallbackParam = page->wkeHandler().loadUrlEndCallbackParam;
    void* data = job->m_hookBufForEndHook ? job->m_hookBufForEndHook->data() : nullptr;
    size_t size = job->m_hookBufForEndHook ? job->m_hookBufForEndHook->size() : 0;

    if (!page->wkeHandler().loadUrlEndCallback)
        return;

    page->wkeHandler().loadUrlEndCallback(page->wkeWebView(), loadUrlEndCallbackParam, urlBuf.data(), job, data, size);
}

}

#endif // net_WebURLLoaderManagerMainTask_h