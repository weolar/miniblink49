#ifndef WebCookieJarImph_h
#define WebCookieJarImph_h

#include "third_party/WebKit/public/platform/WebCookieJar.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"

namespace net {
class ParsedCookie;
class CookieMonster;
}

struct curl_slist;

namespace content {

class HttpCookie;

class WebCookieJarImpl : public blink::WebCookieJar {
public:
    WebCookieJarImpl();
    ~WebCookieJarImpl();
    virtual void setCookie(const blink::WebURL&, const blink::WebURL& firstPartyForCookies, const blink::WebString& cookie) override;
    virtual blink::WebString cookies(const blink::WebURL&, const blink::WebURL& firstPartyForCookies) override;
    virtual blink::WebString cookieRequestHeaderFieldValue(const blink::WebURL&, const blink::WebURL& firstPartyForCookies) override;
    virtual bool cookiesEnabled(const blink::WebURL&, const blink::WebURL& firstPartyForCookies) override { return true; }

    void setCookieFromWinINet(const blink::KURL& url, const WTF::Vector<char>& cookiesLine);
    void setToRecordFromRawHeads(const blink::KURL& url, const WTF::String& rawHeadsString);

    static void deleteCookies(const blink::KURL& url, const String& cookieName);
    static String cookiesForSession(const blink::KURL&, const blink::KURL& url, bool httponly);
    static const curl_slist* WebCookieJarImpl::getAllCookiesBegin();
    static void WebCookieJarImpl::getAllCookiesEnd(const curl_slist* list);

    typedef bool(*CookieVisitor)(void* params, const char* name, const char* value, const char* domain, const char* path, int secure, int httpOnly, int* expires);
    static void visitAllCookie(void* params, CookieVisitor visit);

    static WebCookieJarImpl* inst();

private:
    static WebCookieJarImpl* m_inst;
};

} // content

#endif // WebCookieJarImph_h