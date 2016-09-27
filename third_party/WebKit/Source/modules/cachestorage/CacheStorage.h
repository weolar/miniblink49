// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CacheStorage_h
#define CacheStorage_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/cachestorage/Cache.h"
#include "modules/cachestorage/CacheQueryOptions.h"
#include "modules/fetch/GlobalFetch.h"
#include "public/platform/WebServiceWorkerCacheStorage.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/Noncopyable.h"

namespace blink {

class Cache;
class WebServiceWorkerCacheStorage;

class CacheStorage final : public GarbageCollectedFinalized<CacheStorage>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
    WTF_MAKE_NONCOPYABLE(CacheStorage);
public:
    static CacheStorage* create(WeakPtr<GlobalFetch::ScopedFetcher>, WebServiceWorkerCacheStorage*);
    ~CacheStorage();
    void dispose();

    ScriptPromise open(ScriptState*, const String& cacheName);
    ScriptPromise has(ScriptState*, const String& cacheName);
    ScriptPromise deleteFunction(ScriptState*, const String& cacheName);
    ScriptPromise keys(ScriptState*);
    ScriptPromise match(ScriptState*, const RequestInfo&, const CacheQueryOptions&, ExceptionState&);

    DECLARE_TRACE();

private:
    class Callbacks;
    class WithCacheCallbacks;
    class DeleteCallbacks;
    class KeysCallbacks;
    class MatchCallbacks;

    friend class WithCacheCallbacks;
    friend class DeleteCallbacks;

    CacheStorage(WeakPtr<GlobalFetch::ScopedFetcher>, PassOwnPtr<WebServiceWorkerCacheStorage>);
    ScriptPromise matchImpl(ScriptState*, const Request*, const CacheQueryOptions&);

    WeakPtr<GlobalFetch::ScopedFetcher> m_scopedFetcher;
    OwnPtr<WebServiceWorkerCacheStorage> m_webCacheStorage;
    HeapHashMap<String, Member<Cache>> m_nameToCacheMap;
};

} // namespace blink

#endif // CacheStorage_h
