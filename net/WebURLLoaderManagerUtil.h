
#ifndef net_WebURLLoaderManagerUtil_h
#define net_WebURLLoaderManagerUtil_h

#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"
#include "third_party/WebKit/Source/wtf/text/CString.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "curl/curl.h"

namespace WTF {
class Mutex;
}

namespace net {

const int selectTimeoutMS = 5;
const double pollTimeSeconds = 0.05;
const int maxRunningJobs = 5;

static const bool ignoreSSLErrors = true; //  ("WEBKIT_IGNORE_SSL_ERRORS");
static const int kAllowedProtocols = CURLPROTO_FILE | CURLPROTO_FTP | CURLPROTO_FTPS | CURLPROTO_HTTP | CURLPROTO_HTTPS;

extern char* g_cookieJarPath;

void setCookieJarFullPath(const WCHAR* path);
void setCookieJarPath(const WCHAR* path);
char* cookieJarPath();
CString certificatePath();

#if ENABLE(WEB_TIMING)
int milisecondsSinceRequest(double requestTime);
void calculateWebTimingInformations(ResourceHandleInternal* job);
#endif

// libcurl does not implement its own thread synchronization primitives.
// these two functions provide mutexes for cookies, and for the global DNS
// cache.
WTF::Mutex* sharedResourceMutex(curl_lock_data data);

void curl_lock_callback(CURL* /* handle */, curl_lock_data data, curl_lock_access /* access */, void* /* userPtr */);
void curl_unlock_callback(CURL* /* handle */, curl_lock_data data, void* /* userPtr */);

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

bool isAppendableHeader(const String &key);

}

#endif // net_WebURLLoaderManagerUtil_h