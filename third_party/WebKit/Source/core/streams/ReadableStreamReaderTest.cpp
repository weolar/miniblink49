// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/streams/ReadableStreamReader.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ToV8.h"
#include "bindings/core/v8/V8ThrowException.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/streams/ReadableStream.h"
#include "core/streams/ReadableStreamImpl.h"
#include "core/streams/UnderlyingSource.h"
#include "core/testing/DummyPageHolder.h"
#include <gtest/gtest.h>

namespace blink {

using StringStream = ReadableStreamImpl<ReadableStreamChunkTypeTraits<String>>;

namespace {

struct ReadResult {
    ReadResult() : isDone(false), isSet(false) { }

    String valueString;
    bool isDone;
    bool isSet;
};

class StringCapturingFunction final : public ScriptFunction {
public:
    static v8::Local<v8::Function> createFunction(ScriptState* scriptState, String* value)
    {
        StringCapturingFunction* self = new StringCapturingFunction(scriptState, value);
        return self->bindToV8Function();
    }

private:
    StringCapturingFunction(ScriptState* scriptState, String* value)
        : ScriptFunction(scriptState)
        , m_value(value)
    {
    }

    ScriptValue call(ScriptValue value) override
    {
        ASSERT(!value.isEmpty());
        *m_value = toCoreString(value.v8Value()->ToString(scriptState()->context()).ToLocalChecked());
        return value;
    }

    String* m_value;
};

class ReadResultCapturingFunction final : public ScriptFunction {
public:
    static v8::Local<v8::Function> createFunction(ScriptState* scriptState, ReadResult* value)
    {
        ReadResultCapturingFunction* self = new ReadResultCapturingFunction(scriptState, value);
        return self->bindToV8Function();
    }

private:
    ReadResultCapturingFunction(ScriptState* scriptState, ReadResult* value)
        : ScriptFunction(scriptState)
        , m_result(value)
    {
    }

    ScriptValue call(ScriptValue result) override
    {
        ASSERT(!result.isEmpty());
        v8::Isolate* isolate = scriptState()->isolate();
        if (!result.isObject()) {
            return result;
        }
        v8::Local<v8::Object> object = result.v8Value().As<v8::Object>();
        v8::Local<v8::String> doneString = v8String(isolate, "done");
        v8::Local<v8::String> valueString = v8String(isolate, "value");
        v8::Local<v8::Context> context = scriptState()->context();
        v8::Maybe<bool> hasDone = object->Has(context, doneString);
        v8::Maybe<bool> hasValue = object->Has(context, valueString);

        if (hasDone.IsNothing() || !hasDone.FromJust() || hasValue.IsNothing() || !hasValue.FromJust()) {
            return result;
        }

        v8::Local<v8::Value> done;
        v8::Local<v8::Value> value;

        if (!object->Get(context, doneString).ToLocal(&done) || !object->Get(context, valueString).ToLocal(&value) || !done->IsBoolean()) {
            return result;
        }

        m_result->isSet = true;
        m_result->isDone = done.As<v8::Boolean>()->Value();
        m_result->valueString = toCoreString(value->ToString(scriptState()->context()).ToLocalChecked());
        return result;
    }

    ReadResult* m_result;
};

class NoopUnderlyingSource final : public GarbageCollectedFinalized<NoopUnderlyingSource>, public UnderlyingSource {
    USING_GARBAGE_COLLECTED_MIXIN(NoopUnderlyingSource);
public:
    ~NoopUnderlyingSource() override { }

    void pullSource() override { }
    ScriptPromise cancelSource(ScriptState* scriptState, ScriptValue reason) { return ScriptPromise::cast(scriptState, reason); }
    DEFINE_INLINE_VIRTUAL_TRACE() { UnderlyingSource::trace(visitor); }
};

class PermissiveStrategy final : public StringStream::Strategy {
public:
    bool shouldApplyBackpressure(size_t, ReadableStream*) override { return false; }
};

class ReadableStreamReaderTest : public ::testing::Test {
public:
    ReadableStreamReaderTest()
        : m_page(DummyPageHolder::create(IntSize(1, 1)))
        , m_scope(scriptState())
        , m_exceptionState(ExceptionState::ConstructionContext, "property", "interface", scriptState()->context()->Global(), isolate())
        , m_stream(new StringStream(new NoopUnderlyingSource, new PermissiveStrategy))
    {
        m_stream->didSourceStart();
    }

