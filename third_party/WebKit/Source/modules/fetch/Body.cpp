// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/Body.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/V8ArrayBuffer.h"
#include "bindings/core/v8/V8ThrowException.h"
#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/DOMTypedArray.h"
#include "core/dom/ExceptionCode.h"
#include "core/fileapi/Blob.h"
#include "core/frame/UseCounter.h"
#include "core/streams/ReadableByteStream.h"
#include "core/streams/ReadableByteStreamReader.h"
#include "core/streams/UnderlyingSource.h"
#include "modules/fetch/BodyStreamBuffer.h"
#include "modules/fetch/DataConsumerHandleUtil.h"
#include "modules/fetch/FetchBlobDataConsumerHandle.h"

namespace blink {

class Body::ReadableStreamSource : public GarbageCollectedFinalized<Body::ReadableStreamSource>, public UnderlyingSource, public WebDataConsumerHandle::Client, public BodyStreamBuffer::DrainingStreamNotificationClient {
    USING_GARBAGE_COLLECTED_MIXIN(ReadableStreamSource);
public:
    ReadableStreamSource(ExecutionContext* executionContext, BodyStreamBuffer* buffer)
        : m_bodyStreamBuffer(buffer)
        , m_streamNeedsMore(false)
#if ENABLE(ASSERT)
        , m_drained(false)
        , m_isCloseCalled(false)
        , m_isErrorCalled(false)
#endif
    {
        if (m_bodyStreamBuffer)
            obtainReader();
    }

    ~ReadableStreamSource() override { }

    void startStream(ReadableByteStream* stream)
    {
        m_stream = stream;
        stream->didSourceStart();
        if (!m_bodyStreamBuffer)
            close();
    }
    // Creates a new BodyStreamBuffer to drain the data.
    PassOwnPtr<DrainingBodyStreamBuffer> createDrainingStream()
    {
        if (!m_bodyStreamBuffer)
            return nullptr;

#if ENABLE(ASSERT)
        ASSERT(!m_drained);
        m_drained = true;
        ASSERT(!(m_stream->stateInternal() == ReadableByteStream::Closed && !m_isCloseCalled));
        ASSERT(!(m_stream->stateInternal() == ReadableByteStream::Errored && !m_isErrorCalled));
#endif

        m_reader.clear();
        return DrainingBodyStreamBuffer::create(m_bodyStreamBuffer, this);
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_bodyStreamBuffer);
        visitor->trace(m_stream);
        UnderlyingSource::trace(visitor);
        DrainingStreamNotificationClient::trace(visitor);
    }

    void close()
    {
        m_reader.clear();
        m_stream->close();
        if (m_bodyStreamBuffer)
            m_bodyStreamBuffer = BodyStreamBuffer::createEmpty();
#if ENABLE(ASSERT)
        m_isCloseCalled = true;
#endif
    }

    void error()
    {
        m_reader.clear();
        m_stream->error(DOMException::create(NetworkError, "network error"));
        if (m_bodyStreamBuffer)
            m_bodyStreamBuffer = BodyStreamBuffer::create(createFetchDataConsumerHandleFromWebHandle(createUnexpectedErrorDataConsumerHandle()));
#if ENABLE(ASSERT)
        m_isErrorCalled = true;
#endif
    }

private:
    void obtainReader()
    {
        m_reader = m_bodyStreamBuffer->handle()->obtainReader(this);
    }

    void didFetchDataLoadFinishedFromDrainingStream() override
    {
        ASSERT(m_bodyStreamBuffer);
        ASSERT(m_drained);

#if ENABLE(ASSERT)
        m_drained = false;
#endif
        obtainReader();
        // We have to call didGetReadable() now to call close()/error() if
        // necessary.
        // didGetReadable() would be called asynchronously, but it is too late.
        didGetReadable();
    }

    void didGetReadable() override
    {
        if (!m_streamNeedsMore) {
            // Perform zero-length read to call close()/error() early.
            size_t readSize;
            WebDataConsumerHandle::Result result = m_reader->read(nullptr, 0, WebDataConsumerHandle::FlagNone, &readSize);
            switch (result) {
            case WebDataConsumerHandle::Ok:
            case WebDataConsumerHandle::ShouldWait:
                return;
            case WebDataConsumerHandle::Done:
                close();
                return;
            case WebDataConsumerHandle::Busy:
            case WebDataConsumerHandle::ResourceExhausted:
            case WebDataConsumerHandle::UnexpectedError:
                error();
                return;
            }
        }

        processData();
    }

    // UnderlyingSource functions.
    void pullSource() override
    {
        ASSERT(!m_streamNeedsMore);
        m_streamNeedsMore = true;

        ASSERT(!m_drained);

        processData();
    }

