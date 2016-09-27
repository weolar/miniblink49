// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_GPU_MEMORY_BUFFER_MANAGER_H_
#define GPU_COMMAND_BUFFER_SERVICE_GPU_MEMORY_BUFFER_MANAGER_H_

#include "base/basictypes.h"
#include "gpu/gpu_export.h"
#include "ui/gfx/gpu_memory_buffer.h"

namespace gpu {

class GPU_EXPORT GpuMemoryBufferManagerInterface {
 public:
  virtual ~GpuMemoryBufferManagerInterface() {}

  virtual void RegisterGpuMemoryBuffer(int32 id,
                                       gfx::GpuMemoryBufferHandle buffer,
                                       size_t width,
                                       size_t height,
                                       unsigned internalformat) = 0;
  virtual void UnregisterGpuMemoryBuffer(int32 id) = 0;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_GPU_MEMORY_BUFFER_MANAGER_H_
