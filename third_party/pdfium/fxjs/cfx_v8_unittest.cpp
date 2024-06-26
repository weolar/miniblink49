// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxjs/cfx_v8_unittest.h"

#include <memory>

#include "fxjs/cfx_v8.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"
#include "third_party/base/ptr_util.h"

void FXV8UnitTest::V8IsolateDeleter::operator()(v8::Isolate* ptr) const {
  ptr->Dispose();
}

FXV8UnitTest::FXV8UnitTest() = default;

FXV8UnitTest::~FXV8UnitTest() = default;

void FXV8UnitTest::SetUp() {
  array_buffer_allocator_ = pdfium::MakeUnique<CFX_V8ArrayBufferAllocator>();

  v8::Isolate::CreateParams params;
  params.array_buffer_allocator = array_buffer_allocator_.get();
  isolate_.reset(v8::Isolate::New(params));

  cfx_v8_ = pdfium::MakeUnique<CFX_V8>(isolate_.get());
}

TEST_F(FXV8UnitTest, EmptyLocal) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());
  v8::Context::Scope context_scope(v8::Context::New(isolate()));

  v8::Local<v8::Value> empty;
  EXPECT_FALSE(cfx_v8()->ToBoolean(empty));
  EXPECT_EQ(0, cfx_v8()->ToInt32(empty));
  EXPECT_EQ(0.0, cfx_v8()->ToDouble(empty));
  EXPECT_EQ("", cfx_v8()->ToByteString(empty));
  EXPECT_EQ(L"", cfx_v8()->ToWideString(empty));
  EXPECT_TRUE(cfx_v8()->ToObject(empty).IsEmpty());
  EXPECT_TRUE(cfx_v8()->ToArray(empty).IsEmpty());
}

TEST_F(FXV8UnitTest, NewNull) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());
  v8::Context::Scope context_scope(v8::Context::New(isolate()));

  auto nullz = cfx_v8()->NewNull();
  EXPECT_FALSE(cfx_v8()->ToBoolean(nullz));
  EXPECT_EQ(0, cfx_v8()->ToInt32(nullz));
  EXPECT_EQ(0.0, cfx_v8()->ToDouble(nullz));
  EXPECT_EQ("null", cfx_v8()->ToByteString(nullz));
  EXPECT_EQ(L"null", cfx_v8()->ToWideString(nullz));
  EXPECT_TRUE(cfx_v8()->ToObject(nullz).IsEmpty());
  EXPECT_TRUE(cfx_v8()->ToArray(nullz).IsEmpty());
}

TEST_F(FXV8UnitTest, NewUndefined) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());
  v8::Context::Scope context_scope(v8::Context::New(isolate()));

  auto undef = cfx_v8()->NewUndefined();
  EXPECT_FALSE(cfx_v8()->ToBoolean(undef));
  EXPECT_EQ(0, cfx_v8()->ToInt32(undef));
  EXPECT_TRUE(std::isnan(cfx_v8()->ToDouble(undef)));
  EXPECT_EQ("undefined", cfx_v8()->ToByteString(undef));
  EXPECT_EQ(L"undefined", cfx_v8()->ToWideString(undef));
  EXPECT_TRUE(cfx_v8()->ToObject(undef).IsEmpty());
  EXPECT_TRUE(cfx_v8()->ToArray(undef).IsEmpty());
}

TEST_F(FXV8UnitTest, NewBoolean) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());
  v8::Context::Scope context_scope(v8::Context::New(isolate()));

  auto boolz = cfx_v8()->NewBoolean(true);
  EXPECT_TRUE(cfx_v8()->ToBoolean(boolz));
  EXPECT_EQ(1, cfx_v8()->ToInt32(boolz));
  EXPECT_EQ(1.0, cfx_v8()->ToDouble(boolz));
  EXPECT_EQ("true", cfx_v8()->ToByteString(boolz));
  EXPECT_EQ(L"true", cfx_v8()->ToWideString(boolz));
  EXPECT_TRUE(cfx_v8()->ToObject(boolz).IsEmpty());
  EXPECT_TRUE(cfx_v8()->ToArray(boolz).IsEmpty());

  boolz = cfx_v8()->NewBoolean(false);
  EXPECT_FALSE(cfx_v8()->ToBoolean(boolz));
  EXPECT_EQ(0, cfx_v8()->ToInt32(boolz));
  EXPECT_EQ(0.0, cfx_v8()->ToDouble(boolz));
  EXPECT_EQ("false", cfx_v8()->ToByteString(boolz));
  EXPECT_EQ(L"false", cfx_v8()->ToWideString(boolz));
  EXPECT_TRUE(cfx_v8()->ToObject(boolz).IsEmpty());
  EXPECT_TRUE(cfx_v8()->ToArray(boolz).IsEmpty());
}

TEST_F(FXV8UnitTest, NewNumber) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());
  v8::Context::Scope context_scope(v8::Context::New(isolate()));

  auto num = cfx_v8()->NewNumber(42.1);
  EXPECT_TRUE(cfx_v8()->ToBoolean(num));
  EXPECT_EQ(42, cfx_v8()->ToInt32(num));
  EXPECT_EQ(42.1, cfx_v8()->ToDouble(num));
  EXPECT_EQ("42.1", cfx_v8()->ToByteString(num));
  EXPECT_EQ(L"42.1", cfx_v8()->ToWideString(num));
  EXPECT_TRUE(cfx_v8()->ToObject(num).IsEmpty());
  EXPECT_TRUE(cfx_v8()->ToArray(num).IsEmpty());
}

