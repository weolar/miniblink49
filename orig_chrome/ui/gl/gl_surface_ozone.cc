// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_surface.h"

#include <stddef.h>

#include "base/bind.h"
#include "base/callback.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_vector.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/worker_pool.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gl/egl_util.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_image.h"
#include "ui/gl/gl_image_ozone_native_pixmap.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_surface_egl.h"
#include "ui/gl/gl_surface_osmesa.h"
#include "ui/gl/gl_surface_overlay.h"
#include "ui/gl/gl_surface_stub.h"
#include "ui/gl/scoped_binders.h"
#include "ui/gl/scoped_make_current.h"
#include "ui/ozone/public/native_pixmap.h"
#include "ui/ozone/public/ozone_platform.h"
#include "ui/ozone/public/surface_factory_ozone.h"
#include "ui/ozone/public/surface_ozone_egl.h"

using gl::GLImage;

namespace gfx {

namespace {

    // Helper function for base::Bind to create callback to eglChooseConfig.
    bool EglChooseConfig(EGLDisplay display,
        const int32_t* attribs,
        EGLConfig* configs,
        int32_t config_size,
        int32_t* num_configs)
    {
        return eglChooseConfig(display, attribs, configs, config_size, num_configs);
    }

    // Helper function for base::Bind to create callback to eglGetConfigAttrib.
    bool EglGetConfigAttribute(EGLDisplay display,
        EGLConfig config,
        int32_t attribute,
        int32_t* value)
    {
        return eglGetConfigAttrib(display, config, attribute, value);
    }

    void WaitForFence(EGLDisplay display, EGLSyncKHR fence)
    {
        eglClientWaitSyncKHR(display, fence, EGL_SYNC_FLUSH_COMMANDS_BIT_KHR,
            EGL_FOREVER_KHR);
    }

    // A thin wrapper around GLSurfaceEGL that owns the EGLNativeWindow.
    class GL_EXPORT GLSurfaceOzoneEGL : public NativeViewGLSurfaceEGL {
    public:
        GLSurfaceOzoneEGL(scoped_ptr<ui::SurfaceOzoneEGL> ozone_surface,
            AcceleratedWidget widget);

        // GLSurface:
        bool Initialize(gfx::GLSurface::Format format) override;
        bool Resize(const gfx::Size& size,
            float scale_factor,
            bool has_alpha) override;
        gfx::SwapResult SwapBuffers() override;
        bool ScheduleOverlayPlane(int z_order,
            OverlayTransform transform,
            GLImage* image,
            const Rect& bounds_rect,
            const RectF& crop_rect) override;
        EGLConfig GetConfig() override;

    private:
        using NativeViewGLSurfaceEGL::Initialize;

        ~GLSurfaceOzoneEGL() override;

        bool ReinitializeNativeSurface();

        // The native surface. Deleting this is allowed to free the EGLNativeWindow.
        scoped_ptr<ui::SurfaceOzoneEGL> ozone_surface_;
        AcceleratedWidget widget_;

        DISALLOW_COPY_AND_ASSIGN(GLSurfaceOzoneEGL);
    };

    GLSurfaceOzoneEGL::GLSurfaceOzoneEGL(
        scoped_ptr<ui::SurfaceOzoneEGL> ozone_surface,
        AcceleratedWidget widget)
        : NativeViewGLSurfaceEGL(ozone_surface->GetNativeWindow())
        , ozone_surface_(std::move(ozone_surface))
        , widget_(widget)
    {
    }

    bool GLSurfaceOzoneEGL::Initialize(gfx::GLSurface::Format format)
    {
        format_ = format;
        return Initialize(ozone_surface_->CreateVSyncProvider());
    }

    bool GLSurfaceOzoneEGL::Resize(const gfx::Size& size,
        float scale_factor,
        bool has_alpha)
    {
        if (!ozone_surface_->ResizeNativeWindow(size)) {
            if (!ReinitializeNativeSurface() || !ozone_surface_->ResizeNativeWindow(size))
                return false;
        }

        return NativeViewGLSurfaceEGL::Resize(size, scale_factor, has_alpha);
    }

