// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/sender/fake_software_video_encoder.h"

#include "base/json/json_writer.h"
#include "base/values.h"
#include "media/base/video_frame.h"

#ifndef OFFICIAL_BUILD

namespace media {
namespace cast {

    FakeSoftwareVideoEncoder::FakeSoftwareVideoEncoder(
        const VideoSenderConfig& video_config)
        : video_config_(video_config)
        , next_frame_is_key_(true)
        , frame_id_(0)
        , frame_id_to_reference_(0)
        , frame_size_(0)
    {
    }

    FakeSoftwareVideoEncoder::~FakeSoftwareVideoEncoder() { }

    void FakeSoftwareVideoEncoder::Initialize() { }

    void FakeSoftwareVideoEncoder::Encode(
        const scoped_refptr<media::VideoFrame>& video_frame,
        const base::TimeTicks& reference_time,
        SenderEncodedFrame* encoded_frame)
    {
        DCHECK(encoded_frame);

        if (video_frame->visible_rect().size() != last_frame_size_) {
            next_frame_is_key_ = true;
            last_frame_size_ = video_frame->visible_rect().size();
        }

        encoded_frame->frame_id = frame_id_++;
        if (next_frame_is_key_) {
            encoded_frame->dependency = EncodedFrame::KEY;
            encoded_frame->referenced_frame_id = encoded_frame->frame_id;
            next_frame_is_key_ = false;
        } else {
            encoded_frame->dependency = EncodedFrame::DEPENDENT;
            encoded_frame->referenced_frame_id = encoded_frame->frame_id - 1;
        }
        encoded_frame->rtp_timestamp = TimeDeltaToRtpDelta(video_frame->timestamp(), kVideoFrequency);
        encoded_frame->reference_time = reference_time;

        base::DictionaryValue values;
        values.SetBoolean("key",
            encoded_frame->dependency == EncodedFrame::KEY);
        values.SetInteger("ref", encoded_frame->referenced_frame_id);
        values.SetInteger("id", encoded_frame->frame_id);
        values.SetInteger("size", frame_size_);
        base::JSONWriter::Write(values, &encoded_frame->data);
        encoded_frame->data.resize(
            std::max<size_t>(encoded_frame->data.size(), frame_size_), ' ');

        if (encoded_frame->dependency == EncodedFrame::KEY) {
            encoded_frame->deadline_utilization = 1.0;
            encoded_frame->lossy_utilization = 6.0;
        } else {
            encoded_frame->deadline_utilization = 0.8;
            encoded_frame->lossy_utilization = 0.8;
        }
    }

    void FakeSoftwareVideoEncoder::UpdateRates(uint32 new_bitrate)
    {
        frame_size_ = new_bitrate / video_config_.max_frame_rate / 8;
    }

    void FakeSoftwareVideoEncoder::GenerateKeyFrame()
    {
        next_frame_is_key_ = true;
    }

    void FakeSoftwareVideoEncoder::LatestFrameIdToReference(uint32 frame_id)
    {
        frame_id_to_reference_ = frame_id;
    }

} // namespace cast
} // namespace media

#endif
