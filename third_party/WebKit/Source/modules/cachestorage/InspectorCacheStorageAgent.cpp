// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/cachestorage/InspectorCacheStorageAgent.h"

#include "core/InspectorBackendDispatcher.h"
#include "core/InspectorTypeBuilder.h"
#include "platform/JSONValues.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/DatabaseIdentifier.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/Platform.h"
#include "public/platform/WebServiceWorkerCache.h"
#include "public/platform/WebServiceWorkerCacheError.h"
#include "public/platform/WebServiceWorkerCacheStorage.h"
#include "public/platform/WebServiceWorkerRequest.h"
#include "public/platform/WebServiceWorkerResponse.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"
#include "public/platform/WebVector.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"
#include "wtf/text/StringBuilder.h"

#include <algorithm>

using blink::TypeBuilder::Array;
using blink::TypeBuilder::CacheStorage::Cache;
using blink::TypeBuilder::CacheStorage::DataEntry;

typedef blink::InspectorBackendDispatcher::CacheStorageCommandHandler::DeleteCacheCallback DeleteCacheCallback;
typedef blink::InspectorBackendDispatcher::CacheStorageCommandHandler::DeleteEntryCallback DeleteEntryCallback;
typedef blink::InspectorBackendDispatcher::CacheStorageCommandHandler::RequestCacheNamesCallback RequestCacheNamesCallback;
typedef blink::InspectorBackendDispatcher::CacheStorageCommandHandler::RequestEntriesCallback RequestEntriesCallback;
typedef blink::InspectorBackendDispatcher::CallbackBase RequestCallback;
typedef blink::WebServiceWorkerCache::BatchOperation BatchOperation;

namespace blink {

namespace {

String buildCacheId(const String& securityOrigin, const String& cacheName)
{
    String id(securityOrigin);
    id.append("|");
    id.append(cacheName);
    return id;
}

bool parseCacheId(ErrorString* errorString, const String& id, String* securityOrigin, String* cacheName)
{
    size_t pipe = id.find('|');
    if (pipe == WTF::kNotFound) {
        *errorString = "Invalid cache id.";
        return false;
    }
    *securityOrigin = id.substring(0, pipe);
    *cacheName = id.substring(pipe + 1);
    return true;
}

PassOwnPtr<WebServiceWorkerCacheStorage> assertCacheStorage(ErrorString* errorString, const String& securityOrigin)
{
    RefPtr<SecurityOrigin> secOrigin = SecurityOrigin::createFromString(securityOrigin);
    String identifier = createDatabaseIdentifierFromSecurityOrigin(secOrigin.get());
    OwnPtr<WebServiceWorkerCacheStorage> cache = adoptPtr(Platform::current()->cacheStorage(identifier));
    if (!cache)
        *errorString = "Could not find cache storage.";
    return cache.release();
}

PassOwnPtr<WebServiceWorkerCacheStorage> assertCacheStorageAndNameForId(ErrorString* errorString, const String& cacheId, String* cacheName)
{
    String securityOrigin;
    if (!parseCacheId(errorString, cacheId, &securityOrigin, cacheName)) {
        return nullptr;
    }
    return assertCacheStorage(errorString, securityOrigin);
}

CString serviceWorkerCacheErrorString(WebServiceWorkerCacheError* error)
{
    switch (*error) {
    case WebServiceWorkerCacheErrorNotImplemented:
        return CString("not implemented.");
        break;
    case WebServiceWorkerCacheErrorNotFound:
        return CString("not found.");
        break;
    case WebServiceWorkerCacheErrorExists:
        return CString("cache already exists.");
        break;
    default:
        return CString("unknown error.");
        break;
    }
}

class RequestCacheNames
    : public WebServiceWorkerCacheStorage::CacheStorageKeysCallbacks {
    WTF_MAKE_NONCOPYABLE(RequestCacheNames);

public:
    RequestCacheNames(const String& securityOrigin, PassRefPtrWillBeRawPtr<RequestCacheNamesCallback> callback)
        : m_securityOrigin(securityOrigin)
        , m_callback(callback)
    {
    }

    ~RequestCacheNames() override { }

    void onSuccess(WebVector<WebString>* caches)
    {
        RefPtr<Array<Cache>> array = Array<Cache>::create();
        for (size_t i = 0; i < caches->size(); i++) {
            String name = String((*caches)[i]);
            RefPtr<Cache> entry = Cache::create()
                .setSecurityOrigin(m_securityOrigin)
                .setCacheName(name)
                .setCacheId(buildCacheId(m_securityOrigin, name));
            array->addItem(entry);
        }
        m_callback->sendSuccess(array);
    }

    void onError(WebServiceWorkerCacheError* error)
    {
        m_callback->sendFailure(String::format("Error requesting cache names: %s", serviceWorkerCacheErrorString(error).data()));
    }

private:
    String m_securityOrigin;
    RefPtrWillBePersistent<RequestCacheNamesCallback> m_callback;
};

struct DataRequestParams {
    String cacheName;
    int skipCount;
    int pageSize;
};

struct RequestResponse {
    RequestResponse() { }
    RequestResponse(const String& request, const String& response)
        : request(request)
        , response(response)
    {
    }
    String request;
    String response;
};

class ResponsesAccumulator : public RefCounted<ResponsesAccumulator> {
    WTF_MAKE_NONCOPYABLE(ResponsesAccumulator);

public:
    ResponsesAccumulator(int numResponses, const DataRequestParams& params, PassRefPtrWillBeRawPtr<RequestEntriesCallback> callback)
        : m_params(params)
        , m_numResponsesLeft(numResponses)
        , m_responses(static_cast<size_t>(numResponses))
        , m_callback(callback)
    {
    }