    gfx::SwapResult GLSurfaceOzoneEGL::SwapBuffers()
    {
        gfx::SwapResult result = NativeViewGLSurfaceEGL::SwapBuffers();
        if (result != gfx::SwapResult::SWAP_ACK)
            return result;

        return ozone_surface_->OnSwapBuffers() ? gfx::SwapResult::SWAP_ACK
                                               : gfx::SwapResult::SWAP_FAILED;
    }

    bool GLSurfaceOzoneEGL::ScheduleOverlayPlane(int z_order,
        OverlayTransform transform,
        GLImage* image,
        const Rect& bounds_rect,
        const RectF& crop_rect)
    {
        return image->ScheduleOverlayPlane(widget_, z_order, transform, bounds_rect,
            crop_rect);
    }

    EGLConfig GLSurfaceOzoneEGL::GetConfig()
    {
        if (!config_) {
            // Setup callbacks for configuring EGL on platform.
            EGLDisplay display = GetDisplay();
            ui::EglConfigCallbacks egl;
            egl.choose_config = base::Bind(EglChooseConfig, display);
            egl.get_config_attribute = base::Bind(EglGetConfigAttribute, display);
            egl.get_last_error_string = base::Bind(&ui::GetLastEGLErrorString);

            config_ = ozone_surface_->GetEGLSurfaceConfig(egl);
        }
        if (config_)
            return config_;
        return NativeViewGLSurfaceEGL::GetConfig();
    }

    GLSurfaceOzoneEGL::~GLSurfaceOzoneEGL()
    {
        Destroy(); // The EGL surface must be destroyed before SurfaceOzone.
    }

    bool GLSurfaceOzoneEGL::ReinitializeNativeSurface()
    {
        scoped_ptr<ui::ScopedMakeCurrent> scoped_make_current;
        GLContext* current_context = GLContext::GetCurrent();
        bool was_current = current_context && current_context->IsCurrent(this);
        if (was_current) {
            scoped_make_current.reset(new ui::ScopedMakeCurrent(current_context, this));
        }

        Destroy();
        ozone_surface_ = ui::OzonePlatform::GetInstance()
                             ->GetSurfaceFactoryOzone()
                             ->CreateEGLSurfaceForWidget(widget_);
        if (!ozone_surface_) {
            LOG(ERROR) << "Failed to create native surface.";
            return false;
        }

        window_ = ozone_surface_->GetNativeWindow();
        if (!Initialize(format_)) {
            LOG(ERROR) << "Failed to initialize.";
            return false;
        }

        return true;
    }

    class GL_EXPORT GLSurfaceOzoneSurfaceless : public SurfacelessEGL {
    public:
        GLSurfaceOzoneSurfaceless(scoped_ptr<ui::SurfaceOzoneEGL> ozone_surface,
            AcceleratedWidget widget);

        // GLSurface:
        bool Initialize(gfx::GLSurface::Format format) override;
        bool Resize(const gfx::Size& size,
            float scale_factor,
            bool has_alpha) override;
        gfx::SwapResult SwapBuffers() override;
        bool ScheduleOverlayPlane(int z_order,
            OverlayTransform transform,
            GLImage* image,
            const Rect& bounds_rect,
            const RectF& crop_rect) override;
        bool IsOffscreen() override;
        VSyncProvider* GetVSyncProvider() override;
        bool SupportsAsyncSwap() override;
        bool SupportsPostSubBuffer() override;
        gfx::SwapResult PostSubBuffer(int x, int y, int width, int height) override;
        void SwapBuffersAsync(const SwapCompletionCallback& callback) override;
        void PostSubBufferAsync(int x,
            int y,
            int width,
            int height,
            const SwapCompletionCallback& callback) override;

    protected:
        struct PendingFrame {
            PendingFrame();

            bool ScheduleOverlayPlanes(gfx::AcceleratedWidget widget);

            bool ready;
            std::vector<GLSurfaceOverlay> overlays;
            SwapCompletionCallback callback;
        };

        ~GLSurfaceOzoneSurfaceless() override;

        void SubmitFrame();

