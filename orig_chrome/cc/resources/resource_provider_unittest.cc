// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/resources/resource_provider.h"

#include <algorithm>
#include <map>
#include <set>
#include <vector>

#include "base/bind.h"
#include "base/containers/hash_tables.h"
#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "cc/base/scoped_ptr_deque.h"
#include "cc/output/output_surface.h"
#include "cc/resources/returned_resource.h"
#include "cc/resources/shared_bitmap_manager.h"
#include "cc/resources/single_release_callback.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/fake_output_surface_client.h"
#include "cc/test/test_gpu_memory_buffer_manager.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_texture.h"
#include "cc/test/test_web_graphics_context_3d.h"
#include "cc/trees/blocking_task_runner.h"
#include "gpu/GLES2/gl2extchromium.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/khronos/GLES2/gl2.h"
#include "third_party/khronos/GLES2/gl2ext.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/gpu_memory_buffer.h"

using testing::_;
using testing::Mock;
using testing::NiceMock;
using testing::Return;
using testing::SetArgPointee;
using testing::StrictMock;

namespace cc {
namespace {

    static void EmptyReleaseCallback(uint32 sync_point,
        bool lost_resource,
        BlockingTaskRunner* main_thread_task_runner)
    {
    }

    static void ReleaseCallback(
        uint32* release_sync_point,
        bool* release_lost_resource,
        BlockingTaskRunner** release_main_thread_task_runner,
        uint32 sync_point,
        bool lost_resource,
        BlockingTaskRunner* main_thread_task_runner)
    {
        *release_sync_point = sync_point;
        *release_lost_resource = lost_resource;
        *release_main_thread_task_runner = main_thread_task_runner;
    }

    static void SharedBitmapReleaseCallback(
        scoped_ptr<SharedBitmap> bitmap,
        uint32 sync_point,
        bool lost_resource,
        BlockingTaskRunner* main_thread_task_runner)
    {
    }

    static void ReleaseSharedBitmapCallback(
        scoped_ptr<SharedBitmap> shared_bitmap,
        bool* release_called,
        uint32* release_sync_point,
        bool* lost_resource_result,
        uint32 sync_point,
        bool lost_resource,
        BlockingTaskRunner* main_thread_task_runner)
    {
        *release_called = true;
        *release_sync_point = sync_point;
        *lost_resource_result = lost_resource;
    }

    static scoped_ptr<SharedBitmap> CreateAndFillSharedBitmap(
        SharedBitmapManager* manager,
        const gfx::Size& size,
        uint32_t value)
    {
        scoped_ptr<SharedBitmap> shared_bitmap = manager->AllocateSharedBitmap(size);
        CHECK(shared_bitmap);
        uint32_t* pixels = reinterpret_cast<uint32_t*>(shared_bitmap->pixels());
        CHECK(pixels);
        std::fill_n(pixels, size.GetArea(), value);
        return shared_bitmap.Pass();
    }

    class TextureStateTrackingContext : public TestWebGraphicsContext3D {
    public:
        MOCK_METHOD2(bindTexture, void(GLenum target, GLuint texture));
        MOCK_METHOD3(texParameteri, void(GLenum target, GLenum pname, GLint param));
        MOCK_METHOD1(waitSyncPoint, void(GLuint sync_point));
        MOCK_METHOD0(insertSyncPoint, GLuint(void));
        MOCK_METHOD3(produceTextureDirectCHROMIUM,
            void(GLuint texture, GLenum target, const GLbyte* mailbox));
        MOCK_METHOD2(createAndConsumeTextureCHROMIUM,
            unsigned(GLenum target, const GLbyte* mailbox));

        // Force all textures to be consecutive numbers starting at "1",
        // so we easily can test for them.
        GLuint NextTextureId() override
        {
            base::AutoLock lock(namespace_->lock);
            return namespace_->next_texture_id++;
        }
        void RetireTextureId(GLuint) override { }
    };

    // Shared data between multiple ResourceProviderContext. This contains mailbox
    // contents as well as information about sync points.
    class ContextSharedData {
    public:
        static scoped_ptr<ContextSharedData> Create()
        {
            return make_scoped_ptr(new ContextSharedData());
        }

        uint32 InsertSyncPoint() { return next_sync_point_++; }

        void GenMailbox(GLbyte* mailbox)
        {
            memset(mailbox, 0, GL_MAILBOX_SIZE_CHROMIUM);
            memcpy(mailbox, &next_mailbox_, sizeof(next_mailbox_));
            ++next_mailbox_;
        }

        void ProduceTexture(const GLbyte* mailbox_name,
            uint32 sync_point,
            scoped_refptr<TestTexture> texture)
        {
            unsigned mailbox = 0;
            memcpy(&mailbox, mailbox_name, sizeof(mailbox));
            ASSERT_TRUE(mailbox && mailbox < next_mailbox_);
            textures_[mailbox] = texture;
            ASSERT_LT(sync_point_for_mailbox_[mailbox], sync_point);
            sync_point_for_mailbox_[mailbox] = sync_point;
        }

        scoped_refptr<TestTexture> ConsumeTexture(const GLbyte* mailbox_name,
            uint32 sync_point)
        {
            unsigned mailbox = 0;
            memcpy(&mailbox, mailbox_name, sizeof(mailbox));
            DCHECK(mailbox && mailbox < next_mailbox_);

            // If the latest sync point the context has waited on is before the sync
            // point for when the mailbox was set, pretend we never saw that
            // ProduceTexture.
            if (sync_point_for_mailbox_[mailbox] > sync_point) {
                NOTREACHED();
                return scoped_refptr<TestTexture>();
            }
            return textures_[mailbox];
        }

    private:
        ContextSharedData()
            : next_sync_point_(1)
            , next_mailbox_(1)
        {
        }

        uint32 next_sync_point_;
        unsigned next_mailbox_;
        typedef base::hash_map<unsigned, scoped_refptr<TestTexture>> TextureMap;
        TextureMap textures_;
        base::hash_map<unsigned, uint32> sync_point_for_mailbox_;
    };

    class ResourceProviderContext : public TestWebGraphicsContext3D {
    public:
        static scoped_ptr<ResourceProviderContext> Create(
            ContextSharedData* shared_data)
        {
            return make_scoped_ptr(new ResourceProviderContext(shared_data));
        }

        GLuint insertSyncPoint() override
        {
            uint32 sync_point = shared_data_->InsertSyncPoint();
            // Commit the produceTextureCHROMIUM calls at this point, so that
            // they're associated with the sync point.
            for (PendingProduceTextureList::iterator it = pending_produce_textures_.begin();
                 it != pending_produce_textures_.end();
                 ++it) {
                shared_data_->ProduceTexture(
                    (*it)->mailbox, sync_point, (*it)->texture);
            }
            pending_produce_textures_.clear();
            return sync_point;
        }

        void waitSyncPoint(GLuint sync_point) override
        {
            last_waited_sync_point_ = std::max(sync_point, last_waited_sync_point_);
        }

        unsigned last_waited_sync_point() const { return last_waited_sync_point_; }

        void texStorage2DEXT(GLenum target,
            GLint levels,
            GLuint internalformat,
            GLint width,
            GLint height) override
        {
            CheckTextureIsBound(target);
            ASSERT_EQ(static_cast<unsigned>(GL_TEXTURE_2D), target);
            ASSERT_EQ(1, levels);
            GLenum format = GL_RGBA;
            switch (internalformat) {
            case GL_RGBA8_OES:
                break;
            case GL_BGRA8_EXT:
                format = GL_BGRA_EXT;
                break;
            default:
                NOTREACHED();
            }
            AllocateTexture(gfx::Size(width, height), format);
        }

        void texImage2D(GLenum target,
            GLint level,
            GLenum internalformat,
            GLsizei width,
            GLsizei height,
            GLint border,
            GLenum format,
            GLenum type,
            const void* pixels) override
        {
            CheckTextureIsBound(target);
            ASSERT_EQ(static_cast<unsigned>(GL_TEXTURE_2D), target);
            ASSERT_FALSE(level);
            ASSERT_EQ(internalformat, format);
            ASSERT_FALSE(border);
            ASSERT_EQ(static_cast<unsigned>(GL_UNSIGNED_BYTE), type);
            AllocateTexture(gfx::Size(width, height), format);
            if (pixels)
                SetPixels(0, 0, width, height, pixels);
        }

        void texSubImage2D(GLenum target,
            GLint level,
            GLint xoffset,
            GLint yoffset,
            GLsizei width,
            GLsizei height,
            GLenum format,
            GLenum type,
            const void* pixels) override
        {
            CheckTextureIsBound(target);
            ASSERT_EQ(static_cast<unsigned>(GL_TEXTURE_2D), target);
            ASSERT_FALSE(level);
            ASSERT_EQ(static_cast<unsigned>(GL_UNSIGNED_BYTE), type);
            {
                base::AutoLock lock_for_texture_access(namespace_->lock);
                ASSERT_EQ(GLDataFormat(BoundTexture(target)->format), format);
            }
            ASSERT_TRUE(pixels);
            SetPixels(xoffset, yoffset, width, height, pixels);
        }

        void genMailboxCHROMIUM(GLbyte* mailbox) override
        {
            return shared_data_->GenMailbox(mailbox);
        }

        void produceTextureDirectCHROMIUM(GLuint texture,
            GLenum target,
            const GLbyte* mailbox) override
        {
            // Delay moving the texture into the mailbox until the next
            // InsertSyncPoint, so that it is not visible to other contexts that
            // haven't waited on that sync point.
            scoped_ptr<PendingProduceTexture> pending(new PendingProduceTexture);
            memcpy(pending->mailbox, mailbox, sizeof(pending->mailbox));
            base::AutoLock lock_for_texture_access(namespace_->lock);
            pending->texture = UnboundTexture(texture);
            pending_produce_textures_.push_back(pending.Pass());
        }

        GLuint createAndConsumeTextureCHROMIUM(GLenum target,
            const GLbyte* mailbox) override
        {
            GLuint texture_id = createTexture();
            base::AutoLock lock_for_texture_access(namespace_->lock);
            scoped_refptr<TestTexture> texture = shared_data_->ConsumeTexture(mailbox, last_waited_sync_point_);
            namespace_->textures.Replace(texture_id, texture);
            return texture_id;
        }

        void GetPixels(const gfx::Size& size,
            ResourceFormat format,
            uint8_t* pixels)
        {
            CheckTextureIsBound(GL_TEXTURE_2D);
            base::AutoLock lock_for_texture_access(namespace_->lock);
            scoped_refptr<TestTexture> texture = BoundTexture(GL_TEXTURE_2D);
            ASSERT_EQ(texture->size, size);
            ASSERT_EQ(texture->format, format);
            memcpy(pixels, texture->data.get(), TextureSizeBytes(size, format));
        }

    protected:
        explicit ResourceProviderContext(ContextSharedData* shared_data)
            : shared_data_(shared_data)
            , last_waited_sync_point_(0)
        {
        }

    private:
        void AllocateTexture(const gfx::Size& size, GLenum format)
        {
            CheckTextureIsBound(GL_TEXTURE_2D);
            ResourceFormat texture_format = RGBA_8888;
            switch (format) {
            case GL_RGBA:
                texture_format = RGBA_8888;
                break;
            case GL_BGRA_EXT:
                texture_format = BGRA_8888;
                break;
            }
            base::AutoLock lock_for_texture_access(namespace_->lock);
            BoundTexture(GL_TEXTURE_2D)->Reallocate(size, texture_format);
        }

        void SetPixels(int xoffset,
            int yoffset,
            int width,
            int height,
            const void* pixels)
        {
            CheckTextureIsBound(GL_TEXTURE_2D);
            base::AutoLock lock_for_texture_access(namespace_->lock);
            scoped_refptr<TestTexture> texture = BoundTexture(GL_TEXTURE_2D);
            ASSERT_TRUE(texture->data.get());
            ASSERT_TRUE(xoffset >= 0 && xoffset + width <= texture->size.width());
            ASSERT_TRUE(yoffset >= 0 && yoffset + height <= texture->size.height());
            ASSERT_TRUE(pixels);
            size_t in_pitch = TextureSizeBytes(gfx::Size(width, 1), texture->format);
            size_t out_pitch = TextureSizeBytes(gfx::Size(texture->size.width(), 1), texture->format);
            uint8_t* dest = texture->data.get() + yoffset * out_pitch + TextureSizeBytes(gfx::Size(xoffset, 1), texture->format);
            const uint8_t* src = static_cast<const uint8_t*>(pixels);
            for (int i = 0; i < height; ++i) {
                memcpy(dest, src, in_pitch);
                dest += out_pitch;
                src += in_pitch;
            }
        }

        struct PendingProduceTexture {
            GLbyte mailbox[GL_MAILBOX_SIZE_CHROMIUM];
            scoped_refptr<TestTexture> texture;
        };
        typedef ScopedPtrDeque<PendingProduceTexture> PendingProduceTextureList;
        ContextSharedData* shared_data_;
        GLuint last_waited_sync_point_;
        PendingProduceTextureList pending_produce_textures_;
    };

    void GetResourcePixels(ResourceProvider* resource_provider,
        ResourceProviderContext* context,
        ResourceId id,
        const gfx::Size& size,
        ResourceFormat format,
        uint8_t* pixels)
    {
        resource_provider->WaitSyncPointIfNeeded(id);
        switch (resource_provider->default_resource_type()) {
        case ResourceProvider::RESOURCE_TYPE_GL_TEXTURE: {
            ResourceProvider::ScopedReadLockGL lock_gl(resource_provider, id);
            ASSERT_NE(0U, lock_gl.texture_id());
            context->bindTexture(GL_TEXTURE_2D, lock_gl.texture_id());
            context->GetPixels(size, format, pixels);
            break;
        }
        case ResourceProvider::RESOURCE_TYPE_BITMAP: {
            ResourceProvider::ScopedReadLockSoftware lock_software(resource_provider,
                id);
            memcpy(pixels,
                lock_software.sk_bitmap()->getPixels(),
                lock_software.sk_bitmap()->getSize());
            break;
        }
        }
    }