    void addRequestResponsePair(const WebServiceWorkerRequest& request, const WebServiceWorkerResponse& response)
    {
        ASSERT(m_numResponsesLeft > 0);
        RequestResponse& requestResponse = m_responses.at(m_responses.size() - m_numResponsesLeft);
        requestResponse.request = request.url().string();
        requestResponse.response = response.statusText();

        if (--m_numResponsesLeft != 0)
            return;

        std::sort(m_responses.begin(), m_responses.end(),
            [](const RequestResponse& a, const RequestResponse& b)
            {
                return WTF::codePointCompareLessThan(a.request, b.request);
            });
        if (m_params.skipCount > 0)
            m_responses.remove(0, m_params.skipCount);
        bool hasMore = false;
        if (static_cast<size_t>(m_params.pageSize) < m_responses.size()) {
            m_responses.remove(m_params.pageSize, m_responses.size() - m_params.pageSize);
            hasMore = true;
        }
        RefPtr<Array<DataEntry>> array = Array<DataEntry>::create();
        for (const auto& requestResponse : m_responses) {
            RefPtr<DataEntry> entry = DataEntry::create()
                .setRequest(requestResponse.request)
                .setResponse(requestResponse.response);
            array->addItem(entry);
        }
        m_callback->sendSuccess(array, hasMore);
    }

private:
    DataRequestParams m_params;
    int m_numResponsesLeft;
    Vector<RequestResponse> m_responses;
    RefPtrWillBePersistent<RequestEntriesCallback> m_callback;
};

class GetCacheResponsesForRequestData : public WebServiceWorkerCache::CacheMatchCallbacks {
    WTF_MAKE_NONCOPYABLE(GetCacheResponsesForRequestData);

public:
    GetCacheResponsesForRequestData(
        const DataRequestParams& params, const WebServiceWorkerRequest& request,
        PassRefPtr<ResponsesAccumulator> accum, PassRefPtrWillBeRawPtr<RequestEntriesCallback> callback)
        : m_params(params)
        , m_request(request)
        , m_accumulator(accum)
        , m_callback(callback)
    {
    }
    ~GetCacheResponsesForRequestData() override { }

    void onSuccess(WebServiceWorkerResponse* response)
    {
        m_accumulator->addRequestResponsePair(m_request, *response);
    }