        EGLSyncKHR InsertFence();
        void FenceRetired(EGLSyncKHR fence, PendingFrame* frame);

        void SwapCompleted(const SwapCompletionCallback& callback,
            gfx::SwapResult result);

        // The native surface. Deleting this is allowed to free the EGLNativeWindow.
        scoped_ptr<ui::SurfaceOzoneEGL> ozone_surface_;
        AcceleratedWidget widget_;
        scoped_ptr<VSyncProvider> vsync_provider_;
        ScopedVector<PendingFrame> unsubmitted_frames_;
        bool has_implicit_external_sync_;
        bool last_swap_buffers_result_;
        bool swap_buffers_pending_;

        base::WeakPtrFactory<GLSurfaceOzoneSurfaceless> weak_factory_;

    private:
        DISALLOW_COPY_AND_ASSIGN(GLSurfaceOzoneSurfaceless);
    };

    GLSurfaceOzoneSurfaceless::PendingFrame::PendingFrame()
        : ready(false)
    {
    }

    bool GLSurfaceOzoneSurfaceless::PendingFrame::ScheduleOverlayPlanes(
        gfx::AcceleratedWidget widget)
    {
        for (const auto& overlay : overlays)
            if (!overlay.ScheduleOverlayPlane(widget))
                return false;
        return true;
    }

    GLSurfaceOzoneSurfaceless::GLSurfaceOzoneSurfaceless(
        scoped_ptr<ui::SurfaceOzoneEGL> ozone_surface,
        AcceleratedWidget widget)
        : SurfacelessEGL(gfx::Size())
        , ozone_surface_(std::move(ozone_surface))
        , widget_(widget)
        , has_implicit_external_sync_(
              HasEGLExtension("EGL_ARM_implicit_external_sync"))
        , last_swap_buffers_result_(true)
        , swap_buffers_pending_(false)
        , weak_factory_(this)
    {
        unsubmitted_frames_.push_back(new PendingFrame());
    }

    bool GLSurfaceOzoneSurfaceless::Initialize(gfx::GLSurface::Format format)
    {
        if (!SurfacelessEGL::Initialize(format))
            return false;
        vsync_provider_ = ozone_surface_->CreateVSyncProvider();
        if (!vsync_provider_)
            return false;
        return true;
    }

    bool GLSurfaceOzoneSurfaceless::Resize(const gfx::Size& size,
        float scale_factor,
        bool has_alpha)
    {
        if (!ozone_surface_->ResizeNativeWindow(size))
            return false;

        return SurfacelessEGL::Resize(size, scale_factor, has_alpha);
    }

    gfx::SwapResult GLSurfaceOzoneSurfaceless::SwapBuffers()
    {
        glFlush();
        // TODO: the following should be replaced by a per surface flush as it gets
        // implemented in GL drivers.
        if (has_implicit_external_sync_) {
            EGLSyncKHR fence = InsertFence();
            if (!fence)
                return SwapResult::SWAP_FAILED;

            EGLDisplay display = GetDisplay();
            WaitForFence(display, fence);
            eglDestroySyncKHR(display, fence);
        }

        unsubmitted_frames_.back()->ScheduleOverlayPlanes(widget_);
        unsubmitted_frames_.back()->overlays.clear();

        if (ozone_surface_->IsUniversalDisplayLinkDevice())
            glFinish();

        return ozone_surface_->OnSwapBuffers() ? gfx::SwapResult::SWAP_ACK
                                               : gfx::SwapResult::SWAP_FAILED;
    }

    bool GLSurfaceOzoneSurfaceless::ScheduleOverlayPlane(int z_order,
        OverlayTransform transform,
        GLImage* image,
        const Rect& bounds_rect,
        const RectF& crop_rect)
    {
        unsubmitted_frames_.back()->overlays.push_back(
            GLSurfaceOverlay(z_order, transform, image, bounds_rect, crop_rect));
        return true;
    }

    bool GLSurfaceOzoneSurfaceless::IsOffscreen()
    {
        return false;
    }

    VSyncProvider* GLSurfaceOzoneSurfaceless::GetVSyncProvider()
    {
        return vsync_provider_.get();
    }

