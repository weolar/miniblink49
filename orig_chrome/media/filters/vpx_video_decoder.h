// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FILTERS_VPX_VIDEO_DECODER_H_
#define MEDIA_FILTERS_VPX_VIDEO_DECODER_H_

#include "base/callback.h"
#include "media/base/demuxer_stream.h"
#include "media/base/video_decoder.h"
#include "media/base/video_decoder_config.h"
#include "media/base/video_frame.h"
#include "media/base/video_frame_pool.h"

struct vpx_codec_ctx;
struct vpx_image;

namespace base {
class SingleThreadTaskRunner;
}

namespace media {

// Libvpx video decoder wrapper.
// Note: VpxVideoDecoder accepts only YV12A VP8 content or VP9 content. This is
// done to avoid usurping FFmpeg for all vp8 decoding, because the FFmpeg VP8
// decoder is faster than the libvpx VP8 decoder.
class MEDIA_EXPORT VpxVideoDecoder : public VideoDecoder {
public:
    explicit VpxVideoDecoder(
        const scoped_refptr<base::SingleThreadTaskRunner>& task_runner);
    ~VpxVideoDecoder() override;

    // VideoDecoder implementation.
    std::string GetDisplayName() const override;
    void Initialize(const VideoDecoderConfig& config,
        bool low_delay,
        const InitCB& init_cb,
        const OutputCB& output_cb) override;
    void Decode(const scoped_refptr<DecoderBuffer>& buffer,
        const DecodeCB& decode_cb) override;
    void Reset(const base::Closure& closure) override;

private:
    enum DecoderState {
        kUninitialized,
        kNormal,
        kFlushCodec,
        kDecodeFinished,
        kError
    };

    // Handles (re-)initializing the decoder with a (new) config.
    // Returns true when initialization was successful.
    bool ConfigureDecoder(const VideoDecoderConfig& config);

    void CloseDecoder();

    void DecodeBuffer(const scoped_refptr<DecoderBuffer>& buffer);
    bool VpxDecode(const scoped_refptr<DecoderBuffer>& buffer,
        scoped_refptr<VideoFrame>* video_frame);

    void CopyVpxImageTo(const vpx_image* vpx_image,
        const struct vpx_image* vpx_image_alpha,
        scoped_refptr<VideoFrame>* video_frame);

    scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

    DecoderState state_;

    OutputCB output_cb_;

    // TODO(xhwang): Merge DecodeBuffer() into Decode() and remove this.
    DecodeCB decode_cb_;

    VideoDecoderConfig config_;

    vpx_codec_ctx* vpx_codec_;
    vpx_codec_ctx* vpx_codec_alpha_;

    // Memory pool used for VP9 decoding.
    class MemoryPool;
    scoped_refptr<MemoryPool> memory_pool_;

    VideoFramePool frame_pool_;

    DISALLOW_COPY_AND_ASSIGN(VpxVideoDecoder);
};

} // namespace media

#endif // MEDIA_FILTERS_VPX_VIDEO_DECODER_H_
