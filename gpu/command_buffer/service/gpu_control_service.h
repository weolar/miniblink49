// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_GPU_CONTROL_SERVICE_H_
#define GPU_COMMAND_BUFFER_SERVICE_GPU_CONTROL_SERVICE_H_

#include "base/callback.h"
#include "gpu/command_buffer/common/capabilities.h"
#include "gpu/gpu_export.h"
#include "ui/gfx/gpu_memory_buffer.h"

namespace gpu {
class GpuMemoryBufferManagerInterface;

namespace gles2 {
class QueryManager;
}

class GPU_EXPORT GpuControlService {
 public:
  GpuControlService(GpuMemoryBufferManagerInterface* gpu_memory_buffer_manager,
                    gles2::QueryManager* query_manager);
  virtual ~GpuControlService();

  void SignalQuery(uint32 query, const base::Closure& callback);

  void RegisterGpuMemoryBuffer(int32 id,
                               gfx::GpuMemoryBufferHandle buffer,
                               size_t width,
                               size_t height,
                               unsigned internalformat);
  void UnregisterGpuMemoryBuffer(int32 id);

 private:
  GpuMemoryBufferManagerInterface* gpu_memory_buffer_manager_;
  gles2::QueryManager* query_manager_;

  DISALLOW_COPY_AND_ASSIGN(GpuControlService);
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_GPU_CONTROL_SERVICE_H_
