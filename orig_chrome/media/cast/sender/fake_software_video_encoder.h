// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_SENDER_FAKE_SOFTWARE_VIDEO_ENCODER_H_
#define MEDIA_CAST_SENDER_FAKE_SOFTWARE_VIDEO_ENCODER_H_

#include "media/cast/cast_config.h"
#include "media/cast/sender/software_video_encoder.h"
#include "ui/gfx/geometry/size.h"

namespace media {
namespace cast {

    class FakeSoftwareVideoEncoder : public SoftwareVideoEncoder {
    public:
        FakeSoftwareVideoEncoder(const VideoSenderConfig& video_config);
        ~FakeSoftwareVideoEncoder() final;

        // SoftwareVideoEncoder implementations.
        void Initialize() final;
        void Encode(const scoped_refptr<media::VideoFrame>& video_frame,
            const base::TimeTicks& reference_time,
            SenderEncodedFrame* encoded_frame) final;
        void UpdateRates(uint32 new_bitrate) final;
        void GenerateKeyFrame() final;
        void LatestFrameIdToReference(uint32 frame_id) final;

    private:
        VideoSenderConfig video_config_;
        gfx::Size last_frame_size_;
        bool next_frame_is_key_;
        uint32 frame_id_;
        uint32 frame_id_to_reference_;
        int frame_size_;
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_SENDER_FAKE_SOFTWARE_VIDEO_ENCODER_H_
