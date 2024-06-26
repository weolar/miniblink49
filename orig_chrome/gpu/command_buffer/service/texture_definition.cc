// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/texture_definition.h"

#include <list>

#include "base/lazy_instance.h"
#include "base/memory/linked_ptr.h"
#include "base/memory/scoped_ptr.h"
#include "base/synchronization/lock.h"
#include "base/threading/thread_local.h"
#include "gpu/command_buffer/service/texture_manager.h"
#include "ui/gl/gl_image.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/scoped_binders.h"

#if !defined(OS_MACOSX)
#include "ui/gl/gl_surface_egl.h"
#endif

namespace gpu {
namespace gles2 {

    namespace {

        class GLImageSync : public gl::GLImage {
        public:
            explicit GLImageSync(const scoped_refptr<NativeImageBuffer>& buffer,
                const gfx::Size& size);

            // Implement GLImage.
            void Destroy(bool have_context) override;
            gfx::Size GetSize() override;
            unsigned GetInternalFormat() override;
            bool BindTexImage(unsigned target) override;
            void ReleaseTexImage(unsigned target) override;
            bool CopyTexImage(unsigned target) override;
            bool CopyTexSubImage(unsigned target,
                const gfx::Point& offset,
                const gfx::Rect& rect) override;
            bool ScheduleOverlayPlane(gfx::AcceleratedWidget widget,
                int z_order,
                gfx::OverlayTransform transform,
                const gfx::Rect& bounds_rect,
                const gfx::RectF& crop_rect) override;
            void OnMemoryDump(base::trace_event::ProcessMemoryDump* pmd,
                uint64_t process_tracing_id,
                const std::string& dump_name) override;

        protected:
            ~GLImageSync() override;

        private:
            scoped_refptr<NativeImageBuffer> buffer_;
            gfx::Size size_;

            DISALLOW_COPY_AND_ASSIGN(GLImageSync);
        };

        GLImageSync::GLImageSync(const scoped_refptr<NativeImageBuffer>& buffer,
            const gfx::Size& size)
            : buffer_(buffer)
            , size_(size)
        {
            if (buffer.get())
                buffer->AddClient(this);
        }

        GLImageSync::~GLImageSync()
        {
            if (buffer_.get())
                buffer_->RemoveClient(this);
        }

        void GLImageSync::Destroy(bool have_context)
        {
        }

        gfx::Size GLImageSync::GetSize()
        {
            return size_;
        }

        unsigned GLImageSync::GetInternalFormat()
        {
            return GL_RGBA;
        }

        bool GLImageSync::BindTexImage(unsigned target)
        {
            NOTREACHED();
            return false;
        }

        void GLImageSync::ReleaseTexImage(unsigned target)
        {
            NOTREACHED();
        }

        bool GLImageSync::CopyTexImage(unsigned target)
        {
            return false;
        }

        bool GLImageSync::CopyTexSubImage(unsigned target,
            const gfx::Point& offset,
            const gfx::Rect& rect)
        {
            return false;
        }

        bool GLImageSync::ScheduleOverlayPlane(gfx::AcceleratedWidget widget,
            int z_order,
            gfx::OverlayTransform transform,
            const gfx::Rect& bounds_rect,
            const gfx::RectF& crop_rect)
        {
            NOTREACHED();
            return false;
        }

        void GLImageSync::OnMemoryDump(base::trace_event::ProcessMemoryDump* pmd,
            uint64_t process_tracing_id,
            const std::string& dump_name)
        {
            // TODO(ericrk): Implement GLImage OnMemoryDump. crbug.com/514914
        }

#if !defined(OS_MACOSX)
        class NativeImageBufferEGL : public NativeImageBuffer {
        public:
            static scoped_refptr<NativeImageBufferEGL> Create(GLuint texture_id);

