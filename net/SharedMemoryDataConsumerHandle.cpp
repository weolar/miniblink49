// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/SharedMemoryDataConsumerHandle.h"

#include "net/FixedReceivedData.h"
#include "third_party/WebKit/public/platform/WebThread.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/Source/wtf/ThreadSafeRefCounted.h"
#include "cef/include/base/cef_lock.h"
#include <algorithm>
#include <deque>

namespace net {

namespace {

class DelegateThreadSafeReceivedData final : public RequestPeer::ThreadSafeReceivedData {
public:
    explicit DelegateThreadSafeReceivedData(PassOwnPtr<RequestPeer::ReceivedData> data)
        : m_data(data), m_taskRunner(blink::Platform::current()->currentThread())
    {
    }

    static void DeleteSoon(RequestPeer::ReceivedData* data)
    {
        delete data;
    }

    ~DelegateThreadSafeReceivedData() override
    {
        if (m_taskRunner != blink::Platform::current()->currentThread()) {
            // Delete the data on the original thread.
            m_taskRunner->postTask(FROM_HERE, WTF::bind(&DeleteSoon, m_data.leakPtr()));
        }
    }

    const char* payload() const override { return m_data->payload(); }
    int length() const override { return m_data->length(); }
    int encodedLength() const override { return m_data->encodedLength(); }

private:
    OwnPtr<RequestPeer::ReceivedData> m_data;
    blink::WebThread* m_taskRunner;

    DISALLOW_COPY_AND_ASSIGN(DelegateThreadSafeReceivedData);
};

}  // namespace

using Result = blink::WebDataConsumerHandle::Result;

// All methods (except for ctor/dtor) must be called with |m_lock| aquired
// unless otherwise stated.
class SharedMemoryDataConsumerHandle::Context final : public WTF::ThreadSafeRefCounted<Context> {
public:
    explicit Context(PassOwnPtr<WTF::Closure> onReaderDetached)
        : m_result(Ok)
        , m_firstOffset(0)
        , m_client(nullptr)
        , m_writerTaskRunner(blink::Platform::current()->currentThread())
        , m_onReaderDetached(onReaderDetached)
        , m_isOnReaderDetachedValid(!!m_onReaderDetached)
        , m_isHandleActive(true)
        , m_isTwoPhaseReadInProgress(false)
        , m_notificationTaskRunner(nullptr)
    {
    }

