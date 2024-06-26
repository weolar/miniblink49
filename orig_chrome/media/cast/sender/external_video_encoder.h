// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_SENDER_EXTERNAL_VIDEO_ENCODER_H_
#define MEDIA_CAST_SENDER_EXTERNAL_VIDEO_ENCODER_H_

#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "media/cast/cast_config.h"
#include "media/cast/cast_environment.h"
#include "media/cast/sender/size_adaptable_video_encoder_base.h"
#include "media/cast/sender/video_encoder.h"
#include "media/video/video_encode_accelerator.h"
#include "ui/gfx/geometry/size.h"

namespace media {
namespace cast {

    // Cast MAIN thread proxy to the internal media::VideoEncodeAccelerator
    // implementation running on a separate thread.  Encodes media::VideoFrames and
    // emits media::cast::EncodedFrames.
    class ExternalVideoEncoder : public VideoEncoder {
    public:
        // Returns true if the current platform and system configuration supports
        // using ExternalVideoEncoder with the given |video_config|.
        static bool IsSupported(const VideoSenderConfig& video_config);

        ExternalVideoEncoder(
            const scoped_refptr<CastEnvironment>& cast_environment,
            const VideoSenderConfig& video_config,
            const gfx::Size& frame_size,
            uint32 first_frame_id,
            const StatusChangeCallback& status_change_cb,
            const CreateVideoEncodeAcceleratorCallback& create_vea_cb,
            const CreateVideoEncodeMemoryCallback& create_video_encode_memory_cb);

        ~ExternalVideoEncoder() final;

        // VideoEncoder implementation.
        bool EncodeVideoFrame(
            const scoped_refptr<media::VideoFrame>& video_frame,
            const base::TimeTicks& reference_time,
            const FrameEncodedCallback& frame_encoded_callback) final;
        void SetBitRate(int new_bit_rate) final;
        void GenerateKeyFrame() final;
        void LatestFrameIdToReference(uint32 frame_id) final;

    private:
        class VEAClientImpl;

        // Method invoked by the CreateVideoEncodeAcceleratorCallback to construct a
        // VEAClientImpl to own and interface with a new |vea|.  Upon return,
        // |client_| holds a reference to the new VEAClientImpl.
        void OnCreateVideoEncodeAccelerator(
            const VideoSenderConfig& video_config,
            uint32 first_frame_id,
            const StatusChangeCallback& status_change_cb,
            scoped_refptr<base::SingleThreadTaskRunner> encoder_task_runner,
            scoped_ptr<media::VideoEncodeAccelerator> vea);

        const scoped_refptr<CastEnvironment> cast_environment_;
        const CreateVideoEncodeMemoryCallback create_video_encode_memory_cb_;

        // The size of the visible region of the video frames to be encoded.
        const gfx::Size frame_size_;

        int bit_rate_;
        bool key_frame_requested_;

        scoped_refptr<VEAClientImpl> client_;

        // Provides a weak pointer for the OnCreateVideoEncoderAccelerator() callback.
        // NOTE: Weak pointers must be invalidated before all other member variables.
        base::WeakPtrFactory<ExternalVideoEncoder> weak_factory_;

        DISALLOW_COPY_AND_ASSIGN(ExternalVideoEncoder);
    };

    // An implementation of SizeAdaptableVideoEncoderBase to proxy for
    // ExternalVideoEncoder instances.
    class SizeAdaptableExternalVideoEncoder : public SizeAdaptableVideoEncoderBase {
    public:
        SizeAdaptableExternalVideoEncoder(
            const scoped_refptr<CastEnvironment>& cast_environment,
            const VideoSenderConfig& video_config,
            const StatusChangeCallback& status_change_cb,
            const CreateVideoEncodeAcceleratorCallback& create_vea_cb,
            const CreateVideoEncodeMemoryCallback& create_video_encode_memory_cb);

        ~SizeAdaptableExternalVideoEncoder() final;

    protected:
        scoped_ptr<VideoEncoder> CreateEncoder() final;

    private:
        // Special callbacks needed by media::cast::ExternalVideoEncoder.
        // TODO(miu): Remove these.  http://crbug.com/454029
        const CreateVideoEncodeAcceleratorCallback create_vea_cb_;
        const CreateVideoEncodeMemoryCallback create_video_encode_memory_cb_;

        DISALLOW_COPY_AND_ASSIGN(SizeAdaptableExternalVideoEncoder);
    };

    // A utility class for examining the sequence of frames sent to an external
    // encoder, and returning an estimate of the what the software VP8 encoder would
    // have used for a quantizer value when encoding each frame.  The quantizer
    // value is related to the complexity of the content of the frame.
    class QuantizerEstimator {
    public:
        enum {
            NO_RESULT = -1,
            MIN_VP8_QUANTIZER = 4,
            MAX_VP8_QUANTIZER = 63,
        };

        QuantizerEstimator();
        ~QuantizerEstimator();

        // Discard any state related to the processing of prior frames.
        void Reset();

        // Examine |frame| and estimate and return the quantizer value the software
        // VP8 encoder would have used when encoding the frame, in the range
        // [4.0,63.0].  If |frame| is not in planar YUV format, or its size is empty,
        // this returns |NO_RESULT|.
        double EstimateForKeyFrame(const VideoFrame& frame);
        double EstimateForDeltaFrame(const VideoFrame& frame);

    private:
        enum {
            // The percentage of each frame to sample.  This value is based on an
            // analysis that showed sampling 10% of the rows of a frame generated
            // reasonably accurate results.
            FRAME_SAMPLING_PERCENT = 10,
        };

        // Returns true if the frame is in planar YUV format.
        static bool CanExamineFrame(const VideoFrame& frame);

        // Returns a value in the range [0,log2(num_buckets)], the Shannon Entropy
        // based on the probabilities of values falling within each of the buckets of
        // the given |histogram|.
        static double ComputeEntropyFromHistogram(const int* histogram,
            size_t num_buckets,
            int num_samples);

        // Map the |shannon_entropy| to its corresponding software VP8 quantizer.
        static double ToQuantizerEstimate(double shannon_entropy);

        // A cache of a subset of rows of pixels from the last frame examined.  This
        // is used to compute the entropy of the difference between frames, which in
        // turn is used to compute the entropy and quantizer.
        scoped_ptr<uint8[]> last_frame_pixel_buffer_;
        gfx::Size last_frame_size_;

        DISALLOW_COPY_AND_ASSIGN(QuantizerEstimator);
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_SENDER_EXTERNAL_VIDEO_ENCODER_H_
