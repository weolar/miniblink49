// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/test_web_graphics_context_3d.h"

#include <algorithm>
#include <string>

#include "base/bind.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "base/numerics/safe_conversions.h"
#include "cc/test/test_context_support.h"
#include "gpu/GLES2/gl2extchromium.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/khronos/GLES2/gl2ext.h"

namespace cc {

static unsigned s_context_id = 1;

const GLuint TestWebGraphicsContext3D::kExternalTextureId = 1337;

static base::LazyInstance<base::Lock>::Leaky
    g_shared_namespace_lock
    = LAZY_INSTANCE_INITIALIZER;

TestWebGraphicsContext3D::Namespace*
    TestWebGraphicsContext3D::shared_namespace_
    = NULL;

TestWebGraphicsContext3D::Namespace::Namespace()
    : next_buffer_id(1)
    , next_image_id(1)
    , next_texture_id(1)
    , next_renderbuffer_id(1)
{
}

TestWebGraphicsContext3D::Namespace::~Namespace()
{
    g_shared_namespace_lock.Get().AssertAcquired();
    if (shared_namespace_ == this)
        shared_namespace_ = NULL;
}

// static
scoped_ptr<TestWebGraphicsContext3D> TestWebGraphicsContext3D::Create()
{
    return make_scoped_ptr(new TestWebGraphicsContext3D());
}

TestWebGraphicsContext3D::TestWebGraphicsContext3D()
    : context_id_(s_context_id++)
    , times_bind_texture_succeeds_(-1)
    , times_end_query_succeeds_(-1)
    , context_lost_(false)
    , times_map_buffer_chromium_succeeds_(-1)
    , current_used_transfer_buffer_usage_bytes_(0)
    , max_used_transfer_buffer_usage_bytes_(0)
    , next_program_id_(1000)
    , next_shader_id_(2000)
    , next_framebuffer_id_(1)
    , current_framebuffer_(0)
    , max_texture_size_(2048)
    , reshape_called_(false)
    , width_(0)
    , height_(0)
    , scale_factor_(-1.f)
    , test_support_(NULL)
    , last_update_type_(NO_UPDATE)
    , next_insert_sync_point_(1)
    , last_waited_sync_point_(0)
    , unpack_alignment_(4)
    , bound_buffer_(0)
    , weak_ptr_factory_(this)
{
    CreateNamespace();
    set_support_image(true);
}

TestWebGraphicsContext3D::~TestWebGraphicsContext3D()
{
    base::AutoLock lock(g_shared_namespace_lock.Get());
    namespace_ = NULL;
}

void TestWebGraphicsContext3D::CreateNamespace()
{
    base::AutoLock lock(g_shared_namespace_lock.Get());
    if (shared_namespace_) {
        namespace_ = shared_namespace_;
    } else {
        namespace_ = new Namespace;
        shared_namespace_ = namespace_.get();
    }
}

void TestWebGraphicsContext3D::reshapeWithScaleFactor(
    int width, int height, float scale_factor)
{
    reshape_called_ = true;
    width_ = width;
    height_ = height;
    scale_factor_ = scale_factor;
}

bool TestWebGraphicsContext3D::isContextLost()
{
    return context_lost_;
}

GLenum TestWebGraphicsContext3D::checkFramebufferStatus(
    GLenum target)
{
    if (context_lost_)
        return GL_FRAMEBUFFER_UNDEFINED_OES;
    return GL_FRAMEBUFFER_COMPLETE;
}

GLint TestWebGraphicsContext3D::getUniformLocation(
    GLuint program,
    const GLchar* name)
{
    return 0;
}

GLsizeiptr TestWebGraphicsContext3D::getVertexAttribOffset(
    GLuint index,
    GLenum pname)
{
    return 0;
}

GLboolean TestWebGraphicsContext3D::isBuffer(
    GLuint buffer)
{
    return false;
}

GLboolean TestWebGraphicsContext3D::isEnabled(
    GLenum cap)
{
    return false;
}

GLboolean TestWebGraphicsContext3D::isFramebuffer(
    GLuint framebuffer)
{
    return false;
}

GLboolean TestWebGraphicsContext3D::isProgram(
    GLuint program)
{
    return false;
}

GLboolean TestWebGraphicsContext3D::isRenderbuffer(
    GLuint renderbuffer)
{
    return false;
}

GLboolean TestWebGraphicsContext3D::isShader(
    GLuint shader)
{
    return false;
}

GLboolean TestWebGraphicsContext3D::isTexture(
    GLuint texture)
{
    return false;
}

void TestWebGraphicsContext3D::genBuffers(GLsizei count, GLuint* ids)
{
    for (int i = 0; i < count; ++i)
        ids[i] = NextBufferId();
}

void TestWebGraphicsContext3D::genFramebuffers(
    GLsizei count, GLuint* ids)
{
    for (int i = 0; i < count; ++i)
        ids[i] = NextFramebufferId();
}

void TestWebGraphicsContext3D::genRenderbuffers(
    GLsizei count, GLuint* ids)
{
    for (int i = 0; i < count; ++i)
        ids[i] = NextRenderbufferId();
}

void TestWebGraphicsContext3D::genTextures(GLsizei count, GLuint* ids)
{
    for (int i = 0; i < count; ++i) {
        ids[i] = NextTextureId();
        DCHECK_NE(ids[i], kExternalTextureId);
    }
    base::AutoLock lock(namespace_->lock);
    for (int i = 0; i < count; ++i)
        namespace_->textures.Append(ids[i], new TestTexture());
}

void TestWebGraphicsContext3D::deleteBuffers(GLsizei count, GLuint* ids)
{
    for (int i = 0; i < count; ++i)
        RetireBufferId(ids[i]);
}

void TestWebGraphicsContext3D::deleteFramebuffers(
    GLsizei count, GLuint* ids)
{
    for (int i = 0; i < count; ++i) {
        if (ids[i]) {
            RetireFramebufferId(ids[i]);
            if (ids[i] == current_framebuffer_)
                current_framebuffer_ = 0;
        }
    }
}

void TestWebGraphicsContext3D::deleteRenderbuffers(
    GLsizei count, GLuint* ids)
{
    for (int i = 0; i < count; ++i)
        RetireRenderbufferId(ids[i]);
}

void TestWebGraphicsContext3D::deleteTextures(GLsizei count, GLuint* ids)
{
    for (int i = 0; i < count; ++i)
        RetireTextureId(ids[i]);
    base::AutoLock lock(namespace_->lock);
    for (int i = 0; i < count; ++i) {
        namespace_->textures.Remove(ids[i]);
        texture_targets_.UnbindTexture(ids[i]);
    }
}

GLuint TestWebGraphicsContext3D::createBuffer()
{
    GLuint id;
    genBuffers(1, &id);
    return id;
}

GLuint TestWebGraphicsContext3D::createFramebuffer()
{
    GLuint id;
    genFramebuffers(1, &id);
    return id;
}

GLuint TestWebGraphicsContext3D::createRenderbuffer()
{
    GLuint id;
    genRenderbuffers(1, &id);
    return id;
}

GLuint TestWebGraphicsContext3D::createTexture()
{
    GLuint id;
    genTextures(1, &id);
    return id;
}

void TestWebGraphicsContext3D::deleteBuffer(GLuint id)
{
    deleteBuffers(1, &id);
}

void TestWebGraphicsContext3D::deleteFramebuffer(GLuint id)
{
    deleteFramebuffers(1, &id);
}

void TestWebGraphicsContext3D::deleteRenderbuffer(GLuint id)
{
    deleteRenderbuffers(1, &id);
}

void TestWebGraphicsContext3D::deleteTexture(GLuint id)
{
    deleteTextures(1, &id);
}

unsigned TestWebGraphicsContext3D::createProgram()
{
    unsigned program = next_program_id_++ | context_id_ << 16;
    program_set_.insert(program);
    return program;
}

GLuint TestWebGraphicsContext3D::createShader(GLenum)
{
    unsigned shader = next_shader_id_++ | context_id_ << 16;
    shader_set_.insert(shader);
    return shader;
}

GLuint TestWebGraphicsContext3D::createExternalTexture()
{
    base::AutoLock lock(namespace_->lock);
    namespace_->textures.Append(kExternalTextureId, new TestTexture());
    return kExternalTextureId;
}

void TestWebGraphicsContext3D::deleteProgram(GLuint id)
{
    if (!program_set_.count(id))
        ADD_FAILURE() << "deleteProgram called on unknown program " << id;
    program_set_.erase(id);
}

void TestWebGraphicsContext3D::deleteShader(GLuint id)
{
    if (!shader_set_.count(id))
        ADD_FAILURE() << "deleteShader called on unknown shader " << id;
    shader_set_.erase(id);
}

void TestWebGraphicsContext3D::attachShader(GLuint program, GLuint shader)
{
    if (!program_set_.count(program))
        ADD_FAILURE() << "attachShader called with unknown program " << program;
    if (!shader_set_.count(shader))
        ADD_FAILURE() << "attachShader called with unknown shader " << shader;
}

void TestWebGraphicsContext3D::useProgram(GLuint program)
{
    if (!program)
        return;
    if (!program_set_.count(program))
        ADD_FAILURE() << "useProgram called on unknown program " << program;
}

void TestWebGraphicsContext3D::bindFramebuffer(
    GLenum target, GLuint framebuffer)
{
    base::AutoLock lock_for_framebuffer_access(namespace_->lock);
    if (framebuffer != 0 && framebuffer_set_.find(framebuffer) == framebuffer_set_.end()) {
        ADD_FAILURE() << "bindFramebuffer called with unknown framebuffer";
    } else if (framebuffer != 0 && (framebuffer >> 16) != context_id_) {
        ADD_FAILURE()
            << "bindFramebuffer called with framebuffer from other context";
    } else {
        current_framebuffer_ = framebuffer;
    }
}

void TestWebGraphicsContext3D::bindRenderbuffer(
    GLenum target, GLuint renderbuffer)
{
    if (!renderbuffer)
        return;
    base::AutoLock lock_for_renderbuffer_access(namespace_->lock);
    if (renderbuffer != 0 && namespace_->renderbuffer_set.find(renderbuffer) == namespace_->renderbuffer_set.end()) {
        ADD_FAILURE() << "bindRenderbuffer called with unknown renderbuffer";
    } else if ((renderbuffer >> 16) != context_id_) {
        ADD_FAILURE()
            << "bindRenderbuffer called with renderbuffer from other context";
    }
}

void TestWebGraphicsContext3D::bindTexture(
    GLenum target, GLuint texture_id)
{
    if (times_bind_texture_succeeds_ >= 0) {
        if (!times_bind_texture_succeeds_) {
            loseContextCHROMIUM(GL_GUILTY_CONTEXT_RESET_ARB,
                GL_INNOCENT_CONTEXT_RESET_ARB);
        }
        --times_bind_texture_succeeds_;
    }

    if (!texture_id)
        return;
    base::AutoLock lock(namespace_->lock);
    DCHECK(namespace_->textures.ContainsId(texture_id));
    texture_targets_.BindTexture(target, texture_id);
    used_textures_.insert(texture_id);
}

GLuint TestWebGraphicsContext3D::BoundTextureId(
    GLenum target)
{
    return texture_targets_.BoundTexture(target);
}

scoped_refptr<TestTexture> TestWebGraphicsContext3D::BoundTexture(
    GLenum target)
{
    // The caller is expected to lock the namespace for texture access.
    namespace_->lock.AssertAcquired();
    return namespace_->textures.TextureForId(BoundTextureId(target));
}

scoped_refptr<TestTexture> TestWebGraphicsContext3D::UnboundTexture(
    GLuint texture)
{
    // The caller is expected to lock the namespace for texture access.
    namespace_->lock.AssertAcquired();
    return namespace_->textures.TextureForId(texture);
}

void TestWebGraphicsContext3D::CheckTextureIsBound(GLenum target)
{
    DCHECK(BoundTextureId(target));
}

GLuint TestWebGraphicsContext3D::createQueryEXT() { return 1u; }

void TestWebGraphicsContext3D::endQueryEXT(GLenum target)
{
    if (times_end_query_succeeds_ >= 0) {
        if (!times_end_query_succeeds_) {
            loseContextCHROMIUM(GL_GUILTY_CONTEXT_RESET_ARB,
                GL_INNOCENT_CONTEXT_RESET_ARB);
        }
        --times_end_query_succeeds_;
    }
}

void TestWebGraphicsContext3D::getQueryObjectuivEXT(
    GLuint query,
    GLenum pname,
    GLuint* params)
{
    // If the context is lost, behave as if result is available.
    if (pname == GL_QUERY_RESULT_AVAILABLE_EXT)
        *params = 1;
}

void TestWebGraphicsContext3D::getIntegerv(
    GLenum pname,
    GLint* value)
{
    if (pname == GL_MAX_TEXTURE_SIZE)
        *value = max_texture_size_;
    else if (pname == GL_ACTIVE_TEXTURE)
        *value = GL_TEXTURE0;
    else if (pname == GL_UNPACK_ALIGNMENT)
        *value = unpack_alignment_;
    else if (pname == GL_FRAMEBUFFER_BINDING)
        *value = current_framebuffer_;
}

void TestWebGraphicsContext3D::getProgramiv(GLuint program,
    GLenum pname,
    GLint* value)
{
    if (pname == GL_LINK_STATUS)
        *value = 1;
}

void TestWebGraphicsContext3D::getShaderiv(GLuint shader,
    GLenum pname,
    GLint* value)
{
    if (pname == GL_COMPILE_STATUS)
        *value = 1;
}

void TestWebGraphicsContext3D::getShaderPrecisionFormat(GLenum shadertype,
    GLenum precisiontype,
    GLint* range,
    GLint* precision)
{
    // Return the minimum precision requirements of the GLES2
    // specification.
    switch (precisiontype) {
    case GL_LOW_INT:
        range[0] = 8;
        range[1] = 8;
        *precision = 0;
        break;
    case GL_MEDIUM_INT:
        range[0] = 10;
        range[1] = 10;
        *precision = 0;
        break;
    case GL_HIGH_INT:
        range[0] = 16;
        range[1] = 16;
        *precision = 0;
        break;
    case GL_LOW_FLOAT:
        range[0] = 8;
        range[1] = 8;
        *precision = 8;
        break;
    case GL_MEDIUM_FLOAT:
        range[0] = 14;
        range[1] = 14;
        *precision = 10;
        break;
    case GL_HIGH_FLOAT:
        range[0] = 62;
        range[1] = 62;
        *precision = 16;
        break;
    default:
        NOTREACHED();
        break;
    }
}

void TestWebGraphicsContext3D::genMailboxCHROMIUM(GLbyte* mailbox)
{
    static char mailbox_name1 = '1';
    static char mailbox_name2 = '1';
    mailbox[0] = mailbox_name1;
    mailbox[1] = mailbox_name2;
    mailbox[2] = '\0';
    if (++mailbox_name1 == 0) {
        mailbox_name1 = '1';
        ++mailbox_name2;
    }
}

GLuint TestWebGraphicsContext3D::createAndConsumeTextureCHROMIUM(
    GLenum target,
    const GLbyte* mailbox)
{
    GLuint texture_id = createTexture();
    consumeTextureCHROMIUM(target, mailbox);
    return texture_id;
}

void TestWebGraphicsContext3D::loseContextCHROMIUM(GLenum current,
    GLenum other)
{
    if (context_lost_)
        return;
    context_lost_ = true;
    if (!context_lost_callback_.is_null())
        context_lost_callback_.Run();

    for (size_t i = 0; i < shared_contexts_.size(); ++i)
        shared_contexts_[i]->loseContextCHROMIUM(current, other);
    shared_contexts_.clear();
}

void TestWebGraphicsContext3D::finish()
{
    test_support_->CallAllSyncPointCallbacks();
}

void TestWebGraphicsContext3D::flush()
{
    test_support_->CallAllSyncPointCallbacks();
}

void TestWebGraphicsContext3D::shallowFinishCHROMIUM()
{
    test_support_->CallAllSyncPointCallbacks();
}

GLint TestWebGraphicsContext3D::getAttribLocation(GLuint program,
    const GLchar* name)
{
    return 0;
}

GLenum TestWebGraphicsContext3D::getError() { return GL_NO_ERROR; }

void TestWebGraphicsContext3D::bindBuffer(GLenum target,
    GLuint buffer)
{
    bound_buffer_ = buffer;
    if (!bound_buffer_)
        return;
    unsigned context_id = buffer >> 16;
    unsigned buffer_id = buffer & 0xffff;
    base::AutoLock lock(namespace_->lock);
    DCHECK(buffer_id);
    DCHECK_LT(buffer_id, namespace_->next_buffer_id);
    DCHECK_EQ(context_id, context_id_);

    base::ScopedPtrHashMap<unsigned, scoped_ptr<Buffer>>& buffers = namespace_->buffers;
    if (buffers.count(bound_buffer_) == 0)
        buffers.set(bound_buffer_, make_scoped_ptr(new Buffer).Pass());

    buffers.get(bound_buffer_)->target = target;
}

void TestWebGraphicsContext3D::bufferData(GLenum target,
    GLsizeiptr size,
    const void* data,
    GLenum usage)
{
    base::AutoLock lock(namespace_->lock);
    base::ScopedPtrHashMap<unsigned, scoped_ptr<Buffer>>& buffers = namespace_->buffers;
    DCHECK_GT(buffers.count(bound_buffer_), 0u);
    DCHECK_EQ(target, buffers.get(bound_buffer_)->target);
    Buffer* buffer = buffers.get(bound_buffer_);
    if (context_lost_) {
        buffer->pixels = nullptr;
        return;
    }

    size_t old_size = buffer->size;

    buffer->pixels.reset(new uint8[size]);
    buffer->size = size;
    if (data != NULL)
        memcpy(buffer->pixels.get(), data, size);
    if (buffer->target == GL_PIXEL_UNPACK_TRANSFER_BUFFER_CHROMIUM)
        current_used_transfer_buffer_usage_bytes_ += base::checked_cast<int>(buffer->size) - base::checked_cast<int>(old_size);
    max_used_transfer_buffer_usage_bytes_ = std::max(max_used_transfer_buffer_usage_bytes_,
        current_used_transfer_buffer_usage_bytes_);
}

void TestWebGraphicsContext3D::pixelStorei(GLenum pname, GLint param)
{
    switch (pname) {
    case GL_UNPACK_ALIGNMENT:
        // Param should be a power of two <= 8.
        EXPECT_EQ(0, param & (param - 1));
        EXPECT_GE(8, param);
        switch (param) {
        case 1:
        case 2:
        case 4:
        case 8:
            unpack_alignment_ = param;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void* TestWebGraphicsContext3D::mapBufferCHROMIUM(GLenum target,
    GLenum access)
{
    base::AutoLock lock(namespace_->lock);
    base::ScopedPtrHashMap<unsigned, scoped_ptr<Buffer>>& buffers = namespace_->buffers;
    DCHECK_GT(buffers.count(bound_buffer_), 0u);
    DCHECK_EQ(target, buffers.get(bound_buffer_)->target);
    if (times_map_buffer_chromium_succeeds_ >= 0) {
        if (!times_map_buffer_chromium_succeeds_) {
            return NULL;
        }
        --times_map_buffer_chromium_succeeds_;
    }

    return buffers.get(bound_buffer_)->pixels.get();
}

GLboolean TestWebGraphicsContext3D::unmapBufferCHROMIUM(
    GLenum target)
{
    base::AutoLock lock(namespace_->lock);
    base::ScopedPtrHashMap<unsigned, scoped_ptr<Buffer>>& buffers = namespace_->buffers;
    DCHECK_GT(buffers.count(bound_buffer_), 0u);
    DCHECK_EQ(target, buffers.get(bound_buffer_)->target);
    buffers.get(bound_buffer_)->pixels = nullptr;
    return true;
}

GLuint TestWebGraphicsContext3D::createImageCHROMIUM(ClientBuffer buffer,
    GLsizei width,
    GLsizei height,
    GLenum internalformat)
{
    DCHECK_EQ(GL_RGBA, static_cast<int>(internalformat));
    GLuint image_id = NextImageId();
    base::AutoLock lock(namespace_->lock);
    base::hash_set<unsigned>& images = namespace_->images;
    images.insert(image_id);
    return image_id;
}

void TestWebGraphicsContext3D::destroyImageCHROMIUM(
    GLuint id)
{
    RetireImageId(id);
    base::AutoLock lock(namespace_->lock);
    base::hash_set<unsigned>& images = namespace_->images;
    if (!images.count(id))
        ADD_FAILURE() << "destroyImageCHROMIUM called on unknown image " << id;
    images.erase(id);
}

GLuint TestWebGraphicsContext3D::createGpuMemoryBufferImageCHROMIUM(
    GLsizei width,
    GLsizei height,
    GLenum internalformat,
    GLenum usage)
{
    DCHECK_EQ(GL_RGBA, static_cast<int>(internalformat));
    GLuint image_id = NextImageId();
    base::AutoLock lock(namespace_->lock);
    base::hash_set<unsigned>& images = namespace_->images;
    images.insert(image_id);
    return image_id;
}

unsigned TestWebGraphicsContext3D::insertSyncPoint()
{
    return next_insert_sync_point_++;
}

void TestWebGraphicsContext3D::waitSyncPoint(unsigned sync_point)
{
    if (sync_point)
        last_waited_sync_point_ = sync_point;
}

size_t TestWebGraphicsContext3D::NumTextures() const
{
    base::AutoLock lock(namespace_->lock);
    return namespace_->textures.Size();
}

GLuint TestWebGraphicsContext3D::TextureAt(int i) const
{
    base::AutoLock lock(namespace_->lock);
    return namespace_->textures.IdAt(i);
}

GLuint TestWebGraphicsContext3D::NextTextureId()
{
    base::AutoLock lock(namespace_->lock);
    GLuint texture_id = namespace_->next_texture_id++;
    DCHECK(texture_id < (1 << 16));
    texture_id |= context_id_ << 16;
    return texture_id;
}

void TestWebGraphicsContext3D::RetireTextureId(GLuint id)
{
    base::AutoLock lock(namespace_->lock);
    unsigned context_id = id >> 16;
    unsigned texture_id = id & 0xffff;
    DCHECK(texture_id);
    DCHECK_LT(texture_id, namespace_->next_texture_id);
    DCHECK_EQ(context_id, context_id_);
}

GLuint TestWebGraphicsContext3D::NextBufferId()
{
    base::AutoLock lock(namespace_->lock);
    GLuint buffer_id = namespace_->next_buffer_id++;
    DCHECK(buffer_id < (1 << 16));
    buffer_id |= context_id_ << 16;
    return buffer_id;
}

void TestWebGraphicsContext3D::RetireBufferId(GLuint id)
{
    base::AutoLock lock(namespace_->lock);
    unsigned context_id = id >> 16;
    unsigned buffer_id = id & 0xffff;
    DCHECK(buffer_id);
    DCHECK_LT(buffer_id, namespace_->next_buffer_id);
    DCHECK_EQ(context_id, context_id_);
}

GLuint TestWebGraphicsContext3D::NextImageId()
{
    base::AutoLock lock(namespace_->lock);
    GLuint image_id = namespace_->next_image_id++;
    DCHECK(image_id < (1 << 16));
    image_id |= context_id_ << 16;
    return image_id;
}

void TestWebGraphicsContext3D::RetireImageId(GLuint id)
{
    base::AutoLock lock(namespace_->lock);
    unsigned context_id = id >> 16;
    unsigned image_id = id & 0xffff;
    DCHECK(image_id);
    DCHECK_LT(image_id, namespace_->next_image_id);
    DCHECK_EQ(context_id, context_id_);
}

GLuint TestWebGraphicsContext3D::NextFramebufferId()
{
    base::AutoLock lock_for_framebuffer_access(namespace_->lock);
    GLuint id = next_framebuffer_id_++;
    DCHECK(id < (1 << 16));
    id |= context_id_ << 16;
    framebuffer_set_.insert(id);
    return id;
}

void TestWebGraphicsContext3D::RetireFramebufferId(GLuint id)
{
    base::AutoLock lock_for_framebuffer_access(namespace_->lock);
    DCHECK(framebuffer_set_.find(id) != framebuffer_set_.end());
    framebuffer_set_.erase(id);
}

GLuint TestWebGraphicsContext3D::NextRenderbufferId()
{
    base::AutoLock lock_for_renderbuffer_access(namespace_->lock);
    GLuint id = namespace_->next_renderbuffer_id++;
    DCHECK(id < (1 << 16));
    id |= context_id_ << 16;
    namespace_->renderbuffer_set.insert(id);
    return id;
}

void TestWebGraphicsContext3D::RetireRenderbufferId(GLuint id)
{
    base::AutoLock lock_for_renderbuffer_access(namespace_->lock);
    DCHECK(namespace_->renderbuffer_set.find(id) != namespace_->renderbuffer_set.end());
    namespace_->renderbuffer_set.erase(id);
}

void TestWebGraphicsContext3D::SetMaxTransferBufferUsageBytes(
    size_t max_transfer_buffer_usage_bytes)
{
    test_capabilities_.max_transfer_buffer_usage_bytes = max_transfer_buffer_usage_bytes;
}

void TestWebGraphicsContext3D::SetMaxSamples(int max_samples)
{
    test_capabilities_.gpu.max_samples = max_samples;
}

TestWebGraphicsContext3D::TextureTargets::TextureTargets()
{
    // Initialize default bindings.
    bound_textures_[GL_TEXTURE_2D] = 0;
    bound_textures_[GL_TEXTURE_EXTERNAL_OES] = 0;
    bound_textures_[GL_TEXTURE_RECTANGLE_ARB] = 0;
}

TestWebGraphicsContext3D::TextureTargets::~TextureTargets() { }

void TestWebGraphicsContext3D::TextureTargets::BindTexture(
    GLenum target,
    GLuint id)
{
    // Make sure this is a supported target by seeing if it was bound to before.
    DCHECK(bound_textures_.find(target) != bound_textures_.end());
    bound_textures_[target] = id;
}

void TestWebGraphicsContext3D::texParameteri(GLenum target,
    GLenum pname,
    GLint param)
{
    CheckTextureIsBound(target);
    base::AutoLock lock_for_texture_access(namespace_->lock);
    scoped_refptr<TestTexture> texture = BoundTexture(target);
    DCHECK(texture->IsValidParameter(pname));
    texture->params[pname] = param;
}

void TestWebGraphicsContext3D::getTexParameteriv(GLenum target,
    GLenum pname,
    GLint* value)
{
    CheckTextureIsBound(target);
    base::AutoLock lock_for_texture_access(namespace_->lock);
    scoped_refptr<TestTexture> texture = BoundTexture(target);
    DCHECK(texture->IsValidParameter(pname));
    TestTexture::TextureParametersMap::iterator it = texture->params.find(pname);
    if (it != texture->params.end())
        *value = it->second;
}

void TestWebGraphicsContext3D::TextureTargets::UnbindTexture(
    GLuint id)
{
    // Bind zero to any targets that the id is bound to.
    for (TargetTextureMap::iterator it = bound_textures_.begin();
         it != bound_textures_.end();
         it++) {
        if (it->second == id)
            it->second = 0;
    }
}

GLuint TestWebGraphicsContext3D::TextureTargets::BoundTexture(
    GLenum target)
{
    DCHECK(bound_textures_.find(target) != bound_textures_.end());
    return bound_textures_[target];
}

TestWebGraphicsContext3D::Buffer::Buffer()
    : target(0)
    , size(0)
{
}

TestWebGraphicsContext3D::Buffer::~Buffer() { }

TestWebGraphicsContext3D::Image::Image() { }

TestWebGraphicsContext3D::Image::~Image() { }

} // namespace cc
