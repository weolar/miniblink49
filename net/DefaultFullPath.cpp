#include "net/DefaultFullPath.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "third_party/libcurl/include/curl/curl.h"
#include "net/WebURLLoaderManagerUtil.h"
#include <Shlwapi.h>

namespace net {

static String* kDefaultLocalStorageFullPath = nullptr;

void setDefaultLocalStorageFullPath(const String& path)
{
    WTF::Mutex* mutex = sharedResourceMutex(CURL_LOCK_DATA_COOKIE);
    WTF::Locker<WTF::Mutex> locker(*mutex);

    if (kDefaultLocalStorageFullPath)
        delete kDefaultLocalStorageFullPath;
    kDefaultLocalStorageFullPath = new String(path);

    if (net::kDefaultLocalStorageFullPath->isEmpty()) {
        delete kDefaultLocalStorageFullPath;
        kDefaultLocalStorageFullPath = nullptr;
        return;
    }

    if (!kDefaultLocalStorageFullPath->endsWith(L'\\'))
        kDefaultLocalStorageFullPath->append(L'\\');
}

String getDefaultLocalStorageFullPath()
{
    WTF::Mutex* mutex = sharedResourceMutex(CURL_LOCK_DATA_COOKIE);
    WTF::Locker<WTF::Mutex> locker(*mutex);

    if (kDefaultLocalStorageFullPath)
        return *kDefaultLocalStorageFullPath;

    std::vector<wchar_t> path;
    path.resize(2 * MAX_PATH);
    memset(&path.at(0), 0, sizeof(wchar_t) * (2 * MAX_PATH));
    ::GetModuleFileNameW(nullptr, &path.at(0), MAX_PATH);
    ::PathRemoveFileSpecW(&path.at(0));
    ::PathAppend(&path.at(0), L"LocalStorage");

    size_t size = wcslen(&path.at(0));

    kDefaultLocalStorageFullPath = new String(&path.at(0), size);
    UChar c = kDefaultLocalStorageFullPath->characters16()[size - 1];
    if (L'\\' != c && L'/' != c)
        kDefaultLocalStorageFullPath->append(L'\\');

    return *kDefaultLocalStorageFullPath;
}

}