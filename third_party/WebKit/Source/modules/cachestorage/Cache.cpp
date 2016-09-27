// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/cachestorage/Cache.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8ThrowException.h"
#include "bindings/modules/v8/V8Response.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "modules/cachestorage/CacheStorageError.h"
#include "modules/fetch/BodyStreamBuffer.h"
#include "modules/fetch/FetchDataLoader.h"
#include "modules/fetch/GlobalFetch.h"
#include "modules/fetch/Request.h"
#include "modules/fetch/Response.h"
#include "public/platform/WebServiceWorkerCache.h"

namespace blink {

namespace {

// FIXME: Consider using CallbackPromiseAdapter.
class CacheMatchCallbacks : public WebServiceWorkerCache::CacheMatchCallbacks {
    WTF_MAKE_NONCOPYABLE(CacheMatchCallbacks);
public:
    CacheMatchCallbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
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
class CacheWithResponsesCallbacks : public WebServiceWorkerCache::CacheWithResponsesCallbacks {
    WTF_MAKE_NONCOPYABLE(CacheWithResponsesCallbacks);
public:
    CacheWithResponsesCallbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_resolver(resolver) { }

    void onSuccess(WebVector<WebServiceWorkerResponse>* webResponses) override
    {
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        HeapVector<Member<Response>> responses;
        for (size_t i = 0; i < webResponses->size(); ++i)
            responses.append(Response::create(m_resolver->scriptState()->executionContext(), (*webResponses)[i]));
        m_resolver->resolve(responses);
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

protected:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};

// FIXME: Consider using CallbackPromiseAdapter.
class CacheDeleteCallback : public WebServiceWorkerCache::CacheBatchCallbacks {
    WTF_MAKE_NONCOPYABLE(CacheDeleteCallback);
public:
    CacheDeleteCallback(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_resolver(resolver) { }

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
class CacheWithRequestsCallbacks : public WebServiceWorkerCache::CacheWithRequestsCallbacks {
    WTF_MAKE_NONCOPYABLE(CacheWithRequestsCallbacks);
public:
    CacheWithRequestsCallbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_resolver(resolver) { }

    void onSuccess(WebVector<WebServiceWorkerRequest>* webRequests) override
    {
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        HeapVector<Member<Request>> requests;
        for (size_t i = 0; i < webRequests->size(); ++i)
            requests.append(Request::create(m_resolver->scriptState()->executionContext(), (*webRequests)[i]));
        m_resolver->resolve(requests);
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

// This class provides Promise.all() for ScriptPromise.
// TODO(nhiroki): Move this somewhere else so that other components can reuse.
// TODO(nhiroki): Unfortunately, we have to go through V8 to wait for the fetch
// promise. It should be better to achieve this only within C++ world.
class CacheStoragePromiseAll final : public GarbageCollectedFinalized<CacheStoragePromiseAll> {
public:
    CacheStoragePromiseAll(Vector<ScriptPromise> promises, PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_numberOfPendingPromises(promises.size())
        , m_resolver(resolver)
    {
        m_values.resize(promises.size());
        for (size_t i = 0; i < promises.size(); ++i)
            promises[i].then(createFulfillFunction(i), createRejectFunction());
    }

    void onFulfilled(size_t index, const ScriptValue& value)
    {
        ASSERT(index < m_values.size());
        if (m_isSettled)
            return;
        m_values[index] = value;
        if (--m_numberOfPendingPromises > 0)
            return;
        m_isSettled = true;
        m_resolver->resolve(m_values);
    }

    void onRejected(const ScriptValue& value)
    {
        if (m_isSettled)
            return;
        m_isSettled = true;
        m_resolver->reject(value);
    }

    ScriptPromise promise() { return m_resolver->promise(); }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_resolver);
    }

private:
    class AdapterFunction : public ScriptFunction {
    public:
        enum ResolveType {
            Fulfilled,
            Rejected,
        };

        static v8::Local<v8::Function> create(ScriptState* scriptState, ResolveType resolveType, size_t index, CacheStoragePromiseAll* promiseAll)
        {
            AdapterFunction* self = new AdapterFunction(scriptState, resolveType, index, promiseAll);
            return self->bindToV8Function();
        }

        DEFINE_INLINE_VIRTUAL_TRACE()
        {
            visitor->trace(m_promiseAll);
            ScriptFunction::trace(visitor);
        }

    private:
        AdapterFunction(ScriptState* scriptState, ResolveType resolveType, size_t index, CacheStoragePromiseAll* promiseAll)
            : ScriptFunction(scriptState)
            , m_resolveType(resolveType)
            , m_index(index)
            , m_promiseAll(promiseAll) { }

        ScriptValue call(ScriptValue value) override
        {
            if (m_resolveType == Fulfilled)
                m_promiseAll->onFulfilled(m_index, value);
            else
                m_promiseAll->onRejected(value);
            return ScriptValue(scriptState(), m_promiseAll->promise().v8Value());
        }

        const ResolveType m_resolveType;
        const size_t m_index;
        Member<CacheStoragePromiseAll> m_promiseAll;
    };

    v8::Local<v8::Function> createFulfillFunction(size_t index)
    {
        return AdapterFunction::create(m_resolver->scriptState(), AdapterFunction::Fulfilled, index, this);
    }

    v8::Local<v8::Function> createRejectFunction()
    {
        return AdapterFunction::create(m_resolver->scriptState(), AdapterFunction::Rejected, 0, this);
    }

    size_t m_numberOfPendingPromises;
    RefPtrWillBeMember<ScriptPromiseResolver> m_resolver;
    bool m_isSettled = false;
    Vector<ScriptValue> m_values;
};

} // namespace

class Cache::FetchResolvedForAdd final : public ScriptFunction {
public:
    static v8::Local<v8::Function> create(ScriptState* scriptState, Cache* cache, const HeapVector<Member<Request>>& requests)
    {
        FetchResolvedForAdd* self = new FetchResolvedForAdd(scriptState, cache, requests);
        return self->bindToV8Function();
    }

    ScriptValue call(ScriptValue value) override
    {
        NonThrowableExceptionState exceptionState;
        HeapVector<Member<Response>> responses = toMemberNativeArray<Response, V8Response>(value.v8Value(), m_requests.size(), scriptState()->isolate(), exceptionState);
        ScriptPromise putPromise = m_cache->putImpl(scriptState(), m_requests, responses);
        return ScriptValue(scriptState(), putPromise.v8Value());
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_cache);
        visitor->trace(m_requests);
        ScriptFunction::trace(visitor);
    }

private:
    FetchResolvedForAdd(ScriptState* scriptState, Cache* cache, const HeapVector<Member<Request>>& requests)
        : ScriptFunction(scriptState)
        , m_cache(cache)
        , m_requests(requests)
    {
    }

    Member<Cache> m_cache;
    HeapVector<Member<Request>> m_requests;
};

class Cache::BarrierCallbackForPut final : public GarbageCollectedFinalized<BarrierCallbackForPut> {
public:
    BarrierCallbackForPut(int numberOfOperations, Cache* cache, PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_numberOfRemainingOperations(numberOfOperations)
        , m_cache(cache)
        , m_resolver(resolver)
    {
        ASSERT(0 < m_numberOfRemainingOperations);
        m_batchOperations.resize(numberOfOperations);
    }

    void onSuccess(size_t index, const WebServiceWorkerCache::BatchOperation& batchOperation)
    {
        ASSERT(index < m_batchOperations.size());
        if (m_completed)
            return;
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_batchOperations[index] = batchOperation;
        if (--m_numberOfRemainingOperations != 0)
            return;
        m_cache->webCache()->dispatchBatch(new CallbackPromiseAdapter<void, CacheStorageError>(m_resolver), m_batchOperations);
    }

    void onError(const String& errorMessage)
    {
        if (m_completed)
            return;
        m_completed = true;
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        ScriptState* state = m_resolver->scriptState();
        ScriptState::Scope scope(state);
        m_resolver->reject(V8ThrowException::createTypeError(state->isolate(), errorMessage));
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_cache);
        visitor->trace(m_resolver);
    }

private:
    bool m_completed = false;
    int m_numberOfRemainingOperations;
    Member<Cache> m_cache;
    RefPtrWillBeMember<ScriptPromiseResolver> m_resolver;
    Vector<WebServiceWorkerCache::BatchOperation> m_batchOperations;
};

class Cache::BlobHandleCallbackForPut final : public GarbageCollectedFinalized<BlobHandleCallbackForPut>, public FetchDataLoader::Client {
    USING_GARBAGE_COLLECTED_MIXIN(BlobHandleCallbackForPut);
public:
    BlobHandleCallbackForPut(size_t index, BarrierCallbackForPut* barrierCallback, Request* request, Response* response)
        : m_index(index)
        , m_barrierCallback(barrierCallback)
    {
        request->populateWebServiceWorkerRequest(m_webRequest);
        response->populateWebServiceWorkerResponse(m_webResponse);
    }
    ~BlobHandleCallbackForPut() override { }

