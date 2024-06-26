// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/ffmpeg_demuxer.h"

#include <algorithm>

#include "base/base64.h"
#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/memory/scoped_ptr.h"
#include "base/metrics/histogram_macros.h"
#include "base/metrics/sparse_histogram.h"
#include "base/single_thread_task_runner.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/sys_byteorder.h"
#include "base/task_runner_util.h"
#include "base/thread_task_runner_handle.h"
#include "base/time/time.h"
#include "media/audio/sample_rates.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/decrypt_config.h"
#include "media/base/limits.h"
#include "media/base/media_log.h"
#include "media/base/timestamp_constants.h"
#include "media/ffmpeg/ffmpeg_common.h"
#include "media/filters/ffmpeg_aac_bitstream_converter.h"
#include "media/filters/ffmpeg_bitstream_converter.h"
#include "media/filters/ffmpeg_glue.h"
#include "media/filters/ffmpeg_h264_to_annex_b_bitstream_converter.h"
#include "media/filters/webvtt_util.h"
#include "media/formats/webm/webm_crypto_helpers.h"

#if defined(ENABLE_HEVC_DEMUXING)
#include "media/filters/ffmpeg_h265_to_annex_b_bitstream_converter.h"
#endif

namespace media {

static base::Time ExtractTimelineOffset(AVFormatContext* format_context)
{
    if (strstr(format_context->iformat->name, "webm") || strstr(format_context->iformat->name, "matroska")) {
        const AVDictionaryEntry* entry = av_dict_get(format_context->metadata, "creation_time", NULL, 0);

        base::Time timeline_offset;
        if (entry != NULL && entry->value != NULL && FFmpegUTCDateToTime(entry->value, &timeline_offset)) {
            return timeline_offset;
        }
    }

    return base::Time();
}

static base::TimeDelta FramesToTimeDelta(int frames, double sample_rate)
{
    return base::TimeDelta::FromMicroseconds(
        frames * base::Time::kMicrosecondsPerSecond / sample_rate);
}

static base::TimeDelta ExtractStartTime(AVStream* stream,
    base::TimeDelta start_time_estimate)
{
    DCHECK(start_time_estimate != kNoTimestamp());
    if (stream->start_time == static_cast<int64_t>(AV_NOPTS_VALUE)) {
        return start_time_estimate == kInfiniteDuration() ? kNoTimestamp()
                                                          : start_time_estimate;
    }

    // First try the lower of the estimate and the |start_time| value.
    base::TimeDelta start_time = std::min(ConvertFromTimeBase(stream->time_base, stream->start_time),
        start_time_estimate);

    // Next see if the first buffered pts value is usable.
    if (stream->pts_buffer[0] != static_cast<int64_t>(AV_NOPTS_VALUE)) {
        const base::TimeDelta buffered_pts = ConvertFromTimeBase(stream->time_base, stream->pts_buffer[0]);
        if (buffered_pts < start_time)
            start_time = buffered_pts;
    }

    // NOTE: Do not use AVStream->first_dts since |start_time| should be a
    // presentation timestamp.
    return start_time;
}

// Some videos just want to watch the world burn, with a height of 0; cap the
// "infinite" aspect ratio resulting.
const int kInfiniteRatio = 99999;

// Common aspect ratios (multiplied by 100 and truncated) used for histogramming
// video sizes.  These were taken on 20111103 from
// http://wikipedia.org/wiki/Aspect_ratio_(image)#Previous_and_currently_used_aspect_ratios
const int kCommonAspectRatios100[] = {
    100,
    115,
    133,
    137,
    143,
    150,
    155,
    160,
    166,
    175,
    177,
    185,
    200,
    210,
    220,
    221,
    235,
    237,
    240,
    255,
    259,
    266,
    276,
    293,
    400,
    1200,
    kInfiniteRatio,
};

template <class T> // T has int width() & height() methods.
static void UmaHistogramAspectRatio(const char* name, const T& size)
{
    UMA_HISTOGRAM_CUSTOM_ENUMERATION(
        name,
        // Intentionally use integer division to truncate the result.
        size.height() ? (size.width() * 100) / size.height() : kInfiniteRatio,
        base::CustomHistogram::ArrayToCustomRanges(
            kCommonAspectRatios100, arraysize(kCommonAspectRatios100)));
}

// Record audio decoder config UMA stats corresponding to a src= playback.
static void RecordAudioCodecStats(const AudioDecoderConfig& audio_config)
{
    UMA_HISTOGRAM_ENUMERATION("Media.AudioCodec", audio_config.codec(),
        kAudioCodecMax + 1);
    UMA_HISTOGRAM_ENUMERATION("Media.AudioSampleFormat",
        audio_config.sample_format(), kSampleFormatMax + 1);
    UMA_HISTOGRAM_ENUMERATION("Media.AudioChannelLayout",
        audio_config.channel_layout(),
        CHANNEL_LAYOUT_MAX + 1);
    AudioSampleRate asr;
    if (ToAudioSampleRate(audio_config.samples_per_second(), &asr)) {
        UMA_HISTOGRAM_ENUMERATION("Media.AudioSamplesPerSecond", asr,
            kAudioSampleRateMax + 1);
    } else {
        UMA_HISTOGRAM_COUNTS("Media.AudioSamplesPerSecondUnexpected",
            audio_config.samples_per_second());
    }
}

// Record video decoder config UMA stats corresponding to a src= playback.
static void RecordVideoCodecStats(const VideoDecoderConfig& video_config,
    AVColorRange color_range)
{
    UMA_HISTOGRAM_ENUMERATION("Media.VideoCodec", video_config.codec(),
        kVideoCodecMax + 1);

    // Drop UNKNOWN because U_H_E() uses one bucket for all values less than 1.
    if (video_config.profile() >= 0) {
        UMA_HISTOGRAM_ENUMERATION("Media.VideoCodecProfile", video_config.profile(),
            VIDEO_CODEC_PROFILE_MAX + 1);
    }
    //     UMA_HISTOGRAM_COUNTS_10000("Media.VideoCodedWidth",
    //         video_config.coded_size().width());
    //     UmaHistogramAspectRatio("Media.VideoCodedAspectRatio",
    //         video_config.coded_size());
    //     UMA_HISTOGRAM_COUNTS_10000("Media.VideoVisibleWidth",
    //         video_config.visible_rect().width());
    //     UmaHistogramAspectRatio("Media.VideoVisibleAspectRatio",
    //         video_config.visible_rect());
    //     UMA_HISTOGRAM_ENUMERATION("Media.VideoPixelFormatUnion",
    //         video_config.format(), PIXEL_FORMAT_MAX + 1);
    //     UMA_HISTOGRAM_ENUMERATION("Media.VideoFrameColorSpace",
    //         video_config.color_space(), COLOR_SPACE_MAX + 1);

    // Note the PRESUBMIT_IGNORE_UMA_MAX below, this silences the PRESUBMIT.py
    // check for uma enum max usage, since we're abusing
    // UMA_HISTOGRAM_ENUMERATION to report a discrete value.
    UMA_HISTOGRAM_ENUMERATION("Media.VideoColorRange", color_range,
        AVCOL_RANGE_NB); // PRESUBMIT_IGNORE_UMA_MAX
}

static int32_t GetCodecHash(const AVCodecContext* context)
{
    if (context->codec_descriptor)
        return HashCodecName(context->codec_descriptor->name);
    const AVCodecDescriptor* codec_descriptor = avcodec_descriptor_get(context->codec_id);
    if (codec_descriptor)
        return HashCodecName(codec_descriptor->name);

    // If the codec name can't be determined, return none for tracking.
    return HashCodecName("none");
}

scoped_ptr<FFmpegDemuxerStream> FFmpegDemuxerStream::Create(
    FFmpegDemuxer* demuxer,
    AVStream* stream)
{
    if (!demuxer || !stream)
        return nullptr;

    scoped_ptr<FFmpegDemuxerStream> demuxer_stream;
    scoped_ptr<AudioDecoderConfig> audio_config;
    scoped_ptr<VideoDecoderConfig> video_config;

    if (stream->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
        audio_config.reset(new AudioDecoderConfig());

        // IsValidConfig() checks that the codec is supported and that the channel
        // layout and sample format are valid.
        //
        // TODO(chcunningham): Change AVStreamToAudioDecoderConfig to check
        // IsValidConfig internally and return a null scoped_ptr if not valid.
        if (!AVStreamToAudioDecoderConfig(stream, audio_config.get()) || !audio_config->IsValidConfig())
            return nullptr;

    } else if (stream->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
        video_config.reset(new VideoDecoderConfig());

        // IsValidConfig() checks that the codec is supported and that the channel
        // layout and sample format are valid.
        //
        // TODO(chcunningham): Change AVStreamToVideoDecoderConfig to check
        // IsValidConfig internally and return a null scoped_ptr if not valid.
        if (!AVStreamToVideoDecoderConfig(stream, video_config.get()) || !video_config->IsValidConfig())
            return nullptr;
    }

    return make_scoped_ptr(new FFmpegDemuxerStream(
        demuxer, stream, audio_config.Pass(), video_config.Pass()));
}

//
// FFmpegDemuxerStream
//
FFmpegDemuxerStream::FFmpegDemuxerStream(
    FFmpegDemuxer* demuxer,
    AVStream* stream,
    scoped_ptr<AudioDecoderConfig> audio_config,
    scoped_ptr<VideoDecoderConfig> video_config)
    : demuxer_(demuxer)
    , task_runner_(base::ThreadTaskRunnerHandle::Get())
    , stream_(stream)
    , audio_config_(audio_config.release())
    , video_config_(video_config.release())
    , type_(UNKNOWN)
    , liveness_(LIVENESS_UNKNOWN)
    , end_of_stream_(false)
    , last_packet_timestamp_(kNoTimestamp())
    , last_packet_duration_(kNoTimestamp())
    , video_rotation_(VIDEO_ROTATION_0)
    , fixup_negative_timestamps_(false)
{
    DCHECK(demuxer_);

    bool is_encrypted = false;
    int rotation = 0;
    AVDictionaryEntry* rotation_entry = NULL;

    // Determine our media format.
    switch (stream->codec->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        DCHECK(audio_config_.get() && !video_config_.get());
        type_ = AUDIO;
        is_encrypted = audio_config_->is_encrypted();
        break;
    case AVMEDIA_TYPE_VIDEO:
        DCHECK(video_config_.get() && !audio_config_.get());
        type_ = VIDEO;
        is_encrypted = video_config_->is_encrypted();

        rotation_entry = av_dict_get(stream->metadata, "rotate", NULL, 0);
        if (rotation_entry && rotation_entry->value && rotation_entry->value[0])
            base::StringToInt(rotation_entry->value, &rotation);

        switch (rotation) {
        case 0:
            break;
        case 90:
            video_rotation_ = VIDEO_ROTATION_90;
            break;
        case 180:
            video_rotation_ = VIDEO_ROTATION_180;
            break;
        case 270:
            video_rotation_ = VIDEO_ROTATION_270;
            break;
        default:
            LOG(ERROR) << "Unsupported video rotation metadata: " << rotation;
            break;
        }

        break;
    case AVMEDIA_TYPE_SUBTITLE:
        DCHECK(!video_config_.get() && !audio_config_.get());
        type_ = TEXT;
        break;
    default:
        NOTREACHED();
        break;
    }

    // Calculate the duration.
    duration_ = ConvertStreamTimestamp(stream->time_base, stream->duration);

    if (is_encrypted) {
        //         AVDictionaryEntry* key = av_dict_get(stream->metadata, "enc_key_id", NULL,
        //             0);
        //         DCHECK(key);
        //         DCHECK(key->value);
        //         if (!key || !key->value)
        //             return;
        //         base::StringPiece base64_key_id(key->value);
        //         std::string enc_key_id;
        //         base::Base64Decode(base64_key_id, &enc_key_id);
        //         DCHECK(!enc_key_id.empty());
        //         if (enc_key_id.empty())
        //             return;
        //
        //         encryption_key_id_.assign(enc_key_id);
        //         demuxer_->OnEncryptedMediaInitData(EmeInitDataType::WEBM, enc_key_id);
        DebugBreak();
    }
}

FFmpegDemuxerStream::~FFmpegDemuxerStream()
{
    DCHECK(!demuxer_);
    DCHECK(read_cb_.is_null());
    DCHECK(buffer_queue_.IsEmpty());
}

void FFmpegDemuxerStream::EnqueuePacket(ScopedAVPacket packet)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (!demuxer_ || end_of_stream_) {
        NOTREACHED() << "Attempted to enqueue packet on a stopped stream";
        return;
    }

#if defined(USE_PROPRIETARY_CODECS)
    // Convert the packet if there is a bitstream filter.
    if (packet->data && bitstream_converter_ && !bitstream_converter_->ConvertPacket(packet.get())) {
        LOG(ERROR) << "Format conversion failed.";
    }
#endif

