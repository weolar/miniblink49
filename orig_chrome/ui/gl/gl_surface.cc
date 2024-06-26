// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_surface.h"

#include <algorithm>
#include <vector>

#include "base/command_line.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/threading/thread_local.h"
#include "base/trace_event/trace_event.h"
#include "ui/gfx/swap_result.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_switches.h"

namespace gfx {

namespace {
    base::LazyInstance<base::ThreadLocalPointer<GLSurface>>::Leaky
        current_surface_
        = LAZY_INSTANCE_INITIALIZER;
} // namespace

// static
bool GLSurface::InitializeOneOff()
{
    DCHECK_EQ(kGLImplementationNone, GetGLImplementation());

    TRACE_EVENT0("gpu,startup", "GLSurface::InitializeOneOff");

    std::vector<GLImplementation> allowed_impls;
    GetAllowedGLImplementations(&allowed_impls);
    DCHECK(!allowed_impls.empty());

    base::CommandLine* cmd = base::CommandLine::ForCurrentProcess();

    // The default implementation is always the first one in list.
    GLImplementation impl = allowed_impls[0];
    bool fallback_to_osmesa = false;
    //   if (cmd->HasSwitch(switches::kOverrideUseGLWithOSMesaForTests)) {
    //     impl = kGLImplementationOSMesaGL;
    //   } else if (cmd->HasSwitch(switches::kUseGL)) {
    //     std::string requested_implementation_name = cmd->GetSwitchValueASCII(switches::kUseGL);
    //     if (requested_implementation_name == "any") {
    //       fallback_to_osmesa = true;
    //     } else if (requested_implementation_name == "swiftshader") {
    //       impl = kGLImplementationEGLGLES2;
    //     } else {
    //       impl = GetNamedGLImplementation(requested_implementation_name);
    //       if (std::find(allowed_impls.begin(),
    //                     allowed_impls.end(),
    //                     impl) == allowed_impls.end()) {
    //         LOG(ERROR) << "Requested GL implementation is not available.";
    //         return false;
    //       }
    //     }
    //   }
    impl = kGLImplementationEGLGLES2;

    bool gpu_service_logging = cmd->HasSwitch(switches::kEnableGPUServiceLogging);
    bool disable_gl_drawing = cmd->HasSwitch(switches::kDisableGLDrawingForTests);

    return InitializeOneOffImplementation(
        impl, fallback_to_osmesa, gpu_service_logging, disable_gl_drawing);
}

bool GLSurface::InitializeForce(GLImplementation impl)
{
    return InitializeOneOffImplementation(impl, false, false, false);
}

// static
bool GLSurface::InitializeOneOffImplementation(GLImplementation impl,
    bool fallback_to_osmesa,
    bool gpu_service_logging,
    bool disable_gl_drawing)
{
    bool initialized = InitializeStaticGLBindings(impl) && InitializeOneOffInternal();
    if (!initialized && fallback_to_osmesa) {
        ClearGLBindings();
        initialized = InitializeStaticGLBindings(kGLImplementationOSMesaGL) && InitializeOneOffInternal();
    }
    if (!initialized)
        ClearGLBindings();

    if (initialized) {
        DVLOG(1) << "Using "
                 << GetGLImplementationName(GetGLImplementation())
                 << " GL implementation.";
        if (gpu_service_logging)
            InitializeDebugGLBindings();
        if (disable_gl_drawing)
            InitializeNullDrawGLBindings();
    }
    return initialized;
}

GLSurface::GLSurface() { }

bool GLSurface::Initialize()
{
    return true;
}

bool GLSurface::Resize(const gfx::Size& size, float scale_factor)
{
    NOTIMPLEMENTED();
    return false;
}

bool GLSurface::Recreate()
{
    NOTIMPLEMENTED();
    return false;
}

bool GLSurface::DeferDraws()
{
    return false;
}

bool GLSurface::SupportsPostSubBuffer()
{
    return false;
}

unsigned int GLSurface::GetBackingFrameBufferObject()
{
    return 0;
}

bool GLSurface::SwapBuffersAsync(const SwapCompletionCallback& callback)
{
    DCHECK(!IsSurfaceless());
    gfx::SwapResult result = SwapBuffers();
    callback.Run(result);
    return result == gfx::SwapResult::SWAP_ACK;
}

gfx::SwapResult GLSurface::PostSubBuffer(int x, int y, int width, int height)
{
    return gfx::SwapResult::SWAP_FAILED;
}

bool GLSurface::PostSubBufferAsync(int x,
    int y,
    int width,
    int height,
    const SwapCompletionCallback& callback)
{
    gfx::SwapResult result = PostSubBuffer(x, y, width, height);
    callback.Run(result);
    return result == gfx::SwapResult::SWAP_ACK;
}

bool GLSurface::OnMakeCurrent(GLContext* context)
{
    return true;
}

void GLSurface::NotifyWasBound()
{
}

bool GLSurface::SetBackbufferAllocation(bool allocated)
{
    return true;
}

void GLSurface::SetFrontbufferAllocation(bool allocated)
{
}

void* GLSurface::GetShareHandle()
{
    NOTIMPLEMENTED();
    return NULL;
}

void* GLSurface::GetDisplay()
{
    NOTIMPLEMENTED();
    return NULL;
}

void* GLSurface::GetConfig()
{
    NOTIMPLEMENTED();
    return NULL;
}

unsigned GLSurface::GetFormat()
{
    NOTIMPLEMENTED();
    return 0;
}

VSyncProvider* GLSurface::GetVSyncProvider()
{
    return NULL;
}

bool GLSurface::ScheduleOverlayPlane(int z_order,
    OverlayTransform transform,
    gl::GLImage* image,
    const Rect& bounds_rect,
    const RectF& crop_rect)
{
    NOTIMPLEMENTED();
    return false;
}

bool GLSurface::ScheduleCALayer(gl::GLImage* contents_image,
    const RectF& contents_rect,
    float opacity,
    unsigned background_color,
    const SizeF& bounds_size,
    const gfx::Transform& transform)
{
    NOTIMPLEMENTED();
    return false;
}

bool GLSurface::IsSurfaceless() const
{
    return false;
}

GLSurface* GLSurface::GetCurrent()
{
    return current_surface_.Pointer()->Get();
}

GLSurface::~GLSurface()
{
    if (GetCurrent() == this)
        SetCurrent(NULL);
}

void GLSurface::SetCurrent(GLSurface* surface)
{
    current_surface_.Pointer()->Set(surface);
}

bool GLSurface::ExtensionsContain(const char* c_extensions, const char* name)
{
    DCHECK(name);
    if (!c_extensions)
        return false;
    std::string extensions(c_extensions);
    extensions += " ";

    std::string delimited_name(name);
    delimited_name += " ";

    return extensions.find(delimited_name) != std::string::npos;
}

void GLSurface::OnSetSwapInterval(int interval)
{
}

GLSurfaceAdapter::GLSurfaceAdapter(GLSurface* surface)
    : surface_(surface)
{
}

bool GLSurfaceAdapter::Initialize()
{
    return surface_->Initialize();
}

void GLSurfaceAdapter::Destroy()
{
    surface_->Destroy();
}

bool GLSurfaceAdapter::Resize(const gfx::Size& size, float scale_factor)
{
    return surface_->Resize(size, scale_factor);
}

bool GLSurfaceAdapter::Recreate()
{
    return surface_->Recreate();
}

bool GLSurfaceAdapter::DeferDraws()
{
    return surface_->DeferDraws();
}

bool GLSurfaceAdapter::IsOffscreen()
{
    return surface_->IsOffscreen();
}

gfx::SwapResult GLSurfaceAdapter::SwapBuffers()
{
    return surface_->SwapBuffers();
}

bool GLSurfaceAdapter::SwapBuffersAsync(
    const SwapCompletionCallback& callback)
{
    return surface_->SwapBuffersAsync(callback);
}

gfx::SwapResult GLSurfaceAdapter::PostSubBuffer(int x,
    int y,
    int width,
    int height)
{
    return surface_->PostSubBuffer(x, y, width, height);
}

bool GLSurfaceAdapter::PostSubBufferAsync(
    int x, int y, int width, int height,
    const SwapCompletionCallback& callback)
{
    return surface_->PostSubBufferAsync(x, y, width, height, callback);
}

bool GLSurfaceAdapter::SupportsPostSubBuffer()
{
    return surface_->SupportsPostSubBuffer();
}

gfx::Size GLSurfaceAdapter::GetSize()
{
    return surface_->GetSize();
}

void* GLSurfaceAdapter::GetHandle()
{
    return surface_->GetHandle();
}

unsigned int GLSurfaceAdapter::GetBackingFrameBufferObject()
{
    return surface_->GetBackingFrameBufferObject();
}

bool GLSurfaceAdapter::OnMakeCurrent(GLContext* context)
{
    return surface_->OnMakeCurrent(context);
}

bool GLSurfaceAdapter::SetBackbufferAllocation(bool allocated)
{
    return surface_->SetBackbufferAllocation(allocated);
}

void GLSurfaceAdapter::SetFrontbufferAllocation(bool allocated)
{
    surface_->SetFrontbufferAllocation(allocated);
}

void* GLSurfaceAdapter::GetShareHandle()
{
    return surface_->GetShareHandle();
}

void* GLSurfaceAdapter::GetDisplay()
{
    return surface_->GetDisplay();
}

void* GLSurfaceAdapter::GetConfig()
{
    return surface_->GetConfig();
}

unsigned GLSurfaceAdapter::GetFormat()
{
    return surface_->GetFormat();
}

VSyncProvider* GLSurfaceAdapter::GetVSyncProvider()
{
    return surface_->GetVSyncProvider();
}

bool GLSurfaceAdapter::ScheduleOverlayPlane(int z_order,
    OverlayTransform transform,
    gl::GLImage* image,
    const Rect& bounds_rect,
    const RectF& crop_rect)
{
    return surface_->ScheduleOverlayPlane(
        z_order, transform, image, bounds_rect, crop_rect);
}

bool GLSurfaceAdapter::IsSurfaceless() const
{
    return surface_->IsSurfaceless();
}

GLSurfaceAdapter::~GLSurfaceAdapter() { }

} // namespace gfx
