// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/ffmpeg_audio_decoder.h"

#include "base/callback_helpers.h"
#include "base/single_thread_task_runner.h"
#include "media/base/audio_buffer.h"
#include "media/base/audio_bus.h"
#include "media/base/audio_decoder_config.h"
#include "media/base/audio_discard_helper.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/decoder_buffer.h"
#include "media/base/limits.h"
#include "media/base/timestamp_constants.h"
#include "media/ffmpeg/ffmpeg_common.h"
#include "media/filters/ffmpeg_glue.h"

namespace media {

// Returns true if the decode result was end of stream.
static inline bool IsEndOfStream(int result,
    int decoded_size,
    const scoped_refptr<DecoderBuffer>& input)
{
    // Three conditions to meet to declare end of stream for this decoder:
    // 1. FFmpeg didn't read anything.
    // 2. FFmpeg didn't output anything.
    // 3. An end of stream buffer is received.
    return result == 0 && decoded_size == 0 && input->end_of_stream();
}

// Return the number of channels from the data in |frame|.
static inline int DetermineChannels(AVFrame* frame)
{
#if defined(CHROMIUM_NO_AVFRAME_CHANNELS)
    // When use_system_ffmpeg==1, libav's AVFrame doesn't have channels field.
    return av_get_channel_layout_nb_channels(frame->channel_layout);
#else
    return frame->channels;
#endif
}

// Called by FFmpeg's allocation routine to free a buffer. |opaque| is the
// AudioBuffer allocated, so unref it.
static void ReleaseAudioBufferImpl(void* opaque, uint8* data)
{
    scoped_refptr<AudioBuffer> buffer;
    buffer.swap(reinterpret_cast<AudioBuffer**>(&opaque));
}

// Called by FFmpeg's allocation routine to allocate a buffer. Uses
// AVCodecContext.opaque to get the object reference in order to call
// GetAudioBuffer() to do the actual allocation.
static int GetAudioBuffer(struct AVCodecContext* s, AVFrame* frame, int flags)
{
    DCHECK(s->codec->capabilities & CODEC_CAP_DR1);
    DCHECK_EQ(s->codec_type, AVMEDIA_TYPE_AUDIO);

    // Since this routine is called by FFmpeg when a buffer is required for audio
    // data, use the values supplied by FFmpeg (ignoring the current settings).
    // FFmpegDecode() gets to determine if the buffer is useable or not.
    AVSampleFormat format = static_cast<AVSampleFormat>(frame->format);
    SampleFormat sample_format = AVSampleFormatToSampleFormat(format);
    int channels = DetermineChannels(frame);
    if (channels <= 0 || channels >= limits::kMaxChannels) {
        DLOG(ERROR) << "Requested number of channels (" << channels
                    << ") exceeds limit.";
        return AVERROR(EINVAL);
    }

    int bytes_per_channel = SampleFormatToBytesPerChannel(sample_format);
    if (frame->nb_samples <= 0)
        return AVERROR(EINVAL);

    if (s->channels != channels) {
        DLOG(ERROR) << "AVCodecContext and AVFrame disagree on channel count.";
        return AVERROR(EINVAL);
    }

    // Determine how big the buffer should be and allocate it. FFmpeg may adjust
    // how big each channel data is in order to meet the alignment policy, so
    // we need to take this into consideration.
    int buffer_size_in_bytes = av_samples_get_buffer_size(&frame->linesize[0],
        channels,
        frame->nb_samples,
        format,
        AudioBuffer::kChannelAlignment);
    // Check for errors from av_samples_get_buffer_size().
    if (buffer_size_in_bytes < 0)
        return buffer_size_in_bytes;
    int frames_required = buffer_size_in_bytes / bytes_per_channel / channels;
    DCHECK_GE(frames_required, frame->nb_samples);
    scoped_refptr<AudioBuffer> buffer = AudioBuffer::CreateBuffer(
        sample_format,
        ChannelLayoutToChromeChannelLayout(s->channel_layout, s->channels),
        channels,
        s->sample_rate,
        frames_required);

    // Initialize the data[] and extended_data[] fields to point into the memory
    // allocated for AudioBuffer. |number_of_planes| will be 1 for interleaved
    // audio and equal to |channels| for planar audio.
    int number_of_planes = buffer->channel_data().size();
    if (number_of_planes <= AV_NUM_DATA_POINTERS) {
        DCHECK_EQ(frame->extended_data, frame->data);
        for (int i = 0; i < number_of_planes; ++i)
            frame->data[i] = buffer->channel_data()[i];
    } else {
        // There are more channels than can fit into data[], so allocate
        // extended_data[] and fill appropriately.
        frame->extended_data = static_cast<uint8**>(
            av_malloc(number_of_planes * sizeof(*frame->extended_data)));
        int i = 0;
        for (; i < AV_NUM_DATA_POINTERS; ++i)
            frame->extended_data[i] = frame->data[i] = buffer->channel_data()[i];
        for (; i < number_of_planes; ++i)
            frame->extended_data[i] = buffer->channel_data()[i];
    }

    // Now create an AVBufferRef for the data just allocated. It will own the
    // reference to the AudioBuffer object.
    void* opaque = NULL;
    buffer.swap(reinterpret_cast<AudioBuffer**>(&opaque));
    frame->buf[0] = av_buffer_create(
        frame->data[0], buffer_size_in_bytes, ReleaseAudioBufferImpl, opaque, 0);
    return 0;
}

FFmpegAudioDecoder::FFmpegAudioDecoder(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    const scoped_refptr<MediaLog>& media_log)
    : task_runner_(task_runner)
    , state_(kUninitialized)
    , av_sample_format_(0)
    , media_log_(media_log)
{
}

FFmpegAudioDecoder::~FFmpegAudioDecoder()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (state_ != kUninitialized)
        ReleaseFFmpegResources();
}