    // Get side data if any. For now, the only type of side_data is VP8 Alpha. We
    // keep this generic so that other side_data types in the future can be
    // handled the same way as well.
    av_packet_split_side_data(packet.get());

    scoped_refptr<DecoderBuffer> buffer;

    if (type() == DemuxerStream::TEXT) {
        int id_size = 0;
        uint8* id_data = av_packet_get_side_data(
            packet.get(),
            AV_PKT_DATA_WEBVTT_IDENTIFIER,
            &id_size);

        int settings_size = 0;
        uint8* settings_data = av_packet_get_side_data(
            packet.get(),
            AV_PKT_DATA_WEBVTT_SETTINGS,
            &settings_size);

        std::vector<uint8> side_data;
        MakeSideData(id_data, id_data + id_size, settings_data, settings_data + settings_size, &side_data);

        buffer = DecoderBuffer::CopyFrom(packet.get()->data, packet.get()->size,
            side_data.data(), side_data.size());
    } else {
        int side_data_size = 0;
        uint8* side_data = av_packet_get_side_data(
            packet.get(),
            AV_PKT_DATA_MATROSKA_BLOCKADDITIONAL,
            &side_data_size);

        scoped_ptr<DecryptConfig> decrypt_config;
        int data_offset = 0;
        if ((type() == DemuxerStream::AUDIO && audio_config_->is_encrypted()) || (type() == DemuxerStream::VIDEO && video_config_->is_encrypted())) {
            if (!WebMCreateDecryptConfig(
                    packet->data, packet->size,
                    reinterpret_cast<const uint8*>(encryption_key_id_.data()),
                    encryption_key_id_.size(),
                    &decrypt_config,
                    &data_offset)) {
                LOG(ERROR) << "Creation of DecryptConfig failed.";
            }
        }

        // If a packet is returned by FFmpeg's av_parser_parse2() the packet will
        // reference inner memory of FFmpeg.  As such we should transfer the packet
        // into memory we control.
        if (side_data_size > 0) {
            buffer = DecoderBuffer::CopyFrom(packet.get()->data + data_offset,
                packet.get()->size - data_offset,
                side_data, side_data_size);
        } else {
            buffer = DecoderBuffer::CopyFrom(packet.get()->data + data_offset,
                packet.get()->size - data_offset);
        }

        int skip_samples_size = 0;
        const uint32* skip_samples_ptr = reinterpret_cast<const uint32*>(av_packet_get_side_data(
            packet.get(), AV_PKT_DATA_SKIP_SAMPLES, &skip_samples_size));
        const int kSkipSamplesValidSize = 10;
        const int kSkipEndSamplesOffset = 1;
        if (skip_samples_size >= kSkipSamplesValidSize) {
            // Because FFmpeg rolls codec delay and skip samples into one we can only
            // allow front discard padding on the first buffer.  Otherwise the discard
            // helper can't figure out which data to discard.  See AudioDiscardHelper.
            int discard_front_samples = base::ByteSwapToLE32(*skip_samples_ptr);
            if (last_packet_timestamp_ != kNoTimestamp() && discard_front_samples) {
                DLOG(ERROR) << "Skip samples are only allowed for the first packet.";
                discard_front_samples = 0;
            }

            const int discard_end_samples = base::ByteSwapToLE32(*(skip_samples_ptr + kSkipEndSamplesOffset));
            const int samples_per_second = audio_decoder_config().samples_per_second();
            buffer->set_discard_padding(std::make_pair(
                FramesToTimeDelta(discard_front_samples, samples_per_second),
                FramesToTimeDelta(discard_end_samples, samples_per_second)));
        }

        if (decrypt_config)
            buffer->set_decrypt_config(decrypt_config.Pass());
    }