    ~ReadableStreamReaderTest() override
    {
        EXPECT_FALSE(m_exceptionState.hadException());

        // We need to call |error| in order to make
        // ActiveDOMObject::hasPendingActivity return false.
        m_stream->error(DOMException::create(AbortError, "done"));
    }

    ScriptState* scriptState() { return ScriptState::forMainWorld(m_page->document().frame()); }
    v8::Isolate* isolate() { return scriptState()->isolate(); }
    ExecutionContext* executionContext() { return scriptState()->executionContext(); }

    v8::Local<v8::Function> createCaptor(String* value)
    {
        return StringCapturingFunction::createFunction(scriptState(), value);
    }

    v8::Local<v8::Function> createResultCaptor(ReadResult* value)
    {
        return ReadResultCapturingFunction::createFunction(scriptState(), value);
    }

    OwnPtr<DummyPageHolder> m_page;
    ScriptState::Scope m_scope;
    ExceptionState m_exceptionState;
    Persistent<StringStream> m_stream;
};

TEST_F(ReadableStreamReaderTest, Construct)
{
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);
    EXPECT_TRUE(reader->isActive());
}

TEST_F(ReadableStreamReaderTest, Release)
{
    String onFulfilled, onRejected;
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);
    EXPECT_TRUE(reader->isActive());

    reader->closed(scriptState()).then(createCaptor(&onFulfilled), createCaptor(&onRejected));
    reader->releaseLock(m_exceptionState);
    EXPECT_FALSE(reader->isActive());
    EXPECT_FALSE(m_exceptionState.hadException());

    EXPECT_TRUE(onFulfilled.isNull());
    EXPECT_TRUE(onRejected.isNull());

    isolate()->RunMicrotasks();
    EXPECT_EQ("undefined", onFulfilled);
    EXPECT_TRUE(onRejected.isNull());

    ReadableStreamReader* another = new ReadableStreamReader(executionContext(), m_stream);
    EXPECT_TRUE(another->isActive());
    EXPECT_FALSE(reader->isActive());
    reader->releaseLock(m_exceptionState);
    EXPECT_TRUE(another->isActive());
    EXPECT_FALSE(reader->isActive());
    EXPECT_FALSE(m_exceptionState.hadException());
}

TEST_F(ReadableStreamReaderTest, ReadAfterRelease)
{
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);
    EXPECT_TRUE(reader->isActive());
    reader->releaseLock(m_exceptionState);
    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_FALSE(reader->isActive());

    ReadResult result;
    String onRejected;
    reader->read(scriptState()).then(createResultCaptor(&result), createCaptor(&onRejected));

    EXPECT_FALSE(result.isSet);
    EXPECT_TRUE(onRejected.isNull());
    isolate()->RunMicrotasks();

    EXPECT_TRUE(result.isSet);
    EXPECT_TRUE(result.isDone);
    EXPECT_EQ("undefined", result.valueString);
    EXPECT_TRUE(onRejected.isNull());
}

TEST_F(ReadableStreamReaderTest, ReleaseShouldFailWhenCalledWhileReading)
{
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);
    EXPECT_TRUE(reader->isActive());
    reader->read(scriptState());

    reader->releaseLock(m_exceptionState);
    EXPECT_TRUE(reader->isActive());
    EXPECT_TRUE(m_exceptionState.hadException());
    m_exceptionState.clearException();

    m_stream->enqueue("hello");
    reader->releaseLock(m_exceptionState);
    EXPECT_FALSE(reader->isActive());
    EXPECT_FALSE(m_exceptionState.hadException());
}

