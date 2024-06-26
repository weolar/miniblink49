// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxjs/cfxjs_engine.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/js_embedder_test.h"

namespace {

const double kExpected0 = 6.0;
const double kExpected1 = 7.0;
const double kExpected2 = 8.0;

const wchar_t kScript0[] = L"fred = 6";
const wchar_t kScript1[] = L"fred = 7";
const wchar_t kScript2[] = L"fred = 8";

}  // namespace

using CFXJSEngineEmbedderTest = JSEmbedderTest;

void CheckAssignmentInEngineContext(CFXJS_Engine* current_engine,
                                    double expected) {
  v8::Context::Scope context_scope(current_engine->GetV8Context());
  v8::Local<v8::Object> This = current_engine->GetThisObj();
  v8::Local<v8::Value> fred = current_engine->GetObjectProperty(This, "fred");
  EXPECT_TRUE(fred->IsNumber());
  EXPECT_EQ(expected, current_engine->ToDouble(fred));
}

TEST_F(CFXJSEngineEmbedderTest, Getters) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());
  v8::Context::Scope context_scope(GetV8Context());

  Optional<IJS_Runtime::JS_Error> err = engine()->Execute(WideString(kScript1));
  EXPECT_FALSE(err);
  CheckAssignmentInEngineContext(engine(), kExpected1);
}

TEST_F(CFXJSEngineEmbedderTest, MultipleEngines) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());

  CFXJS_Engine engine1(isolate());
  engine1.InitializeEngine();

  CFXJS_Engine engine2(isolate());
  engine2.InitializeEngine();

  v8::Context::Scope context_scope(GetV8Context());
  {
    Optional<IJS_Runtime::JS_Error> err =
        engine()->Execute(WideString(kScript0));
    EXPECT_FALSE(err);
    CheckAssignmentInEngineContext(engine(), kExpected0);
  }
  {
    // engine1 executing in engine1's context doesn't affect main.
    v8::Context::Scope context_scope1(engine1.GetV8Context());
    Optional<IJS_Runtime::JS_Error> err = engine1.Execute(WideString(kScript1));
    EXPECT_FALSE(err);
    CheckAssignmentInEngineContext(engine(), kExpected0);
    CheckAssignmentInEngineContext(&engine1, kExpected1);
  }
  {
    // engine1 executing in engine2's context doesn't affect engine1.
    v8::Context::Scope context_scope2(engine2.GetV8Context());
    Optional<IJS_Runtime::JS_Error> err = engine1.Execute(WideString(kScript2));
    EXPECT_FALSE(err);
    CheckAssignmentInEngineContext(engine(), kExpected0);
    CheckAssignmentInEngineContext(&engine1, kExpected1);
    CheckAssignmentInEngineContext(&engine2, kExpected2);
  }
  engine1.ReleaseEngine();
  engine2.ReleaseEngine();
}

TEST_F(CFXJSEngineEmbedderTest, JSCompileError) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());
  v8::Context::Scope context_scope(GetV8Context());

  Optional<IJS_Runtime::JS_Error> err =
      engine()->Execute(L"functoon(x) { return x+1; }");
  EXPECT_TRUE(err);
  EXPECT_EQ(L"SyntaxError: Unexpected token {", err->exception);
  EXPECT_EQ(1, err->line);
  EXPECT_EQ(12, err->column);
}

TEST_F(CFXJSEngineEmbedderTest, JSRuntimeError) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());
  v8::Context::Scope context_scope(GetV8Context());

  Optional<IJS_Runtime::JS_Error> err =
      engine()->Execute(L"let a = 3;\nundefined.colour");
  EXPECT_TRUE(err);
  EXPECT_EQ(L"TypeError: Cannot read property 'colour' of undefined",
            err->exception);
  EXPECT_EQ(2, err->line);
  EXPECT_EQ(10, err->column);
}