    if (packet->duration >= 0) {
        buffer->set_duration(
            ConvertStreamTimestamp(stream_->time_base, packet->duration));
    } else {
        // TODO(wolenetz): Remove when FFmpeg stops returning negative durations.
        // https://crbug.com/394418
        DVLOG(1) << "FFmpeg returned a buffer with a negative duration! "
                 << packet->duration;
        buffer->set_duration(kNoTimestamp());
    }

    // Note: If pts is AV_NOPTS_VALUE, stream_timestamp will be kNoTimestamp().
    const base::TimeDelta stream_timestamp = ConvertStreamTimestamp(stream_->time_base, packet->pts);

    if (stream_timestamp != kNoTimestamp()) {
        const bool is_audio = type() == AUDIO;

        // If this file has negative timestamps don't rebase any other stream types
        // against the negative starting time.
        base::TimeDelta start_time = demuxer_->start_time();
        if (fixup_negative_timestamps_ && !is_audio && start_time < base::TimeDelta()) {
            start_time = base::TimeDelta();
        }

        // Don't rebase timestamps for positive start times, the HTML Media Spec
        // details this in section "4.8.10.6 Offsets into the media resource." We
        // will still need to rebase timestamps before seeking with FFmpeg though.
        if (start_time > base::TimeDelta())
            start_time = base::TimeDelta();

        buffer->set_timestamp(stream_timestamp - start_time);

        // If enabled, and no codec delay is present, mark audio packets with
        // negative timestamps for post-decode discard.
        if (fixup_negative_timestamps_ && is_audio && stream_timestamp < base::TimeDelta() && buffer->duration() != kNoTimestamp()) {
            if (!stream_->codec->delay) {
                DCHECK_EQ(buffer->discard_padding().first, base::TimeDelta());

                if (stream_timestamp + buffer->duration() < base::TimeDelta()) {
                    DCHECK_EQ(buffer->discard_padding().second, base::TimeDelta());

                    // Discard the entire packet if it's entirely before zero.
                    buffer->set_discard_padding(
                        std::make_pair(kInfiniteDuration(), base::TimeDelta()));
                } else {
                    // Only discard part of the frame if it overlaps zero.
                    buffer->set_discard_padding(std::make_pair(
                        -stream_timestamp, buffer->discard_padding().second));
                }
            } else {
                // Verify that codec delay would cover discard and that we don't need to
                // mark the packet for post decode discard.  Since timestamps may be in
                // milliseconds and codec delay in nanosecond precision, round up to the
                // nearest millisecond.  See enable_negative_timestamp_fixups().
                DCHECK_LE(-std::ceil(FramesToTimeDelta(
                              audio_decoder_config().codec_delay(),
                              audio_decoder_config().samples_per_second())
                                         .InMillisecondsF()),
                    stream_timestamp.InMillisecondsF());
            }
        }
    } else {
        // If this happens on the first packet, decoders will throw an error.
        buffer->set_timestamp(kNoTimestamp());
    }

    if (last_packet_timestamp_ != kNoTimestamp()) {
        // FFmpeg doesn't support chained ogg correctly.  Instead of guaranteeing
        // continuity across links in the chain it uses the timestamp information
        // from each link directly.  Doing so can lead to timestamps which appear to
        // go backwards in time.
        //
        // If the new link starts with a negative timestamp or a timestamp less than
        // the original (positive) |start_time|, we will get a negative timestamp
        // here.  It's also possible FFmpeg returns kNoTimestamp() here if it's not
        // able to work out a timestamp using the previous link and the next.
        //
        // Fixing chained ogg is non-trivial, so for now just reuse the last good
        // timestamp.  The decoder will rewrite the timestamps to be sample accurate
        // later.  See http://crbug.com/396864.
        if (fixup_negative_timestamps_ && (buffer->timestamp() == kNoTimestamp() || buffer->timestamp() < last_packet_timestamp_)) {
            buffer->set_timestamp(last_packet_timestamp_ + (last_packet_duration_ != kNoTimestamp() ? last_packet_duration_ : base::TimeDelta::FromMicroseconds(1)));
        }

        // The demuxer should always output positive timestamps.
        DCHECK(buffer->timestamp() >= base::TimeDelta());
        DCHECK(buffer->timestamp() != kNoTimestamp());

        if (last_packet_timestamp_ < buffer->timestamp()) {
            buffered_ranges_.Add(last_packet_timestamp_, buffer->timestamp());
            demuxer_->NotifyBufferingChanged();
        }
    }

    if (packet.get()->flags & AV_PKT_FLAG_KEY)
        buffer->set_is_key_frame(true);

    last_packet_timestamp_ = buffer->timestamp();
    last_packet_duration_ = buffer->duration();

    buffer_queue_.Push(buffer);
    SatisfyPendingRead();
}

void FFmpegDemuxerStream::SetEndOfStream()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    end_of_stream_ = true;
    SatisfyPendingRead();
}

void FFmpegDemuxerStream::FlushBuffers()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(read_cb_.is_null()) << "There should be no pending read";

    // H264 and AAC require that we resend the header after flush.
    // Reset bitstream for converter to do so.
    // This is related to chromium issue 140371 (http://crbug.com/140371).
    ResetBitstreamConverter();

    buffer_queue_.Clear();
    end_of_stream_ = false;
    last_packet_timestamp_ = kNoTimestamp();
    last_packet_duration_ = kNoTimestamp();
}

