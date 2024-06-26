// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/resources/video_resource_updater.h"

#include <algorithm>

#include "base/bind.h"
#include "base/trace_event/trace_event.h"
#include "cc/base/math_util.h"
#include "cc/output/gl_renderer.h"
#include "cc/resources/resource_provider.h"
#include "gpu/GLES2/gl2extchromium.h"
#include "gpu/command_buffer/client/gles2_interface.h"
#include "media/base/video_frame.h"
#include "media/renderers/skcanvas_video_renderer.h"
#include "third_party/khronos/GLES2/gl2.h"
#include "third_party/khronos/GLES2/gl2ext.h"
#include "ui/gfx/geometry/size_conversions.h"

namespace cc {

namespace {

    const ResourceFormat kRGBResourceFormat = RGBA_8888;

    VideoFrameExternalResources::ResourceType ResourceTypeForVideoFrame(
        media::VideoFrame* video_frame)
    {
        switch (video_frame->format()) {
        case media::PIXEL_FORMAT_ARGB:
        case media::PIXEL_FORMAT_XRGB:
        case media::PIXEL_FORMAT_UYVY:
            switch (video_frame->mailbox_holder(0).texture_target) {
            case GL_TEXTURE_2D:
                return (video_frame->format() == media::PIXEL_FORMAT_XRGB)
                    ? VideoFrameExternalResources::RGB_RESOURCE
                    : VideoFrameExternalResources::RGBA_RESOURCE;
            case GL_TEXTURE_EXTERNAL_OES:
                return VideoFrameExternalResources::STREAM_TEXTURE_RESOURCE;
            case GL_TEXTURE_RECTANGLE_ARB:
                return VideoFrameExternalResources::IO_SURFACE;
            default:
                NOTREACHED();
                break;
            }
            break;
        case media::PIXEL_FORMAT_I420:
            return VideoFrameExternalResources::YUV_RESOURCE;
            break;
        case media::PIXEL_FORMAT_NV12:
            DCHECK_EQ(static_cast<uint32_t>(GL_TEXTURE_RECTANGLE_ARB),
                video_frame->mailbox_holder(0).texture_target);
            return VideoFrameExternalResources::IO_SURFACE;
            break;
        case media::PIXEL_FORMAT_YV12:
        case media::PIXEL_FORMAT_YV16:
        case media::PIXEL_FORMAT_YV24:
        case media::PIXEL_FORMAT_YV12A:
        case media::PIXEL_FORMAT_NV21:
        case media::PIXEL_FORMAT_YUY2:
        case media::PIXEL_FORMAT_RGB24:
        case media::PIXEL_FORMAT_RGB32:
        case media::PIXEL_FORMAT_MJPEG:
        case media::PIXEL_FORMAT_MT21:
        case media::PIXEL_FORMAT_UNKNOWN:
            break;
        }
        return VideoFrameExternalResources::NONE;
    }

    class SyncTokenClientImpl : public media::VideoFrame::SyncTokenClient {
    public:
        SyncTokenClientImpl(gpu::gles2::GLES2Interface* gl,
            const gpu::SyncToken& sync_token)
            : gl_(gl)
            , sync_token_(sync_token)
        {
        }
        ~SyncTokenClientImpl() override { }
        void GenerateSyncToken(gpu::SyncToken* sync_token) override
        {
            if (sync_token_.HasData()) {
                *sync_token = sync_token_;
            } else {
                const uint64_t fence_sync = gl_->InsertFenceSyncCHROMIUM();
                gl_->ShallowFlushCHROMIUM();
                gl_->GenSyncTokenCHROMIUM(fence_sync, sync_token->GetData());
            }
        }
        void WaitSyncToken(const gpu::SyncToken& sync_token) override
        {
            if (sync_token.HasData()) {
                gl_->WaitSyncTokenCHROMIUM(sync_token.GetConstData());
                if (sync_token_.HasData()) {
                    gl_->WaitSyncTokenCHROMIUM(sync_token_.GetConstData());
                    sync_token_.Clear();
                }
            }
        }

