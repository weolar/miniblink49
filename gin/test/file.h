// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_TEST_FILE_H_
#define GIN_TEST_FILE_H_

#include "v8/include/v8.h"

namespace gin {

class File {
 public:
  static const char kModuleName[];
  static v8::Local<v8::Value> GetModule(v8::Isolate* isolate);
};

}  // namespace gin

#endif  // GIN_TEST_FILE_H_

