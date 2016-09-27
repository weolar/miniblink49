// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/gpu_control_service.h"

#include "gpu/command_buffer/service/gpu_memory_buffer_manager.h"
#include "gpu/command_buffer/service/query_manager.h"

namespace gpu {

GpuControlService::GpuControlService(
    GpuMemoryBufferManagerInterface* gpu_memory_buffer_manager,
    gles2::QueryManager* query_manager)
    : gpu_memory_buffer_manager_(gpu_memory_buffer_manager),
      query_manager_(query_manager) {
}

GpuControlService::~GpuControlService() {
}

void GpuControlService::SignalQuery(uint32 query_id,
                                    const base::Closure& callback) {
  DCHECK(query_manager_);
  gles2::QueryManager::Query* query = query_manager_->GetQuery(query_id);
  if (!query)
    callback.Run();
  else
    query->AddCallback(callback);
}

void GpuControlService::RegisterGpuMemoryBuffer(
    int32 id,
    gfx::GpuMemoryBufferHandle buffer,
    size_t width,
    size_t height,
    unsigned internalformat) {
  gpu_memory_buffer_manager_->RegisterGpuMemoryBuffer(
      id, buffer, width, height, internalformat);
}

void GpuControlService::UnregisterGpuMemoryBuffer(int32 id) {
  gpu_memory_buffer_manager_->UnregisterGpuMemoryBuffer(id);
}

}  // namespace gpu