void FFmpegDemuxerStream::Stop()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    buffer_queue_.Clear();
    if (!read_cb_.is_null()) {
        base::ResetAndReturn(&read_cb_).Run(
            DemuxerStream::kOk, DecoderBuffer::CreateEOSBuffer());
    }
    demuxer_ = NULL;
    stream_ = NULL;
    end_of_stream_ = true;
}

DemuxerStream::Type FFmpegDemuxerStream::type() const
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    return type_;
}

DemuxerStream::Liveness FFmpegDemuxerStream::liveness() const
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    return liveness_;
}

void FFmpegDemuxerStream::Read(const ReadCB& read_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    CHECK(read_cb_.is_null()) << "Overlapping reads are not supported";
    read_cb_ = BindToCurrentLoop(read_cb);

    // Don't accept any additional reads if we've been told to stop.
    // The |demuxer_| may have been destroyed in the pipeline thread.
    //
    // TODO(scherkus): it would be cleaner to reply with an error message.
    if (!demuxer_) {
        base::ResetAndReturn(&read_cb_).Run(
            DemuxerStream::kOk, DecoderBuffer::CreateEOSBuffer());
        return;
    }

    SatisfyPendingRead();
}

void FFmpegDemuxerStream::EnableBitstreamConverter()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

#if defined(USE_PROPRIETARY_CODECS)
    InitBitstreamConverter();
#else
    NOTREACHED() << "Proprietary codecs not enabled.";
#endif
}

void FFmpegDemuxerStream::ResetBitstreamConverter()
{
#if defined(USE_PROPRIETARY_CODECS)
    if (bitstream_converter_)
        InitBitstreamConverter();
#endif // defined(USE_PROPRIETARY_CODECS)
}

void FFmpegDemuxerStream::InitBitstreamConverter()
{
#if defined(USE_PROPRIETARY_CODECS)
    if (stream_->codec->codec_id == AV_CODEC_ID_H264) {
        bitstream_converter_.reset(new FFmpegH264ToAnnexBBitstreamConverter(stream_->codec));
#if defined(ENABLE_HEVC_DEMUXING)
    } else if (stream_->codec->codec_id == AV_CODEC_ID_HEVC) {
        bitstream_converter_.reset(new FFmpegH265ToAnnexBBitstreamConverter(stream_->codec));
#endif
    } else if (stream_->codec->codec_id == AV_CODEC_ID_AAC) {
        bitstream_converter_.reset(new FFmpegAACBitstreamConverter(stream_->codec));
    }
#endif // defined(USE_PROPRIETARY_CODECS)
}

bool FFmpegDemuxerStream::SupportsConfigChanges() { return false; }

AudioDecoderConfig FFmpegDemuxerStream::audio_decoder_config()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(type_, AUDIO);
    DCHECK(audio_config_.get());
    return *audio_config_;
}

VideoDecoderConfig FFmpegDemuxerStream::video_decoder_config()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(type_, VIDEO);
    DCHECK(video_config_.get());
    return *video_config_;
}

VideoRotation FFmpegDemuxerStream::video_rotation()
{
    return video_rotation_;
}

void FFmpegDemuxerStream::SetLiveness(Liveness liveness)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(liveness_, LIVENESS_UNKNOWN);
    liveness_ = liveness;
}

base::TimeDelta FFmpegDemuxerStream::GetElapsedTime() const
{
    return ConvertStreamTimestamp(stream_->time_base, stream_->cur_dts);
}

Ranges<base::TimeDelta> FFmpegDemuxerStream::GetBufferedRanges() const
{
    return buffered_ranges_;
}

void FFmpegDemuxerStream::SatisfyPendingRead()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    if (!read_cb_.is_null()) {
        if (!buffer_queue_.IsEmpty()) {
            base::ResetAndReturn(&read_cb_).Run(
                DemuxerStream::kOk, buffer_queue_.Pop());
        } else if (end_of_stream_) {
            base::ResetAndReturn(&read_cb_).Run(
                DemuxerStream::kOk, DecoderBuffer::CreateEOSBuffer());
        }
    }

    // Have capacity? Ask for more!
    if (HasAvailableCapacity() && !end_of_stream_) {
        demuxer_->NotifyCapacityAvailable();
    }
}

bool FFmpegDemuxerStream::HasAvailableCapacity()
{
    // TODO(scherkus): Remove this return and reenable time-based capacity
    // after our data sources support canceling/concurrent reads, see
    // http://crbug.com/165762 for details.
#if 1
    return !read_cb_.is_null();
#else
    // Try to have one second's worth of encoded data per stream.
    const base::TimeDelta kCapacity = base::TimeDelta::FromSeconds(1);
    return buffer_queue_.IsEmpty() || buffer_queue_.Duration() < kCapacity;
#endif
}

size_t FFmpegDemuxerStream::MemoryUsage() const
{
    return buffer_queue_.data_size();
}

TextKind FFmpegDemuxerStream::GetTextKind() const
{
    DCHECK_EQ(type_, DemuxerStream::TEXT);

    if (stream_->disposition & AV_DISPOSITION_CAPTIONS)
        return kTextCaptions;

    if (stream_->disposition & AV_DISPOSITION_DESCRIPTIONS)
        return kTextDescriptions;

    if (stream_->disposition & AV_DISPOSITION_METADATA)
        return kTextMetadata;

    return kTextSubtitles;
}

std::string FFmpegDemuxerStream::GetMetadata(const char* key) const
{
    const AVDictionaryEntry* entry = av_dict_get(stream_->metadata, key, NULL, 0);
    return (entry == NULL || entry->value == NULL) ? "" : entry->value;
}

// static
base::TimeDelta FFmpegDemuxerStream::ConvertStreamTimestamp(
    const AVRational& time_base, int64 timestamp)
{
    if (timestamp == static_cast<int64>(AV_NOPTS_VALUE))
        return kNoTimestamp();

    return ConvertFromTimeBase(time_base, timestamp);
}

//
// FFmpegDemuxer
//
FFmpegDemuxer::FFmpegDemuxer(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    DataSource* data_source,
    const EncryptedMediaInitDataCB& encrypted_media_init_data_cb,
    const scoped_refptr<MediaLog>& media_log)
    : host_(NULL)
    , task_runner_(task_runner)
    , blocking_thread_("FFmpegDemuxer")
    , pending_read_(false)
    , pending_seek_(false)
    , data_source_(data_source)
    , media_log_(media_log)
    , bitrate_(0)
    , start_time_(kNoTimestamp())
    , preferred_stream_for_seeking_(-1, kNoTimestamp())
    , fallback_stream_for_seeking_(-1, kNoTimestamp())
    , text_enabled_(false)
    , duration_known_(false)
    , encrypted_media_init_data_cb_(encrypted_media_init_data_cb)
    , stream_memory_usage_(0)
    , weak_factory_(this)
{
    DCHECK(task_runner_.get());
    DCHECK(data_source_);
}

FFmpegDemuxer::~FFmpegDemuxer() { }

void FFmpegDemuxer::Stop()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // The order of Stop() and Abort() is important here.  If Abort() is called
    // first, control may pass into FFmpeg where it can destruct buffers that are
    // in the process of being fulfilled by the DataSource.
    data_source_->Stop();
    url_protocol_->Abort();

    // This will block until all tasks complete. Note that after this returns it's
    // possible for reply tasks (e.g., OnReadFrameDone()) to be queued on this
    // thread. Each of the reply task methods must check whether we've stopped the
    // thread and drop their results on the floor.
    blocking_thread_.Stop();

    StreamVector::iterator iter;
    for (iter = streams_.begin(); iter != streams_.end(); ++iter) {
        if (*iter)
            (*iter)->Stop();
    }

    data_source_ = NULL;
}