        private:
            NativeImageBufferEGL(EGLDisplay display, EGLImageKHR image);
            ~NativeImageBufferEGL() override;
            void AddClient(gl::GLImage* client) override;
            void RemoveClient(gl::GLImage* client) override;
            bool IsClient(gl::GLImage* client) override;
            void BindToTexture(GLenum target) const override;

            const EGLDisplay egl_display_;
            const EGLImageKHR egl_image_;

            base::Lock lock_;

            struct ClientInfo {
                explicit ClientInfo(gl::GLImage* client);
                ~ClientInfo();

                gl::GLImage* client;
                bool needs_wait_before_read;
            };
            std::list<ClientInfo> client_infos_;
            gl::GLImage* write_client_;

            DISALLOW_COPY_AND_ASSIGN(NativeImageBufferEGL);
        };

        scoped_refptr<NativeImageBufferEGL> NativeImageBufferEGL::Create(
            GLuint texture_id)
        {
            EGLDisplay egl_display = gfx::GLSurfaceEGL::GetHardwareDisplay();
            EGLContext egl_context = eglGetCurrentContext();

            DCHECK_NE(EGL_NO_CONTEXT, egl_context);
            DCHECK_NE(EGL_NO_DISPLAY, egl_display);
            DCHECK(glIsTexture(texture_id));

            DCHECK(gfx::g_driver_egl.ext.b_EGL_KHR_image_base && gfx::g_driver_egl.ext.b_EGL_KHR_gl_texture_2D_image && gfx::g_driver_gl.ext.b_GL_OES_EGL_image);

            const EGLint egl_attrib_list[] = {
                EGL_GL_TEXTURE_LEVEL_KHR, 0, EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE
            };
            EGLClientBuffer egl_buffer = reinterpret_cast<EGLClientBuffer>(texture_id);
            EGLenum egl_target = EGL_GL_TEXTURE_2D_KHR;

            EGLImageKHR egl_image = eglCreateImageKHR(
                egl_display, egl_context, egl_target, egl_buffer, egl_attrib_list);

            if (egl_image == EGL_NO_IMAGE_KHR) {
                LOG(ERROR) << "eglCreateImageKHR for cross-thread sharing failed: 0x"
                           << std::hex << eglGetError();
                return NULL;
            }

            return new NativeImageBufferEGL(egl_display, egl_image);
        }

        NativeImageBufferEGL::ClientInfo::ClientInfo(gl::GLImage* client)
            : client(client)
            , needs_wait_before_read(true)
        {
        }

        NativeImageBufferEGL::ClientInfo::~ClientInfo() { }

        NativeImageBufferEGL::NativeImageBufferEGL(EGLDisplay display,
            EGLImageKHR image)
            : NativeImageBuffer()
            , egl_display_(display)
            , egl_image_(image)
            , write_client_(NULL)
        {
            DCHECK(egl_display_ != EGL_NO_DISPLAY);
            DCHECK(egl_image_ != EGL_NO_IMAGE_KHR);
        }

        NativeImageBufferEGL::~NativeImageBufferEGL()
        {
            DCHECK(client_infos_.empty());
            if (egl_image_ != EGL_NO_IMAGE_KHR)
                eglDestroyImageKHR(egl_display_, egl_image_);
        }

        void NativeImageBufferEGL::AddClient(gl::GLImage* client)
        {
            base::AutoLock lock(lock_);
            client_infos_.push_back(ClientInfo(client));
        }

        void NativeImageBufferEGL::RemoveClient(gl::GLImage* client)
        {
            base::AutoLock lock(lock_);
            if (write_client_ == client)
                write_client_ = NULL;
            for (std::list<ClientInfo>::iterator it = client_infos_.begin();
                 it != client_infos_.end();
                 it++) {
                if (it->client == client) {
                    client_infos_.erase(it);
                    return;
                }
            }
            NOTREACHED();
        }

