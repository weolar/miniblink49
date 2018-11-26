
#ifndef net_PageNetExtraData_h
#define net_PageNetExtraData_h

#include "third_party/WebKit/Source/wtf/ThreadSafeRefCounted.h"
#include <string>

typedef void CURLSH;

namespace net {
    
class WebCookieJarImpl;

class PageNetExtraData : public WTF::ThreadSafeRefCounted<PageNetExtraData> {
public:
    PageNetExtraData();
    ~PageNetExtraData();

    void setCookieJarFullPath(const std::string& cookieJarFileName);
    CURLSH* getCurlShareHandle();
    std::string getCookieJarFullPath();
    net::WebCookieJarImpl* getCookieJar() const { return m_cookieJar; }

private:
    net::WebCookieJarImpl* m_cookieJar;
};

}

#endif // net_PageCookie_h