// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_CLIENT_GL_IN_PROCESS_CONTEXT_H_
#define GPU_COMMAND_BUFFER_CLIENT_GL_IN_PROCESS_CONTEXT_H_

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "gl_in_process_context_export.h"
#include "gpu/command_buffer/service/in_process_command_buffer.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/gpu_preference.h"

namespace gfx {
class Size;
}

#if defined(OS_ANDROID)
namespace gfx {
class SurfaceTexture;
}
#endif

namespace gpu {

namespace gles2 {
class GLES2Implementation;
}

// The default uninitialized value is -1.
struct GL_IN_PROCESS_CONTEXT_EXPORT GLInProcessContextAttribs {
  GLInProcessContextAttribs();

  int32 alpha_size;
  int32 blue_size;
  int32 green_size;
  int32 red_size;
  int32 depth_size;
  int32 stencil_size;
  int32 samples;
  int32 sample_buffers;
  int32 fail_if_major_perf_caveat;
  int32 lose_context_when_out_of_memory;
#ifdef TENCENT_CHANGES
  int32 share_sys_context;
#endif
};

class GL_IN_PROCESS_CONTEXT_EXPORT GLInProcessContext {
 public:
  virtual ~GLInProcessContext() {}

  // Create a GLInProcessContext, if |is_offscreen| is true, renders to an
  // offscreen context. |attrib_list| must be NULL or a NONE-terminated list
  // of attribute/value pairs.
  // If |surface| is not NULL, then it must match |is_offscreen| and |size|,
  // |window| must be gfx::kNullAcceleratedWidget, and the command buffer
  // service must run on the same thread as this client because GLSurface is
  // not thread safe. If |surface| is NULL, then the other parameters are used
  // to correctly create a surface.
  // Only one of |share_context| and |use_global_share_group| can be used at
  // the same time.
  static GLInProcessContext* Create(
      scoped_refptr<gpu::InProcessCommandBuffer::Service> service,
      scoped_refptr<gfx::GLSurface> surface,
      bool is_offscreen,
      gfx::AcceleratedWidget window,
      const gfx::Size& size,
      GLInProcessContext* share_context,
      bool use_global_share_group,
      const GLInProcessContextAttribs& attribs,
      gfx::GpuPreference gpu_preference);

  virtual void SetContextLostCallback(const base::Closure& callback) = 0;

  // Allows direct access to the GLES2 implementation so a GLInProcessContext
  // can be used without making it current.
  virtual gles2::GLES2Implementation* GetImplementation() = 0;

#if defined(OS_ANDROID)
  virtual scoped_refptr<gfx::SurfaceTexture> GetSurfaceTexture(
      uint32 stream_id) = 0;
#endif
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_CLIENT_GL_IN_PROCESS_CONTEXT_H_
