
#include <windows.h>
#if USING_VC6RT != 1
#include <WinInet.h>
#endif
#include "content/web_impl_win/WebCookieJarImpl.h"
#include "third_party/WebKit/public/platform/WebURL.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "net/cookies/ParsedCookie.h"
#include "net/cookies/CookieMonster.h"

using namespace blink;

namespace content {

WebCookieJarImpl* WebCookieJarImpl::m_inst = nullptr;

WebCookieJarImpl::WebCookieJarImpl()
{
    m_cookieMonster = new net::CookieMonster();
}

WebCookieJarImpl::~WebCookieJarImpl()
{
    delete m_cookieMonster;
}

inline bool isParentDomain(const String& domain, net::ParsedCookie* parsedCookie)
{
	if (!parsedCookie->HasDomain())
		return true;

	const String& reference = parsedCookie->Domain().c_str();
	if (reference.isEmpty())
		return true;

	if (domain.endsWith(reference))
		return true;

	if (domain.length() + 1 == reference.length())
		return reference.endsWith(domain);

	return false;
}

inline bool isParentPath(const String& path, net::ParsedCookie* parsedCookie)
{
	if (!parsedCookie->HasPath())
		return true;

	const String& reference = parsedCookie->Path().c_str();
	return path.startsWith(reference);
}

//----WebCookieJar----
void WebCookieJarImpl::setCookie(const WebURL& webUrl, const WebURL& webFirstPartyForCookies, const WebString& webCookie)
{
    String url = (String)(webUrl.string());
    String cookie = (String)webCookie;

    Vector<char> urlBuffer = WTF::ensureStringToUTF8(url);
    Vector<char> cookieBuffer = WTF::ensureStringToUTF8(cookie);
    ::InternetSetCookieA(urlBuffer.data(), NULL, cookieBuffer.data());

    String rawCookiesHeads = "Set-Cookie: "; 
    rawCookiesHeads.append(cookieBuffer.data());
    setToRecordFromRawHeads(KURL(ParsedURLString, urlBuffer.data()), rawCookiesHeads);
}

// HTTP/1.1 200 OK
// Cache-Control: private,no-cache, no-store, must-revalidate
// Pragma: no-cache,no-cache
// Content-Length: 39336
// Content-Type: text/html; charset=utf-8
// Expires: 0
// X-AspNet-Version: 2.0.50727
// X-b365-ScriptVersion: 4.2.25.0|2.1.49.0|1.6.18.0
// X-UA-Compatible: requiresActiveX=true
// X-b365-AppStatus: Ok
// Set-Cookie: session=processform=0; path=/; secure
// Set-Cookie: pstk=F2045B52B9A04D0BAC692ECD32B0AEB0000003; domain=.48-365365.com; path=/; secure
// Set-Cookie: Hm_lvt_90056b3f84f90da57dc0f40150f005d5=1462436025; domain=passport.baidu.com; path=/; expires=Fri, 05 May 2017 08:13:45 GMT
// Date: Mon, 14 Mar 2016 12:18:39 GMT
void WebCookieJarImpl::setToRecordFromRawHeads(const KURL& url, const String& rawHeadsString)
{
	String rawHeads = ensureStringToUTF8String(rawHeadsString);
	size_t pos = 0;
	size_t endPos = rawHeads.length();

	if (WTF::kNotFound != rawHeads.find("XSRF-TOKEN"))
		OutputDebugStringA("");

	WTF::Vector<String> items; // 分离出Expires: 0这样的一项
	WTF::splitStringToVector(rawHeads, '\n', true, items);
	for (size_t i = 0; i < items.size(); ++i) {
		size_t findPos = items[i].find("Set-Cookie:");
		if (WTF::kNotFound == findPos)
			continue;

		String cookieItems = items[i].right(items[i].length() - findPos - sizeof("Set-Cookie:"));
		if (cookieItems.isNull() || cookieItems.isEmpty())
			continue;

		net::ParsedCookie parsedCookie(WTF::WTFStringToStdString(cookieItems));
		if (!parsedCookie.IsValid())
			return;

		if (parsedCookie.Name() == "XSRF-TOKEN")
			m_X_XSRF_TOKEN = parsedCookie.Value().c_str();

		net::CanonicalCookie* cc = new net::CanonicalCookie(url, parsedCookie);
		m_cookieMonster->SetCanonicalCookie(cc, WTF::currentTime(), net::CookieOptions());
	}
}

void WebCookieJarImpl::setCookieFromWinINet(const KURL& url, const Vector<char>& cookiesLine)
{
	net::ParsedCookie::PairList pairs = net::ParsedCookie::ParseNameValuePairs(cookiesLine.data());
	for (size_t i = 0; i < pairs.size(); ++i) {
		const net::ParsedCookie::TokenValuePair& pair = pairs[i];
		if (pair.first == "XSRF-TOKEN")
			m_X_XSRF_TOKEN = pair.second.c_str();
		m_cookieMonster->UpdataCookie(url, pair.first, pair.second);
	}
}

WebString WebCookieJarImpl::cookies(const WebURL& webUrl, const WebURL& webFirstPartyForCookies)
{
	KURL url = webUrl;
	KURL firstPartyForCookies = webFirstPartyForCookies;

	String domain = url.host();
	String path = url.path();

	net::CookieList cookisList = m_cookieMonster->GetAllCookiesForURLWithOptions(url, net::CookieOptions());

	String result;
	for (size_t i = 0; i < cookisList.size(); ++i) {
		if (i > 0)
			result.append("; ");

		result.append(cookisList.at(i).Name().c_str());
		result.append("=");
		result.append(cookisList.at(i).Value().c_str());
	}

	if (0) {
		OutputDebugStringA(result.utf8().data());
		OutputDebugStringA("\n");
	}

// 	OutputDebugStringW(L"getFromRecord:");
// 	OutputDebugStringW(result.charactersWithNullTermination().data());
// 	OutputDebugStringW(L"\n");

	return result;
}

WebString WebCookieJarImpl::cookieRequestHeaderFieldValue(const WebURL& url, const WebURL& firstPartyForCookies)
{
    notImplemented();
    return WebString();
}

//////////////////////////////////////////////////////////////////////////


// void WebCookieJarImpl::setToRecordFromRawHeads(const KURL& url, const String& rawHeadsString)
// {
//     String rawHeads = ensureStringToUTF8String(rawHeadsString);
// 	size_t pos = 0;
// 	size_t endPos = rawHeads.length();
// 
// 	if (WTF::kNotFound != rawHeads.find("XSRF-TOKEN"))
// 		OutputDebugStringA("");
// 
// 	WTF::Vector<String> items; // 分离出Expires: 0这样的一项
// 	WTF::splitStringToVector(rawHeads, '\n', true, items);
// 	for (size_t i = 0; i < items.size(); ++i) {
// 		size_t findPos = items[i].find("Set-Cookie:");
// 		if (WTF::kNotFound == findPos)
// 			continue;
// 
// 		String cookieItems = items[i].right(items[i].length() - findPos - sizeof("Set-Cookie:"));
// 		if (cookieItems.isNull() || cookieItems.isEmpty())
// 			continue;
// 
// 		net::ParsedCookie* parsedCookie = new net::ParsedCookie(cookieItems.utf8().data());
// 		if (!parsedCookie->IsValid()) {
// 			delete parsedCookie;
// 			return;
// 		}
// 
// 		bool find = false;
// 		for (size_t i = 0; i < m_parsedCookies.size(); ++i) {
// 			net::ParsedCookie* parsedCookieItem = m_parsedCookies[i];
// 			if (parsedCookieItem->HasDomain() != parsedCookie->HasDomain() &&
// 				parsedCookieItem->Domain() != parsedCookie->Domain())
// 				continue;
// 
// 			if (parsedCookieItem->Name() != parsedCookie->Name())
// 				continue;
// 
// 			ASSERT(!find);
// 			delete parsedCookieItem;
// 			m_parsedCookies[i] = parsedCookie;
// 			find = true;
// 		}
// 		if (!find)
// 			m_parsedCookies.append(parsedCookie);
// 	}	
// }

//////////////////////////////////////////////////////////////////////////



WebCookieJarImpl* WebCookieJarImpl::inst()
{
    if (!m_inst)
        m_inst = new WebCookieJarImpl();
    return m_inst;
}

} // namespace content