#ifndef net_WebURLLoaderManagerAsynTask_h
#define net_WebURLLoaderManagerAsynTask_h

#include "net/WebURLLoaderManager.h"
#include "net/WebURLLoaderInternal.h"
#include "platform/MIMETypeRegistry.h"
#include "wke/wkeWebView.h"

namespace net {

class WebURLLoaderManager::IoTask : public blink::WebThread::Task {
public:
    IoTask(WebURLLoaderManager* manager, blink::WebThread* thread, bool start)
        : m_manager(manager)
        , m_thread(thread)
        , m_start(start)
    {
    }

    ~IoTask() override { }

    virtual void run() override
    {
        //WTF::Locker<WTF::Mutex> locker(m_manager->m_shutdownMutex);
        ShutdownReadLocker locker(&m_manager->m_shutdownLock);
        if (!locker.lock())
            return;

        if (!m_manager->downloadOnIoThread())
            return;
        if (m_manager->m_isShutdown)
            return;

        IoTask* task = new IoTask(m_manager, m_thread, true);
        m_thread->postTask(FROM_HERE, task);
    }

private:
    WebURLLoaderManager* m_manager;
    blink::WebThread* m_thread;
    bool m_start;
};

const int kBlackListCancelJobId = -2;

static void releaseJobWithoutCurl(WebURLLoaderManager* manager, WebURLLoaderInternal* job, int jobId)
{
    if (kBlackListCancelJobId == jobId)
        return;

//     job->m_destroingMutex.lock();
//     job->m_state = WebURLLoaderInternal::kDestroyed;
//     while (job->m_ref != 1) { ::Sleep(5); }
// 
//     job->m_handle = nullptr;
//     manager->removeLiveJobs(jobId);
//     delete job;

    WebURLLoaderInternal::release(jobId); // WebURLLoaderManager::doCancel 可能会占用
}

class HookAsynTask : public blink::WebThread::Task {
public:
    HookAsynTask(WebURLLoaderManager* manager, int jobId)
    {
        m_manager = manager;
        m_jobId = jobId;
    }

    ~HookAsynTask() override {}

    virtual void run() override
    {
        AutoLockJob autoLockJob(m_manager, m_jobId);
        JobHead* jobHead = autoLockJob.lock();

        if (!jobHead || JobHead::kLoaderInternal != jobHead->getType())
            return;
        WebURLLoaderInternal* job = (WebURLLoaderInternal*)jobHead;
        if (kNormalCancelled == job->m_cancelledReason) {
            releaseJobWithoutCurl(m_manager, job, m_jobId);
            return;
        }

        blink::KURL url = job->firstRequest()->url();
        job->m_response.setURL(url);
        String urlString = url.getUTF8String();
        job->m_url = fastStrDup(urlString.utf8().data());

        setMIMEType(job, urlString);

        size_t size = 0;
        const char* data = nullptr;

        if (job->m_asynWkeNetSetData) {
            size = job->m_asynWkeNetSetData->size();
            data = job->m_asynWkeNetSetData->data();
        }

        if (job->firstRequest()->downloadToFile() && size > 0) {
            String tempPath = m_manager->handleHeaderForBlobOnMainThread(job, size);
            job->m_response.setDownloadFilePath(tempPath);
        }

        job->m_response.setHTTPStatusCode(200);
        job->m_response.setHTTPStatusText(blink::WebString::fromUTF8("OK"));

        m_manager->handleDidReceiveResponse(job);

        if ((job->m_asynWkeNetSetData) && 
            kNormalCancelled != job->m_cancelledReason &&
            kHookRedirectCancelled != job->m_cancelledReason) { // 可能在didReceiveResponse里被cancel
            job->m_response.setExpectedContentLength(static_cast<long long int>(size));

            dispatchUrlEndHook(job, data, size);

            m_manager->didReceiveDataOrDownload(job, data, size, 0);
            m_manager->handleDidFinishLoading(job, WTF::currentTime(), 0); // 这里会走到WebURLLoaderManager::doCancel，然后导致job被占用
        }

        if (kHookRedirectCancelled != job->m_cancelledReason)
            releaseJobWithoutCurl(m_manager, job, m_jobId);
    }

private:
    void setMIMEType(WebURLLoaderInternal* job, const String& urlString)
    {
        if (!(job->m_response.mimeType().isNull()) && !(job->m_response.mimeType().isEmpty()))
            return;

        int urlHostLength = urlString.length();
        for (int i = 0; i < urlHostLength; ++i) {
            if ('?' != urlString[i])
                continue;
            urlHostLength = i;
            break;
        }
        String urlWithoutQuery(urlString.characters8(), urlHostLength);
        job->m_response.setMIMEType(blink::MIMETypeRegistry::getMIMETypeForPath(urlWithoutQuery));
    }

