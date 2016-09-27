/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "bindings/modules/v8/V8BindingForModules.h"

#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8BindingForTesting.h"
#include "bindings/core/v8/V8PerIsolateData.h"
#include "bindings/modules/v8/ToV8ForModules.h"
#include "modules/indexeddb/IDBKey.h"
#include "modules/indexeddb/IDBKeyPath.h"
#include <gtest/gtest.h>

using namespace blink;

namespace {

IDBKey* checkKeyFromValueAndKeyPathInternal(v8::Isolate* isolate, const ScriptValue& value, const String& keyPath)
{
    IDBKeyPath idbKeyPath(keyPath);
    EXPECT_TRUE(idbKeyPath.isValid());

    NonThrowableExceptionState exceptionState;
    return ScriptValue::to<IDBKey*>(isolate, value, exceptionState, idbKeyPath);
}

void checkKeyPathNullValue(v8::Isolate* isolate, const ScriptValue& value, const String& keyPath)
{
    ASSERT_FALSE(checkKeyFromValueAndKeyPathInternal(isolate, value, keyPath));
}

bool injectKey(ScriptState* scriptState, IDBKey* key, ScriptValue& value, const String& keyPath)
{
    IDBKeyPath idbKeyPath(keyPath);
    EXPECT_TRUE(idbKeyPath.isValid());
    ScriptValue keyValue = ScriptValue::from(scriptState, key);
    return injectV8KeyIntoV8Value(scriptState->isolate(), keyValue.v8Value(), value.v8Value(), idbKeyPath);
}

void checkInjection(ScriptState* scriptState, IDBKey* key, ScriptValue& value, const String& keyPath)
{
    bool result = injectKey(scriptState, key, value, keyPath);
    ASSERT_TRUE(result);
    IDBKey* extractedKey = checkKeyFromValueAndKeyPathInternal(scriptState->isolate(), value, keyPath);
    EXPECT_TRUE(key->isEqual(extractedKey));
}

void checkInjectionIgnored(ScriptState* scriptState, IDBKey* key, ScriptValue& value, const String& keyPath)
{
    bool result = injectKey(scriptState, key, value, keyPath);
    ASSERT_TRUE(result);
    IDBKey* extractedKey = checkKeyFromValueAndKeyPathInternal(scriptState->isolate(), value, keyPath);
    EXPECT_FALSE(key->isEqual(extractedKey));
}

void checkInjectionDisallowed(ScriptState* scriptState, ScriptValue& value, const String& keyPath)
{
    const IDBKeyPath idbKeyPath(keyPath);
    ASSERT_TRUE(idbKeyPath.isValid());
    EXPECT_FALSE(canInjectIDBKeyIntoScriptValue(scriptState->isolate(), value, idbKeyPath));
}

void checkKeyPathStringValue(v8::Isolate* isolate, const ScriptValue& value, const String& keyPath, const String& expected)
{
    IDBKey* idbKey = checkKeyFromValueAndKeyPathInternal(isolate, value, keyPath);
    ASSERT_TRUE(idbKey);
    ASSERT_EQ(IDBKey::StringType, idbKey->type());
    ASSERT_TRUE(expected == idbKey->string());
}

void checkKeyPathNumberValue(v8::Isolate* isolate, const ScriptValue& value, const String& keyPath, int expected)
{
    IDBKey* idbKey = checkKeyFromValueAndKeyPathInternal(isolate, value, keyPath);
    ASSERT_TRUE(idbKey);
    ASSERT_EQ(IDBKey::NumberType, idbKey->type());
    ASSERT_TRUE(expected == idbKey->number());
}

class IDBKeyFromValueAndKeyPathTest : public testing::Test {
public:
    IDBKeyFromValueAndKeyPathTest()
        : m_scope(v8::Isolate::GetCurrent())
    {
    }

