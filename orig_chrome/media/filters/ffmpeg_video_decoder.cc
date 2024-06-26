// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/ffmpeg_video_decoder.h"

#include <algorithm>
#include <string>

#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/command_line.h"
#include "base/location.h"
#include "base/single_thread_task_runner.h"
#include "base/strings/string_number_conversions.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/decoder_buffer.h"
#include "media/base/limits.h"
#include "media/base/media_switches.h"
#include "media/base/pipeline.h"
#include "media/base/timestamp_constants.h"
#include "media/base/video_frame.h"
#include "media/base/video_util.h"
#include "media/ffmpeg/ffmpeg_common.h"
#include "media/filters/ffmpeg_glue.h"

namespace media {

// Always try to use three threads for video decoding.  There is little reason
// not to since current day CPUs tend to be multi-core and we measured
// performance benefits on older machines such as P4s with hyperthreading.
//
// Handling decoding on separate threads also frees up the pipeline thread to
// continue processing. Although it'd be nice to have the option of a single
// decoding thread, FFmpeg treats having one thread the same as having zero
// threads (i.e., avcodec_decode_video() will execute on the calling thread).
// Yet another reason for having two threads :)
static const int kDecodeThreads = 2;
static const int kMaxDecodeThreads = 16;

// Returns the number of threads given the FFmpeg CodecID. Also inspects the
// command line for a valid --video-threads flag.
static int GetThreadCount(AVCodecID codec_id)
{
    // Refer to http://crbug.com/93932 for tsan suppressions on decoding.
    int decode_threads = kDecodeThreads;

    const base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();
    std::string threads(cmd_line->GetSwitchValueASCII(switches::kVideoThreads));
    if (threads.empty() || !base::StringToInt(threads, &decode_threads))
        return decode_threads;

    decode_threads = std::max(decode_threads, 0);
    decode_threads = std::min(decode_threads, kMaxDecodeThreads);
    return decode_threads;
}

static int GetVideoBufferImpl(struct AVCodecContext* s,
    AVFrame* frame,
    int flags)
{
    FFmpegVideoDecoder* decoder = static_cast<FFmpegVideoDecoder*>(s->opaque);
    return decoder->GetVideoBuffer(s, frame, flags);
}

static void ReleaseVideoBufferImpl(void* opaque, uint8* data)
{
    scoped_refptr<VideoFrame> video_frame;
    video_frame.swap(reinterpret_cast<VideoFrame**>(&opaque));
}

// static
bool FFmpegVideoDecoder::IsCodecSupported(VideoCodec codec)
{
    FFmpegGlue::InitializeFFmpeg();
    return avcodec_find_decoder(VideoCodecToCodecID(codec)) != nullptr;
}

FFmpegVideoDecoder::FFmpegVideoDecoder(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner)
    : task_runner_(task_runner)
    , state_(kUninitialized)
    , decode_nalus_(false)
{
}

int FFmpegVideoDecoder::GetVideoBuffer(struct AVCodecContext* codec_context,
    AVFrame* frame,
    int flags)
{
    // Don't use |codec_context_| here! With threaded decoding,
    // it will contain unsynchronized width/height/pix_fmt values,
    // whereas |codec_context| contains the current threads's
    // updated width/height/pix_fmt, which can change for adaptive
    // content.
    const VideoPixelFormat format = AVPixelFormatToVideoPixelFormat(codec_context->pix_fmt);

    if (format == PIXEL_FORMAT_UNKNOWN)
        return AVERROR(EINVAL);
    DCHECK(format == PIXEL_FORMAT_YV12 || format == PIXEL_FORMAT_YV16 || format == PIXEL_FORMAT_YV24);

    gfx::Size size(codec_context->width, codec_context->height);
    const int ret = av_image_check_size(size.width(), size.height(), 0, NULL);
    if (ret < 0)
        return ret;

    gfx::Size natural_size;
    if (codec_context->sample_aspect_ratio.num > 0) {
        natural_size = GetNaturalSize(size,
            codec_context->sample_aspect_ratio.num,
            codec_context->sample_aspect_ratio.den);
    } else {
        natural_size = config_.natural_size();
    }

    // FFmpeg has specific requirements on the allocation size of the frame.  The
    // following logic replicates FFmpeg's allocation strategy to ensure buffers
    // are not overread / overwritten.  See ff_init_buffer_info() for details.
    //
    // When lowres is non-zero, dimensions should be divided by 2^(lowres), but
    // since we don't use this, just DCHECK that it's zero.
    DCHECK_EQ(codec_context->lowres, 0);
    gfx::Size coded_size(std::max(size.width(), codec_context->coded_width),
        std::max(size.height(), codec_context->coded_height));

    if (!VideoFrame::IsValidConfig(format, VideoFrame::STORAGE_UNKNOWN,
            coded_size, gfx::Rect(size), natural_size)) {
        return AVERROR(EINVAL);
    }

    // FFmpeg expects the initialize allocation to be zero-initialized.  Failure
    // to do so can lead to unitialized value usage.  See http://crbug.com/390941
    scoped_refptr<VideoFrame> video_frame = frame_pool_.CreateFrame(
        format, coded_size, gfx::Rect(size), natural_size, kNoTimestamp());

    // Prefer the color space from the codec context. If it's not specified (or is
    // set to an unsupported value), fall back on the value from the config.
    ColorSpace color_space = AVColorSpaceToColorSpace(codec_context->colorspace,
        codec_context->color_range);
    if (color_space == COLOR_SPACE_UNSPECIFIED)
        color_space = config_.color_space();
    video_frame->metadata()->SetInteger(VideoFrameMetadata::COLOR_SPACE,
        color_space);

    for (size_t i = 0; i < VideoFrame::NumPlanes(video_frame->format()); i++) {
        frame->data[i] = video_frame->data(i);
        frame->linesize[i] = video_frame->stride(i);
    }

    frame->width = coded_size.width();
    frame->height = coded_size.height();
    frame->format = codec_context->pix_fmt;
    frame->reordered_opaque = codec_context->reordered_opaque;

    // Now create an AVBufferRef for the data just allocated. It will own the
    // reference to the VideoFrame object.
    void* opaque = NULL;
    video_frame.swap(reinterpret_cast<VideoFrame**>(&opaque));
    frame->buf[0] = av_buffer_create(frame->data[0],
        VideoFrame::AllocationSize(format, coded_size),
        ReleaseVideoBufferImpl,
        opaque,
        0);
    return 0;
}

std::string FFmpegVideoDecoder::GetDisplayName() const
{
    return "FFmpegVideoDecoder";
}

void FFmpegVideoDecoder::Initialize(const VideoDecoderConfig& config,
    bool low_delay,
    const InitCB& init_cb,
    const OutputCB& output_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(config.IsValidConfig());
    DCHECK(!output_cb.is_null());

    InitCB bound_init_cb = BindToCurrentLoop(init_cb);

    if (config.is_encrypted()) {
        bound_init_cb.Run(false);
        return;
    }

    FFmpegGlue::InitializeFFmpeg();
    config_ = config;

    // TODO(xhwang): Only set |config_| after we successfully configure the
    // decoder.
    if (!ConfigureDecoder(low_delay)) {
        bound_init_cb.Run(false);
        return;
    }

    output_cb_ = BindToCurrentLoop(output_cb);

    // Success!
    state_ = kNormal;
    bound_init_cb.Run(true);
}

void FFmpegVideoDecoder::Decode(const scoped_refptr<DecoderBuffer>& buffer,
    const DecodeCB& decode_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(buffer.get());
    DCHECK(!decode_cb.is_null());
    CHECK_NE(state_, kUninitialized);

    DecodeCB decode_cb_bound = BindToCurrentLoop(decode_cb);

    if (state_ == kError) {
        decode_cb_bound.Run(kDecodeError);
        return;
    }

    if (state_ == kDecodeFinished) {
        decode_cb_bound.Run(kOk);
        return;
    }

    DCHECK_EQ(state_, kNormal);

    // During decode, because reads are issued asynchronously, it is possible to
    // receive multiple end of stream buffers since each decode is acked. When the
    // first end of stream buffer is read, FFmpeg may still have frames queued
    // up in the decoder so we need to go through the decode loop until it stops
    // giving sensible data.  After that, the decoder should output empty
    // frames.  There are three states the decoder can be in:
    //
    //   kNormal: This is the starting state. Buffers are decoded. Decode errors
    //            are discarded.
    //   kDecodeFinished: All calls return empty frames.
    //   kError: Unexpected error happened.
    //
    // These are the possible state transitions.
    //
    // kNormal -> kDecodeFinished:
    //     When EOS buffer is received and the codec has been flushed.
    // kNormal -> kError:
    //     A decoding error occurs and decoding needs to stop.
    // (any state) -> kNormal:
    //     Any time Reset() is called.

    bool has_produced_frame;
    do {
        has_produced_frame = false;
        if (!FFmpegDecode(buffer, &has_produced_frame)) {
            state_ = kError;
            decode_cb_bound.Run(kDecodeError);
            return;
        }
        // Repeat to flush the decoder after receiving EOS buffer.
    } while (buffer->end_of_stream() && has_produced_frame);

    if (buffer->end_of_stream())
        state_ = kDecodeFinished;

    // VideoDecoderShim expects that |decode_cb| is called only after
    // |output_cb_|.
    decode_cb_bound.Run(kOk);
}

void FFmpegVideoDecoder::Reset(const base::Closure& closure)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    avcodec_flush_buffers(codec_context_.get());
    state_ = kNormal;
    task_runner_->PostTask(FROM_HERE, closure);
}

