// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/scoped_platform_file_closer.h"

namespace base {
namespace internal {

void PlatformFileCloser::operator()(PlatformFile* file) const {
  if (file && *file != kInvalidPlatformFileValue)
    ClosePlatformFile(*file);
}

}  // namespace internal
}  // namespace base
