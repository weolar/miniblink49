// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/converter.h"

#include <limits.h>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "gin/public/isolate_holder.h"
#include "gin/test/v8_test.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "v8/include/v8.h"

namespace gin {

using v8::Array;
using v8::Boolean;
using v8::HandleScope;
using v8::Integer;
using v8::Local;
using v8::Null;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Undefined;
using v8::Value;

typedef V8Test ConverterTest;

TEST_F(ConverterTest, Bool) {
  HandleScope handle_scope(instance_->isolate());

  EXPECT_TRUE(Converter<bool>::ToV8(instance_->isolate(), true)->StrictEquals(
      Boolean::New(instance_->isolate(), true)));
  EXPECT_TRUE(Converter<bool>::ToV8(instance_->isolate(), false)->StrictEquals(
      Boolean::New(instance_->isolate(), false)));

  struct {
    Local<Value> input;
    bool expected;
  } test_data[] = {
    { Boolean::New(instance_->isolate(), false).As<Value>(), false },
    { Boolean::New(instance_->isolate(), true).As<Value>(), true },
    { Number::New(instance_->isolate(), 0).As<Value>(), false },
    { Number::New(instance_->isolate(), 1).As<Value>(), true },
    { Number::New(instance_->isolate(), -1).As<Value>(), true },
    { Number::New(instance_->isolate(), 0.1).As<Value>(), true },
    { String::NewFromUtf8(instance_->isolate(), "").As<Value>(), false },
    { String::NewFromUtf8(instance_->isolate(), "foo").As<Value>(), true },
    { Object::New(instance_->isolate()).As<Value>(), true },
    { Null(instance_->isolate()).As<Value>(), false },
    { Undefined(instance_->isolate()).As<Value>(), false },
  };

  for (size_t i = 0; i < arraysize(test_data); ++i) {
    bool result = false;
    EXPECT_TRUE(Converter<bool>::FromV8(instance_->isolate(),
                                        test_data[i].input, &result));
    EXPECT_EQ(test_data[i].expected, result);

    result = true;
    EXPECT_TRUE(Converter<bool>::FromV8(instance_->isolate(),
                                        test_data[i].input, &result));
    EXPECT_EQ(test_data[i].expected, result);
  }
}

TEST_F(ConverterTest, Int32) {
  HandleScope handle_scope(instance_->isolate());

  int test_data_to[] = {-1, 0, 1};
  for (size_t i = 0; i < arraysize(test_data_to); ++i) {
    EXPECT_TRUE(Converter<int32_t>::ToV8(instance_->isolate(), test_data_to[i])
                    ->StrictEquals(
                          Integer::New(instance_->isolate(), test_data_to[i])));
  }

  struct {
    v8::Local<v8::Value> input;
    bool expect_sucess;
    int expected_result;
  } test_data_from[] = {
    { Boolean::New(instance_->isolate(), false).As<Value>(), false, 0 },
    { Boolean::New(instance_->isolate(), true).As<Value>(), false, 0 },
    { Integer::New(instance_->isolate(), -1).As<Value>(), true, -1 },
    { Integer::New(instance_->isolate(), 0).As<Value>(), true, 0 },
    { Integer::New(instance_->isolate(), 1).As<Value>(), true, 1 },
    { Number::New(instance_->isolate(), -1).As<Value>(), true, -1 },
    { Number::New(instance_->isolate(), 1.1).As<Value>(), false, 0 },
    { String::NewFromUtf8(instance_->isolate(), "42").As<Value>(), false, 0 },
    { String::NewFromUtf8(instance_->isolate(), "foo").As<Value>(), false, 0 },
    { Object::New(instance_->isolate()).As<Value>(), false, 0 },
    { Array::New(instance_->isolate()).As<Value>(), false, 0 },
    { v8::Null(instance_->isolate()).As<Value>(), false, 0 },
    { v8::Undefined(instance_->isolate()).As<Value>(), false, 0 },
  };

  for (size_t i = 0; i < arraysize(test_data_from); ++i) {
    int32_t result = std::numeric_limits<int32_t>::min();
    bool success = Converter<int32_t>::FromV8(instance_->isolate(),
                                              test_data_from[i].input, &result);
    EXPECT_EQ(test_data_from[i].expect_sucess, success) << i;
    if (success)
      EXPECT_EQ(test_data_from[i].expected_result, result) << i;
  }
}

TEST_F(ConverterTest, Vector) {
  HandleScope handle_scope(instance_->isolate());

  std::vector<int> expected;
  expected.push_back(-1);
  expected.push_back(0);
  expected.push_back(1);

  auto maybe = Converter<std::vector<int>>::ToV8(
      instance_->isolate()->GetCurrentContext(), expected);
  Local<Value> js_value;
  EXPECT_TRUE(maybe.ToLocal(&js_value));
  Local<Array> js_array2 = Local<Array>::Cast(js_value);
  EXPECT_EQ(3u, js_array2->Length());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_TRUE(Integer::New(instance_->isolate(), expected[i])
                    ->StrictEquals(js_array2->Get(static_cast<int>(i))));
  }
}

}  // namespace gin
