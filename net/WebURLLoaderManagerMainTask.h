
#ifndef net_WebURLLoaderManagerMainTask_h
#define net_WebURLLoaderManagerMainTask_h

#include "net/WebURLLoaderInternal.h"
#include "net/WebURLLoaderManagerUtil.h"
#include "net/RequestExtraData.h"
#include "content/browser/WebPage.h"
#include "third_party/WebKit/Source/platform/network/HTTPParsers.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebScheduler.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/Source/wtf/Threading.h"
#include "wke/wkeWebView.h"

namespace net {

// 回调回main线程的task
class WebURLLoaderManagerMainTask : public blink::WebThread::Task{
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
        blink::WebURLError* resourceError;

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
            args->resourceError = new blink::WebURLError();
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
            blink::Platform::current()->mainThread()->scheduler()->postLoadingTask(FROM_HERE, task); // postLoadingTask
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

#if ENABLE_WKE == 1
static bool dispatchResponseToWke(WebURLLoaderInternal* job, const AtomicString& contentType)
{
    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
    if (!requestExtraData)
        return false;

    WebPage* page = requestExtraData->page;
    Vector<char> urlBuf = WTF::ensureStringToUTF8(job->firstRequest()->url().string(), true);

    if (page->wkeHandler().netResponseCallback) {
        if (page->wkeHandler().netResponseCallback(page->wkeWebView(), page->wkeHandler().downloadCallbackParam, urlBuf.data(), job)) {
            blink::WebLocalFrame* frame = requestExtraData->frame;
            frame->stopLoading();
            return true;
        }
    }

    if (equalIgnoringCase(contentType, "application/octet-stream") ||
        equalIgnoringCase(contentType, "application/zip") ||
        equalIgnoringCase(contentType, "application/rar") ||
        contentDispositionType(job->m_response.httpHeaderField("Content-Disposition")) == ContentDispositionAttachment) {
        if (page->wkeHandler().downloadCallback) {
            if (page->wkeHandler().downloadCallback(page->wkeWebView(), page->wkeHandler().downloadCallbackParam, urlBuf.data())) {
                blink::WebLocalFrame* frame = requestExtraData->frame;
                frame->stopLoading();
                return true;
            }
        }
    }
    return false;
}
#endif

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
    if (dispatchResponseToWke(job, contentType))
        return false;
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

}

#endif // net_WebURLLoaderManagerMainTask_h