// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/CompositeDataConsumerHandle.h"

#include "platform/Task.h"
#include "platform/ThreadSafeFunctional.h"
#include "public/platform/Platform.h"
#include "public/platform/WebThread.h"
#include "public/platform/WebTraceLocation.h"
#include "wtf/Locker.h"
#include "wtf/ThreadSafeRefCounted.h"
#include "wtf/ThreadingPrimitives.h"

namespace blink {

using Result = WebDataConsumerHandle::Result;

class CompositeDataConsumerHandle::ReaderImpl final : public WebDataConsumerHandle::Reader {
public:
    explicit ReaderImpl(PassRefPtr<Context>);
    ~ReaderImpl() override;
    Result read(void* data, size_t /* size */, Flags, size_t* readSize) override;
    Result beginRead(const void** buffer, Flags, size_t* available) override;
    Result endRead(size_t readSize) override;

private:
    RefPtr<Context> m_context;
};

class CompositeDataConsumerHandle::Context final : public ThreadSafeRefCounted<Context> {
public:
    using Token = unsigned;
    static PassRefPtr<Context> create(PassOwnPtr<WebDataConsumerHandle> handle) { return adoptRef(new Context(handle)); }
    ~Context()
    {
        ASSERT(!m_readerThread);
        ASSERT(!m_reader);
        ASSERT(!m_client);
    }
    PassOwnPtr<ReaderImpl> obtainReader(Client* client)
    {
        MutexLocker locker(m_mutex);
        ASSERT(!m_readerThread);
        ASSERT(!m_reader);
        ASSERT(!m_client);
        ++m_token;
        m_client = client;
        m_readerThread = Platform::current()->currentThread();
        m_reader = m_handle->obtainReader(m_client);
        return adoptPtr(new ReaderImpl(this));
    }
    void detachReader()
    {
        MutexLocker locker(m_mutex);
        ASSERT(m_readerThread);
        ASSERT(m_readerThread->isCurrentThread());
        ASSERT(m_reader);
        ASSERT(!m_isInTwoPhaseRead);
        ASSERT(!m_isUpdateWaitingForEndRead);
        ++m_token;
        m_reader = nullptr;
        m_readerThread = nullptr;
        m_client = nullptr;
    }
    void update(PassOwnPtr<WebDataConsumerHandle> handle)
    {
        MutexLocker locker(m_mutex);
        m_handle = handle;
        if (!m_readerThread) {
            // There is no reader.
            return;
        }
        ++m_token;
        updateReaderNoLock(m_token);
    }

    Result read(void* data, size_t size, Flags flags, size_t* readSize)
    {
        ASSERT(m_readerThread && m_readerThread->isCurrentThread());
        return m_reader->read(data, size, flags, readSize);
    }
    Result beginRead(const void** buffer, Flags flags, size_t* available)
    {
        ASSERT(m_readerThread && m_readerThread->isCurrentThread());
        ASSERT(!m_isInTwoPhaseRead);
        Result r = m_reader->beginRead(buffer, flags, available);
        m_isInTwoPhaseRead = (r == Ok);
        return r;
    }
    Result endRead(size_t readSize)
    {
        ASSERT(m_readerThread && m_readerThread->isCurrentThread());
        ASSERT(m_isInTwoPhaseRead);
        Result r = m_reader->endRead(readSize);
        m_isInTwoPhaseRead = false;
        if (m_isUpdateWaitingForEndRead) {
            // We need this lock to access |m_handle|.
            MutexLocker locker(m_mutex);
            m_reader = nullptr;
            m_reader = m_handle->obtainReader(m_client);
            m_isUpdateWaitingForEndRead = false;
        }
        return r;
    }

private:
    explicit Context(PassOwnPtr<WebDataConsumerHandle> handle)
        : m_handle(handle)
        , m_readerThread(nullptr)
        , m_client(nullptr)
        , m_token(0)
        , m_isUpdateWaitingForEndRead(false)
        , m_isInTwoPhaseRead(false)
    {
    }
    void updateReader(Token token)
    {
        MutexLocker locker(m_mutex);
        updateReaderNoLock(token);
    }
    void updateReaderNoLock(Token token)
    {
        if (token != m_token) {
            // This request is not fresh. Ignore it.
            return;
        }
        ASSERT(m_readerThread);
        ASSERT(m_reader);
        if (m_readerThread->isCurrentThread()) {
            if (m_isInTwoPhaseRead) {
                // We are waiting for the two-phase read completion.
                m_isUpdateWaitingForEndRead = true;
                return;
            }
            // Unregister the old one, then register the new one.
            m_reader = nullptr;
            m_reader = m_handle->obtainReader(m_client);
            return;
        }
        ++m_token;
        m_readerThread->postTask(FROM_HERE, new Task(threadSafeBind(&Context::updateReader, this, m_token)));
    }

    OwnPtr<Reader> m_reader;
    OwnPtr<WebDataConsumerHandle> m_handle;
    // Note: Holding a WebThread raw pointer is not generally safe, but we can
    // do that in this case because:
    //  1. Destructing a ReaderImpl when the bound thread ends is a user's
    //     responsibility.
    //  2. |m_readerThread| will never be used after the associated reader is
    //     detached.
    WebThread* m_readerThread;
    Client* m_client;
    Token m_token;
    // These boolean values are bound to the reader thread.
    bool m_isUpdateWaitingForEndRead;
    bool m_isInTwoPhaseRead;
    Mutex m_mutex;
};

CompositeDataConsumerHandle::ReaderImpl::ReaderImpl(PassRefPtr<Context> context) : m_context(context) { }

CompositeDataConsumerHandle::ReaderImpl::~ReaderImpl()
{
    m_context->detachReader();
}

Result CompositeDataConsumerHandle::ReaderImpl::read(void* data, size_t size, Flags flags, size_t* readSize)
{
    return m_context->read(data, size, flags, readSize);
}

Result CompositeDataConsumerHandle::ReaderImpl::beginRead(const void** buffer, Flags flags, size_t* available)
{
    return m_context->beginRead(buffer, flags, available);
}

Result CompositeDataConsumerHandle::ReaderImpl::endRead(size_t readSize)
{
    return m_context->endRead(readSize);
}

CompositeDataConsumerHandle::CompositeDataConsumerHandle(PassOwnPtr<WebDataConsumerHandle> handle)
    : m_context(Context::create(handle)) { }

CompositeDataConsumerHandle::~CompositeDataConsumerHandle() { }

WebDataConsumerHandle::Reader* CompositeDataConsumerHandle::obtainReaderInternal(Client* client)
{
    return m_context->obtainReader(client).leakPtr();
}

void CompositeDataConsumerHandle::update(PassOwnPtr<WebDataConsumerHandle> handle)
{
    ASSERT(handle);
    m_context->update(handle);
}

} // namespace blink
