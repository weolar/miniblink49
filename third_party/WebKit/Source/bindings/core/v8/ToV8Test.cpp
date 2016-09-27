// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/ToV8.h"

#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8BindingForTesting.h"
#include "core/testing/GarbageCollectedScriptWrappable.h"
#include "core/testing/RefCountedScriptWrappable.h"
#include "platform/heap/Heap.h"
#include "wtf/Vector.h"
#include <gtest/gtest.h>
#include <limits>

#define TEST_TOV8(expected, value) testToV8(expected, value, __FILE__, __LINE__)

namespace blink {

namespace {

class ToV8Test : public ::testing::Test {
public:
    ToV8Test() : m_scope(v8::Isolate::GetCurrent()) { }

    template<typename T>
    void testToV8(const char* expected, T value, const char* path, int lineNumber)
    {
        v8::Local<v8::Value> actual = toV8(value, m_scope.scriptState()->context()->Global(), m_scope.isolate());
        if (actual.IsEmpty()) {
            ADD_FAILURE_AT(path, lineNumber) << "toV8 returns an empty value.";
            return;
        }
        String actualString = toCoreString(actual->ToString(m_scope.context()).ToLocalChecked());
        if (String(expected) != actualString) {
            ADD_FAILURE_AT(path, lineNumber) << "toV8 returns an incorrect value.\n  Actual: " << actualString.utf8().data() << "\nExpected: " << expected;
            return;
        }
    }

    V8TestingScope m_scope;
};

class GarbageCollectedHolder : public GarbageCollectedFinalized<GarbageCollectedHolder> {
public:
    GarbageCollectedHolder(GarbageCollectedScriptWrappable* scriptWrappable)
        : m_scriptWrappable(scriptWrappable) { }

    DEFINE_INLINE_TRACE() { visitor->trace(m_scriptWrappable); }

    // This should be public in order to access a Member<X> object.
    Member<GarbageCollectedScriptWrappable> m_scriptWrappable;
};

class OffHeapGarbageCollectedHolder {
public:
    OffHeapGarbageCollectedHolder(GarbageCollectedScriptWrappable* scriptWrappable)
        : m_scriptWrappable(scriptWrappable) { }