FFmpegVideoDecoder::~FFmpegVideoDecoder()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (state_ != kUninitialized)
        ReleaseFFmpegResources();
}

bool FFmpegVideoDecoder::FFmpegDecode(
    const scoped_refptr<DecoderBuffer>& buffer,
    bool* has_produced_frame)
{
    DCHECK(!*has_produced_frame);

    // Create a packet for input data.
    // Due to FFmpeg API changes we no longer have const read-only pointers.
    AVPacket packet;
    av_init_packet(&packet);
    if (buffer->end_of_stream()) {
        packet.data = NULL;
        packet.size = 0;
    } else {
        packet.data = const_cast<uint8*>(buffer->data());
        packet.size = buffer->data_size();

        // Let FFmpeg handle presentation timestamp reordering.
        codec_context_->reordered_opaque = buffer->timestamp().InMicroseconds();
    }

    int frame_decoded = 0;
    int result = avcodec_decode_video2(codec_context_.get(),
        av_frame_.get(),
        &frame_decoded,
        &packet);
    // Log the problem if we can't decode a video frame and exit early.
    if (result < 0) {
        LOG(ERROR) << "Error decoding video: " << buffer->AsHumanReadableString();
        return false;
    }

    // FFmpeg says some codecs might have multiple frames per packet.  Previous
    // discussions with rbultje@ indicate this shouldn't be true for the codecs
    // we use.
    DCHECK_EQ(result, packet.size);

    // If no frame was produced then signal that more data is required to
    // produce more frames. This can happen under two circumstances:
    //   1) Decoder was recently initialized/flushed
    //   2) End of stream was reached and all internal frames have been output
    if (frame_decoded == 0) {
        return true;
    }

    // TODO(fbarchard): Work around for FFmpeg http://crbug.com/27675
    // The decoder is in a bad state and not decoding correctly.
    // Checking for NULL avoids a crash in CopyPlane().
    if (!av_frame_->data[VideoFrame::kYPlane] || !av_frame_->data[VideoFrame::kUPlane] || !av_frame_->data[VideoFrame::kVPlane]) {
        LOG(ERROR) << "Video frame was produced yet has invalid frame data.";
        av_frame_unref(av_frame_.get());
        return false;
    }

    scoped_refptr<VideoFrame> frame = reinterpret_cast<VideoFrame*>(av_buffer_get_opaque(av_frame_->buf[0]));
    frame->set_timestamp(
        base::TimeDelta::FromMicroseconds(av_frame_->reordered_opaque));
    *has_produced_frame = true;
    output_cb_.Run(frame);

    av_frame_unref(av_frame_.get());
    return true;
}

