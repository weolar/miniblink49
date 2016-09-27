// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"

#include "bindings/core/v8/ScriptFunction.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/V8Binding.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/testing/DummyPageHolder.h"

#include <gtest/gtest.h>
#include <v8.h>

namespace blink {

namespace {

void callback(const v8::FunctionCallbackInfo<v8::Value>& info) { }

class Function : public ScriptFunction {
public:
    static v8::Local<v8::Function> createFunction(ScriptState* scriptState, String* value)
    {
        Function* self = new Function(scriptState, value);
        return self->bindToV8Function();
    }

private:
    Function(ScriptState* scriptState, String* value)
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

class ScriptPromiseResolverTest : public ::testing::Test {
public:
    ScriptPromiseResolverTest()
        : m_pageHolder(DummyPageHolder::create())
    {
    }

    ~ScriptPromiseResolverTest() override
    {
        ScriptState::Scope scope(scriptState());
        // FIXME: We put this statement here to clear an exception from the
        // isolate.
        createClosure(callback, v8::Undefined(isolate()), isolate());

        // Execute all pending microtasks
        isolate()->RunMicrotasks();
    }

    OwnPtr<DummyPageHolder> m_pageHolder;
    ScriptState* scriptState() const { return ScriptState::forMainWorld(&m_pageHolder->frame()); }
    ExecutionContext* executionContext() const { return &m_pageHolder->document(); }
    v8::Isolate* isolate() const { return scriptState()->isolate(); }
};

TEST_F(ScriptPromiseResolverTest, construct)
{
    ASSERT_FALSE(executionContext()->activeDOMObjectsAreStopped());
    ScriptState::Scope scope(scriptState());
    ScriptPromiseResolver::create(scriptState());
}

TEST_F(ScriptPromiseResolverTest, resolve)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = nullptr;
    ScriptPromise promise;
    {
        ScriptState::Scope scope(scriptState());
        resolver = ScriptPromiseResolver::create(scriptState());
        promise = resolver->promise();
    }

    String onFulfilled, onRejected;
    ASSERT_FALSE(promise.isEmpty());
    {
        ScriptState::Scope scope(scriptState());
        promise.then(Function::createFunction(scriptState(), &onFulfilled), Function::createFunction(scriptState(), &onRejected));
    }

    EXPECT_EQ(String(), onFulfilled);
    EXPECT_EQ(String(), onRejected);

    isolate()->RunMicrotasks();

    EXPECT_EQ(String(), onFulfilled);
    EXPECT_EQ(String(), onRejected);

    resolver->resolve("hello");

    {
        ScriptState::Scope scope(scriptState());
        EXPECT_TRUE(resolver->promise().isEmpty());
    }

    EXPECT_EQ(String(), onFulfilled);
    EXPECT_EQ(String(), onRejected);

    isolate()->RunMicrotasks();

    EXPECT_EQ("hello", onFulfilled);
    EXPECT_EQ(String(), onRejected);

    resolver->resolve("bye");
    resolver->reject("bye");
    isolate()->RunMicrotasks();

    EXPECT_EQ("hello", onFulfilled);
    EXPECT_EQ(String(), onRejected);
}

TEST_F(ScriptPromiseResolverTest, reject)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = nullptr;
    ScriptPromise promise;
    {
        ScriptState::Scope scope(scriptState());
        resolver = ScriptPromiseResolver::create(scriptState());
        promise = resolver->promise();
    }

    String onFulfilled, onRejected;
    ASSERT_FALSE(promise.isEmpty());
    {
        ScriptState::Scope scope(scriptState());
        promise.then(Function::createFunction(scriptState(), &onFulfilled), Function::createFunction(scriptState(), &onRejected));
    }

    EXPECT_EQ(String(), onFulfilled);
    EXPECT_EQ(String(), onRejected);

    isolate()->RunMicrotasks();

    EXPECT_EQ(String(), onFulfilled);
    EXPECT_EQ(String(), onRejected);

    resolver->reject("hello");

    {
        ScriptState::Scope scope(scriptState());
        EXPECT_TRUE(resolver->promise().isEmpty());
    }

    EXPECT_EQ(String(), onFulfilled);
    EXPECT_EQ(String(), onRejected);

    isolate()->RunMicrotasks();

    EXPECT_EQ(String(), onFulfilled);
    EXPECT_EQ("hello", onRejected);

    resolver->resolve("bye");
    resolver->reject("bye");
    isolate()->RunMicrotasks();

