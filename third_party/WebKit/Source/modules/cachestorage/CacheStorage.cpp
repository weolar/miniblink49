// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/cachestorage/CacheStorage.h"

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "modules/cachestorage/CacheStorageError.h"
#include "modules/fetch/Request.h"
#include "modules/fetch/Response.h"
#include "public/platform/WebServiceWorkerCacheError.h"
#include "public/platform/WebServiceWorkerCacheStorage.h"

namespace blink {

namespace {

DOMException* createNoImplementationException()
{
    return DOMException::create(NotSupportedError, "No CacheStorage implementation provided.");
}

}

// FIXME: Consider using CallbackPromiseAdapter.
class CacheStorage::Callbacks final : public WebServiceWorkerCacheStorage::CacheStorageCallbacks {
    WTF_MAKE_NONCOPYABLE(Callbacks);
public:
    explicit Callbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_resolver(resolver) { }
    ~Callbacks() override { }

    void onSuccess() override
    {
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->resolve(true);
        m_resolver.clear();
    }

    // Ownership of |rawReason| must be passed.
    void onError(WebServiceWorkerCacheError* rawReason) override
    {
        OwnPtr<WebServiceWorkerCacheError> reason = adoptPtr(rawReason);
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        if (*reason == WebServiceWorkerCacheErrorNotFound)
            m_resolver->resolve(false);
        else
            m_resolver->reject(CacheStorageError::createException(*reason));
        m_resolver.clear();
    }

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};

// FIXME: Consider using CallbackPromiseAdapter.
class CacheStorage::WithCacheCallbacks final : public WebServiceWorkerCacheStorage::CacheStorageWithCacheCallbacks {
    WTF_MAKE_NONCOPYABLE(WithCacheCallbacks);
public:
    WithCacheCallbacks(const String& cacheName, CacheStorage* cacheStorage, PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_cacheName(cacheName), m_cacheStorage(cacheStorage), m_resolver(resolver) { }
    ~WithCacheCallbacks() override { }

    // Ownership of |rawWebCache| must be passed.
    void onSuccess(WebServiceWorkerCache* rawWebCache) override
    {
        OwnPtr<WebServiceWorkerCache> webCache = adoptPtr(rawWebCache);
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        // FIXME: Remove this once content's WebServiceWorkerCache implementation has landed.
        if (!webCache) {
            m_resolver->reject("not implemented");
            return;
        }
        Cache* cache = Cache::create(m_cacheStorage->m_scopedFetcher, webCache.release());
        m_cacheStorage->m_nameToCacheMap.set(m_cacheName, cache);
        m_resolver->resolve(cache);
        m_resolver.clear();
    }

    // Ownership of |rawReason| must be passed.
    void onError(WebServiceWorkerCacheError* rawReason) override
    {
        OwnPtr<WebServiceWorkerCacheError> reason = adoptPtr(rawReason);
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        if (*reason == WebServiceWorkerCacheErrorNotFound)
            m_resolver->resolve();
        else
            m_resolver->reject(CacheStorageError::createException(*reason));
        m_resolver.clear();
    }

private:
    String m_cacheName;
    Persistent<CacheStorage> m_cacheStorage;
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};

// FIXME: Consider using CallbackPromiseAdapter.
class CacheStorage::MatchCallbacks : public WebServiceWorkerCacheStorage::CacheStorageMatchCallbacks {
    WTF_MAKE_NONCOPYABLE(MatchCallbacks);
public:
    MatchCallbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_resolver(resolver) { }

    void onSuccess(WebServiceWorkerResponse* webResponse) override
    {
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->resolve(Response::create(m_resolver->scriptState()->executionContext(), *webResponse));
        m_resolver.clear();
    }

    // Ownership of |rawReason| must be passed.
    void onError(WebServiceWorkerCacheError* rawReason) override
    {
        OwnPtr<WebServiceWorkerCacheError> reason = adoptPtr(rawReason);
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        if (*reason == WebServiceWorkerCacheErrorNotFound)
            m_resolver->resolve();
        else
            m_resolver->reject(CacheStorageError::createException(*reason));
        m_resolver.clear();
    }

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};


// FIXME: Consider using CallbackPromiseAdapter.
class CacheStorage::DeleteCallbacks final : public WebServiceWorkerCacheStorage::CacheStorageCallbacks {
    WTF_MAKE_NONCOPYABLE(DeleteCallbacks);
public:
    DeleteCallbacks(const String& cacheName, CacheStorage* cacheStorage, PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_cacheName(cacheName), m_cacheStorage(cacheStorage), m_resolver(resolver) { }
    ~DeleteCallbacks() override { }

    void onSuccess() override
    {
        m_cacheStorage->m_nameToCacheMap.remove(m_cacheName);
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->resolve(true);
        m_resolver.clear();
    }

    // Ownership of |rawReason| must be passed.
    void onError(WebServiceWorkerCacheError* rawReason) override
    {
        OwnPtr<WebServiceWorkerCacheError> reason = adoptPtr(rawReason);
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        if (*reason == WebServiceWorkerCacheErrorNotFound)
            m_resolver->resolve(false);
        else
            m_resolver->reject(CacheStorageError::createException(*reason));
        m_resolver.clear();
    }

private:
    String m_cacheName;
    Persistent<CacheStorage> m_cacheStorage;
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};

// FIXME: Consider using CallbackPromiseAdapter.
class CacheStorage::KeysCallbacks final : public WebServiceWorkerCacheStorage::CacheStorageKeysCallbacks {
    WTF_MAKE_NONCOPYABLE(KeysCallbacks);
public:
    explicit KeysCallbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_resolver(resolver) { }
    ~KeysCallbacks() override { }