    void didFetchDataLoadedBlobHandle(PassRefPtr<BlobDataHandle> handle) override
    {
        WebServiceWorkerCache::BatchOperation batchOperation;
        batchOperation.operationType = WebServiceWorkerCache::OperationTypePut;
        batchOperation.request = m_webRequest;
        batchOperation.response = m_webResponse;
        batchOperation.response.setBlobDataHandle(handle);
        m_barrierCallback->onSuccess(m_index, batchOperation);
    }

    void didFetchDataLoadFailed() override
    {
        m_barrierCallback->onError("network error");
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_barrierCallback);
        FetchDataLoader::Client::trace(visitor);
    }

private:
    const size_t m_index;
    Member<BarrierCallbackForPut> m_barrierCallback;

    WebServiceWorkerRequest m_webRequest;
    WebServiceWorkerResponse m_webResponse;
};

Cache* Cache::create(WeakPtr<GlobalFetch::ScopedFetcher> fetcher, PassOwnPtr<WebServiceWorkerCache> webCache)
{
    return new Cache(fetcher, webCache);
}

ScriptPromise Cache::match(ScriptState* scriptState, const RequestInfo& request, const CacheQueryOptions& options, ExceptionState& exceptionState)
{
    ASSERT(!request.isNull());
    if (request.isRequest())
        return matchImpl(scriptState, request.getAsRequest(), options);
    Request* newRequest = Request::create(scriptState, request.getAsUSVString(), exceptionState);
    if (exceptionState.hadException())
        return ScriptPromise();
    return matchImpl(scriptState, newRequest, options);
}

ScriptPromise Cache::matchAll(ScriptState* scriptState, const RequestInfo& request, const CacheQueryOptions& options, ExceptionState& exceptionState)
{
    ASSERT(!request.isNull());
    if (request.isRequest())
        return matchAllImpl(scriptState, request.getAsRequest(), options);
    Request* newRequest = Request::create(scriptState, request.getAsUSVString(), exceptionState);
    if (exceptionState.hadException())
        return ScriptPromise();
    return matchAllImpl(scriptState, newRequest, options);
}

ScriptPromise Cache::add(ScriptState* scriptState, const RequestInfo& request, ExceptionState& exceptionState)
{
    ASSERT(!request.isNull());
    HeapVector<Member<Request>> requests;
    if (request.isRequest()) {
        requests.append(request.getAsRequest());
    } else {
        requests.append(Request::create(scriptState, request.getAsUSVString(), exceptionState));
        if (exceptionState.hadException())
            return ScriptPromise();
    }

    return addAllImpl(scriptState, requests, exceptionState);
}

ScriptPromise Cache::addAll(ScriptState* scriptState, const HeapVector<RequestInfo>& rawRequests, ExceptionState& exceptionState)
{
    HeapVector<Member<Request>> requests;
    for (RequestInfo request : rawRequests) {
        if (request.isRequest()) {
            requests.append(request.getAsRequest());
        } else {
            requests.append(Request::create(scriptState, request.getAsUSVString(), exceptionState));
            if (exceptionState.hadException())
                return ScriptPromise();
        }
    }

    return addAllImpl(scriptState, requests, exceptionState);
}

ScriptPromise Cache::deleteFunction(ScriptState* scriptState, const RequestInfo& request, const CacheQueryOptions& options, ExceptionState& exceptionState)
{
    ASSERT(!request.isNull());
    if (request.isRequest())
        return deleteImpl(scriptState, request.getAsRequest(), options);
    Request* newRequest = Request::create(scriptState, request.getAsUSVString(), exceptionState);
    if (exceptionState.hadException())
        return ScriptPromise();
    return deleteImpl(scriptState, newRequest, options);
}

ScriptPromise Cache::put(ScriptState* scriptState, const RequestInfo& request, Response* response, ExceptionState& exceptionState)
{
    ASSERT(!request.isNull());
    if (request.isRequest())
        return putImpl(scriptState, HeapVector<Member<Request>>(1, request.getAsRequest()), HeapVector<Member<Response>>(1, response));
    Request* newRequest = Request::create(scriptState, request.getAsUSVString(), exceptionState);
    if (exceptionState.hadException())
        return ScriptPromise();
    return putImpl(scriptState, HeapVector<Member<Request>>(1, newRequest), HeapVector<Member<Response>>(1, response));
}

ScriptPromise Cache::keys(ScriptState* scriptState, ExceptionState&)
{
    return keysImpl(scriptState);
}

ScriptPromise Cache::keys(ScriptState* scriptState, const RequestInfo& request, const CacheQueryOptions& options, ExceptionState& exceptionState)
{
    ASSERT(!request.isNull());
    if (request.isRequest())
        return keysImpl(scriptState, request.getAsRequest(), options);
    Request* newRequest = Request::create(scriptState, request.getAsUSVString(), exceptionState);
    if (exceptionState.hadException())
        return ScriptPromise();
    return keysImpl(scriptState, newRequest, options);
}

// static
WebServiceWorkerCache::QueryParams Cache::toWebQueryParams(const CacheQueryOptions& options)
{
    WebServiceWorkerCache::QueryParams webQueryParams;
    webQueryParams.ignoreSearch = options.ignoreSearch();
    webQueryParams.ignoreMethod = options.ignoreMethod();
    webQueryParams.ignoreVary = options.ignoreVary();
    webQueryParams.cacheName = options.cacheName();
    return webQueryParams;
}

Cache::Cache(WeakPtr<GlobalFetch::ScopedFetcher> fetcher, PassOwnPtr<WebServiceWorkerCache> webCache)
    : m_scopedFetcher(fetcher)
    , m_webCache(webCache) { }

ScriptPromise Cache::matchImpl(ScriptState* scriptState, const Request* request, const CacheQueryOptions& options)
{
    WebServiceWorkerRequest webRequest;
    request->populateWebServiceWorkerRequest(webRequest);

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    const ScriptPromise promise = resolver->promise();
    m_webCache->dispatchMatch(new CacheMatchCallbacks(resolver), webRequest, toWebQueryParams(options));
    return promise;
}

ScriptPromise Cache::matchAllImpl(ScriptState* scriptState, const Request* request, const CacheQueryOptions& options)
{
    WebServiceWorkerRequest webRequest;
    request->populateWebServiceWorkerRequest(webRequest);

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    const ScriptPromise promise = resolver->promise();
    m_webCache->dispatchMatchAll(new CacheWithResponsesCallbacks(resolver), webRequest, toWebQueryParams(options));
    return promise;
}

ScriptPromise Cache::addAllImpl(ScriptState* scriptState, const HeapVector<Member<Request>>& requests, ExceptionState& exceptionState)
{
    Vector<RequestInfo> requestInfos;
    requestInfos.resize(requests.size());
    Vector<ScriptPromise> promises;
    promises.resize(requests.size());
    for (size_t i = 0; i < requests.size(); ++i) {
        if (!requests[i]->url().protocolIsInHTTPFamily())
            return ScriptPromise::reject(scriptState, V8ThrowException::createTypeError(scriptState->isolate(), "Add/AddAll does not support schemes other than \"http\" or \"https\""));
        if (requests[i]->method() != "GET")
            return ScriptPromise::reject(scriptState, V8ThrowException::createTypeError(scriptState->isolate(), "Add/AddAll only supports the GET request method."));
        requestInfos[i].setRequest(requests[i]);

        promises[i] = m_scopedFetcher->fetch(scriptState, requestInfos[i], Dictionary(), exceptionState);
    }

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    CacheStoragePromiseAll* promiseAll = new CacheStoragePromiseAll(promises, resolver.get());
    return promiseAll->promise().then(FetchResolvedForAdd::create(scriptState, this, requests));
}

ScriptPromise Cache::deleteImpl(ScriptState* scriptState, const Request* request, const CacheQueryOptions& options)
{
    WebVector<WebServiceWorkerCache::BatchOperation> batchOperations(size_t(1));
    batchOperations[0].operationType = WebServiceWorkerCache::OperationTypeDelete;
    request->populateWebServiceWorkerRequest(batchOperations[0].request);
    batchOperations[0].matchParams = toWebQueryParams(options);

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    const ScriptPromise promise = resolver->promise();
    m_webCache->dispatchBatch(new CacheDeleteCallback(resolver), batchOperations);
    return promise;
}

ScriptPromise Cache::putImpl(ScriptState* scriptState, const HeapVector<Member<Request>>& requests, const HeapVector<Member<Response>>& responses)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    const ScriptPromise promise = resolver->promise();
    BarrierCallbackForPut* barrierCallback = new BarrierCallbackForPut(requests.size(), this, resolver.get());

