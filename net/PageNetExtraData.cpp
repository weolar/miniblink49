
#define CURL_STATICLIB  

#include "net/PageNetExtraData.h"
#include "net/WebURLLoaderManagerUtil.h"
#include "net/cookies/WebCookieJarCurlImpl.h"
#include "net/cookies/CookieJarMgr.h"

namespace net {
    
PageNetExtraData::PageNetExtraData()
{
    m_cookieJar = nullptr;
}

PageNetExtraData::~PageNetExtraData()
{
    if (m_cookieJar)
        delete m_cookieJar;
}

void PageNetExtraData::setCookieJarFullPath(const std::string& path)
{
    WTF::Mutex* mutex = sharedResourceMutex(CURL_LOCK_DATA_COOKIE);
    WTF::Locker<WTF::Mutex> locker(*mutex);

    if (m_cookieJar) {
        OutputDebugStringA("PageNetExtraData::setCookieJarPath has benn set");
        return;
    }

    WebCookieJarImpl* cookieJar = CookieJarMgr::getInst()->createOrGet(path);
    m_cookieJar = cookieJar;
}

CURLSH* PageNetExtraData::getCurlShareHandle()
{
    if (m_cookieJar)
        return m_cookieJar->getCurlShareHandle();
    return nullptr;
}

std::string PageNetExtraData::getCookieJarFullPath()
{
    if (m_cookieJar)
        return m_cookieJar->getCookieJarFullPath();
    return "";
}

}