    bool GLSurfaceOzoneSurfaceless::SupportsAsyncSwap()
    {
        return true;
    }

    bool GLSurfaceOzoneSurfaceless::SupportsPostSubBuffer()
    {
        return true;
    }

    gfx::SwapResult GLSurfaceOzoneSurfaceless::PostSubBuffer(int x,
        int y,
        int width,
        int height)
    {
        // The actual sub buffer handling is handled at higher layers.
        NOTREACHED();
        return gfx::SwapResult::SWAP_FAILED;
    }

    void GLSurfaceOzoneSurfaceless::SwapBuffersAsync(
        const SwapCompletionCallback& callback)
    {
        // If last swap failed, don't try to schedule new ones.
        if (!last_swap_buffers_result_) {
            callback.Run(gfx::SwapResult::SWAP_FAILED);
            return;
        }

        glFlush();

        SwapCompletionCallback surface_swap_callback = base::Bind(&GLSurfaceOzoneSurfaceless::SwapCompleted,
            weak_factory_.GetWeakPtr(), callback);

        PendingFrame* frame = unsubmitted_frames_.back();
        frame->callback = surface_swap_callback;
        unsubmitted_frames_.push_back(new PendingFrame());

        // TODO: the following should be replaced by a per surface flush as it gets
        // implemented in GL drivers.
        if (has_implicit_external_sync_) {
            EGLSyncKHR fence = InsertFence();
            if (!fence) {
                callback.Run(gfx::SwapResult::SWAP_FAILED);
                return;
            }

            base::Closure fence_wait_task = base::Bind(&WaitForFence, GetDisplay(), fence);

            base::Closure fence_retired_callback = base::Bind(&GLSurfaceOzoneSurfaceless::FenceRetired,
                weak_factory_.GetWeakPtr(), fence, frame);

            base::WorkerPool::PostTaskAndReply(FROM_HERE, fence_wait_task,
                fence_retired_callback, false);
            return; // Defer frame submission until fence signals.
        }

        frame->ready = true;
        SubmitFrame();
    }

    void GLSurfaceOzoneSurfaceless::PostSubBufferAsync(
        int x,
        int y,
        int width,
        int height,
        const SwapCompletionCallback& callback)
    {
        // The actual sub buffer handling is handled at higher layers.
        SwapBuffersAsync(callback);
    }

    GLSurfaceOzoneSurfaceless::~GLSurfaceOzoneSurfaceless()
    {
        Destroy(); // The EGL surface must be destroyed before SurfaceOzone.
    }

    void GLSurfaceOzoneSurfaceless::SubmitFrame()
    {
        DCHECK(!unsubmitted_frames_.empty());

        if (unsubmitted_frames_.front()->ready && !swap_buffers_pending_) {
            scoped_ptr<PendingFrame> frame(unsubmitted_frames_.front());
            unsubmitted_frames_.weak_erase(unsubmitted_frames_.begin());
            swap_buffers_pending_ = true;

            if (!frame->ScheduleOverlayPlanes(widget_)) {
                // |callback| is a wrapper for SwapCompleted(). Call it to properly
                // propagate the failed state.
                frame->callback.Run(gfx::SwapResult::SWAP_FAILED);
                return;
            }

            if (ozone_surface_->IsUniversalDisplayLinkDevice())
                glFinish();

            ozone_surface_->OnSwapBuffersAsync(frame->callback);
        }
    }

    EGLSyncKHR GLSurfaceOzoneSurfaceless::InsertFence()
    {
        const EGLint attrib_list[] = { EGL_SYNC_CONDITION_KHR,
            EGL_SYNC_PRIOR_COMMANDS_IMPLICIT_EXTERNAL_ARM,
            EGL_NONE };
        return eglCreateSyncKHR(GetDisplay(), EGL_SYNC_FENCE_KHR, attrib_list);
    }

    void GLSurfaceOzoneSurfaceless::FenceRetired(EGLSyncKHR fence,
        PendingFrame* frame)
    {
        eglDestroySyncKHR(GetDisplay(), fence);
        frame->ready = true;
        SubmitFrame();
    }

