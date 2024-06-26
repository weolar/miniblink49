// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/resources/resource_provider.h"

#include <algorithm>
#include <limits>

#include "base/atomic_sequence_num.h"
#include "base/bind.h"
#include "base/containers/hash_tables.h"
#include "base/metrics/histogram.h"
#include "base/numerics/safe_math.h"
#include "base/stl_util.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/thread_task_runner_handle.h"
// #include "base/trace_event/memory_dump_manager.h"
#include "base/trace_event/common/trace_event_common.h"
#include "base/trace_event/trace_event.h"
#include "cc/resources/platform_color.h"
#include "cc/resources/resource_util.h"
#include "cc/resources/returned_resource.h"
#include "cc/resources/shared_bitmap_manager.h"
#include "cc/resources/transferable_resource.h"
#include "gpu/GLES2/gl2extchromium.h"
#include "gpu/command_buffer/client/context_support.h"
#include "gpu/command_buffer/client/gles2_interface.h"
#include "gpu/command_buffer/client/gpu_memory_buffer_manager.h"
#include "third_party/khronos/GLES2/gl2.h"
#include "third_party/khronos/GLES2/gl2ext.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/GrContext.h"
#include "third_party/skia/include/gpu/GrTextureProvider.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/vector2d.h"
//#include "ui/gl/trace_util.h"

using gpu::gles2::GLES2Interface;

#define GL_TEXTURE_POOL_UNMANAGED_CHROMIUM 1
#define GL_TEXTURE_POOL_MANAGED_CHROMIUM 2

namespace cc {

class IdAllocator {
public:
    virtual ~IdAllocator() { }

    virtual GLuint NextId() = 0;

protected:
    IdAllocator(GLES2Interface* gl, size_t id_allocation_chunk_size)
        : gl_(gl)
        , id_allocation_chunk_size_(id_allocation_chunk_size)
        , ids_(new GLuint[id_allocation_chunk_size])
        , next_id_index_(id_allocation_chunk_size)
    {
        DCHECK(id_allocation_chunk_size_);
        DCHECK_LE(id_allocation_chunk_size_,
            static_cast<size_t>(std::numeric_limits<int>::max()));
    }

    GLES2Interface* gl_;
    const size_t id_allocation_chunk_size_;
    scoped_ptr<GLuint[]> ids_;
    size_t next_id_index_;
};

namespace {

    GLenum TextureToStorageFormat(ResourceFormat format)
    {
        GLenum storage_format = GL_RGBA8_OES;
        switch (format) {
        case RGBA_8888:
            break;
        case BGRA_8888:
            storage_format = GL_BGRA8_EXT;
            break;
        case RGBA_4444:
        case ALPHA_8:
        case LUMINANCE_8:
        case RGB_565:
        case ETC1:
        case RED_8:
            NOTREACHED();
            break;
        }

        return storage_format;
    }

    bool IsFormatSupportedForStorage(ResourceFormat format, bool use_bgra)
    {
        switch (format) {
        case RGBA_8888:
            return true;
        case BGRA_8888:
            return use_bgra;
        case RGBA_4444:
        case ALPHA_8:
        case LUMINANCE_8:
        case RGB_565:
        case ETC1:
        case RED_8:
            return false;
        }
        return false;
    }

    GrPixelConfig ToGrPixelConfig(ResourceFormat format)
    {
        switch (format) {
        case RGBA_8888:
            return kRGBA_8888_GrPixelConfig;
        case BGRA_8888:
            return kBGRA_8888_GrPixelConfig;
        case RGBA_4444:
            return kRGBA_4444_GrPixelConfig;
        default:
            break;
        }
        DCHECK(false) << "Unsupported resource format.";
        return kSkia8888_GrPixelConfig;
    }

    class ScopedSetActiveTexture {
    public:
        ScopedSetActiveTexture(GLES2Interface* gl, GLenum unit)
            : gl_(gl)
            , unit_(unit)
        {
            DCHECK_EQ(GL_TEXTURE0, ResourceProvider::GetActiveTextureUnit(gl_));

            if (unit_ != GL_TEXTURE0)
                gl_->ActiveTexture(unit_);
        }

        ~ScopedSetActiveTexture()
        {
            // Active unit being GL_TEXTURE0 is effectively the ground state.
            if (unit_ != GL_TEXTURE0)
                gl_->ActiveTexture(GL_TEXTURE0);
        }

    private:
        GLES2Interface* gl_;
        GLenum unit_;
    };

    class TextureIdAllocator : public IdAllocator {
    public:
        TextureIdAllocator(GLES2Interface* gl,
            size_t texture_id_allocation_chunk_size)
            : IdAllocator(gl, texture_id_allocation_chunk_size)
        {
        }
        ~TextureIdAllocator() override
        {
            gl_->DeleteTextures(
                static_cast<int>(id_allocation_chunk_size_ - next_id_index_),
                ids_.get() + next_id_index_);
        }

        // Overridden from IdAllocator:
        GLuint NextId() override
        {
            if (next_id_index_ == id_allocation_chunk_size_) {
                gl_->GenTextures(static_cast<int>(id_allocation_chunk_size_), ids_.get());
                next_id_index_ = 0;
            }

            return ids_[next_id_index_++];
        }

    private:
        DISALLOW_COPY_AND_ASSIGN(TextureIdAllocator);
    };

    class BufferIdAllocator : public IdAllocator {
    public:
        BufferIdAllocator(GLES2Interface* gl, size_t buffer_id_allocation_chunk_size)
            : IdAllocator(gl, buffer_id_allocation_chunk_size)
        {
        }
        ~BufferIdAllocator() override
        {
            gl_->DeleteBuffers(
                static_cast<int>(id_allocation_chunk_size_ - next_id_index_),
                ids_.get() + next_id_index_);
        }

        // Overridden from IdAllocator:
        GLuint NextId() override
        {
            if (next_id_index_ == id_allocation_chunk_size_) {
                gl_->GenBuffers(static_cast<int>(id_allocation_chunk_size_), ids_.get());
                next_id_index_ = 0;
            }

            return ids_[next_id_index_++];
        }

    private:
        DISALLOW_COPY_AND_ASSIGN(BufferIdAllocator);
    };

