
#define CURL_STATICLIB  

#include "net/PageNetExtraData.h"
#include "net/WebURLLoaderManagerUtil.h"
#include "net/cookies/WebCookieJarCurlImpl.h"

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

void PageNetExtraData::setCookieJarFullPath(const std::string& cookieJarFileName)
{
    if (m_cookieJar) {
        OutputDebugStringA("PageNetExtraData::setCookieJarPath has benn set");
        return;
    }

    m_cookieJar = new WebCookieJarImpl(cookieJarFileName);
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