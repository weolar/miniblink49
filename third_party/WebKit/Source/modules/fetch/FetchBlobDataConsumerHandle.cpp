// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/FetchBlobDataConsumerHandle.h"

#include "core/dom/ActiveDOMObject.h"
#include "core/dom/CrossThreadTask.h"
#include "core/dom/ExecutionContext.h"
#include "core/fetch/FetchInitiatorTypeNames.h"
#include "core/loader/ThreadableLoaderClient.h"
#include "modules/fetch/CompositeDataConsumerHandle.h"
#include "modules/fetch/DataConsumerHandleUtil.h"
#include "platform/Task.h"
#include "platform/blob/BlobRegistry.h"
#include "platform/blob/BlobURL.h"
#include "platform/network/ResourceRequest.h"
#include "public/platform/Platform.h"
#include "public/platform/WebTraceLocation.h"
#include "wtf/Locker.h"
#include "wtf/ThreadingPrimitives.h"

namespace blink {

using Result = FetchBlobDataConsumerHandle::Result;

namespace {

// CrossThreadHolder<T> provides cross-thread access to |obj| of class T
// bound to the thread of |executionContext| where |obj| is created.
// - CrossThreadHolder<T> can be passed across threads.
// - |obj|'s methods are called on the thread of |executionContext|
//   via CrossThreadHolder<T>::postTask().
// - |obj| is destructed on the thread of |executionContext|
//   when |executionContext| is stopped or
//   CrossThreadHolder is destructed (earlier of them).
//   Note: |obj|'s destruction can be slightly after CrossThreadHolder.
template<typename T>
class CrossThreadHolder {
public:
    // Must be called on the thread where |obj| is created
    // (== the thread of |executionContext|).
    // The current thread must be attached to Oilpan.
    static PassOwnPtr<CrossThreadHolder<T>> create(ExecutionContext* executionContext, PassOwnPtr<T> obj)
    {
        ASSERT(executionContext->isContextThread());
        return adoptPtr(new CrossThreadHolder(executionContext, obj));
    }

    // Can be called from any thread.
    // Executes |task| with |obj| and |executionContext| on the thread of
    // |executionContext|.
    // NOTE: |task| might be silently ignored (i.e. not executed) and
    // destructed (possibly on the calling thread or on the thread of
    // |executionContext|) when |executionContext| is stopped or
    // CrossThreadHolder is destructed.
    void postTask(PassOwnPtr<WTF::Function<void(T*, ExecutionContext*)>> task)
    {
        MutexLocker locker(m_mutex->mutex());
        if (!m_bridge) {
            // The bridge has already disappeared.
            return;
        }
        m_bridge->executionContext()->postTask(FROM_HERE, createCrossThreadTask(&Bridge::runTask, m_bridge.get(), task));
    }

    ~CrossThreadHolder()
    {
        MutexLocker locker(m_mutex->mutex());
        clearInternal();
    }

private:
    // Object graph:
    //                 +------+                          +-----------------+
    //     T <-OwnPtr- |Bridge| ---------RawPtr--------> |CrossThreadHolder|
    //                 |      | <-CrossThreadPersistent- |                 |
    //                 +------+                          +-----------------+
    //                    |                                    |
    //                    +--RefPtr--> MutexWrapper <--RefPtr--+
    // The CrossThreadPersistent/RawPtr between CrossThreadHolder and Bridge
    // are protected by MutexWrapper
    // and cleared when CrossThreadHolder::clearInternal() is called, i.e.:
    // [1] when |executionContext| is stopped, or
    // [2] when CrossThreadHolder is destructed.
    // Then Bridge is shortly garbage collected and T is destructed.

    class MutexWrapper : public ThreadSafeRefCounted<MutexWrapper> {
    public:
        static PassRefPtr<MutexWrapper> create() { return adoptRef(new MutexWrapper()); }
        Mutex& mutex() { return m_mutex; }
    private:
        MutexWrapper() = default;
        Mutex m_mutex;
    };