        bool NativeImageBufferEGL::IsClient(gl::GLImage* client)
        {
            base::AutoLock lock(lock_);
            for (std::list<ClientInfo>::iterator it = client_infos_.begin();
                 it != client_infos_.end();
                 it++) {
                if (it->client == client)
                    return true;
            }
            return false;
        }

        void NativeImageBufferEGL::BindToTexture(GLenum target) const
        {
            DCHECK(egl_image_ != EGL_NO_IMAGE_KHR);
            glEGLImageTargetTexture2DOES(target, egl_image_);
            DCHECK_EQ(static_cast<EGLint>(EGL_SUCCESS), eglGetError());
            DCHECK_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
        }

#endif

        class NativeImageBufferStub : public NativeImageBuffer {
        public:
            NativeImageBufferStub()
                : NativeImageBuffer()
            {
            }

        private:
            ~NativeImageBufferStub() override { }
            void AddClient(gl::GLImage* client) override { }
            void RemoveClient(gl::GLImage* client) override { }
            bool IsClient(gl::GLImage* client) override { return true; }
            void BindToTexture(GLenum target) const override { }

            DISALLOW_COPY_AND_ASSIGN(NativeImageBufferStub);
        };

        bool g_avoid_egl_target_texture_reuse = false;

    } // anonymous namespace

    // static
    scoped_refptr<NativeImageBuffer> NativeImageBuffer::Create(GLuint texture_id)
    {
        switch (gfx::GetGLImplementation()) {
#if !defined(OS_MACOSX)
        case gfx::kGLImplementationEGLGLES2:
            return NativeImageBufferEGL::Create(texture_id);
#endif
        case gfx::kGLImplementationMockGL:
            return new NativeImageBufferStub;
        default:
            NOTREACHED();
            return NULL;
        }
    }

    // static
    void TextureDefinition::AvoidEGLTargetTextureReuse()
    {
        g_avoid_egl_target_texture_reuse = true;
    }

    TextureDefinition::LevelInfo::LevelInfo()
        : target(0)
        , internal_format(0)
        , width(0)
        , height(0)
        , depth(0)
        , border(0)
        , format(0)
        , type(0)
    {
    }

    TextureDefinition::LevelInfo::LevelInfo(GLenum target,
        GLenum internal_format,
        GLsizei width,
        GLsizei height,
        GLsizei depth,
        GLint border,
        GLenum format,
        GLenum type,
        const gfx::Rect& cleared_rect)
        : target(target)
        , internal_format(internal_format)
        , width(width)
        , height(height)
        , depth(depth)
        , border(border)
        , format(format)
        , type(type)
        , cleared_rect(cleared_rect)
    {
    }

    TextureDefinition::LevelInfo::~LevelInfo() { }

    TextureDefinition::TextureDefinition()
        : version_(0)
        , target_(0)
        , min_filter_(0)
        , mag_filter_(0)
        , wrap_s_(0)
        , wrap_t_(0)
        , usage_(0)
        , immutable_(true)
    {
    }

    TextureDefinition::TextureDefinition(
        Texture* texture,
        unsigned int version,
        const scoped_refptr<NativeImageBuffer>& image_buffer)
        : version_(version)
        , target_(texture->target())
        , image_buffer_(image_buffer)
        , min_filter_(texture->min_filter())
        , mag_filter_(texture->mag_filter())
        , wrap_s_(texture->wrap_s())
        , wrap_t_(texture->wrap_t())
        , usage_(texture->usage())
        , immutable_(texture->IsImmutable())
        , defined_(texture->IsDefined())
    {
        DCHECK(!image_buffer_.get() || defined_);
        if (!image_buffer_.get() && defined_) {
            image_buffer_ = NativeImageBuffer::Create(texture->service_id());
            DCHECK(image_buffer_.get());
        }

        const Texture::FaceInfo& first_face = texture->face_infos_[0];
        if (image_buffer_.get()) {
            scoped_refptr<gl::GLImage> gl_image(new GLImageSync(
                image_buffer_, gfx::Size(first_face.level_infos[0].width, first_face.level_infos[0].height)));
            texture->SetLevelImage(target_, 0, gl_image.get(), Texture::BOUND);
        }

        const Texture::LevelInfo& level = first_face.level_infos[0];
        level_info_ = LevelInfo(level.target, level.internal_format, level.width,
            level.height, level.depth, level.border, level.format,
            level.type, level.cleared_rect);
    }

