// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_GPU_CLIENT_GRCONTEXT_FOR_WEBGRAPHICSCONTEXT3D_H_
#define CONTENT_COMMON_GPU_CLIENT_GRCONTEXT_FOR_WEBGRAPHICSCONTEXT3D_H_

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/threading/thread_checker.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/gpu/gl/GrGLInterface.h"

class GrContext;

namespace gpu_blink {
class WebGraphicsContext3DImpl;
}

namespace content {

// Wrap WebGraphicsContext3DImpl into a GrGLInterface object, which allows
// the WebGraphicsContext3DImpl to be jointly refcounted (indirectly)
// by the GrContext and the context provider. This makes it legal for the
// GrContext to be invoked when it outlives the context provider that created
// it. By doing this we no longer have to worry about use after free errors
// caused a lack of consideration for object destruction order.
class GrGLInterfaceForWebGraphicsContext3D final : public GrGLInterface {
public:
    GrGLInterfaceForWebGraphicsContext3D(
        scoped_ptr<gpu_blink::WebGraphicsContext3DImpl> context3d);
    ~GrGLInterfaceForWebGraphicsContext3D() final;

    void BindToCurrentThread();

    gpu_blink::WebGraphicsContext3DImpl* WebContext3D() const
    {
        return context3d_.get();
    }

private:
    base::ThreadChecker context_thread_checker_;
    scoped_ptr<gpu_blink::WebGraphicsContext3DImpl> context3d_;
};

// This class binds an offscreen GrContext to an offscreen context3d. The
// context3d is used by the GrContext so must be valid as long as this class
// is alive.
class GrContextForWebGraphicsContext3D {
public:
    explicit GrContextForWebGraphicsContext3D(
        skia::RefPtr<GrGLInterfaceForWebGraphicsContext3D> context3d);
    virtual ~GrContextForWebGraphicsContext3D();

    GrContext* get() { return gr_context_.get(); }

    void OnLostContext();
    void FreeGpuResources();

private:
    skia::RefPtr<class GrContext> gr_context_;

    DISALLOW_COPY_AND_ASSIGN(GrContextForWebGraphicsContext3D);
};

} // namespace content

#endif // CONTENT_COMMON_GPU_CLIENT_GRCONTEXT_FOR_WEBGRAPHICSCONTEXT3D_H_
