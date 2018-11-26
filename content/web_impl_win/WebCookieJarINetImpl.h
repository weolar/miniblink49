#ifndef WebCookieJarImph_h
#define WebCookieJarImph_h

#include "third_party/WebKit/public/platform/WebCookieJar.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"

namespace net {
class ParsedCookie;
class CookieMonster;
}

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

    String X_XSRF_TOKEN() const { return m_X_XSRF_TOKEN; }

    static WebCookieJarImpl* inst();

private:
    WTF::Vector<net::ParsedCookie*> m_parsedCookies;

    net::CookieMonster* m_cookieMonster;

    static WebCookieJarImpl* m_inst;

    String m_X_XSRF_TOKEN;
};

} // content

#endif // WebCookieJarImph_h