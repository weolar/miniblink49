// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_SENDER_VIDEO_SENDER_H_
#define MEDIA_CAST_SENDER_VIDEO_SENDER_H_

#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/non_thread_safe.h"
#include "base/time/tick_clock.h"
#include "base/time/time.h"
#include "media/cast/cast_config.h"
#include "media/cast/sender/congestion_control.h"
#include "media/cast/sender/frame_sender.h"

namespace media {

class VideoFrame;

namespace cast {

    class CastTransportSender;
    class VideoEncoder;
    class VideoFrameFactory;

    typedef base::Callback<void(base::TimeDelta)> PlayoutDelayChangeCB;

    // Not thread safe. Only called from the main cast thread.
    // This class owns all objects related to sending video, objects that create RTP
    // packets, congestion control, video encoder, parsing and sending of
    // RTCP packets.
    // Additionally it posts a bunch of delayed tasks to the main thread for various
    // timeouts.
    class VideoSender : public FrameSender,
                        public base::NonThreadSafe,
                        public base::SupportsWeakPtr<VideoSender> {
    public:
        VideoSender(scoped_refptr<CastEnvironment> cast_environment,
            const VideoSenderConfig& video_config,
            const StatusChangeCallback& status_change_cb,
            const CreateVideoEncodeAcceleratorCallback& create_vea_cb,
            const CreateVideoEncodeMemoryCallback& create_video_encode_mem_cb,
            CastTransportSender* const transport_sender,
            const PlayoutDelayChangeCB& playout_delay_change_cb);

        ~VideoSender() override;

        // Note: It is not guaranteed that |video_frame| will actually be encoded and
        // sent, if VideoSender detects too many frames in flight.  Therefore, clients
        // should be careful about the rate at which this method is called.
        void InsertRawVideoFrame(const scoped_refptr<media::VideoFrame>& video_frame,
            const base::TimeTicks& reference_time);

        // Creates a |VideoFrameFactory| object to vend |VideoFrame| object with
        // encoder affinity (defined as offering some sort of performance benefit). If
        // the encoder does not have any such capability, returns null.
        scoped_ptr<VideoFrameFactory> CreateVideoFrameFactory();

    protected:
        int GetNumberOfFramesInEncoder() const final;
        base::TimeDelta GetInFlightMediaDuration() const final;
        void OnAck(uint32 frame_id) final;

        // Return the maximum target bitrate that should be used for the given video
        // |frame|.  This will be provided to CongestionControl as a soft maximum
        // limit, and should be interpreted as "the point above which the extra
        // encoder CPU time + network bandwidth usage isn't warranted for the amount
        // of further quality improvement to be gained."
        static int GetMaximumTargetBitrateForFrame(const media::VideoFrame& frame);

    private:
        // Called by the |video_encoder_| with the next EncodedFrame to send.
        void OnEncodedVideoFrame(const scoped_refptr<media::VideoFrame>& video_frame,
            int encoder_bitrate,
            scoped_ptr<SenderEncodedFrame> encoded_frame);

        // Encodes media::VideoFrame images into EncodedFrames.  Per configuration,
        // this will point to either the internal software-based encoder or a proxy to
        // a hardware-based encoder.
        scoped_ptr<VideoEncoder> video_encoder_;

        // The number of frames queued for encoding, but not yet sent.
        int frames_in_encoder_;

        // The duration of video queued for encoding, but not yet sent.
        base::TimeDelta duration_in_encoder_;

        // The timestamp of the frame that was last enqueued in |video_encoder_|.
        RtpTimestamp last_enqueued_frame_rtp_timestamp_;
        base::TimeTicks last_enqueued_frame_reference_time_;

        // Remember what we set the bitrate to before, no need to set it again if
        // we get the same value.
        int last_bitrate_;

        PlayoutDelayChangeCB playout_delay_change_cb_;

        // The video encoder's performance metrics as of the last call to
        // OnEncodedVideoFrame().  See header file comments for SenderEncodedFrame for
        // an explanation of these values.
        double last_reported_deadline_utilization_;
        double last_reported_lossy_utilization_;

        // NOTE: Weak pointers must be invalidated before all other member variables.
        base::WeakPtrFactory<VideoSender> weak_factory_;

        DISALLOW_COPY_AND_ASSIGN(VideoSender);
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_SENDER_VIDEO_SENDER_H_