    void dispatchUrlEndHook(WebURLLoaderInternal* job, const char* data, size_t size)
    {
        RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(job->firstRequest()->extraData());
        if (!requestExtraData)
            return;

        content::WebPage* page = requestExtraData->page;
        if (!page)
            return;

        wkeLoadUrlEndCallback loadUrlEndCallback = page->wkeHandler().loadUrlEndCallback;
        void* loadUrlEndCallbackParam = page->wkeHandler().loadUrlEndCallbackParam;
        if (1 == job->m_isHookRequest && loadUrlEndCallback)
            loadUrlEndCallback(page->wkeWebView(), loadUrlEndCallbackParam, job->m_url, job, (void*)data, size);
    }

    WebURLLoaderManager* m_manager;
    int m_jobId;
};

class BlackListCancelTask : public blink::WebThread::Task {
public:
    BlackListCancelTask(WebURLLoaderManager* manager, int jobId)
    {
        m_manager = manager;
        m_jobId = jobId;
        
        JobHead* jobHead = m_manager->checkJob(m_jobId);
        if (!jobHead || JobHead::kLoaderInternal != jobHead->getType())
            return;
        WebURLLoaderInternal* job = (WebURLLoaderInternal*)jobHead;

        job->m_isBlackList = true;
    }

    ~BlackListCancelTask() override { }

    static void cancel(WebURLLoaderManager* manager, WebURLLoaderInternal* job, int jobId)
    {
        job->m_isBlackList = true;
        job->m_response.setURL(job->firstRequest()->url());

        WebURLLoaderManager::sharedInstance()->handleDidReceiveResponse(job);
        //job->client()->didReceiveResponse(job->loader(), job->m_response);

        // 可能在didReceiveResponse里被cancel
        // WebURLLoaderManager::sharedInstance()->didReceiveDataOrDownload(job, static_cast<char*>(""), 0, 0);
        if (!job->isCancelled()) {
            WebURLError error;
            error.domain = WebString(String(job->m_url));
            error.reason = -1;
            error.localizedDescription = WebString::fromUTF8("black list");
            WebURLLoaderManager::sharedInstance()->handleDidFail(job, error);

            if (!job->isCancelled())
                WebURLLoaderManager::sharedInstance()->doCancel(job, kNormalCancelled);
            RELEASE_ASSERT(job->isCancelled());
        }
        releaseJobWithoutCurl(manager, job, jobId);
    }

    virtual void run() override
    {
        AutoLockJob autoLockJob(m_manager, m_jobId);
        JobHead* jobHead = autoLockJob.lock();

        //JobHead* jobHead = m_manager->checkJob(m_jobId);
        if (!jobHead || JobHead::kLoaderInternal != jobHead->getType())
            return;
        WebURLLoaderInternal* job = (WebURLLoaderInternal*)jobHead;
        if (job->isCancelled())
            return;

        cancel(m_manager, job, m_jobId);
    }

private:
    WebURLLoaderManager* m_manager;
    int m_jobId;
};

}

#endif // net_WebURLLoaderManagerAsynTask_h