// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/test/gtest.h"

#include "base/bind.h"
#include "base/logging.h"
#include "gin/arguments.h"
#include "gin/converter.h"
#include "gin/function_template.h"
#include "gin/object_template_builder.h"
#include "gin/per_isolate_data.h"
#include "gin/public/wrapper_info.h"
#include "gin/wrappable.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace gin {

namespace {

void Fail(const std::string& description) {
  FAIL() << description;
}

void ExpectTrue(bool condition, const std::string& description) {
  EXPECT_TRUE(condition) << description;
}

void ExpectFalse(bool condition, const std::string& description) {
  EXPECT_FALSE(condition) << description;
}

void ExpectEqual(const v8::Local<v8::Value> expected,
                 const v8::Local<v8::Value> actual,
                 const std::string& description) {
  EXPECT_TRUE(expected->StrictEquals(actual)) << description;
}

WrapperInfo g_wrapper_info = { kEmbedderNativeGin };

}  // namespace

const char GTest::kModuleName[] = "gtest";

v8::Local<v8::Value> GTest::GetModule(v8::Isolate* isolate) {
  PerIsolateData* data = PerIsolateData::From(isolate);
  v8::Local<v8::ObjectTemplate> templ =
      data->GetObjectTemplate(&g_wrapper_info);
  if (templ.IsEmpty()) {
    templ = ObjectTemplateBuilder(isolate)
        .SetMethod("fail", Fail)
        .SetMethod("expectTrue", ExpectTrue)
        .SetMethod("expectFalse", ExpectFalse)
        .SetMethod("expectEqual", ExpectEqual)
        .Build();
    data->SetObjectTemplate(&g_wrapper_info, templ);
  }
  return templ->NewInstance();
}

}  // namespace gin
