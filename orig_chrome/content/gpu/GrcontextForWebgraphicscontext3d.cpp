// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/gpu/\GrcontextForWebgraphicscontext3d.h"

#include "base/lazy_instance.h"
#include "base/trace_event/trace_event.h"
#include "gpu/blink/webgraphicscontext3d_impl.h"
#include "gpu/command_buffer/client/gles2_lib.h"
#include "gpu/skia_bindings/gl_bindings_skia_cmd_buffer.h"
#include "third_party/skia/include/gpu/GrContext.h"

using gpu_blink::WebGraphicsContext3DImpl;

namespace content {

namespace {

    // Singleton used to initialize and terminate the gles2 library.
    class GLES2Initializer {
    public:
        GLES2Initializer()
        {
            gles2::Initialize();
        }

        ~GLES2Initializer()
        {
            gles2::Terminate();
        }

    private:
        DISALLOW_COPY_AND_ASSIGN(GLES2Initializer);
    };

    base::LazyInstance<GLES2Initializer> g_gles2_initializer = LAZY_INSTANCE_INITIALIZER;

    void BindWebGraphicsContext3DGLContextCallback(const GrGLInterface* interfacePtr)
    {
        gles2::SetGLContext(static_cast<const GrGLInterfaceForWebGraphicsContext3D*>(interfacePtr)->WebContext3D()->GetGLInterface());
    }

} // namespace anonymous

GrContextForWebGraphicsContext3D::GrContextForWebGraphicsContext3D(skia::RefPtr<GrGLInterfaceForWebGraphicsContext3D> gr_interface)
{
    if (!gr_interface || !gr_interface->WebContext3D())
        return;

    // Ensure the gles2 library is initialized first in a thread safe way.
    g_gles2_initializer.Get();
    gles2::SetGLContext(gr_interface->WebContext3D()->GetGLInterface());

    skia_bindings::InitCommandBufferSkiaGLBinding(gr_interface.get());

    //gr_interface->fCallback = BindWebGraphicsContext3DGLContextCallback;
    DebugBreak();

    gr_context_ = skia::AdoptRef(GrContext::Create(
        kOpenGL_GrBackend,
        reinterpret_cast<GrBackendContext>(gr_interface.get())));
    if (gr_context_) {
        // The limit of the number of GPU resources we hold in the GrContext's
        // GPU cache.
        static const int kMaxGaneshResourceCacheCount = 2048;
        // The limit of the bytes allocated toward GPU resources in the GrContext's
        // GPU cache.
        static const size_t kMaxGaneshResourceCacheBytes = 96 * 1024 * 1024;

        gr_context_->setResourceCacheLimits(kMaxGaneshResourceCacheCount,
            kMaxGaneshResourceCacheBytes);
    }
}

GrContextForWebGraphicsContext3D::~GrContextForWebGraphicsContext3D()
{
}

void GrContextForWebGraphicsContext3D::OnLostContext()
{
    if (gr_context_)
        gr_context_->abandonContext();
}

void GrContextForWebGraphicsContext3D::FreeGpuResources()
{
    if (gr_context_) {
        TRACE_EVENT_INSTANT0("gpu", "GrContext::freeGpuResources", TRACE_EVENT_SCOPE_THREAD);
        gr_context_->freeGpuResources();
    }
}

GrGLInterfaceForWebGraphicsContext3D::GrGLInterfaceForWebGraphicsContext3D(scoped_ptr<gpu_blink::WebGraphicsContext3DImpl> context3d)
    : context3d_(context3d.Pass())
{
}

void GrGLInterfaceForWebGraphicsContext3D::BindToCurrentThread()
{
    context_thread_checker_.DetachFromThread();
}

GrGLInterfaceForWebGraphicsContext3D::~GrGLInterfaceForWebGraphicsContext3D()
{
    DCHECK(context_thread_checker_.CalledOnValidThread());
#if !defined(NDEBUG)
    // Set all the function pointers to zero, in order to crash if function
    // pointers are used after free.
    memset(&fFunctions, 0, sizeof(GrGLInterface::Functions));
#endif
}

} // namespace content