    bool isEmpty() const
    {
        m_lock.AssertAcquired();
        return m_queue.empty();
    }
    void clearIfNecessary()
    {
        m_lock.AssertAcquired();
        if (!isHandleLocked() && !isHandleActive()) {
            // No one is interested in the contents.
            if (m_isOnReaderDetachedValid) {
                // We post a task even in the writer thread in order to avoid a
                // reentrance problem as calling |m_onReaderDetached| may manipulate
                // the context synchronously.
                m_writerTaskRunner->postTask(FROM_HERE, m_onReaderDetached);
            }
            clear();
        }
    }
    void clearQueue()
    {
        m_lock.AssertAcquired();
        for (auto& data : m_queue) {
            delete data;
        }
        m_queue.clear();
        m_firstOffset = 0;
    }
    RequestPeer::ThreadSafeReceivedData* top()
    {
        m_lock.AssertAcquired();
        return m_queue.front();
    }
    void push(PassOwnPtr<RequestPeer::ThreadSafeReceivedData> data)
    {
        m_lock.AssertAcquired();
        m_queue.push_back(data.leakPtr());
    }
    size_t firstOffset() const
    {
        m_lock.AssertAcquired();
        return m_firstOffset;
    }
    Result result() const
    {
        m_lock.AssertAcquired();
        return m_result;
    }
    void setResult(Result r)
    {
        m_lock.AssertAcquired();
        m_result = r;
    }
    void acquireReaderLock(Client* client)
    {
        m_lock.AssertAcquired();
        ASSERT(!m_notificationTaskRunner);
        ASSERT(!m_client);
        m_notificationTaskRunner = blink::Platform::current()->currentThread();
        m_client = client;
        if (client && !(isEmpty() && result() == Ok)) {
            // We cannot notify synchronously because the user doesn't have the reader yet.
            m_notificationTaskRunner->postTask(FROM_HERE, WTF::bind(&Context::notifyInternal, this, false));
        }
    }
    void releaseReaderLock()
    {
        m_lock.AssertAcquired();
        ASSERT(m_notificationTaskRunner);
        m_notificationTaskRunner = nullptr;
        m_client = nullptr;
    }
    void postNotify()
    {
        m_lock.AssertAcquired();
        auto runner = m_notificationTaskRunner;
        if (!runner)
            return;
        // We don't re-post the task when the runner changes while waiting for
        // this task because in this case a new reader is obtained and
        // notification is already done at the reader creation time if necessary.
        runner->postTask(FROM_HERE, WTF::bind(&Context::notifyInternal, this, false));
    }
    // Must be called with |m_lock| not aquired.
    void notify() { notifyInternal(true); }
    // This function doesn't work in the destructor if |m_onReaderDetached| is
    // not null.
    void resetOnReaderDetached()
    {
        m_lock.AssertAcquired();
        if (!m_onReaderDetached) {
            ASSERT(!m_isOnReaderDetachedValid);
            return;
        }
        m_isOnReaderDetachedValid = false;
        if (m_writerTaskRunner == blink::Platform::current()->currentThread()) {
            // We can reset the closure immediately.
            m_onReaderDetached.leakPtr();
        } else {
            // We need to reset |m_onReaderDetached| on the right thread because it
            // might lead to the object destruction.
            m_writerTaskRunner->postTask(FROM_HERE, WTF::bind(&Context::resetOnReaderDetachedWithLock, this));
        }
    }
    bool isHandleLocked() const
    {
        m_lock.AssertAcquired();
        return m_notificationTaskRunner;
    }
    bool isReaderBoundToCurrentThread() const
    {
        m_lock.AssertAcquired();
        return m_notificationTaskRunner && m_notificationTaskRunner == blink::Platform::current()->currentThread();
    }
    bool isHandleActive() const
    {
        m_lock.AssertAcquired();
        return m_isHandleActive;
    }
    void setIsHandleActive(bool b)
    {
        m_lock.AssertAcquired();
        m_isHandleActive = b;
    }
    void consume(size_t s)
    {
        m_lock.AssertAcquired();
        m_firstOffset += s;
        RequestPeer::ThreadSafeReceivedData* topValue = top();
        if (static_cast<size_t>(topValue->length()) <= m_firstOffset) {
            delete topValue;
            m_queue.pop_front();
            m_firstOffset = 0;
        }
    }
    bool isTwoPhaseReadInProgress() const
    {
        m_lock.AssertAcquired();
        return m_isTwoPhaseReadInProgress;
    }
    void setIsTwoPhaseReadInProgress(bool b)
    {
        m_lock.AssertAcquired();
        m_isTwoPhaseReadInProgress = b;
    }
    // Can be called with |m_lock| not aquired.
    base::Lock& lock() { return m_lock; }

private:
    // Must be called with |m_lock| not aquired.
    void notifyInternal(bool repost)
    {
        blink::WebThread* runner;
        {
            base::AutoLock lock(m_lock);
            runner = m_notificationTaskRunner;
        }
        if (!runner)
            return;

        if (runner == blink::Platform::current()->currentThread()) {
            // It is safe to access member variables without lock because |m_client|
            // is bound to the current thread.
            if (m_client)
                m_client->didGetReadable();
            return;
        }
        if (repost) {
            // We don't re-post the task when the runner changes while waiting for
            // this task because in this case a new reader is obtained and
            // notification is already done at the reader creation time if necessary.
            runner->postTask(FROM_HERE, WTF::bind(&Context::notifyInternal, this, false));
        }
    }
    void clear()
    {
        m_lock.AssertAcquired();
        for (auto& data : m_queue) {
            delete data;
        }
        m_queue.clear();
        m_firstOffset = 0;
        m_client = nullptr;
        // Note this doesn't work in the destructor if |m_onReaderDetached| is not
        // null. We have an assert in the destructor.
        resetOnReaderDetached();
    }
    // Must be called with |m_lock| not aquired.
    void resetOnReaderDetachedWithLock()
    {
        base::AutoLock lock(m_lock);
        resetOnReaderDetached();
    }

