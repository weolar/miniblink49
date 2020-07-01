
#include "net/WebStorageAreaImpl.h"

#include "third_party/WebKit/public/web/WebStorageEventDispatcher.h"
#include "third_party/WebKit/public/platform/WebURL.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/wtf/text/StringBuilder.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "net/FileSystem.h"
#include "wtf/text/WTFStringUtil.h"

namespace net {

//String* kDefaultLocalStorageFullPath = nullptr;

static String buildOriginString(const blink::KURL& pageUrl)
{
    WTF::StringBuilder builder;
    String origin = pageUrl.protocol();
    if (pageUrl.protocol() == "file") {
        return "file://";
    }

    builder.append(pageUrl.protocol());
    builder.appendLiteral("://");
    builder.append(pageUrl.host());

    if (pageUrl.hasPort()) {
        builder.append(':');
        builder.appendNumber(pageUrl.port());
    }

    return builder.toString();
}

static String buildOriginLocalFileNameString(const blink::KURL& pageUrl)
{
    WTF::StringBuilder builder;
    String origin = pageUrl.protocol();
    if (pageUrl.protocol() == "file") {
        return "file";
    }

    builder.append(pageUrl.protocol());
    builder.appendLiteral("_");
    builder.append(pageUrl.host());

    if (pageUrl.hasPort()) {
        builder.append('_');
        builder.appendNumber(pageUrl.port());
    }

    return builder.toString();
}

//static char* kLocalStorageDirectoryName = "LocalStorage";
static char* kLocalStorageExtensionName = ".localstorage";

static char* kSeparator = "--mb-sep--\n";
static size_t kSeparatorLength = 11;

static char* kEmptySeprator = "--mb-ept--";// (char)0x1f;
static size_t kEmptySepratorLength = 10;

static String buildLocalStorageDirectoryPath(const String& localPath)
{
//     String localStoragePath;
//     localStoragePath = localPath;
//     localStoragePath.append(kLocalStorageDirectoryName);
//     return localStoragePath;
    return localPath;
}

static String buildLocalStorageFileNameString(const String& localPath, const blink::KURL& originUrl)
{
    String localStoragePath;
    localStoragePath.append(buildLocalStorageDirectoryPath(localPath));
    localStoragePath.append('\\');
    localStoragePath.append(buildOriginLocalFileNameString(originUrl));
    localStoragePath.append(kLocalStorageExtensionName);

    return localStoragePath;
}

WebStorageAreaImpl::WebStorageAreaImpl(/*const String& localPath, */net::DOMStorageMap* cachedArea, const blink::WebString& origin, bool isLocal)
    : m_cachedArea(cachedArea)
    , m_isLocal(isLocal)
    , m_delaySaveTimer(this, &WebStorageAreaImpl::delaySaveTimerFired)
    , m_iteratorIndex(UINT_MAX)
{
    m_origin = (String)origin;
    
    //loadFromFile(localPath);
}

WebStorageAreaImpl::~WebStorageAreaImpl()
{
    delaySaveTimerFired(nullptr);
}

void WebStorageAreaImpl::loadFromBufferImpl(const Vector<char>& buffer, const blink::KURL& originUrl)
{
    const char* pos = &buffer[0];
    bool isKey = true;
    String key;
    String value;
    for (size_t i = 0; i < buffer.size() - kSeparatorLength + 1; ++i) {
        if (0 != strncmp(kSeparator, &buffer[i], kSeparatorLength))
            continue;

        const char* posEnd = &buffer[i];
        String keyOrValue(pos, posEnd - pos);
        if (isKey)
            key = keyOrValue;
        else {
            value = keyOrValue;

            key = WTF::ensureUTF16String(key);
            value = WTF::ensureUTF16String(value);

            if (value == kEmptySeprator)
                value = "";

            WebStorageArea::Result result;
            setItemImpl(key, value, originUrl, result, true);
        }
        pos = posEnd + kSeparatorLength;
        isKey = !isKey;
        i += kSeparatorLength;
    }
}

void WebStorageAreaImpl::loadFromFile(const String& localPath)
{
    if (!m_isLocal)
        return;

    m_localPath = localPath;

    static bool isFirstLoad = true;
    if (!isFirstLoad)
        return;
    isFirstLoad = false;

    blink::KURL originUrl(blink::ParsedURLString, m_origin);
    if (!originUrl.isValid())
        return;

    String localStoragePath = buildLocalStorageFileNameString(localPath, originUrl);

    net::PlatformFileHandle handle = openFile(localStoragePath, net::OpenForRead);
    if (!net::isHandleValid(handle))
        return;
    long long size = 0;
    if (!net::getFileSize(handle, size) || 0 == size) {
        net::closeFile(handle);
        return;
    }

    Vector<char> buffer;
    buffer.resize((size_t)size);
    int readResult = net::readFromFile(handle, buffer.data(), buffer.size());
    if (0 == readResult) {
        net::closeFile(handle);
        return;
    }

    if (buffer.size() < kSeparatorLength || 0 != strncmp(kSeparator, &buffer[buffer.size() - kSeparatorLength], kSeparatorLength))
        buffer.append(kSeparator, kSeparatorLength);
    
    loadFromBufferImpl(buffer, originUrl);
    
    net::closeFile(handle);
}

void WebStorageAreaImpl::delaySaveTimerFired(blink::Timer<WebStorageAreaImpl>*)
{
    if (!m_isLocal)
        return;
    blink::KURL originUrl(blink::ParsedURLString, m_origin);
    if (!originUrl.isValid())
        return;

    net::DOMStorageMap::iterator it = m_cachedArea->find(m_origin);
    HashMap<String, String>* pageStorageArea;
    if (it == m_cachedArea->end())
        return;
    pageStorageArea = it->value;
    if (0 == pageStorageArea->size())
        return;

    String localStorageDirectoryPath = buildLocalStorageDirectoryPath(m_localPath);
    if (!net::fileExists(localStorageDirectoryPath))
        net::createDirectory(localStorageDirectoryPath);

    String localStoragePath = buildLocalStorageFileNameString(m_localPath, originUrl);

    net::deleteFile(localStoragePath);
    net::PlatformFileHandle handle = openFile(localStoragePath, net::OpenForWrite);
    if (!net::isHandleValid(handle))
        return;

    Vector<char> buffer;
    for (HashMap<String, String>::const_iterator it = pageStorageArea->begin(); it != pageStorageArea->end(); ++it) {
        const String& key = it->key;
        Vector<char> keyBuffer = WTF::ensureStringToUTF8(key, false);
        const String& value = it->value;
        Vector<char> valueBuffer = WTF::ensureStringToUTF8(value, false);

        if (0 == keyBuffer.size())
            buffer.append(kEmptySeprator, kEmptySepratorLength);
        else
            buffer.append(keyBuffer.data(), keyBuffer.size());
        buffer.append(kSeparator, kSeparatorLength);

        if (0 == valueBuffer.size())
            buffer.append(kEmptySeprator, kEmptySepratorLength);
        else
            buffer.append(valueBuffer.data(), valueBuffer.size());
        buffer.append(kSeparator, kSeparatorLength);
    }

    net::writeToFile(handle, buffer.data(), buffer.size());
    net::closeFile(handle);
}

unsigned WebStorageAreaImpl::length()
{
    net::DOMStorageMap::iterator it = m_cachedArea->find(m_origin);
    if (it == m_cachedArea->end())
        return 0;

    return it->value->size();
}

blink::WebString WebStorageAreaImpl::key(unsigned index)
{
    unsigned len = length();
    if (0 == len)
        return String();

    if (index >= length())
        return String();

    if (!setIteratorToIndex(index))
        return String();

    const String& key = m_iterator->key;
    if (key.isNull() || key.isEmpty())
        return String();

    return key;
}

bool WebStorageAreaImpl::setIteratorToIndex(unsigned index)
{
    // FIXME: Once we have bidirectional iterators for HashMap we can be more intelligent about this.
    // The requested index will be closest to begin(), our current iterator, or end(), and we
    // can take the shortest route.
    // Until that mechanism is available, we'll always increment our iterator from begin() or current.
    net::DOMStorageMap::iterator it = m_cachedArea->find(m_origin);
    if (it == m_cachedArea->end())
        return false;
    HashMap<String, String>* pageStorageArea = it->value;

    if (0 == index) {
        setToIteratorZero(pageStorageArea);
        return true;
    }

    if (m_iteratorIndex == index)
        return true;

    if (index < m_iteratorIndex)
        setToIteratorZero(pageStorageArea);

    while (m_iteratorIndex < index) {
        ++m_iteratorIndex;
        ++m_iterator;
        RELEASE_ASSERT(m_iterator != pageStorageArea->end());
    }

    if (m_iterator == pageStorageArea->end())
        return false;
    return true;
}

void WebStorageAreaImpl::setToIteratorZero(HashMap<String, String>* pageStorageArea)
{
    m_iteratorIndex = 0;
    m_iterator = pageStorageArea->begin();
    RELEASE_ASSERT(m_iterator != pageStorageArea->end());
}

blink::WebString WebStorageAreaImpl::getItem(const blink::WebString& key)
{
    if (key.isNull() || key.isEmpty())
        return blink::WebString();

    String keyString = key;

    net::DOMStorageMap::iterator it = m_cachedArea->find(m_origin);
    if (it == m_cachedArea->end())
        return blink::WebString();

    HashMap<String, String>* pageStorageArea = it->value;
    size_t size = pageStorageArea->size();

//     HashMap<String, String>::iterator itor = pageStorageArea->begin();
//     for (; itor != pageStorageArea->end(); ++itor) {
//         String keyStr = itor->key;
//         String valueStr = itor->value;
// 
//         String output = String::format("WebStorageAreaImpl::getItem: %s , %s\n", keyStr.utf8().data(), valueStr.utf8().data());
//         OutputDebugStringA(output.utf8().data());
//     }

    HashMap<String, String>::iterator keyValueIt = pageStorageArea->find(keyString);
    if (keyValueIt == pageStorageArea->end())
        return blink::WebString();

    String value(keyValueIt->value);
    return value;
}

void WebStorageAreaImpl::setItemImpl(const blink::WebString& key, const blink::WebString& value, const blink::WebURL& pageUrl, blink::WebStorageArea::Result& result, bool isFromLoad)
{
    String pageString = (String)pageUrl.string();
    String origin = buildOriginString(pageUrl);
    String keyString = key;
    String valueString = value;
    if (keyString.isNull())
        keyString = "";
    if (valueString.isNull())
        valueString = "";

    net::DOMStorageMap::iterator it = m_cachedArea->find(origin);
    HashMap<String, String>* pageStorageArea;
    if (it == m_cachedArea->end()) {
        pageStorageArea = new HashMap<String, String>();
        m_cachedArea->set(origin, pageStorageArea);
    } else
        pageStorageArea = it->value;

    size_t sizeOld = pageStorageArea->size();

    String oldValue;
    HashMap<String, String>::iterator iter = pageStorageArea->find(keyString);
    if (pageStorageArea->end() != iter)
        oldValue = iter->value;
    pageStorageArea->set(keyString, valueString);
    size_t size = pageStorageArea->size();

    result = WebStorageArea::ResultOK;

    if (m_delaySaveTimer.isActive())
        m_delaySaveTimer.stop();
    if (m_isLocal)
        m_delaySaveTimer.startOneShot(0.5, FROM_HERE);

    invalidateIterator();
    dispatchStorageEvent(keyString, oldValue, valueString, pageUrl);
}

void WebStorageAreaImpl::setItem(const blink::WebString& key, const blink::WebString& value, const blink::WebURL& pageUrl, blink::WebStorageArea::Result& result)
{
    setItemImpl(key, value, pageUrl, result, false);
}

void WebStorageAreaImpl::removeItem(const blink::WebString& key, const blink::WebURL& pageUrl)
{
    String pageString = (String)pageUrl.string();
    String origin = buildOriginString(pageUrl);
    String keyString = key;

    net::DOMStorageMap::iterator it = m_cachedArea->find(origin);
    if (it == m_cachedArea->end())
        return;

    HashMap<String, String>* pageStorageArea = it->value;
    size_t size = pageStorageArea->size();

    String oldValue;
    HashMap<String, String>::iterator iter = pageStorageArea->find(keyString);
    if (pageStorageArea->end() != iter)
        oldValue = iter->value;

    pageStorageArea->remove(keyString);
    invalidateIterator();

    size = pageStorageArea->size();
    if (0 == size) {
        delete it->value;
        m_cachedArea->remove(it);
    }

    dispatchStorageEvent(key, oldValue, String(), pageUrl);
}

void WebStorageAreaImpl::clear(const blink::WebURL& pageUrl)
{
    String pageString = (String)pageUrl.string();
    String origin = buildOriginString(pageUrl);

    net::DOMStorageMap::iterator it = m_cachedArea->find(origin);
    if (it == m_cachedArea->end())
        return;

    delete it->value;
    m_cachedArea->remove(it);

    invalidateIterator();
    dispatchStorageEvent(String(), String(), String(), pageUrl);
}

void WebStorageAreaImpl::dispatchStorageEvent(const String& key, const String& oldValue, const String& newValue, const blink::WebURL& pageUrl)
{
    if (oldValue == newValue)
        return;

    if (m_isLocal) {
        blink::WebStorageEventDispatcher::dispatchLocalStorageEvent(key, oldValue, newValue, blink::KURL(blink::ParsedURLString, m_origin), pageUrl, nullptr, true);
    } else {
        blink::WebStorageNamespace* webStorageNamespace = ((content::BlinkPlatformImpl*)blink::Platform::current())->createSessionStorageNamespace();
        blink::WebStorageEventDispatcher::dispatchSessionStorageEvent(key, oldValue, newValue, blink::KURL(blink::ParsedURLString, m_origin), pageUrl, *webStorageNamespace, nullptr, true);
    }
}

void WebStorageAreaImpl::invalidateIterator()
{
    net::DOMStorageMap::iterator it = m_cachedArea->find(m_origin);
    if (it == m_cachedArea->end())
        return;

    m_iterator = it->value->end();
    m_iteratorIndex = UINT_MAX;
}

size_t WebStorageAreaImpl::memoryBytesUsedByCache() const
{
    return 100;
}

} // blink