void FFmpegDemuxer::Seek(base::TimeDelta time, const PipelineStatusCB& cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    CHECK(!pending_seek_);

    // TODO(scherkus): Inspect |pending_read_| and cancel IO via |blocking_url_|,
    // otherwise we can end up waiting for a pre-seek read to complete even though
    // we know we're going to drop it on the floor.

    // FFmpeg requires seeks to be adjusted according to the lowest starting time.
    // Since EnqueuePacket() rebased negative timestamps by the start time, we
    // must correct the shift here.
    //
    // Additionally, to workaround limitations in how we expose seekable ranges to
    // Blink (http://crbug.com/137275), we also want to clamp seeks before the
    // start time to the start time.
    base::TimeDelta seek_time = start_time_ < base::TimeDelta()
        ? time + start_time_
        : time < start_time_ ? start_time_ : time;

    // When seeking in an opus stream we need to ensure we deliver enough data to
    // satisfy the seek preroll; otherwise the audio at the actual seek time will
    // not be entirely accurate.
    FFmpegDemuxerStream* audio_stream = GetFFmpegStream(DemuxerStream::AUDIO);
    if (audio_stream) {
        const AudioDecoderConfig& config = audio_stream->audio_decoder_config();
        if (config.codec() == kCodecOpus)
            seek_time = std::max(start_time_, seek_time - config.seek_preroll());
    }

    // Choose the seeking stream based on whether it contains the seek time, if no
    // match can be found prefer the preferred stream.
    //
    // TODO(dalecurtis): Currently FFmpeg does not ensure that all streams in a
    // given container will demux all packets after the seek point.  Instead it
    // only guarantees that all packets after the file position of the seek will
    // be demuxed.  It's an open question whether FFmpeg should fix this:
    // http://lists.ffmpeg.org/pipermail/ffmpeg-devel/2014-June/159212.html
    // Tracked by http://crbug.com/387996.
    DCHECK(preferred_stream_for_seeking_.second != kNoTimestamp());
    const int stream_index = seek_time < preferred_stream_for_seeking_.second && seek_time >= fallback_stream_for_seeking_.second
        ? fallback_stream_for_seeking_.first
        : preferred_stream_for_seeking_.first;
    DCHECK_NE(stream_index, -1);

    const AVStream* seeking_stream = glue_->format_context()->streams[stream_index];

    pending_seek_ = true;
    base::PostTaskAndReplyWithResult(
        blocking_thread_.task_runner().get(),
        FROM_HERE,
        base::Bind(&av_seek_frame,
            glue_->format_context(),
            seeking_stream->index,
            ConvertToTimeBase(seeking_stream->time_base, seek_time),
            // Always seek to a timestamp <= to the desired timestamp.
            AVSEEK_FLAG_BACKWARD),
        base::Bind(
            &FFmpegDemuxer::OnSeekFrameDone, weak_factory_.GetWeakPtr(), cb));
}

std::string FFmpegDemuxer::GetDisplayName() const
{
    return "FFmpegDemuxer";
}

void FFmpegDemuxer::Initialize(DemuxerHost* host,
    const PipelineStatusCB& status_cb,
    bool enable_text_tracks)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    host_ = host;
    text_enabled_ = enable_text_tracks;

    url_protocol_.reset(new BlockingUrlProtocol(data_source_, BindToCurrentLoop(base::Bind(&FFmpegDemuxer::OnDataSourceError, base::Unretained(this)))));
    glue_.reset(new FFmpegGlue(url_protocol_.get()));
    AVFormatContext* format_context = glue_->format_context();

    // Disable ID3v1 tag reading to avoid costly seeks to end of file for data we
    // don't use.  FFmpeg will only read ID3v1 tags if no other metadata is
    // available, so add a metadata entry to ensure some is always present.
    av_dict_set(&format_context->metadata, "skip_id3v1_tags", "", 0);

    // Ensure ffmpeg doesn't give up too early while looking for stream params;
    // this does not increase the amount of data downloaded.  The default value
    // is 5 AV_TIME_BASE units (1 second each), which prevents some oddly muxed
    // streams from being detected properly; this value was chosen arbitrarily.
    format_context->max_analyze_duration = 60 * AV_TIME_BASE;

    // Open the AVFormatContext using our glue layer.
    CHECK(blocking_thread_.Start());
    base::PostTaskAndReplyWithResult(
        blocking_thread_.task_runner().get(),
        FROM_HERE,
        base::Bind(&FFmpegGlue::OpenContext, base::Unretained(glue_.get())),
        base::Bind(&FFmpegDemuxer::OnOpenContextDone,
            weak_factory_.GetWeakPtr(),
            status_cb));
}

base::Time FFmpegDemuxer::GetTimelineOffset() const
{
    return timeline_offset_;
}

DemuxerStream* FFmpegDemuxer::GetStream(DemuxerStream::Type type)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    return GetFFmpegStream(type);
}

FFmpegDemuxerStream* FFmpegDemuxer::GetFFmpegStream(
    DemuxerStream::Type type) const
{
    StreamVector::const_iterator iter;
    for (iter = streams_.begin(); iter != streams_.end(); ++iter) {
        if (*iter && (*iter)->type() == type) {
            return *iter;
        }
    }
    return NULL;
}

base::TimeDelta FFmpegDemuxer::GetStartTime() const
{
    return std::max(start_time_, base::TimeDelta());
}

void FFmpegDemuxer::AddTextStreams()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    for (StreamVector::size_type idx = 0; idx < streams_.size(); ++idx) {
        FFmpegDemuxerStream* stream = streams_[idx];
        if (stream == NULL || stream->type() != DemuxerStream::TEXT)
            continue;

        TextKind kind = stream->GetTextKind();
        std::string title = stream->GetMetadata("title");
        std::string language = stream->GetMetadata("language");

        // TODO: Implement "id" metadata in FFMPEG.
        // See: http://crbug.com/323183
        host_->AddTextStream(stream, TextTrackConfig(kind, title, language, std::string()));
    }
}

int64_t FFmpegDemuxer::GetMemoryUsage() const
{
    base::AutoLock locker(stream_memory_usage_lock_);
    return stream_memory_usage_;
}

// Helper for calculating the bitrate of the media based on information stored
// in |format_context| or failing that the size and duration of the media.
//
// Returns 0 if a bitrate could not be determined.
static int CalculateBitrate(
    AVFormatContext* format_context,
    const base::TimeDelta& duration,
    int64 filesize_in_bytes)
{
    // If there is a bitrate set on the container, use it.
    if (format_context->bit_rate > 0)
        return format_context->bit_rate;

    // Then try to sum the bitrates individually per stream.
    int bitrate = 0;
    for (size_t i = 0; i < format_context->nb_streams; ++i) {
        AVCodecContext* codec_context = format_context->streams[i]->codec;
        bitrate += codec_context->bit_rate;
    }
    if (bitrate > 0)
        return bitrate;

    // See if we can approximate the bitrate as long as we have a filesize and
    // valid duration.
    if (duration.InMicroseconds() <= 0 || duration == kInfiniteDuration() || filesize_in_bytes == 0) {
        return 0;
    }

    // Do math in floating point as we'd overflow an int64 if the filesize was
    // larger than ~1073GB.
    double bytes = filesize_in_bytes;
    double duration_us = duration.InMicroseconds();
    return bytes * 8000000.0 / duration_us;
}