    void onError(WebServiceWorkerCacheError* error)
    {
        m_callback->sendFailure(String::format("Error requesting responses for cache  %s: %s", m_params.cacheName.utf8().data(), serviceWorkerCacheErrorString(error).data()));
    }

private:
    DataRequestParams m_params;
    WebServiceWorkerRequest m_request;
    RefPtr<ResponsesAccumulator> m_accumulator;
    RefPtrWillBePersistent<RequestEntriesCallback> m_callback;
};

class GetCacheKeysForRequestData : public WebServiceWorkerCache::CacheWithRequestsCallbacks {
    WTF_MAKE_NONCOPYABLE(GetCacheKeysForRequestData);

public:
    GetCacheKeysForRequestData(const DataRequestParams& params, PassOwnPtr<WebServiceWorkerCache> cache, PassRefPtrWillBeRawPtr<RequestEntriesCallback> callback)
        : m_params(params)
        , m_cache(cache)
        , m_callback(callback)
    {
    }
    ~GetCacheKeysForRequestData() override { }

    void onSuccess(WebVector<WebServiceWorkerRequest>* requests)
    {
        if (requests->isEmpty()) {
            RefPtr<Array<DataEntry>> array = Array<DataEntry>::create();
            m_callback->sendSuccess(array, false);
            return;
        }
        RefPtr<ResponsesAccumulator> accumulator = adoptRef(new ResponsesAccumulator(requests->size(), m_params, m_callback));

        for (size_t i = 0; i < requests->size(); i++) {
            const auto& request = (*requests)[i];
            auto* cacheRequest = new GetCacheResponsesForRequestData(m_params, request, accumulator, m_callback);
            m_cache->dispatchMatch(cacheRequest, request, WebServiceWorkerCache::QueryParams());
        }
    }

    void onError(WebServiceWorkerCacheError* error)
    {
        m_callback->sendFailure(String::format("Error requesting requests for cache %s: %s", m_params.cacheName.utf8().data(), serviceWorkerCacheErrorString(error).data()));
    }

private:
    DataRequestParams m_params;
    OwnPtr<WebServiceWorkerCache> m_cache;
    RefPtrWillBePersistent<RequestEntriesCallback> m_callback;
};

class GetCacheForRequestData
    : public WebServiceWorkerCacheStorage::CacheStorageWithCacheCallbacks {
    WTF_MAKE_NONCOPYABLE(GetCacheForRequestData);

public:
    GetCacheForRequestData(const DataRequestParams& params, PassRefPtrWillBeRawPtr<RequestEntriesCallback> callback)
        : m_params(params)
        , m_callback(callback)
    {
    }
    ~GetCacheForRequestData() override { }

    void onSuccess(WebServiceWorkerCache* cache)
    {
        auto* cacheRequest = new GetCacheKeysForRequestData(m_params, adoptPtr(cache), m_callback);
        cache->dispatchKeys(cacheRequest, nullptr, WebServiceWorkerCache::QueryParams());
    }

    void onError(WebServiceWorkerCacheError* error)
    {
        m_callback->sendFailure(String::format("Error requesting cache %s: %s", m_params.cacheName.utf8().data(), serviceWorkerCacheErrorString(error).data()));
    }

private:
    DataRequestParams m_params;
    RefPtrWillBePersistent<RequestEntriesCallback> m_callback;
};

class DeleteCache : public WebServiceWorkerCacheStorage::CacheStorageCallbacks {
    WTF_MAKE_NONCOPYABLE(DeleteCache);

public:
    DeleteCache(PassRefPtrWillBeRawPtr<DeleteCacheCallback> callback)
        : m_callback(callback)
    {
    }
    ~DeleteCache() override { }

    void onSuccess()
    {
        m_callback->sendSuccess();
    }

    void onError(WebServiceWorkerCacheError* error)
    {
        m_callback->sendFailure(String::format("Error requesting cache names: %s", serviceWorkerCacheErrorString(error).data()));
    }

private:
    RefPtrWillBePersistent<DeleteCacheCallback> m_callback;
};

class DeleteCacheEntry : public WebServiceWorkerCache::CacheBatchCallbacks {
    WTF_MAKE_NONCOPYABLE(DeleteCacheEntry);
public:

    DeleteCacheEntry(PassRefPtrWillBeRawPtr<DeleteEntryCallback> callback)
        : m_callback(callback)
    {
    }
    ~DeleteCacheEntry() override { }

    void onSuccess()
    {
        m_callback->sendSuccess();
    }

