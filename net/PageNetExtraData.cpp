
#define CURL_STATICLIB  

#include "net/PageNetExtraData.h"
#include "net/WebURLLoaderManagerUtil.h"
#include "net/cookies/WebCookieJarCurlImpl.h"
#include "net/cookies/CookieJarMgr.h"
#include "net/StorageMgr.h"
#include "net/WebStorageNamespaceImpl.h"
#include <Shlwapi.h>

namespace net {
    
PageNetExtraData::PageNetExtraData()
{
    m_cookieJar = nullptr;
    m_localStorage = nullptr;
}

PageNetExtraData::~PageNetExtraData()
{
    if (m_cookieJar)
        delete m_cookieJar;
}

void PageNetExtraData::setCookieJarFullPath(const std::string& fullPathUtf8)
{
    WTF::Mutex* mutex = sharedResourceMutex(CURL_LOCK_DATA_COOKIE);
    WTF::Locker<WTF::Mutex> locker(*mutex);

    if (m_cookieJar) {
        OutputDebugStringA("PageNetExtraData::setCookieJarPath has been set");
        return;
    }

    WebCookieJarImpl* cookieJar = CookieJarMgr::getInst()->createOrGet(fullPathUtf8);
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

String* kDefaultLocalStorageFullPath = nullptr;

blink::WebStorageNamespace* PageNetExtraData::createWebStorageNamespace()
{       
    if (!kDefaultLocalStorageFullPath) {
        std::vector<wchar_t> path;
        path.resize(MAX_PATH + 1);
        memset(&path.at(0), 0, sizeof(wchar_t) * (MAX_PATH + 1));
        ::GetModuleFileNameW(nullptr, &path.at(0), MAX_PATH);
        ::PathRemoveFileSpecW(&path.at(0));

        size_t size = wcslen(&path.at(0));

        kDefaultLocalStorageFullPath = new String(&path.at(0), size);
        UChar c = kDefaultLocalStorageFullPath->characters16()[size - 1];
        if (L'\\' != c && L'/' != c)
            kDefaultLocalStorageFullPath->append(L'\\');
    }
    
    if (m_localStotageFullPath.isEmpty())
        m_localStotageFullPath = *kDefaultLocalStorageFullPath;

    WebStorageNamespaceImpl* storageArea = nullptr;
    if (m_localStorage) {
        storageArea = new WebStorageNamespaceImpl(m_localStotageFullPath, kLocalStorageNamespaceId, m_localStorage, true);
        return (blink::WebStorageNamespace*)storageArea;
    }

    WTF::Vector<char> buffer = WTF::ensureStringToUTF8(m_localStotageFullPath, true);
    setLocalStorageFullPath(buffer.data());
    
    storageArea = new WebStorageNamespaceImpl(m_localStotageFullPath, kLocalStorageNamespaceId, m_localStorage, true);
    return (blink::WebStorageNamespace*)storageArea;
}

void PageNetExtraData::setLocalStorageFullPath(const std::string& fullPathUtf8)
{
    WTF::Mutex* mutex = sharedResourceMutex(CURL_LOCK_DATA_COOKIE);
    WTF::Locker<WTF::Mutex> locker(*mutex);

    if (m_localStorage) {
        OutputDebugStringA("PageNetExtraData::setStorageAreaFullPath has been set");
        return;
    }
    
    std::vector<UChar> fullPathW;
    WTF::MByteToWChar(fullPathUtf8.c_str(), fullPathUtf8.size(), &fullPathW, CP_UTF8);

    String path(&fullPathW.at(0), fullPathW.size());
    m_localStotageFullPath = path;

    m_localStorage = StorageMgr::getInst()->createOrGet(path);
}

}