    void GLSurfaceOzoneSurfaceless::SwapCompleted(
        const SwapCompletionCallback& callback,
        gfx::SwapResult result)
    {
        callback.Run(result);
        swap_buffers_pending_ = false;
        if (result == gfx::SwapResult::SWAP_FAILED) {
            last_swap_buffers_result_ = false;
            return;
        }

        SubmitFrame();
    }

    // This provides surface-like semantics implemented through surfaceless.
    // A framebuffer is bound automatically.
    class GL_EXPORT GLSurfaceOzoneSurfacelessSurfaceImpl
        : public GLSurfaceOzoneSurfaceless {
    public:
        GLSurfaceOzoneSurfacelessSurfaceImpl(
            scoped_ptr<ui::SurfaceOzoneEGL> ozone_surface,
            AcceleratedWidget widget);

        // GLSurface:
        unsigned int GetBackingFrameBufferObject() override;
        bool OnMakeCurrent(GLContext* context) override;
        bool Resize(const gfx::Size& size,
            float scale_factor,
            bool has_alpha) override;
        bool SupportsPostSubBuffer() override;
        gfx::SwapResult SwapBuffers() override;
        void SwapBuffersAsync(const SwapCompletionCallback& callback) override;
        void Destroy() override;
        bool IsSurfaceless() const override;

    private:
        ~GLSurfaceOzoneSurfacelessSurfaceImpl() override;

        void BindFramebuffer();
        bool CreatePixmaps();

        scoped_refptr<GLContext> context_;
        GLuint fbo_;
        GLuint textures_[2];
        scoped_refptr<GLImage> images_[2];
        int current_surface_;
        DISALLOW_COPY_AND_ASSIGN(GLSurfaceOzoneSurfacelessSurfaceImpl);
    };

    GLSurfaceOzoneSurfacelessSurfaceImpl::GLSurfaceOzoneSurfacelessSurfaceImpl(
        scoped_ptr<ui::SurfaceOzoneEGL> ozone_surface,
        AcceleratedWidget widget)
        : GLSurfaceOzoneSurfaceless(std::move(ozone_surface), widget)
        , context_(nullptr)
        , fbo_(0)
        , current_surface_(0)
    {
        for (auto& texture : textures_)
            texture = 0;
    }

    unsigned int
    GLSurfaceOzoneSurfacelessSurfaceImpl::GetBackingFrameBufferObject()
    {
        return fbo_;
    }

    bool GLSurfaceOzoneSurfacelessSurfaceImpl::OnMakeCurrent(GLContext* context)
    {
        DCHECK(!context_ || context == context_);
        context_ = context;
        if (!fbo_) {
            glGenFramebuffersEXT(1, &fbo_);
            if (!fbo_)
                return false;
            glGenTextures(arraysize(textures_), textures_);
            if (!CreatePixmaps())
                return false;
        }
        BindFramebuffer();
        glBindFramebufferEXT(GL_FRAMEBUFFER, fbo_);
        return SurfacelessEGL::OnMakeCurrent(context);
    }

    bool GLSurfaceOzoneSurfacelessSurfaceImpl::Resize(const gfx::Size& size,
        float scale_factor,
        bool has_alpha)
    {
        if (size == GetSize())
            return true;
        // Alpha value isn't actually used in allocating buffers yet, so always use
        // true instead.
        return GLSurfaceOzoneSurfaceless::Resize(size, scale_factor, true) && CreatePixmaps();
    }

    bool GLSurfaceOzoneSurfacelessSurfaceImpl::SupportsPostSubBuffer()
    {
        return false;
    }

    gfx::SwapResult GLSurfaceOzoneSurfacelessSurfaceImpl::SwapBuffers()
    {
        if (!images_[current_surface_]->ScheduleOverlayPlane(
                widget_, 0, OverlayTransform::OVERLAY_TRANSFORM_NONE,
                gfx::Rect(GetSize()), gfx::RectF(1, 1)))
            return gfx::SwapResult::SWAP_FAILED;
        gfx::SwapResult result = GLSurfaceOzoneSurfaceless::SwapBuffers();
        if (result != gfx::SwapResult::SWAP_ACK)
            return result;
        current_surface_ ^= 1;
        BindFramebuffer();
        return gfx::SwapResult::SWAP_ACK;
    }