TEST_F(ReadableStreamReaderTest, EnqueueThenRead)
{
    m_stream->enqueue("hello");
    m_stream->enqueue("world");
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);
    EXPECT_EQ(ReadableStream::Readable, m_stream->stateInternal());

    ReadResult result;
    String onRejected;
    reader->read(scriptState()).then(createResultCaptor(&result), createCaptor(&onRejected));

    EXPECT_FALSE(result.isSet);
    EXPECT_TRUE(onRejected.isNull());

    isolate()->RunMicrotasks();

    EXPECT_TRUE(result.isSet);
    EXPECT_FALSE(result.isDone);
    EXPECT_EQ("hello", result.valueString);
    EXPECT_TRUE(onRejected.isNull());

    ReadResult result2;
    String onRejected2;
    reader->read(scriptState()).then(createResultCaptor(&result2), createCaptor(&onRejected2));

    EXPECT_FALSE(result2.isSet);
    EXPECT_TRUE(onRejected2.isNull());

    isolate()->RunMicrotasks();

    EXPECT_TRUE(result2.isSet);
    EXPECT_FALSE(result2.isDone);
    EXPECT_EQ("world", result2.valueString);
    EXPECT_TRUE(onRejected2.isNull());
}

TEST_F(ReadableStreamReaderTest, ReadThenEnqueue)
{
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);
    EXPECT_EQ(ReadableStream::Readable, m_stream->stateInternal());

    ReadResult result, result2;
    String onRejected, onRejected2;
    reader->read(scriptState()).then(createResultCaptor(&result), createCaptor(&onRejected));
    reader->read(scriptState()).then(createResultCaptor(&result2), createCaptor(&onRejected2));

    EXPECT_FALSE(result.isSet);
    EXPECT_TRUE(onRejected.isNull());
    EXPECT_FALSE(result2.isSet);
    EXPECT_TRUE(onRejected2.isNull());

    isolate()->RunMicrotasks();

    EXPECT_FALSE(result.isSet);
    EXPECT_TRUE(onRejected.isNull());
    EXPECT_FALSE(result2.isSet);
    EXPECT_TRUE(onRejected2.isNull());

    m_stream->enqueue("hello");
    isolate()->RunMicrotasks();

    EXPECT_TRUE(result.isSet);
    EXPECT_FALSE(result.isDone);
    EXPECT_EQ("hello", result.valueString);
    EXPECT_TRUE(onRejected.isNull());
    EXPECT_FALSE(result2.isSet);
    EXPECT_TRUE(onRejected2.isNull());

    m_stream->enqueue("world");
    isolate()->RunMicrotasks();

    EXPECT_TRUE(result2.isSet);
    EXPECT_FALSE(result2.isDone);
    EXPECT_EQ("world", result2.valueString);
    EXPECT_TRUE(onRejected2.isNull());
}

TEST_F(ReadableStreamReaderTest, ClosedReader)
{
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);

    m_stream->close();

    EXPECT_FALSE(reader->isActive());

    String onClosedFulfilled, onClosedRejected;
    ReadResult result;
    String onReadRejected;
    isolate()->RunMicrotasks();
    reader->closed(scriptState()).then(createCaptor(&onClosedFulfilled), createCaptor(&onClosedRejected));
    reader->read(scriptState()).then(createResultCaptor(&result), createCaptor(&onReadRejected));
    EXPECT_TRUE(onClosedFulfilled.isNull());
    EXPECT_TRUE(onClosedRejected.isNull());
    EXPECT_FALSE(result.isSet);
    EXPECT_TRUE(onReadRejected.isNull());

    isolate()->RunMicrotasks();
    EXPECT_EQ("undefined", onClosedFulfilled);
    EXPECT_TRUE(onClosedRejected.isNull());
    EXPECT_TRUE(result.isSet);
    EXPECT_TRUE(result.isDone);
    EXPECT_EQ("undefined", result.valueString);
    EXPECT_TRUE(onReadRejected.isNull());
}

TEST_F(ReadableStreamReaderTest, ErroredReader)
{
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);

    m_stream->error(DOMException::create(SyntaxError, "some error"));

    EXPECT_EQ(ReadableStream::Errored, m_stream->stateInternal());
    EXPECT_FALSE(reader->isActive());

    String onClosedFulfilled, onClosedRejected;
    String onReadFulfilled, onReadRejected;
    isolate()->RunMicrotasks();
    reader->closed(scriptState()).then(createCaptor(&onClosedFulfilled), createCaptor(&onClosedRejected));
    reader->read(scriptState()).then(createCaptor(&onReadFulfilled), createCaptor(&onReadRejected));
    EXPECT_TRUE(onClosedFulfilled.isNull());
    EXPECT_TRUE(onClosedRejected.isNull());
    EXPECT_TRUE(onReadFulfilled.isNull());
    EXPECT_TRUE(onReadRejected.isNull());

    isolate()->RunMicrotasks();
    EXPECT_TRUE(onClosedFulfilled.isNull());
    EXPECT_EQ("SyntaxError: some error", onClosedRejected);
    EXPECT_TRUE(onReadFulfilled.isNull());
    EXPECT_EQ("SyntaxError: some error", onReadRejected);
}