    EXPECT_EQ(String(), onFulfilled);
    EXPECT_EQ("hello", onRejected);
}

TEST_F(ScriptPromiseResolverTest, stop)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = nullptr;
    ScriptPromise promise;
    {
        ScriptState::Scope scope(scriptState());
        resolver = ScriptPromiseResolver::create(scriptState());
        promise = resolver->promise();
    }

    String onFulfilled, onRejected;
    ASSERT_FALSE(promise.isEmpty());
    {
        ScriptState::Scope scope(scriptState());
        promise.then(Function::createFunction(scriptState(), &onFulfilled), Function::createFunction(scriptState(), &onRejected));
    }

    executionContext()->stopActiveDOMObjects();
    {
        ScriptState::Scope scope(scriptState());
        EXPECT_TRUE(resolver->promise().isEmpty());
    }

    resolver->resolve("hello");
    isolate()->RunMicrotasks();

    EXPECT_EQ(String(), onFulfilled);
    EXPECT_EQ(String(), onRejected);
}

#if !ENABLE(OILPAN)
TEST_F(ScriptPromiseResolverTest, keepAliveUntilResolved)
{
    RefPtr<ScriptPromiseResolver> resolver;
    {
        ScriptState::Scope scope(scriptState());
        resolver = ScriptPromiseResolver::create(scriptState());
    }
    EXPECT_EQ(1, resolver->refCount());
    resolver->keepAliveWhilePending();
    EXPECT_EQ(2, resolver->refCount());

    resolver->resolve("hello");
    EXPECT_EQ(1, resolver->refCount());
}

TEST_F(ScriptPromiseResolverTest, keepAliveUntilRejected)
{
    RefPtr<ScriptPromiseResolver> resolver;
    {
        ScriptState::Scope scope(scriptState());
        resolver = ScriptPromiseResolver::create(scriptState());
    }
    EXPECT_EQ(1, resolver->refCount());
    resolver->keepAliveWhilePending();
    EXPECT_EQ(2, resolver->refCount());

    resolver->reject("hello");
    EXPECT_EQ(1, resolver->refCount());
}

TEST_F(ScriptPromiseResolverTest, keepAliveUntilStopped)
{
    RefPtr<ScriptPromiseResolver> resolver;
    {
        ScriptState::Scope scope(scriptState());
        resolver = ScriptPromiseResolver::create(scriptState());
    }
    EXPECT_EQ(1, resolver->refCount());
    resolver->keepAliveWhilePending();
    EXPECT_EQ(2, resolver->refCount());

    executionContext()->stopActiveDOMObjects();
    EXPECT_EQ(1, resolver->refCount());
}

TEST_F(ScriptPromiseResolverTest, suspend)
{
    RefPtr<ScriptPromiseResolver> resolver;
    {
        ScriptState::Scope scope(scriptState());
        resolver = ScriptPromiseResolver::create(scriptState());
    }
    EXPECT_EQ(1, resolver->refCount());
    resolver->keepAliveWhilePending();
    EXPECT_EQ(2, resolver->refCount());
    executionContext()->suspendActiveDOMObjects();
    resolver->resolve("hello");
    EXPECT_EQ(3, resolver->refCount());

    executionContext()->stopActiveDOMObjects();
    EXPECT_EQ(1, resolver->refCount());
}
#endif

TEST_F(ScriptPromiseResolverTest, resolveVoid)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = nullptr;
    ScriptPromise promise;
    {
        ScriptState::Scope scope(scriptState());
        resolver = ScriptPromiseResolver::create(scriptState());
        promise = resolver->promise();
    }

    String onFulfilled, onRejected;
    ASSERT_FALSE(promise.isEmpty());
    {
        ScriptState::Scope scope(scriptState());
        promise.then(Function::createFunction(scriptState(), &onFulfilled), Function::createFunction(scriptState(), &onRejected));
    }

    resolver->resolve();
    isolate()->RunMicrotasks();

    EXPECT_EQ("undefined", onFulfilled);
    EXPECT_EQ(String(), onRejected);
}

TEST_F(ScriptPromiseResolverTest, rejectVoid)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = nullptr;
    ScriptPromise promise;
    {
        ScriptState::Scope scope(scriptState());
        resolver = ScriptPromiseResolver::create(scriptState());
        promise = resolver->promise();
    }

    String onFulfilled, onRejected;
    ASSERT_FALSE(promise.isEmpty());
    {
        ScriptState::Scope scope(scriptState());
        promise.then(Function::createFunction(scriptState(), &onFulfilled), Function::createFunction(scriptState(), &onRejected));
    }

    resolver->reject();
    isolate()->RunMicrotasks();

    EXPECT_EQ(String(), onFulfilled);
    EXPECT_EQ("undefined", onRejected);
}

} // namespace

} // namespace blink
