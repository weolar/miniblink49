// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Cache_h
#define Cache_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/modules/v8/UnionTypesModules.h"
#include "modules/ModulesExport.h"
#include "modules/cachestorage/CacheQueryOptions.h"
#include "modules/fetch/GlobalFetch.h"
#include "public/platform/WebServiceWorkerCache.h"
#include "public/platform/WebServiceWorkerCacheError.h"
#include "wtf/Forward.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ExceptionState;
class Response;
class Request;
class ScriptState;

typedef RequestOrUSVString RequestInfo;

class MODULES_EXPORT Cache final : public GarbageCollectedFinalized<Cache>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
    WTF_MAKE_NONCOPYABLE(Cache);
public:
    static Cache* create(WeakPtr<GlobalFetch::ScopedFetcher>, PassOwnPtr<WebServiceWorkerCache>);

    // From Cache.idl:
    ScriptPromise match(ScriptState*, const RequestInfo&, const CacheQueryOptions&, ExceptionState&);
    ScriptPromise matchAll(ScriptState*, const RequestInfo&, const CacheQueryOptions&, ExceptionState&);
    ScriptPromise add(ScriptState*, const RequestInfo&, ExceptionState&);
    ScriptPromise addAll(ScriptState*, const HeapVector<RequestInfo>&, ExceptionState&);
    ScriptPromise deleteFunction(ScriptState*, const RequestInfo&, const CacheQueryOptions&, ExceptionState&);
    ScriptPromise put(ScriptState*, const RequestInfo&, Response*, ExceptionState&);
    ScriptPromise keys(ScriptState*, ExceptionState&);
    ScriptPromise keys(ScriptState*, const RequestInfo&, const CacheQueryOptions&, ExceptionState&);

    static WebServiceWorkerCache::QueryParams toWebQueryParams(const CacheQueryOptions&);

    DEFINE_INLINE_TRACE() { }

private:
    class BarrierCallbackForPut;
    class BlobHandleCallbackForPut;
    class FetchResolvedForAdd;
    friend class FetchResolvedForAdd;
    Cache(WeakPtr<GlobalFetch::ScopedFetcher>, PassOwnPtr<WebServiceWorkerCache>);

    ScriptPromise matchImpl(ScriptState*, const Request*, const CacheQueryOptions&);
    ScriptPromise matchAllImpl(ScriptState*, const Request*, const CacheQueryOptions&);
    ScriptPromise addAllImpl(ScriptState*, const HeapVector<Member<Request>>&, ExceptionState&);
    ScriptPromise deleteImpl(ScriptState*, const Request*, const CacheQueryOptions&);
    ScriptPromise putImpl(ScriptState*, const HeapVector<Member<Request>>&, const HeapVector<Member<Response>>&);
    ScriptPromise keysImpl(ScriptState*);
    ScriptPromise keysImpl(ScriptState*, const Request*, const CacheQueryOptions&);

    WebServiceWorkerCache* webCache() const;

    WeakPtr<GlobalFetch::ScopedFetcher> m_scopedFetcher;
    OwnPtr<WebServiceWorkerCache> m_webCache;
};

} // namespace blink

#endif // Cache_h