void FFmpegDemuxer::OnOpenContextDone(const PipelineStatusCB& status_cb,
    bool result)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    if (!blocking_thread_.IsRunning()) {
        MEDIA_LOG(ERROR, media_log_) << GetDisplayName() << ": bad state";
        status_cb.Run(PIPELINE_ERROR_ABORT);
        return;
    }

    if (!result) {
        MEDIA_LOG(ERROR, media_log_) << GetDisplayName() << ": open context failed";
        status_cb.Run(DEMUXER_ERROR_COULD_NOT_OPEN);
        return;
    }

    // Fully initialize AVFormatContext by parsing the stream a little.
    base::PostTaskAndReplyWithResult(
        blocking_thread_.task_runner().get(),
        FROM_HERE,
        base::Bind(&avformat_find_stream_info,
            glue_->format_context(),
            static_cast<AVDictionary**>(NULL)),
        base::Bind(&FFmpegDemuxer::OnFindStreamInfoDone,
            weak_factory_.GetWeakPtr(),
            status_cb));
}

void FFmpegDemuxer::OnFindStreamInfoDone(const PipelineStatusCB& status_cb,
    int result)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    if (!blocking_thread_.IsRunning() || !data_source_) {
        MEDIA_LOG(ERROR, media_log_) << GetDisplayName() << ": bad state";
        status_cb.Run(PIPELINE_ERROR_ABORT);
        return;
    }

    if (result < 0) {
        MEDIA_LOG(ERROR, media_log_) << GetDisplayName()
                                     << ": find stream info failed";
        status_cb.Run(DEMUXER_ERROR_COULD_NOT_PARSE);
        return;
    }

    // Create demuxer stream entries for each possible AVStream. Each stream
    // is examined to determine if it is supported or not (is the codec enabled
    // for it in this release?). Unsupported streams are skipped, allowing for
    // partial playback. At least one audio or video stream must be playable.
    AVFormatContext* format_context = glue_->format_context();
    streams_.resize(format_context->nb_streams);

    // Estimate the start time for each stream by looking through the packets
    // buffered during avformat_find_stream_info().  These values will be
    // considered later when determining the actual stream start time.
    //
    // These packets haven't been completely processed yet, so only look through
    // these values if the AVFormatContext has a valid start time.
    //
    // If no estimate is found, the stream entry will be kInfiniteDuration().
    std::vector<base::TimeDelta> start_time_estimates(format_context->nb_streams,
        kInfiniteDuration());
    const AVFormatInternal* internal = format_context->internal;
    if (internal && internal->packet_buffer && format_context->start_time != static_cast<int64>(AV_NOPTS_VALUE)) {
        struct AVPacketList* packet_buffer = internal->packet_buffer;
        while (packet_buffer != internal->packet_buffer_end) {
            DCHECK_LT(static_cast<size_t>(packet_buffer->pkt.stream_index),
                start_time_estimates.size());
            const AVStream* stream = format_context->streams[packet_buffer->pkt.stream_index];
            if (packet_buffer->pkt.pts != static_cast<int64>(AV_NOPTS_VALUE)) {
                const base::TimeDelta packet_pts = ConvertFromTimeBase(stream->time_base, packet_buffer->pkt.pts);
                if (packet_pts < start_time_estimates[stream->index])
                    start_time_estimates[stream->index] = packet_pts;
            }
            packet_buffer = packet_buffer->next;
        }
    }

    AVStream* audio_stream = NULL;
    AudioDecoderConfig audio_config;
    AVStream* video_stream = NULL;
    VideoDecoderConfig video_config;

    // If available, |start_time_| will be set to the lowest stream start time.
    start_time_ = kInfiniteDuration();

    base::TimeDelta max_duration;
    for (size_t i = 0; i < format_context->nb_streams; ++i) {
        AVStream* stream = format_context->streams[i];
        const AVCodecContext* codec_context = stream->codec;
        const AVMediaType codec_type = codec_context->codec_type;

        if (codec_type == AVMEDIA_TYPE_AUDIO) {
            if (audio_stream)
                continue;

            // Log the codec detected, whether it is supported or not.
            UMA_HISTOGRAM_SPARSE_SLOWLY("Media.DetectedAudioCodecHash",
                GetCodecHash(codec_context));
        } else if (codec_type == AVMEDIA_TYPE_VIDEO) {
            if (video_stream)
                continue;

#if defined(ENABLE_HEVC_DEMUXING)
            if (stream->codec->codec_id == AV_CODEC_ID_HEVC) {
                // If ffmpeg is built without HEVC parser/decoder support, it will be
                // able to demux HEVC based solely on container-provided information,
                // but unable to get some of the parameters without parsing the stream
                // (e.g. coded size needs to be read from SPS, pixel format is typically
                // deduced from decoder config in hvcC box). These are not really needed
                // when using external decoder (e.g. hardware decoder), so override them
                // here, to make sure this translates into a valid VideoDecoderConfig.
                if (stream->codec->coded_width == 0 && stream->codec->coded_height == 0) {
                    DCHECK(stream->codec->width > 0);
                    DCHECK(stream->codec->height > 0);
                    stream->codec->coded_width = stream->codec->width;
                    stream->codec->coded_height = stream->codec->height;
                }
                if (stream->codec->pix_fmt == AV_PIX_FMT_NONE) {
                    stream->codec->pix_fmt = AV_PIX_FMT_YUV420P;
                }
            }
#endif
            // Log the codec detected, whether it is supported or not.
            UMA_HISTOGRAM_SPARSE_SLOWLY("Media.DetectedVideoCodecHash",
                GetCodecHash(codec_context));
        } else if (codec_type == AVMEDIA_TYPE_SUBTITLE) {
            if (codec_context->codec_id != AV_CODEC_ID_WEBVTT || !text_enabled_) {
                continue;
            }
        } else {
            continue;
        }

        // Attempt to create a FFmpegDemuxerStream from the AVStream. This will
        // return nullptr if the AVStream is invalid. Validity checks will verify
        // things like: codec, channel layout, sample/pixel format, etc...
        scoped_ptr<FFmpegDemuxerStream> demuxer_stream = FFmpegDemuxerStream::Create(this, stream);
        if (demuxer_stream.get()) {
            streams_[i] = demuxer_stream.release();
        } else {
            // This AVStream does not successfully convert.
            continue;
        }

        // Note when we find our audio/video stream (we only want one of each) and
        // record src= playback UMA stats for the stream's decoder config.
        if (codec_type == AVMEDIA_TYPE_AUDIO) {
            CHECK(!audio_stream);
            audio_stream = stream;
            audio_config = streams_[i]->audio_decoder_config();
            RecordAudioCodecStats(audio_config);
        } else if (codec_type == AVMEDIA_TYPE_VIDEO) {
            CHECK(!video_stream);
            video_stream = stream;
            video_config = streams_[i]->video_decoder_config();
            RecordVideoCodecStats(video_config, stream->codec->color_range);
        }

        max_duration = std::max(max_duration, streams_[i]->duration());

        const base::TimeDelta start_time = ExtractStartTime(stream, start_time_estimates[i]);
        const bool has_start_time = start_time != kNoTimestamp();

        // Always prefer the video stream for seeking.  If none exists, we'll swap
        // the fallback stream with the preferred stream below.
        if (codec_type == AVMEDIA_TYPE_VIDEO) {
            preferred_stream_for_seeking_ = StreamSeekInfo(i, has_start_time ? start_time : base::TimeDelta());
        }

        if (!has_start_time)
            continue;

        if (start_time < start_time_) {
            start_time_ = start_time;

            // Choose the stream with the lowest starting time as the fallback stream
            // for seeking.  Video should always be preferred.
            fallback_stream_for_seeking_ = StreamSeekInfo(i, start_time);
        }
    }

    if (!audio_stream && !video_stream) {
        MEDIA_LOG(ERROR, media_log_) << GetDisplayName()
                                     << ": no supported streams";
        status_cb.Run(DEMUXER_ERROR_NO_SUPPORTED_STREAMS);
        return;
    }

    if (text_enabled_)
        AddTextStreams();

    if (format_context->duration != static_cast<int64_t>(AV_NOPTS_VALUE)) {
        // If there is a duration value in the container use that to find the
        // maximum between it and the duration from A/V streams.
        const AVRational av_time_base = { 1, AV_TIME_BASE };
        max_duration = std::max(max_duration,
            ConvertFromTimeBase(av_time_base, format_context->duration));
    } else {
        // The duration is unknown, in which case this is likely a live stream.
        max_duration = kInfiniteDuration();
    }

    // FFmpeg represents audio data marked as before the beginning of stream as
    // having negative timestamps.  This data must be discarded after it has been
    // decoded, not before since it is used to warmup the decoder.  There are
    // currently two known cases for this: vorbis in ogg and opus in ogg and webm.
    //
    // For API clarity, it was decided that the rest of the media pipeline should
    // not be exposed to negative timestamps.  Which means we need to rebase these
    // negative timestamps and mark them for discard post decoding.
    //
    // Post-decode frame dropping for packets with negative timestamps is outlined
    // in section A.2 in the Ogg Vorbis spec:
    // http://xiph.org/vorbis/doc/Vorbis_I_spec.html
    //
    // FFmpeg's use of negative timestamps for opus pre-skip is nonstandard, but
    // for more information on pre-skip see section 4.2 of the Ogg Opus spec:
    // https://tools.ietf.org/html/draft-ietf-codec-oggopus-08#section-4.2
    if (audio_stream && (audio_stream->codec->codec_id == AV_CODEC_ID_OPUS || (strcmp(format_context->iformat->name, "ogg") == 0 && audio_stream->codec->codec_id == AV_CODEC_ID_VORBIS))) {
        for (size_t i = 0; i < streams_.size(); ++i) {
            if (streams_[i])
                streams_[i]->enable_negative_timestamp_fixups();
        }

        // Fixup the seeking information to avoid selecting the audio stream simply
        // because it has a lower starting time.
        if (fallback_stream_for_seeking_.first == audio_stream->index && fallback_stream_for_seeking_.second < base::TimeDelta()) {
            fallback_stream_for_seeking_.second = base::TimeDelta();
        }
    }

    // If no start time could be determined, default to zero and prefer the video
    // stream over the audio stream for seeking.  E.g., The WAV demuxer does not
    // put timestamps on its frames.
    if (start_time_ == kInfiniteDuration()) {
        start_time_ = base::TimeDelta();
        preferred_stream_for_seeking_ = StreamSeekInfo(
            video_stream ? video_stream->index : audio_stream->index, start_time_);
    } else if (!video_stream) {
        // If no video stream exists, use the audio or text stream found above.
        preferred_stream_for_seeking_ = fallback_stream_for_seeking_;
    }

    // MPEG-4 B-frames cause grief for a simple container like AVI. Enable PTS
    // generation so we always get timestamps, see http://crbug.com/169570
    if (strcmp(format_context->iformat->name, "avi") == 0)
        format_context->flags |= AVFMT_FLAG_GENPTS;

    // For testing purposes, don't overwrite the timeline offset if set already.
    if (timeline_offset_.is_null())
        timeline_offset_ = ExtractTimelineOffset(format_context);

    // Since we're shifting the externally visible start time to zero, we need to
    // adjust the timeline offset to compensate.
    if (!timeline_offset_.is_null() && start_time_ < base::TimeDelta())
        timeline_offset_ += start_time_;

    if (max_duration == kInfiniteDuration() && !timeline_offset_.is_null()) {
        SetLiveness(DemuxerStream::LIVENESS_LIVE);
    } else if (max_duration != kInfiniteDuration()) {
        SetLiveness(DemuxerStream::LIVENESS_RECORDED);
    } else {
        SetLiveness(DemuxerStream::LIVENESS_UNKNOWN);
    }

    // Good to go: set the duration and bitrate and notify we're done
    // initializing.
    host_->SetDuration(max_duration);
    duration_known_ = (max_duration != kInfiniteDuration());

    int64 filesize_in_bytes = 0;
    url_protocol_->GetSize(&filesize_in_bytes);
    bitrate_ = CalculateBitrate(format_context, max_duration, filesize_in_bytes);
    if (bitrate_ > 0)
        data_source_->SetBitrate(bitrate_);

    // Audio logging
    if (audio_stream) {
        AVCodecContext* audio_codec = audio_stream->codec;
        media_log_->SetBooleanProperty("found_audio_stream", true);

        SampleFormat sample_format = audio_config.sample_format();
        std::string sample_name = SampleFormatToString(sample_format);

        media_log_->SetStringProperty("audio_sample_format", sample_name);

        AVCodec* codec = avcodec_find_decoder(audio_codec->codec_id);
        if (codec) {
            media_log_->SetStringProperty("audio_codec_name", codec->name);
        }

        media_log_->SetIntegerProperty("audio_channels_count",
            audio_codec->channels);
        media_log_->SetIntegerProperty("audio_samples_per_second",
            audio_config.samples_per_second());
    } else {
        media_log_->SetBooleanProperty("found_audio_stream", false);
    }

    // Video logging
    if (video_stream) {
        AVCodecContext* video_codec = video_stream->codec;
        media_log_->SetBooleanProperty("found_video_stream", true);

        AVCodec* codec = avcodec_find_decoder(video_codec->codec_id);
        if (codec) {
            media_log_->SetStringProperty("video_codec_name", codec->name);
        } else if (video_codec->codec_id == AV_CODEC_ID_VP9) {
            // ffmpeg doesn't know about VP9 decoder. So we need to log it explicitly.
            media_log_->SetStringProperty("video_codec_name", "vp9");
        }

        media_log_->SetIntegerProperty("width", video_codec->width);
        media_log_->SetIntegerProperty("height", video_codec->height);
        media_log_->SetIntegerProperty("coded_width",
            video_codec->coded_width);
        media_log_->SetIntegerProperty("coded_height",
            video_codec->coded_height);
        media_log_->SetStringProperty(
            "time_base",
            base::StringPrintf("%d/%d",
                video_codec->time_base.num,
                video_codec->time_base.den));
        media_log_->SetStringProperty(
            "video_format", VideoPixelFormatToString(video_config.format()));
        media_log_->SetBooleanProperty("video_is_encrypted",
            video_config.is_encrypted());
    } else {
        media_log_->SetBooleanProperty("found_video_stream", false);
    }

    media_log_->SetTimeProperty("max_duration", max_duration);
    media_log_->SetTimeProperty("start_time", start_time_);
    media_log_->SetIntegerProperty("bitrate", bitrate_);

    status_cb.Run(PIPELINE_OK);
}