    // All methods except for clearInternal()
    // must be called on |executionContext|'s thread.
    class Bridge
        : public GarbageCollectedFinalized<Bridge>
        , public ActiveDOMObject {
        WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(Bridge);
    public:
        Bridge(ExecutionContext* executionContext, PassOwnPtr<T> obj, PassRefPtr<MutexWrapper> mutex, CrossThreadHolder* holder)
            : ActiveDOMObject(executionContext)
            , m_obj(obj)
            , m_mutex(mutex)
            , m_holder(holder)
        {
            suspendIfNeeded();
        }

        DEFINE_INLINE_TRACE()
        {
            ActiveDOMObject::trace(visitor);
        }

        T* getObject() const { return m_obj.get(); }

        // Must be protected by |m_mutex|.
        // Is called from CrossThreadHolder::clearInternal() and
        // can be called on any thread.
        void clearInternal()
        {
            // We don't clear |m_obj| here to destruct |m_obj| on the thread
            // of |executionContext|.
            m_holder = nullptr;
        }

        void runTask(PassOwnPtr<WTF::Function<void(T*, ExecutionContext*)>> task)
        {
            ASSERT(executionContext()->isContextThread());
            if (m_obj)
                (*task)(m_obj.get(), executionContext());
        }

    private:
        // ActiveDOMObject
        void stop() override
        {
            ASSERT(executionContext()->isContextThread());
            {
                MutexLocker locker(m_mutex->mutex());
                if (m_holder)
                    m_holder->clearInternal();
                ASSERT(!m_holder);
            }

            // We have to destruct |*m_obj| here because destructing |*m_obj|
            // in ~Bridge() might be too late when |executionContext| is
            // stopped.
            m_obj.clear();
        }


        OwnPtr<T> m_obj;
        // All accesses to |m_holder| must be protected by |m_mutex|.
        RefPtr<MutexWrapper> m_mutex;
        CrossThreadHolder* m_holder;
    };

    // Must be protected by |m_mutex|.
    void clearInternal()
    {
        if (m_bridge)
            m_bridge->clearInternal();
        m_bridge.clear();
    }

    CrossThreadHolder(ExecutionContext* executionContext, PassOwnPtr<T> obj)
        : m_mutex(MutexWrapper::create())
        , m_bridge(new Bridge(executionContext, obj, m_mutex, this))
    {
    }

    RefPtr<MutexWrapper> m_mutex;
    // |m_bridge| is protected by |m_mutex|.
    // |m_bridge| is cleared before the thread that allocated |*m_bridge|
    // is stopped.
    CrossThreadPersistent<Bridge> m_bridge;
};

// Object graph:
// +-------------+                                          +-------------+
// |HandleWrapper|<-----------------------------------------|ReaderContext|
// |             |  +-------------+  +-----------+   +---+  |             |
// |             |<-|LoaderContext|<-|CTH::Bridge|<->|CTH|<-|             |
// +-------------+  +-------------+  +-----------+   +---+  +-------------+
//                              |
//          ThreadableLoader <--+
//
// When the loader thread is stopped, CrossThreadHolder::Bridge and
// LoaderContext (and thus ThreadableLoader) is destructed:
// +-------------+                                          +-------------+
// |HandleWrapper|<-----------------------------------------|ReaderContext|
// |             |                                   +---+  |             |
// |             |                                   |CTH|<-|             |
// +-------------+                                   +---+  +-------------+
// and the rest will be destructed when ReaderContext is destructed.
//
// When ReaderContext is destructed, CrossThreadHolder is destructed:
// +-------------+
// |HandleWrapper|
// |             |  +-------------+  +-----------+
// |             |<-|LoaderContext|<-|CTH::Bridge|
// +-------------+  +-------------+  +-----------+
//                              |
//          ThreadableLoader <--+
// and the rest will be shortly destructed when CrossThreadHolder::Bridge
// is garbage collected.

// LoaderContext is created and destructed on the same thread
// (call this thread loader thread).
// All methods must be called on the loader thread.
class LoaderContext {
public:
    virtual ~LoaderContext() { }
    virtual void start(ExecutionContext*) = 0;
};

class HandleWrapper : public ThreadSafeRefCounted<HandleWrapper> {
public:
    static PassRefPtr<HandleWrapper> create() { return adoptRef(new HandleWrapper()); }
    CompositeDataConsumerHandle* handle() { return m_handle.get(); }
private:
    HandleWrapper()
        : m_handle(CompositeDataConsumerHandle::create(createWaitingDataConsumerHandle())) { }

