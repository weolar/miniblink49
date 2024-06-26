// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_VIDEO_FAKE_VIDEO_ENCODE_ACCELERATOR_H_
#define MEDIA_VIDEO_FAKE_VIDEO_ENCODE_ACCELERATOR_H_

#include <list>
#include <queue>
#include <vector>

#include "base/memory/weak_ptr.h"
#include "media/base/bitstream_buffer.h"
#include "media/base/media_export.h"
#include "media/video/video_encode_accelerator.h"

namespace base {

class SingleThreadTaskRunner;

} // namespace base

namespace media {

class MEDIA_EXPORT FakeVideoEncodeAccelerator : public VideoEncodeAccelerator {
public:
    explicit FakeVideoEncodeAccelerator(
        const scoped_refptr<base::SingleThreadTaskRunner>& task_runner);
    ~FakeVideoEncodeAccelerator() override;

    VideoEncodeAccelerator::SupportedProfiles GetSupportedProfiles() override;
    bool Initialize(VideoPixelFormat input_format,
        const gfx::Size& input_visible_size,
        VideoCodecProfile output_profile,
        uint32 initial_bitrate,
        Client* client) override;
    void Encode(const scoped_refptr<VideoFrame>& frame,
        bool force_keyframe) override;
    void UseOutputBitstreamBuffer(const BitstreamBuffer& buffer) override;
    void RequestEncodingParametersChange(uint32 bitrate,
        uint32 framerate) override;
    void Destroy() override;

    const std::vector<uint32>& stored_bitrates() const
    {
        return stored_bitrates_;
    }
    void SendDummyFrameForTesting(bool key_frame);
    void SetWillInitializationSucceed(bool will_initialization_succeed);

private:
    void DoRequireBitstreamBuffers(unsigned int input_count,
        const gfx::Size& input_coded_size,
        size_t output_buffer_size) const;
    void EncodeTask();
    void DoBitstreamBufferReady(int32 bitstream_buffer_id,
        size_t payload_size,
        bool key_frame) const;

    // Our original (constructor) calling message loop used for all tasks.
    const scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
    std::vector<uint32> stored_bitrates_;
    bool will_initialization_succeed_;

    VideoEncodeAccelerator::Client* client_;

    // Keeps track of if the current frame is the first encoded frame. This
    // is used to force a fake key frame for the first encoded frame.
    bool next_frame_is_first_frame_;

    // A queue containing the necessary data for incoming frames. The boolean
    // represent whether the queued frame should force a key frame.
    std::queue<bool> queued_frames_;

    // A list of buffers available for putting fake encoded frames in.
    std::list<BitstreamBuffer> available_buffers_;

    base::WeakPtrFactory<FakeVideoEncodeAccelerator> weak_this_factory_;

    DISALLOW_COPY_AND_ASSIGN(FakeVideoEncodeAccelerator);
};

} // namespace media

#endif // MEDIA_VIDEO_FAKE_VIDEO_ENCODE_ACCELERATOR_H_
