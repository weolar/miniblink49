// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/FetchDataLoader.h"

#include "core/html/parser/TextResourceDecoder.h"
#include "wtf/ArrayBufferBuilder.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"

namespace blink {

namespace {

class FetchDataLoaderAsBlobHandle
    : public FetchDataLoader
    , public WebDataConsumerHandle::Client {
public:
    explicit FetchDataLoaderAsBlobHandle(const String& mimeType)
        : m_client(nullptr)
        , m_mimeType(mimeType) { }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        FetchDataLoader::trace(visitor);
        visitor->trace(m_client);
    }

private:
    void start(FetchDataConsumerHandle* handle, FetchDataLoader::Client* client) override
    {
        ASSERT(!m_client);
        ASSERT(!m_reader);

        m_client = client;
        // Passing |this| here is safe because |this| owns |m_reader|.
        m_reader = handle->obtainReader(this);
        RefPtr<BlobDataHandle> blobHandle = m_reader->drainAsBlobDataHandle();
        if (blobHandle) {
            ASSERT(blobHandle->size() != kuint64max);
            m_reader.clear();
            if (blobHandle->type() != m_mimeType) {
                // A new BlobDataHandle is created to override the Blob's type.
                m_client->didFetchDataLoadedBlobHandle(BlobDataHandle::create(blobHandle->uuid(), m_mimeType, blobHandle->size()));
            } else {
                m_client->didFetchDataLoadedBlobHandle(blobHandle);
            }
            m_client.clear();
            return;
        }

        // We read data from |m_reader| and create a new blob.
        m_blobData = BlobData::create();
        m_blobData->setContentType(m_mimeType);
    }

    void didGetReadable() override
    {
        ASSERT(m_client);
        ASSERT(m_reader);

        while (true) {
            const void* buffer;
            size_t available;
            WebDataConsumerHandle::Result result = m_reader->beginRead(&buffer, WebDataConsumerHandle::FlagNone, &available);

            switch (result) {
            case WebDataConsumerHandle::Ok:
                m_blobData->appendBytes(buffer, available);
                m_reader->endRead(available);
                break;

            case WebDataConsumerHandle::Done: {
                m_reader.clear();
                long long size = m_blobData->length();
                m_client->didFetchDataLoadedBlobHandle(BlobDataHandle::create(m_blobData.release(), size));
                m_client.clear();
                return;
            }

            case WebDataConsumerHandle::ShouldWait:
                return;

            case WebDataConsumerHandle::Busy:
            case WebDataConsumerHandle::ResourceExhausted:
            case WebDataConsumerHandle::UnexpectedError:
                m_reader.clear();
                m_blobData.clear();
                m_client->didFetchDataLoadFailed();
                m_client.clear();
                return;
            }
        }
    }

    void cancel() override
    {
        m_reader.clear();
        m_blobData.clear();
        m_client.clear();
    }

    OwnPtr<FetchDataConsumerHandle::Reader> m_reader;
    Member<FetchDataLoader::Client> m_client;

    String m_mimeType;
    OwnPtr<BlobData> m_blobData;
};

class FetchDataLoaderAsArrayBuffer
    : public FetchDataLoader
    , public WebDataConsumerHandle::Client {
public:
    FetchDataLoaderAsArrayBuffer()
        : m_client(nullptr) { }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        FetchDataLoader::trace(visitor);
        visitor->trace(m_client);
    }

protected:
    void start(FetchDataConsumerHandle* handle, FetchDataLoader::Client* client) override
    {
        ASSERT(!m_client);
        ASSERT(!m_rawData);
        ASSERT(!m_reader);
        m_client = client;
        m_rawData = adoptPtr(new ArrayBufferBuilder());
        m_reader = handle->obtainReader(this);
    }

    void didGetReadable() override
    {
        ASSERT(m_client);
        ASSERT(m_rawData);
        ASSERT(m_reader);

        while (true) {
            const void* buffer;
            size_t available;
            WebDataConsumerHandle::Result result = m_reader->beginRead(&buffer, WebDataConsumerHandle::FlagNone, &available);

            switch (result) {
            case WebDataConsumerHandle::Ok:
                if (available > 0) {
                    unsigned bytesAppended = m_rawData->append(static_cast<const char*>(buffer), available);
                    if (!bytesAppended) {
                        m_reader->endRead(0);
                        error();
                        return;
                    }
                    ASSERT(bytesAppended == available);
                }
                m_reader->endRead(available);
                break;

            case WebDataConsumerHandle::Done:
                m_reader.clear();
                m_client->didFetchDataLoadedArrayBuffer(DOMArrayBuffer::create(m_rawData->toArrayBuffer()));
                m_rawData.clear();
                m_client.clear();
                return;

            case WebDataConsumerHandle::ShouldWait:
                return;

            case WebDataConsumerHandle::Busy:
            case WebDataConsumerHandle::ResourceExhausted:
            case WebDataConsumerHandle::UnexpectedError:
                error();
                return;
            }
        }
    }

    void error()
    {
        m_reader.clear();
        m_rawData.clear();
        m_client->didFetchDataLoadFailed();
        m_client.clear();
    }

    void cancel() override
    {
        m_reader.clear();
        m_rawData.clear();
        m_client.clear();
    }

    OwnPtr<FetchDataConsumerHandle::Reader> m_reader;
    Member<FetchDataLoader::Client> m_client;