    // Generates process-unique IDs to use for tracing a ResourceProvider's
    // resources.
    base::StaticAtomicSequenceNumber g_next_resource_provider_tracing_id;

} // namespace

ResourceProvider::Resource::~Resource() { }

ResourceProvider::Resource::Resource(GLuint texture_id,
    const gfx::Size& size,
    Origin origin,
    GLenum target,
    GLenum filter,
    GLenum texture_pool,
    GLint wrap_mode,
    TextureHint hint,
    ResourceFormat format)
    : child_id(0)
    , gl_id(texture_id)
    , gl_pixel_buffer_id(0)
    , gl_upload_query_id(0)
    , gl_read_lock_query_id(0)
    , pixels(NULL)
    , lock_for_read_count(0)
    , imported_count(0)
    , exported_count(0)
    , dirty_image(false)
    , locked_for_write(false)
    , lost(false)
    , marked_for_deletion(false)
    , allocated(false)
    , read_lock_fences_enabled(false)
    , has_shared_bitmap_id(false)
    , read_lock_fence(NULL)
    , size(size)
    , origin(origin)
    , target(target)
    , original_filter(filter)
    , filter(filter)
    , image_id(0)
    , bound_image_id(0)
    , texture_pool(texture_pool)
    , wrap_mode(wrap_mode)
    , hint(hint)
    , type(RESOURCE_TYPE_GL_TEXTURE)
    , format(format)
    , shared_bitmap(NULL)
    , gpu_memory_buffer(NULL)
{
    DCHECK(wrap_mode == GL_CLAMP_TO_EDGE || wrap_mode == GL_REPEAT);
    DCHECK_EQ(origin == INTERNAL, !!texture_pool);
}

ResourceProvider::Resource::Resource(uint8_t* pixels,
    SharedBitmap* bitmap,
    const gfx::Size& size,
    Origin origin,
    GLenum filter,
    GLint wrap_mode)
    : child_id(0)
    , gl_id(0)
    , gl_pixel_buffer_id(0)
    , gl_upload_query_id(0)
    , gl_read_lock_query_id(0)
    , pixels(pixels)
    , lock_for_read_count(0)
    , imported_count(0)
    , exported_count(0)
    , dirty_image(false)
    , locked_for_write(false)
    , lost(false)
    , marked_for_deletion(false)
    , allocated(false)
    , read_lock_fences_enabled(false)
    , has_shared_bitmap_id(!!bitmap)
    , read_lock_fence(NULL)
    , size(size)
    , origin(origin)
    , target(0)
    , original_filter(filter)
    , filter(filter)
    , image_id(0)
    , bound_image_id(0)
    , texture_pool(0)
    , wrap_mode(wrap_mode)
    , hint(TEXTURE_HINT_IMMUTABLE)
    , type(RESOURCE_TYPE_BITMAP)
    , format(RGBA_8888)
    , shared_bitmap(bitmap)
    , gpu_memory_buffer(NULL)
{
    DCHECK(wrap_mode == GL_CLAMP_TO_EDGE || wrap_mode == GL_REPEAT);
    DCHECK(origin == DELEGATED || pixels);
    if (bitmap)
        shared_bitmap_id = bitmap->id();
}

ResourceProvider::Resource::Resource(const SharedBitmapId& bitmap_id,
    const gfx::Size& size,
    Origin origin,
    GLenum filter,
    GLint wrap_mode)
    : child_id(0)
    , gl_id(0)
    , gl_pixel_buffer_id(0)
    , gl_upload_query_id(0)
    , gl_read_lock_query_id(0)
    , pixels(NULL)
    , lock_for_read_count(0)
    , imported_count(0)
    , exported_count(0)
    , dirty_image(false)
    , locked_for_write(false)
    , lost(false)
    , marked_for_deletion(false)
    , allocated(false)
    , read_lock_fences_enabled(false)
    , has_shared_bitmap_id(true)
    , read_lock_fence(NULL)
    , size(size)
    , origin(origin)
    , target(0)
    , original_filter(filter)
    , filter(filter)
    , image_id(0)
    , bound_image_id(0)
    , texture_pool(0)
    , wrap_mode(wrap_mode)
    , hint(TEXTURE_HINT_IMMUTABLE)
    , type(RESOURCE_TYPE_BITMAP)
    , format(RGBA_8888)
    , shared_bitmap_id(bitmap_id)
    , shared_bitmap(NULL)
    , gpu_memory_buffer(NULL)
{
    DCHECK(wrap_mode == GL_CLAMP_TO_EDGE || wrap_mode == GL_REPEAT);
}

ResourceProvider::Child::Child()
    : marked_for_deletion(false)
    , needs_sync_points(true)
{
}

ResourceProvider::Child::~Child() { }

scoped_ptr<ResourceProvider> ResourceProvider::Create(
    OutputSurface* output_surface,
    SharedBitmapManager* shared_bitmap_manager,
    gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager,
    BlockingTaskRunner* blocking_main_thread_task_runner,
    int highp_threshold_min,
    size_t id_allocation_chunk_size,
    const std::vector<unsigned>& use_image_texture_targets)
{
    scoped_ptr<ResourceProvider> resource_provider(new ResourceProvider(
        output_surface, shared_bitmap_manager, gpu_memory_buffer_manager,
        blocking_main_thread_task_runner, highp_threshold_min,
        id_allocation_chunk_size, use_image_texture_targets));
    resource_provider->Initialize();
    return resource_provider;
}

ResourceProvider::~ResourceProvider()
{
    //   base::trace_event::MemoryDumpManager::GetInstance()->UnregisterDumpProvider(
    //       this);

    while (!children_.empty())
        DestroyChildInternal(children_.begin(), FOR_SHUTDOWN);
    while (!resources_.empty())
        DeleteResourceInternal(resources_.begin(), FOR_SHUTDOWN);

    GLES2Interface* gl = ContextGL();
    if (default_resource_type_ != RESOURCE_TYPE_GL_TEXTURE) {
        // We are not in GL mode, but double check before returning.
        DCHECK(!gl);
        return;
    }

    DCHECK(gl);
#if DCHECK_IS_ON()
    // Check that all GL resources has been deleted.
    for (ResourceMap::const_iterator itr = resources_.begin();
         itr != resources_.end(); ++itr) {
        DCHECK_NE(RESOURCE_TYPE_GL_TEXTURE, itr->second.type);
    }
#endif // DCHECK_IS_ON()

    texture_id_allocator_ = nullptr;
    buffer_id_allocator_ = nullptr;
    gl->Finish();
}

bool ResourceProvider::InUseByConsumer(ResourceId id)
{
    Resource* resource = GetResource(id);
    return resource->lock_for_read_count > 0 || resource->exported_count > 0 || resource->lost;
}

bool ResourceProvider::IsLost(ResourceId id)
{
    Resource* resource = GetResource(id);
    return resource->lost;
}

void ResourceProvider::LoseResourceForTesting(ResourceId id)
{
    Resource* resource = GetResource(id);
    DCHECK(resource);
    resource->lost = true;
}

ResourceId ResourceProvider::CreateResource(const gfx::Size& size,
    GLint wrap_mode,
    TextureHint hint,
    ResourceFormat format)
{
    DCHECK(!size.IsEmpty());
    switch (default_resource_type_) {
    case RESOURCE_TYPE_GL_TEXTURE:
        return CreateGLTexture(size,
            GL_TEXTURE_2D,
            GL_TEXTURE_POOL_UNMANAGED_CHROMIUM,
            wrap_mode,
            hint,
            format);
    case RESOURCE_TYPE_BITMAP:
        DCHECK_EQ(RGBA_8888, format);
        return CreateBitmap(size, wrap_mode);
    }

    LOG(FATAL) << "Invalid default resource type.";
    return 0;
}

ResourceId ResourceProvider::CreateManagedResource(const gfx::Size& size,
    GLenum target,
    GLint wrap_mode,
    TextureHint hint,
    ResourceFormat format)
{
    DCHECK(!size.IsEmpty());
    switch (default_resource_type_) {
    case RESOURCE_TYPE_GL_TEXTURE:
        return CreateGLTexture(size,
            target,
            GL_TEXTURE_POOL_MANAGED_CHROMIUM,
            wrap_mode,
            hint,
            format);
    case RESOURCE_TYPE_BITMAP:
        DCHECK_EQ(RGBA_8888, format);
        return CreateBitmap(size, wrap_mode);
    }

    LOG(FATAL) << "Invalid default resource type.";
    return 0;
}

ResourceId ResourceProvider::CreateGLTexture(const gfx::Size& size,
    GLenum target,
    GLenum texture_pool,
    GLint wrap_mode,
    TextureHint hint,
    ResourceFormat format)
{
    DCHECK_LE(size.width(), max_texture_size_);
    DCHECK_LE(size.height(), max_texture_size_);
    DCHECK(thread_checker_.CalledOnValidThread());

    ResourceId id = next_id_++;
    Resource* resource = InsertResource(
        id, Resource(0, size, Resource::INTERNAL, target, GL_LINEAR, texture_pool, wrap_mode, hint, format));
    resource->allocated = false;
    return id;
}

ResourceId ResourceProvider::CreateBitmap(const gfx::Size& size,
    GLint wrap_mode)
{
    DCHECK(thread_checker_.CalledOnValidThread());

    scoped_ptr<SharedBitmap> bitmap = shared_bitmap_manager_->AllocateSharedBitmap(size);
    uint8_t* pixels = bitmap->pixels();
    DCHECK(pixels);

    ResourceId id = next_id_++;
    Resource* resource = InsertResource(id, Resource(pixels, bitmap.release(), size, Resource::INTERNAL, GL_LINEAR, wrap_mode));
    resource->allocated = true;
    return id;
}

ResourceId ResourceProvider::CreateResourceFromIOSurface(
    const gfx::Size& size,
    unsigned io_surface_id)
{
    DCHECK(thread_checker_.CalledOnValidThread());

    ResourceId id = next_id_++;
    Resource* resource = InsertResource(
        id, Resource(0, gfx::Size(), Resource::INTERNAL, GL_TEXTURE_RECTANGLE_ARB, GL_LINEAR, GL_TEXTURE_POOL_UNMANAGED_CHROMIUM, GL_CLAMP_TO_EDGE, TEXTURE_HINT_IMMUTABLE, RGBA_8888));
    LazyCreate(resource);
    GLES2Interface* gl = ContextGL();
    DCHECK(gl);
    gl->BindTexture(GL_TEXTURE_RECTANGLE_ARB, resource->gl_id);
    gl->TexImageIOSurface2DCHROMIUM(
        GL_TEXTURE_RECTANGLE_ARB, size.width(), size.height(), io_surface_id, 0);
    resource->allocated = true;
    return id;
}

ResourceId ResourceProvider::CreateResourceFromTextureMailbox(
    const TextureMailbox& mailbox,
    scoped_ptr<SingleReleaseCallbackImpl> release_callback_impl,
    bool read_lock_fences_enabled)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    // Just store the information. Mailbox will be consumed in LockForRead().
    ResourceId id = next_id_++;
    DCHECK(mailbox.IsValid());
    Resource* resource = nullptr;
    if (mailbox.IsTexture()) {
        resource = InsertResource(
            id, Resource(0, gfx::Size(), Resource::EXTERNAL, mailbox.target(), mailbox.nearest_neighbor() ? GL_NEAREST : GL_LINEAR, 0, GL_CLAMP_TO_EDGE, TEXTURE_HINT_IMMUTABLE, RGBA_8888));
    } else {
        DCHECK(mailbox.IsSharedMemory());
        SharedBitmap* shared_bitmap = mailbox.shared_bitmap();
        uint8_t* pixels = shared_bitmap->pixels();
        DCHECK(pixels);
        resource = InsertResource(
            id, Resource(pixels, shared_bitmap, mailbox.size_in_pixels(), Resource::EXTERNAL, GL_LINEAR, GL_CLAMP_TO_EDGE));
    }
    resource->allocated = true;
    resource->mailbox = mailbox;
    resource->release_callback_impl = base::Bind(&SingleReleaseCallbackImpl::Run,
        base::Owned(release_callback_impl.release()));
    resource->read_lock_fences_enabled = read_lock_fences_enabled;
    return id;
}

