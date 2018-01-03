
#include <windows.h>

#include "content/web_impl_win/WebCookieJarCurlImpl.h"

#define CURL_STATICLIB 
#include "third_party/libcurl/include/curl/curl.h"

#include "net/WebURLLoaderManager.h"

#include "third_party/WebKit/public/platform/WebURL.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "third_party/WebKit/Source/wtf/DateMath.h"
#include "third_party/WebKit/Source/wtf/HashMap.h"
#include "third_party/WebKit/Source/wtf/text/StringBuilder.h"
#include "third_party/WebKit/Source/wtf/text/StringHash.h"

using namespace blink;

namespace content {

WebCookieJarImpl* WebCookieJarImpl::m_inst = nullptr;

static void readCurlCookieToken(const char*& cookie, String& token)
{
    // Read the next token from a cookie with the Netscape cookie format.
    // Curl separates each token in line with tab character.
    const char* cookieStart = cookie;
    while (cookie && cookie[0] && cookie[0] != '\t')
        cookie++;
    token = String(cookieStart, cookie - cookieStart);
    if (cookie[0] == '\t')
        cookie++;
}

static bool domainMatch(const String& cookieDomain, const String& host)
{
    size_t index = host.find(cookieDomain);

    bool tailMatch = (index != WTF::kNotFound && index + cookieDomain.length() == host.length());

    // Check if host equals cookie domain.
    if (tailMatch && !index)
        return true;

    // Check if host is a subdomain of the domain in the cookie.
    // Curl uses a '.' in front of domains to indicate it's valid on subdomains.
    if (tailMatch && index > 0 && host[index] == '.')
        return true;

    // Check the special case where host equals the cookie domain, except for a leading '.' in the cookie domain.
    // E.g. cookie domain is .apple.com and host is apple.com. 
    if (cookieDomain[0] == '.' && cookieDomain.find(host) == 1)
        return true;

    return false;
}

static const char* equalDelimiters = "__curlequal__";

static void appendDotIfNeeded(String* domain)
{
    Vector<char> domainBuffer = WTF::ensureStringToUTF8(*domain, false);
    int dotCount = 0;
    for (size_t i = 0; i < domainBuffer.size(); ++i) {
        if ('.' == domainBuffer[i])
            ++dotCount;
    }

    if (1 == dotCount)
        domain->insert((const LChar *)".", 1, 0);
}

static void addMatchingCurlCookie(const char* cookie, const String& domain, const String& path, StringBuilder& cookies, bool httponly)
{
    // Check if the cookie matches domain and path, and is not expired.
    // If so, add it to the list of cookies.
    //
    // Description of the Netscape cookie file format which Curl uses:
    //
    // .netscape.com     TRUE   /  FALSE  946684799   NETSCAPE_ID  100103
    //
    // Each line represents a single piece of stored information. A tab is inserted between each of the fields.
    //
    // From left-to-right, here is what each field represents:
    //
    // domain - The domain that created AND that can read the variable.
    // flag - A TRUE/FALSE value indicating if all machines within a given domain can access the variable. This value is set automatically by the browser, depending on the value you set for domain.
    // path - The path within the domain that the variable is valid for.
    // secure - A TRUE/FALSE value indicating if a secure connection with the domain is needed to access the variable.
    // expiration - The UNIX time that the variable will expire on. UNIX time is defined as the number of seconds since Jan 1, 1970 00:00:00 GMT.
    // name - The name of the variable.
    // value - The value of the variable.

    if (!cookie)
        return;

    String cookieDomain;
    readCurlCookieToken(cookie, cookieDomain);

    bool subDomain = false;

    // HttpOnly cookie entries begin with "#HttpOnly_".
    if (cookieDomain.startsWith("#HttpOnly_")) {
        if (httponly)
            cookieDomain.remove(0, 10);
        else
            return;
    }

    if (!domainMatch(cookieDomain, domain))
        return;

    String strBoolean;
    readCurlCookieToken(cookie, strBoolean);

    String strPath;
    readCurlCookieToken(cookie, strPath);

    // Check if path matches
    int index = path.find(strPath);
    if (index)
        return;

    String strSecure;
    readCurlCookieToken(cookie, strSecure);

    String strExpires;
    readCurlCookieToken(cookie, strExpires);

    int expires = strExpires.toInt();

    __int64 now = 0;
    time(&now);

    // Check if cookie has expired
    if (expires && now > expires)
        return;

    String strName;
    readCurlCookieToken(cookie, strName);

    String strValue;
    readCurlCookieToken(cookie, strValue);

    // The cookie matches, add it to the cookie list.

    if (cookies.length() > 0)
        cookies.append("; ");

    cookies.append(strName);
    cookies.append("=");
    cookies.append(strValue);
}

static String getNetscapeCookieFormat(const KURL& url, const String& value)
{
    // Constructs a cookie string in Netscape Cookie file format.

    if (value.isEmpty())
        return "";

    String valueStr = WTF::ensureStringToUTF8String(value);

    Vector<String> attributes;
    valueStr.split(';', false, attributes);

    if (!attributes.size())
        return "";

    // First attribute should be <cookiename>=<cookievalue>
    String cookieName, cookieValue;
    Vector<String>::iterator attribute = attributes.begin();
    if (attribute->contains('=')) {
        Vector<String> nameValuePair;
        attribute->split('=', true, nameValuePair);
        cookieName = nameValuePair[0];

        if (2 < nameValuePair.size()) {
            for (size_t i = 1; i < nameValuePair.size() - 1; ++i) {
                cookieName.append('='); // equalDelimiters
                cookieName.append(nameValuePair[i]);
            }
            
        }
        cookieValue = nameValuePair.last();
    } else {
        // According to RFC6265 we should ignore the entire
        // set-cookie string now, but other browsers appear
        // to treat this as <cookiename>=<empty>
        cookieName = *attribute;
    }

    int expires = 0;
    String secure = "FALSE";
    String path = url.baseAsString().substring(url.pathStart());
    if (path.length() > 1 && path.endsWith('/'))
        path.remove(path.length() - 1);
    String domain = url.host();

    // Iterate through remaining attributes
    for (++attribute; attribute != attributes.end(); ++attribute) {
        if (attribute->contains('=')) {
            Vector<String> keyValuePair;
            attribute->split('=', true, keyValuePair);
            String key = keyValuePair[0].stripWhiteSpace().lower();
            String val = keyValuePair[1].stripWhiteSpace();
            if (key == "expires") {
                CString dateStr(reinterpret_cast<const char*>(val.characters8()), val.length());
                expires = WTF::parseDateFromNullTerminatedCharacters(dateStr.data()) / WTF::msPerSecond;
            }
            else if (key == "max-age")
                expires = time(0) + val.toInt();
            else if (key == "domain")
                domain = val;
            else if (key == "path")
                path = val;
        }
        else {
            String key = attribute->stripWhiteSpace().lower();
            if (key == "secure")
                secure = "TRUE";
        }
    }

    appendDotIfNeeded(&domain);
    String allowSubdomains = domain.startsWith('.') ? "TRUE" : "FALSE";
    String expiresStr = String::number(expires);

    int finalStringLength = domain.length() + path.length() + expiresStr.length() + cookieName.length();
    finalStringLength += cookieValue.length() + secure.length() + allowSubdomains.length();
    finalStringLength += 6; // Account for \t separators.

    StringBuilder cookieStr;
    cookieStr.reserveCapacity(finalStringLength);
    cookieStr.append(domain + "\t");
    cookieStr.append(allowSubdomains + "\t");
    cookieStr.append(path + "\t");
    cookieStr.append(secure + "\t");
    cookieStr.append(expiresStr + "\t");
    cookieStr.append(cookieName + "\t");
    cookieStr.append(cookieValue);

    return cookieStr.toString();
}

static void setCookiesFromDOM(const KURL&, const KURL& url, const String& value)
{
    if (!net::WebURLLoaderManager::sharedInstance())
        return;

    CURL* curl = curl_easy_init();

    if (!curl)
        return;

    const char* cookieJarFileName = net::WebURLLoaderManager::sharedInstance()->getCookieJarFileName();
    CURLSH* curlsh = net::WebURLLoaderManager::sharedInstance()->getCurlShareHandle();

    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookieJarFileName); // weolar
    curl_easy_setopt(curl, CURLOPT_SHARE, curlsh);