    private:
        gpu::gles2::GLES2Interface* gl_;
        gpu::SyncToken sync_token_;
    };

} // namespace

VideoResourceUpdater::PlaneResource::PlaneResource(
    unsigned int resource_id,
    const gfx::Size& resource_size,
    ResourceFormat resource_format,
    gpu::Mailbox mailbox)
    : resource_id(resource_id)
    , resource_size(resource_size)
    , resource_format(resource_format)
    , mailbox(mailbox)
    , ref_count(0)
    , frame_ptr(nullptr)
    , plane_index(0u)
{
}

bool VideoResourceUpdater::PlaneResourceMatchesUniqueID(
    const PlaneResource& plane_resource,
    const media::VideoFrame* video_frame,
    size_t plane_index)
{
    return plane_resource.frame_ptr == video_frame && plane_resource.plane_index == plane_index && plane_resource.timestamp == video_frame->timestamp();
}

void VideoResourceUpdater::SetPlaneResourceUniqueId(
    const media::VideoFrame* video_frame,
    size_t plane_index,
    PlaneResource* plane_resource)
{
    plane_resource->frame_ptr = video_frame;
    plane_resource->plane_index = plane_index;
    plane_resource->timestamp = video_frame->timestamp();
}

VideoFrameExternalResources::VideoFrameExternalResources()
    : type(NONE)
    , read_lock_fences_enabled(false)
{
}

VideoFrameExternalResources::~VideoFrameExternalResources() { }

VideoResourceUpdater::VideoResourceUpdater(ContextProvider* context_provider,
    ResourceProvider* resource_provider)
    : context_provider_(context_provider)
    , resource_provider_(resource_provider)
{
}

VideoResourceUpdater::~VideoResourceUpdater()
{
    for (const PlaneResource& plane_resource : all_resources_)
        resource_provider_->DeleteResource(plane_resource.resource_id);
}

VideoResourceUpdater::ResourceList::iterator
VideoResourceUpdater::AllocateResource(const gfx::Size& plane_size,
    ResourceFormat format,
    bool has_mailbox)
{
    // TODO(danakj): Abstract out hw/sw resource create/delete from
    // ResourceProvider and stop using ResourceProvider in this class.
    const ResourceId resource_id = resource_provider_->CreateResource(
        plane_size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE,
        format);
    if (resource_id == 0)
        return all_resources_.end();

    gpu::Mailbox mailbox;
    if (has_mailbox) {
        DCHECK(context_provider_);

        gpu::gles2::GLES2Interface* gl = context_provider_->ContextGL();

        gl->GenMailboxCHROMIUM(mailbox.name);
        ResourceProvider::ScopedWriteLockGL lock(resource_provider_, resource_id);
        gl->ProduceTextureDirectCHROMIUM(lock.texture_id(), GL_TEXTURE_2D,
            mailbox.name);
    }
    all_resources_.push_front(
        PlaneResource(resource_id, plane_size, format, mailbox));
    return all_resources_.begin();
}

void VideoResourceUpdater::DeleteResource(ResourceList::iterator resource_it)
{
    DCHECK_EQ(resource_it->ref_count, 0);
    resource_provider_->DeleteResource(resource_it->resource_id);
    all_resources_.erase(resource_it);
}

VideoFrameExternalResources VideoResourceUpdater::
    CreateExternalResourcesFromVideoFrame(
        const scoped_refptr<media::VideoFrame>& video_frame)
{
#if defined(VIDEO_HOLE)
    if (video_frame->storage_type() == media::VideoFrame::STORAGE_HOLE) {
        VideoFrameExternalResources external_resources;
        external_resources.type = VideoFrameExternalResources::HOLE;
        return external_resources;
    }
#endif // defined(VIDEO_HOLE)
    if (video_frame->format() == media::PIXEL_FORMAT_UNKNOWN)
        return VideoFrameExternalResources();
    DCHECK(video_frame->HasTextures() || video_frame->IsMappable());
    if (video_frame->HasTextures())
        return CreateForHardwarePlanes(video_frame);
    else
        return CreateForSoftwarePlanes(video_frame);
}

// For frames that we receive in software format, determine the dimensions of
// each plane in the frame.
static gfx::Size SoftwarePlaneDimension(
    const scoped_refptr<media::VideoFrame>& input_frame,
    bool software_compositor,
    size_t plane_index)
{
    if (!software_compositor) {
        return media::VideoFrame::PlaneSize(
            input_frame->format(), plane_index, input_frame->coded_size());
    }
    return input_frame->coded_size();
}

VideoFrameExternalResources VideoResourceUpdater::CreateForSoftwarePlanes(
    const scoped_refptr<media::VideoFrame>& video_frame)
{
    TRACE_EVENT0("cc", "VideoResourceUpdater::CreateForSoftwarePlanes");
    const media::VideoPixelFormat input_frame_format = video_frame->format();

    // Only YUV software video frames are supported.
    if (!media::IsYuvPlanar(input_frame_format)) {
        NOTREACHED() << media::VideoPixelFormatToString(input_frame_format);
        return VideoFrameExternalResources();
    }

    const bool software_compositor = context_provider_ == NULL;

    ResourceFormat output_resource_format = resource_provider_->yuv_resource_format();
    size_t output_plane_count = media::VideoFrame::NumPlanes(input_frame_format);

    // TODO(skaslev): If we're in software compositing mode, we do the YUV -> RGB
    // conversion here. That involves an extra copy of each frame to a bitmap.
    // Obviously, this is suboptimal and should be addressed once ubercompositor
    // starts shaping up.
    if (software_compositor) {
        output_resource_format = kRGBResourceFormat;
        output_plane_count = 1;
    }

    // Drop recycled resources that are the wrong format.
    for (auto it = all_resources_.begin(); it != all_resources_.end();) {
        if (it->ref_count == 0 && it->resource_format != output_resource_format)
            DeleteResource(it++);
        else
            ++it;
    }

    const int max_resource_size = resource_provider_->max_texture_size();
    std::vector<ResourceList::iterator> plane_resources;
    for (size_t i = 0; i < output_plane_count; ++i) {
        gfx::Size output_plane_resource_size = SoftwarePlaneDimension(video_frame, software_compositor, i);
        if (output_plane_resource_size.IsEmpty() || output_plane_resource_size.width() > max_resource_size || output_plane_resource_size.height() > max_resource_size) {
            break;
        }

        // Try recycle a previously-allocated resource.
        ResourceList::iterator resource_it = all_resources_.end();
        for (auto it = all_resources_.begin(); it != all_resources_.end(); ++it) {
            if (it->resource_size == output_plane_resource_size && it->resource_format == output_resource_format) {
                if (PlaneResourceMatchesUniqueID(*it, video_frame.get(), i)) {
                    // Bingo, we found a resource that already contains the data we are
                    // planning to put in it. It's safe to reuse it even if
                    // resource_provider_ holds some references to it, because those
                    // references are read-only.
                    resource_it = it;
                    break;
                }

                // This extra check is needed because resources backed by SharedMemory
                // are not ref-counted, unlike mailboxes. Full discussion in
                // codereview.chromium.org/145273021.
                const bool in_use = software_compositor && resource_provider_->InUseByConsumer(it->resource_id);
                if (it->ref_count == 0 && !in_use) {
                    // We found a resource with the correct size that we can overwrite.
                    resource_it = it;
                }
            }
        }

        // Check if we need to allocate a new resource.
        if (resource_it == all_resources_.end()) {
            resource_it = AllocateResource(output_plane_resource_size, output_resource_format,
                !software_compositor);
        }
        if (resource_it == all_resources_.end())
            break;

        ++resource_it->ref_count;
        plane_resources.push_back(resource_it);
    }

    if (plane_resources.size() != output_plane_count) {
        // Allocation failed, nothing will be returned so restore reference counts.
        for (ResourceList::iterator resource_it : plane_resources)
            --resource_it->ref_count;
        return VideoFrameExternalResources();
    }

    VideoFrameExternalResources external_resources;

    if (software_compositor) {
        DCHECK_EQ(plane_resources.size(), 1u);
        PlaneResource& plane_resource = *plane_resources[0];
        DCHECK_EQ(plane_resource.resource_format, kRGBResourceFormat);
        DCHECK(plane_resource.mailbox.IsZero());

        if (!PlaneResourceMatchesUniqueID(plane_resource, video_frame.get(), 0)) {
            // We need to transfer data from |video_frame| to the plane resource.
            if (!video_renderer_)
                video_renderer_.reset(new media::SkCanvasVideoRenderer);

            ResourceProvider::ScopedWriteLockSoftware lock(
                resource_provider_, plane_resource.resource_id);
            SkCanvas canvas(lock.sk_bitmap());
            // This is software path, so canvas and video_frame are always backed
            // by software.
            video_renderer_->Copy(video_frame, &canvas, media::Context3D());
            SetPlaneResourceUniqueId(video_frame.get(), 0, &plane_resource);
        }

        external_resources.software_resources.push_back(plane_resource.resource_id);
        external_resources.software_release_callback = base::Bind(&RecycleResource, AsWeakPtr(), plane_resource.resource_id);
        external_resources.type = VideoFrameExternalResources::SOFTWARE_RESOURCE;
        return external_resources;
    }

    for (size_t i = 0; i < plane_resources.size(); ++i) {
        PlaneResource& plane_resource = *plane_resources[i];
        // Update each plane's resource id with its content.
        DCHECK_EQ(plane_resource.resource_format,
            resource_provider_->yuv_resource_format());

        if (!PlaneResourceMatchesUniqueID(plane_resource, video_frame.get(), i)) {
            // We need to transfer data from |video_frame| to the plane resource.
            // TODO(reveman): Can use GpuMemoryBuffers here to improve performance.

            // The |resource_size_pixels| is the size of the resource we want to
            // upload to.
            gfx::Size resource_size_pixels = plane_resource.resource_size;
            // The |video_stride_pixels| is the width of the video frame we are
            // uploading (including non-frame data to fill in the stride).
            int video_stride_pixels = video_frame->stride(i);

            size_t bytes_per_row = ResourceUtil::UncheckedWidthInBytes<size_t>(
                resource_size_pixels.width(), plane_resource.resource_format);
            // Use 4-byte row alignment (OpenGL default) for upload performance.
            // Assuming that GL_UNPACK_ALIGNMENT has not changed from default.
            size_t upload_image_stride = MathUtil::UncheckedRoundUp<size_t>(bytes_per_row, 4u);

            const uint8_t* pixels;
            size_t video_bytes_per_row = ResourceUtil::UncheckedWidthInBytes<size_t>(
                video_stride_pixels, plane_resource.resource_format);
            if (upload_image_stride == video_bytes_per_row) {
                pixels = video_frame->data(i);
            } else {
                // Avoid malloc for each frame/plane if possible.
                size_t needed_size = upload_image_stride * resource_size_pixels.height();
                if (upload_pixels_.size() < needed_size)
                    upload_pixels_.resize(needed_size);
                for (int row = 0; row < resource_size_pixels.height(); ++row) {
                    uint8_t* dst = &upload_pixels_[upload_image_stride * row];
                    const uint8_t* src = video_frame->data(i) + (video_bytes_per_row * row);
                    memcpy(dst, src, bytes_per_row);
                }
                pixels = &upload_pixels_[0];
            }

            resource_provider_->CopyToResource(plane_resource.resource_id, pixels,
                resource_size_pixels);
            SetPlaneResourceUniqueId(video_frame.get(), i, &plane_resource);
        }

        external_resources.mailboxes.push_back(
            TextureMailbox(plane_resource.mailbox, GL_TEXTURE_2D, gpu::SyncToken()));
        external_resources.release_callbacks.push_back(
            base::Bind(&RecycleResource, AsWeakPtr(), plane_resource.resource_id));
    }

    external_resources.type = VideoFrameExternalResources::YUV_RESOURCE;
    return external_resources;
}

// static
void VideoResourceUpdater::ReturnTexture(
    base::WeakPtr<VideoResourceUpdater> updater,
    const scoped_refptr<media::VideoFrame>& video_frame,
    const gpu::SyncToken& sync_token,
    bool lost_resource,
    BlockingTaskRunner* main_thread_task_runner)
{
    // TODO(dshwang) this case should be forwarded to the decoder as lost
    // resource.
    if (lost_resource || !updater.get())
        return;
    // Update the release sync point in |video_frame| with |sync_point|
    // returned by the compositor and emit a WaitSyncPointCHROMIUM on
    // |video_frame|'s previous sync point using the current GL context.
    SyncTokenClientImpl client(updater->context_provider_->ContextGL(), sync_token);
    video_frame->UpdateReleaseSyncToken(&client);
}

VideoFrameExternalResources VideoResourceUpdater::CreateForHardwarePlanes(
    const scoped_refptr<media::VideoFrame>& video_frame)
{
    TRACE_EVENT0("cc", "VideoResourceUpdater::CreateForHardwarePlanes");
    DCHECK(video_frame->HasTextures());
    if (!context_provider_)
        return VideoFrameExternalResources();

    VideoFrameExternalResources external_resources;
    external_resources.read_lock_fences_enabled = true;

    external_resources.type = ResourceTypeForVideoFrame(video_frame.get());
    if (external_resources.type == VideoFrameExternalResources::NONE) {
        DLOG(ERROR) << "Unsupported Texture format"
                    << media::VideoPixelFormatToString(video_frame->format());
        return external_resources;
    }

    const size_t num_planes = media::VideoFrame::NumPlanes(video_frame->format());
    for (size_t i = 0; i < num_planes; ++i) {
        const gpu::MailboxHolder& mailbox_holder = video_frame->mailbox_holder(i);
        if (mailbox_holder.mailbox.IsZero())
            break;
        external_resources.mailboxes.push_back(
            TextureMailbox(mailbox_holder.mailbox, mailbox_holder.texture_target,
                mailbox_holder.sync_token, video_frame->coded_size(),
                video_frame->metadata()->IsTrue(
                    media::VideoFrameMetadata::ALLOW_OVERLAY)));
        external_resources.release_callbacks.push_back(
            base::Bind(&ReturnTexture, AsWeakPtr(), video_frame));
    }
    return external_resources;
}

// static
void VideoResourceUpdater::RecycleResource(
    base::WeakPtr<VideoResourceUpdater> updater,
    ResourceId resource_id,
    const gpu::SyncToken& sync_token,
    bool lost_resource,
    BlockingTaskRunner* main_thread_task_runner)
{
    if (!updater.get()) {
        // Resource was already deleted.
        return;
    }

    const ResourceList::iterator resource_it = std::find_if(
        updater->all_resources_.begin(), updater->all_resources_.end(),
        [resource_id](const PlaneResource& plane_resource) {
            return plane_resource.resource_id == resource_id;
        });
    if (resource_it == updater->all_resources_.end())
        return;

    ContextProvider* context_provider = updater->context_provider_;
    if (context_provider && sync_token.HasData()) {
        context_provider->ContextGL()->WaitSyncTokenCHROMIUM(sync_token.GetConstData());
    }

    if (lost_resource) {
        resource_it->ref_count = 0;
        updater->DeleteResource(resource_it);
        return;
    }

    --resource_it->ref_count;
    DCHECK_GE(resource_it->ref_count, 0);
}

} // namespace cc
