// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxjs/cfxjs_engine.h"

#include <memory>

#include "fxjs/cfx_v8_unittest.h"
#include "fxjs/cjs_object.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/base/ptr_util.h"

class FXJSEngineUnitTest : public FXV8UnitTest {
 public:
  FXJSEngineUnitTest() = default;
  ~FXJSEngineUnitTest() override = default;

  void SetUp() override {
    FXV8UnitTest::SetUp();
    FXJS_Initialize(1, isolate());
    engine_ = pdfium::MakeUnique<CFXJS_Engine>(isolate());
  }

  void TearDown() override { FXJS_Release(); }

  CFXJS_Engine* engine() const { return engine_.get(); }

 private:
  std::unique_ptr<CFXJS_Engine> engine_;
};

static bool perm_created = false;
static bool perm_destroyed = false;
static bool temp_created = false;
static bool temp_destroyed = false;

TEST_F(FXJSEngineUnitTest, GC) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());

  // Object: 0
  engine()->DefineObj("perm", FXJSOBJTYPE_DYNAMIC,
                      [](CFXJS_Engine* pEngine, v8::Local<v8::Object> obj) {
                        pEngine->SetObjectPrivate(
                            obj, pdfium::MakeUnique<CJS_Object>(obj, nullptr));
                        perm_created = true;
                      },
                      [](v8::Local<v8::Object> obj) {
                        perm_destroyed = true;
                        CFXJS_Engine::SetObjectPrivate(obj, nullptr);
                      });

  // Object: 1
  engine()->DefineObj("temp", FXJSOBJTYPE_DYNAMIC,
                      [](CFXJS_Engine* pEngine, v8::Local<v8::Object> obj) {
                        pEngine->SetObjectPrivate(
                            obj, pdfium::MakeUnique<CJS_Object>(obj, nullptr));
                        temp_created = true;
                      },
                      [](v8::Local<v8::Object> obj) {
                        temp_destroyed = true;
                        CFXJS_Engine::SetObjectPrivate(obj, nullptr);
                      });

  engine()->InitializeEngine();

  v8::Context::Scope context_scope(engine()->GetV8Context());
  v8::Local<v8::Object> perm =
      engine()->NewFXJSBoundObject(0, FXJSOBJTYPE_DYNAMIC);
  EXPECT_FALSE(perm.IsEmpty());
  EXPECT_TRUE(perm_created);
  EXPECT_FALSE(perm_destroyed);

  {
    v8::HandleScope inner_handle_scope(isolate());
    v8::Local<v8::Object> temp =
        engine()->NewFXJSBoundObject(1, FXJSOBJTYPE_DYNAMIC);
    EXPECT_FALSE(temp.IsEmpty());
    EXPECT_TRUE(temp_created);
    EXPECT_FALSE(temp_destroyed);
  }

  Optional<IJS_Runtime::JS_Error> err = engine()->Execute(L"gc();");
  EXPECT_FALSE(err);

  EXPECT_TRUE(perm_created);
  EXPECT_FALSE(perm_destroyed);
  EXPECT_TRUE(temp_created);
  // TODO(tsepez): temp_destroyed should be true, but it isnt.
  // EXPECT_TRUE(temp_destroyed);

  engine()->ReleaseEngine();
  EXPECT_TRUE(perm_created);
  EXPECT_TRUE(perm_destroyed);
  EXPECT_TRUE(temp_created);
  EXPECT_TRUE(temp_destroyed);
}
