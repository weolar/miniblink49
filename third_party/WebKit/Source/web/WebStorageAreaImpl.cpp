
#include "third_party/WebKit/Source/web/WebStorageAreaImpl.h"
#include "third_party/WebKit/public/web/WebStorageEventDispatcher.h"
#include "third_party/WebKit/public/platform/WebURL.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/wtf/text/StringBuilder.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "net/FileSystem.h"
#include "wtf/text/WTFStringUtil.h"

extern String* kLocalStorageFullPath;

namespace blink {

static String buildOriginString(const KURL& pageUrl)
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

static String buildOriginLocalFileNameString(const KURL& pageUrl)
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

static char* kLocalStorageDirectoryName = "LocalStorage";
static char* kLocalStorageExtensionName = ".localstorage";
static char kSeparator = (char)0x1f;

static String buildLocalStorageDirectoryPath()
{
    String localStoragePath;
    if (kLocalStorageFullPath)
        localStoragePath = *kLocalStorageFullPath;
    localStoragePath.append(kLocalStorageDirectoryName);
    return localStoragePath;
}

static String buildLocalStorageFileNameString(const KURL& originUrl)
{
    String localStoragePath;
    localStoragePath.append(buildLocalStorageDirectoryPath());
    localStoragePath.append('\\');
    localStoragePath.append(buildOriginLocalFileNameString(originUrl));
    localStoragePath.append(kLocalStorageExtensionName);

    return localStoragePath;
}

WebStorageAreaImpl::WebStorageAreaImpl(DOMStorageMap* cachedArea, const WebString& origin, bool isLocal)
    : m_cachedArea(cachedArea)
    , m_isLocal(isLocal)
    , m_delaySaveTimer(this, &WebStorageAreaImpl::delaySaveTimerFired)
{
    m_origin = (String)origin;

    if (m_isLocal)
        loadFromFile();
}

WebStorageAreaImpl::~WebStorageAreaImpl()
{
    delaySaveTimerFired(nullptr);
}

void WebStorageAreaImpl::loadFromFile()
{
    KURL originUrl(ParsedURLString, m_origin);
    if (!originUrl.isValid())
        return;

    String localStoragePath = buildLocalStorageFileNameString(originUrl);

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

    if (kSeparator != buffer[buffer.size() - 1])
        buffer.append(kSeparator);

    const char* pos = &buffer[0];
    bool isKey = true;
    String key;
    String value;
    for (size_t i = 0; i < buffer.size(); ++i) {
        if (kSeparator != buffer[i])
            continue;

        const char* posEnd = &buffer[i];
        String keyOrValue(pos, posEnd - pos);
        if (isKey)
            key = keyOrValue;
        else {
            value = keyOrValue;

            key = WTF::ensureUTF16String(key);
            value = WTF::ensureUTF16String(value);

            WebStorageArea::Result result;
            setItem(key, value, originUrl, result);
        }
        pos = posEnd + 1;
        isKey = !isKey;
        ++i;
    }
    net::closeFile(handle);
}

void WebStorageAreaImpl::delaySaveTimerFired(blink::Timer<WebStorageAreaImpl>*)
{
    if (!m_isLocal)
        return;
    KURL originUrl(ParsedURLString, m_origin);
    if (!originUrl.isValid())
        return;

    DOMStorageMap::iterator it = m_cachedArea->find(m_origin);
    HashMap<String, String>* pageStorageArea;
    if (it == m_cachedArea->end())
        return;
    pageStorageArea = it->value;
    if (0 == pageStorageArea->size())
        return;

    if (!net::fileExists(buildLocalStorageDirectoryPath()))
        net::createDirectory(buildLocalStorageDirectoryPath());

    String localStoragePath = buildLocalStorageFileNameString(originUrl);

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

        buffer.append(keyBuffer.data(), keyBuffer.size());
        buffer.append(kSeparator);
        buffer.append(valueBuffer.data(), valueBuffer.size());
        buffer.append(kSeparator);
    }

    net::writeToFile(handle, buffer.data(), buffer.size());
    net::closeFile(handle);
}

unsigned WebStorageAreaImpl::length()
{
    DOMStorageMap::iterator it = m_cachedArea->find(m_origin);
    if (it == m_cachedArea->end())
        return 0;

    return it->value->size();
}

WebString WebStorageAreaImpl::key(unsigned index)
{
    if (index >= length())
        return String();

    setIteratorToIndex(index);
    return m_iterator->key;
}