TEST_F(ReadableStreamReaderTest, PendingReadsShouldBeResolvedWhenClosed)
{
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);
    EXPECT_EQ(ReadableStream::Readable, m_stream->stateInternal());

    ReadResult result, result2;
    String onRejected, onRejected2;
    reader->read(scriptState()).then(createResultCaptor(&result), createCaptor(&onRejected));
    reader->read(scriptState()).then(createResultCaptor(&result2), createCaptor(&onRejected2));

    isolate()->RunMicrotasks();
    EXPECT_FALSE(result.isSet);
    EXPECT_TRUE(onRejected.isNull());
    EXPECT_FALSE(result2.isSet);
    EXPECT_TRUE(onRejected2.isNull());

    m_stream->close();
    EXPECT_FALSE(result.isSet);
    EXPECT_TRUE(onRejected.isNull());
    EXPECT_FALSE(result2.isSet);
    EXPECT_TRUE(onRejected2.isNull());

    isolate()->RunMicrotasks();

    EXPECT_TRUE(result.isSet);
    EXPECT_TRUE(result.isDone);
    EXPECT_EQ("undefined", result.valueString);
    EXPECT_TRUE(onRejected.isNull());

    EXPECT_TRUE(result2.isSet);
    EXPECT_TRUE(result2.isDone);
    EXPECT_EQ("undefined", result2.valueString);
    EXPECT_TRUE(onRejected2.isNull());
}

TEST_F(ReadableStreamReaderTest, PendingReadsShouldBeRejectedWhenErrored)
{
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);
    EXPECT_EQ(ReadableStream::Readable, m_stream->stateInternal());

    String onFulfilled, onFulfilled2;
    String onRejected, onRejected2;
    reader->read(scriptState()).then(createCaptor(&onFulfilled), createCaptor(&onRejected));
    reader->read(scriptState()).then(createCaptor(&onFulfilled2), createCaptor(&onRejected2));

    isolate()->RunMicrotasks();
    EXPECT_TRUE(onFulfilled.isNull());
    EXPECT_TRUE(onRejected.isNull());
    EXPECT_TRUE(onFulfilled2.isNull());
    EXPECT_TRUE(onRejected2.isNull());

    m_stream->error(DOMException::create(SyntaxError, "some error"));
    EXPECT_TRUE(onFulfilled.isNull());
    EXPECT_TRUE(onRejected.isNull());
    EXPECT_TRUE(onFulfilled2.isNull());
    EXPECT_TRUE(onRejected2.isNull());

    isolate()->RunMicrotasks();

    EXPECT_TRUE(onFulfilled.isNull());
    EXPECT_EQ(onRejected, "SyntaxError: some error");
    EXPECT_TRUE(onFulfilled2.isNull());
    EXPECT_EQ(onRejected2, "SyntaxError: some error");
}

TEST_F(ReadableStreamReaderTest, PendingReadsShouldBeResolvedWhenCanceled)
{
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);
    EXPECT_EQ(ReadableStream::Readable, m_stream->stateInternal());

    ReadResult result, result2;
    String onRejected, onRejected2;
    reader->read(scriptState()).then(createResultCaptor(&result), createCaptor(&onRejected));
    reader->read(scriptState()).then(createResultCaptor(&result2), createCaptor(&onRejected2));

    isolate()->RunMicrotasks();
    EXPECT_FALSE(result.isSet);
    EXPECT_TRUE(onRejected.isNull());
    EXPECT_FALSE(result2.isSet);
    EXPECT_TRUE(onRejected2.isNull());

    reader->cancel(scriptState(), ScriptValue(scriptState(), v8::Undefined(isolate())));
    EXPECT_FALSE(reader->isActive());
    EXPECT_FALSE(result.isSet);
    EXPECT_TRUE(onRejected.isNull());
    EXPECT_FALSE(result2.isSet);
    EXPECT_TRUE(onRejected2.isNull());

    isolate()->RunMicrotasks();

    EXPECT_TRUE(result.isSet);
    EXPECT_TRUE(result.isDone);
    EXPECT_EQ("undefined", result.valueString);
    EXPECT_TRUE(onRejected.isNull());

    EXPECT_TRUE(result2.isSet);
    EXPECT_TRUE(result2.isDone);
    EXPECT_EQ("undefined", result2.valueString);
    EXPECT_TRUE(onRejected2.isNull());
}

