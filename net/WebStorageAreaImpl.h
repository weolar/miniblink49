// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_DOM_STORAGE_WEBSTORAGEAREA_IMPL_H_
#define CONTENT_RENDERER_DOM_STORAGE_WEBSTORAGEAREA_IMPL_H_

#include "third_party/WebKit/public/platform/WebStorageArea.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "net/WebStorageNamespaceImpl.h"
#include "third_party/WebKit/Source/platform/Timer.h"
#include "wtf/HashMap.h"

namespace blink {
class KURL;
class DOMStorageCachedArea;
}

namespace net {
    
class WebStorageAreaImpl : public blink::WebStorageArea {
public:
    WebStorageAreaImpl(/*const String& localPath,*/ net::DOMStorageMap* cachedArea, const blink::WebString& origin, bool isLocal);
    virtual ~WebStorageAreaImpl();

    // See WebStorageArea.h for documentation on these functions.
    virtual unsigned length();
    virtual blink::WebString key(unsigned index);
    virtual blink::WebString getItem(const blink::WebString& key);
    virtual void setItem(const blink::WebString& key, const blink::WebString& value, const blink::WebURL& pageUrl, blink::WebStorageArea::Result& result);
    virtual void removeItem(const blink::WebString& key, const blink::WebURL& page_url);
    virtual void clear(const blink::WebURL& url);
    virtual size_t memoryBytesUsedByCache() const;

    void loadFromFile(const String& localPath);

private:
    void loadFromBufferImpl(const Vector<char>& buffer, const blink::KURL& originUrl);
    void delaySaveTimerFired(blink::Timer<WebStorageAreaImpl>*);

    void setItemImpl(const blink::WebString& key, const blink::WebString& value, const blink::WebURL& page_url, blink::WebStorageArea::Result& result, bool isFromLoad);
        
    void invalidateIterator();
    bool setIteratorToIndex(unsigned);
    void setToIteratorZero(HashMap<String, String>* pageStorageArea);

    void dispatchStorageEvent(const String& key, const String& oldValue, const String& newValue, const blink::WebURL& pageUrl);
    
    String m_origin;
    net::DOMStorageMap* m_cachedArea;
    bool m_isLocal;
    String m_localPath;

    blink::Timer<WebStorageAreaImpl> m_delaySaveTimer;

    HashMap<String, String>::iterator m_iterator;
    unsigned m_iteratorIndex;
};

}  // namespace blink

#endif  // CONTENT_RENDERER_DOM_STORAGE_WEBSTORAGEAREA_IMPL_H_