ResourceId ResourceProvider::CreateResourceFromTextureMailbox(
    const TextureMailbox& mailbox,
    scoped_ptr<SingleReleaseCallbackImpl> release_callback_impl)
{
    return CreateResourceFromTextureMailbox(mailbox, release_callback_impl.Pass(),
        false);
}

void ResourceProvider::DeleteResource(ResourceId id)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    ResourceMap::iterator it = resources_.find(id);
    CHECK(it != resources_.end());
    Resource* resource = &it->second;
    DCHECK(!resource->marked_for_deletion);
    DCHECK_EQ(resource->imported_count, 0);
    DCHECK(!resource->locked_for_write);

    if (resource->exported_count > 0 || resource->lock_for_read_count > 0 || !ReadLockFenceHasPassed(resource)) {
        resource->marked_for_deletion = true;
        return;
    } else {
        DeleteResourceInternal(it, NORMAL);
    }
}

void ResourceProvider::DeleteResourceInternal(ResourceMap::iterator it,
    DeleteStyle style)
{
    TRACE_EVENT0("cc", "ResourceProvider::DeleteResourceInternal");
    Resource* resource = &it->second;
    bool lost_resource = resource->lost;

    DCHECK(resource->exported_count == 0 || style != NORMAL);
    if (style == FOR_SHUTDOWN && resource->exported_count > 0)
        lost_resource = true;

    if (resource->image_id) {
        DCHECK(resource->origin == Resource::INTERNAL);
        GLES2Interface* gl = ContextGL();
        DCHECK(gl);
        gl->DestroyImageCHROMIUM(resource->image_id);
    }
    if (resource->gl_upload_query_id) {
        DCHECK(resource->origin == Resource::INTERNAL);
        GLES2Interface* gl = ContextGL();
        DCHECK(gl);
        gl->DeleteQueriesEXT(1, &resource->gl_upload_query_id);
    }
    if (resource->gl_read_lock_query_id) {
        DCHECK(resource->origin == Resource::INTERNAL);
        GLES2Interface* gl = ContextGL();
        DCHECK(gl);
        gl->DeleteQueriesEXT(1, &resource->gl_read_lock_query_id);
    }
    if (resource->gl_pixel_buffer_id) {
        DCHECK(resource->origin == Resource::INTERNAL);
        GLES2Interface* gl = ContextGL();
        DCHECK(gl);
        gl->DeleteBuffers(1, &resource->gl_pixel_buffer_id);
    }
    if (resource->origin == Resource::EXTERNAL) {
        DCHECK(resource->mailbox.IsValid());
        gpu::SyncToken sync_token = resource->mailbox.sync_token();
        if (resource->type == RESOURCE_TYPE_GL_TEXTURE) {
            DCHECK(resource->mailbox.IsTexture());
            lost_resource |= lost_output_surface_;
            GLES2Interface* gl = ContextGL();
            DCHECK(gl);
            if (resource->gl_id) {
                gl->DeleteTextures(1, &resource->gl_id);
                resource->gl_id = 0;
                if (!lost_resource)
                    sync_token = gpu::SyncToken(gl->InsertSyncPointCHROMIUM());
            }
        } else {
            DCHECK(resource->mailbox.IsSharedMemory());
            resource->shared_bitmap = nullptr;
            resource->pixels = nullptr;
        }
        resource->release_callback_impl.Run(
            sync_token, lost_resource, blocking_main_thread_task_runner_);
    }
    if (resource->gl_id) {
        GLES2Interface* gl = ContextGL();
        DCHECK(gl);
        gl->DeleteTextures(1, &resource->gl_id);
        resource->gl_id = 0;
    }
    if (resource->shared_bitmap) {
        DCHECK(resource->origin != Resource::EXTERNAL);
        DCHECK_EQ(RESOURCE_TYPE_BITMAP, resource->type);
        delete resource->shared_bitmap;
        resource->pixels = NULL;
    }
    if (resource->pixels) {
        DCHECK(resource->origin == Resource::INTERNAL);
        delete[] resource->pixels;
        resource->pixels = NULL;
    }
    if (resource->gpu_memory_buffer) {
        DCHECK(resource->origin == Resource::INTERNAL);
        delete resource->gpu_memory_buffer;
        resource->gpu_memory_buffer = NULL;
    }
    resources_.erase(it);
}

ResourceProvider::ResourceType ResourceProvider::GetResourceType(
    ResourceId id)
{
    return GetResource(id)->type;
}

void ResourceProvider::CopyToResource(ResourceId id,
    const uint8_t* image,
    const gfx::Size& image_size)
{
    Resource* resource = GetResource(id);
    DCHECK(!resource->locked_for_write);
    DCHECK(!resource->lock_for_read_count);
    DCHECK(resource->origin == Resource::INTERNAL);
    DCHECK_EQ(resource->exported_count, 0);
    DCHECK(ReadLockFenceHasPassed(resource));
    LazyAllocate(resource);

    DCHECK_EQ(image_size.width(), resource->size.width());
    DCHECK_EQ(image_size.height(), resource->size.height());

    if (resource->type == RESOURCE_TYPE_BITMAP) {
        DCHECK_EQ(RESOURCE_TYPE_BITMAP, resource->type);
        DCHECK(resource->allocated);
        DCHECK_EQ(RGBA_8888, resource->format);
        SkImageInfo source_info = SkImageInfo::MakeN32Premul(image_size.width(), image_size.height());
        size_t image_stride = image_size.width() * 4;

        ScopedWriteLockSoftware lock(this, id);
        SkCanvas dest(lock.sk_bitmap());
        dest.writePixels(source_info, image, image_stride, 0, 0);
    } else {
        DCHECK(resource->gl_id);
        DCHECK_EQ(resource->target, static_cast<GLenum>(GL_TEXTURE_2D));
        GLES2Interface* gl = ContextGL();
        DCHECK(gl);
        gl->BindTexture(GL_TEXTURE_2D, resource->gl_id);

        if (resource->format == ETC1) {
            int image_bytes = ResourceUtil::CheckedSizeInBytes<int>(image_size, ETC1);
            gl->CompressedTexImage2D(GL_TEXTURE_2D, 0, GLInternalFormat(ETC1),
                image_size.width(), image_size.height(), 0,
                image_bytes, image);
        } else {
            gl->TexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_size.width(),
                image_size.height(), GLDataFormat(resource->format),
                GLDataType(resource->format), image);
        }
    }
}

