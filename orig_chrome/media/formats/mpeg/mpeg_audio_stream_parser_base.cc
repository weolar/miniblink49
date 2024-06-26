// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/formats/mpeg/mpeg_audio_stream_parser_base.h"

#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/message_loop/message_loop.h"
#include "media/base/stream_parser_buffer.h"
#include "media/base/text_track_config.h"
#include "media/base/timestamp_constants.h"
#include "media/base/video_decoder_config.h"

namespace media {

static const uint32 kICYStartCode = 0x49435920; // 'ICY '

// Arbitrary upper bound on the size of an IceCast header before it
// triggers an error.
static const int kMaxIcecastHeaderSize = 4096;

static const uint32 kID3StartCodeMask = 0xffffff00;
static const uint32 kID3v1StartCode = 0x54414700; // 'TAG\0'
static const int kID3v1Size = 128;
static const int kID3v1ExtendedSize = 227;
static const uint32 kID3v2StartCode = 0x49443300; // 'ID3\0'

static int LocateEndOfHeaders(const uint8_t* buf, int buf_len, int i)
{
    bool was_lf = false;
    char last_c = '\0';
    for (; i < buf_len; ++i) {
        char c = buf[i];
        if (c == '\n') {
            if (was_lf)
                return i + 1;
            was_lf = true;
        } else if (c != '\r' || last_c != '\n') {
            was_lf = false;
        }
        last_c = c;
    }
    return -1;
}

MPEGAudioStreamParserBase::MPEGAudioStreamParserBase(uint32 start_code_mask,
    AudioCodec audio_codec,
    int codec_delay)
    : state_(UNINITIALIZED)
    , in_media_segment_(false)
    , start_code_mask_(start_code_mask)
    , audio_codec_(audio_codec)
    , codec_delay_(codec_delay)
{
}

MPEGAudioStreamParserBase::~MPEGAudioStreamParserBase() { }

void MPEGAudioStreamParserBase::Init(
    const InitCB& init_cb,
    const NewConfigCB& config_cb,
    const NewBuffersCB& new_buffers_cb,
    bool ignore_text_tracks,
    const EncryptedMediaInitDataCB& encrypted_media_init_data_cb,
    const NewMediaSegmentCB& new_segment_cb,
    const base::Closure& end_of_segment_cb,
    const scoped_refptr<MediaLog>& media_log)
{
    DVLOG(1) << __FUNCTION__;
    DCHECK_EQ(state_, UNINITIALIZED);
    init_cb_ = init_cb;
    config_cb_ = config_cb;
    new_buffers_cb_ = new_buffers_cb;
    new_segment_cb_ = new_segment_cb;
    end_of_segment_cb_ = end_of_segment_cb;
    media_log_ = media_log;

    ChangeState(INITIALIZED);
}

void MPEGAudioStreamParserBase::Flush()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK_NE(state_, UNINITIALIZED);
    queue_.Reset();
    if (timestamp_helper_)
        timestamp_helper_->SetBaseTimestamp(base::TimeDelta());
    in_media_segment_ = false;
}

bool MPEGAudioStreamParserBase::Parse(const uint8* buf, int size)
{
    DVLOG(1) << __FUNCTION__ << "(" << size << ")";
    DCHECK(buf);
    DCHECK_GT(size, 0);
    DCHECK_NE(state_, UNINITIALIZED);

    if (state_ == PARSE_ERROR)
        return false;

    DCHECK_EQ(state_, INITIALIZED);

    queue_.Push(buf, size);

    bool end_of_segment = true;
    BufferQueue buffers;
    for (;;) {
        const uint8* data;
        int data_size;
        queue_.Peek(&data, &data_size);

        if (data_size < 4)
            break;

        uint32 start_code = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
        int bytes_read = 0;
        bool parsed_metadata = true;
        if ((start_code & start_code_mask_) == start_code_mask_) {
            bytes_read = ParseFrame(data, data_size, &buffers);

            // Only allow the current segment to end if a full frame has been parsed.
            end_of_segment = bytes_read > 0;
            parsed_metadata = false;
        } else if (start_code == kICYStartCode) {
            bytes_read = ParseIcecastHeader(data, data_size);
        } else if ((start_code & kID3StartCodeMask) == kID3v1StartCode) {
            bytes_read = ParseID3v1(data, data_size);
        } else if ((start_code & kID3StartCodeMask) == kID3v2StartCode) {
            bytes_read = ParseID3v2(data, data_size);
        } else {
            bytes_read = FindNextValidStartCode(data, data_size);

            if (bytes_read > 0) {
                DVLOG(1) << "Unexpected start code 0x" << std::hex << start_code;
                DVLOG(1) << "SKIPPING " << bytes_read << " bytes of garbage.";
            }
        }

        CHECK_LE(bytes_read, data_size);

        if (bytes_read < 0) {
            ChangeState(PARSE_ERROR);
            return false;
        } else if (bytes_read == 0) {
            // Need more data.
            break;
        }

        // Send pending buffers if we have encountered metadata.
        if (parsed_metadata && !buffers.empty() && !SendBuffers(&buffers, true))
            return false;

        queue_.Pop(bytes_read);
        end_of_segment = true;
    }

    if (buffers.empty())
        return true;

    // Send buffers collected in this append that haven't been sent yet.
    return SendBuffers(&buffers, end_of_segment);
}

