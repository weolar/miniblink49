
#ifndef net_PageNetExtraData_h
#define net_PageNetExtraData_h

#include "third_party/WebKit/Source/wtf/ThreadSafeRefCounted.h"
#include "net/StorageDef.h"
#include <string>

typedef void CURL;
typedef void CURLSH;

namespace blink {
class WebStorageNamespace;
}

namespace net {
    
class WebCookieJarImpl;
class WebStorageNamespaceImpl;

class PageNetExtraData : public WTF::ThreadSafeRefCounted<PageNetExtraData> {
public:
    PageNetExtraData();
    ~PageNetExtraData();

    void setCookieJarFullPath(const std::string& fullPathUtf8);
    CURLSH* getCurlShareHandle();
    std::string getCookieJarFullPath();
    WebCookieJarImpl* getCookieJar() const { return m_cookieJar; }

    void setLocalStorageFullPath(const std::string& fullPathUtf8);
    blink::WebStorageNamespace* createWebStorageNamespace();

private:
    WebCookieJarImpl* m_cookieJar;
    DOMStorageMap* m_localStorage;
    String m_localStotageFullPath;
};

}

#endif // net_PageCookie_h