ResourceProvider::Resource* ResourceProvider::InsertResource(
    ResourceId id,
    const Resource& resource)
{
    std::pair<ResourceMap::iterator, bool> result = resources_.insert(ResourceMap::value_type(id, resource));
    DCHECK(result.second);
    return &result.first->second;
}

ResourceProvider::Resource* ResourceProvider::GetResource(ResourceId id)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(id);
    ResourceMap::iterator it = resources_.find(id);
    DCHECK(it != resources_.end());
    return &it->second;
}

const ResourceProvider::Resource* ResourceProvider::LockForRead(ResourceId id)
{
    Resource* resource = GetResource(id);
    DCHECK(!resource->locked_for_write) << "locked for write: "
                                        << resource->locked_for_write;
    DCHECK_EQ(resource->exported_count, 0);
    // Uninitialized! Call SetPixels or LockForWrite first.
    DCHECK(resource->allocated);

    LazyCreate(resource);

    if (resource->type == RESOURCE_TYPE_GL_TEXTURE && !resource->gl_id) {
        DCHECK(resource->origin != Resource::INTERNAL);
        DCHECK(resource->mailbox.IsTexture());

        // Mailbox sync_points must be processed by a call to
        // WaitSyncPointIfNeeded() prior to calling LockForRead().
        DCHECK(!resource->mailbox.sync_token().HasData());

        GLES2Interface* gl = ContextGL();
        DCHECK(gl);
        resource->gl_id = gl->CreateAndConsumeTextureCHROMIUM(
            resource->mailbox.target(), resource->mailbox.name());
    }

    if (!resource->pixels && resource->has_shared_bitmap_id && shared_bitmap_manager_) {
        scoped_ptr<SharedBitmap> bitmap = shared_bitmap_manager_->GetSharedBitmapFromId(
            resource->size, resource->shared_bitmap_id);
        if (bitmap) {
            resource->shared_bitmap = bitmap.release();
            resource->pixels = resource->shared_bitmap->pixels();
        }
    }

    resource->lock_for_read_count++;
    if (resource->read_lock_fences_enabled) {
        if (current_read_lock_fence_.get())
            current_read_lock_fence_->Set();
        resource->read_lock_fence = current_read_lock_fence_;
    }

    return resource;
}

void ResourceProvider::UnlockForRead(ResourceId id)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    ResourceMap::iterator it = resources_.find(id);
    CHECK(it != resources_.end());

    Resource* resource = &it->second;
    DCHECK_GT(resource->lock_for_read_count, 0);
    DCHECK_EQ(resource->exported_count, 0);
    resource->lock_for_read_count--;
    if (resource->marked_for_deletion && !resource->lock_for_read_count) {
        if (!resource->child_id) {
            // The resource belongs to this ResourceProvider, so it can be destroyed.
            DeleteResourceInternal(it, NORMAL);
        } else {
            ChildMap::iterator child_it = children_.find(resource->child_id);
            ResourceIdArray unused;
            unused.push_back(id);
            DeleteAndReturnUnusedResourcesToChild(child_it, NORMAL, unused);
        }
    }
}

ResourceProvider::Resource* ResourceProvider::LockForWrite(ResourceId id)
{
    Resource* resource = GetResource(id);
    DCHECK(CanLockForWrite(id));

    resource->locked_for_write = true;
    return resource;
}

bool ResourceProvider::CanLockForWrite(ResourceId id)
{
    Resource* resource = GetResource(id);
    return !resource->locked_for_write && !resource->lock_for_read_count && !resource->exported_count && resource->origin == Resource::INTERNAL && !resource->lost && ReadLockFenceHasPassed(resource);
}

void ResourceProvider::UnlockForWrite(ResourceProvider::Resource* resource)
{
    DCHECK(resource->locked_for_write);
    DCHECK_EQ(resource->exported_count, 0);
    DCHECK(resource->origin == Resource::INTERNAL);
    resource->locked_for_write = false;
}

void ResourceProvider::EnableReadLockFencesForTesting(ResourceId id)
{
    Resource* resource = GetResource(id);
    DCHECK(resource);
    resource->read_lock_fences_enabled = true;
}

ResourceProvider::ScopedReadLockGL::ScopedReadLockGL(
    ResourceProvider* resource_provider,
    ResourceId resource_id)
    : resource_provider_(resource_provider)
    , resource_id_(resource_id)
    , resource_(resource_provider->LockForRead(resource_id))
{
    DCHECK(resource_);
}

ResourceProvider::ScopedReadLockGL::~ScopedReadLockGL()
{
    resource_provider_->UnlockForRead(resource_id_);
}

ResourceProvider::ScopedSamplerGL::ScopedSamplerGL(
    ResourceProvider* resource_provider,
    ResourceId resource_id,
    GLenum filter)
    : ScopedReadLockGL(resource_provider, resource_id)
    , unit_(GL_TEXTURE0)
    , target_(resource_provider->BindForSampling(resource_id, unit_, filter))
{
}

ResourceProvider::ScopedSamplerGL::ScopedSamplerGL(
    ResourceProvider* resource_provider,
    ResourceId resource_id,
    GLenum unit,
    GLenum filter)
    : ScopedReadLockGL(resource_provider, resource_id)
    , unit_(unit)
    , target_(resource_provider->BindForSampling(resource_id, unit_, filter))
{
}

ResourceProvider::ScopedSamplerGL::~ScopedSamplerGL()
{
}

ResourceProvider::ScopedWriteLockGL::ScopedWriteLockGL(
    ResourceProvider* resource_provider,
    ResourceId resource_id)
    : resource_provider_(resource_provider)
    , resource_(resource_provider->LockForWrite(resource_id))
{
    resource_provider_->LazyAllocate(resource_);
    texture_id_ = resource_->gl_id;
    DCHECK(texture_id_);
}

ResourceProvider::ScopedWriteLockGL::~ScopedWriteLockGL()
{
    resource_provider_->UnlockForWrite(resource_);
}

void ResourceProvider::PopulateSkBitmapWithResource(
    SkBitmap* sk_bitmap, const Resource* resource)
{
    DCHECK_EQ(RGBA_8888, resource->format);
    SkImageInfo info = SkImageInfo::MakeN32Premul(resource->size.width(),
        resource->size.height());
    sk_bitmap->installPixels(info, resource->pixels, info.minRowBytes());
}

ResourceProvider::ScopedReadLockSoftware::ScopedReadLockSoftware(
    ResourceProvider* resource_provider,
    ResourceId resource_id)
    : resource_provider_(resource_provider)
    , resource_id_(resource_id)
{
    const Resource* resource = resource_provider->LockForRead(resource_id);
    wrap_mode_ = resource->wrap_mode;
    ResourceProvider::PopulateSkBitmapWithResource(&sk_bitmap_, resource);
}

ResourceProvider::ScopedReadLockSoftware::~ScopedReadLockSoftware()
{
    resource_provider_->UnlockForRead(resource_id_);
}