    void GLSurfaceOzoneSurfacelessSurfaceImpl::SwapBuffersAsync(
        const SwapCompletionCallback& callback)
    {
        if (!images_[current_surface_]->ScheduleOverlayPlane(
                widget_, 0, OverlayTransform::OVERLAY_TRANSFORM_NONE,
                gfx::Rect(GetSize()), gfx::RectF(1, 1))) {
            callback.Run(gfx::SwapResult::SWAP_FAILED);
            return;
        }
        GLSurfaceOzoneSurfaceless::SwapBuffersAsync(callback);
        current_surface_ ^= 1;
        BindFramebuffer();
    }

    void GLSurfaceOzoneSurfacelessSurfaceImpl::Destroy()
    {
        if (!context_)
            return;
        scoped_refptr<gfx::GLContext> previous_context = gfx::GLContext::GetCurrent();
        scoped_refptr<gfx::GLSurface> previous_surface = gfx::GLSurface::GetCurrent();
        context_->MakeCurrent(this);

        glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
        if (fbo_) {
            glDeleteTextures(arraysize(textures_), textures_);
            for (auto& texture : textures_)
                texture = 0;
            glDeleteFramebuffersEXT(1, &fbo_);
            fbo_ = 0;
        }
        for (auto image : images_) {
            if (image)
                image->Destroy(true);
        }

        if (previous_context.get()) {
            previous_context->MakeCurrent(previous_surface.get());
        } else {
            context_->ReleaseCurrent(this);
        }
    }

    bool GLSurfaceOzoneSurfacelessSurfaceImpl::IsSurfaceless() const
    {
        return false;
    }

    GLSurfaceOzoneSurfacelessSurfaceImpl::~GLSurfaceOzoneSurfacelessSurfaceImpl()
    {
        Destroy();
    }

    void GLSurfaceOzoneSurfacelessSurfaceImpl::BindFramebuffer()
    {
        ScopedFrameBufferBinder fb(fbo_);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
            textures_[current_surface_], 0);
    }

    bool GLSurfaceOzoneSurfacelessSurfaceImpl::CreatePixmaps()
    {
        if (!fbo_)
            return true;
        for (size_t i = 0; i < arraysize(textures_); i++) {
            scoped_refptr<ui::NativePixmap> pixmap = ui::OzonePlatform::GetInstance()
                                                         ->GetSurfaceFactoryOzone()
                                                         ->CreateNativePixmap(widget_, GetSize(),
                                                             gfx::BufferFormat::BGRA_8888,
                                                             gfx::BufferUsage::SCANOUT);
            if (!pixmap)
                return false;
            scoped_refptr<GLImageOzoneNativePixmap> image = new GLImageOzoneNativePixmap(GetSize(), GL_BGRA_EXT);
            if (!image->Initialize(pixmap.get(), gfx::BufferFormat::BGRA_8888))
                return false;
            images_[i] = image;
            // Bind image to texture.
            ScopedTextureBinder binder(GL_TEXTURE_2D, textures_[i]);
            if (!images_[i]->BindTexImage(GL_TEXTURE_2D))
                return false;
        }
        return true;
    }

    scoped_refptr<GLSurface> CreateViewGLSurfaceOzone(
        gfx::AcceleratedWidget window)
    {
        scoped_ptr<ui::SurfaceOzoneEGL> surface_ozone = ui::OzonePlatform::GetInstance()
                                                            ->GetSurfaceFactoryOzone()
                                                            ->CreateEGLSurfaceForWidget(window);
        if (!surface_ozone)
            return nullptr;
        scoped_refptr<GLSurface> surface = new GLSurfaceOzoneEGL(std::move(surface_ozone), window);
        if (!surface->Initialize())
            return nullptr;
        return surface;
    }