    // CURL accepts cookies in either Set-Cookie or Netscape file format.
    // However with Set-Cookie format, there is no way to specify that we
    // should not allow cookies to be read from subdomains, which is the
    // required behavior if the domain field is not explicity specified.
    String cookie = getNetscapeCookieFormat(url, value);
    cookie = WTF::ensureStringToUTF8String(cookie);

    CString strCookie(reinterpret_cast<const char*>(cookie.characters8()), cookie.length());

    curl_easy_setopt(curl, CURLOPT_COOKIELIST, strCookie.data());

    curl_easy_cleanup(curl);

//     OutputDebugStringA(strCookie.data());
//     OutputDebugStringA("\n");
}

const curl_slist* WebCookieJarImpl::getAllCookiesBegin()
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return nullptr;

    CURLSH* curlsh = net::WebURLLoaderManager::sharedInstance()->getCurlShareHandle();

    curl_easy_setopt(curl, CURLOPT_SHARE, curlsh);

    curl_slist* list = nullptr;
    curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &list);
    curl_easy_cleanup(curl);
    return list;
}

void WebCookieJarImpl::getAllCookiesEnd(const curl_slist* list)
{
    curl_slist_free_all((curl_slist*)list);
}

void WebCookieJarImpl::visitAllCookie(void* params, CookieVisitor visit)
{
    if (!net::WebURLLoaderManager::sharedInstance())
        return;

    CURL* curl = curl_easy_init();
    if (!curl)
        return;

    CURLSH* curlsh = net::WebURLLoaderManager::sharedInstance()->getCurlShareHandle();
    curl_easy_setopt(curl, CURLOPT_SHARE, curlsh);

    curl_slist* list = nullptr;
    curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &list);

    if (!list) {
        curl_easy_cleanup(curl);
        return;
    }

    Vector<String> needDeleteCookies;

    struct curl_slist* item = list;
    while (item) {
        const char* cookie = item->data;

        String cookieDomain;
        String domain;
        readCurlCookieToken(cookie, cookieDomain);
        domain = cookieDomain;
        bool isHttponly = cookieDomain.startsWith("#HttpOnly_");
        if (isHttponly)
            domain.remove(0, 10);

        String strBoolean;
        readCurlCookieToken(cookie, strBoolean);

        String strPath;
        readCurlCookieToken(cookie, strPath);

        String strSecure;
        readCurlCookieToken(cookie, strSecure);

        String strExpires;
        readCurlCookieToken(cookie, strExpires);

        int expires = strExpires.toInt();
        int secure = (strSecure == "TRUE" ? 1 : 0);

        String strName;
        readCurlCookieToken(cookie, strName);

        String strValue;
        readCurlCookieToken(cookie, strValue);

        bool isDelete = visit(params, strName.utf8().data(), strValue.utf8().data(), domain.utf8().data(), strPath.utf8().data(),
            secure, isHttponly, strExpires.isEmpty() ? nullptr : &expires);
        if (isDelete) {
            //String deleteCooie = "Set-cookie: " + strName + "=none;expires=Monday, 13-Jun-1988 03:04:55 GMT; domain=" + cookieDomain + "; path=/;";
            String deleteCooie = cookieDomain;
            deleteCooie.append("\t");
            deleteCooie.append(strBoolean);
            deleteCooie.append("\t");
            deleteCooie.append(strPath);
            deleteCooie.append("\t");
            deleteCooie.append(strSecure);
            deleteCooie.append("\t");
            deleteCooie.append("123");
            deleteCooie.append("\t");
            deleteCooie.append(strName);
            deleteCooie.append("\t");
            deleteCooie.append("none");
            needDeleteCookies.append(deleteCooie);
        }

        item = item->next;
    }

    for (size_t i = 0; i < needDeleteCookies.size(); ++i) {
        String deleteCooie = needDeleteCookies[i];
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, deleteCooie.utf8().data());
    }
    curl_easy_setopt(curl, CURLOPT_COOKIELIST, "Set-cookie: __deleteCookie__=none;expires=Monday, 13-Jun-1988 03:04:55 GMT; domain=testmbcookie.com");

    curl_slist_free_all(list);
    curl_easy_cleanup(curl);
}

