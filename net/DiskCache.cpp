
#include "net/DiskCache.h"
#include "net/DefaultFullPath.h"
#include "net/FileSystem.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/core/fetch/Resource.h"
#include "third_party/WebKit/Source/core/fetch/MemoryCache.h"
#include "third_party/WebKit/Source/wtf/StringHasher.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "third_party/WebKit/Source/platform/heap/HeapAllocator.h"
#include "third_party/WebKit/Source/platform/SharedBuffer.h"
#include "base/values.h"
#include "base/json/json_writer.h"
#include "base/json/json_reader.h"
#include "wke/wkeGlobalVar.h"

namespace net {
    
DiskCache::DiskCache()
{
    m_cacheList = nullptr;
}

DiskCache::~DiskCache()
{
    if (m_cacheList)
        delete m_cacheList;
}

static base::ListValue* initFromJsonFileImpl(PlatformFileHandle handle)
{
    std::vector<char> buffer;
    net::readFromFile(handle, &buffer);
    if (0 == buffer.size())
        return nullptr;
    buffer.push_back('\0');

    std::string json(&buffer[0]);

    base::JSONReader jsonReader;
    std::unique_ptr<base::Value> jsonVal = jsonReader.ReadToValue(json);
    if (!jsonVal)
        return nullptr;

    if (!jsonVal->IsType(base::Value::TYPE_LIST))
        return nullptr;

    base::ListValue* jsonListVal = nullptr;
    if (!jsonVal->GetAsList(&jsonListVal))
        return nullptr;

    jsonVal.release();

    return jsonListVal;
}

void DiskCache::initFromJsonFile()
{
    if (!wke::g_diskCacheEnable)
        return;

    String path = net::getDefaultLocalStorageFullPath();
    path.append(L"memcache.js");
    PlatformFileHandle handle = net::openFile(path, net::OpenForRead);
    if (!isHandleValid(handle))
        return;

    m_cacheList = initFromJsonFileImpl(handle);
    closeFile(handle);
}

DiskCacheItem* DiskCache::getCacheUrlItem(const blink::KURL& kurl)
{
    if (!m_cacheList)
        return nullptr;

    for (size_t i = 0; i < m_cacheList->GetSize(); ++i) {
        const base::Value* val = nullptr;
        m_cacheList->Get(i, &val);

        if (!val->IsType(base::Value::TYPE_DICTIONARY))
            continue;

        const base::DictionaryValue* dirVal = nullptr;
        if (!val->GetAsDictionary(&dirVal))
            continue;

        std::string cacheId;
        std::string url;
        std::string mime;
        size_t size = 0;
        int nameInt = 0;
        if (!dirVal->GetString("cacheId", &cacheId))
            continue;
        if (!dirVal->GetString("url", &url))
            continue;
        if (!dirVal->GetInteger("name", &nameInt))
            continue;
        if (!dirVal->GetInteger("size", (int*)&size))
            continue;
        if (!dirVal->GetString("mime", &mime))
            continue;

        if (kurl.getUTF8String() != url.c_str())
            continue;

        String path = net::getDefaultLocalStorageFullPath();
        path.append(String::format("diskcache\\%u.tmp", nameInt));

        PlatformFileHandle handle = openFile(path, OpenForRead);
        if (!isHandleValid(handle))
            return nullptr;

        DiskCacheItem* result = new DiskCacheItem();
        net::readFromFile(handle, &result->content);
        if (0 == result->content.size() || size != result->content.size()) {
            delete result;
            return nullptr;
        }
        result->mime = mime.c_str();

        closeFile(handle);

        deleteFile(path);

        m_cacheList->Remove(i, nullptr);

        return result;
    }

    return nullptr;
}

static const char* getMimeByType(blink::Resource::Type type)
{
    if (blink::Resource::CSSStyleSheet == type)
        return "text/css";
    if (blink::Resource::Script == type)
        return "text/javascript";
    if (blink::Resource::Image == type)
        return "image/png";

    return nullptr;
}

static void deleteDirectoryFiles(const String& directory)
{
    Vector<String> files = listDirectory(directory, "*.*");
    for (size_t i = 0; i < files.size(); ++i) {
        String file = files[i];
        deleteFile(file);
    }
}

void DiskCache::saveMemoryCache()
{
    if (!wke::g_diskCacheEnable)
        return;

    String path = net::getDefaultLocalStorageFullPath();
    path.append(L"memcache.js");
    if (fileExists(path) && !deleteFile(path))
        return;

    if (!recursiveCreateDirectory(net::getDefaultLocalStorageFullPath()))
        return;

    PlatformFileHandle handle = net::openFile(path, net::OpenForWrite);
    if (!isHandleValid(handle))
        return;

    // TODO delete all files
    String diskcacheDir = net::getDefaultLocalStorageFullPath();
    diskcacheDir.append("diskcache\\");
    deleteDirectoryFiles(diskcacheDir);

    base::ListValue cacheJsons;
    bool isCreateDir = false;

    size_t totalSize = 0;
    const size_t kMaxSize = 20 * 1024 * 1024;

    const blink::HeapHashSet<blink::Member<blink::Resource>>& resources = blink::memoryCache()->getLiveResources();
    blink::HeapHashSet<blink::Member<blink::Resource>>::const_iterator it = resources.begin();
    for (; it != resources.end(); ++it) {
        blink::Resource* resource = *it;

        blink::SharedBuffer* buffer = resource->resourceBuffer();
        if (!buffer)
            continue;
        totalSize += buffer->size();

        if (totalSize > kMaxSize)
            break;

        blink::Resource::Type type = resource->type();
        const char* mime = getMimeByType(type);
        if (!mime)
            continue;

        std::string cacheId = WTF::WTFStringToStdString(resource->cacheIdentifier());
        std::string url = WTF::WTFStringToStdString(resource->url().getUTF8String());

        base::DictionaryValue* cacheJson = new base::DictionaryValue();

        unsigned int nameInt = WTF::StringHasher::computeHash<LChar>((const LChar*)url.c_str(), url.size());
        cacheJson->SetString("cacheId", cacheId);
        cacheJson->SetString("url", url);
        cacheJson->SetInteger("name", (int)nameInt);
        String nameStr = String::format("%u", nameInt);
        cacheJson->SetString("nameStr", nameStr.utf8().data());
        cacheJson->SetString("mime", mime);
        cacheJson->SetInteger("size", buffer->size());

        cacheJsons.Append(cacheJson);

        String resPath = net::getDefaultLocalStorageFullPath();

        resPath.append("diskcache\\");
        if (!fileExists(resPath) && !createDirectory(resPath))
            continue;
        
        resPath.append(String::format("%u.tmp", nameInt));
        if (fileExists(resPath) && !deleteFile(resPath))
            continue;
        PlatformFileHandle resHandle = net::openFile(resPath, net::OpenForWrite);
        if (!isHandleValid(resHandle))
            continue;

        net::writeToFile(resHandle, buffer->data(), buffer->size());
    }

    std::string json;
    base::JSONWriter::Write(cacheJsons, &json);

    net::writeToFile(handle, json.c_str(), json.size());

    closeFile(handle);
}

}