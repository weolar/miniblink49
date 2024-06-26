// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_CONTEXT_WGL_H_
#define UI_GL_GL_CONTEXT_WGL_H_

#include <string>

#include "ui/gfx/native_widget_types.h"
#include "ui/gl/gl_context.h"

namespace gfx {

class GLSurface;

// This class is a wrapper around a GL context.
class GLContextWGL : public GLContextReal {
public:
    explicit GLContextWGL(GLShareGroup* share_group);

    // Implement GLContext.
    bool Initialize(GLSurface* compatible_surface,
        GpuPreference gpu_preference) override;
    bool MakeCurrent(GLSurface* surface) override;
    void ReleaseCurrent(GLSurface* surface) override;
    bool IsCurrent(GLSurface* surface) override;
    void* GetHandle() override;
    void OnSetSwapInterval(int interval) override;
    std::string GetExtensions() override;

private:
    ~GLContextWGL() override;
    void Destroy();

    HGLRC context_;

    DISALLOW_COPY_AND_ASSIGN(GLContextWGL);
};

} // namespace gfx

#endif // UI_GL_GL_CONTEXT_WGL_H_
