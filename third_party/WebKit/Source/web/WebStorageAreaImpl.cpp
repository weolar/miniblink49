
#include "third_party/WebKit/Source/web/WebStorageAreaImpl.h"
#include "third_party/WebKit/public/platform/WebURL.h"

namespace blink {

WebStorageAreaImpl::WebStorageAreaImpl(DOMStorageMap& cachedArea, const WebString& origin)
    : m_cachedArea(cachedArea)
{
    m_origin = (String)origin;
}

WebStorageAreaImpl::~WebStorageAreaImpl()
{
    
}

unsigned WebStorageAreaImpl::length()
{
    DOMStorageMap::iterator it = m_cachedArea.find(m_origin);
    if (it == m_cachedArea.end())
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
    DOMStorageMap::iterator it = m_cachedArea.find(m_origin);
    if (it == m_cachedArea.end())
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
    DOMStorageMap::iterator it = m_cachedArea.find(pageString);
    HashMap<String, String>* pageStorageArea;
    if (it == m_cachedArea.end()) {
        pageStorageArea = new HashMap<String, String>();
        m_cachedArea.set(pageString, pageStorageArea);
    } else
        pageStorageArea = it->value;

    pageStorageArea->set(key, value);
    result = WebStorageArea::ResultOK;
}

void WebStorageAreaImpl::removeItem(const WebString& key, const WebURL& pageUrl)
{
    String pageString = (String)pageUrl.string();
    DOMStorageMap::iterator it = m_cachedArea.find(pageString);
    if (it == m_cachedArea.end())
        return;

    it->value->remove((String)key);

    if (0 == it->value->size()) {
        delete it->value;
        m_cachedArea.remove(it);
    }
}

void WebStorageAreaImpl::clear(const WebURL& url)
{
    String pageString = (String)url.string();
    DOMStorageMap::iterator it = m_cachedArea.find(pageString);
    if (it == m_cachedArea.end())
        return;

    delete it->value;
    m_cachedArea.remove(it);
}

size_t WebStorageAreaImpl::memoryBytesUsedByCache() const
{
    return 100;
}

} // blink