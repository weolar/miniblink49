// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_SENDER_CODECS_VP8_VP8_ENCODER_H_
#define MEDIA_CAST_SENDER_CODECS_VP8_VP8_ENCODER_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/threading/thread_checker.h"
#include "media/cast/cast_config.h"
#include "media/cast/sender/software_video_encoder.h"
#include "third_party/libvpx_new/source/libvpx/vpx/vpx_encoder.h"
#include "ui/gfx/geometry/size.h"

namespace media {
class VideoFrame;
}

namespace media {
namespace cast {

    class Vp8Encoder : public SoftwareVideoEncoder {
    public:
        explicit Vp8Encoder(const VideoSenderConfig& video_config);

        ~Vp8Encoder() final;

        // SoftwareVideoEncoder implementations.
        void Initialize() final;
        void Encode(const scoped_refptr<media::VideoFrame>& video_frame,
            const base::TimeTicks& reference_time,
            SenderEncodedFrame* encoded_frame) final;
        void UpdateRates(uint32 new_bitrate) final;
        void GenerateKeyFrame() final;
        void LatestFrameIdToReference(uint32 frame_id) final;

    private:
        enum { kNumberOfVp8VideoBuffers = 3 };

        enum Vp8Buffers {
            kAltRefBuffer = 0,
            kGoldenBuffer = 1,
            kLastBuffer = 2,
            kNoBuffer = 3 // Note: must be last.
        };

        enum Vp8BufferState {
            kBufferStartState,
            kBufferSent,
            kBufferAcked
        };

        struct BufferState {
            uint32 frame_id;
            Vp8BufferState state;
        };

        bool is_initialized() const
        {
            // ConfigureForNewFrameSize() sets the timebase denominator value to
            // non-zero if the encoder is successfully initialized, and it is zero
            // otherwise.
            return config_.g_timebase.den != 0;
        }

        // If the |encoder_| is live, attempt reconfiguration to allow it to encode
        // frames at a new |frame_size|.  Otherwise, tear it down and re-create a new
        // |encoder_| instance.
        void ConfigureForNewFrameSize(const gfx::Size& frame_size);

        // Calculate which next Vp8 buffers to update with the next frame.
        Vp8Buffers GetNextBufferToUpdate();

        // Get encoder flags for our referenced encoder buffers.
        // Return which previous frame to reference.
        uint32 GetCodecReferenceFlags(vpx_codec_flags_t* flags);

        // Get encoder flags for our encoder buffers to update with next frame.
        void GetCodecUpdateFlags(Vp8Buffers buffer_to_update,
            vpx_codec_flags_t* flags);

        const VideoSenderConfig cast_config_;
        const bool use_multiple_video_buffers_;

        // VP8 internal objects.  These are valid for use only while is_initialized()
        // returns true.
        vpx_codec_enc_cfg_t config_;
        vpx_codec_ctx_t encoder_;

        // Set to true to request the next frame emitted by Vp8Encoder be a key frame.
        bool key_frame_requested_;

        // Saves the current bitrate setting, for when the |encoder_| is reconfigured
        // for different frame sizes.
        int bitrate_kbit_;

        // The |VideoFrame::timestamp()| of the last encoded frame.  This is used to
        // predict the duration of the next frame.
        base::TimeDelta last_frame_timestamp_;

        // The last encoded frame's ID.
        uint32 last_encoded_frame_id_;

        // Used to track which buffers are old enough to be re-used.
        uint32 last_acked_frame_id_;

        // Used by GetNextBufferToUpdate() to track how many consecutive times the
        // newest buffer had to be overwritten.
        int undroppable_frames_;

        // Tracks the lifecycle and dependency state of each of the three buffers.
        BufferState buffer_state_[kNumberOfVp8VideoBuffers];

        // This is bound to the thread where Initialize() is called.
        base::ThreadChecker thread_checker_;

        // Set to true once a frame with zero-length encoded data has been
        // encountered.
        // TODO(miu): Remove after discovering cause.  http://crbug.com/519022
        bool has_seen_zero_length_encoded_frame_;

        DISALLOW_COPY_AND_ASSIGN(Vp8Encoder);
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_SENDER_CODECS_VP8_VP8_ENCODER_H_