    OwnPtr<CompositeDataConsumerHandle> m_handle;
};

// All methods must be called on the loader thread.
class BlobLoaderContext final
    : public LoaderContext
    , public ThreadableLoaderClient {
public:
    BlobLoaderContext(PassRefPtr<HandleWrapper> handleWrapper, PassRefPtr<BlobDataHandle> blobDataHandle, FetchBlobDataConsumerHandle::LoaderFactory* loaderFactory)
        : m_handleWrapper(handleWrapper)
        , m_blobDataHandle(blobDataHandle)
        , m_loaderFactory(loaderFactory)
        , m_receivedResponse(false) { }

    ~BlobLoaderContext() override
    {
        if (m_loader && !m_receivedResponse)
            m_handleWrapper->handle()->update(createUnexpectedErrorDataConsumerHandle());
        if (m_loader) {
            m_loader->cancel();
            m_loader.clear();
        }
    }

    void start(ExecutionContext* executionContext) override
    {
        ASSERT(executionContext->isContextThread());
        ASSERT(!m_loader);

        m_loader = createLoader(executionContext, this);
        if (!m_loader)
            m_handleWrapper->handle()->update(createUnexpectedErrorDataConsumerHandle());
    }

private:
    PassRefPtr<ThreadableLoader> createLoader(ExecutionContext* executionContext, ThreadableLoaderClient* client) const
    {
        KURL url = BlobURL::createPublicURL(executionContext->securityOrigin());
        if (url.isEmpty()) {
            return nullptr;
        }
        BlobRegistry::registerPublicBlobURL(executionContext->securityOrigin(), url, m_blobDataHandle);

        ResourceRequest request(url);
        request.setRequestContext(WebURLRequest::RequestContextInternal);
        request.setUseStreamOnResponse(true);

        ThreadableLoaderOptions options;
        options.preflightPolicy = ConsiderPreflight;
        options.crossOriginRequestPolicy = DenyCrossOriginRequests;
        options.contentSecurityPolicyEnforcement = DoNotEnforceContentSecurityPolicy;
        options.initiator = FetchInitiatorTypeNames::internal;

        ResourceLoaderOptions resourceLoaderOptions;
        resourceLoaderOptions.dataBufferingPolicy = DoNotBufferData;

        return m_loaderFactory->create(*executionContext, client, request, options, resourceLoaderOptions);
    }

    // ThreadableLoaderClient
    void didReceiveResponse(unsigned long, const ResourceResponse&, PassOwnPtr<WebDataConsumerHandle> handle) override
    {
        ASSERT(!m_receivedResponse);
        m_receivedResponse = true;
        if (!handle) {
            // Here we assume WebURLLoader must return the response body as
            // |WebDataConsumerHandle| since we call
            // request.setUseStreamOnResponse().
            m_handleWrapper->handle()->update(createUnexpectedErrorDataConsumerHandle());
            return;
        }
        m_handleWrapper->handle()->update(handle);
    }

    void didFinishLoading(unsigned long, double) override
    {
        m_loader.clear();
    }

    void didFail(const ResourceError&) override
    {
        if (!m_receivedResponse)
            m_handleWrapper->handle()->update(createUnexpectedErrorDataConsumerHandle());
        m_loader.clear();
    }

    void didFailRedirectCheck() override
    {
        // We don't expect redirects for Blob loading.
        ASSERT_NOT_REACHED();
    }

    RefPtr<HandleWrapper> m_handleWrapper;

    RefPtr<BlobDataHandle> m_blobDataHandle;
    Persistent<FetchBlobDataConsumerHandle::LoaderFactory> m_loaderFactory;
    RefPtr<ThreadableLoader> m_loader;

    bool m_receivedResponse;
};

class DefaultLoaderFactory final : public FetchBlobDataConsumerHandle::LoaderFactory {
public:
    PassRefPtr<ThreadableLoader> create(
        ExecutionContext& executionContext,
        ThreadableLoaderClient* client,
        const ResourceRequest& request,
        const ThreadableLoaderOptions& options,
        const ResourceLoaderOptions& resourceLoaderOptions) override
    {
        return ThreadableLoader::create(executionContext, client, request, options, resourceLoaderOptions);
    }
};

} // namespace

// ReaderContext is referenced from FetchBlobDataConsumerHandle and
// ReaderContext::ReaderImpl.
// All functions/members must be called/accessed only on the reader thread,
// except for constructor, destructor, and obtainReader().
class FetchBlobDataConsumerHandle::ReaderContext final : public ThreadSafeRefCounted<ReaderContext> {
public:
    class ReaderImpl : public FetchDataConsumerHandle::Reader {
    public:
        ReaderImpl(Client* client, PassRefPtr<ReaderContext> readerContext, PassOwnPtr<WebDataConsumerHandle::Reader> reader)
            : m_readerContext(readerContext)
            , m_reader(reader)
            , m_notifier(client) { }
        ~ReaderImpl() override { }

        Result read(void* data, size_t size, Flags flags, size_t* readSize) override
        {
            if (m_readerContext->drained())
                return Done;
            m_readerContext->ensureStartLoader();
            Result r = m_reader->read(data, size, flags, readSize);
            if (r != ShouldWait && !(r == Ok && *readSize == 0)) {
                // We read non-empty data, so we cannot use the blob data
                // handle which represents the whole data.
                m_readerContext->clearBlobDataHandleForDrain();
            }
            return r;
        }

