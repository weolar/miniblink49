// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebServiceWorkerCacheStorage_h
#define WebServiceWorkerCacheStorage_h

#include "WebCommon.h"
#include "public/platform/WebCallbacks.h"
#include "public/platform/WebServiceWorkerCache.h"
#include "public/platform/WebServiceWorkerCacheError.h"
#include "public/platform/WebString.h"
#include "public/platform/WebVector.h"

namespace blink {

class WebServiceWorkerCache;

// An interface to the CacheStorage API, implemented by the embedder and passed in to Blink. Blink's implementation
// of the ServiceWorker spec will call these methods to create/open caches, and expect callbacks from the embedder
// after operations complete.
class WebServiceWorkerCacheStorage {
public:
    typedef WebCallbacks<void, WebServiceWorkerCacheError> CacheStorageCallbacks;
    typedef WebCallbacks<WebServiceWorkerCache, WebServiceWorkerCacheError> CacheStorageWithCacheCallbacks;
    typedef WebCallbacks<WebVector<WebString>, WebServiceWorkerCacheError> CacheStorageKeysCallbacks;
    typedef WebCallbacks<WebServiceWorkerResponse, WebServiceWorkerCacheError> CacheStorageMatchCallbacks;

    virtual ~WebServiceWorkerCacheStorage() { }

    // Ownership of the CacheStorage*Callbacks methods passes to the WebServiceWorkerCacheStorage instance, which
    // will delete it after calling onSuccess or onFailure.

    // dispatchOpen() can return a WebServiceWorkerCache object. These
    // objects are owned by Blink and should be destroyed when they are no
    // longer needed.
    virtual void dispatchHas(CacheStorageCallbacks*, const WebString& cacheName) = 0;
    virtual void dispatchOpen(CacheStorageWithCacheCallbacks*, const WebString& cacheName) = 0;
    virtual void dispatchDelete(CacheStorageCallbacks*, const WebString& cacheName) = 0;
    virtual void dispatchKeys(CacheStorageKeysCallbacks*) = 0;
    virtual void dispatchMatch(CacheStorageMatchCallbacks*, const WebServiceWorkerRequest&, const WebServiceWorkerCache::QueryParams&) = 0;
};

} // namespace blink

#endif // WebServiceWorkerCacheStorage_h
