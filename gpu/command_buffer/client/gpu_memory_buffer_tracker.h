// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_CLIENT_GPU_MEMORY_BUFFER_TRACKER_H_
#define GPU_COMMAND_BUFFER_CLIENT_GPU_MEMORY_BUFFER_TRACKER_H_

#include "base/basictypes.h"
#include "base/containers/hash_tables.h"
#include "gles2_impl_export.h"

namespace gfx {
class GpuMemoryBuffer;
}

namespace gpu {
class GpuControl;

namespace gles2 {

// Tracks GPU memory buffer objects on the client side.
class GLES2_IMPL_EXPORT GpuMemoryBufferTracker {
 public:
  explicit GpuMemoryBufferTracker(GpuControl* gpu_control);
  virtual ~GpuMemoryBufferTracker();

  int32 CreateBuffer(size_t width,
                     size_t height,
                     int32 internalformat,
                     int32 usage);
  gfx::GpuMemoryBuffer* GetBuffer(int32 image_id);
  void RemoveBuffer(int32 image_id);

 private:
  typedef base::hash_map<int32, gfx::GpuMemoryBuffer*> BufferMap;
  BufferMap buffers_;
  GpuControl* gpu_control_;

  DISALLOW_COPY_AND_ASSIGN(GpuMemoryBufferTracker);
};

}  // namespace gles2
}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_CLIENT_GPU_MEMORY_BUFFER_TRACKER_H_
