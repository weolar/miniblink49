
#ifndef net_PageNetExtraData_h
#define net_PageNetExtraData_h

#include "third_party/WebKit/Source/wtf/ThreadSafeRefCounted.h"
#include <string>

typedef void CURLSH;

namespace net {
    
class PageNetExtraData : public WTF::ThreadSafeRefCounted<PageNetExtraData> {
public:
    PageNetExtraData();
    ~PageNetExtraData();

    void setCookieJarPath(const std::string& cookieJarFileName);
    CURLSH* getCurlShareHandle() { return m_curlShareHandle; }
    std::string getCookieJarFileName() { return m_cookieJarFileName; }

private:
    CURLSH* m_curlShareHandle;
    std::string m_cookieJarFileName;
};

}

#endif // net_PageCookie_h