    ScriptPromise cancelSource(ScriptState* scriptState, ScriptValue reason) override
    {
        close();
        return ScriptPromise::cast(scriptState, v8::Undefined(scriptState->isolate()));
    }

    // Reads data and writes the data to |m_stream|, as long as data are
    // available and the stream has pending reads.
    void processData()
    {
        ASSERT(m_reader);
        while (m_streamNeedsMore) {
            const void* buffer;
            size_t available;
            WebDataConsumerHandle::Result result = m_reader->beginRead(&buffer, WebDataConsumerHandle::FlagNone, &available);
            switch (result) {
            case WebDataConsumerHandle::Ok:
                m_streamNeedsMore = m_stream->enqueue(DOMUint8Array::create(static_cast<const unsigned char*>(buffer), available));
                m_reader->endRead(available);
                break;

            case WebDataConsumerHandle::Done:
                close();
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

    // Source of data.
    Member<BodyStreamBuffer> m_bodyStreamBuffer;
    OwnPtr<FetchDataConsumerHandle::Reader> m_reader;

    Member<ReadableByteStream> m_stream;
    bool m_streamNeedsMore;
#if ENABLE(ASSERT)
    bool m_drained;
    bool m_isCloseCalled;
    bool m_isErrorCalled;
#endif
};

ScriptPromise Body::readAsync(ScriptState* scriptState, ResponseType type)
{
    if (bodyUsed())
        return ScriptPromise::reject(scriptState, V8ThrowException::createTypeError(scriptState->isolate(), "Already read"));

    // When the main thread sends a V8::TerminateExecution() signal to a worker
    // thread, any V8 API on the worker thread starts returning an empty
    // handle. This can happen in Body::readAsync. To avoid the situation, we
    // first check the ExecutionContext and return immediately if it's already
    // gone (which means that the V8::TerminateExecution() signal has been sent
    // to this worker thread).
    ExecutionContext* executionContext = scriptState->executionContext();
    if (!executionContext)
        return ScriptPromise();

    lockBody();
    m_responseType = type;

    ASSERT(!m_resolver);
    m_resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = m_resolver->promise();

    if (m_stream->stateInternal() == ReadableStream::Closed) {
        resolveWithEmptyDataSynchronously();
    } else if (m_stream->stateInternal() == ReadableStream::Errored) {
        m_resolver->reject(m_stream->storedException());
        m_resolver.clear();
    } else {
        readAsyncFromDrainingBodyStreamBuffer(createDrainingStream(), mimeType());
    }
    return promise;
}

void Body::resolveWithEmptyDataSynchronously()
{
    // We resolve the resolver manually in order not to use member
    // variables.
    switch (m_responseType) {
    case ResponseAsArrayBuffer:
        m_resolver->resolve(DOMArrayBuffer::create(nullptr, 0));
        break;
    case ResponseAsBlob: {
        OwnPtr<BlobData> blobData = BlobData::create();
        blobData->setContentType(mimeType());
        m_resolver->resolve(Blob::create(BlobDataHandle::create(blobData.release(), 0)));
        break;
    }
    case ResponseAsText:
        m_resolver->resolve(String());
        break;
    case ResponseAsFormData:
        // TODO(yhirano): Implement this.
        ASSERT_NOT_REACHED();
        break;
    case ResponseAsJSON: {
        ScriptState::Scope scope(m_resolver->scriptState());
        m_resolver->reject(V8ThrowException::createSyntaxError(m_resolver->scriptState()->isolate(), "Unexpected end of input"));
        break;
    }
    case ResponseUnknown:
        ASSERT_NOT_REACHED();
        break;
    }
    m_resolver.clear();
}

void Body::readAsyncFromDrainingBodyStreamBuffer(PassOwnPtr<DrainingBodyStreamBuffer> buffer, const String& mimeType)
{
    if (!buffer) {
        resolveWithEmptyDataSynchronously();
        m_streamSource->close();
        return;
    }

    FetchDataLoader* fetchDataLoader = nullptr;

    switch (m_responseType) {
    case ResponseAsArrayBuffer:
        fetchDataLoader = FetchDataLoader::createLoaderAsArrayBuffer();
        break;

    case ResponseAsJSON:
    case ResponseAsText:
        fetchDataLoader = FetchDataLoader::createLoaderAsString();
        break;

    case ResponseAsBlob:
        fetchDataLoader = FetchDataLoader::createLoaderAsBlobHandle(mimeType);
        break;

    case ResponseAsFormData:
        // FIXME: Implement this.
        ASSERT_NOT_REACHED();
        return;

    default:
        ASSERT_NOT_REACHED();
        return;
    }

    buffer->startLoading(executionContext(), fetchDataLoader, this);
}

ScriptPromise Body::arrayBuffer(ScriptState* scriptState)
{
    return readAsync(scriptState, ResponseAsArrayBuffer);
}

ScriptPromise Body::blob(ScriptState* scriptState)
{
    return readAsync(scriptState, ResponseAsBlob);
}

ScriptPromise Body::formData(ScriptState* scriptState)
{
    return readAsync(scriptState, ResponseAsFormData);
}

ScriptPromise Body::json(ScriptState* scriptState)
{
    return readAsync(scriptState, ResponseAsJSON);
}

ScriptPromise Body::text(ScriptState* scriptState)
{
    return readAsync(scriptState, ResponseAsText);
}

ReadableByteStream* Body::body()
{
    UseCounter::count(executionContext(), UseCounter::FetchBodyStream);
    return m_stream;
}

bool Body::bodyUsed() const
{
    return m_bodyUsed || m_stream->isLocked();
}

void Body::lockBody(LockBodyOption option)
{
    ASSERT(!bodyUsed());
    if (option == PassBody)
        m_bodyUsed = true;
    ASSERT(!m_stream->isLocked());
    TrackExceptionState exceptionState;
    m_stream->getBytesReader(executionContext(), exceptionState);
    ASSERT(!exceptionState.hadException());
}

void Body::setBody(BodyStreamBuffer* buffer)
{
    m_streamSource = new ReadableStreamSource(executionContext(), buffer);
    m_stream = new ReadableByteStream(m_streamSource, new ReadableByteStream::StrictStrategy);
    m_streamSource->startStream(m_stream);
}

PassOwnPtr<DrainingBodyStreamBuffer> Body::createDrainingStream()
{
    return m_streamSource->createDrainingStream();
}

bool Body::hasPendingActivity() const
{
    if (executionContext()->activeDOMObjectsAreStopped())
        return false;
    if (m_resolver)
        return true;
    if (m_stream->isLocked())
        return true;
    return false;
}

DEFINE_TRACE(Body)
{
    visitor->trace(m_resolver);
    visitor->trace(m_stream);
    visitor->trace(m_streamSource);
    ActiveDOMObject::trace(visitor);
    FetchDataLoader::Client::trace(visitor);
}

Body::Body(ExecutionContext* context)
    : ActiveDOMObject(context)
    , m_bodyUsed(false)
    , m_responseType(ResponseType::ResponseUnknown)
    , m_streamSource(new ReadableStreamSource(context, nullptr))
    , m_stream(new ReadableByteStream(m_streamSource, new ReadableByteStream::StrictStrategy))
{
    m_streamSource->startStream(m_stream);
}

void Body::resolveJSON(const String& string)
{
    ASSERT(m_responseType == ResponseAsJSON);
    ScriptState::Scope scope(m_resolver->scriptState());
    v8::Isolate* isolate = m_resolver->scriptState()->isolate();
    v8::Local<v8::String> inputString = v8String(isolate, string);
    v8::TryCatch trycatch(isolate);
    v8::Local<v8::Value> parsed;
    if (v8Call(v8::JSON::Parse(isolate, inputString), parsed, trycatch))
        m_resolver->resolve(parsed);
    else
        m_resolver->reject(trycatch.Exception());
}

// FetchDataLoader::Client functions.
void Body::didFetchDataLoadFailed()
{
    if (!executionContext() || executionContext()->activeDOMObjectsAreStopped())
        return;

    if (m_resolver) {
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped()) {
            m_resolver.clear();
            return;
        }
        ScriptState* state = m_resolver->scriptState();
        ScriptState::Scope scope(state);
        m_resolver->reject(V8ThrowException::createTypeError(state->isolate(), "Failed to fetch"));
        m_resolver.clear();
    }
}

void Body::didFetchDataLoadedBlobHandle(PassRefPtr<BlobDataHandle> blobDataHandle)
{
    if (!executionContext() || executionContext()->activeDOMObjectsAreStopped())
        return;

    ASSERT(m_responseType == ResponseAsBlob);
    m_resolver->resolve(Blob::create(blobDataHandle));
    m_resolver.clear();
}

void Body::didFetchDataLoadedArrayBuffer(PassRefPtr<DOMArrayBuffer> arrayBuffer)
{
    if (!executionContext() || executionContext()->activeDOMObjectsAreStopped())
        return;

    ASSERT(m_responseType == ResponseAsArrayBuffer);
    m_resolver->resolve(arrayBuffer);
    m_resolver.clear();
}

void Body::didFetchDataLoadedString(const String& str)
{
    if (!executionContext() || executionContext()->activeDOMObjectsAreStopped())
        return;

    switch (m_responseType) {
    case ResponseAsJSON:
        resolveJSON(str);
        break;
    case ResponseAsText:
        m_resolver->resolve(str);
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    m_resolver.clear();
}

} // namespace blink
