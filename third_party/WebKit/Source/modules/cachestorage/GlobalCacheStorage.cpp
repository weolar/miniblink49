// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/cachestorage/GlobalCacheStorage.h"

#include "core/dom/ExecutionContext.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/UseCounter.h"
#include "core/workers/WorkerGlobalScope.h"
#include "modules/cachestorage/CacheStorage.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/DatabaseIdentifier.h"
#include "public/platform/Platform.h"

namespace blink {

namespace {

template <typename T>
class GlobalCacheStorageImpl final : public NoBaseWillBeGarbageCollectedFinalized<GlobalCacheStorageImpl<T>>, public WillBeHeapSupplement<T> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(GlobalCacheStorageImpl);
public:
    static GlobalCacheStorageImpl& from(T& supplementable, ExecutionContext* executionContext)
    {
        GlobalCacheStorageImpl* supplement = static_cast<GlobalCacheStorageImpl*>(WillBeHeapSupplement<T>::from(supplementable, name()));
        if (!supplement) {
            supplement = new GlobalCacheStorageImpl();
            WillBeHeapSupplement<T>::provideTo(supplementable, name(), adoptPtrWillBeNoop(supplement));
        }
        return *supplement;
    }

    ~GlobalCacheStorageImpl()
    {
        if (m_caches)
            m_caches->dispose();
    }

    CacheStorage* caches(T& fetchingScope, ExceptionState& exceptionState)
    {
        ExecutionContext* context = fetchingScope.executionContext();
        if (!context->securityOrigin()->canAccessCacheStorage()) {
            if (context->securityContext().isSandboxed(SandboxOrigin))
                exceptionState.throwSecurityError("Cache storage is disabled because the context is sandboxed and lacks the 'allow-same-origin' flag.");
            else if (context->url().protocolIs("data"))
                exceptionState.throwSecurityError("Cache storage is disabled inside 'data:' URLs.");
            else
                exceptionState.throwSecurityError("Access to cache storage is denied.");
            return nullptr;
        }

        if (!m_caches) {
            String identifier = createDatabaseIdentifierFromSecurityOrigin(context->securityOrigin());
            ASSERT(!identifier.isEmpty());
            m_caches = CacheStorage::create(GlobalFetch::ScopedFetcher::from(fetchingScope), Platform::current()->cacheStorage(identifier));
        }
        return m_caches;
    }

    // Promptly dispose of associated CacheStorage.
    EAGERLY_FINALIZE();
    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_caches);
        WillBeHeapSupplement<T>::trace(visitor);
    }

private:
    GlobalCacheStorageImpl()
    {
    }

    static const char* name() { return "CacheStorage"; }

    PersistentWillBeMember<CacheStorage> m_caches;
};

} // namespace

CacheStorage* GlobalCacheStorage::caches(DOMWindow& window, ExceptionState& exceptionState)
{
    return GlobalCacheStorageImpl<LocalDOMWindow>::from(toLocalDOMWindow(window), window.executionContext()).caches(toLocalDOMWindow(window), exceptionState);
}

CacheStorage* GlobalCacheStorage::caches(WorkerGlobalScope& worker, ExceptionState& exceptionState)
{
    return GlobalCacheStorageImpl<WorkerGlobalScope>::from(worker, worker.executionContext()).caches(worker, exceptionState);
}

} // namespace blink
