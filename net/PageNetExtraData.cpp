
#define CURL_STATICLIB  

#include "net/PageNetExtraData.h"
#include "net/WebURLLoaderManagerUtil.h"

namespace net {
    
PageNetExtraData::PageNetExtraData()
{
    m_curlShareHandle = nullptr;
    OutputDebugStringA("PageNetExtraData");
}

PageNetExtraData::~PageNetExtraData()
{
    OutputDebugStringA("~~~~PageNetExtraData");
    if (m_curlShareHandle) {
        curl_share_cleanup(m_curlShareHandle);
    }
}

void PageNetExtraData::setCookieJarPath(const std::string& cookieJarFileName)
{
    if (!m_curlShareHandle) {
        m_curlShareHandle = curl_share_init();
        curl_share_setopt(m_curlShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
        curl_share_setopt(m_curlShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
        curl_share_setopt(m_curlShareHandle, CURLSHOPT_LOCKFUNC, net::curl_lock_callback);
        curl_share_setopt(m_curlShareHandle, CURLSHOPT_UNLOCKFUNC, net::curl_unlock_callback);
    }
}

}