// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_context_cgl.h"

#include <OpenGL/CGLRenderers.h>
#include <OpenGL/CGLTypes.h>
#include <vector>

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/trace_event/trace_event.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/gpu_switching_manager.h"

namespace gfx {

namespace {

    bool g_support_renderer_switching;

    struct CGLRendererInfoObjDeleter {
        void operator()(CGLRendererInfoObj* x)
        {
            if (x)
                CGLDestroyRendererInfo(*x);
        }
    };

} // namespace

static CGLPixelFormatObj GetPixelFormat()
{
    static CGLPixelFormatObj format;
    if (format)
        return format;
    std::vector<CGLPixelFormatAttribute> attribs;
    // If the system supports dual gpus then allow offline renderers for every
    // context, so that they can all be in the same share group.
    if (ui::GpuSwitchingManager::GetInstance()->SupportsDualGpus()) {
        attribs.push_back(kCGLPFAAllowOfflineRenderers);
        g_support_renderer_switching = true;
    }
    if (GetGLImplementation() == kGLImplementationAppleGL) {
        attribs.push_back(kCGLPFARendererID);
        attribs.push_back((CGLPixelFormatAttribute)kCGLRendererGenericFloatID);
        g_support_renderer_switching = false;
    }
    if (GetGLImplementation() == kGLImplementationDesktopGLCoreProfile) {
        // These constants don't exist in the 10.6 SDK against which
        // Chromium currently compiles.
        const int kOpenGLProfile = 99;
        const int kOpenGL3_2Core = 0x3200;
        attribs.push_back(static_cast<CGLPixelFormatAttribute>(kOpenGLProfile));
        attribs.push_back(static_cast<CGLPixelFormatAttribute>(kOpenGL3_2Core));
    }

    attribs.push_back((CGLPixelFormatAttribute)0);

    GLint num_virtual_screens;
    if (CGLChoosePixelFormat(&attribs.front(),
            &format,
            &num_virtual_screens)
        != kCGLNoError) {
        LOG(ERROR) << "Error choosing pixel format.";
        return nullptr;
    }
    if (!format) {
        LOG(ERROR) << "format == 0.";
        return nullptr;
    }
    DCHECK_NE(num_virtual_screens, 0);
    return format;
}

GLContextCGL::GLContextCGL(GLShareGroup* share_group)
    : GLContextReal(share_group)
    , context_(nullptr)
    , gpu_preference_(PreferIntegratedGpu)
    , discrete_pixelformat_(nullptr)
    , screen_(-1)
    , renderer_id_(-1)
    , safe_to_force_gpu_switch_(true)
{
}

bool GLContextCGL::Initialize(GLSurface* compatible_surface,
    GpuPreference gpu_preference)
{
    DCHECK(compatible_surface);

    gpu_preference = ui::GpuSwitchingManager::GetInstance()->AdjustGpuPreference(
        gpu_preference);

    GLContextCGL* share_context = share_group() ? static_cast<GLContextCGL*>(share_group()->GetContext()) : nullptr;

    CGLPixelFormatObj format = GetPixelFormat();
    if (!format)
        return false;

    // If using the discrete gpu, create a pixel format requiring it before we
    // create the context.
    if (!ui::GpuSwitchingManager::GetInstance()->SupportsDualGpus() || gpu_preference == PreferDiscreteGpu) {
        std::vector<CGLPixelFormatAttribute> discrete_attribs;
        discrete_attribs.push_back((CGLPixelFormatAttribute)0);
        GLint num_pixel_formats;
        if (CGLChoosePixelFormat(&discrete_attribs.front(),
                &discrete_pixelformat_,
                &num_pixel_formats)
            != kCGLNoError) {
            LOG(ERROR) << "Error choosing pixel format.";
            return false;
        }
        // The renderer might be switched after this, so ignore the saved ID.
        share_group()->SetRendererID(-1);
    }

    CGLError res = CGLCreateContext(
        format,
        share_context ? static_cast<CGLContextObj>(share_context->GetHandle()) : nullptr,
        reinterpret_cast<CGLContextObj*>(&context_));
    if (res != kCGLNoError) {
        LOG(ERROR) << "Error creating context.";
        Destroy();
        return false;
    }

    gpu_preference_ = gpu_preference;
    // Contexts that prefer integrated gpu are known to use only the subset of GL
    // that can be safely migrated between the iGPU and the dGPU. Mark those
    // contexts as safe to forcibly transition between the GPUs by default.
    // http://crbug.com/180876, http://crbug.com/227228
    safe_to_force_gpu_switch_ = gpu_preference == PreferIntegratedGpu;
    return true;
}

void GLContextCGL::Destroy()
{
    if (discrete_pixelformat_) {
        if (base::MessageLoop::current() != nullptr) {
            // Delay releasing the pixel format for 10 seconds to reduce the number of
            // unnecessary GPU switches.
            base::MessageLoop::current()->PostDelayedTask(
                FROM_HERE, base::Bind(&CGLReleasePixelFormat, discrete_pixelformat_),
                base::TimeDelta::FromSeconds(10));
        } else {
            CGLReleasePixelFormat(discrete_pixelformat_);
        }
        discrete_pixelformat_ = nullptr;
    }
    if (context_) {
        CGLDestroyContext(static_cast<CGLContextObj>(context_));
        context_ = nullptr;
    }
}

bool GLContextCGL::ForceGpuSwitchIfNeeded()
{
    DCHECK(context_);

    // The call to CGLSetVirtualScreen can hang on some AMD drivers
    // http://crbug.com/227228
    if (safe_to_force_gpu_switch_) {
        int renderer_id = share_group()->GetRendererID();
        int screen;
        CGLGetVirtualScreen(static_cast<CGLContextObj>(context_), &screen);

        if (g_support_renderer_switching && !discrete_pixelformat_ && renderer_id != -1 && (screen != screen_ || renderer_id != renderer_id_)) {
            // Attempt to find a virtual screen that's using the requested renderer,
            // and switch the context to use that screen. Don't attempt to switch if
            // the context requires the discrete GPU.
            CGLPixelFormatObj format = GetPixelFormat();
            int virtual_screen_count;
            if (CGLDescribePixelFormat(format, 0, kCGLPFAVirtualScreenCount,
                    &virtual_screen_count)
                != kCGLNoError)
                return false;

            for (int i = 0; i < virtual_screen_count; ++i) {
                int screen_renderer_id;
                if (CGLDescribePixelFormat(format, i, kCGLPFARendererID,
                        &screen_renderer_id)
                    != kCGLNoError)
                    return false;

                screen_renderer_id &= kCGLRendererIDMatchingMask;
                if (screen_renderer_id == renderer_id) {
                    CGLSetVirtualScreen(static_cast<CGLContextObj>(context_), i);
                    screen_ = i;
                    break;
                }
            }
            renderer_id_ = renderer_id;
        }
    }
    return true;
}

bool GLContextCGL::MakeCurrent(GLSurface* surface)
{
    DCHECK(context_);

    if (!ForceGpuSwitchIfNeeded())
        return false;

    if (IsCurrent(surface))
        return true;

    ScopedReleaseCurrent release_current;
    TRACE_EVENT0("gpu", "GLContextCGL::MakeCurrent");

    if (CGLSetCurrentContext(
            static_cast<CGLContextObj>(context_))
        != kCGLNoError) {
        LOG(ERROR) << "Unable to make gl context current.";
        return false;
    }

    // Set this as soon as the context is current, since we might call into GL.
    SetRealGLApi();

    SetCurrent(surface);
    if (!InitializeDynamicBindings()) {
        return false;
    }

    if (!surface->OnMakeCurrent(this)) {
        LOG(ERROR) << "Unable to make gl context current.";
        return false;
    }

    release_current.Cancel();
    return true;
}

void GLContextCGL::ReleaseCurrent(GLSurface* surface)
{
    if (!IsCurrent(surface))
        return;

    SetCurrent(nullptr);
    CGLSetCurrentContext(nullptr);
}

bool GLContextCGL::IsCurrent(GLSurface* surface)
{
    bool native_context_is_current = CGLGetCurrentContext() == context_;

    // If our context is current then our notion of which GLContext is
    // current must be correct. On the other hand, third-party code
    // using OpenGL might change the current context.
    DCHECK(!native_context_is_current || (GetRealCurrent() == this));

    if (!native_context_is_current)
        return false;

    return true;
}

void* GLContextCGL::GetHandle()
{
    return context_;
}

void GLContextCGL::OnSetSwapInterval(int interval)
{
    DCHECK(IsCurrent(nullptr));
}

void GLContextCGL::SetSafeToForceGpuSwitch()
{
    safe_to_force_gpu_switch_ = true;
}

GLContextCGL::~GLContextCGL()
{
    Destroy();
}

GpuPreference GLContextCGL::GetGpuPreference()
{
    return gpu_preference_;
}

} // namespace gfx