    OwnPtr<ArrayBufferBuilder> m_rawData;
};

class FetchDataLoaderAsString
    : public FetchDataLoader
    , public WebDataConsumerHandle::Client {
public:
    FetchDataLoaderAsString()
        : m_client(nullptr) { }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        FetchDataLoader::trace(visitor);
        visitor->trace(m_client);
    }

protected:
    void start(FetchDataConsumerHandle* handle, FetchDataLoader::Client* client) override
    {
        ASSERT(!m_client);
        ASSERT(!m_decoder);
        ASSERT(!m_reader);
        m_client = client;
        m_decoder = TextResourceDecoder::create("text/plain", UTF8Encoding());
        m_reader = handle->obtainReader(this);
    }

    void didGetReadable() override
    {
        ASSERT(m_client);
        ASSERT(m_decoder);
        ASSERT(m_reader);

        while (true) {
            const void* buffer;
            size_t available;
            WebDataConsumerHandle::Result result = m_reader->beginRead(&buffer, WebDataConsumerHandle::FlagNone, &available);

            switch (result) {
            case WebDataConsumerHandle::Ok:
                if (available > 0)
                    m_builder.append(m_decoder->decode(static_cast<const char*>(buffer), available));
                m_reader->endRead(available);
                break;

            case WebDataConsumerHandle::Done:
                m_reader.clear();
                m_builder.append(m_decoder->flush());
                m_client->didFetchDataLoadedString(m_builder.toString());
                m_builder.clear();
                m_decoder.clear();
                m_client.clear();
                return;

            case WebDataConsumerHandle::ShouldWait:
                return;

            case WebDataConsumerHandle::Busy:
            case WebDataConsumerHandle::ResourceExhausted:
            case WebDataConsumerHandle::UnexpectedError:
                error();
                return;
            }
        }
    }

    void error()
    {
        m_reader.clear();
        m_builder.clear();
        m_decoder.clear();
        m_client->didFetchDataLoadFailed();
        m_client.clear();
    }

    void cancel() override
    {
        m_reader.clear();
        m_builder.clear();
        m_decoder.clear();
        m_client.clear();
    }

    OwnPtr<FetchDataConsumerHandle::Reader> m_reader;
    Member<FetchDataLoader::Client> m_client;

    OwnPtr<TextResourceDecoder> m_decoder;
    StringBuilder m_builder;
};

class FetchDataLoaderAsStream
    : public FetchDataLoader
    , public WebDataConsumerHandle::Client {
public:
    explicit FetchDataLoaderAsStream(Stream* outStream)
        : m_client(nullptr)
        , m_outStream(outStream) { }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        FetchDataLoader::trace(visitor);
        visitor->trace(m_client);
        visitor->trace(m_outStream);
    }

protected:
    void start(FetchDataConsumerHandle* handle, FetchDataLoader::Client* client) override
    {
        ASSERT(!m_client);
        ASSERT(!m_reader);
        m_client = client;
        m_reader = handle->obtainReader(this);
    }

    void didGetReadable() override
    {
        ASSERT(m_client);
        ASSERT(m_reader);

        bool needToFlush = false;
        while (true) {
            const void* buffer;
            size_t available;
            WebDataConsumerHandle::Result result = m_reader->beginRead(&buffer, WebDataConsumerHandle::FlagNone, &available);

            switch (result) {
            case WebDataConsumerHandle::Ok:
                m_outStream->addData(static_cast<const char*>(buffer), available);
                m_reader->endRead(available);
                needToFlush = true;
                break;

            case WebDataConsumerHandle::Done:
                m_reader.clear();
                if (needToFlush)
                    m_outStream->flush();
                m_outStream->finalize();
                m_client->didFetchDataLoadedStream();
                cleanup();
                return;

            case WebDataConsumerHandle::ShouldWait:
                if (needToFlush)
                    m_outStream->flush();
                return;

            case WebDataConsumerHandle::Busy:
            case WebDataConsumerHandle::ResourceExhausted:
            case WebDataConsumerHandle::UnexpectedError:
                // If the stream is aborted soon after the stream is registered
                // to the StreamRegistry, ServiceWorkerURLRequestJob may not
                // notice the error and continue waiting forever.
                // FIXME: Add new message to report the error to the browser
                // process.
                m_reader.clear();
                m_outStream->abort();
                m_client->didFetchDataLoadFailed();
                cleanup();
                return;
            }
        }
    }

    void cancel() override
    {
        cleanup();
    }

    void cleanup()
    {
        m_reader.clear();
        m_client.clear();
        m_outStream.clear();
    }

    OwnPtr<FetchDataConsumerHandle::Reader> m_reader;
    Member<FetchDataLoader::Client> m_client;

    Member<Stream> m_outStream;
};


} // namespace

FetchDataLoader* FetchDataLoader::createLoaderAsBlobHandle(const String& mimeType)
{
    return new FetchDataLoaderAsBlobHandle(mimeType);
}

FetchDataLoader* FetchDataLoader::createLoaderAsArrayBuffer()
{
    return new FetchDataLoaderAsArrayBuffer();
}

FetchDataLoader* FetchDataLoader::createLoaderAsString()
{
    return new FetchDataLoaderAsString();
}

FetchDataLoader* FetchDataLoader::createLoaderAsStream(Stream* outStream)
{
    return new FetchDataLoaderAsStream(outStream);
}

} // namespace blink