    // This should be public in order to access a Persistent<X> object.
    Persistent<GarbageCollectedScriptWrappable> m_scriptWrappable;
};

TEST_F(ToV8Test, refCountedScriptWrappable)
{
    RefPtr<RefCountedScriptWrappable> object = RefCountedScriptWrappable::create("hello");

    TEST_TOV8("hello", object);
    TEST_TOV8("hello", object.get());
    TEST_TOV8("hello", object.release());

    ASSERT_FALSE(object);

    TEST_TOV8("null", object);
    TEST_TOV8("null", object.get());
    TEST_TOV8("null", object.release());
}

TEST_F(ToV8Test, garbageCollectedScriptWrappable)
{
    GarbageCollectedScriptWrappable* object = new GarbageCollectedScriptWrappable("world");
    GarbageCollectedHolder holder(object);
    OffHeapGarbageCollectedHolder offHeapHolder(object);

    TEST_TOV8("world", object);
    TEST_TOV8("world", RawPtr<GarbageCollectedScriptWrappable>(object));
    TEST_TOV8("world", holder.m_scriptWrappable);
    TEST_TOV8("world", offHeapHolder.m_scriptWrappable);

    object = nullptr;
    holder.m_scriptWrappable = nullptr;
    offHeapHolder.m_scriptWrappable = nullptr;

    TEST_TOV8("null", object);
    TEST_TOV8("null", RawPtr<GarbageCollectedScriptWrappable>(object));
    TEST_TOV8("null", holder.m_scriptWrappable);
    TEST_TOV8("null", offHeapHolder.m_scriptWrappable);
}

TEST_F(ToV8Test, string)
{
    char arrayString[] = "arrayString";
    const char constArrayString[] = "constArrayString";
    TEST_TOV8("arrayString", arrayString);
    TEST_TOV8("constArrayString", constArrayString);
    TEST_TOV8("pointer", const_cast<char*>("pointer"));
    TEST_TOV8("constPointer", static_cast<const char*>("constPointer"));
    TEST_TOV8("coreString", String("coreString"));
    TEST_TOV8("atomicString", AtomicString("atomicString"));

    // Null strings are converted to empty strings.
    TEST_TOV8("", static_cast<char*>(nullptr));
    TEST_TOV8("", static_cast<const char*>(nullptr));
    TEST_TOV8("", String());
    TEST_TOV8("", AtomicString());
}

TEST_F(ToV8Test, numeric)
{
    TEST_TOV8("0", static_cast<int>(0));
    TEST_TOV8("1", static_cast<int>(1));
    TEST_TOV8("-1", static_cast<int>(-1));

    TEST_TOV8("-2", static_cast<long>(-2));
    TEST_TOV8("2", static_cast<unsigned>(2));
    TEST_TOV8("2", static_cast<unsigned long>(2));

    TEST_TOV8("-2147483648", std::numeric_limits<int32_t>::min());
    TEST_TOV8("2147483647", std::numeric_limits<int32_t>::max());
    TEST_TOV8("4294967295", std::numeric_limits<uint32_t>::max());
    // v8::Number can represent exact numbers in [-(2^53-1), 2^53-1].
    TEST_TOV8("-9007199254740991", -9007199254740991); // -(2^53-1)
    TEST_TOV8("9007199254740991", 9007199254740991); // 2^53-1

    TEST_TOV8("0.5", static_cast<double>(0.5));
    TEST_TOV8("-0.5", static_cast<float>(-0.5));
    TEST_TOV8("NaN", std::numeric_limits<double>::quiet_NaN());
    TEST_TOV8("Infinity", std::numeric_limits<double>::infinity());
    TEST_TOV8("-Infinity", -std::numeric_limits<double>::infinity());
}

TEST_F(ToV8Test, boolean)
{
    TEST_TOV8("true", true);
    TEST_TOV8("false", false);
}

TEST_F(ToV8Test, v8Value)
{
    v8::Local<v8::Value> localValue(v8::Number::New(m_scope.isolate(), 1234));
    v8::Local<v8::Value> handleValue(v8::Number::New(m_scope.isolate(), 5678));

    TEST_TOV8("1234", localValue);
    TEST_TOV8("5678", handleValue);
}

TEST_F(ToV8Test, undefinedType)
{
    TEST_TOV8("undefined", ToV8UndefinedGenerator());
}

TEST_F(ToV8Test, scriptValue)
{
    ScriptValue value(m_scope.scriptState(), v8::Number::New(m_scope.isolate(), 1234));

    TEST_TOV8("1234", value);
}

TEST_F(ToV8Test, vector)
{
    Vector<RefPtr<RefCountedScriptWrappable>> v;
    v.append(RefCountedScriptWrappable::create("foo"));
    v.append(RefCountedScriptWrappable::create("bar"));

    TEST_TOV8("foo,bar", v);
}

TEST_F(ToV8Test, stringVectors)
{
    Vector<String> stringVector;
    stringVector.append("foo");
    stringVector.append("bar");
    TEST_TOV8("foo,bar", stringVector);

    Vector<AtomicString> atomicStringVector;
    atomicStringVector.append("quux");
    atomicStringVector.append("bar");
    TEST_TOV8("quux,bar", atomicStringVector);
}

TEST_F(ToV8Test, basicTypeVectors)
{
    Vector<int> intVector;
    intVector.append(42);
    intVector.append(23);
    TEST_TOV8("42,23", intVector);

    Vector<long> longVector;
    longVector.append(31773);
    longVector.append(404);
    TEST_TOV8("31773,404", longVector);

    Vector<unsigned> unsignedVector;
    unsignedVector.append(1);
    unsignedVector.append(2);
    TEST_TOV8("1,2", unsignedVector);

    Vector<unsigned long> unsignedLongVector;
    unsignedLongVector.append(1001);
    unsignedLongVector.append(2002);
    TEST_TOV8("1001,2002", unsignedLongVector);

    Vector<float> floatVector;
    floatVector.append(0.125);
    floatVector.append(1.);
    TEST_TOV8("0.125,1", floatVector);

    Vector<double> doubleVector;
    doubleVector.append(2.3);
    doubleVector.append(4.2);
    TEST_TOV8("2.3,4.2", doubleVector);

    Vector<bool> boolVector;
    boolVector.append(true);
    boolVector.append(true);
    boolVector.append(false);
    TEST_TOV8("true,true,false", boolVector);
}

TEST_F(ToV8Test, dictionaryVector)
{
    Vector<std::pair<String, int>> dictionary;
    dictionary.append(std::make_pair("one", 1));
    dictionary.append(std::make_pair("two", 2));
    TEST_TOV8("[object Object]", dictionary);
    v8::Local<v8::Context> context = m_scope.scriptState()->context();
    v8::Local<v8::Object> result = toV8(dictionary, context->Global(), m_scope.isolate())->ToObject(context).ToLocalChecked();
    v8::Local<v8::Value> one = result->Get(context, v8String(m_scope.isolate(), "one")).ToLocalChecked();
    EXPECT_EQ(1, one->NumberValue(context).FromJust());
    v8::Local<v8::Value> two = result->Get(context, v8String(m_scope.isolate(), "two")).ToLocalChecked();
    EXPECT_EQ(2, two->NumberValue(context).FromJust());
}

TEST_F(ToV8Test, heapVector)
{
    HeapVector<Member<GarbageCollectedScriptWrappable>> v;
    v.append(new GarbageCollectedScriptWrappable("hoge"));
    v.append(new GarbageCollectedScriptWrappable("fuga"));
    v.append(nullptr);

    TEST_TOV8("hoge,fuga,", v);
}

TEST_F(ToV8Test, stringHeapVectors)
{
    HeapVector<String> stringVector;
    stringVector.append("foo");
    stringVector.append("bar");
    TEST_TOV8("foo,bar", stringVector);

    HeapVector<AtomicString> atomicStringVector;
    atomicStringVector.append("quux");
    atomicStringVector.append("bar");
    TEST_TOV8("quux,bar", atomicStringVector);
}

TEST_F(ToV8Test, basicTypeHeapVectors)
{
    HeapVector<int> intVector;
    intVector.append(42);
    intVector.append(23);
    TEST_TOV8("42,23", intVector);

    HeapVector<long> longVector;
    longVector.append(31773);
    longVector.append(404);
    TEST_TOV8("31773,404", longVector);

    HeapVector<unsigned> unsignedVector;
    unsignedVector.append(1);
    unsignedVector.append(2);
    TEST_TOV8("1,2", unsignedVector);

    HeapVector<unsigned long> unsignedLongVector;
    unsignedLongVector.append(1001);
    unsignedLongVector.append(2002);
    TEST_TOV8("1001,2002", unsignedLongVector);

    HeapVector<float> floatVector;
    floatVector.append(0.125);
    floatVector.append(1.);
    TEST_TOV8("0.125,1", floatVector);

    HeapVector<double> doubleVector;
    doubleVector.append(2.3);
    doubleVector.append(4.2);
    TEST_TOV8("2.3,4.2", doubleVector);

    HeapVector<bool> boolVector;
    boolVector.append(true);
    boolVector.append(true);
    boolVector.append(false);
    TEST_TOV8("true,true,false", boolVector);
}

} // namespace

} // namespace blink