    class ResourceProviderTest
        : public testing::TestWithParam<ResourceProvider::ResourceType> {
    public:
        explicit ResourceProviderTest(bool child_needs_sync_point)
            : shared_data_(ContextSharedData::Create())
            , context3d_(NULL)
            , child_context_(NULL)
            , main_thread_task_runner_(BlockingTaskRunner::Create(NULL))
        {
            switch (GetParam()) {
            case ResourceProvider::RESOURCE_TYPE_GL_TEXTURE: {
                scoped_ptr<ResourceProviderContext> context3d(
                    ResourceProviderContext::Create(shared_data_.get()));
                context3d_ = context3d.get();

                scoped_refptr<TestContextProvider> context_provider = TestContextProvider::Create(context3d.Pass());

                output_surface_ = FakeOutputSurface::Create3d(context_provider);

                scoped_ptr<ResourceProviderContext> child_context_owned = ResourceProviderContext::Create(shared_data_.get());
                child_context_ = child_context_owned.get();
                if (child_needs_sync_point) {
                    child_output_surface_ = FakeOutputSurface::Create3d(child_context_owned.Pass());
                } else {
                    child_output_surface_ = FakeOutputSurface::CreateNoRequireSyncPoint(
                        child_context_owned.Pass());
                }
                break;
            }
            case ResourceProvider::RESOURCE_TYPE_BITMAP:
                output_surface_ = FakeOutputSurface::CreateSoftware(
                    make_scoped_ptr(new SoftwareOutputDevice));
                child_output_surface_ = FakeOutputSurface::CreateSoftware(
                    make_scoped_ptr(new SoftwareOutputDevice));
                break;
            }
            CHECK(output_surface_->BindToClient(&output_surface_client_));
            CHECK(child_output_surface_->BindToClient(&child_output_surface_client_));

            shared_bitmap_manager_.reset(new TestSharedBitmapManager);
            gpu_memory_buffer_manager_.reset(new TestGpuMemoryBufferManager);

            resource_provider_ = ResourceProvider::Create(
                output_surface_.get(), shared_bitmap_manager_.get(),
                gpu_memory_buffer_manager_.get(), main_thread_task_runner_.get(), 0, 1,
                use_image_texture_targets_);
            child_resource_provider_ = ResourceProvider::Create(
                child_output_surface_.get(), shared_bitmap_manager_.get(),
                gpu_memory_buffer_manager_.get(), main_thread_task_runner_.get(), 0, 1,
                use_image_texture_targets_);
        }

        ResourceProviderTest()
            : ResourceProviderTest(true)
        {
        }

        static void CollectResources(ReturnedResourceArray* array,
            const ReturnedResourceArray& returned,
            BlockingTaskRunner* main_thread_task_runner)
        {
            array->insert(array->end(), returned.begin(), returned.end());
        }

        static ReturnCallback GetReturnCallback(ReturnedResourceArray* array)
        {
            return base::Bind(&ResourceProviderTest::CollectResources, array);
        }

        static void SetResourceFilter(ResourceProvider* resource_provider,
            ResourceId id,
            GLenum filter)
        {
            ResourceProvider::ScopedSamplerGL sampler(
                resource_provider, id, GL_TEXTURE_2D, filter);
        }

        ResourceProviderContext* context() { return context3d_; }

        ResourceId CreateChildMailbox(uint32* release_sync_point,
            bool* lost_resource,
            bool* release_called,
            uint32* sync_point)
        {
            if (GetParam() == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE) {
                unsigned texture = child_context_->createTexture();
                gpu::Mailbox gpu_mailbox;
                child_context_->genMailboxCHROMIUM(gpu_mailbox.name);
                child_context_->produceTextureDirectCHROMIUM(texture, GL_TEXTURE_2D,
                    gpu_mailbox.name);
                *sync_point = child_context_->insertSyncPoint();
                EXPECT_LT(0u, *sync_point);

                scoped_ptr<SharedBitmap> shared_bitmap;
                scoped_ptr<SingleReleaseCallbackImpl> callback = SingleReleaseCallbackImpl::Create(base::Bind(
                    ReleaseSharedBitmapCallback, base::Passed(&shared_bitmap),
                    release_called, release_sync_point, lost_resource));
                return child_resource_provider_->CreateResourceFromTextureMailbox(
                    TextureMailbox(gpu_mailbox, GL_TEXTURE_2D, *sync_point),
                    callback.Pass());
            } else {
                gfx::Size size(64, 64);
                scoped_ptr<SharedBitmap> shared_bitmap(
                    CreateAndFillSharedBitmap(shared_bitmap_manager_.get(), size, 0));

                SharedBitmap* shared_bitmap_ptr = shared_bitmap.get();
                scoped_ptr<SingleReleaseCallbackImpl> callback = SingleReleaseCallbackImpl::Create(base::Bind(
                    ReleaseSharedBitmapCallback, base::Passed(&shared_bitmap),
                    release_called, release_sync_point, lost_resource));
                return child_resource_provider_->CreateResourceFromTextureMailbox(
                    TextureMailbox(shared_bitmap_ptr, size), callback.Pass());
            }
        }

    public:
        static std::vector<unsigned> use_image_texture_targets()
        {
            return use_image_texture_targets_;
        }

    protected:
        static std::vector<unsigned> use_image_texture_targets_;
        scoped_ptr<ContextSharedData> shared_data_;
        ResourceProviderContext* context3d_;
        ResourceProviderContext* child_context_;
        FakeOutputSurfaceClient output_surface_client_;
        FakeOutputSurfaceClient child_output_surface_client_;
        scoped_ptr<OutputSurface> output_surface_;
        scoped_ptr<OutputSurface> child_output_surface_;
        scoped_ptr<BlockingTaskRunner> main_thread_task_runner_;
        scoped_ptr<ResourceProvider> resource_provider_;
        scoped_ptr<ResourceProvider> child_resource_provider_;
        scoped_ptr<TestSharedBitmapManager> shared_bitmap_manager_;
        scoped_ptr<TestGpuMemoryBufferManager> gpu_memory_buffer_manager_;
    };

    std::vector<unsigned> ResourceProviderTest::use_image_texture_targets_ = std::vector<unsigned>(static_cast<size_t>(gfx::BufferFormat::LAST) + 1,
        GL_TEXTURE_2D);