    for (size_t i = 0; i < requests.size(); ++i) {
        KURL url(KURL(), requests[i]->url());
        if (!url.protocolIsInHTTPFamily()) {
            barrierCallback->onError("Request scheme '" + url.protocol() + "' is unsupported");
            return promise;
        }
        if (requests[i]->method() != "GET") {
            barrierCallback->onError("Request method '" + requests[i]->method() + "' is unsupported");
            return promise;
        }
        if (requests[i]->hasBody() && requests[i]->bodyUsed()) {
            barrierCallback->onError("Request body is already used");
            return promise;
        }
        if (responses[i]->hasBody() && responses[i]->bodyUsed()) {
            barrierCallback->onError("Response body is already used");
            return promise;
        }

        if (requests[i]->hasBody())
            requests[i]->lockBody(Body::PassBody);
        if (responses[i]->hasBody())
            responses[i]->lockBody(Body::PassBody);

        if (OwnPtr<DrainingBodyStreamBuffer> buffer = responses[i]->createInternalDrainingStream()) {
            // If the response has body, read the all data and create
            // the blob handle and dispatch the put batch asynchronously.
            FetchDataLoader* loader = FetchDataLoader::createLoaderAsBlobHandle(responses[i]->internalMIMEType());
            buffer->startLoading(scriptState->executionContext(), loader, new BlobHandleCallbackForPut(i, barrierCallback, requests[i], responses[i]));
            continue;
        }

        WebServiceWorkerCache::BatchOperation batchOperation;
        batchOperation.operationType = WebServiceWorkerCache::OperationTypePut;
        requests[i]->populateWebServiceWorkerRequest(batchOperation.request);
        responses[i]->populateWebServiceWorkerResponse(batchOperation.response);
        barrierCallback->onSuccess(i, batchOperation);
    }

    return promise;
}

ScriptPromise Cache::keysImpl(ScriptState* scriptState)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    const ScriptPromise promise = resolver->promise();
    m_webCache->dispatchKeys(new CacheWithRequestsCallbacks(resolver), 0, WebServiceWorkerCache::QueryParams());
    return promise;
}

ScriptPromise Cache::keysImpl(ScriptState* scriptState, const Request* request, const CacheQueryOptions& options)
{
    WebServiceWorkerRequest webRequest;
    request->populateWebServiceWorkerRequest(webRequest);

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    const ScriptPromise promise = resolver->promise();
    m_webCache->dispatchKeys(new CacheWithRequestsCallbacks(resolver), 0, toWebQueryParams(options));
    return promise;
}

WebServiceWorkerCache* Cache::webCache() const
{
    return m_webCache.get();
}

} // namespace blink
