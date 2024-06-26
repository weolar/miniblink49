// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RESOURCES_VIDEO_RESOURCE_UPDATER_H_
#define CC_RESOURCES_VIDEO_RESOURCE_UPDATER_H_

#include <list>
#include <vector>

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "cc/base/cc_export.h"
#include "cc/resources/release_callback_impl.h"
#include "cc/resources/resource_format.h"
#include "cc/resources/texture_mailbox.h"
#include "ui/gfx/geometry/size.h"

namespace media {
class SkCanvasVideoRenderer;
class VideoFrame;
}

namespace cc {
class ContextProvider;
class ResourceProvider;

class CC_EXPORT VideoFrameExternalResources {
public:
    // Specifies what type of data is contained in the mailboxes, as well as how
    // many mailboxes will be present.
    enum ResourceType {
        NONE,
        YUV_RESOURCE,
        RGB_RESOURCE,
        RGBA_RESOURCE,
        STREAM_TEXTURE_RESOURCE,
        IO_SURFACE,

#if defined(VIDEO_HOLE)
        // TODO(danakj): Implement this with a solid color layer instead of a video
        // frame and video layer.
        HOLE,
#endif // defined(VIDEO_HOLE)

        // TODO(danakj): Remove this and abstract TextureMailbox into
        // "ExternalResource" that can hold a hardware or software backing.
        SOFTWARE_RESOURCE
    };

    ResourceType type;
    std::vector<TextureMailbox> mailboxes;
    std::vector<ReleaseCallbackImpl> release_callbacks;
    bool read_lock_fences_enabled;

    // TODO(danakj): Remove these too.
    std::vector<unsigned> software_resources;
    ReleaseCallbackImpl software_release_callback;

    VideoFrameExternalResources();
    ~VideoFrameExternalResources();
};

// VideoResourceUpdater is used by the video system to produce frame content as
// resources consumable by the compositor.
class CC_EXPORT VideoResourceUpdater
    : public base::SupportsWeakPtr<VideoResourceUpdater> {
public:
    VideoResourceUpdater(ContextProvider* context_provider,
        ResourceProvider* resource_provider);
    ~VideoResourceUpdater();

    VideoFrameExternalResources CreateExternalResourcesFromVideoFrame(
        const scoped_refptr<media::VideoFrame>& video_frame);

private:
    struct PlaneResource {
        unsigned resource_id;
        gfx::Size resource_size;
        ResourceFormat resource_format;
        gpu::Mailbox mailbox;
        // The balance between the number of times this resource has been returned
        // from CreateForSoftwarePlanes vs released in RecycleResource.
        int ref_count;
        // These last three members will be used for identifying the data stored in
        // this resource, and uniquely identifies a media::VideoFrame plane. The
        // frame pointer will only be used for pointer comparison, i.e. the
        // underlying data will not be accessed.
        const void* frame_ptr;
        size_t plane_index;
        base::TimeDelta timestamp;

        PlaneResource(unsigned resource_id,
            const gfx::Size& resource_size,
            ResourceFormat resource_format,
            gpu::Mailbox mailbox);
    };

    static bool PlaneResourceMatchesUniqueID(const PlaneResource& plane_resource,
        const media::VideoFrame* video_frame,
        size_t plane_index);

    static void SetPlaneResourceUniqueId(const media::VideoFrame* video_frame,
        size_t plane_index,
        PlaneResource* plane_resource);

    // This needs to be a container where iterators can be erased without
    // invalidating other iterators.
    typedef std::list<PlaneResource> ResourceList;
    ResourceList::iterator AllocateResource(const gfx::Size& plane_size,
        ResourceFormat format,
        bool has_mailbox);
    void DeleteResource(ResourceList::iterator resource_it);
    bool VerifyFrame(const scoped_refptr<media::VideoFrame>& video_frame);
    VideoFrameExternalResources CreateForHardwarePlanes(
        const scoped_refptr<media::VideoFrame>& video_frame);
    VideoFrameExternalResources CreateForSoftwarePlanes(
        const scoped_refptr<media::VideoFrame>& video_frame);

    static void RecycleResource(base::WeakPtr<VideoResourceUpdater> updater,
        unsigned resource_id,
        const gpu::SyncToken& sync_token,
        bool lost_resource,
        BlockingTaskRunner* main_thread_task_runner);
    static void ReturnTexture(base::WeakPtr<VideoResourceUpdater> updater,
        const scoped_refptr<media::VideoFrame>& video_frame,
        const gpu::SyncToken& sync_token,
        bool lost_resource,
        BlockingTaskRunner* main_thread_task_runner);

    ContextProvider* context_provider_;
    ResourceProvider* resource_provider_;
    scoped_ptr<media::SkCanvasVideoRenderer> video_renderer_;
    std::vector<uint8_t> upload_pixels_;

    // Recycle resources so that we can reduce the number of allocations and
    // data transfers.
    ResourceList all_resources_;

    DISALLOW_COPY_AND_ASSIGN(VideoResourceUpdater);
};

} // namespace cc

#endif // CC_RESOURCES_VIDEO_RESOURCE_UPDATER_H_