TEST_F(FXV8UnitTest, NewString) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());
  v8::Context::Scope context_scope(v8::Context::New(isolate()));

  auto str = cfx_v8()->NewString("123");
  EXPECT_TRUE(cfx_v8()->ToBoolean(str));
  EXPECT_EQ(123, cfx_v8()->ToInt32(str));
  EXPECT_EQ(123, cfx_v8()->ToDouble(str));
  EXPECT_EQ("123", cfx_v8()->ToByteString(str));
  EXPECT_EQ(L"123", cfx_v8()->ToWideString(str));
  EXPECT_TRUE(cfx_v8()->ToObject(str).IsEmpty());
  EXPECT_TRUE(cfx_v8()->ToArray(str).IsEmpty());

  auto str2 = cfx_v8()->NewString(L"123");
  EXPECT_TRUE(cfx_v8()->ToBoolean(str2));
  EXPECT_EQ(123, cfx_v8()->ToInt32(str2));
  EXPECT_EQ(123, cfx_v8()->ToDouble(str2));
  EXPECT_EQ("123", cfx_v8()->ToByteString(str2));
  EXPECT_EQ(L"123", cfx_v8()->ToWideString(str2));
  EXPECT_TRUE(cfx_v8()->ToObject(str2).IsEmpty());
  EXPECT_TRUE(cfx_v8()->ToArray(str2).IsEmpty());
}

TEST_F(FXV8UnitTest, NewDate) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());
  v8::Context::Scope context_scope(v8::Context::New(isolate()));

  auto date = cfx_v8()->NewDate(1111111111);
  EXPECT_TRUE(cfx_v8()->ToBoolean(date));
  EXPECT_EQ(1111111111, cfx_v8()->ToInt32(date));
  EXPECT_EQ(1111111111.0, cfx_v8()->ToDouble(date));
  EXPECT_NE("", cfx_v8()->ToByteString(date));   // exact format varies.
  EXPECT_NE(L"", cfx_v8()->ToWideString(date));  // exact format varies.
  EXPECT_TRUE(cfx_v8()->ToObject(date)->IsObject());
  EXPECT_TRUE(cfx_v8()->ToArray(date).IsEmpty());
}

TEST_F(FXV8UnitTest, NewArray) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());
  v8::Context::Scope context_scope(v8::Context::New(isolate()));

  auto array = cfx_v8()->NewArray();
  EXPECT_EQ(0u, cfx_v8()->GetArrayLength(array));
  EXPECT_FALSE(cfx_v8()->GetArrayElement(array, 2).IsEmpty());
  EXPECT_TRUE(cfx_v8()->GetArrayElement(array, 2)->IsUndefined());
  EXPECT_EQ(0u, cfx_v8()->GetArrayLength(array));

  cfx_v8()->PutArrayElement(array, 3, cfx_v8()->NewNumber(12));
  EXPECT_FALSE(cfx_v8()->GetArrayElement(array, 2).IsEmpty());
  EXPECT_TRUE(cfx_v8()->GetArrayElement(array, 2)->IsUndefined());
  EXPECT_FALSE(cfx_v8()->GetArrayElement(array, 3).IsEmpty());
  EXPECT_TRUE(cfx_v8()->GetArrayElement(array, 3)->IsNumber());
  EXPECT_EQ(4u, cfx_v8()->GetArrayLength(array));

  EXPECT_TRUE(cfx_v8()->ToBoolean(array));
  EXPECT_EQ(0, cfx_v8()->ToInt32(array));
  double d = cfx_v8()->ToDouble(array);
  EXPECT_NE(d, d);  // i.e. NaN.
  EXPECT_EQ(L",,,12", cfx_v8()->ToWideString(array));
  EXPECT_TRUE(cfx_v8()->ToObject(array)->IsObject());
  EXPECT_TRUE(cfx_v8()->ToArray(array)->IsArray());
}

TEST_F(FXV8UnitTest, NewObject) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());
  v8::Context::Scope context_scope(v8::Context::New(isolate()));

  auto object = cfx_v8()->NewObject();
  ASSERT_FALSE(object.IsEmpty());
  EXPECT_EQ(0u, cfx_v8()->GetObjectPropertyNames(object).size());
  EXPECT_FALSE(cfx_v8()->GetObjectProperty(object, "clams").IsEmpty());
  EXPECT_TRUE(cfx_v8()->GetObjectProperty(object, "clams")->IsUndefined());
  EXPECT_EQ(0u, cfx_v8()->GetObjectPropertyNames(object).size());

  cfx_v8()->PutObjectProperty(object, "clams", cfx_v8()->NewNumber(12));
  EXPECT_FALSE(cfx_v8()->GetObjectProperty(object, "clams").IsEmpty());
  EXPECT_TRUE(cfx_v8()->GetObjectProperty(object, "clams")->IsNumber());
  EXPECT_EQ(1u, cfx_v8()->GetObjectPropertyNames(object).size());
  EXPECT_EQ(L"clams", cfx_v8()->GetObjectPropertyNames(object)[0]);

  EXPECT_TRUE(cfx_v8()->ToBoolean(object));
  EXPECT_EQ(0, cfx_v8()->ToInt32(object));
  double d = cfx_v8()->ToDouble(object);
  EXPECT_NE(d, d);  // i.e. NaN.
  EXPECT_EQ(L"[object Object]", cfx_v8()->ToWideString(object));
  EXPECT_TRUE(cfx_v8()->ToObject(object)->IsObject());
  EXPECT_TRUE(cfx_v8()->ToArray(object).IsEmpty());
}
