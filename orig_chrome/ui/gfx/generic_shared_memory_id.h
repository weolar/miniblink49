// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_GENERIC_SHARED_MEMORY_ID_H_
#define UI_GFX_GENERIC_SHARED_MEMORY_ID_H_

#include <stddef.h>
#include <stdint.h>

#include "base/containers/hash_tables.h"
#include "base/hash.h"
//#include "base/trace_event/memory_allocator_dump.h"
#include "ui/gfx/gfx_export.h"

namespace gfx {

// Defines an ID type which is used across all types of shared memory
// allocations in content/. This ID type is in ui/gfx, as components outside
// content/ may need to hold an ID (but should not generate one).
class GFX_EXPORT GenericSharedMemoryId {
public:
    int id;

    // Invalid ID is -1 to match semantics of base::StaticAtomicSequenceNumber.
    GenericSharedMemoryId()
        : id(-1)
    {
    }
    explicit GenericSharedMemoryId(int id)
        : id(id)
    {
    }
    GenericSharedMemoryId(const GenericSharedMemoryId& other) = default;
    GenericSharedMemoryId& operator=(const GenericSharedMemoryId& other) = default;

    bool operator==(const GenericSharedMemoryId& other) const
    {
        return id == other.id;
    }

    bool operator<(const GenericSharedMemoryId& other) const
    {
        return id < other.id;
    }
};

// Generates GUID which can be used to trace shared memory using its
// GenericSharedMemoryId.
// GFX_EXPORT base::trace_event::MemoryAllocatorDumpGuid
// GetGenericSharedMemoryGUIDForTracing(
//     uint64_t tracing_process_id,
//     GenericSharedMemoryId generic_shared_memory_id);

} // namespace gfx

namespace BASE_HASH_NAMESPACE {

#if USING_VC6RT != 1

template <>
struct hash<gfx::GenericSharedMemoryId> {
    size_t operator()(gfx::GenericSharedMemoryId key) const
    {
        return BASE_HASH_NAMESPACE::hash<int>()(key.id);
    }
};

template <typename Second>
struct hash<std::pair<gfx::GenericSharedMemoryId, Second>> {
    size_t operator()(
        const std::pair<gfx::GenericSharedMemoryId, Second>& pair) const
    {
        return base::HashInts(pair.first.id, pair.second);
    }
};

#endif

} // namespace BASE_HASH_NAMESPACE

#endif // UI_GFX_GENERIC_SHARED_MEMORY_ID_H_
