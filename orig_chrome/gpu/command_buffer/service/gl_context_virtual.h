// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_GL_CONTEXT_VIRTUAL_H_
#define GPU_COMMAND_BUFFER_SERVICE_GL_CONTEXT_VIRTUAL_H_

#include "base/callback_forward.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "gpu/gpu_export.h"
#include "ui/gl/gl_context.h"
#include <string>

namespace gfx {
class GPUPreference;
class GPUTimingClient;
class GLShareGroup;
class GLSurface;
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

    // Implement GLContext.
    bool Initialize(gfx::GLSurface* compatible_surface,
        gfx::GpuPreference gpu_preference) override;
    bool MakeCurrent(gfx::GLSurface* surface) override;
    void ReleaseCurrent(gfx::GLSurface* surface) override;
    bool IsCurrent(gfx::GLSurface* surface) override;
    void* GetHandle() override;
    scoped_refptr<gfx::GPUTimingClient> CreateGPUTimingClient() override;
    void OnSetSwapInterval(int interval) override;
    std::string GetExtensions() override;
    bool GetTotalGpuMemory(size_t* bytes) override;
    void SetSafeToForceGpuSwitch() override;
    bool WasAllocatedUsingRobustnessExtension() override;
    void SetUnbindFboOnMakeCurrent() override;
    base::Closure GetStateWasDirtiedExternallyCallback() override;
    void RestoreStateIfDirtiedExternally() override;

protected:
    ~GLContextVirtual() override;

private:
    void Destroy();

    scoped_refptr<gfx::GLContext> shared_context_;
    base::WeakPtr<gles2::GLES2Decoder> decoder_;

    DISALLOW_COPY_AND_ASSIGN(GLContextVirtual);
};

} // namespace gpu

#endif // GPU_COMMAND_BUFFER_SERVICE_GL_CONTEXT_VIRTUAL_H_
