
#include "third_party/WebKit/Source/web/WebStorageAreaImpl.h"
#include "third_party/WebKit/public/platform/WebURL.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/wtf/text/StringBuilder.h"
#include "net/FileSystem.h"
#include "wtf/text/WTFStringUtil.h"

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



static String buildLocalStorageFileNameString(const KURL& originUrl)
{
    String localStoragePath = kLocalStorageDirectoryName;
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
        if ('\n' != buffer[i])
            continue;

        const char* posEnd = &buffer[i];
        String keyOrValue(pos, posEnd - pos);
        if (isKey)
            key = keyOrValue;
        else {
            value = keyOrValue;
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

    if (!net::fileExists(kLocalStorageDirectoryName))
        net::createDirectory(kLocalStorageDirectoryName);

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
    notImplemented();
    return WebString();
}

WebString WebStorageAreaImpl::getItem(const WebString& key)
{
    DOMStorageMap::iterator it = m_cachedArea->find(m_origin);
    if (it == m_cachedArea->end())
        return WebString();

    HashMap<String, String>::iterator keyValueIt = (it->value)->find((String)key);
    if (keyValueIt == (it->value)->end())
        return WebString();
    return WebString(keyValueIt->value);
}

void WebStorageAreaImpl::setItem(const WebString& key, const WebString& value,
    const WebURL& pageUrl, WebStorageArea::Result& result)
{
    String pageString = (String)pageUrl.string();
    String origin = buildOriginString(pageUrl);

    DOMStorageMap::iterator it = m_cachedArea->find(origin);
    HashMap<String, String>* pageStorageArea;
    if (it == m_cachedArea->end()) {
        pageStorageArea = new HashMap<String, String>();
        m_cachedArea->set(origin, pageStorageArea);
    } else
        pageStorageArea = it->value;

    pageStorageArea->set(key, value);
    result = WebStorageArea::ResultOK;

    if (m_delaySaveTimer.isActive())
        m_delaySaveTimer.stop();
    m_delaySaveTimer.startOneShot(30000, FROM_HERE);
}

void WebStorageAreaImpl::removeItem(const WebString& key, const WebURL& pageUrl)
{
    String pageString = (String)pageUrl.string();
    DOMStorageMap::iterator it = m_cachedArea->find(pageString);
    if (it == m_cachedArea->end())
        return;

    it->value->remove((String)key);

    if (0 == it->value->size()) {
        delete it->value;
        m_cachedArea->remove(it);
    }
}

void WebStorageAreaImpl::clear(const WebURL& url)
{
    String pageString = (String)url.string();
    DOMStorageMap::iterator it = m_cachedArea->find(pageString);
    if (it == m_cachedArea->end())
        return;

    delete it->value;
    m_cachedArea->remove(it);
}

size_t WebStorageAreaImpl::memoryBytesUsedByCache() const
{
    return 100;
}

} // blink