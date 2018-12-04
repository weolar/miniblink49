#ifndef net_WebURLLoaderManagerAsynTask_h
#define net_WebURLLoaderManagerAsynTask_h

#include "net/WebURLLoaderManager.h"
#include "net/WebURLLoaderInternal.h"
#include "platform/MIMETypeRegistry.h"

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
        WTF::Locker<WTF::Mutex> locker(m_manager->m_shutdownMutex);
        if (!m_manager->downloadOnIoThread())
            return;

        if (m_manager->m_isShutdown)
            return;

        IoTask* task = new IoTask(m_manager, m_thread, true);
        m_thread->postDelayedTask(FROM_HERE, task, 1);
    }

private:
    WebURLLoaderManager* m_manager;
    blink::WebThread* m_thread;
    bool m_start;
};

const int kBlackListCancelJobId = -2;

static void releaseJobWithoutCurl(WebURLLoaderInternal* job, int jobId)
{
    if (kBlackListCancelJobId == jobId)
        return;
    RELEASE_ASSERT(job->m_ref == 0);
    job->m_handle = nullptr;
    WebURLLoaderManager::sharedInstance()->removeLiveJobs(jobId);
    delete job;
}

class WkeAsynTask : public blink::WebThread::Task {
public:
    WkeAsynTask(WebURLLoaderManager* manager, int jobId)
    {
        m_manager = manager;
        m_jobId = jobId;
    }

    ~WkeAsynTask() override { }

    virtual void run() override
    {
        JobHead* jobHead = m_manager->checkJob(m_jobId);
        if (!jobHead || JobHead::kLoaderInternal != jobHead->getType())
            return;
        WebURLLoaderInternal* job = (WebURLLoaderInternal*)jobHead;
        if (kNormalCancelled == job->m_cancelledReason) {
            releaseJobWithoutCurl(job, m_jobId);
            return;
        }
        blink::KURL url = job->firstRequest()->url();
        job->m_response.setURL(url);
        String urlString = url.getUTF8String();
        job->m_url = fastStrDup(urlString.utf8().data());

        if (job->m_response.mimeType().isNull() || job->m_response.mimeType().isEmpty()) {
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

        if (job->firstRequest()->downloadToFile() && job->m_asynWkeNetSetData) {
            String tempPath = m_manager->handleHeaderForBlobOnMainThread(job, job->m_asynWkeNetSetData->size());
            job->m_response.setDownloadFilePath(tempPath);
        }

        job->m_response.setHTTPStatusCode(200);
        job->m_response.setHTTPStatusText(blink::WebString::fromUTF8("OK"));

        m_manager->handleDidReceiveResponse(job);

        if (job->m_asynWkeNetSetData && kNormalCancelled != job->m_cancelledReason) { // 可能在didReceiveResponse里被cancel
            job->m_response.setExpectedContentLength(static_cast<long long int>(job->m_asynWkeNetSetData->size()));
            m_manager->didReceiveDataOrDownload(job, job->m_asynWkeNetSetData->data(), job->m_asynWkeNetSetData->size(), 0);
            m_manager->handleDidFinishLoading(job, WTF::currentTime(), 0);
        }
        if (kHookRedirectCancelled != job->m_cancelledReason)
            releaseJobWithoutCurl(job, m_jobId);
    }

private:
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

    static void cancel(WebURLLoaderInternal* job, int jobId)
    {
        job->m_isBlackList = true;
        job->m_response.setURL(job->firstRequest()->url());
        job->client()->didReceiveResponse(job->loader(), job->m_response);
        if (!job->isCancelled()) { // 可能在didReceiveResponse里被cancel
                                   //WebURLLoaderManager::sharedInstance()->didReceiveDataOrDownload(job, static_cast<char*>(""), 0, 0);

            WebURLError error;
            error.domain = WebString(String(job->m_url));
            error.reason = -1;
            error.localizedDescription = WebString::fromUTF8("black list");
            WebURLLoaderManager::sharedInstance()->handleDidFail(job, error);
            RELEASE_ASSERT(job->isCancelled());
        }
        releaseJobWithoutCurl(job, jobId);
    }

    virtual void run() override
    {
        JobHead* jobHead = m_manager->checkJob(m_jobId);
        if (!jobHead || JobHead::kLoaderInternal != jobHead->getType())
            return;
        WebURLLoaderInternal* job = (WebURLLoaderInternal*)jobHead;
        if (job->isCancelled())
            return;

        cancel(job, m_jobId);
    }

private:
    WebURLLoaderManager* m_manager;
    int m_jobId;
};

}

#endif // net_WebURLLoaderManagerAsynTask_h