ResourceProvider::ScopedWriteLockSoftware::ScopedWriteLockSoftware(
    ResourceProvider* resource_provider,
    ResourceId resource_id)
    : resource_provider_(resource_provider)
    , resource_(resource_provider->LockForWrite(resource_id))
{
    ResourceProvider::PopulateSkBitmapWithResource(&sk_bitmap_, resource_);
    DCHECK(valid());
}

ResourceProvider::ScopedWriteLockSoftware::~ScopedWriteLockSoftware()
{
    DCHECK(thread_checker_.CalledOnValidThread());
    resource_provider_->UnlockForWrite(resource_);
}

ResourceProvider::ScopedWriteLockGpuMemoryBuffer::
    ScopedWriteLockGpuMemoryBuffer(ResourceProvider* resource_provider,
        ResourceId resource_id)
    : resource_provider_(resource_provider)
    , resource_(resource_provider->LockForWrite(resource_id))
    , gpu_memory_buffer_manager_(resource_provider->gpu_memory_buffer_manager_)
    , gpu_memory_buffer_(nullptr)
    , size_(resource_->size)
    , format_(resource_->format)
{
    DCHECK_EQ(RESOURCE_TYPE_GL_TEXTURE, resource_->type);
    std::swap(gpu_memory_buffer_, resource_->gpu_memory_buffer);
}

ResourceProvider::ScopedWriteLockGpuMemoryBuffer::
    ~ScopedWriteLockGpuMemoryBuffer()
{
    DCHECK(thread_checker_.CalledOnValidThread());
    resource_provider_->UnlockForWrite(resource_);
    if (!gpu_memory_buffer_)
        return;

    resource_provider_->LazyCreate(resource_);

    if (!resource_->image_id) {
        GLES2Interface* gl = resource_provider_->ContextGL();
        DCHECK(gl);

#if defined(OS_CHROMEOS)
        // TODO(reveman): GL_COMMANDS_ISSUED_CHROMIUM is used for synchronization
        // on ChromeOS to avoid some performance issues. This only works with
        // shared memory backed buffers. crbug.com/436314
        DCHECK_EQ(gpu_memory_buffer_->GetHandle().type, gfx::SHARED_MEMORY_BUFFER);
#endif

        resource_->image_id = gl->CreateImageCHROMIUM(
            gpu_memory_buffer_->AsClientBuffer(), size_.width(), size_.height(),
            GLInternalFormat(resource_->format));
    }

    std::swap(resource_->gpu_memory_buffer, gpu_memory_buffer_);
    resource_->allocated = true;
    resource_->dirty_image = true;

    // GpuMemoryBuffer provides direct access to the memory used by the GPU.
    // Read lock fences are required to ensure that we're not trying to map a
    // buffer that is currently in-use by the GPU.
    resource_->read_lock_fences_enabled = true;
}

gfx::GpuMemoryBuffer*
ResourceProvider::ScopedWriteLockGpuMemoryBuffer::GetGpuMemoryBuffer()
{
    if (gpu_memory_buffer_)
        return gpu_memory_buffer_;
    scoped_ptr<gfx::GpuMemoryBuffer> gpu_memory_buffer = gpu_memory_buffer_manager_->AllocateGpuMemoryBuffer(
        size_, BufferFormat(format_), gfx::BufferUsage::GPU_READ_CPU_READ_WRITE);
    gpu_memory_buffer_ = gpu_memory_buffer.release();
    return gpu_memory_buffer_;
}

ResourceProvider::ScopedWriteLockGr::ScopedWriteLockGr(
    ResourceProvider* resource_provider,
    ResourceId resource_id)
    : resource_provider_(resource_provider)
    , resource_(resource_provider->LockForWrite(resource_id))
{
    DCHECK(thread_checker_.CalledOnValidThread());
    resource_provider_->LazyAllocate(resource_);
}

ResourceProvider::ScopedWriteLockGr::~ScopedWriteLockGr()
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(resource_->locked_for_write);
    resource_provider_->UnlockForWrite(resource_);
}

void ResourceProvider::ScopedWriteLockGr::InitSkSurface(
    bool use_distance_field_text,
    bool can_use_lcd_text,
    int msaa_sample_count)
{
    DCHECK(resource_->locked_for_write);

    GrBackendTextureDesc desc;
    desc.fFlags = kRenderTarget_GrBackendTextureFlag;
    desc.fWidth = resource_->size.width();
    desc.fHeight = resource_->size.height();
    desc.fConfig = ToGrPixelConfig(resource_->format);
    desc.fOrigin = kTopLeft_GrSurfaceOrigin;
    desc.fTextureHandle = resource_->gl_id;
    desc.fSampleCnt = msaa_sample_count;

    bool use_worker_context = true;
    class GrContext* gr_context = resource_provider_->GrContext(use_worker_context);
    uint32_t flags = use_distance_field_text ? SkSurfaceProps::kUseDistanceFieldFonts_Flag : 0;
    // Use unknown pixel geometry to disable LCD text.
    SkSurfaceProps surface_props(flags, kUnknown_SkPixelGeometry);
    if (can_use_lcd_text) {
        // LegacyFontHost will get LCD text and skia figures out what type to use.
        surface_props = SkSurfaceProps(flags, SkSurfaceProps::kLegacyFontHost_InitType);
    }
    sk_surface_ = skia::AdoptRef(
        SkSurface::NewWrappedRenderTarget(gr_context, desc, &surface_props));
}

void ResourceProvider::ScopedWriteLockGr::ReleaseSkSurface()
{
    sk_surface_.clear();
}

ResourceProvider::SynchronousFence::SynchronousFence(
    gpu::gles2::GLES2Interface* gl)
    : gl_(gl)
    , has_synchronized_(true)
{
}

ResourceProvider::SynchronousFence::~SynchronousFence()
{
}

void ResourceProvider::SynchronousFence::Set()
{
    has_synchronized_ = false;
}

bool ResourceProvider::SynchronousFence::HasPassed()
{
    if (!has_synchronized_) {
        has_synchronized_ = true;
        Synchronize();
    }
    return true;
}

void ResourceProvider::SynchronousFence::Wait()
{
    HasPassed();
}

void ResourceProvider::SynchronousFence::Synchronize()
{
    TRACE_EVENT0("cc", "ResourceProvider::SynchronousFence::Synchronize");
    gl_->Finish();
}

ResourceProvider::ResourceProvider(
    OutputSurface* output_surface,
    SharedBitmapManager* shared_bitmap_manager,
    gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager,
    BlockingTaskRunner* blocking_main_thread_task_runner,
    int highp_threshold_min,
    size_t id_allocation_chunk_size,
    const std::vector<unsigned>& use_image_texture_targets)
    : output_surface_(output_surface)
    , shared_bitmap_manager_(shared_bitmap_manager)
    , gpu_memory_buffer_manager_(gpu_memory_buffer_manager)
    , blocking_main_thread_task_runner_(blocking_main_thread_task_runner)
    , lost_output_surface_(false)
    , highp_threshold_min_(highp_threshold_min)
    , next_id_(1)
    , next_child_(1)
    , default_resource_type_(RESOURCE_TYPE_BITMAP)
    , use_texture_storage_ext_(false)
    , use_texture_format_bgra_(false)
    , use_texture_usage_hint_(false)
    , use_compressed_texture_etc1_(false)
    , yuv_resource_format_(LUMINANCE_8)
    , max_texture_size_(0)
    , best_texture_format_(RGBA_8888)
    , best_render_buffer_format_(RGBA_8888)
    , id_allocation_chunk_size_(id_allocation_chunk_size)
    , use_sync_query_(false)
    , use_image_texture_targets_(use_image_texture_targets)
    , tracing_id_(g_next_resource_provider_tracing_id.GetNext())
{
    DCHECK(output_surface_->HasClient());
    DCHECK(id_allocation_chunk_size_);
}