std::string FFmpegAudioDecoder::GetDisplayName() const
{
    return "FFmpegAudioDecoder";
}

void FFmpegAudioDecoder::Initialize(const AudioDecoderConfig& config,
    const InitCB& init_cb,
    const OutputCB& output_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(config.IsValidConfig());

    InitCB bound_init_cb = BindToCurrentLoop(init_cb);

    if (config.is_encrypted()) {
        bound_init_cb.Run(false);
        return;
    }

    FFmpegGlue::InitializeFFmpeg();
    config_ = config;

    // TODO(xhwang): Only set |config_| after we successfully configure the
    // decoder. Make sure we clean up all member variables upon failure.
    if (!ConfigureDecoder()) {
        bound_init_cb.Run(false);
        return;
    }

    // Success!
    output_cb_ = BindToCurrentLoop(output_cb);
    state_ = kNormal;
    bound_init_cb.Run(true);
}

void FFmpegAudioDecoder::Decode(const scoped_refptr<DecoderBuffer>& buffer,
    const DecodeCB& decode_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(!decode_cb.is_null());
    CHECK_NE(state_, kUninitialized);
    DecodeCB decode_cb_bound = BindToCurrentLoop(decode_cb);

    if (state_ == kError) {
        decode_cb_bound.Run(kDecodeError);
        return;
    }

    // Do nothing if decoding has finished.
    if (state_ == kDecodeFinished) {
        decode_cb_bound.Run(kOk);
        return;
    }

    DecodeBuffer(buffer, decode_cb_bound);
}

void FFmpegAudioDecoder::Reset(const base::Closure& closure)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    avcodec_flush_buffers(codec_context_.get());
    state_ = kNormal;
    ResetTimestampState();
    task_runner_->PostTask(FROM_HERE, closure);
}

void FFmpegAudioDecoder::DecodeBuffer(
    const scoped_refptr<DecoderBuffer>& buffer,
    const DecodeCB& decode_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_NE(state_, kUninitialized);
    DCHECK_NE(state_, kDecodeFinished);
    DCHECK_NE(state_, kError);
    DCHECK(buffer.get());

    // Make sure we are notified if http://crbug.com/49709 returns.  Issue also
    // occurs with some damaged files.
    if (!buffer->end_of_stream() && buffer->timestamp() == kNoTimestamp()) {
        DVLOG(1) << "Received a buffer without timestamps!";
        decode_cb.Run(kDecodeError);
        return;
    }

    bool has_produced_frame;
    do {
        has_produced_frame = false;
        if (!FFmpegDecode(buffer, &has_produced_frame)) {
            state_ = kError;
            decode_cb.Run(kDecodeError);
            return;
        }
        // Repeat to flush the decoder after receiving EOS buffer.
    } while (buffer->end_of_stream() && has_produced_frame);

    if (buffer->end_of_stream())
        state_ = kDecodeFinished;

    decode_cb.Run(kOk);
}

