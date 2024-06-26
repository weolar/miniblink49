// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/gpu_memory_buffer.h"

namespace gfx {

// base::trace_event::MemoryAllocatorDumpGuid GetGpuMemoryBufferGUIDForTracing(uint64_t tracing_process_id, GpuMemoryBufferId buffer_id) {
//   // TODO(ericrk): Currently this function just wraps
//   // GetGenericSharedMemoryGUIDForTracing, we may want to special case this if
//   // the GPU memory buffer is not backed by shared memory.
//   return gfx::GetGenericSharedMemoryGUIDForTracing(tracing_process_id, buffer_id);
// }

GpuMemoryBufferHandle::GpuMemoryBufferHandle()
    : type(EMPTY_BUFFER)
    , id(0)
    , handle(base::SharedMemory::NULLHandle())
{
}

GpuMemoryBufferHandle::~GpuMemoryBufferHandle() { }

bool GpuMemoryBuffer::IsInUseByMacOSWindowServer() const
{
    return false;
}

} // namespace gfx