void ResourceProvider::Initialize()
{
    DCHECK(thread_checker_.CalledOnValidThread());

    // In certain cases, ThreadTaskRunnerHandle isn't set (Android Webview).
    // Don't register a dump provider in these cases.
    // TODO(ericrk): Get this working in Android Webview. crbug.com/517156
    //   if (base::ThreadTaskRunnerHandle::IsSet()) {
    //     base::trace_event::MemoryDumpManager::GetInstance()->RegisterDumpProvider(
    //         this, "ResourceProvider::Initialize", base::ThreadTaskRunnerHandle::Get());
    //   }

    GLES2Interface* gl = ContextGL();
    if (!gl) {
        default_resource_type_ = RESOURCE_TYPE_BITMAP;
        // Pick an arbitrary limit here similar to what hardware might.
        max_texture_size_ = 16 * 1024;
        best_texture_format_ = RGBA_8888;
        return;
    }

    DCHECK(!texture_id_allocator_);
    DCHECK(!buffer_id_allocator_);

    const ContextProvider::Capabilities& caps = output_surface_->context_provider()->ContextCapabilities();

    default_resource_type_ = RESOURCE_TYPE_GL_TEXTURE;
    use_texture_storage_ext_ = caps.gpu.texture_storage;
    use_texture_format_bgra_ = caps.gpu.texture_format_bgra8888;
    use_texture_usage_hint_ = caps.gpu.texture_usage;
    use_compressed_texture_etc1_ = caps.gpu.texture_format_etc1;
    yuv_resource_format_ = caps.gpu.texture_rg ? RED_8 : LUMINANCE_8;
    use_sync_query_ = caps.gpu.sync_query;

    max_texture_size_ = 0; // Context expects cleared value.
    gl->GetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size_);
    best_texture_format_ = PlatformColor::BestTextureFormat(use_texture_format_bgra_);

    best_render_buffer_format_ = PlatformColor::BestTextureFormat(caps.gpu.render_buffer_format_bgra8888);

    texture_id_allocator_.reset(
        new TextureIdAllocator(gl, id_allocation_chunk_size_));
    buffer_id_allocator_.reset(
        new BufferIdAllocator(gl, id_allocation_chunk_size_));
}

int ResourceProvider::CreateChild(const ReturnCallback& return_callback)
{
    DCHECK(thread_checker_.CalledOnValidThread());

    Child child_info;
    child_info.return_callback = return_callback;

    int child = next_child_++;
    children_[child] = child_info;
    return child;
}

void ResourceProvider::SetChildNeedsSyncPoints(int child_id, bool needs)
{
    ChildMap::iterator it = children_.find(child_id);
    DCHECK(it != children_.end());
    it->second.needs_sync_points = needs;
}

void ResourceProvider::DestroyChild(int child_id)
{
    ChildMap::iterator it = children_.find(child_id);
    DCHECK(it != children_.end());
    DestroyChildInternal(it, NORMAL);
}

void ResourceProvider::DestroyChildInternal(ChildMap::iterator it,
    DeleteStyle style)
{
    DCHECK(thread_checker_.CalledOnValidThread());

    Child& child = it->second;
    DCHECK(style == FOR_SHUTDOWN || !child.marked_for_deletion);

    ResourceIdArray resources_for_child;

    for (ResourceIdMap::iterator child_it = child.child_to_parent_map.begin();
         child_it != child.child_to_parent_map.end();
         ++child_it) {
        ResourceId id = child_it->second;
        resources_for_child.push_back(id);
    }

    child.marked_for_deletion = true;

    DeleteAndReturnUnusedResourcesToChild(it, style, resources_for_child);
}

const ResourceProvider::ResourceIdMap& ResourceProvider::GetChildToParentMap(
    int child) const
{
    DCHECK(thread_checker_.CalledOnValidThread());
    ChildMap::const_iterator it = children_.find(child);
    DCHECK(it != children_.end());
    DCHECK(!it->second.marked_for_deletion);
    return it->second.child_to_parent_map;
}

void ResourceProvider::PrepareSendToParent(const ResourceIdArray& resources,
    TransferableResourceArray* list)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    GLES2Interface* gl = ContextGL();
    bool need_sync_point = false;
    for (ResourceIdArray::const_iterator it = resources.begin();
         it != resources.end();
         ++it) {
        TransferableResource resource;
        TransferResource(gl, *it, &resource);
        if (!resource.mailbox_holder.sync_token.HasData() && !resource.is_software)
            need_sync_point = true;
        ++resources_.find(*it)->second.exported_count;
        list->push_back(resource);
    }
    if (need_sync_point && output_surface_->capabilities().delegated_sync_points_required) {
        gpu::SyncToken sync_token(gl->InsertSyncPointCHROMIUM());
        for (TransferableResourceArray::iterator it = list->begin();
             it != list->end();
             ++it) {
            if (!it->mailbox_holder.sync_token.HasData())
                it->mailbox_holder.sync_token = sync_token;
        }
    }
}

void ResourceProvider::ReceiveFromChild(
    int child, const TransferableResourceArray& resources)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    GLES2Interface* gl = ContextGL();
    Child& child_info = children_.find(child)->second;
    DCHECK(!child_info.marked_for_deletion);
    for (TransferableResourceArray::const_iterator it = resources.begin();
         it != resources.end();
         ++it) {
        ResourceIdMap::iterator resource_in_map_it = child_info.child_to_parent_map.find(it->id);
        if (resource_in_map_it != child_info.child_to_parent_map.end()) {
            Resource* resource = GetResource(resource_in_map_it->second);
            resource->marked_for_deletion = false;
            resource->imported_count++;
            continue;
        }

        if ((!it->is_software && !gl) || (it->is_software && !shared_bitmap_manager_)) {
            TRACE_EVENT0("cc", "ResourceProvider::ReceiveFromChild dropping invalid");
            ReturnedResourceArray to_return;
            to_return.push_back(it->ToReturnedResource());
            child_info.return_callback.Run(to_return,
                blocking_main_thread_task_runner_);
            continue;
        }

        ResourceId local_id = next_id_++;
        Resource* resource = nullptr;
        if (it->is_software) {
            resource = InsertResource(
                local_id,
                Resource(it->mailbox_holder.mailbox, it->size, Resource::DELEGATED,
                    GL_LINEAR, it->is_repeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
        } else {
            resource = InsertResource(
                local_id, Resource(0, it->size, Resource::DELEGATED, it->mailbox_holder.texture_target, it->filter, 0, it->is_repeated ? GL_REPEAT : GL_CLAMP_TO_EDGE, TEXTURE_HINT_IMMUTABLE, it->format));
            resource->mailbox = TextureMailbox(it->mailbox_holder.mailbox,
                it->mailbox_holder.texture_target,
                it->mailbox_holder.sync_token);
            resource->read_lock_fences_enabled = it->read_lock_fences_enabled;
        }
        resource->child_id = child;
        // Don't allocate a texture for a child.
        resource->allocated = true;
        resource->imported_count = 1;
        child_info.parent_to_child_map[local_id] = it->id;
        child_info.child_to_parent_map[it->id] = local_id;
    }
}

void ResourceProvider::DeclareUsedResourcesFromChild(
    int child,
    const ResourceIdSet& resources_from_child)
{
    DCHECK(thread_checker_.CalledOnValidThread());

    ChildMap::iterator child_it = children_.find(child);
    DCHECK(child_it != children_.end());
    Child& child_info = child_it->second;
    DCHECK(!child_info.marked_for_deletion);

    ResourceIdArray unused;
    for (ResourceIdMap::iterator it = child_info.child_to_parent_map.begin();
         it != child_info.child_to_parent_map.end();
         ++it) {
        ResourceId local_id = it->second;
        bool resource_is_in_use = resources_from_child.count(it->first) > 0;
        if (!resource_is_in_use)
            unused.push_back(local_id);
    }
    DeleteAndReturnUnusedResourcesToChild(child_it, NORMAL, unused);
}

void ResourceProvider::ReceiveReturnsFromParent(
    const ReturnedResourceArray& resources)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    GLES2Interface* gl = ContextGL();

    base::hash_map<int, ResourceIdArray> resources_for_child;

    for (const ReturnedResource& returned : resources) {
        ResourceId local_id = returned.id;
        ResourceMap::iterator map_iterator = resources_.find(local_id);
        // Resource was already lost (e.g. it belonged to a child that was
        // destroyed).
        if (map_iterator == resources_.end())
            continue;

        Resource* resource = &map_iterator->second;

        CHECK_GE(resource->exported_count, returned.count);
        resource->exported_count -= returned.count;
        resource->lost |= returned.lost;
        if (resource->exported_count)
            continue;

        if (returned.sync_token.HasData()) {
            DCHECK(!resource->has_shared_bitmap_id);
            if (resource->origin == Resource::INTERNAL) {
                DCHECK(resource->gl_id);
                gl->WaitSyncTokenCHROMIUM(returned.sync_token.GetConstData());
            } else {
                DCHECK(!resource->gl_id);
                resource->mailbox.set_sync_token(returned.sync_token);
            }
        }

        if (!resource->marked_for_deletion)
            continue;

        if (!resource->child_id) {
            // The resource belongs to this ResourceProvider, so it can be destroyed.
            DeleteResourceInternal(map_iterator, NORMAL);
            continue;
        }

        DCHECK(resource->origin == Resource::DELEGATED);
        resources_for_child[resource->child_id].push_back(local_id);
    }

    for (const auto& children : resources_for_child) {
        ChildMap::iterator child_it = children_.find(children.first);
        DCHECK(child_it != children_.end());
        DeleteAndReturnUnusedResourcesToChild(child_it, NORMAL, children.second);
    }
}

