// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_DOM_STORAGE_WEBSTORAGENAMESPACE_IMPL_H_
#define CONTENT_RENDERER_DOM_STORAGE_WEBSTORAGENAMESPACE_IMPL_H_

#include "net/StorageDef.h"
#include "third_party/WebKit/public/platform/WebStorageNamespace.h"
#include "wtf/HashMap.h"
#include "wtf/text/WTFString.h"
#include "wtf/text/StringHash.h"

namespace net {

class WebStorageNamespaceImpl : public blink::WebStorageNamespace {
public:
    // The default constructor creates a local storage namespace, the second
    // constructor should be used for session storage namepaces.
    WebStorageNamespaceImpl(const String& localPath, int64 namespaceId, DOMStorageMap* cachedArea, bool isLocal);
    virtual ~WebStorageNamespaceImpl();

    // See WebStorageNamespace.h for documentation on these functions.
    virtual blink::WebStorageArea* createStorageArea(const blink::WebString& origin) override;
    virtual blink::WebStorageNamespace* copy();
    virtual bool isSameNamespace(const WebStorageNamespace&) const override;

private:
    DOMStorageMap* m_cachedArea;
    int64 m_namespaceId;
    bool m_isLocal;
    String m_localPath;
};

}  // namespace content

#endif  // CONTENT_RENDERER_DOM_STORAGE_WEBSTORAGENAMESPACE_IMPL_H_