    void CheckCreateResource(ResourceProvider::ResourceType expected_default_type,
        ResourceProvider* resource_provider,
        ResourceProviderContext* context)
    {
        DCHECK_EQ(expected_default_type, resource_provider->default_resource_type());

        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        size_t pixel_size = TextureSizeBytes(size, format);
        ASSERT_EQ(4U, pixel_size);

        ResourceId id = resource_provider->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        EXPECT_EQ(1, static_cast<int>(resource_provider->num_resources()));
        if (expected_default_type == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            EXPECT_EQ(0u, context->NumTextures());

        uint8_t data[4] = { 1, 2, 3, 4 };
        resource_provider->CopyToResource(id, data, size);
        if (expected_default_type == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            EXPECT_EQ(1u, context->NumTextures());

        uint8_t result[4] = { 0 };
        GetResourcePixels(resource_provider, context, id, size, format, result);
        EXPECT_EQ(0, memcmp(data, result, pixel_size));

        resource_provider->DeleteResource(id);
        EXPECT_EQ(0, static_cast<int>(resource_provider->num_resources()));
        if (expected_default_type == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            EXPECT_EQ(0u, context->NumTextures());
    }

    TEST_P(ResourceProviderTest, Basic)
    {
        CheckCreateResource(GetParam(), resource_provider_.get(), context());
    }

    TEST_P(ResourceProviderTest, SimpleUpload)
    {
        gfx::Size size(2, 2);
        ResourceFormat format = RGBA_8888;
        size_t pixel_size = TextureSizeBytes(size, format);
        ASSERT_EQ(16U, pixel_size);

        ResourceId id = resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);

        uint8_t image[16] = { 0 };
        resource_provider_->CopyToResource(id, image, size);
        {
            uint8_t result[16] = { 0 };
            uint8_t expected[16] = { 0 };
            GetResourcePixels(resource_provider_.get(), context(), id, size, format,
                result);
            EXPECT_EQ(0, memcmp(expected, result, pixel_size));
        }

        for (uint8_t i = 0; i < pixel_size; ++i)
            image[i] = i;
        resource_provider_->CopyToResource(id, image, size);
        {
            uint8_t result[16] = { 0 };
            uint8_t expected[16] = {
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
            };
            GetResourcePixels(resource_provider_.get(), context(), id, size, format,
                result);
            EXPECT_EQ(0, memcmp(expected, result, pixel_size));
        }
    }

    TEST_P(ResourceProviderTest, TransferGLResources)
    {
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;
        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        size_t pixel_size = TextureSizeBytes(size, format);
        ASSERT_EQ(4U, pixel_size);

        ResourceId id1 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data1[4] = { 1, 2, 3, 4 };
        child_resource_provider_->CopyToResource(id1, data1, size);

        ResourceId id2 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data2[4] = { 5, 5, 5, 5 };
        child_resource_provider_->CopyToResource(id2, data2, size);

        ResourceId id3 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        {
            ResourceProvider::ScopedWriteLockGpuMemoryBuffer lock(
                child_resource_provider_.get(), id3);
            EXPECT_TRUE(lock.GetGpuMemoryBuffer());
        }

        GLuint external_texture_id = child_context_->createExternalTexture();

        gpu::Mailbox external_mailbox;
        child_context_->genMailboxCHROMIUM(external_mailbox.name);
        child_context_->produceTextureDirectCHROMIUM(
            external_texture_id, GL_TEXTURE_EXTERNAL_OES, external_mailbox.name);
        const GLuint external_sync_point = child_context_->insertSyncPoint();
        ResourceId id4 = child_resource_provider_->CreateResourceFromTextureMailbox(
            TextureMailbox(external_mailbox, GL_TEXTURE_EXTERNAL_OES,
                external_sync_point),
            SingleReleaseCallbackImpl::Create(base::Bind(&EmptyReleaseCallback)));

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));
        {
            // Transfer some resources to the parent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id1);
            resource_ids_to_transfer.push_back(id2);
            resource_ids_to_transfer.push_back(id3);
            resource_ids_to_transfer.push_back(id4);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            ASSERT_EQ(4u, list.size());
            EXPECT_NE(0u, list[0].mailbox_holder.sync_point);
            EXPECT_NE(0u, list[1].mailbox_holder.sync_point);
            EXPECT_EQ(list[0].mailbox_holder.sync_point,
                list[1].mailbox_holder.sync_point);
            EXPECT_NE(0u, list[2].mailbox_holder.sync_point);
            EXPECT_EQ(list[0].mailbox_holder.sync_point,
                list[2].mailbox_holder.sync_point);
            EXPECT_EQ(external_sync_point, list[3].mailbox_holder.sync_point);
            EXPECT_EQ(static_cast<GLenum>(GL_TEXTURE_2D),
                list[0].mailbox_holder.texture_target);
            EXPECT_EQ(static_cast<GLenum>(GL_TEXTURE_2D),
                list[1].mailbox_holder.texture_target);
            EXPECT_EQ(static_cast<GLenum>(GL_TEXTURE_2D),
                list[2].mailbox_holder.texture_target);
            EXPECT_EQ(static_cast<GLenum>(GL_TEXTURE_EXTERNAL_OES),
                list[3].mailbox_holder.texture_target);
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id1));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id2));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id3));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id4));
            resource_provider_->ReceiveFromChild(child_id, list);
            EXPECT_NE(list[0].mailbox_holder.sync_point,
                context3d_->last_waited_sync_point());
            {
                resource_provider_->WaitSyncPointIfNeeded(list[0].id);
                ResourceProvider::ScopedReadLockGL lock(resource_provider_.get(),
                    list[0].id);
            }
            EXPECT_EQ(list[0].mailbox_holder.sync_point,
                context3d_->last_waited_sync_point());
            ResourceProvider::ResourceIdSet resource_ids_to_receive;
            resource_ids_to_receive.insert(id1);
            resource_ids_to_receive.insert(id2);
            resource_ids_to_receive.insert(id3);
            resource_ids_to_receive.insert(id4);
            resource_provider_->DeclareUsedResourcesFromChild(child_id,
                resource_ids_to_receive);
        }

        EXPECT_EQ(4u, resource_provider_->num_resources());
        ResourceProvider::ResourceIdMap resource_map = resource_provider_->GetChildToParentMap(child_id);
        ResourceId mapped_id1 = resource_map[id1];
        ResourceId mapped_id2 = resource_map[id2];
        ResourceId mapped_id3 = resource_map[id3];
        ResourceId mapped_id4 = resource_map[id4];
        EXPECT_NE(0u, mapped_id1);
        EXPECT_NE(0u, mapped_id2);
        EXPECT_NE(0u, mapped_id3);
        EXPECT_NE(0u, mapped_id4);
        EXPECT_FALSE(resource_provider_->InUseByConsumer(id1));
        EXPECT_FALSE(resource_provider_->InUseByConsumer(id2));
        EXPECT_FALSE(resource_provider_->InUseByConsumer(id3));
        EXPECT_FALSE(resource_provider_->InUseByConsumer(id4));

        uint8_t result[4] = { 0 };
        GetResourcePixels(
            resource_provider_.get(), context(), mapped_id1, size, format, result);
        EXPECT_EQ(0, memcmp(data1, result, pixel_size));

        GetResourcePixels(
            resource_provider_.get(), context(), mapped_id2, size, format, result);
        EXPECT_EQ(0, memcmp(data2, result, pixel_size));

        {
            // Check that transfering again the same resource from the child to the
            // parent works.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id1);
            resource_ids_to_transfer.push_back(id2);
            resource_ids_to_transfer.push_back(id3);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            EXPECT_EQ(3u, list.size());
            EXPECT_EQ(id1, list[0].id);
            EXPECT_EQ(id2, list[1].id);
            EXPECT_EQ(id3, list[2].id);
            EXPECT_EQ(static_cast<GLenum>(GL_TEXTURE_2D),
                list[0].mailbox_holder.texture_target);
            EXPECT_EQ(static_cast<GLenum>(GL_TEXTURE_2D),
                list[1].mailbox_holder.texture_target);
            EXPECT_EQ(static_cast<GLenum>(GL_TEXTURE_2D),
                list[2].mailbox_holder.texture_target);
            ReturnedResourceArray returned;
            TransferableResource::ReturnResources(list, &returned);
            child_resource_provider_->ReceiveReturnsFromParent(returned);
            // ids were exported twice, we returned them only once, they should still
            // be in-use.
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id1));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id2));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id3));
        }
        {
            EXPECT_EQ(0u, returned_to_child.size());

            // Transfer resources back from the parent to the child. Set no resources as
            // being in use.
            ResourceProvider::ResourceIdSet no_resources;
            resource_provider_->DeclareUsedResourcesFromChild(child_id, no_resources);

            ASSERT_EQ(4u, returned_to_child.size());
            EXPECT_NE(0u, returned_to_child[0].sync_point);
            EXPECT_NE(0u, returned_to_child[1].sync_point);
            EXPECT_NE(0u, returned_to_child[2].sync_point);
            EXPECT_NE(0u, returned_to_child[3].sync_point);
            EXPECT_FALSE(returned_to_child[0].lost);
            EXPECT_FALSE(returned_to_child[1].lost);
            EXPECT_FALSE(returned_to_child[2].lost);
            EXPECT_FALSE(returned_to_child[3].lost);
            child_resource_provider_->ReceiveReturnsFromParent(returned_to_child);
            returned_to_child.clear();
        }
        EXPECT_FALSE(child_resource_provider_->InUseByConsumer(id1));
        EXPECT_FALSE(child_resource_provider_->InUseByConsumer(id2));
        EXPECT_FALSE(child_resource_provider_->InUseByConsumer(id3));
        EXPECT_FALSE(child_resource_provider_->InUseByConsumer(id4));

        {
            child_resource_provider_->WaitSyncPointIfNeeded(id1);
            ResourceProvider::ScopedReadLockGL lock(child_resource_provider_.get(),
                id1);
            ASSERT_NE(0U, lock.texture_id());
            child_context_->bindTexture(GL_TEXTURE_2D, lock.texture_id());
            child_context_->GetPixels(size, format, result);
            EXPECT_EQ(0, memcmp(data1, result, pixel_size));
        }
        {
            child_resource_provider_->WaitSyncPointIfNeeded(id2);
            ResourceProvider::ScopedReadLockGL lock(child_resource_provider_.get(),
                id2);
            ASSERT_NE(0U, lock.texture_id());
            child_context_->bindTexture(GL_TEXTURE_2D, lock.texture_id());
            child_context_->GetPixels(size, format, result);
            EXPECT_EQ(0, memcmp(data2, result, pixel_size));
        }
        {
            child_resource_provider_->WaitSyncPointIfNeeded(id3);
            ResourceProvider::ScopedReadLockGL lock(child_resource_provider_.get(),
                id3);
            ASSERT_NE(0U, lock.texture_id());
            child_context_->bindTexture(GL_TEXTURE_2D, lock.texture_id());
        }
        {
            // Transfer resources to the parent again.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id1);
            resource_ids_to_transfer.push_back(id2);
            resource_ids_to_transfer.push_back(id3);
            resource_ids_to_transfer.push_back(id4);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            ASSERT_EQ(4u, list.size());
            EXPECT_EQ(id1, list[0].id);
            EXPECT_EQ(id2, list[1].id);
            EXPECT_EQ(id3, list[2].id);
            EXPECT_EQ(id4, list[3].id);
            EXPECT_NE(0u, list[0].mailbox_holder.sync_point);
            EXPECT_NE(0u, list[1].mailbox_holder.sync_point);
            EXPECT_NE(0u, list[2].mailbox_holder.sync_point);
            EXPECT_NE(0u, list[3].mailbox_holder.sync_point);
            EXPECT_EQ(static_cast<GLenum>(GL_TEXTURE_2D),
                list[0].mailbox_holder.texture_target);
            EXPECT_EQ(static_cast<GLenum>(GL_TEXTURE_2D),
                list[1].mailbox_holder.texture_target);
            EXPECT_EQ(static_cast<GLenum>(GL_TEXTURE_2D),
                list[2].mailbox_holder.texture_target);
            EXPECT_EQ(static_cast<GLenum>(GL_TEXTURE_EXTERNAL_OES),
                list[3].mailbox_holder.texture_target);
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id1));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id2));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id3));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id4));
            resource_provider_->ReceiveFromChild(child_id, list);
            ResourceProvider::ResourceIdSet resource_ids_to_receive;
            resource_ids_to_receive.insert(id1);
            resource_ids_to_receive.insert(id2);
            resource_ids_to_receive.insert(id3);
            resource_ids_to_receive.insert(id4);
            resource_provider_->DeclareUsedResourcesFromChild(child_id,
                resource_ids_to_receive);
        }

        EXPECT_EQ(0u, returned_to_child.size());

        EXPECT_EQ(4u, resource_provider_->num_resources());
        resource_provider_->DestroyChild(child_id);
        EXPECT_EQ(0u, resource_provider_->num_resources());

        ASSERT_EQ(4u, returned_to_child.size());
        EXPECT_NE(0u, returned_to_child[0].sync_point);
        EXPECT_NE(0u, returned_to_child[1].sync_point);
        EXPECT_NE(0u, returned_to_child[2].sync_point);
        EXPECT_NE(0u, returned_to_child[3].sync_point);
        EXPECT_FALSE(returned_to_child[0].lost);
        EXPECT_FALSE(returned_to_child[1].lost);
        EXPECT_FALSE(returned_to_child[2].lost);
        EXPECT_FALSE(returned_to_child[3].lost);
    }

    class ResourceProviderTestNoSyncPoint : public ResourceProviderTest {
    public:
        ResourceProviderTestNoSyncPoint()
            : ResourceProviderTest(false)
        {
            EXPECT_EQ(ResourceProvider::RESOURCE_TYPE_GL_TEXTURE, GetParam());
        }
    };

    TEST_P(ResourceProviderTestNoSyncPoint, TransferGLResources)
    {
        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        size_t pixel_size = TextureSizeBytes(size, format);
        ASSERT_EQ(4U, pixel_size);

        ResourceId id1 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data1[4] = { 1, 2, 3, 4 };
        child_resource_provider_->CopyToResource(id1, data1, size);

        ResourceId id2 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        {
            // Ensure locking the memory buffer doesn't create an unnecessary sync
            // point.
            ResourceProvider::ScopedWriteLockGpuMemoryBuffer lock(
                child_resource_provider_.get(), id2);
            EXPECT_TRUE(lock.GetGpuMemoryBuffer());
        }

        GLuint external_texture_id = child_context_->createExternalTexture();

        // A sync point is specified directly and should be used.
        gpu::Mailbox external_mailbox;
        child_context_->genMailboxCHROMIUM(external_mailbox.name);
        child_context_->produceTextureDirectCHROMIUM(
            external_texture_id, GL_TEXTURE_EXTERNAL_OES, external_mailbox.name);
        const GLuint external_sync_point = child_context_->insertSyncPoint();
        ResourceId id3 = child_resource_provider_->CreateResourceFromTextureMailbox(
            TextureMailbox(external_mailbox, GL_TEXTURE_EXTERNAL_OES,
                external_sync_point),
            SingleReleaseCallbackImpl::Create(base::Bind(&EmptyReleaseCallback)));

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));
        resource_provider_->SetChildNeedsSyncPoints(child_id, false);
        {
            // Transfer some resources to the parent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id1);
            resource_ids_to_transfer.push_back(id2);
            resource_ids_to_transfer.push_back(id3);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            ASSERT_EQ(3u, list.size());
            // Standard resources shouldn't require creating and sending a sync point.
            EXPECT_EQ(0u, list[0].mailbox_holder.sync_point);
            EXPECT_EQ(0u, list[1].mailbox_holder.sync_point);
            // A given sync point should be passed through.
            EXPECT_EQ(external_sync_point, list[2].mailbox_holder.sync_point);
            resource_provider_->ReceiveFromChild(child_id, list);

            ResourceProvider::ResourceIdSet resource_ids_to_receive;
            resource_ids_to_receive.insert(id1);
            resource_ids_to_receive.insert(id2);
            resource_ids_to_receive.insert(id3);
            resource_provider_->DeclareUsedResourcesFromChild(child_id,
                resource_ids_to_receive);
        }

        {
            EXPECT_EQ(0u, returned_to_child.size());

            // Transfer resources back from the parent to the child. Set no resources as
            // being in use.
            ResourceProvider::ResourceIdSet no_resources;
            resource_provider_->DeclareUsedResourcesFromChild(child_id, no_resources);

            ASSERT_EQ(3u, returned_to_child.size());
            std::map<ResourceId, unsigned int> returned_sync_points;
            for (const auto& returned : returned_to_child)
                returned_sync_points[returned.id] = returned.sync_point;

            EXPECT_TRUE(returned_sync_points.find(id1) != returned_sync_points.end());
            // No new sync point should be created transferring back.
            EXPECT_TRUE(returned_sync_points.find(id1) != returned_sync_points.end());
            EXPECT_EQ(0u, returned_sync_points[id1]);
            EXPECT_TRUE(returned_sync_points.find(id2) != returned_sync_points.end());
            EXPECT_EQ(0u, returned_sync_points[id2]);
            // Original sync point given should be returned.
            EXPECT_TRUE(returned_sync_points.find(id3) != returned_sync_points.end());
            EXPECT_EQ(external_sync_point, returned_sync_points[id3]);
            EXPECT_FALSE(returned_to_child[0].lost);
            EXPECT_FALSE(returned_to_child[1].lost);
            EXPECT_FALSE(returned_to_child[2].lost);
            child_resource_provider_->ReceiveReturnsFromParent(returned_to_child);
            returned_to_child.clear();
        }

        resource_provider_->DestroyChild(child_id);
    }

    INSTANTIATE_TEST_CASE_P(
        ResourceProviderTests,
        ResourceProviderTestNoSyncPoint,
        ::testing::Values(ResourceProvider::RESOURCE_TYPE_GL_TEXTURE));

    TEST_P(ResourceProviderTest, ReadLockCountStopsReturnToChildOrDelete)
    {
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;
        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;

        ResourceId id1 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data1[4] = { 1, 2, 3, 4 };
        child_resource_provider_->CopyToResource(id1, data1, size);

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));
        {
            // Transfer some resources to the parent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id1);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            ASSERT_EQ(1u, list.size());
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id1));

            resource_provider_->ReceiveFromChild(child_id, list);

            resource_provider_->WaitSyncPointIfNeeded(list[0].id);
            ResourceProvider::ScopedReadLockGL lock(resource_provider_.get(),
                list[0].id);

            resource_provider_->DeclareUsedResourcesFromChild(
                child_id, ResourceProvider::ResourceIdSet());
            EXPECT_EQ(0u, returned_to_child.size());
        }

        EXPECT_EQ(1u, returned_to_child.size());
        child_resource_provider_->ReceiveReturnsFromParent(returned_to_child);

        {
            child_resource_provider_->WaitSyncPointIfNeeded(id1);
            ResourceProvider::ScopedReadLockGL lock(child_resource_provider_.get(),
                id1);
            child_resource_provider_->DeleteResource(id1);
            EXPECT_EQ(1u, child_resource_provider_->num_resources());
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id1));
        }

        EXPECT_EQ(0u, child_resource_provider_->num_resources());
        resource_provider_->DestroyChild(child_id);
    }

    class TestFence : public ResourceProvider::Fence {
    public:
        TestFence() { }

        void Set() override { }
        bool HasPassed() override { return passed; }
        void Wait() override { }

        bool passed = false;

    private:
        ~TestFence() override { }
    };

    TEST_P(ResourceProviderTest, ReadLockFenceStopsReturnToChildOrDelete)
    {
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;
        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;

        ResourceId id1 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data1[4] = { 1, 2, 3, 4 };
        child_resource_provider_->CopyToResource(id1, data1, size);
        child_resource_provider_->EnableReadLockFencesForTesting(id1);
        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));

        // Transfer some resources to the parent.
        ResourceProvider::ResourceIdArray resource_ids_to_transfer;
        resource_ids_to_transfer.push_back(id1);
        TransferableResourceArray list;
        child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
            &list);
        ASSERT_EQ(1u, list.size());
        EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id1));
        EXPECT_TRUE(list[0].read_lock_fences_enabled);

        resource_provider_->ReceiveFromChild(child_id, list);

        scoped_refptr<TestFence> fence(new TestFence);
        resource_provider_->SetReadLockFence(fence.get());
        {
            unsigned parent_id = list.front().id;
            resource_provider_->WaitSyncPointIfNeeded(parent_id);
            ResourceProvider::ScopedReadLockGL lock(resource_provider_.get(),
                parent_id);
        }
        resource_provider_->DeclareUsedResourcesFromChild(
            child_id, ResourceProvider::ResourceIdSet());
        EXPECT_EQ(0u, returned_to_child.size());

        resource_provider_->DeclareUsedResourcesFromChild(
            child_id, ResourceProvider::ResourceIdSet());
        EXPECT_EQ(0u, returned_to_child.size());
        fence->passed = true;

        resource_provider_->DeclareUsedResourcesFromChild(
            child_id, ResourceProvider::ResourceIdSet());
        EXPECT_EQ(1u, returned_to_child.size());

        child_resource_provider_->ReceiveReturnsFromParent(returned_to_child);
        child_resource_provider_->DeleteResource(id1);
        EXPECT_EQ(0u, child_resource_provider_->num_resources());
    }

    TEST_P(ResourceProviderTest, ReadLockFenceDestroyChild)
    {
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;
        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;

        ResourceId id1 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data[4] = { 1, 2, 3, 4 };
        child_resource_provider_->CopyToResource(id1, data, size);
        child_resource_provider_->EnableReadLockFencesForTesting(id1);

        ResourceId id2 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        child_resource_provider_->CopyToResource(id2, data, size);

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));

        // Transfer resources to the parent.
        ResourceProvider::ResourceIdArray resource_ids_to_transfer;
        resource_ids_to_transfer.push_back(id1);
        resource_ids_to_transfer.push_back(id2);
        TransferableResourceArray list;
        child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
            &list);
        ASSERT_EQ(2u, list.size());
        EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id1));
        EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id2));

        resource_provider_->ReceiveFromChild(child_id, list);

        scoped_refptr<TestFence> fence(new TestFence);
        resource_provider_->SetReadLockFence(fence.get());
        {
            for (size_t i = 0; i < list.size(); i++) {
                unsigned parent_id = list[i].id;
                resource_provider_->WaitSyncPointIfNeeded(parent_id);
                ResourceProvider::ScopedReadLockGL lock(resource_provider_.get(),
                    parent_id);
            }
        }
        EXPECT_EQ(0u, returned_to_child.size());

        EXPECT_EQ(2u, resource_provider_->num_resources());

        resource_provider_->DestroyChild(child_id);

        EXPECT_EQ(0u, resource_provider_->num_resources());
        EXPECT_EQ(2u, returned_to_child.size());

        // id1 should be lost and id2 should not.
        EXPECT_EQ(returned_to_child[0].lost, returned_to_child[0].id == id1);
        EXPECT_EQ(returned_to_child[1].lost, returned_to_child[1].id == id1);

        child_resource_provider_->ReceiveReturnsFromParent(returned_to_child);
        child_resource_provider_->DeleteResource(id1);
        child_resource_provider_->DeleteResource(id2);
        EXPECT_EQ(0u, child_resource_provider_->num_resources());
    }

    TEST_P(ResourceProviderTest, ReadLockFenceContextLost)
    {
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;
        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;

        ResourceId id1 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data[4] = { 1, 2, 3, 4 };
        child_resource_provider_->CopyToResource(id1, data, size);
        child_resource_provider_->EnableReadLockFencesForTesting(id1);

        ResourceId id2 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        child_resource_provider_->CopyToResource(id2, data, size);

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));

        // Transfer resources to the parent.
        ResourceProvider::ResourceIdArray resource_ids_to_transfer;
        resource_ids_to_transfer.push_back(id1);
        resource_ids_to_transfer.push_back(id2);
        TransferableResourceArray list;
        child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
            &list);
        ASSERT_EQ(2u, list.size());
        EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id1));
        EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id2));

        resource_provider_->ReceiveFromChild(child_id, list);

        scoped_refptr<TestFence> fence(new TestFence);
        resource_provider_->SetReadLockFence(fence.get());
        {
            for (size_t i = 0; i < list.size(); i++) {
                unsigned parent_id = list[i].id;
                resource_provider_->WaitSyncPointIfNeeded(parent_id);
                ResourceProvider::ScopedReadLockGL lock(resource_provider_.get(),
                    parent_id);
            }
        }
        EXPECT_EQ(0u, returned_to_child.size());

        EXPECT_EQ(2u, resource_provider_->num_resources());
        resource_provider_->DidLoseOutputSurface();
        resource_provider_ = nullptr;

        EXPECT_EQ(2u, returned_to_child.size());

        EXPECT_TRUE(returned_to_child[0].lost);
        EXPECT_TRUE(returned_to_child[1].lost);
    }

    TEST_P(ResourceProviderTest, TransferSoftwareResources)
    {
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_BITMAP)
            return;

        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        size_t pixel_size = TextureSizeBytes(size, format);
        ASSERT_EQ(4U, pixel_size);

        ResourceId id1 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data1[4] = { 1, 2, 3, 4 };
        child_resource_provider_->CopyToResource(id1, data1, size);

        ResourceId id2 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data2[4] = { 5, 5, 5, 5 };
        child_resource_provider_->CopyToResource(id2, data2, size);

        scoped_ptr<SharedBitmap> shared_bitmap(CreateAndFillSharedBitmap(
            shared_bitmap_manager_.get(), gfx::Size(1, 1), 0));
        SharedBitmap* shared_bitmap_ptr = shared_bitmap.get();
        ResourceId id3 = child_resource_provider_->CreateResourceFromTextureMailbox(
            TextureMailbox(shared_bitmap_ptr, gfx::Size(1, 1)),
            SingleReleaseCallbackImpl::Create(base::Bind(
                &SharedBitmapReleaseCallback, base::Passed(&shared_bitmap))));

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));
        {
            // Transfer some resources to the parent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id1);
            resource_ids_to_transfer.push_back(id2);
            resource_ids_to_transfer.push_back(id3);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            ASSERT_EQ(3u, list.size());
            EXPECT_EQ(0u, list[0].mailbox_holder.sync_point);
            EXPECT_EQ(0u, list[1].mailbox_holder.sync_point);
            EXPECT_EQ(0u, list[2].mailbox_holder.sync_point);
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id1));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id2));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id3));
            resource_provider_->ReceiveFromChild(child_id, list);
            ResourceProvider::ResourceIdSet resource_ids_to_receive;
            resource_ids_to_receive.insert(id1);
            resource_ids_to_receive.insert(id2);
            resource_ids_to_receive.insert(id3);
            resource_provider_->DeclareUsedResourcesFromChild(child_id,
                resource_ids_to_receive);
        }

        EXPECT_EQ(3u, resource_provider_->num_resources());
        ResourceProvider::ResourceIdMap resource_map = resource_provider_->GetChildToParentMap(child_id);
        ResourceId mapped_id1 = resource_map[id1];
        ResourceId mapped_id2 = resource_map[id2];
        ResourceId mapped_id3 = resource_map[id3];
        EXPECT_NE(0u, mapped_id1);
        EXPECT_NE(0u, mapped_id2);
        EXPECT_NE(0u, mapped_id3);
        EXPECT_FALSE(resource_provider_->InUseByConsumer(id1));
        EXPECT_FALSE(resource_provider_->InUseByConsumer(id2));
        EXPECT_FALSE(resource_provider_->InUseByConsumer(id3));

        uint8_t result[4] = { 0 };
        GetResourcePixels(
            resource_provider_.get(), context(), mapped_id1, size, format, result);
        EXPECT_EQ(0, memcmp(data1, result, pixel_size));

        GetResourcePixels(
            resource_provider_.get(), context(), mapped_id2, size, format, result);
        EXPECT_EQ(0, memcmp(data2, result, pixel_size));

        {
            // Check that transfering again the same resource from the child to the
            // parent works.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id1);
            resource_ids_to_transfer.push_back(id2);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            EXPECT_EQ(2u, list.size());
            EXPECT_EQ(id1, list[0].id);
            EXPECT_EQ(id2, list[1].id);
            ReturnedResourceArray returned;
            TransferableResource::ReturnResources(list, &returned);
            child_resource_provider_->ReceiveReturnsFromParent(returned);
            // ids were exported twice, we returned them only once, they should still
            // be in-use.
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id1));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id2));
        }
        {
            EXPECT_EQ(0u, returned_to_child.size());

            // Transfer resources back from the parent to the child. Set no resources as
            // being in use.
            ResourceProvider::ResourceIdSet no_resources;
            resource_provider_->DeclareUsedResourcesFromChild(child_id, no_resources);

            ASSERT_EQ(3u, returned_to_child.size());
            EXPECT_EQ(0u, returned_to_child[0].sync_point);
            EXPECT_EQ(0u, returned_to_child[1].sync_point);
            EXPECT_EQ(0u, returned_to_child[2].sync_point);
            std::set<ResourceId> expected_ids;
            expected_ids.insert(id1);
            expected_ids.insert(id2);
            expected_ids.insert(id3);
            std::set<ResourceId> returned_ids;
            for (unsigned i = 0; i < 3; i++)
                returned_ids.insert(returned_to_child[i].id);
            EXPECT_EQ(expected_ids, returned_ids);
            EXPECT_FALSE(returned_to_child[0].lost);
            EXPECT_FALSE(returned_to_child[1].lost);
            EXPECT_FALSE(returned_to_child[2].lost);
            child_resource_provider_->ReceiveReturnsFromParent(returned_to_child);
            returned_to_child.clear();
        }
        EXPECT_FALSE(child_resource_provider_->InUseByConsumer(id1));
        EXPECT_FALSE(child_resource_provider_->InUseByConsumer(id2));
        EXPECT_FALSE(child_resource_provider_->InUseByConsumer(id3));

        {
            ResourceProvider::ScopedReadLockSoftware lock(
                child_resource_provider_.get(), id1);
            const SkBitmap* sk_bitmap = lock.sk_bitmap();
            EXPECT_EQ(sk_bitmap->width(), size.width());
            EXPECT_EQ(sk_bitmap->height(), size.height());
            EXPECT_EQ(0, memcmp(data1, sk_bitmap->getPixels(), pixel_size));
        }
        {
            ResourceProvider::ScopedReadLockSoftware lock(
                child_resource_provider_.get(), id2);
            const SkBitmap* sk_bitmap = lock.sk_bitmap();
            EXPECT_EQ(sk_bitmap->width(), size.width());
            EXPECT_EQ(sk_bitmap->height(), size.height());
            EXPECT_EQ(0, memcmp(data2, sk_bitmap->getPixels(), pixel_size));
        }
        {
            // Transfer resources to the parent again.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id1);
            resource_ids_to_transfer.push_back(id2);
            resource_ids_to_transfer.push_back(id3);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            ASSERT_EQ(3u, list.size());
            EXPECT_EQ(id1, list[0].id);
            EXPECT_EQ(id2, list[1].id);
            EXPECT_EQ(id3, list[2].id);
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id1));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id2));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id3));
            resource_provider_->ReceiveFromChild(child_id, list);
            ResourceProvider::ResourceIdSet resource_ids_to_receive;
            resource_ids_to_receive.insert(id1);
            resource_ids_to_receive.insert(id2);
            resource_ids_to_receive.insert(id3);
            resource_provider_->DeclareUsedResourcesFromChild(child_id,
                resource_ids_to_receive);
        }

        EXPECT_EQ(0u, returned_to_child.size());

        EXPECT_EQ(3u, resource_provider_->num_resources());
        resource_provider_->DestroyChild(child_id);
        EXPECT_EQ(0u, resource_provider_->num_resources());

        ASSERT_EQ(3u, returned_to_child.size());
        EXPECT_EQ(0u, returned_to_child[0].sync_point);
        EXPECT_EQ(0u, returned_to_child[1].sync_point);
        EXPECT_EQ(0u, returned_to_child[2].sync_point);
        std::set<ResourceId> expected_ids;
        expected_ids.insert(id1);
        expected_ids.insert(id2);
        expected_ids.insert(id3);
        std::set<ResourceId> returned_ids;
        for (unsigned i = 0; i < 3; i++)
            returned_ids.insert(returned_to_child[i].id);
        EXPECT_EQ(expected_ids, returned_ids);
        EXPECT_FALSE(returned_to_child[0].lost);
        EXPECT_FALSE(returned_to_child[1].lost);
        EXPECT_FALSE(returned_to_child[2].lost);
    }

    TEST_P(ResourceProviderTest, TransferGLToSoftware)
    {
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_BITMAP)
            return;

        scoped_ptr<ResourceProviderContext> child_context_owned(
            ResourceProviderContext::Create(shared_data_.get()));

        FakeOutputSurfaceClient child_output_surface_client;
        scoped_ptr<OutputSurface> child_output_surface(
            FakeOutputSurface::Create3d(child_context_owned.Pass()));
        CHECK(child_output_surface->BindToClient(&child_output_surface_client));

        scoped_ptr<ResourceProvider> child_resource_provider(ResourceProvider::Create(
            child_output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), NULL, 0, 1,
            use_image_texture_targets_));

        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        size_t pixel_size = TextureSizeBytes(size, format);
        ASSERT_EQ(4U, pixel_size);

        ResourceId id1 = child_resource_provider->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data1[4] = { 1, 2, 3, 4 };
        child_resource_provider->CopyToResource(id1, data1, size);

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));
        {
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id1);
            TransferableResourceArray list;
            child_resource_provider->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            ASSERT_EQ(1u, list.size());
            EXPECT_NE(0u, list[0].mailbox_holder.sync_point);
            EXPECT_EQ(static_cast<GLenum>(GL_TEXTURE_2D),
                list[0].mailbox_holder.texture_target);
            EXPECT_TRUE(child_resource_provider->InUseByConsumer(id1));
            resource_provider_->ReceiveFromChild(child_id, list);
        }

        EXPECT_EQ(0u, resource_provider_->num_resources());
        ASSERT_EQ(1u, returned_to_child.size());
        EXPECT_EQ(returned_to_child[0].id, id1);
        ResourceProvider::ResourceIdMap resource_map = resource_provider_->GetChildToParentMap(child_id);
        ResourceId mapped_id1 = resource_map[id1];
        EXPECT_EQ(0u, mapped_id1);

        resource_provider_->DestroyChild(child_id);
        EXPECT_EQ(0u, resource_provider_->num_resources());

        ASSERT_EQ(1u, returned_to_child.size());
        EXPECT_FALSE(returned_to_child[0].lost);
    }

    TEST_P(ResourceProviderTest, TransferInvalidSoftware)
    {
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_BITMAP)
            return;

        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        size_t pixel_size = TextureSizeBytes(size, format);
        ASSERT_EQ(4U, pixel_size);

        ResourceId id1 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data1[4] = { 1, 2, 3, 4 };
        child_resource_provider_->CopyToResource(id1, data1, size);

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));
        {
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id1);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            ASSERT_EQ(1u, list.size());
            // Make invalid.
            list[0].mailbox_holder.mailbox.name[1] = 5;
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id1));
            resource_provider_->ReceiveFromChild(child_id, list);
        }

        EXPECT_EQ(1u, resource_provider_->num_resources());
        EXPECT_EQ(0u, returned_to_child.size());

        ResourceProvider::ResourceIdMap resource_map = resource_provider_->GetChildToParentMap(child_id);
        ResourceId mapped_id1 = resource_map[id1];
        EXPECT_NE(0u, mapped_id1);
        {
            ResourceProvider::ScopedReadLockSoftware lock(resource_provider_.get(),
                mapped_id1);
            EXPECT_FALSE(lock.valid());
        }

        resource_provider_->DestroyChild(child_id);
        EXPECT_EQ(0u, resource_provider_->num_resources());

        ASSERT_EQ(1u, returned_to_child.size());
        EXPECT_FALSE(returned_to_child[0].lost);
    }

    TEST_P(ResourceProviderTest, DeleteExportedResources)
    {
        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        size_t pixel_size = TextureSizeBytes(size, format);
        ASSERT_EQ(4U, pixel_size);

        ResourceId id1 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data1[4] = { 1, 2, 3, 4 };
        child_resource_provider_->CopyToResource(id1, data1, size);

        ResourceId id2 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data2[4] = { 5, 5, 5, 5 };
        child_resource_provider_->CopyToResource(id2, data2, size);

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));
        {
            // Transfer some resources to the parent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id1);
            resource_ids_to_transfer.push_back(id2);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            ASSERT_EQ(2u, list.size());
            if (GetParam() == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE) {
                EXPECT_NE(0u, list[0].mailbox_holder.sync_point);
                EXPECT_NE(0u, list[1].mailbox_holder.sync_point);
            }
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id1));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id2));
            resource_provider_->ReceiveFromChild(child_id, list);
            ResourceProvider::ResourceIdSet resource_ids_to_receive;
            resource_ids_to_receive.insert(id1);
            resource_ids_to_receive.insert(id2);
            resource_provider_->DeclareUsedResourcesFromChild(child_id,
                resource_ids_to_receive);
        }

        EXPECT_EQ(2u, resource_provider_->num_resources());
        ResourceProvider::ResourceIdMap resource_map = resource_provider_->GetChildToParentMap(child_id);
        ResourceId mapped_id1 = resource_map[id1];
        ResourceId mapped_id2 = resource_map[id2];
        EXPECT_NE(0u, mapped_id1);
        EXPECT_NE(0u, mapped_id2);
        EXPECT_FALSE(resource_provider_->InUseByConsumer(id1));
        EXPECT_FALSE(resource_provider_->InUseByConsumer(id2));

        {
            // The parent transfers the resources to the grandparent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(mapped_id1);
            resource_ids_to_transfer.push_back(mapped_id2);
            TransferableResourceArray list;
            resource_provider_->PrepareSendToParent(resource_ids_to_transfer, &list);

            ASSERT_EQ(2u, list.size());
            if (GetParam() == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE) {
                EXPECT_NE(0u, list[0].mailbox_holder.sync_point);
                EXPECT_NE(0u, list[1].mailbox_holder.sync_point);
            }
            EXPECT_TRUE(resource_provider_->InUseByConsumer(id1));
            EXPECT_TRUE(resource_provider_->InUseByConsumer(id2));

            // Release the resource in the parent. Set no resources as being in use. The
            // resources are exported so that can't be transferred back yet.
            ResourceProvider::ResourceIdSet no_resources;
            resource_provider_->DeclareUsedResourcesFromChild(child_id, no_resources);

            EXPECT_EQ(0u, returned_to_child.size());
            EXPECT_EQ(2u, resource_provider_->num_resources());

            // Return the resources from the grandparent to the parent. They should be
            // returned to the child then.
            EXPECT_EQ(2u, list.size());
            EXPECT_EQ(mapped_id1, list[0].id);
            EXPECT_EQ(mapped_id2, list[1].id);
            ReturnedResourceArray returned;
            TransferableResource::ReturnResources(list, &returned);
            resource_provider_->ReceiveReturnsFromParent(returned);

            EXPECT_EQ(0u, resource_provider_->num_resources());
            ASSERT_EQ(2u, returned_to_child.size());
            if (GetParam() == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE) {
                EXPECT_NE(0u, returned_to_child[0].sync_point);
                EXPECT_NE(0u, returned_to_child[1].sync_point);
            }
            EXPECT_FALSE(returned_to_child[0].lost);
            EXPECT_FALSE(returned_to_child[1].lost);
        }
    }

    TEST_P(ResourceProviderTest, DestroyChildWithExportedResources)
    {
        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        size_t pixel_size = TextureSizeBytes(size, format);
        ASSERT_EQ(4U, pixel_size);

        ResourceId id1 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data1[4] = { 1, 2, 3, 4 };
        child_resource_provider_->CopyToResource(id1, data1, size);

        ResourceId id2 = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data2[4] = { 5, 5, 5, 5 };
        child_resource_provider_->CopyToResource(id2, data2, size);

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));
        {
            // Transfer some resources to the parent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id1);
            resource_ids_to_transfer.push_back(id2);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            ASSERT_EQ(2u, list.size());
            if (GetParam() == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE) {
                EXPECT_NE(0u, list[0].mailbox_holder.sync_point);
                EXPECT_NE(0u, list[1].mailbox_holder.sync_point);
            }
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id1));
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id2));
            resource_provider_->ReceiveFromChild(child_id, list);
            ResourceProvider::ResourceIdSet resource_ids_to_receive;
            resource_ids_to_receive.insert(id1);
            resource_ids_to_receive.insert(id2);
            resource_provider_->DeclareUsedResourcesFromChild(child_id,
                resource_ids_to_receive);
        }

        EXPECT_EQ(2u, resource_provider_->num_resources());
        ResourceProvider::ResourceIdMap resource_map = resource_provider_->GetChildToParentMap(child_id);
        ResourceId mapped_id1 = resource_map[id1];
        ResourceId mapped_id2 = resource_map[id2];
        EXPECT_NE(0u, mapped_id1);
        EXPECT_NE(0u, mapped_id2);
        EXPECT_FALSE(resource_provider_->InUseByConsumer(id1));
        EXPECT_FALSE(resource_provider_->InUseByConsumer(id2));

        {
            // The parent transfers the resources to the grandparent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(mapped_id1);
            resource_ids_to_transfer.push_back(mapped_id2);
            TransferableResourceArray list;
            resource_provider_->PrepareSendToParent(resource_ids_to_transfer, &list);

            ASSERT_EQ(2u, list.size());
            if (GetParam() == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE) {
                EXPECT_NE(0u, list[0].mailbox_holder.sync_point);
                EXPECT_NE(0u, list[1].mailbox_holder.sync_point);
            }
            EXPECT_TRUE(resource_provider_->InUseByConsumer(id1));
            EXPECT_TRUE(resource_provider_->InUseByConsumer(id2));

            // Release the resource in the parent. Set no resources as being in use. The
            // resources are exported so that can't be transferred back yet.
            ResourceProvider::ResourceIdSet no_resources;
            resource_provider_->DeclareUsedResourcesFromChild(child_id, no_resources);

            // Destroy the child, the resources should not be returned yet.
            EXPECT_EQ(0u, returned_to_child.size());
            EXPECT_EQ(2u, resource_provider_->num_resources());

            resource_provider_->DestroyChild(child_id);

            EXPECT_EQ(2u, resource_provider_->num_resources());
            ASSERT_EQ(0u, returned_to_child.size());

            // Return a resource from the grandparent, it should be returned at this
            // point.
            EXPECT_EQ(2u, list.size());
            EXPECT_EQ(mapped_id1, list[0].id);
            EXPECT_EQ(mapped_id2, list[1].id);
            TransferableResourceArray return_list;
            return_list.push_back(list[1]);
            list.pop_back();
            ReturnedResourceArray returned;
            TransferableResource::ReturnResources(return_list, &returned);
            resource_provider_->ReceiveReturnsFromParent(returned);

            EXPECT_EQ(1u, resource_provider_->num_resources());
            ASSERT_EQ(1u, returned_to_child.size());
            if (GetParam() == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE) {
                EXPECT_NE(0u, returned_to_child[0].sync_point);
            }
            EXPECT_FALSE(returned_to_child[0].lost);
            returned_to_child.clear();

            // Destroy the parent resource provider. The resource that's left should be
            // lost at this point, and returned.
            resource_provider_ = nullptr;
            ASSERT_EQ(1u, returned_to_child.size());
            if (GetParam() == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE) {
                EXPECT_NE(0u, returned_to_child[0].sync_point);
            }
            EXPECT_TRUE(returned_to_child[0].lost);
        }
    }

    TEST_P(ResourceProviderTest, DeleteTransferredResources)
    {
        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        size_t pixel_size = TextureSizeBytes(size, format);
        ASSERT_EQ(4U, pixel_size);

        ResourceId id = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data[4] = { 1, 2, 3, 4 };
        child_resource_provider_->CopyToResource(id, data, size);

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));
        {
            // Transfer some resource to the parent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            ASSERT_EQ(1u, list.size());
            if (GetParam() == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
                EXPECT_NE(0u, list[0].mailbox_holder.sync_point);
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id));
            resource_provider_->ReceiveFromChild(child_id, list);
            ResourceProvider::ResourceIdSet resource_ids_to_receive;
            resource_ids_to_receive.insert(id);
            resource_provider_->DeclareUsedResourcesFromChild(child_id,
                resource_ids_to_receive);
        }

        // Delete textures in the child, while they are transfered.
        child_resource_provider_->DeleteResource(id);
        EXPECT_EQ(1u, child_resource_provider_->num_resources());
        {
            EXPECT_EQ(0u, returned_to_child.size());

            // Transfer resources back from the parent to the child. Set no resources as
            // being in use.
            ResourceProvider::ResourceIdSet no_resources;
            resource_provider_->DeclareUsedResourcesFromChild(child_id, no_resources);

            ASSERT_EQ(1u, returned_to_child.size());
            if (GetParam() == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
                EXPECT_NE(0u, returned_to_child[0].sync_point);
            child_resource_provider_->ReceiveReturnsFromParent(returned_to_child);
        }
        EXPECT_EQ(0u, child_resource_provider_->num_resources());
    }

    TEST_P(ResourceProviderTest, UnuseTransferredResources)
    {
        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        size_t pixel_size = TextureSizeBytes(size, format);
        ASSERT_EQ(4U, pixel_size);

        ResourceId id = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        uint8_t data[4] = { 1, 2, 3, 4 };
        child_resource_provider_->CopyToResource(id, data, size);

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));
        const ResourceProvider::ResourceIdMap& map = resource_provider_->GetChildToParentMap(child_id);
        {
            // Transfer some resource to the parent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id));
            resource_provider_->ReceiveFromChild(child_id, list);
            ResourceProvider::ResourceIdSet resource_ids_to_receive;
            resource_ids_to_receive.insert(id);
            resource_provider_->DeclareUsedResourcesFromChild(child_id,
                resource_ids_to_receive);
        }
        TransferableResourceArray sent_to_top_level;
        {
            // Parent transfers to top-level.
            ASSERT_TRUE(map.find(id) != map.end());
            ResourceId parent_id = map.find(id)->second;
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(parent_id);
            resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &sent_to_top_level);
            EXPECT_TRUE(resource_provider_->InUseByConsumer(parent_id));
        }
        {
            // Stop using resource.
            ResourceProvider::ResourceIdSet empty;
            resource_provider_->DeclareUsedResourcesFromChild(child_id, empty);
            // Resource is not yet returned to the child, since it's in use by the
            // top-level.
            EXPECT_TRUE(returned_to_child.empty());
        }
        {
            // Send the resource to the parent again.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(id);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            EXPECT_TRUE(child_resource_provider_->InUseByConsumer(id));
            resource_provider_->ReceiveFromChild(child_id, list);
            ResourceProvider::ResourceIdSet resource_ids_to_receive;
            resource_ids_to_receive.insert(id);
            resource_provider_->DeclareUsedResourcesFromChild(child_id,
                resource_ids_to_receive);
        }
        {
            // Receive returns back from top-level.
            ReturnedResourceArray returned;
            TransferableResource::ReturnResources(sent_to_top_level, &returned);
            resource_provider_->ReceiveReturnsFromParent(returned);
            // Resource is still not yet returned to the child, since it's declared used
            // in the parent.
            EXPECT_TRUE(returned_to_child.empty());
            ASSERT_TRUE(map.find(id) != map.end());
            ResourceId parent_id = map.find(id)->second;
            EXPECT_FALSE(resource_provider_->InUseByConsumer(parent_id));
        }
        {
            sent_to_top_level.clear();
            // Parent transfers again to top-level.
            ASSERT_TRUE(map.find(id) != map.end());
            ResourceId parent_id = map.find(id)->second;
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(parent_id);
            resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &sent_to_top_level);
            EXPECT_TRUE(resource_provider_->InUseByConsumer(parent_id));
        }
        {
            // Receive returns back from top-level.
            ReturnedResourceArray returned;
            TransferableResource::ReturnResources(sent_to_top_level, &returned);
            resource_provider_->ReceiveReturnsFromParent(returned);
            // Resource is still not yet returned to the child, since it's still
            // declared used in the parent.
            EXPECT_TRUE(returned_to_child.empty());
            ASSERT_TRUE(map.find(id) != map.end());
            ResourceId parent_id = map.find(id)->second;
            EXPECT_FALSE(resource_provider_->InUseByConsumer(parent_id));
        }
        {
            // Stop using resource.
            ResourceProvider::ResourceIdSet empty;
            resource_provider_->DeclareUsedResourcesFromChild(child_id, empty);
            // Resource should have been returned to the child, since it's no longer in
            // use by the top-level.
            ASSERT_EQ(1u, returned_to_child.size());
            EXPECT_EQ(id, returned_to_child[0].id);
            EXPECT_EQ(2, returned_to_child[0].count);
            child_resource_provider_->ReceiveReturnsFromParent(returned_to_child);
            returned_to_child.clear();
            EXPECT_FALSE(child_resource_provider_->InUseByConsumer(id));
        }
    }

    class ResourceProviderTestTextureFilters : public ResourceProviderTest {
    public:
        static void RunTest(GLenum child_filter, GLenum parent_filter)
        {
            scoped_ptr<TextureStateTrackingContext> child_context_owned(
                new TextureStateTrackingContext);
            TextureStateTrackingContext* child_context = child_context_owned.get();

            FakeOutputSurfaceClient child_output_surface_client;
            scoped_ptr<OutputSurface> child_output_surface(
                FakeOutputSurface::Create3d(child_context_owned.Pass()));
            CHECK(child_output_surface->BindToClient(&child_output_surface_client));
            scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
                new TestSharedBitmapManager());

            scoped_ptr<ResourceProvider> child_resource_provider(
                ResourceProvider::Create(child_output_surface.get(),
                    shared_bitmap_manager.get(), NULL, NULL, 0, 1,
                    use_image_texture_targets_));

            scoped_ptr<TextureStateTrackingContext> parent_context_owned(
                new TextureStateTrackingContext);
            TextureStateTrackingContext* parent_context = parent_context_owned.get();

            FakeOutputSurfaceClient parent_output_surface_client;
            scoped_ptr<OutputSurface> parent_output_surface(
                FakeOutputSurface::Create3d(parent_context_owned.Pass()));
            CHECK(parent_output_surface->BindToClient(&parent_output_surface_client));

            scoped_ptr<ResourceProvider> parent_resource_provider(
                ResourceProvider::Create(parent_output_surface.get(),
                    shared_bitmap_manager.get(), NULL, NULL, 0, 1,
                    use_image_texture_targets_));

            gfx::Size size(1, 1);
            ResourceFormat format = RGBA_8888;
            int child_texture_id = 1;
            int parent_texture_id = 2;

            size_t pixel_size = TextureSizeBytes(size, format);
            ASSERT_EQ(4U, pixel_size);

            ResourceId id = child_resource_provider->CreateResource(
                size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE,
                format);

            // The new texture is created with GL_LINEAR.
            EXPECT_CALL(*child_context, bindTexture(GL_TEXTURE_2D, child_texture_id))
                .Times(2); // Once to create and once to allocate.
            EXPECT_CALL(*child_context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            EXPECT_CALL(*child_context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            EXPECT_CALL(
                *child_context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            EXPECT_CALL(
                *child_context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            EXPECT_CALL(*child_context,
                texParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_POOL_CHROMIUM,
                    GL_TEXTURE_POOL_UNMANAGED_CHROMIUM));
            child_resource_provider->AllocateForTesting(id);
            Mock::VerifyAndClearExpectations(child_context);

            uint8_t data[4] = { 1, 2, 3, 4 };

            EXPECT_CALL(*child_context, bindTexture(GL_TEXTURE_2D, child_texture_id));
            child_resource_provider->CopyToResource(id, data, size);
            Mock::VerifyAndClearExpectations(child_context);

            // The texture is set to |child_filter| in the child.
            EXPECT_CALL(*child_context, bindTexture(GL_TEXTURE_2D, child_texture_id));
            if (child_filter != GL_LINEAR) {
                EXPECT_CALL(
                    *child_context,
                    texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, child_filter));
                EXPECT_CALL(
                    *child_context,
                    texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, child_filter));
            }
            SetResourceFilter(child_resource_provider.get(), id, child_filter);
            Mock::VerifyAndClearExpectations(child_context);

            ReturnedResourceArray returned_to_child;
            int child_id = parent_resource_provider->CreateChild(
                GetReturnCallback(&returned_to_child));
            {
                // Transfer some resource to the parent.
                ResourceProvider::ResourceIdArray resource_ids_to_transfer;
                resource_ids_to_transfer.push_back(id);
                TransferableResourceArray list;

                EXPECT_CALL(*child_context,
                    produceTextureDirectCHROMIUM(_, GL_TEXTURE_2D, _));
                EXPECT_CALL(*child_context, insertSyncPoint());
                child_resource_provider->PrepareSendToParent(resource_ids_to_transfer,
                    &list);
                Mock::VerifyAndClearExpectations(child_context);

                ASSERT_EQ(1u, list.size());
                EXPECT_EQ(static_cast<unsigned>(child_filter), list[0].filter);

                EXPECT_CALL(*parent_context,
                    createAndConsumeTextureCHROMIUM(GL_TEXTURE_2D, _))
                    .WillOnce(Return(parent_texture_id));

                parent_resource_provider->ReceiveFromChild(child_id, list);
                {
                    parent_resource_provider->WaitSyncPointIfNeeded(list[0].id);
                    ResourceProvider::ScopedReadLockGL lock(parent_resource_provider.get(),
                        list[0].id);
                }
                Mock::VerifyAndClearExpectations(parent_context);

                ResourceProvider::ResourceIdSet resource_ids_to_receive;
                resource_ids_to_receive.insert(id);
                parent_resource_provider->DeclareUsedResourcesFromChild(
                    child_id, resource_ids_to_receive);
                Mock::VerifyAndClearExpectations(parent_context);
            }
            ResourceProvider::ResourceIdMap resource_map = parent_resource_provider->GetChildToParentMap(child_id);
            ResourceId mapped_id = resource_map[id];
            EXPECT_NE(0u, mapped_id);

            // The texture is set to |parent_filter| in the parent.
            EXPECT_CALL(*parent_context, bindTexture(GL_TEXTURE_2D, parent_texture_id));
            EXPECT_CALL(
                *parent_context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, parent_filter));
            EXPECT_CALL(
                *parent_context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, parent_filter));
            SetResourceFilter(parent_resource_provider.get(), mapped_id, parent_filter);
            Mock::VerifyAndClearExpectations(parent_context);

            // The texture should be reset to |child_filter| in the parent when it is
            // returned, since that is how it was received.
            EXPECT_CALL(*parent_context, bindTexture(GL_TEXTURE_2D, parent_texture_id));
            EXPECT_CALL(
                *parent_context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, child_filter));
            EXPECT_CALL(
                *parent_context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, child_filter));

            {
                EXPECT_EQ(0u, returned_to_child.size());

                // Transfer resources back from the parent to the child. Set no resources
                // as being in use.
                ResourceProvider::ResourceIdSet no_resources;
                EXPECT_CALL(*parent_context, insertSyncPoint());
                parent_resource_provider->DeclareUsedResourcesFromChild(child_id,
                    no_resources);
                Mock::VerifyAndClearExpectations(parent_context);

                ASSERT_EQ(1u, returned_to_child.size());
                child_resource_provider->ReceiveReturnsFromParent(returned_to_child);
            }

            // The child remembers the texture filter is set to |child_filter|.
            EXPECT_CALL(*child_context, bindTexture(GL_TEXTURE_2D, child_texture_id));
            SetResourceFilter(child_resource_provider.get(), id, child_filter);
            Mock::VerifyAndClearExpectations(child_context);
        }
    };

    TEST_P(ResourceProviderTest, TextureFilters_ChildNearestParentLinear)
    {
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;
        ResourceProviderTestTextureFilters::RunTest(GL_NEAREST, GL_LINEAR);
    }

    TEST_P(ResourceProviderTest, TextureFilters_ChildLinearParentNearest)
    {
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;
        ResourceProviderTestTextureFilters::RunTest(GL_LINEAR, GL_NEAREST);
    }

    TEST_P(ResourceProviderTest, TransferMailboxResources)
    {
        // Other mailbox transfers tested elsewhere.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;
        unsigned texture = context()->createTexture();
        context()->bindTexture(GL_TEXTURE_2D, texture);
        uint8_t data[4] = { 1, 2, 3, 4 };
        context()->texImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data);
        gpu::Mailbox mailbox;
        context()->genMailboxCHROMIUM(mailbox.name);
        context()->produceTextureDirectCHROMIUM(texture, GL_TEXTURE_2D, mailbox.name);
        uint32 sync_point = context()->insertSyncPoint();

        // All the logic below assumes that the sync points are all positive.
        EXPECT_LT(0u, sync_point);

        uint32 release_sync_point = 0;
        bool lost_resource = false;
        BlockingTaskRunner* main_thread_task_runner = NULL;
        ReleaseCallbackImpl callback = base::Bind(ReleaseCallback,
            &release_sync_point,
            &lost_resource,
            &main_thread_task_runner);
        ResourceId resource = resource_provider_->CreateResourceFromTextureMailbox(
            TextureMailbox(mailbox, GL_TEXTURE_2D, sync_point),
            SingleReleaseCallbackImpl::Create(callback));
        EXPECT_EQ(1u, context()->NumTextures());
        EXPECT_EQ(0u, release_sync_point);
        {
            // Transfer the resource, expect the sync points to be consistent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(resource);
            TransferableResourceArray list;
            resource_provider_->PrepareSendToParent(resource_ids_to_transfer, &list);
            ASSERT_EQ(1u, list.size());
            EXPECT_LE(sync_point, list[0].mailbox_holder.sync_point);
            EXPECT_EQ(0,
                memcmp(mailbox.name,
                    list[0].mailbox_holder.mailbox.name,
                    sizeof(mailbox.name)));
            EXPECT_EQ(0u, release_sync_point);

            context()->waitSyncPoint(list[0].mailbox_holder.sync_point);
            unsigned other_texture = context()->createAndConsumeTextureCHROMIUM(GL_TEXTURE_2D, mailbox.name);
            uint8_t test_data[4] = { 0 };
            context()->GetPixels(
                gfx::Size(1, 1), RGBA_8888, test_data);
            EXPECT_EQ(0, memcmp(data, test_data, sizeof(data)));

            context()->produceTextureDirectCHROMIUM(other_texture, GL_TEXTURE_2D,
                mailbox.name);
            context()->deleteTexture(other_texture);
            list[0].mailbox_holder.sync_point = context()->insertSyncPoint();
            EXPECT_LT(0u, list[0].mailbox_holder.sync_point);

            // Receive the resource, then delete it, expect the sync points to be
            // consistent.
            ReturnedResourceArray returned;
            TransferableResource::ReturnResources(list, &returned);
            resource_provider_->ReceiveReturnsFromParent(returned);
            EXPECT_EQ(1u, context()->NumTextures());
            EXPECT_EQ(0u, release_sync_point);

            resource_provider_->DeleteResource(resource);
            EXPECT_LE(list[0].mailbox_holder.sync_point, release_sync_point);
            EXPECT_FALSE(lost_resource);
            EXPECT_EQ(main_thread_task_runner_.get(), main_thread_task_runner);
        }

        // We're going to do the same thing as above, but testing the case where we
        // delete the resource before we receive it back.
        sync_point = release_sync_point;
        EXPECT_LT(0u, sync_point);
        release_sync_point = 0;
        resource = resource_provider_->CreateResourceFromTextureMailbox(
            TextureMailbox(mailbox, GL_TEXTURE_2D, sync_point),
            SingleReleaseCallbackImpl::Create(callback));
        EXPECT_EQ(1u, context()->NumTextures());
        EXPECT_EQ(0u, release_sync_point);
        {
            // Transfer the resource, expect the sync points to be consistent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(resource);
            TransferableResourceArray list;
            resource_provider_->PrepareSendToParent(resource_ids_to_transfer, &list);
            ASSERT_EQ(1u, list.size());
            EXPECT_LE(sync_point, list[0].mailbox_holder.sync_point);
            EXPECT_EQ(0,
                memcmp(mailbox.name,
                    list[0].mailbox_holder.mailbox.name,
                    sizeof(mailbox.name)));
            EXPECT_EQ(0u, release_sync_point);

            context()->waitSyncPoint(list[0].mailbox_holder.sync_point);
            unsigned other_texture = context()->createAndConsumeTextureCHROMIUM(GL_TEXTURE_2D, mailbox.name);
            uint8_t test_data[4] = { 0 };
            context()->GetPixels(
                gfx::Size(1, 1), RGBA_8888, test_data);
            EXPECT_EQ(0, memcmp(data, test_data, sizeof(data)));

            context()->produceTextureDirectCHROMIUM(other_texture, GL_TEXTURE_2D,
                mailbox.name);
            context()->deleteTexture(other_texture);
            list[0].mailbox_holder.sync_point = context()->insertSyncPoint();
            EXPECT_LT(0u, list[0].mailbox_holder.sync_point);

            // Delete the resource, which shouldn't do anything.
            resource_provider_->DeleteResource(resource);
            EXPECT_EQ(1u, context()->NumTextures());
            EXPECT_EQ(0u, release_sync_point);

            // Then receive the resource which should release the mailbox, expect the
            // sync points to be consistent.
            ReturnedResourceArray returned;
            TransferableResource::ReturnResources(list, &returned);
            resource_provider_->ReceiveReturnsFromParent(returned);
            EXPECT_LE(list[0].mailbox_holder.sync_point, release_sync_point);
            EXPECT_FALSE(lost_resource);
            EXPECT_EQ(main_thread_task_runner_.get(), main_thread_task_runner);
        }

        context()->waitSyncPoint(release_sync_point);
        texture = context()->createAndConsumeTextureCHROMIUM(GL_TEXTURE_2D, mailbox.name);
        context()->deleteTexture(texture);
    }

    TEST_P(ResourceProviderTest, LostResourceInParent)
    {
        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        ResourceId resource = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        child_resource_provider_->AllocateForTesting(resource);
        // Expect a GL resource to be lost.
        bool should_lose_resource = GetParam() == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE;

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));
        {
            // Transfer the resource to the parent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(resource);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            EXPECT_EQ(1u, list.size());

            resource_provider_->ReceiveFromChild(child_id, list);
            ResourceProvider::ResourceIdSet resource_ids_to_receive;
            resource_ids_to_receive.insert(resource);
            resource_provider_->DeclareUsedResourcesFromChild(child_id,
                resource_ids_to_receive);
        }

        // Lose the output surface in the parent.
        resource_provider_->DidLoseOutputSurface();

        {
            EXPECT_EQ(0u, returned_to_child.size());

            // Transfer resources back from the parent to the child. Set no resources as
            // being in use.
            ResourceProvider::ResourceIdSet no_resources;
            resource_provider_->DeclareUsedResourcesFromChild(child_id, no_resources);

            // Expect a GL resource to be lost.
            ASSERT_EQ(1u, returned_to_child.size());
            EXPECT_EQ(should_lose_resource, returned_to_child[0].lost);
            child_resource_provider_->ReceiveReturnsFromParent(returned_to_child);
            returned_to_child.clear();
        }

        // A GL resource should be lost.
        EXPECT_EQ(should_lose_resource, child_resource_provider_->IsLost(resource));

        // Lost resources stay in use in the parent forever.
        EXPECT_EQ(should_lose_resource,
            child_resource_provider_->InUseByConsumer(resource));
    }

    TEST_P(ResourceProviderTest, LostResourceInGrandParent)
    {
        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        ResourceId resource = child_resource_provider_->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        child_resource_provider_->AllocateForTesting(resource);

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));
        {
            // Transfer the resource to the parent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(resource);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            EXPECT_EQ(1u, list.size());

            resource_provider_->ReceiveFromChild(child_id, list);
            ResourceProvider::ResourceIdSet resource_ids_to_receive;
            resource_ids_to_receive.insert(resource);
            resource_provider_->DeclareUsedResourcesFromChild(child_id,
                resource_ids_to_receive);
        }

        {
            ResourceProvider::ResourceIdMap resource_map = resource_provider_->GetChildToParentMap(child_id);
            ResourceId parent_resource = resource_map[resource];
            EXPECT_NE(0u, parent_resource);

            // Transfer to a grandparent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(parent_resource);
            TransferableResourceArray list;
            resource_provider_->PrepareSendToParent(resource_ids_to_transfer, &list);

            // Receive back a lost resource from the grandparent.
            EXPECT_EQ(1u, list.size());
            EXPECT_EQ(parent_resource, list[0].id);
            ReturnedResourceArray returned;
            TransferableResource::ReturnResources(list, &returned);
            EXPECT_EQ(1u, returned.size());
            EXPECT_EQ(parent_resource, returned[0].id);
            returned[0].lost = true;
            resource_provider_->ReceiveReturnsFromParent(returned);

            // The resource should be lost.
            EXPECT_TRUE(resource_provider_->IsLost(parent_resource));

            // Lost resources stay in use in the parent forever.
            EXPECT_TRUE(resource_provider_->InUseByConsumer(parent_resource));
        }

        {
            EXPECT_EQ(0u, returned_to_child.size());

            // Transfer resources back from the parent to the child. Set no resources as
            // being in use.
            ResourceProvider::ResourceIdSet no_resources;
            resource_provider_->DeclareUsedResourcesFromChild(child_id, no_resources);

            // Expect the resource to be lost.
            ASSERT_EQ(1u, returned_to_child.size());
            EXPECT_TRUE(returned_to_child[0].lost);
            child_resource_provider_->ReceiveReturnsFromParent(returned_to_child);
            returned_to_child.clear();
        }

        // The resource should be lost.
        EXPECT_TRUE(child_resource_provider_->IsLost(resource));

        // Lost resources stay in use in the parent forever.
        EXPECT_TRUE(child_resource_provider_->InUseByConsumer(resource));
    }

    TEST_P(ResourceProviderTest, LostMailboxInParent)
    {
        uint32 release_sync_point = 0;
        bool lost_resource = false;
        bool release_called = false;
        uint32 sync_point = 0;
        ResourceId resource = CreateChildMailbox(&release_sync_point, &lost_resource,
            &release_called, &sync_point);

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));
        {
            // Transfer the resource to the parent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(resource);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            EXPECT_EQ(1u, list.size());

            resource_provider_->ReceiveFromChild(child_id, list);
            ResourceProvider::ResourceIdSet resource_ids_to_receive;
            resource_ids_to_receive.insert(resource);
            resource_provider_->DeclareUsedResourcesFromChild(child_id,
                resource_ids_to_receive);
        }

        // Lose the output surface in the parent.
        resource_provider_->DidLoseOutputSurface();

        {
            EXPECT_EQ(0u, returned_to_child.size());

            // Transfer resources back from the parent to the child. Set no resources as
            // being in use.
            ResourceProvider::ResourceIdSet no_resources;
            resource_provider_->DeclareUsedResourcesFromChild(child_id, no_resources);

            ASSERT_EQ(1u, returned_to_child.size());
            // Losing an output surface only loses hardware resources.
            EXPECT_EQ(returned_to_child[0].lost,
                GetParam() == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE);
            child_resource_provider_->ReceiveReturnsFromParent(returned_to_child);
            returned_to_child.clear();
        }

        // Delete the resource in the child. Expect the resource to be lost if it's
        // a GL texture.
        child_resource_provider_->DeleteResource(resource);
        EXPECT_EQ(lost_resource,
            GetParam() == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE);
    }

    TEST_P(ResourceProviderTest, LostMailboxInGrandParent)
    {
        uint32 release_sync_point = 0;
        bool lost_resource = false;
        bool release_called = false;
        uint32 sync_point = 0;
        ResourceId resource = CreateChildMailbox(&release_sync_point, &lost_resource,
            &release_called, &sync_point);

        ReturnedResourceArray returned_to_child;
        int child_id = resource_provider_->CreateChild(GetReturnCallback(&returned_to_child));
        {
            // Transfer the resource to the parent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(resource);
            TransferableResourceArray list;
            child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
                &list);
            EXPECT_EQ(1u, list.size());

            resource_provider_->ReceiveFromChild(child_id, list);
            ResourceProvider::ResourceIdSet resource_ids_to_receive;
            resource_ids_to_receive.insert(resource);
            resource_provider_->DeclareUsedResourcesFromChild(child_id,
                resource_ids_to_receive);
        }

        {
            ResourceProvider::ResourceIdMap resource_map = resource_provider_->GetChildToParentMap(child_id);
            ResourceId parent_resource = resource_map[resource];
            EXPECT_NE(0u, parent_resource);

            // Transfer to a grandparent.
            ResourceProvider::ResourceIdArray resource_ids_to_transfer;
            resource_ids_to_transfer.push_back(parent_resource);
            TransferableResourceArray list;
            resource_provider_->PrepareSendToParent(resource_ids_to_transfer, &list);

            // Receive back a lost resource from the grandparent.
            EXPECT_EQ(1u, list.size());
            EXPECT_EQ(parent_resource, list[0].id);
            ReturnedResourceArray returned;
            TransferableResource::ReturnResources(list, &returned);
            EXPECT_EQ(1u, returned.size());
            EXPECT_EQ(parent_resource, returned[0].id);
            returned[0].lost = true;
            resource_provider_->ReceiveReturnsFromParent(returned);
        }

        {
            EXPECT_EQ(0u, returned_to_child.size());

            // Transfer resources back from the parent to the child. Set no resources as
            // being in use.
            ResourceProvider::ResourceIdSet no_resources;
            resource_provider_->DeclareUsedResourcesFromChild(child_id, no_resources);

            // Expect the resource to be lost.
            ASSERT_EQ(1u, returned_to_child.size());
            EXPECT_TRUE(returned_to_child[0].lost);
            child_resource_provider_->ReceiveReturnsFromParent(returned_to_child);
            returned_to_child.clear();
        }

        // Delete the resource in the child. Expect the resource to be lost.
        child_resource_provider_->DeleteResource(resource);
        EXPECT_TRUE(lost_resource);
    }

    TEST_P(ResourceProviderTest, Shutdown)
    {
        uint32 release_sync_point = 0;
        bool lost_resource = false;
        bool release_called = false;
        uint32 sync_point = 0;
        CreateChildMailbox(
            &release_sync_point, &lost_resource, &release_called, &sync_point);

        EXPECT_EQ(0u, release_sync_point);
        EXPECT_FALSE(lost_resource);

        child_resource_provider_ = nullptr;

        if (GetParam() == ResourceProvider::RESOURCE_TYPE_GL_TEXTURE) {
            EXPECT_LE(sync_point, release_sync_point);
        }
        EXPECT_TRUE(release_called);
        EXPECT_FALSE(lost_resource);
    }

    TEST_P(ResourceProviderTest, ShutdownWithExportedResource)
    {
        uint32 release_sync_point = 0;
        bool lost_resource = false;
        bool release_called = false;
        uint32 sync_point = 0;
        ResourceId resource = CreateChildMailbox(&release_sync_point, &lost_resource,
            &release_called, &sync_point);

        // Transfer the resource, so we can't release it properly on shutdown.
        ResourceProvider::ResourceIdArray resource_ids_to_transfer;
        resource_ids_to_transfer.push_back(resource);
        TransferableResourceArray list;
        child_resource_provider_->PrepareSendToParent(resource_ids_to_transfer,
            &list);

        EXPECT_EQ(0u, release_sync_point);
        EXPECT_FALSE(lost_resource);

        child_resource_provider_ = nullptr;

        // Since the resource is in the parent, the child considers it lost.
        EXPECT_EQ(0u, release_sync_point);
        EXPECT_TRUE(lost_resource);
    }

    TEST_P(ResourceProviderTest, LostContext)
    {
        // TextureMailbox callbacks only exist for GL textures for now.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;
        unsigned texture = context()->createTexture();
        context()->bindTexture(GL_TEXTURE_2D, texture);
        gpu::Mailbox mailbox;
        context()->genMailboxCHROMIUM(mailbox.name);
        context()->produceTextureDirectCHROMIUM(texture, GL_TEXTURE_2D, mailbox.name);
        uint32 sync_point = context()->insertSyncPoint();

        EXPECT_LT(0u, sync_point);

        uint32 release_sync_point = 0;
        bool lost_resource = false;
        BlockingTaskRunner* main_thread_task_runner = NULL;
        scoped_ptr<SingleReleaseCallbackImpl> callback = SingleReleaseCallbackImpl::Create(base::Bind(ReleaseCallback,
            &release_sync_point,
            &lost_resource,
            &main_thread_task_runner));
        resource_provider_->CreateResourceFromTextureMailbox(
            TextureMailbox(mailbox, GL_TEXTURE_2D, sync_point), callback.Pass());

        EXPECT_EQ(0u, release_sync_point);
        EXPECT_FALSE(lost_resource);
        EXPECT_EQ(NULL, main_thread_task_runner);

        resource_provider_->DidLoseOutputSurface();
        resource_provider_ = nullptr;

        EXPECT_LE(sync_point, release_sync_point);
        EXPECT_TRUE(lost_resource);
        EXPECT_EQ(main_thread_task_runner_.get(), main_thread_task_runner);
    }

    TEST_P(ResourceProviderTest, ScopedSampler)
    {
        // Sampling is only supported for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;

        scoped_ptr<TextureStateTrackingContext> context_owned(
            new TextureStateTrackingContext);
        TextureStateTrackingContext* context = context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), NULL, 0, 1,
            use_image_texture_targets_));

        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        int texture_id = 1;

        ResourceId id = resource_provider->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);

        // Check that the texture gets created with the right sampler settings.
        EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, texture_id))
            .Times(2); // Once to create and once to allocate.
        EXPECT_CALL(*context,
            texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        EXPECT_CALL(*context,
            texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        EXPECT_CALL(
            *context,
            texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        EXPECT_CALL(
            *context,
            texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        EXPECT_CALL(*context,
            texParameteri(GL_TEXTURE_2D,
                GL_TEXTURE_POOL_CHROMIUM,
                GL_TEXTURE_POOL_UNMANAGED_CHROMIUM));

        resource_provider->AllocateForTesting(id);
        Mock::VerifyAndClearExpectations(context);

        // Creating a sampler with the default filter should not change any texture
        // parameters.
        {
            EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, texture_id));
            ResourceProvider::ScopedSamplerGL sampler(
                resource_provider.get(), id, GL_TEXTURE_2D, GL_LINEAR);
            Mock::VerifyAndClearExpectations(context);
        }

        // Using a different filter should be reflected in the texture parameters.
        {
            EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, texture_id));
            EXPECT_CALL(
                *context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            EXPECT_CALL(
                *context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            ResourceProvider::ScopedSamplerGL sampler(
                resource_provider.get(), id, GL_TEXTURE_2D, GL_NEAREST);
            Mock::VerifyAndClearExpectations(context);
        }

        // Test resetting to the default filter.
        {
            EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, texture_id));
            EXPECT_CALL(*context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            EXPECT_CALL(*context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            ResourceProvider::ScopedSamplerGL sampler(
                resource_provider.get(), id, GL_TEXTURE_2D, GL_LINEAR);
            Mock::VerifyAndClearExpectations(context);
        }
    }

    TEST_P(ResourceProviderTest, ManagedResource)
    {
        // Sampling is only supported for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;

        scoped_ptr<TextureStateTrackingContext> context_owned(
            new TextureStateTrackingContext);
        TextureStateTrackingContext* context = context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), NULL, 0, 1,
            use_image_texture_targets_));

        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        int texture_id = 1;

        // Check that the texture gets created with the right sampler settings.
        ResourceId id = resource_provider->CreateManagedResource(
            size, GL_TEXTURE_2D, GL_CLAMP_TO_EDGE,
            ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
        EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, texture_id));
        EXPECT_CALL(*context,
            texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        EXPECT_CALL(*context,
            texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        EXPECT_CALL(
            *context,
            texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        EXPECT_CALL(
            *context,
            texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        EXPECT_CALL(*context,
            texParameteri(GL_TEXTURE_2D,
                GL_TEXTURE_POOL_CHROMIUM,
                GL_TEXTURE_POOL_MANAGED_CHROMIUM));
        resource_provider->CreateForTesting(id);
        EXPECT_NE(0u, id);

        Mock::VerifyAndClearExpectations(context);
    }

    TEST_P(ResourceProviderTest, TextureWrapMode)
    {
        // Sampling is only supported for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;

        scoped_ptr<TextureStateTrackingContext> context_owned(
            new TextureStateTrackingContext);
        TextureStateTrackingContext* context = context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), NULL, 0, 1,
            use_image_texture_targets_));

        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        GLenum texture_pool = GL_TEXTURE_POOL_UNMANAGED_CHROMIUM;

        for (int texture_id = 1; texture_id <= 2; ++texture_id) {
            GLint wrap_mode = texture_id == 1 ? GL_CLAMP_TO_EDGE : GL_REPEAT;
            // Check that the texture gets created with the right sampler settings.
            ResourceId id = resource_provider->CreateGLTexture(
                size, GL_TEXTURE_2D, texture_pool, wrap_mode,
                ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);
            EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, texture_id));
            EXPECT_CALL(*context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            EXPECT_CALL(*context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            EXPECT_CALL(*context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode));
            EXPECT_CALL(*context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode));
            EXPECT_CALL(*context,
                texParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_POOL_CHROMIUM,
                    GL_TEXTURE_POOL_UNMANAGED_CHROMIUM));
            resource_provider->CreateForTesting(id);
            EXPECT_NE(0u, id);

            Mock::VerifyAndClearExpectations(context);
        }
    }

    TEST_P(ResourceProviderTest, TextureHint)
    {
        // Sampling is only supported for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;

        scoped_ptr<TextureStateTrackingContext> context_owned(
            new TextureStateTrackingContext);
        TextureStateTrackingContext* context = context_owned.get();
        context->set_support_texture_storage(true);
        context->set_support_texture_usage(true);

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), NULL, 0, 1,
            use_image_texture_targets_));

        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;
        GLenum texture_pool = GL_TEXTURE_POOL_UNMANAGED_CHROMIUM;

        const ResourceProvider::TextureHint hints[4] = {
            ResourceProvider::TEXTURE_HINT_DEFAULT,
            ResourceProvider::TEXTURE_HINT_IMMUTABLE,
            ResourceProvider::TEXTURE_HINT_FRAMEBUFFER,
            ResourceProvider::TEXTURE_HINT_IMMUTABLE_FRAMEBUFFER,
        };
        for (GLuint texture_id = 1; texture_id <= arraysize(hints); ++texture_id) {
            // Check that the texture gets created with the right sampler settings.
            ResourceId id = resource_provider->CreateGLTexture(
                size, GL_TEXTURE_2D, texture_pool, GL_CLAMP_TO_EDGE,
                hints[texture_id - 1], format);
            EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, texture_id));
            EXPECT_CALL(*context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            EXPECT_CALL(*context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            EXPECT_CALL(
                *context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            EXPECT_CALL(
                *context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            EXPECT_CALL(*context,
                texParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_POOL_CHROMIUM,
                    GL_TEXTURE_POOL_UNMANAGED_CHROMIUM));
            // Check only TEXTURE_HINT_FRAMEBUFFER set GL_TEXTURE_USAGE_ANGLE.
            bool is_framebuffer_hint = hints[texture_id - 1] & ResourceProvider::TEXTURE_HINT_FRAMEBUFFER;
            EXPECT_CALL(*context,
                texParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_USAGE_ANGLE,
                    GL_FRAMEBUFFER_ATTACHMENT_ANGLE))
                .Times(is_framebuffer_hint ? 1 : 0);
            resource_provider->CreateForTesting(id);
            EXPECT_NE(0u, id);

            Mock::VerifyAndClearExpectations(context);
        }
    }

    TEST_P(ResourceProviderTest, TextureMailbox_SharedMemory)
    {
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_BITMAP)
            return;

        gfx::Size size(64, 64);
        const uint32_t kBadBeef = 0xbadbeef;
        scoped_ptr<SharedBitmap> shared_bitmap(
            CreateAndFillSharedBitmap(shared_bitmap_manager_.get(), size, kBadBeef));

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::CreateSoftware(make_scoped_ptr(
                new SoftwareOutputDevice)));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), main_thread_task_runner_.get(), 0, 1,
            use_image_texture_targets_));

        uint32 release_sync_point = 0;
        bool lost_resource = false;
        BlockingTaskRunner* main_thread_task_runner = NULL;
        scoped_ptr<SingleReleaseCallbackImpl> callback = SingleReleaseCallbackImpl::Create(base::Bind(&ReleaseCallback,
            &release_sync_point,
            &lost_resource,
            &main_thread_task_runner));
        TextureMailbox mailbox(shared_bitmap.get(), size);

        ResourceId id = resource_provider->CreateResourceFromTextureMailbox(
            mailbox, callback.Pass());
        EXPECT_NE(0u, id);

        {
            ResourceProvider::ScopedReadLockSoftware lock(resource_provider.get(), id);
            const SkBitmap* sk_bitmap = lock.sk_bitmap();
            EXPECT_EQ(sk_bitmap->width(), size.width());
            EXPECT_EQ(sk_bitmap->height(), size.height());
            EXPECT_EQ(*sk_bitmap->getAddr32(16, 16), kBadBeef);
        }

        resource_provider->DeleteResource(id);
        EXPECT_EQ(0u, release_sync_point);
        EXPECT_FALSE(lost_resource);
        EXPECT_EQ(main_thread_task_runner_.get(), main_thread_task_runner);
    }

    class ResourceProviderTestTextureMailboxGLFilters
        : public ResourceProviderTest {
    public:
        static void RunTest(TestSharedBitmapManager* shared_bitmap_manager,
            TestGpuMemoryBufferManager* gpu_memory_buffer_manager,
            BlockingTaskRunner* main_thread_task_runner,
            bool mailbox_nearest_neighbor,
            GLenum sampler_filter)
        {
            scoped_ptr<TextureStateTrackingContext> context_owned(
                new TextureStateTrackingContext);
            TextureStateTrackingContext* context = context_owned.get();

            FakeOutputSurfaceClient output_surface_client;
            scoped_ptr<OutputSurface> output_surface(
                FakeOutputSurface::Create3d(context_owned.Pass()));
            CHECK(output_surface->BindToClient(&output_surface_client));

            scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
                output_surface.get(), shared_bitmap_manager, gpu_memory_buffer_manager,
                main_thread_task_runner, 0, 1, use_image_texture_targets_));

            unsigned texture_id = 1;
            uint32 sync_point = 30;
            unsigned target = GL_TEXTURE_2D;

            EXPECT_CALL(*context, bindTexture(_, _)).Times(0);
            EXPECT_CALL(*context, waitSyncPoint(_)).Times(0);
            EXPECT_CALL(*context, insertSyncPoint()).Times(0);
            EXPECT_CALL(*context, produceTextureDirectCHROMIUM(_, _, _)).Times(0);
            EXPECT_CALL(*context, createAndConsumeTextureCHROMIUM(_, _)).Times(0);

            gpu::Mailbox gpu_mailbox;
            memcpy(gpu_mailbox.name, "Hello world", strlen("Hello world") + 1);
            uint32 release_sync_point = 0;
            bool lost_resource = false;
            BlockingTaskRunner* mailbox_task_runner = NULL;
            scoped_ptr<SingleReleaseCallbackImpl> callback = SingleReleaseCallbackImpl::Create(base::Bind(&ReleaseCallback,
                &release_sync_point,
                &lost_resource,
                &mailbox_task_runner));

            TextureMailbox mailbox(gpu_mailbox, target, sync_point);
            mailbox.set_nearest_neighbor(mailbox_nearest_neighbor);

            ResourceId id = resource_provider->CreateResourceFromTextureMailbox(
                mailbox, callback.Pass());
            EXPECT_NE(0u, id);

            Mock::VerifyAndClearExpectations(context);

            {
                // Mailbox sync point WaitSyncPoint before using the texture.
                EXPECT_CALL(*context, waitSyncPoint(sync_point));
                resource_provider->WaitSyncPointIfNeeded(id);
                Mock::VerifyAndClearExpectations(context);

                EXPECT_CALL(*context, createAndConsumeTextureCHROMIUM(target, _))
                    .WillOnce(Return(texture_id));
                EXPECT_CALL(*context, bindTexture(target, texture_id));

                EXPECT_CALL(*context, insertSyncPoint()).Times(0);
                EXPECT_CALL(*context, produceTextureDirectCHROMIUM(_, _, _)).Times(0);

                // The sampler will reset these if |mailbox_nearest_neighbor| does not
                // match |sampler_filter|.
                if (mailbox_nearest_neighbor != (sampler_filter == GL_NEAREST)) {
                    EXPECT_CALL(*context, texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler_filter));
                    EXPECT_CALL(*context, texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler_filter));
                }

                ResourceProvider::ScopedSamplerGL lock(
                    resource_provider.get(), id, sampler_filter);
                Mock::VerifyAndClearExpectations(context);

                // When done with it, a sync point should be inserted, but no produce is
                // necessary.
                EXPECT_CALL(*context, bindTexture(_, _)).Times(0);
                EXPECT_CALL(*context, insertSyncPoint());
                EXPECT_CALL(*context, produceTextureDirectCHROMIUM(_, _, _)).Times(0);

                EXPECT_CALL(*context, waitSyncPoint(_)).Times(0);
                EXPECT_CALL(*context, createAndConsumeTextureCHROMIUM(_, _)).Times(0);
            }

            resource_provider->DeleteResource(id);
            EXPECT_EQ(0u, release_sync_point);
            EXPECT_FALSE(lost_resource);
            EXPECT_EQ(main_thread_task_runner, mailbox_task_runner);
        }
    };

    TEST_P(ResourceProviderTest, TextureMailbox_GLTexture2D_LinearToLinear)
    {
        // Mailboxing is only supported for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;

        ResourceProviderTestTextureMailboxGLFilters::RunTest(
            shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(),
            main_thread_task_runner_.get(),
            false,
            GL_LINEAR);
    }

    TEST_P(ResourceProviderTest, TextureMailbox_GLTexture2D_NearestToNearest)
    {
        // Mailboxing is only supported for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;

        ResourceProviderTestTextureMailboxGLFilters::RunTest(
            shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(),
            main_thread_task_runner_.get(),
            true,
            GL_NEAREST);
    }

    TEST_P(ResourceProviderTest, TextureMailbox_GLTexture2D_NearestToLinear)
    {
        // Mailboxing is only supported for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;

        ResourceProviderTestTextureMailboxGLFilters::RunTest(
            shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(),
            main_thread_task_runner_.get(),
            true,
            GL_LINEAR);
    }

    TEST_P(ResourceProviderTest, TextureMailbox_GLTexture2D_LinearToNearest)
    {
        // Mailboxing is only supported for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;

        ResourceProviderTestTextureMailboxGLFilters::RunTest(
            shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(),
            main_thread_task_runner_.get(),
            false,
            GL_NEAREST);
    }

    TEST_P(ResourceProviderTest, TextureMailbox_GLTextureExternalOES)
    {
        // Mailboxing is only supported for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;

        scoped_ptr<TextureStateTrackingContext> context_owned(
            new TextureStateTrackingContext);
        TextureStateTrackingContext* context = context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), NULL, 0, 1,
            use_image_texture_targets_));

        uint32 sync_point = 30;
        unsigned target = GL_TEXTURE_EXTERNAL_OES;

        EXPECT_CALL(*context, bindTexture(_, _)).Times(0);
        EXPECT_CALL(*context, waitSyncPoint(_)).Times(0);
        EXPECT_CALL(*context, insertSyncPoint()).Times(0);
        EXPECT_CALL(*context, produceTextureDirectCHROMIUM(_, _, _)).Times(0);
        EXPECT_CALL(*context, createAndConsumeTextureCHROMIUM(_, _)).Times(0);

        gpu::Mailbox gpu_mailbox;
        memcpy(gpu_mailbox.name, "Hello world", strlen("Hello world") + 1);
        scoped_ptr<SingleReleaseCallbackImpl> callback = SingleReleaseCallbackImpl::Create(base::Bind(&EmptyReleaseCallback));

        TextureMailbox mailbox(gpu_mailbox, target, sync_point);

        ResourceId id = resource_provider->CreateResourceFromTextureMailbox(
            mailbox, callback.Pass());
        EXPECT_NE(0u, id);

        Mock::VerifyAndClearExpectations(context);

        {
            // Mailbox sync point WaitSyncPoint before using the texture.
            EXPECT_CALL(*context, waitSyncPoint(sync_point));
            resource_provider->WaitSyncPointIfNeeded(id);
            Mock::VerifyAndClearExpectations(context);

            unsigned texture_id = 1;

            EXPECT_CALL(*context, createAndConsumeTextureCHROMIUM(target, _))
                .WillOnce(Return(texture_id));

            EXPECT_CALL(*context, insertSyncPoint()).Times(0);
            EXPECT_CALL(*context, produceTextureDirectCHROMIUM(_, _, _)).Times(0);

            ResourceProvider::ScopedReadLockGL lock(resource_provider.get(), id);
            Mock::VerifyAndClearExpectations(context);

            // When done with it, a sync point should be inserted, but no produce is
            // necessary.
            EXPECT_CALL(*context, bindTexture(_, _)).Times(0);
            EXPECT_CALL(*context, insertSyncPoint());
            EXPECT_CALL(*context, produceTextureDirectCHROMIUM(_, _, _)).Times(0);

            EXPECT_CALL(*context, waitSyncPoint(_)).Times(0);
            EXPECT_CALL(*context, createAndConsumeTextureCHROMIUM(_, _)).Times(0);
        }
    }

    TEST_P(ResourceProviderTest,
        TextureMailbox_WaitSyncPointIfNeeded_WithSyncPoint)
    {
        // Mailboxing is only supported for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;

        scoped_ptr<TextureStateTrackingContext> context_owned(
            new TextureStateTrackingContext);
        TextureStateTrackingContext* context = context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), NULL, 0, 1,
            use_image_texture_targets_));

        uint32 sync_point = 30;
        unsigned target = GL_TEXTURE_2D;

        EXPECT_CALL(*context, bindTexture(_, _)).Times(0);
        EXPECT_CALL(*context, waitSyncPoint(_)).Times(0);
        EXPECT_CALL(*context, insertSyncPoint()).Times(0);
        EXPECT_CALL(*context, produceTextureDirectCHROMIUM(_, _, _)).Times(0);
        EXPECT_CALL(*context, createAndConsumeTextureCHROMIUM(_, _)).Times(0);

        gpu::Mailbox gpu_mailbox;
        memcpy(gpu_mailbox.name, "Hello world", strlen("Hello world") + 1);
        scoped_ptr<SingleReleaseCallbackImpl> callback = SingleReleaseCallbackImpl::Create(base::Bind(&EmptyReleaseCallback));

        TextureMailbox mailbox(gpu_mailbox, target, sync_point);

        ResourceId id = resource_provider->CreateResourceFromTextureMailbox(
            mailbox, callback.Pass());
        EXPECT_NE(0u, id);

        Mock::VerifyAndClearExpectations(context);

        {
            // First call to WaitSyncPointIfNeeded should call waitSyncPoint.
            EXPECT_CALL(*context, waitSyncPoint(sync_point));
            resource_provider->WaitSyncPointIfNeeded(id);
            Mock::VerifyAndClearExpectations(context);

            // Subsequent calls to WaitSyncPointIfNeeded shouldn't call waitSyncPoint.
            EXPECT_CALL(*context, waitSyncPoint(_)).Times(0);
            resource_provider->WaitSyncPointIfNeeded(id);
            Mock::VerifyAndClearExpectations(context);
        }
    }

    TEST_P(ResourceProviderTest, TextureMailbox_WaitSyncPointIfNeeded_NoSyncPoint)
    {
        // Mailboxing is only supported for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;

        scoped_ptr<TextureStateTrackingContext> context_owned(
            new TextureStateTrackingContext);
        TextureStateTrackingContext* context = context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), NULL, 0, 1,
            use_image_texture_targets_));

        uint32 sync_point = 0;
        unsigned target = GL_TEXTURE_2D;

        EXPECT_CALL(*context, bindTexture(_, _)).Times(0);
        EXPECT_CALL(*context, waitSyncPoint(_)).Times(0);
        EXPECT_CALL(*context, insertSyncPoint()).Times(0);
        EXPECT_CALL(*context, produceTextureDirectCHROMIUM(_, _, _)).Times(0);
        EXPECT_CALL(*context, createAndConsumeTextureCHROMIUM(_, _)).Times(0);

        gpu::Mailbox gpu_mailbox;
        memcpy(gpu_mailbox.name, "Hello world", strlen("Hello world") + 1);
        scoped_ptr<SingleReleaseCallbackImpl> callback = SingleReleaseCallbackImpl::Create(base::Bind(&EmptyReleaseCallback));

        TextureMailbox mailbox(gpu_mailbox, target, sync_point);

        ResourceId id = resource_provider->CreateResourceFromTextureMailbox(
            mailbox, callback.Pass());
        EXPECT_NE(0u, id);

        Mock::VerifyAndClearExpectations(context);

        {
            // WaitSyncPointIfNeeded with sync_point == 0 shouldn't call waitSyncPoint.
            EXPECT_CALL(*context, waitSyncPoint(_)).Times(0);
            resource_provider->WaitSyncPointIfNeeded(id);
            Mock::VerifyAndClearExpectations(context);
        }
    }

    class AllocationTrackingContext3D : public TestWebGraphicsContext3D {
    public:
        MOCK_METHOD0(NextTextureId, GLuint());
        MOCK_METHOD1(RetireTextureId, void(GLuint id));
        MOCK_METHOD2(bindTexture, void(GLenum target, GLuint texture));
        MOCK_METHOD5(texStorage2DEXT,
            void(GLenum target,
                GLint levels,
                GLuint internalformat,
                GLint width,
                GLint height));
        MOCK_METHOD9(texImage2D,
            void(GLenum target,
                GLint level,
                GLenum internalformat,
                GLsizei width,
                GLsizei height,
                GLint border,
                GLenum format,
                GLenum type,
                const void* pixels));
        MOCK_METHOD9(texSubImage2D,
            void(GLenum target,
                GLint level,
                GLint xoffset,
                GLint yoffset,
                GLsizei width,
                GLsizei height,
                GLenum format,
                GLenum type,
                const void* pixels));
        MOCK_METHOD9(asyncTexImage2DCHROMIUM,
            void(GLenum target,
                GLint level,
                GLenum internalformat,
                GLsizei width,
                GLsizei height,
                GLint border,
                GLenum format,
                GLenum type,
                const void* pixels));
        MOCK_METHOD9(asyncTexSubImage2DCHROMIUM,
            void(GLenum target,
                GLint level,
                GLint xoffset,
                GLint yoffset,
                GLsizei width,
                GLsizei height,
                GLenum format,
                GLenum type,
                const void* pixels));
        MOCK_METHOD8(compressedTexImage2D,
            void(GLenum target,
                GLint level,
                GLenum internalformat,
                GLsizei width,
                GLsizei height,
                GLint border,
                GLsizei image_size,
                const void* data));
        MOCK_METHOD1(waitAsyncTexImage2DCHROMIUM, void(GLenum));
        MOCK_METHOD4(createImageCHROMIUM,
            GLuint(ClientBuffer, GLsizei, GLsizei, GLenum));
        MOCK_METHOD1(destroyImageCHROMIUM, void(GLuint));
        MOCK_METHOD2(bindTexImage2DCHROMIUM, void(GLenum, GLint));
        MOCK_METHOD2(releaseTexImage2DCHROMIUM, void(GLenum, GLint));

        // We're mocking bindTexture, so we override
        // TestWebGraphicsContext3D::texParameteri to avoid assertions related to the
        // currently bound texture.
        virtual void texParameteri(GLenum target, GLenum pname, GLint param) { }
    };

    TEST_P(ResourceProviderTest, TextureAllocation)
    {
        // Only for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;
        scoped_ptr<AllocationTrackingContext3D> context_owned(
            new StrictMock<AllocationTrackingContext3D>);
        AllocationTrackingContext3D* context = context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), NULL, 0, 1,
            use_image_texture_targets_));

        gfx::Size size(2, 2);
        gfx::Vector2d offset(0, 0);
        ResourceFormat format = RGBA_8888;
        ResourceId id = 0;
        uint8_t pixels[16] = { 0 };
        int texture_id = 123;

        // Lazy allocation. Don't allocate when creating the resource.
        id = resource_provider->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);

        EXPECT_CALL(*context, NextTextureId()).WillOnce(Return(texture_id));
        EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, texture_id)).Times(1);
        resource_provider->CreateForTesting(id);

        EXPECT_CALL(*context, RetireTextureId(texture_id)).Times(1);
        resource_provider->DeleteResource(id);

        Mock::VerifyAndClearExpectations(context);

        // Do allocate when we set the pixels.
        id = resource_provider->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);

        EXPECT_CALL(*context, NextTextureId()).WillOnce(Return(texture_id));
        EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, texture_id)).Times(3);
        EXPECT_CALL(*context, texImage2D(_, _, _, 2, 2, _, _, _, _)).Times(1);
        EXPECT_CALL(*context, texSubImage2D(_, _, _, _, 2, 2, _, _, _)).Times(1);
        resource_provider->CopyToResource(id, pixels, size);

        EXPECT_CALL(*context, RetireTextureId(texture_id)).Times(1);
        resource_provider->DeleteResource(id);

        Mock::VerifyAndClearExpectations(context);
    }

    TEST_P(ResourceProviderTest, TextureAllocationHint)
    {
        // Only for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;
        scoped_ptr<AllocationTrackingContext3D> context_owned(
            new StrictMock<AllocationTrackingContext3D>);
        AllocationTrackingContext3D* context = context_owned.get();
        context->set_support_texture_storage(true);
        context->set_support_texture_usage(true);

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), NULL, 0, 1,
            use_image_texture_targets_));

        gfx::Size size(2, 2);

        const ResourceFormat formats[2] = { RGBA_8888, BGRA_8888 };
        const ResourceProvider::TextureHint hints[4] = {
            ResourceProvider::TEXTURE_HINT_DEFAULT,
            ResourceProvider::TEXTURE_HINT_IMMUTABLE,
            ResourceProvider::TEXTURE_HINT_FRAMEBUFFER,
            ResourceProvider::TEXTURE_HINT_IMMUTABLE_FRAMEBUFFER,
        };
        for (size_t i = 0; i < arraysize(formats); ++i) {
            for (GLuint texture_id = 1; texture_id <= arraysize(hints); ++texture_id) {
                // Lazy allocation. Don't allocate when creating the resource.
                ResourceId id = resource_provider->CreateResource(
                    size, GL_CLAMP_TO_EDGE, hints[texture_id - 1], formats[i]);

                EXPECT_CALL(*context, NextTextureId()).WillOnce(Return(texture_id));
                EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, texture_id)).Times(2);
                bool is_immutable_hint = hints[texture_id - 1] & ResourceProvider::TEXTURE_HINT_IMMUTABLE;
                bool support_immutable_texture = is_immutable_hint && formats[i] == RGBA_8888;
                EXPECT_CALL(*context, texStorage2DEXT(_, _, _, 2, 2))
                    .Times(support_immutable_texture ? 1 : 0);
                EXPECT_CALL(*context, texImage2D(_, _, _, 2, 2, _, _, _, _))
                    .Times(support_immutable_texture ? 0 : 1);
                resource_provider->AllocateForTesting(id);

                EXPECT_CALL(*context, RetireTextureId(texture_id)).Times(1);
                resource_provider->DeleteResource(id);

                Mock::VerifyAndClearExpectations(context);
            }
        }
    }

    TEST_P(ResourceProviderTest, TextureAllocationHint_BGRA)
    {
        // Only for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;
        scoped_ptr<AllocationTrackingContext3D> context_owned(
            new StrictMock<AllocationTrackingContext3D>);
        AllocationTrackingContext3D* context = context_owned.get();
        context->set_support_texture_format_bgra8888(true);
        context->set_support_texture_storage(true);
        context->set_support_texture_usage(true);

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), NULL, 0, 1,
            use_image_texture_targets_));

        gfx::Size size(2, 2);
        const ResourceFormat formats[2] = { RGBA_8888, BGRA_8888 };

        const ResourceProvider::TextureHint hints[4] = {
            ResourceProvider::TEXTURE_HINT_DEFAULT,
            ResourceProvider::TEXTURE_HINT_IMMUTABLE,
            ResourceProvider::TEXTURE_HINT_FRAMEBUFFER,
            ResourceProvider::TEXTURE_HINT_IMMUTABLE_FRAMEBUFFER,
        };
        for (size_t i = 0; i < arraysize(formats); ++i) {
            for (GLuint texture_id = 1; texture_id <= arraysize(hints); ++texture_id) {
                // Lazy allocation. Don't allocate when creating the resource.
                ResourceId id = resource_provider->CreateResource(
                    size, GL_CLAMP_TO_EDGE, hints[texture_id - 1], formats[i]);

                EXPECT_CALL(*context, NextTextureId()).WillOnce(Return(texture_id));
                EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, texture_id)).Times(2);
                bool is_immutable_hint = hints[texture_id - 1] & ResourceProvider::TEXTURE_HINT_IMMUTABLE;
                EXPECT_CALL(*context, texStorage2DEXT(_, _, _, 2, 2))
                    .Times(is_immutable_hint ? 1 : 0);
                EXPECT_CALL(*context, texImage2D(_, _, _, 2, 2, _, _, _, _))
                    .Times(is_immutable_hint ? 0 : 1);
                resource_provider->AllocateForTesting(id);

                EXPECT_CALL(*context, RetireTextureId(texture_id)).Times(1);
                resource_provider->DeleteResource(id);

                Mock::VerifyAndClearExpectations(context);
            }
        }
    }

    TEST_P(ResourceProviderTest, Image_GLTexture)
    {
        // Only for GL textures.
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;
        scoped_ptr<AllocationTrackingContext3D> context_owned(
            new StrictMock<AllocationTrackingContext3D>);
        AllocationTrackingContext3D* context = context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        const int kWidth = 2;
        const int kHeight = 2;
        gfx::Size size(kWidth, kHeight);
        ResourceFormat format = RGBA_8888;
        ResourceId id = 0;
        const unsigned kTextureId = 123u;
        const unsigned kImageId = 234u;

        scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), NULL, 0, 1,
            use_image_texture_targets_));

        id = resource_provider->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, format);

        EXPECT_CALL(*context, NextTextureId())
            .WillOnce(Return(kTextureId))
            .RetiresOnSaturation();
        EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, kTextureId))
            .Times(1)
            .RetiresOnSaturation();
        EXPECT_CALL(*context, createImageCHROMIUM(_, kWidth, kHeight, GL_RGBA))
            .WillOnce(Return(kImageId))
            .RetiresOnSaturation();
        {
            ResourceProvider::ScopedWriteLockGpuMemoryBuffer lock(
                resource_provider.get(), id);
            EXPECT_TRUE(lock.GetGpuMemoryBuffer());
        }

        EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, kTextureId))
            .Times(1)
            .RetiresOnSaturation();
        EXPECT_CALL(*context, bindTexImage2DCHROMIUM(GL_TEXTURE_2D, kImageId))
            .Times(1)
            .RetiresOnSaturation();
        {
            ResourceProvider::ScopedSamplerGL lock_gl(
                resource_provider.get(), id, GL_TEXTURE_2D, GL_LINEAR);
            EXPECT_EQ(kTextureId, lock_gl.texture_id());
        }

        {
            ResourceProvider::ScopedWriteLockGpuMemoryBuffer lock(
                resource_provider.get(), id);
            EXPECT_TRUE(lock.GetGpuMemoryBuffer());
        }

        EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, kTextureId)).Times(1).RetiresOnSaturation();
        EXPECT_CALL(*context, releaseTexImage2DCHROMIUM(GL_TEXTURE_2D, kImageId))
            .Times(1)
            .RetiresOnSaturation();
        EXPECT_CALL(*context, bindTexImage2DCHROMIUM(GL_TEXTURE_2D, kImageId))
            .Times(1)
            .RetiresOnSaturation();
        EXPECT_CALL(*context, RetireTextureId(kTextureId))
            .Times(1)
            .RetiresOnSaturation();
        {
            ResourceProvider::ScopedSamplerGL lock_gl(
                resource_provider.get(), id, GL_TEXTURE_2D, GL_LINEAR);
            EXPECT_EQ(kTextureId, lock_gl.texture_id());
        }

        EXPECT_CALL(*context, destroyImageCHROMIUM(kImageId))
            .Times(1)
            .RetiresOnSaturation();
    }

    TEST_P(ResourceProviderTest, CompressedTextureETC1Allocate)
    {
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;

        scoped_ptr<AllocationTrackingContext3D> context_owned(
            new AllocationTrackingContext3D);
        AllocationTrackingContext3D* context = context_owned.get();
        context_owned->set_support_compressed_texture_etc1(true);

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        gfx::Size size(4, 4);
        scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), NULL, 0, 1,
            use_image_texture_targets_));
        int texture_id = 123;

        ResourceId id = resource_provider->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, ETC1);
        EXPECT_NE(0u, id);
        EXPECT_CALL(*context, NextTextureId()).WillOnce(Return(texture_id));
        EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, texture_id)).Times(2);
        resource_provider->AllocateForTesting(id);

        EXPECT_CALL(*context, RetireTextureId(texture_id)).Times(1);
        resource_provider->DeleteResource(id);
    }

    TEST_P(ResourceProviderTest, CompressedTextureETC1Upload)
    {
        if (GetParam() != ResourceProvider::RESOURCE_TYPE_GL_TEXTURE)
            return;

        scoped_ptr<AllocationTrackingContext3D> context_owned(
            new AllocationTrackingContext3D);
        AllocationTrackingContext3D* context = context_owned.get();
        context_owned->set_support_compressed_texture_etc1(true);

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        gfx::Size size(4, 4);
        scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager_.get(),
            gpu_memory_buffer_manager_.get(), NULL, 0, 1,
            use_image_texture_targets_));
        int texture_id = 123;
        uint8_t pixels[8];

        ResourceId id = resource_provider->CreateResource(
            size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE, ETC1);
        EXPECT_NE(0u, id);
        EXPECT_CALL(*context, NextTextureId()).WillOnce(Return(texture_id));
        EXPECT_CALL(*context, bindTexture(GL_TEXTURE_2D, texture_id)).Times(3);
        EXPECT_CALL(*context,
            compressedTexImage2D(
                _, 0, _, size.width(), size.height(), _, _, _))
            .Times(1);
        resource_provider->CopyToResource(id, pixels, size);

        EXPECT_CALL(*context, RetireTextureId(texture_id)).Times(1);
        resource_provider->DeleteResource(id);
    }

    INSTANTIATE_TEST_CASE_P(
        ResourceProviderTests,
        ResourceProviderTest,
        ::testing::Values(ResourceProvider::RESOURCE_TYPE_GL_TEXTURE,
            ResourceProvider::RESOURCE_TYPE_BITMAP));

    class TextureIdAllocationTrackingContext : public TestWebGraphicsContext3D {
    public:
        GLuint NextTextureId() override
        {
            base::AutoLock lock(namespace_->lock);
            return namespace_->next_texture_id++;
        }
        void RetireTextureId(GLuint) override { }
        GLuint PeekTextureId()
        {
            base::AutoLock lock(namespace_->lock);
            return namespace_->next_texture_id;
        }
    };

    TEST(ResourceProviderTest, TextureAllocationChunkSize)
    {
        scoped_ptr<TextureIdAllocationTrackingContext> context_owned(
            new TextureIdAllocationTrackingContext);
        TextureIdAllocationTrackingContext* context = context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));
        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());

        gfx::Size size(1, 1);
        ResourceFormat format = RGBA_8888;

        {
            size_t kTextureAllocationChunkSize = 1;
            scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
                output_surface.get(), shared_bitmap_manager.get(), NULL, NULL, 0,
                kTextureAllocationChunkSize,
                ResourceProviderTest::use_image_texture_targets()));

            ResourceId id = resource_provider->CreateResource(
                size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE,
                format);
            resource_provider->AllocateForTesting(id);
            Mock::VerifyAndClearExpectations(context);

            DCHECK_EQ(2u, context->PeekTextureId());
            resource_provider->DeleteResource(id);
        }

        {
            size_t kTextureAllocationChunkSize = 8;
            scoped_ptr<ResourceProvider> resource_provider(ResourceProvider::Create(
                output_surface.get(), shared_bitmap_manager.get(), NULL, NULL, 0,
                kTextureAllocationChunkSize,
                ResourceProviderTest::use_image_texture_targets()));

            ResourceId id = resource_provider->CreateResource(
                size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE,
                format);
            resource_provider->AllocateForTesting(id);
            Mock::VerifyAndClearExpectations(context);

            DCHECK_EQ(10u, context->PeekTextureId());
            resource_provider->DeleteResource(id);
        }
    }

} // namespace
} // namespace cc
