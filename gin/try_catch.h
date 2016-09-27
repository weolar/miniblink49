// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_TRY_CATCH_H_
#define GIN_TRY_CATCH_H_

#include <string>

#include "base/basictypes.h"
#include "gin/gin_export.h"
#include "v8/include/v8.h"

namespace gin {

// TryCatch is a convenient wrapper around v8::TryCatch.
class GIN_EXPORT TryCatch {
 public:
  explicit TryCatch(v8::Isolate* isolate);
  ~TryCatch();

  bool HasCaught();
  std::string GetStackTrace();

 private:
  v8::Isolate* isolate_;
  v8::TryCatch try_catch_;

  DISALLOW_COPY_AND_ASSIGN(TryCatch);
};

}  // namespace gin

#endif  // GIN_TRY_CATCH_H_