struct CookieVisitorForDeleteInfo {
    KURL url;
    String cookieName;
};

static bool cookieVisitorForDelete(void* params, const char* name, const char* value, const char* domain, const char* path, int secure, int httpOnly, int* expires)
{
    CookieVisitorForDeleteInfo* info = (CookieVisitorForDeleteInfo*)params;
    if (domainMatch(domain, info->url.host()))
        return true;
    return false;
}

void WebCookieJarImpl::deleteCookies(const KURL& url, const String& cookieName)
{
    CookieVisitorForDeleteInfo info;
    info.url = url;
    info.cookieName = cookieName;
    visitAllCookie(&info, cookieVisitorForDelete);
}

String WebCookieJarImpl::cookiesForSession(const KURL&, const KURL& url, bool httponly)
{
    if (!net::WebURLLoaderManager::sharedInstance())
        return "";

    String cookies;
    CURL* curl = curl_easy_init();

    if (!curl)
        return cookies;

    CURLSH* curlsh = net::WebURLLoaderManager::sharedInstance()->getCurlShareHandle();

    curl_easy_setopt(curl, CURLOPT_SHARE, curlsh);

    curl_slist* list = nullptr;
    curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &list);

    if (list) {
        String domain = url.host();
        String path = url.path();
        StringBuilder cookiesBuilder;

        struct curl_slist* item = list;
        while (item) {
            const char* cookie = item->data;
            addMatchingCurlCookie(cookie, domain, path, cookiesBuilder, httponly);
            item = item->next;
        }

        cookies = cookiesBuilder.toString();
        curl_slist_free_all(list);
    }

    curl_easy_cleanup(curl);

    return cookies;
}

