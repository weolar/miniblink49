// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/sender/video_encoder.h"

#include "media/cast/sender/external_video_encoder.h"
#include "media/cast/sender/video_encoder_impl.h"

#if defined(OS_MACOSX)
#include "media/cast/sender/h264_vt_encoder.h"
#endif

namespace media {
namespace cast {

    // static
    scoped_ptr<VideoEncoder> VideoEncoder::Create(
        const scoped_refptr<CastEnvironment>& cast_environment,
        const VideoSenderConfig& video_config,
        const StatusChangeCallback& status_change_cb,
        const CreateVideoEncodeAcceleratorCallback& create_vea_cb,
        const CreateVideoEncodeMemoryCallback& create_video_encode_memory_cb)
    {
        // On MacOS or IOS, attempt to use the system VideoToolbox library to
        // perform optimized H.264 encoding.
#if defined(OS_MACOSX) || defined(OS_IOS)
        if (!video_config.use_external_encoder && H264VideoToolboxEncoder::IsSupported(video_config)) {
            return scoped_ptr<VideoEncoder>(new H264VideoToolboxEncoder(
                cast_environment, video_config, status_change_cb));
        }
#endif // defined(OS_MACOSX)

#if !defined(OS_IOS)
        // If the system provides a hardware-accelerated encoder, use it.
        if (ExternalVideoEncoder::IsSupported(video_config)) {
            return scoped_ptr<VideoEncoder>(new SizeAdaptableExternalVideoEncoder(
                cast_environment,
                video_config,
                status_change_cb,
                create_vea_cb,
                create_video_encode_memory_cb));
        }

        // Attempt to use the software encoder implementation.
        if (VideoEncoderImpl::IsSupported(video_config)) {
            return scoped_ptr<VideoEncoder>(new VideoEncoderImpl(
                cast_environment,
                video_config,
                status_change_cb));
        }
#endif // !defined(OS_IOS)

        // No encoder implementation will suffice.
        return nullptr;
    }

    scoped_ptr<VideoFrameFactory> VideoEncoder::CreateVideoFrameFactory()
    {
        return nullptr;
    }

    void VideoEncoder::EmitFrames()
    {
    }

} // namespace cast
} // namespace media
