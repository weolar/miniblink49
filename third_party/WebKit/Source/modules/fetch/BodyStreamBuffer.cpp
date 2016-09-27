// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/BodyStreamBuffer.h"

#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/ExceptionCode.h"

namespace blink {

BodyStreamBuffer* BodyStreamBuffer::createEmpty()
{
    return BodyStreamBuffer::create(createFetchDataConsumerHandleFromWebHandle(createDoneDataConsumerHandle()));
}

FetchDataConsumerHandle* BodyStreamBuffer::handle() const
{
    ASSERT(!m_fetchDataLoader);
    ASSERT(!m_drainingStreamNotificationClient);
    return m_handle.get();
}

PassOwnPtr<FetchDataConsumerHandle> BodyStreamBuffer::releaseHandle()
{
    ASSERT(!m_fetchDataLoader);
    ASSERT(!m_drainingStreamNotificationClient);
    return m_handle.release();
}

class LoaderHolder final : public GarbageCollectedFinalized<LoaderHolder>, public ActiveDOMObject, public FetchDataLoader::Client {
    USING_GARBAGE_COLLECTED_MIXIN(LoaderHolder);
public:
    LoaderHolder(ExecutionContext* executionContext, BodyStreamBuffer* buffer, FetchDataLoader* loader, FetchDataLoader::Client* client)
        : ActiveDOMObject(executionContext)
        , m_buffer(buffer)
        , m_loader(loader)
        , m_client(client)
    {
        suspendIfNeeded();
    }

    void start(FetchDataConsumerHandle* handle)
    {
        m_loader->start(handle, this);
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_buffer);
        visitor->trace(m_loader);
        visitor->trace(m_client);
        ActiveDOMObject::trace(visitor);
        FetchDataLoader::Client::trace(visitor);
    }

private:
    void didFetchDataLoadedBlobHandle(PassRefPtr<BlobDataHandle> blobDataHandle) override
    {
        m_loader.clear();
        if (m_client)
            m_client->didFetchDataLoadedBlobHandle(blobDataHandle);
        m_buffer->didFetchDataLoadFinished();
    }
    void didFetchDataLoadedArrayBuffer(PassRefPtr<DOMArrayBuffer> arrayBuffer) override
    {
        m_loader.clear();
        if (m_client)
            m_client->didFetchDataLoadedArrayBuffer(arrayBuffer);
        m_buffer->didFetchDataLoadFinished();
    }
    void didFetchDataLoadedString(const String& str) override
    {
        m_loader.clear();
        if (m_client)
            m_client->didFetchDataLoadedString(str);
        m_buffer->didFetchDataLoadFinished();
    }
    void didFetchDataLoadedStream() override
    {
        m_loader.clear();
        if (m_client)
            m_client->didFetchDataLoadedStream();
        m_buffer->didFetchDataLoadFinished();
    }
    void didFetchDataLoadFailed() override
    {
        m_loader.clear();
        if (m_client)
            m_client->didFetchDataLoadFailed();
        m_buffer->didFetchDataLoadFinished();
    }

    void stop() override
    {
        if (m_loader) {
            m_loader->cancel();
            m_loader.clear();
            m_buffer->didFetchDataLoadFinished();
        }
    }

    Member<BodyStreamBuffer> m_buffer;
    Member<FetchDataLoader> m_loader;
    Member<FetchDataLoader::Client> m_client;
};

void BodyStreamBuffer::setDrainingStreamNotificationClient(DrainingStreamNotificationClient* client)
{
    ASSERT(!m_fetchDataLoader);
    ASSERT(!m_drainingStreamNotificationClient);
    m_drainingStreamNotificationClient = client;
}

void BodyStreamBuffer::startLoading(ExecutionContext* executionContext, FetchDataLoader* fetchDataLoader, FetchDataLoader::Client* client)
{
    ASSERT(!m_fetchDataLoader);
    auto holder = new LoaderHolder(executionContext, this, fetchDataLoader, client);
    m_fetchDataLoader = holder;
    holder->start(m_handle.get());
}

void BodyStreamBuffer::doDrainingStreamNotification()
{
    ASSERT(!m_fetchDataLoader);
    DrainingStreamNotificationClient* client = m_drainingStreamNotificationClient;
    m_drainingStreamNotificationClient.clear();
    if (client)
        client->didFetchDataLoadFinishedFromDrainingStream();
}

void BodyStreamBuffer::clearDrainingStreamNotification()
{
    ASSERT(!m_fetchDataLoader);
    m_drainingStreamNotificationClient.clear();
}

void BodyStreamBuffer::didFetchDataLoadFinished()
{
    ASSERT(m_fetchDataLoader);
    m_fetchDataLoader.clear();
    doDrainingStreamNotification();
}

DrainingBodyStreamBuffer::~DrainingBodyStreamBuffer()
{
    if (m_buffer)
        m_buffer->doDrainingStreamNotification();
}

void DrainingBodyStreamBuffer::startLoading(ExecutionContext* executionContext, FetchDataLoader* fetchDataLoader, FetchDataLoader::Client* client)
{
    if (!m_buffer)
        return;

    m_buffer->startLoading(executionContext, fetchDataLoader, client);
    m_buffer.clear();
}

BodyStreamBuffer* DrainingBodyStreamBuffer::leakBuffer()
{
    if (!m_buffer)
        return nullptr;

    m_buffer->clearDrainingStreamNotification();
    BodyStreamBuffer* buffer = m_buffer;
    m_buffer.clear();
    return buffer;
}

PassRefPtr<BlobDataHandle> DrainingBodyStreamBuffer::drainAsBlobDataHandle(FetchDataConsumerHandle::Reader::BlobSizePolicy blobSizePolicy)
{
    if (!m_buffer)
        return nullptr;

    RefPtr<BlobDataHandle> blobDataHandle = m_buffer->m_handle->obtainReader(nullptr)->drainAsBlobDataHandle(blobSizePolicy);
    if (!blobDataHandle)
        return nullptr;
    m_buffer->doDrainingStreamNotification();
    m_buffer.clear();
    return blobDataHandle.release();
}

DrainingBodyStreamBuffer::DrainingBodyStreamBuffer(BodyStreamBuffer* buffer, BodyStreamBuffer::DrainingStreamNotificationClient* client)
    : m_buffer(buffer)
{
    ASSERT(client);
    m_buffer->setDrainingStreamNotificationClient(client);
}

} // namespace blink
