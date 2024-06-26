// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FILTERS_FFMPEG_VIDEO_DECODER_H_
#define MEDIA_FILTERS_FFMPEG_VIDEO_DECODER_H_

#include <list>

#include "base/callback.h"
#include "base/memory/scoped_ptr.h"
#include "media/base/video_decoder.h"
#include "media/base/video_decoder_config.h"
#include "media/base/video_frame_pool.h"
#include "media/ffmpeg/ffmpeg_deleters.h"

struct AVCodecContext;
struct AVFrame;

namespace base {
class SingleThreadTaskRunner;
}

namespace media {

class DecoderBuffer;

class MEDIA_EXPORT FFmpegVideoDecoder : public VideoDecoder {
public:
    static bool IsCodecSupported(VideoCodec codec);

    explicit FFmpegVideoDecoder(
        const scoped_refptr<base::SingleThreadTaskRunner>& task_runner);
    ~FFmpegVideoDecoder() override;

    // Allow decoding of individual NALU. Entire frames are required by default.
    // Disables low-latency mode. Must be called before Initialize().
    void set_decode_nalus(bool decode_nalus) { decode_nalus_ = decode_nalus; }

    // VideoDecoder implementation.
    std::string GetDisplayName() const override;
    void Initialize(const VideoDecoderConfig& config,
        bool low_delay,
        const InitCB& init_cb,
        const OutputCB& output_cb) override;
    void Decode(const scoped_refptr<DecoderBuffer>& buffer,
        const DecodeCB& decode_cb) override;
    void Reset(const base::Closure& closure) override;

    // Callback called from within FFmpeg to allocate a buffer based on
    // the dimensions of |codec_context|. See AVCodecContext.get_buffer2
    // documentation inside FFmpeg.
    int GetVideoBuffer(struct AVCodecContext* codec_context,
        AVFrame* frame,
        int flags);

private:
    enum DecoderState {
        kUninitialized,
        kNormal,
        kDecodeFinished,
        kError
    };

    // Handles decoding an unencrypted encoded buffer.
    bool FFmpegDecode(const scoped_refptr<DecoderBuffer>& buffer,
        bool* has_produced_frame);

    // Handles (re-)initializing the decoder with a (new) config.
    // Returns true if initialization was successful.
    bool ConfigureDecoder(bool low_delay);

    // Releases resources associated with |codec_context_| and |av_frame_|
    // and resets them to NULL.
    void ReleaseFFmpegResources();

    scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

    DecoderState state_;

    OutputCB output_cb_;

    // FFmpeg structures owned by this object.
    scoped_ptr<AVCodecContext, ScopedPtrAVFreeContext> codec_context_;
    scoped_ptr<AVFrame, ScopedPtrAVFreeFrame> av_frame_;

    VideoDecoderConfig config_;

    VideoFramePool frame_pool_;

    bool decode_nalus_;

    DISALLOW_COPY_AND_ASSIGN(FFmpegVideoDecoder);
};

} // namespace media

#endif // MEDIA_FILTERS_FFMPEG_VIDEO_DECODER_H_