void FFmpegVideoDecoder::ReleaseFFmpegResources()
{
    codec_context_.reset();
    av_frame_.reset();
}

bool FFmpegVideoDecoder::ConfigureDecoder(bool low_delay)
{
    DCHECK(config_.IsValidConfig());
    DCHECK(!config_.is_encrypted());

    // Release existing decoder resources if necessary.
    ReleaseFFmpegResources();

    // Initialize AVCodecContext structure.
    codec_context_.reset(avcodec_alloc_context3(NULL));
    VideoDecoderConfigToAVCodecContext(config_, codec_context_.get());

    codec_context_->thread_count = GetThreadCount(codec_context_->codec_id);
    codec_context_->thread_type = low_delay ? FF_THREAD_SLICE : FF_THREAD_FRAME;
    codec_context_->opaque = this;
    codec_context_->flags |= CODEC_FLAG_EMU_EDGE;
    codec_context_->get_buffer2 = GetVideoBufferImpl;
    codec_context_->refcounted_frames = 1;

    if (decode_nalus_)
        codec_context_->flags2 |= CODEC_FLAG2_CHUNKS;

    AVCodec* codec = avcodec_find_decoder(codec_context_->codec_id);
    if (!codec || avcodec_open2(codec_context_.get(), codec, NULL) < 0) {
        ReleaseFFmpegResources();
        return false;
    }

    av_frame_.reset(av_frame_alloc());
    return true;
}

} // namespace media