    friend class WTF::ThreadSafeRefCounted<Context>;
    ~Context()
    {
        base::AutoLock lock(m_lock);
        ASSERT(!m_onReaderDetached);

        // This is necessary because the queue stores raw pointers.
        clear();
    }

    base::Lock m_lock;
    // |m_result| stores the ultimate state of this handle if it has. Otherwise, |Ok| is set.
    Result m_result;
    // TODO(yhirano): Use std::deque<PassOwnPtr<ThreadSafeReceivedData>> once it is allowed.
    std::deque<RequestPeer::ThreadSafeReceivedData*> m_queue;
    size_t m_firstOffset;
    Client* m_client;
    blink::WebThread* m_notificationTaskRunner;
    blink::WebThread* m_writerTaskRunner;
    PassOwnPtr<WTF::Closure> m_onReaderDetached;
    // We need this boolean variable to remember if |m_onReaderDetached| is
    // callable because we need to reset |m_onReaderDetached| only on the writer
    // thread and hence |m_onReaderDetached.is_null()| is untrustworthy on
    // other threads.
    bool m_isOnReaderDetachedValid;
    bool m_isHandleActive;
    bool m_isTwoPhaseReadInProgress;

    DISALLOW_COPY_AND_ASSIGN(Context);
};

SharedMemoryDataConsumerHandle::Writer::Writer(const PassRefPtr<Context>& context, BackpressureMode mode)
    : m_context(context), m_mode(mode)
{
}

SharedMemoryDataConsumerHandle::Writer::~Writer()
{
    close();
    base::AutoLock lock(m_context->lock());
    m_context->resetOnReaderDetached();
}

void SharedMemoryDataConsumerHandle::Writer::addData(PassOwnPtr<RequestPeer::ReceivedData> data)
{
    if (!data->length()) {
        // We omit empty data.
        return;
    }

    bool needsNotification = false;
    {
        base::AutoLock lock(m_context->lock());
        if (!m_context->isHandleActive() && !m_context->isHandleLocked()) {
            // No one is interested in the data.
            return;
        }

        needsNotification = m_context->isEmpty();
        RequestPeer::ThreadSafeReceivedData* dataToPass;
        if (m_mode == kApplyBackpressure) {
            dataToPass = (new DelegateThreadSafeReceivedData(data));
        } else {
            dataToPass = (new FixedReceivedData(data.leakPtr()));
        }
        m_context->push(adoptPtr(dataToPass));
    }

    if (needsNotification) {
        // We CAN issue the notification synchronously if the associated reader
        // lives in this thread, because this function cannot be called in the
        // client's callback.
        m_context->notify();
    }
}

void SharedMemoryDataConsumerHandle::Writer::close()
{
    base::AutoLock lock(m_context->lock());
    if (m_context->result() == Ok) {
        m_context->setResult(Done);
        m_context->resetOnReaderDetached();
        if (m_context->isEmpty()) {
            // We cannot issue the notification synchronously because this function
            // can be called in the client's callback.
            m_context->postNotify();
        }
    }
}

void SharedMemoryDataConsumerHandle::Writer::fail()
{
    base::AutoLock lock(m_context->lock());
    if (m_context->result() == Ok) {
        // TODO(yhirano): Use an appropriate error code other than
        // UnexpectedError.
        m_context->setResult(UnexpectedError);

        if (m_context->isTwoPhaseReadInProgress()) {
            // If we are in two-phase read session, we cannot discard the data. We
            // will clear the queue at the end of the session.
        } else {
            m_context->clearQueue();
        }

        m_context->resetOnReaderDetached();
        // We cannot issue the notification synchronously because this function can
        // be called in the client's callback.
        m_context->postNotify();
    }
}

SharedMemoryDataConsumerHandle::ReaderImpl::ReaderImpl(PassRefPtr<Context> context, Client* client)
    : m_context(context)
{
    base::AutoLock lock(m_context->lock());
    RELEASE_ASSERT(!m_context->isHandleLocked());
    m_context->acquireReaderLock(client);
}

SharedMemoryDataConsumerHandle::ReaderImpl::~ReaderImpl()
{
    base::AutoLock lock(m_context->lock());
    m_context->releaseReaderLock();
    m_context->clearIfNecessary();
}

Result SharedMemoryDataConsumerHandle::ReaderImpl::read(void* data, size_t size, Flags flags, size_t* readSizeToReturn)
{
    base::AutoLock lock(m_context->lock());

    size_t totalReadSize = 0;
    *readSizeToReturn = 0;

    if (m_context->result() == Ok && m_context->isTwoPhaseReadInProgress())
        m_context->setResult(UnexpectedError);

    if (m_context->result() != Ok && m_context->result() != Done)
        return m_context->result();

    while (!m_context->isEmpty() && totalReadSize < size) {
        const auto& top = m_context->top();
        size_t readable = top->length() - m_context->firstOffset();
        size_t writable = size - totalReadSize;
        size_t read_size = std::min(readable, writable);
        const char* begin = top->payload() + m_context->firstOffset();
        std::copy(begin, begin + read_size,
            static_cast<char*>(data) + totalReadSize);
        totalReadSize += read_size;
        m_context->consume(read_size);
    }
    *readSizeToReturn = totalReadSize;
    if (totalReadSize || !m_context->isEmpty())
        return Ok;
    if (m_context->result() == Done)
        return Done;
    return ShouldWait;
}

Result SharedMemoryDataConsumerHandle::ReaderImpl::beginRead(const void** buffer, Flags flags, size_t* available)
{
    *buffer = nullptr;
    *available = 0;

    base::AutoLock lock(m_context->lock());

    if (m_context->result() == Ok && m_context->isTwoPhaseReadInProgress())
        m_context->setResult(UnexpectedError);

    if (m_context->result() != Ok && m_context->result() != Done)
        return m_context->result();

    if (m_context->isEmpty())
        return m_context->result() == Done ? Done : ShouldWait;

    m_context->setIsTwoPhaseReadInProgress(true);
    const auto& top = m_context->top();
    *buffer = top->payload() + m_context->firstOffset();
    *available = top->length() - m_context->firstOffset();

    return Ok;
}

Result SharedMemoryDataConsumerHandle::ReaderImpl::endRead(size_t read_size)
{
    base::AutoLock lock(m_context->lock());

    if (!m_context->isTwoPhaseReadInProgress())
        return UnexpectedError;

    m_context->setIsTwoPhaseReadInProgress(false);
    if (m_context->result() != Ok && m_context->result() != Done) {
        // We have an error, so we can discard the stored data.
        m_context->clearQueue();
    } else {
        m_context->consume(read_size);
    }

    return Ok;
}

SharedMemoryDataConsumerHandle::SharedMemoryDataConsumerHandle(BackpressureMode mode, Writer** writer)
    : SharedMemoryDataConsumerHandle(mode, nullptr, writer)
{
}

SharedMemoryDataConsumerHandle::SharedMemoryDataConsumerHandle(BackpressureMode mode, PassOwnPtr<WTF::Closure> onReaderDetached, Writer** writer)
    : m_context(new Context(onReaderDetached))
{
    *writer = (new Writer(m_context, mode));
}

SharedMemoryDataConsumerHandle::~SharedMemoryDataConsumerHandle()
{
    base::AutoLock lock(m_context->lock());
    m_context->setIsHandleActive(false);
    m_context->clearIfNecessary();
}

PassOwnPtr<blink::WebDataConsumerHandle::Reader>
SharedMemoryDataConsumerHandle::otainReader(Client* client)
{
    return adoptPtr(obtainReaderInternal(client));
}

SharedMemoryDataConsumerHandle::ReaderImpl*
SharedMemoryDataConsumerHandle::obtainReaderInternal(Client* client)
{
    return new ReaderImpl(m_context, client);
}

const char* SharedMemoryDataConsumerHandle::debugName() const
{
    return "SharedMemoryDataConsumerHandle";
}

}  // namespace net
