// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/video_frame_pool.h"

#include <list>

#include "base/bind.h"
#include "base/memory/ref_counted.h"
#include "base/synchronization/lock.h"

namespace media {

class VideoFramePool::PoolImpl
    : public base::RefCountedThreadSafe<VideoFramePool::PoolImpl> {
public:
    PoolImpl();

    // See VideoFramePool::CreateFrame() for usage.
    scoped_refptr<VideoFrame> CreateFrame(VideoPixelFormat format,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        base::TimeDelta timestamp);

    // Shuts down the frame pool and releases all frames in |frames_|.
    // Once this is called frames will no longer be inserted back into
    // |frames_|.
    void Shutdown();

    size_t GetPoolSizeForTesting() const { return frames_.size(); }

private:
    friend class base::RefCountedThreadSafe<VideoFramePool::PoolImpl>;
    ~PoolImpl();

    // Called when the frame wrapper gets destroyed.
    // |frame| is the actual frame that was wrapped and is placed
    // in |frames_| by this function so it can be reused.
    void FrameReleased(const scoped_refptr<VideoFrame>& frame);

    base::Lock lock_;
    bool is_shutdown_;
    std::list<scoped_refptr<VideoFrame>> frames_;

    DISALLOW_COPY_AND_ASSIGN(PoolImpl);
};

VideoFramePool::PoolImpl::PoolImpl()
    : is_shutdown_(false)
{
}

VideoFramePool::PoolImpl::~PoolImpl()
{
    DCHECK(is_shutdown_);
}

scoped_refptr<VideoFrame> VideoFramePool::PoolImpl::CreateFrame(
    VideoPixelFormat format,
    const gfx::Size& coded_size,
    const gfx::Rect& visible_rect,
    const gfx::Size& natural_size,
    base::TimeDelta timestamp)
{
    base::AutoLock auto_lock(lock_);
    DCHECK(!is_shutdown_);

    scoped_refptr<VideoFrame> frame;
    while (!frame.get() && !frames_.empty()) {
        scoped_refptr<VideoFrame> pool_frame = frames_.front();
        frames_.pop_front();

        if (pool_frame->format() == format && pool_frame->coded_size() == coded_size && pool_frame->visible_rect() == visible_rect && pool_frame->natural_size() == natural_size) {
            frame = pool_frame;
            frame->set_timestamp(timestamp);
            frame->metadata()->Clear();
            break;
        }
    }

    if (!frame.get()) {
        frame = VideoFrame::CreateZeroInitializedFrame(
            format, coded_size, visible_rect, natural_size, timestamp);
        LOG_IF(ERROR, !frame.get()) << "Failed to create a video frame";
    }

    scoped_refptr<VideoFrame> wrapped_frame = VideoFrame::WrapVideoFrame(
        frame, frame->visible_rect(), frame->natural_size());
    wrapped_frame->AddDestructionObserver(
        base::Bind(&VideoFramePool::PoolImpl::FrameReleased, this, frame));
    return wrapped_frame;
}

void VideoFramePool::PoolImpl::Shutdown()
{
    base::AutoLock auto_lock(lock_);
    is_shutdown_ = true;
    frames_.clear();
}

void VideoFramePool::PoolImpl::FrameReleased(
    const scoped_refptr<VideoFrame>& frame)
{
    base::AutoLock auto_lock(lock_);
    if (is_shutdown_)
        return;

    frames_.push_back(frame);
}

VideoFramePool::VideoFramePool()
    : pool_(new PoolImpl())
{
}

VideoFramePool::~VideoFramePool()
{
    pool_->Shutdown();
}

scoped_refptr<VideoFrame> VideoFramePool::CreateFrame(
    VideoPixelFormat format,
    const gfx::Size& coded_size,
    const gfx::Rect& visible_rect,
    const gfx::Size& natural_size,
    base::TimeDelta timestamp)
{
    return pool_->CreateFrame(format, coded_size, visible_rect, natural_size,
        timestamp);
}

size_t VideoFramePool::GetPoolSizeForTesting() const
{
    return pool_->GetPoolSizeForTesting();
}

} // namespace media
