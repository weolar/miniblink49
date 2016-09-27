// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/client/gpu_memory_buffer_tracker.h"

#include "base/memory/scoped_ptr.h"
#include "gpu/command_buffer/client/gles2_implementation.h"
#include "gpu/command_buffer/client/gpu_control.h"

namespace gpu {
namespace gles2 {

GpuMemoryBufferTracker::GpuMemoryBufferTracker(GpuControl* gpu_control)
    : gpu_control_(gpu_control) {
}

GpuMemoryBufferTracker::~GpuMemoryBufferTracker() {
  while (!buffers_.empty()) {
    RemoveBuffer(buffers_.begin()->first);
  }
}

int32 GpuMemoryBufferTracker::CreateBuffer(size_t width,
                                           size_t height,
                                           int32 internalformat,
                                           int32 usage) {
  int32 image_id = 0;
  DCHECK(gpu_control_);
  gfx::GpuMemoryBuffer* buffer = gpu_control_->CreateGpuMemoryBuffer(
      width, height, internalformat, usage, &image_id);
  if (!buffer)
    return 0;

  std::pair<BufferMap::iterator, bool> result =
      buffers_.insert(std::make_pair(image_id, buffer));
  DCHECK(result.second);

  return image_id;
}

gfx::GpuMemoryBuffer* GpuMemoryBufferTracker::GetBuffer(int32 image_id) {
  BufferMap::iterator it = buffers_.find(image_id);
  return (it != buffers_.end()) ? it->second : NULL;
}

void GpuMemoryBufferTracker::RemoveBuffer(int32 image_id) {
  BufferMap::iterator buffer_it = buffers_.find(image_id);
  if (buffer_it != buffers_.end())
    buffers_.erase(buffer_it);
  DCHECK(gpu_control_);
  gpu_control_->DestroyGpuMemoryBuffer(image_id);
}

}  // namespace gles2
}  // namespace gpu