void ResourceProvider::TransferResource(GLES2Interface* gl,
    ResourceId id,
    TransferableResource* resource)
{
    Resource* source = GetResource(id);
    DCHECK(!source->locked_for_write);
    DCHECK(!source->lock_for_read_count);
    DCHECK(source->origin != Resource::EXTERNAL || source->mailbox.IsValid());
    DCHECK(source->allocated);
    resource->id = id;
    resource->format = source->format;
    resource->mailbox_holder.texture_target = source->target;
    resource->filter = source->filter;
    resource->size = source->size;
    resource->read_lock_fences_enabled = source->read_lock_fences_enabled;
    resource->is_repeated = (source->wrap_mode == GL_REPEAT);

    if (source->type == RESOURCE_TYPE_BITMAP) {
        resource->mailbox_holder.mailbox = source->shared_bitmap_id;
        resource->is_software = true;
    } else if (!source->mailbox.IsValid()) {
        LazyCreate(source);
        DCHECK(source->gl_id);
        DCHECK(source->origin == Resource::INTERNAL);
        if (source->image_id) {
            DCHECK(source->dirty_image);
            gl->BindTexture(resource->mailbox_holder.texture_target, source->gl_id);
            BindImageForSampling(source);
        }
        // This is a resource allocated by the compositor, we need to produce it.
        // Don't set a sync point, the caller will do it.
        gl->GenMailboxCHROMIUM(resource->mailbox_holder.mailbox.name);
        gl->ProduceTextureDirectCHROMIUM(source->gl_id,
            resource->mailbox_holder.texture_target,
            resource->mailbox_holder.mailbox.name);

        source->mailbox = TextureMailbox(resource->mailbox_holder);
    } else {
        DCHECK(source->mailbox.IsTexture());
        if (source->image_id && source->dirty_image) {
            DCHECK(source->gl_id);
            DCHECK(source->origin == Resource::INTERNAL);
            gl->BindTexture(resource->mailbox_holder.texture_target, source->gl_id);
            BindImageForSampling(source);
        }
        // This is either an external resource, or a compositor resource that we
        // already exported. Make sure to forward the sync point that we were given.
        resource->mailbox_holder.mailbox = source->mailbox.mailbox();
        resource->mailbox_holder.texture_target = source->mailbox.target();
        resource->mailbox_holder.sync_token = source->mailbox.sync_token();
        source->mailbox.set_sync_token(gpu::SyncToken());
    }
}

void ResourceProvider::DeleteAndReturnUnusedResourcesToChild(
    ChildMap::iterator child_it,
    DeleteStyle style,
    const ResourceIdArray& unused)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(child_it != children_.end());
    Child* child_info = &child_it->second;

    if (unused.empty() && !child_info->marked_for_deletion)
        return;

    ReturnedResourceArray to_return;

    GLES2Interface* gl = ContextGL();
    bool need_sync_point = false;
    for (size_t i = 0; i < unused.size(); ++i) {
        ResourceId local_id = unused[i];

        ResourceMap::iterator it = resources_.find(local_id);
        CHECK(it != resources_.end());
        Resource& resource = it->second;

        DCHECK(!resource.locked_for_write);
        DCHECK(child_info->parent_to_child_map.count(local_id));

        ResourceId child_id = child_info->parent_to_child_map[local_id];
        DCHECK(child_info->child_to_parent_map.count(child_id));

        bool is_lost = resource.lost || (resource.type == RESOURCE_TYPE_GL_TEXTURE && lost_output_surface_);
        if (resource.exported_count > 0 || resource.lock_for_read_count > 0) {
            if (style != FOR_SHUTDOWN) {
                // Defer this resource deletion.
                resource.marked_for_deletion = true;
                continue;
            }
            // We can't postpone the deletion, so we'll have to lose it.
            is_lost = true;
        } else if (!ReadLockFenceHasPassed(&resource)) {
            // TODO(dcastagna): see if it's possible to use this logic for
            // the branch above too, where the resource is locked or still exported.
            if (style != FOR_SHUTDOWN && !child_info->marked_for_deletion) {
                // Defer this resource deletion.
                resource.marked_for_deletion = true;
                continue;
            }
            // We can't postpone the deletion, so we'll have to lose it.
            is_lost = true;
        }

        if (gl && resource.filter != resource.original_filter) {
            DCHECK(resource.target);
            DCHECK(resource.gl_id);

            gl->BindTexture(resource.target, resource.gl_id);
            gl->TexParameteri(resource.target, GL_TEXTURE_MIN_FILTER,
                resource.original_filter);
            gl->TexParameteri(resource.target, GL_TEXTURE_MAG_FILTER,
                resource.original_filter);
        }

        ReturnedResource returned;
        returned.id = child_id;
        returned.sync_token = resource.mailbox.sync_token();
        if (!returned.sync_token.HasData() && resource.type == RESOURCE_TYPE_GL_TEXTURE)
            need_sync_point = true;
        returned.count = resource.imported_count;
        returned.lost = is_lost;
        to_return.push_back(returned);

        child_info->parent_to_child_map.erase(local_id);
        child_info->child_to_parent_map.erase(child_id);
        resource.imported_count = 0;
        DeleteResourceInternal(it, style);
    }
    if (need_sync_point && child_info->needs_sync_points) {
        DCHECK(gl);
        gpu::SyncToken sync_token(gl->InsertSyncPointCHROMIUM());
        for (size_t i = 0; i < to_return.size(); ++i) {
            if (!to_return[i].sync_token.HasData())
                to_return[i].sync_token = sync_token;
        }
    }

    if (!to_return.empty())
        child_info->return_callback.Run(to_return,
            blocking_main_thread_task_runner_);

    if (child_info->marked_for_deletion && child_info->parent_to_child_map.empty()) {
        DCHECK(child_info->child_to_parent_map.empty());
        children_.erase(child_it);
    }
}

GLenum ResourceProvider::BindForSampling(ResourceId resource_id,
    GLenum unit,
    GLenum filter)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    GLES2Interface* gl = ContextGL();
    ResourceMap::iterator it = resources_.find(resource_id);
    DCHECK(it != resources_.end());
    Resource* resource = &it->second;
    DCHECK(resource->lock_for_read_count);
    DCHECK(!resource->locked_for_write);

    ScopedSetActiveTexture scoped_active_tex(gl, unit);
    GLenum target = resource->target;
    gl->BindTexture(target, resource->gl_id);
    if (filter != resource->filter) {
        gl->TexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
        gl->TexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
        resource->filter = filter;
    }

    if (resource->image_id && resource->dirty_image)
        BindImageForSampling(resource);

    return target;
}

