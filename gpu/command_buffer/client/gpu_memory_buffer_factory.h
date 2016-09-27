// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_CLIENT_GPU_MEMORY_FACTORY_BUFFER_H_
#define GPU_COMMAND_BUFFER_CLIENT_GPU_MEMORY_FACTORY_BUFFER_H_

#include "gpu/gpu_export.h"

namespace gfx {
class GpuMemoryBuffer;
}

namespace gpu {

class GPU_EXPORT GpuMemoryBufferFactory {
 public:
  virtual gfx::GpuMemoryBuffer* CreateGpuMemoryBuffer(size_t width,
                                                      size_t height,
                                                      unsigned internalformat,
                                                      unsigned usage) = 0;

 protected:
  virtual ~GpuMemoryBufferFactory() {}
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_CLIENT_GPU_MEMORY_BUFFER_FACTORY_H_