void MPEGAudioStreamParserBase::ChangeState(State state)
{
    DVLOG(1) << __FUNCTION__ << "() : " << state_ << " -> " << state;
    state_ = state;
}

int MPEGAudioStreamParserBase::ParseFrame(const uint8* data,
    int size,
    BufferQueue* buffers)
{
    DVLOG(2) << __FUNCTION__ << "(" << size << ")";

    int sample_rate;
    ChannelLayout channel_layout;
    int frame_size;
    int sample_count;
    bool metadata_frame = false;
    int bytes_read = ParseFrameHeader(data,
        size,
        &frame_size,
        &sample_rate,
        &channel_layout,
        &sample_count,
        &metadata_frame);

    if (bytes_read <= 0)
        return bytes_read;

    // Make sure data contains the entire frame.
    if (size < frame_size)
        return 0;

    DVLOG(2) << " sample_rate " << sample_rate
             << " channel_layout " << channel_layout
             << " frame_size " << frame_size
             << " sample_count " << sample_count;

    if (config_.IsValidConfig() && (config_.samples_per_second() != sample_rate || config_.channel_layout() != channel_layout)) {
        // Clear config data so that a config change is initiated.
        config_ = AudioDecoderConfig();

        // Send all buffers associated with the previous config.
        if (!buffers->empty() && !SendBuffers(buffers, true))
            return -1;
    }

    if (!config_.IsValidConfig()) {
        config_.Initialize(audio_codec_,
            kSampleFormatF32,
            channel_layout,
            sample_rate,
            std::vector<uint8_t>(),
            false,
            base::TimeDelta(),
            codec_delay_);

        base::TimeDelta base_timestamp;
        if (timestamp_helper_)
            base_timestamp = timestamp_helper_->GetTimestamp();

        timestamp_helper_.reset(new AudioTimestampHelper(sample_rate));
        timestamp_helper_->SetBaseTimestamp(base_timestamp);

        VideoDecoderConfig video_config;
        if (!config_cb_.Run(config_, video_config, TextTrackConfigMap()))
            return -1;

        if (!init_cb_.is_null()) {
            InitParameters params(kInfiniteDuration());
            params.auto_update_timestamp_offset = true;
            base::ResetAndReturn(&init_cb_).Run(params);
        }
    }

    if (metadata_frame)
        return frame_size;

    // TODO(wolenetz/acolwell): Validate and use a common cross-parser TrackId
    // type and allow multiple audio tracks, if applicable. See
    // https://crbug.com/341581.
    scoped_refptr<StreamParserBuffer> buffer = StreamParserBuffer::CopyFrom(data, frame_size, true,
        DemuxerStream::AUDIO, 0);
    buffer->set_timestamp(timestamp_helper_->GetTimestamp());
    buffer->set_duration(timestamp_helper_->GetFrameDuration(sample_count));
    buffers->push_back(buffer);

    timestamp_helper_->AddFrames(sample_count);

    return frame_size;
}

int MPEGAudioStreamParserBase::ParseIcecastHeader(const uint8* data, int size)
{
    DVLOG(1) << __FUNCTION__ << "(" << size << ")";

    if (size < 4)
        return 0;

    if (memcmp("ICY ", data, 4))
        return -1;

    int locate_size = std::min(size, kMaxIcecastHeaderSize);
    int offset = LocateEndOfHeaders(data, locate_size, 4);
    if (offset < 0) {
        if (locate_size == kMaxIcecastHeaderSize) {
            MEDIA_LOG(ERROR, media_log_) << "Icecast header is too large.";
            return -1;
        }

        return 0;
    }

    return offset;
}