WebString WebStorageAreaImpl::getItem(const WebString& key)
{
    String keyString = key;

    DOMStorageMap::iterator it = m_cachedArea->find(m_origin);
    if (it == m_cachedArea->end())
        return WebString();

    HashMap<String, String>* pageStorageArea = it->value;
    size_t size = pageStorageArea->size();

    HashMap<String, String>::iterator keyValueIt = pageStorageArea->find(keyString);
    if (keyValueIt == pageStorageArea->end())
        return WebString();

    return WebString(keyValueIt->value);
}

void WebStorageAreaImpl::setItem(const WebString& key, const WebString& value, const WebURL& pageUrl, WebStorageArea::Result& result)
{
    String pageString = (String)pageUrl.string();
    String origin = buildOriginString(pageUrl);
    String keyString = key;

    DOMStorageMap::iterator it = m_cachedArea->find(origin);
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
    pageStorageArea->set(keyString, value);
    size_t size = pageStorageArea->size();

    result = WebStorageArea::ResultOK;

    if (m_delaySaveTimer.isActive())
        m_delaySaveTimer.stop();
    if (m_isLocal)
        m_delaySaveTimer.startOneShot(0.5, FROM_HERE);

    invalidateIterator();
    dispatchStorageEvent(key, oldValue, value, pageUrl);
}

void WebStorageAreaImpl::removeItem(const WebString& key, const WebURL& pageUrl)
{
    String pageString = (String)pageUrl.string();
    String origin = buildOriginString(pageUrl);
    String keyString = key;

    DOMStorageMap::iterator it = m_cachedArea->find(origin);
    if (it == m_cachedArea->end())
        return;

    HashMap<String, String>* pageStorageArea = it->value;
    size_t size = pageStorageArea->size();

//     String output = String::format("removeItem: %p %s %d\n", m_cachedArea, keyString.utf8().data(), size);
//     OutputDebugStringA(output.utf8().data());

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

void WebStorageAreaImpl::clear(const WebURL& pageUrl)
{
    String pageString = (String)pageUrl.string();
    String origin = buildOriginString(pageUrl);

    DOMStorageMap::iterator it = m_cachedArea->find(origin);
    if (it == m_cachedArea->end())
        return;

    delete it->value;
    m_cachedArea->remove(it);

    invalidateIterator();
    dispatchStorageEvent(String(), String(), String(), pageUrl);
}

void WebStorageAreaImpl::dispatchStorageEvent(const String& key, const String& oldValue, const String& newValue, const WebURL& pageUrl)
{
    if (oldValue == newValue)
        return;

    if (m_isLocal) {
        WebStorageEventDispatcher::dispatchLocalStorageEvent(key, oldValue, newValue, KURL(ParsedURLString, m_origin), pageUrl, nullptr, true);
    } else {
        WebStorageNamespace* webStorageNamespace = ((content::BlinkPlatformImpl*)Platform::current())->createSessionStorageNamespace();
        WebStorageEventDispatcher::dispatchSessionStorageEvent(key, oldValue, newValue, KURL(ParsedURLString, m_origin), pageUrl, *webStorageNamespace, nullptr, true);
    }
}

void WebStorageAreaImpl::invalidateIterator()
{
    DOMStorageMap::iterator it = m_cachedArea->find(m_origin);
    if (it == m_cachedArea->end())
        return;

    m_iterator = it->value->end();
    m_iteratorIndex = UINT_MAX;
}

void WebStorageAreaImpl::setIteratorToIndex(unsigned index)
{
    // FIXME: Once we have bidirectional iterators for HashMap we can be more intelligent about this.
    // The requested index will be closest to begin(), our current iterator, or end(), and we
    // can take the shortest route.
    // Until that mechanism is available, we'll always increment our iterator from begin() or current.
    DOMStorageMap::iterator it = m_cachedArea->find(m_origin);
    if (it == m_cachedArea->end())
        return;

    if (m_iteratorIndex == index)
        return;

    if (index < m_iteratorIndex) {
        m_iteratorIndex = 0;
        m_iterator = it->value->begin();
        ASSERT(m_iterator != it->value->end());
    }

    while (m_iteratorIndex < index) {
        ++m_iteratorIndex;
        ++m_iterator;
        ASSERT(m_iterator != it->value->end());
    }
}

size_t WebStorageAreaImpl::memoryBytesUsedByCache() const
{
    return 100;
}

} // blink