    void onError(WebServiceWorkerCacheError* error)
    {
        m_callback->sendFailure(String::format("Error requesting cache names: %s", serviceWorkerCacheErrorString(error).data()));
    }

private:
    RefPtrWillBePersistent<DeleteEntryCallback> m_callback;
};

class GetCacheForDeleteEntry
    : public WebServiceWorkerCacheStorage::CacheStorageWithCacheCallbacks {
    WTF_MAKE_NONCOPYABLE(GetCacheForDeleteEntry);

public:
    GetCacheForDeleteEntry(const String& requestSpec, const String& cacheName, PassRefPtrWillBeRawPtr<DeleteEntryCallback> callback)
        : m_requestSpec(requestSpec)
        , m_cacheName(cacheName)
        , m_callback(callback)
    {
    }
    ~GetCacheForDeleteEntry() override { }

    void onSuccess(WebServiceWorkerCache* cache)
    {
        auto* deleteRequest = new DeleteCacheEntry( m_callback);
        BatchOperation deleteOperation;
        deleteOperation.operationType = WebServiceWorkerCache::OperationTypeDelete;
        deleteOperation.request.setURL(KURL(ParsedURLString, m_requestSpec));
        Vector<BatchOperation> operations;
        operations.append(deleteOperation);
        cache->dispatchBatch(deleteRequest, WebVector<BatchOperation>(operations));
    }

    void onError(WebServiceWorkerCacheError* error)
    {
        m_callback->sendFailure(String::format("Error requesting cache %s: %s", m_cacheName.utf8().data(), serviceWorkerCacheErrorString(error).data()));
    }

private:
    String m_requestSpec;
    String m_cacheName;
    RefPtrWillBePersistent<DeleteEntryCallback> m_callback;
};

} // namespace

InspectorCacheStorageAgent::InspectorCacheStorageAgent()
    : InspectorBaseAgent<InspectorCacheStorageAgent, InspectorFrontend::CacheStorage>("CacheStorage")
{
}

InspectorCacheStorageAgent::~InspectorCacheStorageAgent() { }

DEFINE_TRACE(InspectorCacheStorageAgent)
{
    InspectorBaseAgent::trace(visitor);
}

void InspectorCacheStorageAgent::requestCacheNames(ErrorString* errorString, const String& securityOrigin, PassRefPtrWillBeRawPtr<RequestCacheNamesCallback> callback)
{
    OwnPtr<WebServiceWorkerCacheStorage> cache = assertCacheStorage(errorString, securityOrigin);
    if (!cache) {
        callback->sendFailure(*errorString);
        return;
    }
    cache->dispatchKeys(new RequestCacheNames(securityOrigin, callback));
}

void InspectorCacheStorageAgent::requestEntries(ErrorString* errorString, const String& cacheId, int skipCount, int pageSize, PassRefPtrWillBeRawPtr<RequestEntriesCallback> callback)
{
    String cacheName;
    OwnPtr<WebServiceWorkerCacheStorage> cache = assertCacheStorageAndNameForId(errorString, cacheId, &cacheName);
    if (!cache) {
        callback->sendFailure(*errorString);
        return;
    }
    DataRequestParams params;
    params.cacheName = cacheName;
    params.pageSize = pageSize;
    params.skipCount = skipCount;
    cache->dispatchOpen(new GetCacheForRequestData(params, callback), WebString(cacheName));
}

void InspectorCacheStorageAgent::deleteCache(ErrorString* errorString, const String& cacheId, PassRefPtrWillBeRawPtr<DeleteCacheCallback> callback)
{
    String cacheName;
    OwnPtr<WebServiceWorkerCacheStorage> cache = assertCacheStorageAndNameForId(errorString, cacheId, &cacheName);
    if (!cache) {
        callback->sendFailure(*errorString);
        return;
    }
    cache->dispatchDelete(new DeleteCache(callback), WebString(cacheName));
}

void InspectorCacheStorageAgent::deleteEntry(ErrorString* errorString, const String& cacheId, const String& request, PassRefPtrWillBeRawPtr<DeleteEntryCallback> callback)
{
    String cacheName;
    OwnPtr<WebServiceWorkerCacheStorage> cache = assertCacheStorageAndNameForId(errorString, cacheId, &cacheName);
    if (!cache) {
        callback->sendFailure(*errorString);
        return;
    }
    cache->dispatchOpen(new GetCacheForDeleteEntry(request, cacheName, callback), WebString(cacheName));
}


} // namespace blink