void FFmpegDemuxer::OnSeekFrameDone(const PipelineStatusCB& cb, int result)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    CHECK(pending_seek_);
    pending_seek_ = false;

    if (!blocking_thread_.IsRunning()) {
        MEDIA_LOG(ERROR, media_log_) << GetDisplayName() << ": bad state";
        cb.Run(PIPELINE_ERROR_ABORT);
        return;
    }

    if (result < 0) {
        // Use VLOG(1) instead of NOTIMPLEMENTED() to prevent the message being
        // captured from stdout and contaminates testing.
        // TODO(scherkus): Implement this properly and signal error (BUG=23447).
        VLOG(1) << "Not implemented";
    }

    // Tell streams to flush buffers due to seeking.
    StreamVector::iterator iter;
    for (iter = streams_.begin(); iter != streams_.end(); ++iter) {
        if (*iter)
            (*iter)->FlushBuffers();
    }

    // Resume reading until capacity.
    ReadFrameIfNeeded();

    // Notify we're finished seeking.
    cb.Run(PIPELINE_OK);
}

void FFmpegDemuxer::ReadFrameIfNeeded()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // Make sure we have work to do before reading.
    if (!blocking_thread_.IsRunning() || !StreamsHaveAvailableCapacity() || pending_read_ || pending_seek_) {
        return;
    }

    // Allocate and read an AVPacket from the media. Save |packet_ptr| since
    // evaluation order of packet.get() and base::Passed(&packet) is
    // undefined.
    ScopedAVPacket packet(new AVPacket());
    AVPacket* packet_ptr = packet.get();

    pending_read_ = true;
    base::PostTaskAndReplyWithResult(
        blocking_thread_.task_runner().get(),
        FROM_HERE,
        base::Bind(&av_read_frame, glue_->format_context(), packet_ptr),
        base::Bind(&FFmpegDemuxer::OnReadFrameDone,
            weak_factory_.GetWeakPtr(),
            base::Passed(&packet)));
}

