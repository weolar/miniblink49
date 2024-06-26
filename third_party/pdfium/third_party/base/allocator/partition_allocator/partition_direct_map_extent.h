// Copyright (c) 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BASE_ALLOCATOR_PARTITION_ALLOCATOR_PARTITION_DIRECT_MAP_EXTENT_H_
#define THIRD_PARTY_BASE_ALLOCATOR_PARTITION_ALLOCATOR_PARTITION_DIRECT_MAP_EXTENT_H_

#include "third_party/base/allocator/partition_allocator/partition_bucket.h"
#include "third_party/base/allocator/partition_allocator/partition_page.h"

namespace pdfium {
namespace base {
namespace internal {

struct PartitionDirectMapExtent {
  PartitionDirectMapExtent* next_extent;
  PartitionDirectMapExtent* prev_extent;
  PartitionBucket* bucket;
  size_t map_size;  // Mapped size, not including guard pages and meta-data.

  ALWAYS_INLINE static PartitionDirectMapExtent* FromPage(PartitionPage* page);
};

ALWAYS_INLINE PartitionDirectMapExtent* PartitionDirectMapExtent::FromPage(
    PartitionPage* page) {
  DCHECK(page->bucket->is_direct_mapped());
  return reinterpret_cast<PartitionDirectMapExtent*>(
      reinterpret_cast<char*>(page) + 3 * kPageMetadataSize);
}

}  // namespace internal
}  // namespace base
}  // namespace pdfium

#endif  // THIRD_PARTY_BASE_ALLOCATOR_PARTITION_ALLOCATOR_PARTITION_DIRECT_MAP_EXTENT_H_