        Result beginRead(const void** buffer, Flags flags, size_t* available) override
        {
            if (m_readerContext->drained())
                return Done;
            m_readerContext->ensureStartLoader();
            Result r = m_reader->beginRead(buffer, flags, available);
            if (r != ShouldWait && !(r == Ok && *available == 0)) {
                // We read non-empty data, so we cannot use the blob data
                // handle which represents the whole data.
                m_readerContext->clearBlobDataHandleForDrain();
            }
            return r;
        }

        Result endRead(size_t readSize) override
        {
            return m_reader->endRead(readSize);
        }

        PassRefPtr<BlobDataHandle> drainAsBlobDataHandle(BlobSizePolicy blobSizePolicy) override
        {
            if (!m_readerContext->m_blobDataHandleForDrain)
                return nullptr;
            if (blobSizePolicy == DisallowBlobWithInvalidSize && m_readerContext->m_blobDataHandleForDrain->size() == kuint64max)
                return nullptr;
            RefPtr<BlobDataHandle> blobDataHandle = m_readerContext->m_blobDataHandleForDrain;
            m_readerContext->setDrained();
            m_readerContext->clearBlobDataHandleForDrain();
            return blobDataHandle.release();
        }

    private:
        RefPtr<ReaderContext> m_readerContext;
        OwnPtr<WebDataConsumerHandle::Reader> m_reader;
        NotifyOnReaderCreationHelper m_notifier;
    };

    ReaderContext(ExecutionContext* executionContext, PassRefPtr<BlobDataHandle> blobDataHandle, FetchBlobDataConsumerHandle::LoaderFactory* loaderFactory)
        : m_handleWrapper(HandleWrapper::create())
        , m_blobDataHandleForDrain(blobDataHandle)
        , m_loaderContextHolder(CrossThreadHolder<LoaderContext>::create(executionContext, adoptPtr(new BlobLoaderContext(m_handleWrapper, m_blobDataHandleForDrain, loaderFactory))))
        , m_loaderStarted(false)
        , m_drained(false)
    {
    }

    PassOwnPtr<FetchDataConsumerHandle::Reader> obtainReader(WebDataConsumerHandle::Client* client)
    {
        return adoptPtr(new ReaderImpl(client, this, m_handleWrapper->handle()->obtainReader(client)));
    }

private:
    void ensureStartLoader()
    {
        if (m_loaderStarted)
            return;
        m_loaderStarted = true;
        m_loaderContextHolder->postTask(threadSafeBind<LoaderContext*, ExecutionContext*>(&LoaderContext::start));
    }

    void clearBlobDataHandleForDrain()
    {
        m_blobDataHandleForDrain.clear();
    }

    bool drained() const { return m_drained; }
    void setDrained() { m_drained = true; }

    RefPtr<HandleWrapper> m_handleWrapper;
    RefPtr<BlobDataHandle> m_blobDataHandleForDrain;
    OwnPtr<CrossThreadHolder<LoaderContext>> m_loaderContextHolder;

    bool m_loaderStarted;
    bool m_drained;
};

FetchBlobDataConsumerHandle::FetchBlobDataConsumerHandle(ExecutionContext* executionContext, PassRefPtr<BlobDataHandle> blobDataHandle, LoaderFactory* loaderFactory)
    : m_readerContext(adoptRef(new ReaderContext(executionContext, blobDataHandle, loaderFactory)))
{
}

FetchBlobDataConsumerHandle::~FetchBlobDataConsumerHandle()
{
}

PassOwnPtr<FetchDataConsumerHandle> FetchBlobDataConsumerHandle::create(ExecutionContext* executionContext, PassRefPtr<BlobDataHandle> blobDataHandle, LoaderFactory* loaderFactory)
{
    if (!blobDataHandle)
        return createFetchDataConsumerHandleFromWebHandle(createDoneDataConsumerHandle());

    return adoptPtr(new FetchBlobDataConsumerHandle(executionContext, blobDataHandle, loaderFactory));
}

PassOwnPtr<FetchDataConsumerHandle> FetchBlobDataConsumerHandle::create(ExecutionContext* executionContext, PassRefPtr<BlobDataHandle> blobDataHandle)
{
    if (!blobDataHandle)
        return createFetchDataConsumerHandleFromWebHandle(createDoneDataConsumerHandle());

    return adoptPtr(new FetchBlobDataConsumerHandle(executionContext, blobDataHandle, new DefaultLoaderFactory));
}

FetchDataConsumerHandle::Reader* FetchBlobDataConsumerHandle::obtainReaderInternal(Client* client)
{
    return m_readerContext->obtainReader(client).leakPtr();
}

} // namespace blink
