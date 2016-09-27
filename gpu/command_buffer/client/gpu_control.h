// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_CLIENT_GPU_CONTROL_H_
#define GPU_COMMAND_BUFFER_CLIENT_GPU_CONTROL_H_

#include <stdint.h>

#include <vector>

#include "base/callback.h"
#include "base/macros.h"
#include "gpu/command_buffer/common/capabilities.h"
#include "gpu/command_buffer/common/mailbox.h"
#include "gpu/gpu_export.h"

namespace gfx {
class GpuMemoryBuffer;
}

namespace gpu {

// Common interface for GpuControl implementations.
class GPU_EXPORT GpuControl {
 public:
  GpuControl() {}
  virtual ~GpuControl() {}

  virtual Capabilities GetCapabilities() = 0;

  // Create a gpu memory buffer of the given dimensions and format. Returns
  // its ID or -1 on error.
  virtual gfx::GpuMemoryBuffer* CreateGpuMemoryBuffer(
      size_t width,
      size_t height,
      unsigned internalformat,
      unsigned usage,
      int32_t* id) = 0;

  // Destroy a gpu memory buffer. The ID must be positive.
  virtual void DestroyGpuMemoryBuffer(int32_t id) = 0;

  // Inserts a sync point, returning its ID. Sync point IDs are global and can
  // be used for cross-context synchronization.
  virtual uint32_t InsertSyncPoint() = 0;

  // Runs |callback| when a sync point is reached.
  virtual void SignalSyncPoint(uint32_t sync_point,
                               const base::Closure& callback) = 0;

  // Runs |callback| when a query created via glCreateQueryEXT() has cleared
  // passed the glEndQueryEXT() point.
  virtual void SignalQuery(uint32_t query, const base::Closure& callback) = 0;

  virtual void SetSurfaceVisible(bool visible) = 0;

  // Invokes the callback once the context has been flushed.
  virtual void Echo(const base::Closure& callback) = 0;

  // Attaches an external stream to the texture given by |texture_id| and
  // returns a stream identifier.
  virtual uint32_t CreateStreamTexture(uint32_t texture_id) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(GpuControl);
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_CLIENT_GPU_CONTROL_H_