WebCookieJarImpl::WebCookieJarImpl()
{

}

WebCookieJarImpl::~WebCookieJarImpl()
{

}

//----WebCookieJar----
void WebCookieJarImpl::setCookie(const WebURL& webUrl, const WebURL& webFirstPartyForCookies, const WebString& webCookie)
{
    setCookiesFromDOM(webFirstPartyForCookies, webUrl, webCookie);
}

WebString WebCookieJarImpl::cookies(const WebURL& webUrl, const WebURL& webFirstPartyForCookies)
{
    return cookiesForSession(webFirstPartyForCookies, webUrl, false);
}

WebString WebCookieJarImpl::cookieRequestHeaderFieldValue(const WebURL& webUrl, const WebURL& webFirstPartyForCookies)
{
    return cookiesForSession(webFirstPartyForCookies, webUrl, false);
}

void WebCookieJarImpl::setToRecordFromRawHeads(const KURL& url, const String& rawHeadsString)
{
    notImplemented();
}

void WebCookieJarImpl::setCookieFromWinINet(const KURL& url, const Vector<char>& cookiesLine)
{
    notImplemented();
}

//////////////////////////////////////////////////////////////////////////

WebCookieJarImpl* WebCookieJarImpl::inst()
{
    if (!m_inst)
        m_inst = new WebCookieJarImpl();
    return m_inst;
}

} // namespace content