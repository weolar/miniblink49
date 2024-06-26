// Copyright (c) 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/base/allocator/partition_allocator/partition_oom.h"

#include "build/build_config.h"
#include "third_party/base/allocator/partition_allocator/oom.h"

namespace pdfium {
namespace base {
namespace internal {

void NOINLINE PartitionExcessiveAllocationSize() {
  OOM_CRASH();
}

#if !defined(ARCH_CPU_64_BITS)
NOINLINE void PartitionOutOfMemoryWithLotsOfUncommitedPages() {
  OOM_CRASH();
}
#endif

}  // namespace internal
}  // namespace base
}  // namespace pdfium