void ResourceProvider::CreateForTesting(ResourceId id)
{
    LazyCreate(GetResource(id));
}

GLenum ResourceProvider::TargetForTesting(ResourceId id)
{
    Resource* resource = GetResource(id);
    return resource->target;
}

void ResourceProvider::LazyCreate(Resource* resource)
{
    if (resource->type != RESOURCE_TYPE_GL_TEXTURE || resource->origin != Resource::INTERNAL)
        return;

    if (resource->gl_id)
        return;

    DCHECK(resource->texture_pool);
    DCHECK(resource->origin == Resource::INTERNAL);
    DCHECK(!resource->mailbox.IsValid());
    resource->gl_id = texture_id_allocator_->NextId();

    GLES2Interface* gl = ContextGL();
    DCHECK(gl);

    // Create and set texture properties. Allocation is delayed until needed.
    gl->BindTexture(resource->target, resource->gl_id);
    gl->TexParameteri(resource->target, GL_TEXTURE_MIN_FILTER,
        resource->original_filter);
    gl->TexParameteri(resource->target, GL_TEXTURE_MAG_FILTER,
        resource->original_filter);
    gl->TexParameteri(resource->target, GL_TEXTURE_WRAP_S, resource->wrap_mode);
    gl->TexParameteri(resource->target, GL_TEXTURE_WRAP_T, resource->wrap_mode);
    //gl->TexParameteri(resource->target, GL_TEXTURE_POOL_CHROMIUM, resource->texture_pool);
    if (use_texture_usage_hint_ && (resource->hint & TEXTURE_HINT_FRAMEBUFFER)) {
        gl->TexParameteri(resource->target, GL_TEXTURE_USAGE_ANGLE,
            GL_FRAMEBUFFER_ATTACHMENT_ANGLE);
    }
}

void ResourceProvider::AllocateForTesting(ResourceId id)
{
    LazyAllocate(GetResource(id));
}

void ResourceProvider::LazyAllocate(Resource* resource)
{
    DCHECK(resource);
    if (resource->allocated)
        return;
    LazyCreate(resource);
    if (!resource->gl_id)
        return;
    resource->allocated = true;
    GLES2Interface* gl = ContextGL();
    gfx::Size& size = resource->size;
    ResourceFormat format = resource->format;
    gl->BindTexture(resource->target, resource->gl_id);
    if (use_texture_storage_ext_ && IsFormatSupportedForStorage(format, use_texture_format_bgra_) && (resource->hint & TEXTURE_HINT_IMMUTABLE)) {
        GLenum storage_format = TextureToStorageFormat(format);
        gl->TexStorage2DEXT(resource->target, 1, storage_format, size.width(),
            size.height());
    } else {
        // ETC1 does not support preallocation.
        if (format != ETC1) {
            gl->TexImage2D(resource->target, 0, GLInternalFormat(format),
                size.width(), size.height(), 0, GLDataFormat(format),
                GLDataType(format), NULL);
        }
    }
}

void ResourceProvider::BindImageForSampling(Resource* resource)
{
    GLES2Interface* gl = ContextGL();
    DCHECK(resource->gl_id);
    DCHECK(resource->image_id);

    // Release image currently bound to texture.
    if (resource->bound_image_id)
        gl->ReleaseTexImage2DCHROMIUM(resource->target, resource->bound_image_id);
    gl->BindTexImage2DCHROMIUM(resource->target, resource->image_id);
    resource->bound_image_id = resource->image_id;
    resource->dirty_image = false;
}

void ResourceProvider::WaitSyncPointIfNeeded(ResourceId id)
{
    Resource* resource = GetResource(id);
    DCHECK_EQ(resource->exported_count, 0);
    DCHECK(resource->allocated);
    if (resource->type != RESOURCE_TYPE_GL_TEXTURE || resource->gl_id)
        return;
    if (!resource->mailbox.sync_token().HasData())
        return;
    DCHECK(resource->mailbox.IsValid());
    GLES2Interface* gl = ContextGL();
    DCHECK(gl);
    gl->WaitSyncTokenCHROMIUM(resource->mailbox.sync_token().GetConstData());
    resource->mailbox.set_sync_token(gpu::SyncToken());
}

GLint ResourceProvider::GetActiveTextureUnit(GLES2Interface* gl)
{
    GLint active_unit = 0;
    gl->GetIntegerv(GL_ACTIVE_TEXTURE, &active_unit);
    return active_unit;
}

GLenum ResourceProvider::GetImageTextureTarget(ResourceFormat format)
{
    gfx::BufferFormat buffer_format = BufferFormat(format);
    DCHECK_GT(use_image_texture_targets_.size(),
        static_cast<size_t>(buffer_format));
    return use_image_texture_targets_[static_cast<size_t>(buffer_format)];
}

void ResourceProvider::ValidateResource(ResourceId id) const
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(id);
    DCHECK(resources_.find(id) != resources_.end());
}

GLES2Interface* ResourceProvider::ContextGL() const
{
    ContextProvider* context_provider = output_surface_->context_provider();
    return context_provider ? context_provider->ContextGL() : NULL;
}

class GrContext* ResourceProvider::GrContext(bool worker_context) const
{
    ContextProvider* context_provider = worker_context ? output_surface_->worker_context_provider()
                                                       : output_surface_->context_provider();
    return context_provider ? context_provider->GrContext() : NULL;
}

bool ResourceProvider::OnMemoryDump(
    const base::trace_event::MemoryDumpArgs& args,
    base::trace_event::ProcessMemoryDump* pmd)
{
    //   DCHECK(thread_checker_.CalledOnValidThread());
    //
    //   const uint64 tracing_process_id =
    //       base::trace_event::MemoryDumpManager::GetInstance()
    //           ->GetTracingProcessId();
    //
    //   for (const auto& resource_entry : resources_) {
    //     const auto& resource = resource_entry.second;
    //
    //     // Resource IDs are not process-unique, so log with the ResourceProvider's
    //     // unique id.
    //     std::string dump_name =
    //         base::StringPrintf("cc/resource_memory/provider_%d/resource_%d",
    //                            tracing_id_, resource_entry.first);
    //     base::trace_event::MemoryAllocatorDump* dump =
    //         pmd->CreateAllocatorDump(dump_name);
    //
    //     uint64_t total_bytes = ResourceUtil::UncheckedSizeInBytesAligned<size_t>(
    //         resource.size, resource.format);
    //     dump->AddScalar(base::trace_event::MemoryAllocatorDump::kNameSize,
    //                     base::trace_event::MemoryAllocatorDump::kUnitsBytes,
    //                     static_cast<uint64_t>(total_bytes));
    //
    //     // Resources which are shared across processes require a shared GUID to
    //     // prevent double counting the memory. We currently support shared GUIDs for
    //     // GpuMemoryBuffer, SharedBitmap, and GL backed resources.
    //     base::trace_event::MemoryAllocatorDumpGuid guid;
    //     if (resource.gpu_memory_buffer) {
    //       DebugBreak();
    // //       guid = gfx::GetGpuMemoryBufferGUIDForTracing(
    // //           tracing_process_id, resource.gpu_memory_buffer->GetHandle().id);
    //     } else if (resource.shared_bitmap) {
    //       guid = GetSharedBitmapGUIDForTracing(resource.shared_bitmap->id());
    //     } else if (resource.gl_id && resource.allocated) {
    //       DebugBreak();
    // //       guid = gfx::GetGLTextureClientGUIDForTracing(
    // //           output_surface_->context_provider()
    // //               ->ContextSupport()
    // //               ->ShareGroupTracingGUID(),
    // //           resource.gl_id);
    //     }
    //
    //     if (!guid.empty()) {
    //       const int kImportance = 2;
    //       pmd->CreateSharedGlobalAllocatorDump(guid);
    //       pmd->AddOwnershipEdge(dump->guid(), guid, kImportance);
    //     }
    //   }

    return true;
}

} // namespace cc
