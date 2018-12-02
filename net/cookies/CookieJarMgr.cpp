
#include "net/cookies/CookieJarMgr.h"
#include "net/cookies/WebCookieJarCurlImpl.h"

namespace net {

CookieJarMgr* CookieJarMgr::m_inst = nullptr;

WebCookieJarImpl* CookieJarMgr::createOrGet(const std::string& fullPath)
{
    WebCookieJarImpl* cookiejar = nullptr;
    std::map<std::string, WebCookieJarImpl*>::iterator it = m_pathToCookies.find(fullPath);
    if (m_pathToCookies.end() != it)
        return it->second;

    cookiejar = WebCookieJarImpl::create(fullPath);
    m_pathToCookies.insert(std::pair<std::string, WebCookieJarImpl*>(fullPath, cookiejar));
    return cookiejar;
}
    
}