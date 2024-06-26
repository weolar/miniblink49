// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_SURFACE_EGL_X11_H_
#define UI_GL_GL_SURFACE_EGL_X11_H_

#include <stdint.h>

#include <string>

#include "base/macros.h"
#include "ui/events/platform/platform_event_dispatcher.h"
#include "ui/gl/gl_surface_egl.h"

namespace gfx {

// Encapsulates an EGL surface bound to a view using the X Window System.
class GL_EXPORT NativeViewGLSurfaceEGLX11 : public NativeViewGLSurfaceEGL,
                                            public ui::PlatformEventDispatcher {
public:
    explicit NativeViewGLSurfaceEGLX11(EGLNativeWindowType window);

    // NativeViewGLSurfaceEGL overrides.
    EGLConfig GetConfig() override;
    void Destroy() override;
    bool Resize(const gfx::Size& size,
        float scale_factor,
        bool has_alpha) override;
    bool InitializeNativeWindow() override;

private:
    ~NativeViewGLSurfaceEGLX11() override;

    EGLNativeWindowType parent_window_;

    // PlatformEventDispatcher implementation.
    bool CanDispatchEvent(const ui::PlatformEvent& event) override;
    uint32_t DispatchEvent(const ui::PlatformEvent& event) override;

    DISALLOW_COPY_AND_ASSIGN(NativeViewGLSurfaceEGLX11);
};

} // namespace gfx

#endif // UI_GL_GL_SURFACE_EGL_X11_H_