void FFmpegDemuxer::OnReadFrameDone(ScopedAVPacket packet, int result)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(pending_read_);
    pending_read_ = false;

    if (!blocking_thread_.IsRunning() || pending_seek_) {
        return;
    }

    // Max allowed memory usage, all streams combined.
    const int64_t kDemuxerMemoryLimit = 150 * 1024 * 1024;
    const bool is_max_memory_usage_reached = UpdateMemoryUsage() > kDemuxerMemoryLimit;

    // Consider the stream as ended if:
    // - either underlying ffmpeg returned an error
    // - or FFMpegDemuxer reached the maximum allowed memory usage.
    if (result < 0 || is_max_memory_usage_reached) {
        // Update the duration based on the highest elapsed time across all streams
        // if it was previously unknown.
        if (!duration_known_) {
            base::TimeDelta max_duration;

            for (StreamVector::iterator iter = streams_.begin();
                 iter != streams_.end();
                 ++iter) {
                if (!*iter)
                    continue;

                base::TimeDelta duration = (*iter)->GetElapsedTime();
                if (duration != kNoTimestamp() && duration > max_duration)
                    max_duration = duration;
            }

            if (max_duration > base::TimeDelta()) {
                host_->SetDuration(max_duration);
                duration_known_ = true;
            }
        }
        // If we have reached the end of stream, tell the downstream filters about
        // the event.
        StreamHasEnded();
        return;
    }

    // Queue the packet with the appropriate stream.
    DCHECK_GE(packet->stream_index, 0);
    DCHECK_LT(packet->stream_index, static_cast<int>(streams_.size()));

    // Defend against ffmpeg giving us a bad stream index.
    if (packet->stream_index >= 0 && packet->stream_index < static_cast<int>(streams_.size()) && streams_[packet->stream_index]) {
        // TODO(scherkus): Fix demuxing upstream to never return packets w/o data
        // when av_read_frame() returns success code. See bug comment for ideas:
        //
        // https://code.google.com/p/chromium/issues/detail?id=169133#c10
        if (!packet->data) {
            ScopedAVPacket new_packet(new AVPacket());
            av_new_packet(new_packet.get(), 0);
            av_packet_copy_props(new_packet.get(), packet.get());
            packet.swap(new_packet);
        }

        FFmpegDemuxerStream* demuxer_stream = streams_[packet->stream_index];
        demuxer_stream->EnqueuePacket(packet.Pass());
    }

    // Keep reading until we've reached capacity.
    ReadFrameIfNeeded();
}

bool FFmpegDemuxer::StreamsHaveAvailableCapacity()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    StreamVector::iterator iter;
    for (iter = streams_.begin(); iter != streams_.end(); ++iter) {
        if (*iter && (*iter)->HasAvailableCapacity()) {
            return true;
        }
    }
    return false;
}

int64_t FFmpegDemuxer::UpdateMemoryUsage()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    base::AutoLock locker(stream_memory_usage_lock_);
    stream_memory_usage_ = 0;
    for (const auto& stream : streams_) {
        if (stream)
            stream_memory_usage_ += stream->MemoryUsage();
    }
    return stream_memory_usage_;
}

void FFmpegDemuxer::StreamHasEnded()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    StreamVector::iterator iter;
    for (iter = streams_.begin(); iter != streams_.end(); ++iter) {
        if (!*iter)
            continue;
        (*iter)->SetEndOfStream();
    }
}

void FFmpegDemuxer::OnEncryptedMediaInitData(
    EmeInitDataType init_data_type,
    const std::string& encryption_key_id)
{
    //std::vector<uint8> key_id_local(encryption_key_id.begin(), encryption_key_id.end());
    std::vector<uint8> key_id_local(encryption_key_id.size());
    memcpy(key_id_local.data(), encryption_key_id.c_str(), encryption_key_id.size());

    encrypted_media_init_data_cb_.Run(init_data_type, key_id_local);
}

void FFmpegDemuxer::NotifyCapacityAvailable()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    ReadFrameIfNeeded();
}

void FFmpegDemuxer::NotifyBufferingChanged()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    Ranges<base::TimeDelta> buffered;
    FFmpegDemuxerStream* audio = GetFFmpegStream(DemuxerStream::AUDIO);
    FFmpegDemuxerStream* video = GetFFmpegStream(DemuxerStream::VIDEO);
    if (audio && video) {
        buffered = audio->GetBufferedRanges().IntersectionWith(
            video->GetBufferedRanges());
    } else if (audio) {
        buffered = audio->GetBufferedRanges();
    } else if (video) {
        buffered = video->GetBufferedRanges();
    }
    for (size_t i = 0; i < buffered.size(); ++i)
        host_->AddBufferedTimeRange(buffered.start(i), buffered.end(i));
}

void FFmpegDemuxer::OnDataSourceError()
{
    MEDIA_LOG(ERROR, media_log_) << GetDisplayName() << ": data source error";
    host_->OnDemuxerError(PIPELINE_ERROR_READ);
}

void FFmpegDemuxer::SetLiveness(DemuxerStream::Liveness liveness)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    for (const auto& stream : streams_) { // |stream| is a ref to a pointer.
        if (stream)
            stream->SetLiveness(liveness);
    }
}

} // namespace media
