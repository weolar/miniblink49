// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_FILES_SCOPED_PLATFORM_FILE_CLOSER_H_
#define BASE_FILES_SCOPED_PLATFORM_FILE_CLOSER_H_

#if USING_CHROMIUM_BASE == 1
#include "base/memory/scoped_ptr.h"
#else
#include "cef/include/base/cef_scoped_ptr.h"
#endif
#include "base/platform_file.h"

namespace base {

namespace internal {

struct BASE_EXPORT PlatformFileCloser {
  void operator()(PlatformFile* file) const;
};

}  // namespace internal

typedef scoped_ptr<PlatformFile, internal::PlatformFileCloser>
    ScopedPlatformFileCloser;

}  // namespace base

#endif  // BASE_FILES_SCOPED_PLATFORM_FILE_CLOSER_H_