bool FFmpegAudioDecoder::FFmpegDecode(
    const scoped_refptr<DecoderBuffer>& buffer,
    bool* has_produced_frame)
{
    DCHECK(!*has_produced_frame);

    AVPacket packet;
    av_init_packet(&packet);
    if (buffer->end_of_stream()) {
        packet.data = NULL;
        packet.size = 0;
    } else {
        packet.data = const_cast<uint8*>(buffer->data());
        packet.size = buffer->data_size();
    }

    // Each audio packet may contain several frames, so we must call the decoder
    // until we've exhausted the packet.  Regardless of the packet size we always
    // want to hand it to the decoder at least once, otherwise we would end up
    // skipping end of stream packets since they have a size of zero.
    do {
        int frame_decoded = 0;
        const int result = avcodec_decode_audio4(
            codec_context_.get(), av_frame_.get(), &frame_decoded, &packet);

        if (result < 0) {
            DCHECK(!buffer->end_of_stream())
                << "End of stream buffer produced an error! "
                << "This is quite possibly a bug in the audio decoder not handling "
                << "end of stream AVPackets correctly.";

            MEDIA_LOG(DEBUG, media_log_)
                << "Dropping audio frame which failed decode with timestamp: "
                << buffer->timestamp().InMicroseconds()
                << " us, duration: " << buffer->duration().InMicroseconds()
                << " us, packet size: " << buffer->data_size() << " bytes";

            break;
        }

        // Update packet size and data pointer in case we need to call the decoder
        // with the remaining bytes from this packet.
        packet.size -= result;
        packet.data += result;

        scoped_refptr<AudioBuffer> output;
        const int channels = DetermineChannels(av_frame_.get());
        if (frame_decoded) {
            if (av_frame_->sample_rate != config_.samples_per_second() || channels != ChannelLayoutToChannelCount(config_.channel_layout()) || av_frame_->format != av_sample_format_) {
                DLOG(ERROR) << "Unsupported midstream configuration change!"
                            << " Sample Rate: " << av_frame_->sample_rate << " vs "
                            << config_.samples_per_second()
                            << ", Channels: " << channels << " vs "
                            << ChannelLayoutToChannelCount(config_.channel_layout())
                            << ", Sample Format: " << av_frame_->format << " vs "
                            << av_sample_format_;

                if (config_.codec() == kCodecAAC && av_frame_->sample_rate == 2 * config_.samples_per_second()) {
                    MEDIA_LOG(DEBUG, media_log_)
                        << "Implicit HE-AAC signalling is being"
                        << " used. Please use mp4a.40.5 instead of"
                        << " mp4a.40.2 in the mimetype.";
                }
                // This is an unrecoverable error, so bail out.
                av_frame_unref(av_frame_.get());
                return false;
            }

            // Get the AudioBuffer that the data was decoded into. Adjust the number
            // of frames, in case fewer than requested were actually decoded.
            output = reinterpret_cast<AudioBuffer*>(
                av_buffer_get_opaque(av_frame_->buf[0]));

            DCHECK_EQ(ChannelLayoutToChannelCount(config_.channel_layout()),
                output->channel_count());
            const int unread_frames = output->frame_count() - av_frame_->nb_samples;
            DCHECK_GE(unread_frames, 0);
            if (unread_frames > 0)
                output->TrimEnd(unread_frames);
            av_frame_unref(av_frame_.get());
        }

        // WARNING: |av_frame_| no longer has valid data at this point.
        const int decoded_frames = frame_decoded ? output->frame_count() : 0;
        if (IsEndOfStream(result, decoded_frames, buffer)) {
            DCHECK_EQ(packet.size, 0);
        } else if (discard_helper_->ProcessBuffers(buffer, output)) {
            *has_produced_frame = true;
            output_cb_.Run(output);
        }
    } while (packet.size > 0);

    return true;
}

void FFmpegAudioDecoder::ReleaseFFmpegResources()
{
    codec_context_.reset();
    av_frame_.reset();
}

bool FFmpegAudioDecoder::ConfigureDecoder()
{
    DCHECK(config_.IsValidConfig());
    DCHECK(!config_.is_encrypted());

    // Release existing decoder resources if necessary.
    ReleaseFFmpegResources();

    // Initialize AVCodecContext structure.
    codec_context_.reset(avcodec_alloc_context3(NULL));
    AudioDecoderConfigToAVCodecContext(config_, codec_context_.get());

    codec_context_->opaque = this;
    codec_context_->get_buffer2 = GetAudioBuffer;
    codec_context_->refcounted_frames = 1;

    AVCodec* codec = avcodec_find_decoder(codec_context_->codec_id);
    if (!codec || avcodec_open2(codec_context_.get(), codec, NULL) < 0) {
        DLOG(ERROR) << "Could not initialize audio decoder: "
                    << codec_context_->codec_id;
        ReleaseFFmpegResources();
        state_ = kUninitialized;
        return false;
    }

    // Success!
    av_frame_.reset(av_frame_alloc());
    av_sample_format_ = codec_context_->sample_fmt;

    if (codec_context_->channels != ChannelLayoutToChannelCount(config_.channel_layout())) {
        DLOG(ERROR) << "Audio configuration specified "
                    << ChannelLayoutToChannelCount(config_.channel_layout())
                    << " channels, but FFmpeg thinks the file contains "
                    << codec_context_->channels << " channels";
        ReleaseFFmpegResources();
        state_ = kUninitialized;
        return false;
    }

    ResetTimestampState();
    return true;
}

void FFmpegAudioDecoder::ResetTimestampState()
{
    discard_helper_.reset(new AudioDiscardHelper(config_.samples_per_second(),
        config_.codec_delay()));
    discard_helper_->Reset(config_.codec_delay());
}

} // namespace media
