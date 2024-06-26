// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_SURFACE_EGL_H_
#define UI_GL_GL_SURFACE_EGL_H_

#if defined(OS_WIN)
#include <windows.h>
#endif

#include <string>

#include "base/command_line.h"
#include "base/compiler_specific.h"
#include "base/time/time.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/vsync_provider.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_surface.h"

namespace gfx {

// Get default EGL display for GLSurfaceEGL (differs by platform).
EGLNativeDisplayType GetPlatformDefaultEGLNativeDisplay();

// If adding a new type, also add it to EGLDisplayType in
// tools/metrics/histograms/histograms.xml. Don't remove or reorder entries.
enum DisplayType {
    DEFAULT = 0,
    SWIFT_SHADER = 1,
    ANGLE_WARP = 2,
    ANGLE_D3D9 = 3,
    ANGLE_D3D11 = 4,
    ANGLE_OPENGL = 5,
    ANGLE_OPENGLES = 6,
    DISPLAY_TYPE_MAX = 7,
};

GL_EXPORT void GetEGLInitDisplays(bool supports_angle_d3d,
    bool supports_angle_opengl,
    const base::CommandLine* command_line,
    std::vector<DisplayType>* init_displays);

// Interface for EGL surface.
class GL_EXPORT GLSurfaceEGL : public GLSurface {
public:
    GLSurfaceEGL();

    // Implement GLSurface.
    EGLDisplay GetDisplay() override;

    static bool InitializeOneOff();
    static EGLDisplay GetHardwareDisplay();
    static EGLDisplay InitializeDisplay();
    static EGLNativeDisplayType GetNativeDisplay();

    // These aren't particularly tied to surfaces, but since we already
    // have the static InitializeOneOff here, it's easiest to reuse its
    // initialization guards.
    static const char* GetEGLExtensions();
    static bool HasEGLExtension(const char* name);
    static bool IsCreateContextRobustnessSupported();
    static bool IsEGLSurfacelessContextSupported();

protected:
    ~GLSurfaceEGL() override;

private:
    DISALLOW_COPY_AND_ASSIGN(GLSurfaceEGL);
};

// Encapsulates an EGL surface bound to a view.
class GL_EXPORT NativeViewGLSurfaceEGL : public GLSurfaceEGL {
public:
    explicit NativeViewGLSurfaceEGL(EGLNativeWindowType window);

    // Implement GLSurface.
    EGLConfig GetConfig() override;
    bool Initialize() override;
    void Destroy() override;
    bool Resize(const gfx::Size& size, float scale_factor) override;
    bool Recreate() override;
    bool IsOffscreen() override;
    gfx::SwapResult SwapBuffers() override;
    gfx::Size GetSize() override;
    EGLSurface GetHandle() override;
    bool SupportsPostSubBuffer() override;
    gfx::SwapResult PostSubBuffer(int x, int y, int width, int height) override;
    VSyncProvider* GetVSyncProvider() override;

    // Create a NativeViewGLSurfaceEGL with an externally provided VSyncProvider.
    // Takes ownership of the VSyncProvider.
    virtual bool Initialize(scoped_ptr<VSyncProvider> sync_provider);

protected:
    ~NativeViewGLSurfaceEGL() override;

    EGLNativeWindowType window_;

    void OnSetSwapInterval(int interval) override;

private:
    EGLSurface surface_;
    bool supports_post_sub_buffer_;
    EGLConfig config_;
    gfx::Size size_;

    scoped_ptr<VSyncProvider> vsync_provider_;

    int swap_interval_;

#if defined(OS_WIN)
    bool vsync_override_;

    unsigned int swap_generation_;
    static unsigned int current_swap_generation_;
    static unsigned int swaps_this_generation_;
    static unsigned int last_multiswap_generation_;
#endif

    DISALLOW_COPY_AND_ASSIGN(NativeViewGLSurfaceEGL);
};

// Encapsulates a pbuffer EGL surface.
class GL_EXPORT PbufferGLSurfaceEGL : public GLSurfaceEGL {
public:
    explicit PbufferGLSurfaceEGL(const gfx::Size& size);

    // Implement GLSurface.
    EGLConfig GetConfig() override;
    bool Initialize() override;
    void Destroy() override;
    bool IsOffscreen() override;
    gfx::SwapResult SwapBuffers() override;
    gfx::Size GetSize() override;
    bool Resize(const gfx::Size& size, float scale_factor) override;
    EGLSurface GetHandle() override;
    void* GetShareHandle() override;

protected:
    ~PbufferGLSurfaceEGL() override;

private:
    gfx::Size size_;
    EGLSurface surface_;

    DISALLOW_COPY_AND_ASSIGN(PbufferGLSurfaceEGL);
};

// SurfacelessEGL is used as Offscreen surface when platform supports
// KHR_surfaceless_context and GL_OES_surfaceless_context. This would avoid the
// need to create a dummy EGLsurface in case we render to client API targets.
class GL_EXPORT SurfacelessEGL : public GLSurfaceEGL {
public:
    explicit SurfacelessEGL(const gfx::Size& size);

    // Implement GLSurface.
    EGLConfig GetConfig() override;
    bool Initialize() override;
    void Destroy() override;
    bool IsOffscreen() override;
    bool IsSurfaceless() const override;
    gfx::SwapResult SwapBuffers() override;
    gfx::Size GetSize() override;
    bool Resize(const gfx::Size& size, float scale_factor) override;
    EGLSurface GetHandle() override;
    void* GetShareHandle() override;

protected:
    ~SurfacelessEGL() override;

private:
    gfx::Size size_;
    DISALLOW_COPY_AND_ASSIGN(SurfacelessEGL);
};

} // namespace gfx

#endif // UI_GL_GL_SURFACE_EGL_H_
