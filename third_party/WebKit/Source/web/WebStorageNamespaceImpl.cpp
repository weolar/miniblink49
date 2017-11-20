// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/WebKit/Source/web/WebStorageNamespaceImpl.h"
#include "third_party/WebKit/Source/web/WebStorageAreaImpl.h"

#include "third_party/WebKit/public/platform/WebString.h"

using blink::WebStorageArea;
using blink::WebStorageNamespace;
using blink::WebString;

namespace blink {

WebStorageNamespaceImpl::WebStorageNamespaceImpl(int64 namespaceId, DOMStorageMap* cachedArea, bool isLocal)
    : m_cachedArea(cachedArea)
    , m_namespaceId(namespaceId)
    , m_isLocal(isLocal)
{

}


WebStorageNamespaceImpl::~WebStorageNamespaceImpl() {
}

WebStorageArea* WebStorageNamespaceImpl::createStorageArea(const WebString& origin) 
{
    return new WebStorageAreaImpl(m_cachedArea, origin, m_isLocal);
}

WebStorageNamespace* WebStorageNamespaceImpl::copy() {
    // By returning NULL, we're telling WebKit to lazily fetch it the next time
    // session storage is used.  In the WebViewClient::createView, we do the
    // book-keeping necessary to make it a true copy-on-write despite not doing
    // anything here, now.
    return NULL;
}

bool WebStorageNamespaceImpl::isSameNamespace(const WebStorageNamespace& other) const {
    const WebStorageNamespaceImpl* other_impl = static_cast<const WebStorageNamespaceImpl*>(&other);
    return m_namespaceId == other_impl->m_namespaceId;
    return false;
}

}  // namespace content
