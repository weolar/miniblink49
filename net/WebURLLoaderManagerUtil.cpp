
#include "net/WebURLLoaderManagerUtil.h"

#include "net/WebURLLoaderManager.h"
#include "net/ActivatingObjCheck.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/web/WebIconURL.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "wke/wkeWebView.h"
#include "content/browser/WebPage.h"
#include <shlwapi.h>

namespace net {

char* g_cookieJarPath = nullptr;

CString certificatePath()
{
#if 0 
    char* envPath = getenv("CURL_CA_BUNDLE_PATH");
    if (envPath)
        return envPath;
#endif
    return CString();
}

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

char* cookieJarPath()
{
    if (g_cookieJarPath)
        return g_cookieJarPath;

    char* cookieJarPathStr = "cookies.dat";
    g_cookieJarPath = (char*)malloc(strlen(cookieJarPathStr) + 1);
    strcpy(g_cookieJarPath, cookieJarPathStr);
    return g_cookieJarPath;
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

struct GetFaviconTask {
    int m_ref;
    int m_id;

    std::string url;
    wkeOnNetGetFavicon callback;
    wkeWebView webView;
    void* param;
    wkeMemBuf* buf;

    static size_t writeData(void* buffer, size_t size, size_t nmemb, void* userp)
    {
        Vector<char>* bufferCache = (Vector<char>*)userp;
        bufferCache->append((char*)buffer, size * nmemb);

        return size * nmemb;
    }

    static void onNetGetFaviconFinish(int jobId, int webviewId)
    {
        AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
        WebURLLoaderInternal* job = autoLockJob.lock();
        if (!job || !net::ActivatingObjCheck::inst()->isActivating(webviewId))
            return;

        GetFaviconTask* self = (GetFaviconTask*)job;
        self->callback(self->webView, self->param, self->url.c_str(), self->buf);
    }

    static void onNetGetFavicon(int jobId, int webviewId)
    {
        AutoLockJob autoLockJob(WebURLLoaderManager::sharedInstance(), jobId);
        WebURLLoaderInternal* job = autoLockJob.lock();
        if (!job || !net::ActivatingObjCheck::inst()->isActivating(webviewId))
            return;

        GetFaviconTask* self = (GetFaviconTask*)job;
        CURL* curl = curl_easy_init();
        if (!curl) {
            blink::Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(onNetGetFaviconFinish, jobId, webviewId));
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, self->url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5000);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, FALSE);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);

        Vector<char> buffer;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        int res = curl_easy_perform(curl);
        if (CURLE_OK != res) {
            curl_easy_cleanup(curl);
            blink::Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(onNetGetFaviconFinish, jobId, webviewId));
            return;
        }

        wkeMemBuf* buf = wkeCreateMemBuf(self->webView, buffer.data(), buffer.size());
        self->callback(self->webView, self->param, self->url.c_str(), buf);

        curl_easy_cleanup(curl);
        blink::Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(onNetGetFaviconFinish, jobId, webviewId));
    }
};

int getFavicon(wkeWebView webView, wkeOnNetGetFavicon callback, void* param)
{
    if (!webView || !webView->getWebPage() || !webView->getWebPage()->mainFrame())
        return -1;

    blink::WebVector<blink::WebIconURL> urls = webView->getWebPage()->mainFrame()->iconURLs(blink::WebIconURL::TypeFavicon);
    if (urls.isEmpty())
        return -1;

    GetFaviconTask* task = new GetFaviconTask();
    task->m_ref = 0;
    task->callback = callback;
    task->param = param;
    task->webView = webView;
    task->buf = nullptr;
    WebURLLoaderInternal* job = (WebURLLoaderInternal*)task;
    
    for (size_t i = 0; i < urls.size(); ++i) {
        blink::WebIconURL iconURL = urls[i];
        blink::KURL kurl = iconURL.iconURL();
        task->url = kurl.getUTF8String().utf8().data();
    }

    if (task->url.empty()) {
        delete task;
        return -1;
    }

    int jobId = WebURLLoaderManager::sharedInstance()->addLiveJobs(job);
    WebURLLoaderManager::sharedInstance()->getIoThread()->postTask(FROM_HERE, WTF::bind(&GetFaviconTask::onNetGetFavicon, jobId, webView->getId()));
    return jobId;
}

}