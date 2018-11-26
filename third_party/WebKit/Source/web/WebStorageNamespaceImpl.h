// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_DOM_STORAGE_WEBSTORAGENAMESPACE_IMPL_H_
#define CONTENT_RENDERER_DOM_STORAGE_WEBSTORAGENAMESPACE_IMPL_H_

#include "third_party/WebKit/public/platform/WebStorageNamespace.h"
#include "wtf/HashMap.h"
#include "wtf/text/WTFString.h"
#include "wtf/text/StringHash.h"

namespace blink {

// The quota for each storage area.
// This value is enforced in renderer processes and the browser process.
const size_t kPerStorageAreaQuota = 10 * 1024 * 1024;

// In the browser process we allow some overage to
// accomodate concurrent writes from different renderers
// that were allowed because the limit imposed in the renderer
// wasn't exceeded.
const size_t kPerStorageAreaOverQuotaAllowance = 100 * 1024;

// Value to indicate the localstorage namespace vs non-zero
// values for sessionstorage namespaces.
const int64 kLocalStorageNamespaceId = 0;

const int64 kInvalidSessionStorageNamespaceId = kLocalStorageNamespaceId;

// Start purging memory if the number of in-memory areas exceeds this.
const int64 kMaxInMemoryStorageAreas = 100;

typedef HashMap<String, HashMap<String, String>* > DOMStorageMap;

class WebStorageNamespaceImpl : public blink::WebStorageNamespace {
public:
    // The default constructor creates a local storage namespace, the second
    // constructor should be used for session storage namepaces.
    WebStorageNamespaceImpl(int64 namespaceId, DOMStorageMap* cachedArea, bool isLocal);
    virtual ~WebStorageNamespaceImpl();

    // See WebStorageNamespace.h for documentation on these functions.
    virtual blink::WebStorageArea* createStorageArea(const blink::WebString& origin) override;
    virtual blink::WebStorageNamespace* copy();
    virtual bool isSameNamespace(const WebStorageNamespace&) const override;

private:
    DOMStorageMap* m_cachedArea;
    int64 m_namespaceId;
    bool m_isLocal;
};

}  // namespace content

#endif  // CONTENT_RENDERER_DOM_STORAGE_WEBSTORAGENAMESPACE_IMPL_H_