    TextureDefinition::~TextureDefinition()
    {
    }

    Texture* TextureDefinition::CreateTexture() const
    {
        GLuint texture_id;
        glGenTextures(1, &texture_id);

        Texture* texture(new Texture(texture_id));
        UpdateTextureInternal(texture);

        return texture;
    }

    void TextureDefinition::UpdateTextureInternal(Texture* texture) const
    {
        gfx::ScopedTextureBinder texture_binder(target_, texture->service_id());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t_);

        if (image_buffer_.get()) {
            gl::GLImage* existing_image = texture->GetLevelImage(target_, 0);
            // Don't need to re-bind if already bound before.
            if (!existing_image || !image_buffer_->IsClient(existing_image)) {
                image_buffer_->BindToTexture(target_);
            }
        }

        if (defined_) {
            texture->face_infos_.resize(1);
            texture->face_infos_[0].level_infos.resize(1);
            texture->SetLevelInfo(NULL, level_info_.target, 0,
                level_info_.internal_format, level_info_.width,
                level_info_.height, level_info_.depth,
                level_info_.border, level_info_.format,
                level_info_.type, level_info_.cleared_rect);
        }

        if (image_buffer_.get()) {
            texture->SetLevelImage(
                target_, 0,
                new GLImageSync(image_buffer_,
                    gfx::Size(level_info_.width, level_info_.height)),
                Texture::BOUND);
        }

        texture->target_ = target_;
        texture->SetImmutable(immutable_);
        texture->min_filter_ = min_filter_;
        texture->mag_filter_ = mag_filter_;
        texture->wrap_s_ = wrap_s_;
        texture->wrap_t_ = wrap_t_;
        texture->usage_ = usage_;
    }

    void TextureDefinition::UpdateTexture(Texture* texture) const
    {
        GLuint old_service_id = 0u;
        if (image_buffer_.get() && g_avoid_egl_target_texture_reuse) {
            GLuint service_id = 0u;
            glGenTextures(1, &service_id);
            old_service_id = texture->service_id();
            texture->SetServiceId(service_id);

            DCHECK_EQ(static_cast<GLenum>(GL_TEXTURE_2D), target_);
            GLint bound_id = 0;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound_id);
            if (bound_id == static_cast<GLint>(old_service_id)) {
                glBindTexture(target_, service_id);
            }
            texture->SetLevelImage(target_, 0, NULL, Texture::UNBOUND);
        }

        UpdateTextureInternal(texture);

        if (old_service_id) {
            glDeleteTextures(1, &old_service_id);
        }
    }

    bool TextureDefinition::Matches(const Texture* texture) const
    {
        DCHECK(target_ == texture->target());
        if (texture->min_filter_ != min_filter_ || texture->mag_filter_ != mag_filter_ || texture->wrap_s_ != wrap_s_ || texture->wrap_t_ != wrap_t_ || texture->SafeToRenderFrom() != SafeToRenderFrom()) {
            return false;
        }

        // Texture became defined.
        if (!image_buffer_.get() && texture->IsDefined())
            return false;

        // All structural changes should have orphaned the texture.
        if (image_buffer_.get() && !texture->GetLevelImage(texture->target(), 0))
            return false;

        return true;
    }

    bool TextureDefinition::SafeToRenderFrom() const
    {
        return level_info_.cleared_rect.Contains(
            gfx::Rect(level_info_.width, level_info_.height));
    }

} // namespace gles2
} // namespace gpu
