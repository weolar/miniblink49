// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_GL_CONTEXT_VIRTUAL_H_
#define GPU_COMMAND_BUFFER_SERVICE_GL_CONTEXT_VIRTUAL_H_

#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "gpu/gpu_export.h"
#include "ui/gl/gl_context.h"

namespace gfx {
class Display;
class GLSurface;
class GLStateRestorer;
}

namespace gpu {
namespace gles2 {
class GLES2Decoder;
}

// Encapsulates a virtual OpenGL context.
class GPU_EXPORT GLContextVirtual : public gfx::GLContext {
 public:
  GLContextVirtual(
      gfx::GLShareGroup* share_group,
      gfx::GLContext* shared_context,
      base::WeakPtr<gles2::GLES2Decoder> decoder);

  gfx::Display* display();

  // Implement GLContext.
  virtual bool Initialize(
      gfx::GLSurface* compatible_surface,
      gfx::GpuPreference gpu_preference) OVERRIDE;
  virtual void Destroy() OVERRIDE;
  virtual bool MakeCurrent(gfx::GLSurface* surface) OVERRIDE;
  virtual void ReleaseCurrent(gfx::GLSurface* surface) OVERRIDE;
  virtual bool IsCurrent(gfx::GLSurface* surface) OVERRIDE;
  virtual void* GetHandle() OVERRIDE;
  virtual void SetSwapInterval(int interval) OVERRIDE;
  virtual std::string GetExtensions() OVERRIDE;
  virtual bool GetTotalGpuMemory(size_t* bytes) OVERRIDE;
  virtual void SetSafeToForceGpuSwitch() OVERRIDE;
  virtual bool WasAllocatedUsingRobustnessExtension() OVERRIDE;
  virtual void SetUnbindFboOnMakeCurrent() OVERRIDE;

 protected:
  virtual ~GLContextVirtual();

 private:
  scoped_refptr<gfx::GLContext> shared_context_;
  gfx::Display* display_;
  base::WeakPtr<gles2::GLES2Decoder> decoder_;

  DISALLOW_COPY_AND_ASSIGN(GLContextVirtual);
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_GL_CONTEXT_VIRTUAL_H_