    void onSuccess(WebVector<WebString>* keys) override
    {
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        Vector<String> wtfKeys;
        for (size_t i = 0; i < keys->size(); ++i)
            wtfKeys.append((*keys)[i]);
        m_resolver->resolve(wtfKeys);
        m_resolver.clear();
    }

    // Ownership of |rawReason| must be passed.
    void onError(WebServiceWorkerCacheError* rawReason) override
    {
        OwnPtr<WebServiceWorkerCacheError> reason = adoptPtr(rawReason);
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->reject(CacheStorageError::createException(*reason));
        m_resolver.clear();
    }

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};

CacheStorage* CacheStorage::create(WeakPtr<GlobalFetch::ScopedFetcher> fetcher, WebServiceWorkerCacheStorage* webCacheStorage)
{
    return new CacheStorage(fetcher, adoptPtr(webCacheStorage));
}

ScriptPromise CacheStorage::open(ScriptState* scriptState, const String& cacheName)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    const ScriptPromise promise = resolver->promise();

    if (m_nameToCacheMap.contains(cacheName)) {
        Cache* cache = m_nameToCacheMap.find(cacheName)->value;
        resolver->resolve(cache);
        return promise;
    }

    if (m_webCacheStorage)
        m_webCacheStorage->dispatchOpen(new WithCacheCallbacks(cacheName, this, resolver), cacheName);
    else
        resolver->reject(createNoImplementationException());

    return promise;
}

ScriptPromise CacheStorage::has(ScriptState* scriptState, const String& cacheName)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    const ScriptPromise promise = resolver->promise();

    if (m_nameToCacheMap.contains(cacheName)) {
        resolver->resolve(true);
        return promise;
    }

    if (m_webCacheStorage)
        m_webCacheStorage->dispatchHas(new Callbacks(resolver), cacheName);
    else
        resolver->reject(createNoImplementationException());

    return promise;
}

ScriptPromise CacheStorage::deleteFunction(ScriptState* scriptState, const String& cacheName)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    const ScriptPromise promise = resolver->promise();

    if (m_webCacheStorage)
        m_webCacheStorage->dispatchDelete(new DeleteCallbacks(cacheName, this, resolver), cacheName);
    else
        resolver->reject(createNoImplementationException());

    return promise;
}

ScriptPromise CacheStorage::keys(ScriptState* scriptState)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    const ScriptPromise promise = resolver->promise();

    if (m_webCacheStorage)
        m_webCacheStorage->dispatchKeys(new KeysCallbacks(resolver));
    else
        resolver->reject(createNoImplementationException());

    return promise;
}

ScriptPromise CacheStorage::match(ScriptState* scriptState, const RequestInfo& request, const CacheQueryOptions& options, ExceptionState& exceptionState)
{
    ASSERT(!request.isNull());

    if (request.isRequest())
        return matchImpl(scriptState, request.getAsRequest(), options);
    Request* newRequest = Request::create(scriptState, request.getAsUSVString(), exceptionState);
    if (exceptionState.hadException())
        return ScriptPromise();
    return matchImpl(scriptState, newRequest, options);
}

ScriptPromise CacheStorage::matchImpl(ScriptState* scriptState, const Request* request, const CacheQueryOptions& options)
{
    WebServiceWorkerRequest webRequest;
    request->populateWebServiceWorkerRequest(webRequest);

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    const ScriptPromise promise = resolver->promise();

    if (m_webCacheStorage)
        m_webCacheStorage->dispatchMatch(new MatchCallbacks(resolver), webRequest, Cache::toWebQueryParams(options));
    else
        resolver->reject(createNoImplementationException());

    return promise;
}

CacheStorage::CacheStorage(WeakPtr<GlobalFetch::ScopedFetcher> fetcher, PassOwnPtr<WebServiceWorkerCacheStorage> webCacheStorage)
    : m_scopedFetcher(fetcher)
    , m_webCacheStorage(webCacheStorage)
{
}

CacheStorage::~CacheStorage()
{
}

void CacheStorage::dispose()
{
    m_webCacheStorage.clear();
}

DEFINE_TRACE(CacheStorage)
{
    visitor->trace(m_nameToCacheMap);
}

} // namespace blink
