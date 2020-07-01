
#include "net/WebURLLoaderManagerUtil.h"

#include "net/WebURLLoaderInternal.h"
#include "net/WebURLLoaderManager.h"
#include "net/ActivatingObjCheck.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/web/WebIconURL.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "wke/wkeWebView.h"
#include "content/browser/WebPage.h"
#include <shlwapi.h>

namespace net {
	
CString certificatePath()
{
#if 0 
    char* envPath = getenv("CURL_CA_BUNDLE_PATH");
    if (envPath)
        return envPath;
#endif
    return CString();
}


#if ENABLE(WEB_TIMING)
int milisecondsSinceRequest(double requestTime)
{
    return static_cast<int>((monotonicallyIncreasingTime() - requestTime) * 1000.0);
}

void calculateWebTimingInformations(ResourceHandleInternal* job)
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
WTF::Mutex* sharedResourceMutex(curl_lock_data data)
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

void curl_lock_callback(CURL* /* handle */, curl_lock_data data, curl_lock_access /* access */, void* /* userPtr */)
{
    if (WTF::Mutex* mutex = sharedResourceMutex(data))
        mutex->lock();
}

void curl_unlock_callback(CURL* /* handle */, curl_lock_data data, void* /* userPtr */)
{
    if (WTF::Mutex* mutex = sharedResourceMutex(data))
        mutex->unlock();
}

bool isAppendableHeader(const String &key) {
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

class GetFaviconTask : public JobHead {
    std::string m_url;
    wkeOnNetGetFaviconCallback m_callback;
    wkeWebView m_webView;
    void* m_param;
    wkeMemBuf* m_buf;
    int m_webviewId;

public:
    GetFaviconTask(wkeOnNetGetFaviconCallback callback, void* param, wkeWebView webView)
    {
        m_ref = 0;
        m_id = 0;
        m_webviewId = webView->getId();
        m_type = kGetFaviconTask;
        m_callback = callback;
        m_param = param;
        m_webView = webView;
        m_buf = nullptr;
    }

    virtual ~GetFaviconTask() override
    {
        if (m_buf)
            wkeFreeMemBuf(m_buf);
        WebURLLoaderManager* manager = WebURLLoaderManager::sharedInstance();
        if (manager)
            manager->removeLiveJobs(m_id);
    }

    void setUrl(const char* url) { m_url = url; }
    bool isUrlEmpty() const { return m_url.empty(); }

    int run()
    {
        WebURLLoaderManager* manager = WebURLLoaderManager::sharedInstance();
        if (!manager) {
            delete this;
            return 0;
        }
        WebURLLoaderInternal* job = (WebURLLoaderInternal*)this;
        int jobId = manager->addLiveJobs(job);
        m_id = jobId;
        manager->getIoThread()->postTask(FROM_HERE, WTF::bind(&GetFaviconTask::getFaviconUrl, jobId, m_webView->getId()));
        return jobId;
    }

    virtual void cancel() override
    {
        if (net::ActivatingObjCheck::inst()->isActivating(m_webviewId))
            m_callback(m_webView, m_param, "", nullptr);
    }

private:
    static size_t writeData(void* buffer, size_t size, size_t nmemb, void* userp)
    {
        Vector<char>* bufferCache = (Vector<char>*)userp;
        bufferCache->append((char*)buffer, size * nmemb);
        return size * nmemb;
    }

    static void exit(GetFaviconTask* self, int jobId)
    {
        WebURLLoaderManager* manager = WebURLLoaderManager::sharedInstance();
        if (manager)
            manager->removeLiveJobs(jobId);
        delete self;
    }

    static void onNetGetFaviconFinish(int jobId, int webviewId)
    {
        AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
        JobHead* job = autoLockJob.lockJobHead();
        if (!job || JobHead::kGetFaviconTask != job->getType())
            return;

        GetFaviconTask* self = (GetFaviconTask*)job;
        if (!net::ActivatingObjCheck::inst()->isActivating(webviewId)) {
            exit(self, jobId);
            return;
        }

        self->m_callback(self->m_webView, self->m_param, self->m_url.c_str(), self->m_buf);
        exit(self, jobId);
    }

    static void onNetGetFavicon(int jobId, int webviewId)
    {
        AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
        JobHead* job = autoLockJob.lockJobHead();
        if (!job || JobHead::kGetFaviconTask != job->getType())
            return;

        GetFaviconTask* self = (GetFaviconTask*)job;
        if (!net::ActivatingObjCheck::inst()->isActivating(webviewId)) {
            exit(self, jobId);
            return;
        }

        self->onNetGetFaviconImpl(jobId, webviewId);
    }

    void onNetGetFaviconImpl(int jobId, int webviewId) {
        CURL* curl = curl_easy_init();
        if (!curl) {
            blink::Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(onNetGetFaviconFinish, jobId, webviewId));
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5000);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, FALSE);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);

        Vector<char> buffer;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        int res = curl_easy_perform(curl);
        if (CURLE_OK != res || 0 == buffer.size()) {
            curl_easy_cleanup(curl);
            blink::Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(onNetGetFaviconFinish, jobId, webviewId));
            return;
        }

        m_buf = wkeCreateMemBuf(m_webView, buffer.data(), buffer.size());
        curl_easy_cleanup(curl);
        blink::Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(onNetGetFaviconFinish, jobId, webviewId));
    }

    static void getFaviconUrl(int jobId, int webviewId)
    {
        AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
        JobHead* job = autoLockJob.lockJobHead();
        if (!job || JobHead::kGetFaviconTask != job->getType())
            return;

        GetFaviconTask* self = (GetFaviconTask*)job;
        if (!net::ActivatingObjCheck::inst()->isActivating(webviewId)) {
            exit(self, jobId);
            return;
        }

        WebURLLoaderManager* manager = WebURLLoaderManager::sharedInstance();
        if (!manager) {
            self->m_callback(self->m_webView, self->m_param, "", nullptr);
            exit(self, jobId);
            return;
        }
        
        blink::WebVector<blink::WebIconURL> urls = self->m_webView->getWebPage()->mainFrame()->iconURLs(blink::WebIconURL::TypeFavicon);
        for (size_t i = 0; i < urls.size(); ++i) {
            blink::WebIconURL iconURL = urls[i];
            blink::KURL kurl = iconURL.iconURL();
            self->setUrl(kurl.getUTF8String().utf8().data());
        }

        if (urls.isEmpty()) {
            self->m_callback(self->m_webView, self->m_param, "", nullptr);
            exit(self, jobId);
            return;
        }
        blink::Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(&GetFaviconTask::onNetGetFavicon, jobId, self->m_webView->getId()));
    }
};

int getFavicon(wkeWebView webView, wkeOnNetGetFaviconCallback callback, void* param)
{
    if (!webView || !webView->getWebPage() || !webView->getWebPage()->mainFrame())
        return 0;

    GetFaviconTask* task = new GetFaviconTask(callback, param, webView);
    return task->run();
}

}