TEST_F(ReadableStreamReaderTest, CancelShouldNotWorkWhenNotActive)
{
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);
    reader->releaseLock(m_exceptionState);
    EXPECT_FALSE(reader->isActive());

    String onFulfilled, onRejected;
    reader->cancel(scriptState(), ScriptValue(scriptState(), v8::Undefined(isolate()))).then(createCaptor(&onFulfilled), createCaptor(&onRejected));
    EXPECT_EQ(ReadableStream::Readable, m_stream->stateInternal());

    EXPECT_TRUE(onFulfilled.isNull());
    EXPECT_TRUE(onRejected.isNull());

    isolate()->RunMicrotasks();

    EXPECT_EQ("undefined", onFulfilled);
    EXPECT_TRUE(onRejected.isNull());
    EXPECT_EQ(ReadableStream::Readable, m_stream->stateInternal());
}

TEST_F(ReadableStreamReaderTest, Cancel)
{
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);
    EXPECT_EQ(ReadableStream::Readable, m_stream->stateInternal());

    String onClosedFulfilled, onClosedRejected;
    String onCancelFulfilled, onCancelRejected;
    reader->closed(scriptState()).then(createCaptor(&onClosedFulfilled), createCaptor(&onClosedRejected));
    reader->cancel(scriptState(), ScriptValue(scriptState(), v8::Undefined(isolate()))).then(createCaptor(&onCancelFulfilled), createCaptor(&onCancelRejected));

    EXPECT_EQ(ReadableStream::Closed, m_stream->stateInternal());
    EXPECT_TRUE(onClosedFulfilled.isNull());
    EXPECT_TRUE(onClosedRejected.isNull());
    EXPECT_TRUE(onCancelFulfilled.isNull());
    EXPECT_TRUE(onCancelRejected.isNull());

    isolate()->RunMicrotasks();
    EXPECT_EQ("undefined", onClosedFulfilled);
    EXPECT_TRUE(onClosedRejected.isNull());
    EXPECT_EQ("undefined", onCancelFulfilled);
    EXPECT_TRUE(onCancelRejected.isNull());
}

TEST_F(ReadableStreamReaderTest, Close)
{
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);
    EXPECT_EQ(ReadableStream::Readable, m_stream->stateInternal());

    String onFulfilled, onRejected;
    reader->closed(scriptState()).then(createCaptor(&onFulfilled), createCaptor(&onRejected));

    m_stream->close();

    EXPECT_EQ(ReadableStream::Closed, m_stream->stateInternal());
    EXPECT_TRUE(onFulfilled.isNull());
    EXPECT_TRUE(onRejected.isNull());

    isolate()->RunMicrotasks();
    EXPECT_EQ("undefined", onFulfilled);
    EXPECT_TRUE(onRejected.isNull());
}

TEST_F(ReadableStreamReaderTest, Error)
{
    ReadableStreamReader* reader = new ReadableStreamReader(executionContext(), m_stream);
    EXPECT_EQ(ReadableStream::Readable, m_stream->stateInternal());

    String onFulfilled, onRejected;
    reader->closed(scriptState()).then(createCaptor(&onFulfilled), createCaptor(&onRejected));

    m_stream->error(DOMException::create(SyntaxError, "some error"));

    EXPECT_EQ(ReadableStream::Errored, m_stream->stateInternal());
    EXPECT_TRUE(onFulfilled.isNull());
    EXPECT_TRUE(onRejected.isNull());

    isolate()->RunMicrotasks();
    EXPECT_TRUE(onFulfilled.isNull());
    EXPECT_EQ("SyntaxError: some error", onRejected);
}

} // namespace

} // namespace blink
