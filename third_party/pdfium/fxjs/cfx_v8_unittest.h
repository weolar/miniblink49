// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FXJS_CFX_V8_UNITTEST_H_
#define FXJS_CFX_V8_UNITTEST_H_

#include <memory>

#include "testing/gtest/include/gtest/gtest.h"

class CFX_V8;
class CFX_V8ArrayBufferAllocator;

namespace v8 {
class Isolate;
}  // namespace v8

class FXV8UnitTest : public ::testing::Test {
 public:
  struct V8IsolateDeleter {
    void operator()(v8::Isolate* ptr) const;
  };

  FXV8UnitTest();
  ~FXV8UnitTest() override;

  void SetUp() override;

  v8::Isolate* isolate() const { return isolate_.get(); }
  CFX_V8* cfx_v8() const { return cfx_v8_.get(); }

 protected:
  std::unique_ptr<CFX_V8ArrayBufferAllocator> array_buffer_allocator_;
  std::unique_ptr<v8::Isolate, V8IsolateDeleter> isolate_;
  std::unique_ptr<CFX_V8> cfx_v8_;
};

#endif  // FXJS_CFX_V8_UNITTEST_H_