    ScriptState* scriptState() const { return m_scope.scriptState(); }

private:
    V8TestingScope m_scope;
};

TEST_F(IDBKeyFromValueAndKeyPathTest, TopLevelPropertyStringValue)
{
    v8::Isolate* isolate = scriptState()->isolate();

    // object = { foo: "zoo" }
    v8::Local<v8::Object> object = v8::Object::New(isolate);
    ASSERT_TRUE(v8CallBoolean(object->Set(scriptState()->context(), v8AtomicString(isolate, "foo"), v8AtomicString(isolate, "zoo"))));

    ScriptValue scriptValue(scriptState(), object);

    checkKeyPathStringValue(isolate, scriptValue, "foo", "zoo");
    checkKeyPathNullValue(isolate, scriptValue, "bar");
}

TEST_F(IDBKeyFromValueAndKeyPathTest, TopLevelPropertyNumberValue)
{
    v8::Isolate* isolate = scriptState()->isolate();

    // object = { foo: 456 }
    v8::Local<v8::Object> object = v8::Object::New(isolate);
    ASSERT_TRUE(v8CallBoolean(object->Set(scriptState()->context(), v8AtomicString(isolate, "foo"), v8::Number::New(isolate, 456))));

    ScriptValue scriptValue(scriptState(), object);

    checkKeyPathNumberValue(isolate, scriptValue, "foo", 456);
    checkKeyPathNullValue(isolate, scriptValue, "bar");
}

TEST_F(IDBKeyFromValueAndKeyPathTest, SubProperty)
{
    v8::Isolate* isolate = scriptState()->isolate();

    // object = { foo: { bar: "zee" } }
    v8::Local<v8::Object> object = v8::Object::New(isolate);
    v8::Local<v8::Object> subProperty = v8::Object::New(isolate);
    ASSERT_TRUE(v8CallBoolean(subProperty->Set(scriptState()->context(), v8AtomicString(isolate, "bar"), v8AtomicString(isolate, "zee"))));
    ASSERT_TRUE(v8CallBoolean(object->Set(scriptState()->context(), v8AtomicString(isolate, "foo"), subProperty)));

    ScriptValue scriptValue(scriptState(), object);

    checkKeyPathStringValue(isolate, scriptValue, "foo.bar", "zee");
    checkKeyPathNullValue(isolate, scriptValue, "bar");
}

class InjectIDBKeyTest : public IDBKeyFromValueAndKeyPathTest {
};

TEST_F(InjectIDBKeyTest, ImplicitValues)
{
    v8::Isolate* isolate = scriptState()->isolate();
    {
        v8::Local<v8::String> string = v8String(isolate, "string");
        ScriptValue value = ScriptValue(scriptState(), string);
        checkInjectionIgnored(scriptState(), IDBKey::createNumber(123), value, "length");
    }
    {
        v8::Local<v8::Array> array = v8::Array::New(isolate);
        ScriptValue value = ScriptValue(scriptState(), array);
        checkInjectionIgnored(scriptState(), IDBKey::createNumber(456), value, "length");
    }
}

TEST_F(InjectIDBKeyTest, TopLevelPropertyStringValue)
{
    v8::Isolate* isolate = scriptState()->isolate();

    // object = { foo: "zoo" }
    v8::Local<v8::Object> object = v8::Object::New(isolate);
    ASSERT_TRUE(v8CallBoolean(object->Set(scriptState()->context(), v8AtomicString(isolate, "foo"), v8AtomicString(isolate, "zoo"))));

    ScriptValue scriptObject(scriptState(), object);
    checkInjection(scriptState(), IDBKey::createString("myNewKey"), scriptObject, "bar");
    checkInjection(scriptState(), IDBKey::createNumber(1234), scriptObject, "bar");

    checkInjectionDisallowed(scriptState(), scriptObject, "foo.bar");
}

TEST_F(InjectIDBKeyTest, SubProperty)
{
    v8::Isolate* isolate = scriptState()->isolate();

    // object = { foo: { bar: "zee" } }
    v8::Local<v8::Object> object = v8::Object::New(isolate);
    v8::Local<v8::Object> subProperty = v8::Object::New(isolate);
    ASSERT_TRUE(v8CallBoolean(subProperty->Set(scriptState()->context(), v8AtomicString(isolate, "bar"), v8AtomicString(isolate, "zee"))));
    ASSERT_TRUE(v8CallBoolean(object->Set(scriptState()->context(), v8AtomicString(isolate, "foo"), subProperty)));

    ScriptValue scriptObject(scriptState(), object);
    checkInjection(scriptState(), IDBKey::createString("myNewKey"), scriptObject, "foo.baz");
    checkInjection(scriptState(), IDBKey::createNumber(789), scriptObject, "foo.baz");
    checkInjection(scriptState(), IDBKey::createDate(4567), scriptObject, "foo.baz");
    checkInjection(scriptState(), IDBKey::createDate(4567), scriptObject, "bar");
    checkInjection(scriptState(), IDBKey::createArray(IDBKey::KeyArray()), scriptObject, "foo.baz");
    checkInjection(scriptState(), IDBKey::createArray(IDBKey::KeyArray()), scriptObject, "bar");

    checkInjectionDisallowed(scriptState(), scriptObject, "foo.bar.baz");
    checkInjection(scriptState(), IDBKey::createString("zoo"), scriptObject, "foo.xyz.foo");
}

} // namespace