int MPEGAudioStreamParserBase::ParseID3v1(const uint8* data, int size)
{
    DVLOG(1) << __FUNCTION__ << "(" << size << ")";

    if (size < kID3v1Size)
        return 0;

    // TODO(acolwell): Add code to actually validate ID3v1 data and
    // expose it as a metadata text track.
    return !memcmp(data, "TAG+", 4) ? kID3v1ExtendedSize : kID3v1Size;
}

int MPEGAudioStreamParserBase::ParseID3v2(const uint8* data, int size)
{
    DVLOG(1) << __FUNCTION__ << "(" << size << ")";

    if (size < 10)
        return 0;

    BitReader reader(data, size);
    int32 id;
    int version;
    uint8 flags;
    int32 id3_size;

    if (!reader.ReadBits(24, &id) || !reader.ReadBits(16, &version) || !reader.ReadBits(8, &flags) || !ParseSyncSafeInt(&reader, &id3_size)) {
        return -1;
    }

    int32 actual_tag_size = 10 + id3_size;

    // Increment size if 'Footer present' flag is set.
    if (flags & 0x10)
        actual_tag_size += 10;

    // Make sure we have the entire tag.
    if (size < actual_tag_size)
        return 0;

    // TODO(acolwell): Add code to actually validate ID3v2 data and
    // expose it as a metadata text track.
    return actual_tag_size;
}

bool MPEGAudioStreamParserBase::ParseSyncSafeInt(BitReader* reader,
    int32* value)
{
    *value = 0;
    for (int i = 0; i < 4; ++i) {
        uint8 tmp;
        if (!reader->ReadBits(1, &tmp) || tmp != 0) {
            MEDIA_LOG(ERROR, media_log_) << "ID3 syncsafe integer byte MSb is not 0!";
            return false;
        }

        if (!reader->ReadBits(7, &tmp))
            return false;

        *value <<= 7;
        *value += tmp;
    }

    return true;
}

int MPEGAudioStreamParserBase::FindNextValidStartCode(const uint8* data,
    int size) const
{
    const uint8* start = data;
    const uint8* end = data + size;

    while (start < end) {
        int bytes_left = end - start;
        const uint8* candidate_start_code = static_cast<const uint8*>(memchr(start, 0xff, bytes_left));

        if (!candidate_start_code)
            return 0;

        bool parse_header_failed = false;
        const uint8* sync = candidate_start_code;
        // Try to find 3 valid frames in a row. 3 was selected to decrease
        // the probability of false positives.
        for (int i = 0; i < 3; ++i) {
            int sync_size = end - sync;
            int frame_size;
            int sync_bytes = ParseFrameHeader(
                sync, sync_size, &frame_size, NULL, NULL, NULL, NULL);

            if (sync_bytes == 0)
                return 0;

            if (sync_bytes > 0) {
                DCHECK_LT(sync_bytes, sync_size);

                // Skip over this frame so we can check the next one.
                sync += frame_size;

                // Make sure the next frame starts inside the buffer.
                if (sync >= end)
                    return 0;
            } else {
                DVLOG(1) << "ParseFrameHeader() " << i << " failed @" << (sync - data);
                parse_header_failed = true;
                break;
            }
        }

        if (parse_header_failed) {
            // One of the frame header parses failed so |candidate_start_code|
            // did not point to the start of a real frame. Move |start| forward
            // so we can find the next candidate.
            start = candidate_start_code + 1;
            continue;
        }

        return candidate_start_code - data;
    }

    return 0;
}

bool MPEGAudioStreamParserBase::SendBuffers(BufferQueue* buffers,
    bool end_of_segment)
{
    DCHECK(!buffers->empty());

    if (!in_media_segment_) {
        in_media_segment_ = true;
        new_segment_cb_.Run();
    }

    BufferQueue empty_video_buffers;
    TextBufferQueueMap empty_text_map;
    if (!new_buffers_cb_.Run(*buffers, empty_video_buffers, empty_text_map))
        return false;
    buffers->clear();

    if (end_of_segment) {
        in_media_segment_ = false;
        end_of_segment_cb_.Run();
    }

    timestamp_helper_->SetBaseTimestamp(base::TimeDelta());
    return true;
}

} // namespace media
