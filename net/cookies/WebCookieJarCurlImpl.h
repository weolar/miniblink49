#ifndef net_cookies_WebCookieJarImph_h
#define net_cookies_WebCookieJarImph_h

#include "third_party/WebKit/public/platform/WebCookieJar.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"

namespace net {
class ParsedCookie;
class CookieMonster;
}

struct curl_slist;
typedef void CURLSH;
typedef void CURL;

namespace net {
    
class WebCookieJarImpl : public blink::WebCookieJar {
public:
    WebCookieJarImpl(std::string cookieJarFileName);
    ~WebCookieJarImpl();
    virtual void setCookie(const blink::WebURL&, const blink::WebURL& firstPartyForCookies, const blink::WebString& cookie) override;
    virtual blink::WebString cookies(const blink::WebURL&, const blink::WebURL& firstPartyForCookies) override;
    virtual blink::WebString cookieRequestHeaderFieldValue(const blink::WebURL&, const blink::WebURL& firstPartyForCookies) override;
    virtual bool cookiesEnabled(const blink::WebURL&, const blink::WebURL& firstPartyForCookies) override { return true; }

    void setCookieFromWinINet(const blink::KURL& url, const WTF::Vector<char>& cookiesLine);
    void setToRecordFromRawHeads(const blink::KURL& url, const WTF::String& rawHeadsString);

    void setCookieJarFullPath(const WCHAR* path);

    void deleteCookies(const blink::KURL& url, const String& cookieName);
    String getCookiesForSession(const blink::KURL&, const blink::KURL& url, bool httponly);
    const curl_slist* WebCookieJarImpl::getAllCookiesBegin();
    void WebCookieJarImpl::getAllCookiesEnd(const curl_slist* list);

    typedef bool(*CookieVisitor)(void* params, const char* name, const char* value, const char* domain, const char* path, int secure, int httpOnly, int* expires);
    void visitAllCookie(void* params, CookieVisitor visit);

    //static WebCookieJarImpl* getShare();

    CURLSH* getCurlShareHandle() const
    {
        return m_curlShareHandle;
    }
    std::string getCookieJarFullPath();

private:
    void flushCurlCookie(CURL* curl);
    void setCookiesFromDOM(const blink::KURL&, const blink::KURL& url, const String& value);
    CURLSH* m_curlShareHandle;
    std::string m_cookieJarFileName;
    bool m_dirty;

    //static WebCookieJarImpl* m_inst;
};

} // content

#endif // net_cookies_WebCookieJarImph_h