    scoped_refptr<GLSurface> CreateViewGLSurfaceOzoneSurfacelessSurfaceImpl(
        gfx::AcceleratedWidget window)
    {
        scoped_ptr<ui::SurfaceOzoneEGL> surface_ozone = ui::OzonePlatform::GetInstance()
                                                            ->GetSurfaceFactoryOzone()
                                                            ->CreateSurfacelessEGLSurfaceForWidget(window);
        if (!surface_ozone)
            return nullptr;
        scoped_refptr<GLSurface> surface = new GLSurfaceOzoneSurfacelessSurfaceImpl(
            std::move(surface_ozone), window);
        if (!surface->Initialize())
            return nullptr;
        return surface;
    }

} // namespace

// static
bool GLSurface::InitializeOneOffInternal()
{
    switch (GetGLImplementation()) {
    case kGLImplementationEGLGLES2:
        if (!GLSurfaceEGL::InitializeOneOff()) {
            LOG(ERROR) << "GLSurfaceEGL::InitializeOneOff failed.";
            return false;
        }

        return true;
    case kGLImplementationOSMesaGL:
    case kGLImplementationMockGL:
        return true;
    default:
        return false;
    }
}

// static
scoped_refptr<GLSurface> GLSurface::CreateSurfacelessViewGLSurface(
    gfx::AcceleratedWidget window)
{
    if (GetGLImplementation() == kGLImplementationEGLGLES2 && window != kNullAcceleratedWidget && GLSurfaceEGL::IsEGLSurfacelessContextSupported()) {
        scoped_ptr<ui::SurfaceOzoneEGL> surface_ozone = ui::OzonePlatform::GetInstance()
                                                            ->GetSurfaceFactoryOzone()
                                                            ->CreateSurfacelessEGLSurfaceForWidget(window);
        if (!surface_ozone)
            return nullptr;
        scoped_refptr<GLSurface> surface;
        surface = new GLSurfaceOzoneSurfaceless(std::move(surface_ozone), window);
        if (surface->Initialize())
            return surface;
    }

    return nullptr;
}

// static
scoped_refptr<GLSurface> GLSurface::CreateViewGLSurface(
    gfx::AcceleratedWidget window)
{
    if (GetGLImplementation() == kGLImplementationOSMesaGL) {
        scoped_refptr<GLSurface> surface(new GLSurfaceOSMesaHeadless());
        if (!surface->Initialize())
            return nullptr;
        return surface;
    }
    DCHECK(GetGLImplementation() == kGLImplementationEGLGLES2);
    if (window != kNullAcceleratedWidget) {
        scoped_refptr<GLSurface> surface;
        if (GLSurfaceEGL::IsEGLSurfacelessContextSupported())
            surface = CreateViewGLSurfaceOzoneSurfacelessSurfaceImpl(window);
        if (!surface)
            surface = CreateViewGLSurfaceOzone(window);
        return surface;
    } else {
        scoped_refptr<GLSurface> surface = new GLSurfaceStub();
        if (surface->Initialize())
            return surface;
    }
    return nullptr;
}

// static
scoped_refptr<GLSurface> GLSurface::CreateOffscreenGLSurface(
    const gfx::Size& size, GLSurface::Format format)
{
    switch (GetGLImplementation()) {
    case kGLImplementationOSMesaGL: {
        scoped_refptr<GLSurface> surface(
            new GLSurfaceOSMesa(OSMesaSurfaceFormatBGRA, size));
        if (!surface->Initialize(format))
            return nullptr;

        return surface;
    }
    case kGLImplementationEGLGLES2: {
        scoped_refptr<GLSurface> surface;
        if (GLSurfaceEGL::IsEGLSurfacelessContextSupported() && (size.width() == 0 && size.height() == 0)) {
            surface = new SurfacelessEGL(size);
        } else {
            surface = new PbufferGLSurfaceEGL(size);
        }

        if (!surface->Initialize(format))
            return nullptr;
        return surface;
    }
    case kGLImplementationMockGL:
        return new GLSurfaceStub;
    default:
        NOTREACHED();
        return nullptr;
    }
}

EGLNativeDisplayType GetPlatformDefaultEGLNativeDisplay()
{
    return ui::OzonePlatform::GetInstance()
        ->GetSurfaceFactoryOzone()
        ->GetNativeDisplay();
}

} // namespace gfx
