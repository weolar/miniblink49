// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_TEST_GTEST_H_
#define GIN_TEST_GTEST_H_

#include "v8/include/v8.h"

namespace gin {

// This module provides bindings to gtest. Most tests should use an idiomatic
// JavaScript testing API, but this module is available for tests that need a
// low-level integration with gtest.
class GTest {
 public:
  static const char kModuleName[];
  static v8::Local<v8::Value> GetModule(v8::Isolate* isolate);
};

}  // namespace gin

#endif  // GIN_TEST_GTEST_H_
