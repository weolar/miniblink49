// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/formats/webm/webm_stream_parser.h"

#include <string>

#include "base/callback.h"
#include "base/callback_helpers.h"
#include "base/logging.h"
#include "media/base/timestamp_constants.h"
#include "media/formats/webm/webm_cluster_parser.h"
#include "media/formats/webm/webm_constants.h"
#include "media/formats/webm/webm_content_encodings.h"
#include "media/formats/webm/webm_info_parser.h"
#include "media/formats/webm/webm_tracks_parser.h"

namespace media {

WebMStreamParser::WebMStreamParser()
    : state_(kWaitingForInit)
    , unknown_segment_size_(false)
{
}

WebMStreamParser::~WebMStreamParser()
{
}

void WebMStreamParser::Init(
    const InitCB& init_cb,
    const NewConfigCB& config_cb,
    const NewBuffersCB& new_buffers_cb,
    bool ignore_text_tracks,
    const EncryptedMediaInitDataCB& encrypted_media_init_data_cb,
    const NewMediaSegmentCB& new_segment_cb,
    const base::Closure& end_of_segment_cb,
    const scoped_refptr<MediaLog>& media_log)
{
    DCHECK_EQ(state_, kWaitingForInit);
    DCHECK(init_cb_.is_null());
    DCHECK(!init_cb.is_null());
    DCHECK(!config_cb.is_null());
    DCHECK(!new_buffers_cb.is_null());
    DCHECK(!encrypted_media_init_data_cb.is_null());
    DCHECK(!new_segment_cb.is_null());
    DCHECK(!end_of_segment_cb.is_null());

    ChangeState(kParsingHeaders);
    init_cb_ = init_cb;
    config_cb_ = config_cb;
    new_buffers_cb_ = new_buffers_cb;
    ignore_text_tracks_ = ignore_text_tracks;
    encrypted_media_init_data_cb_ = encrypted_media_init_data_cb;
    new_segment_cb_ = new_segment_cb;
    end_of_segment_cb_ = end_of_segment_cb;
    media_log_ = media_log;
}

void WebMStreamParser::Flush()
{
    DCHECK_NE(state_, kWaitingForInit);

    byte_queue_.Reset();
    if (cluster_parser_)
        cluster_parser_->Reset();
    if (state_ == kParsingClusters) {
        ChangeState(kParsingHeaders);
        end_of_segment_cb_.Run();
    }
}

bool WebMStreamParser::Parse(const uint8* buf, int size)
{
    DCHECK_NE(state_, kWaitingForInit);

    if (state_ == kError)
        return false;

    byte_queue_.Push(buf, size);

    int result = 0;
    int bytes_parsed = 0;
    const uint8* cur = NULL;
    int cur_size = 0;

    byte_queue_.Peek(&cur, &cur_size);
    while (cur_size > 0) {
        State oldState = state_;
        switch (state_) {
        case kParsingHeaders:
            result = ParseInfoAndTracks(cur, cur_size);
            break;

        case kParsingClusters:
            result = ParseCluster(cur, cur_size);
            break;

        case kWaitingForInit:
        case kError:
            return false;
        }

        if (result < 0) {
            ChangeState(kError);
            return false;
        }

        if (state_ == oldState && result == 0)
            break;

        DCHECK_GE(result, 0);
        cur += result;
        cur_size -= result;
        bytes_parsed += result;
    }

    byte_queue_.Pop(bytes_parsed);
    return true;
}

void WebMStreamParser::ChangeState(State new_state)
{
    DVLOG(1) << "ChangeState() : " << state_ << " -> " << new_state;
    state_ = new_state;
}

int WebMStreamParser::ParseInfoAndTracks(const uint8* data, int size)
{
    DVLOG(2) << "ParseInfoAndTracks()";
    DCHECK(data);
    DCHECK_GT(size, 0);

    const uint8* cur = data;
    int cur_size = size;
    int bytes_parsed = 0;

    int id;
    int64 element_size;
    int result = WebMParseElementHeader(cur, cur_size, &id, &element_size);

    if (result <= 0)
        return result;

    switch (id) {
    case kWebMIdEBMLHeader:
    case kWebMIdSeekHead:
    case kWebMIdVoid:
    case kWebMIdCRC32:
    case kWebMIdCues:
    case kWebMIdChapters:
    case kWebMIdTags:
    case kWebMIdAttachments:
        // TODO(matthewjheaney): Implement support for chapters.
        if (cur_size < (result + element_size)) {
            // We don't have the whole element yet. Signal we need more data.
            return 0;
        }
        // Skip the element.
        return result + element_size;
        break;
    case kWebMIdCluster:
        if (!cluster_parser_) {
            MEDIA_LOG(ERROR, media_log_) << "Found Cluster element before Info.";
            return -1;
        }
        ChangeState(kParsingClusters);
        new_segment_cb_.Run();
        return 0;
        break;
    case kWebMIdSegment:
        // Segment of unknown size indicates live stream.
        if (element_size == kWebMUnknownSize)
            unknown_segment_size_ = true;
        // Just consume the segment header.
        return result;
        break;
    case kWebMIdInfo:
        // We've found the element we are looking for.
        break;
    default: {
        MEDIA_LOG(ERROR, media_log_) << "Unexpected element ID 0x" << std::hex
                                     << id;
        return -1;
    }
    }

    WebMInfoParser info_parser;
    result = info_parser.Parse(cur, cur_size);

    if (result <= 0)
        return result;

    cur += result;
    cur_size -= result;
    bytes_parsed += result;

    WebMTracksParser tracks_parser(media_log_, ignore_text_tracks_);
    result = tracks_parser.Parse(cur, cur_size);

    if (result <= 0)
        return result;

    bytes_parsed += result;

    double timecode_scale_in_us = info_parser.timecode_scale() / 1000.0;
    InitParameters params(kInfiniteDuration());

    if (info_parser.duration() > 0) {
        int64 duration_in_us = info_parser.duration() * timecode_scale_in_us;
        params.duration = base::TimeDelta::FromMicroseconds(duration_in_us);
    }

    params.timeline_offset = info_parser.date_utc();

    if (unknown_segment_size_ && (info_parser.duration() <= 0) && !info_parser.date_utc().is_null()) {
        params.liveness = DemuxerStream::LIVENESS_LIVE;
    } else if (info_parser.duration() >= 0) {
        params.liveness = DemuxerStream::LIVENESS_RECORDED;
    } else {
        params.liveness = DemuxerStream::LIVENESS_UNKNOWN;
    }

    const AudioDecoderConfig& audio_config = tracks_parser.audio_decoder_config();
    if (audio_config.is_encrypted())
        OnEncryptedMediaInitData(tracks_parser.audio_encryption_key_id());

    const VideoDecoderConfig& video_config = tracks_parser.video_decoder_config();
    if (video_config.is_encrypted())
        OnEncryptedMediaInitData(tracks_parser.video_encryption_key_id());

    if (!config_cb_.Run(audio_config,
            video_config,
            tracks_parser.text_tracks())) {
        DVLOG(1) << "New config data isn't allowed.";
        return -1;
    }

    cluster_parser_.reset(new WebMClusterParser(
        info_parser.timecode_scale(), tracks_parser.audio_track_num(),
        tracks_parser.GetAudioDefaultDuration(timecode_scale_in_us),
        tracks_parser.video_track_num(),
        tracks_parser.GetVideoDefaultDuration(timecode_scale_in_us),
        tracks_parser.text_tracks(), tracks_parser.ignored_tracks(),
        tracks_parser.audio_encryption_key_id(),
        tracks_parser.video_encryption_key_id(), audio_config.codec(),
        media_log_));

    if (!init_cb_.is_null())
        base::ResetAndReturn(&init_cb_).Run(params);

    return bytes_parsed;
}

int WebMStreamParser::ParseCluster(const uint8* data, int size)
{
    if (!cluster_parser_)
        return -1;

    int bytes_parsed = cluster_parser_->Parse(data, size);
    if (bytes_parsed < 0)
        return bytes_parsed;

    const BufferQueue& audio_buffers = cluster_parser_->GetAudioBuffers();
    const BufferQueue& video_buffers = cluster_parser_->GetVideoBuffers();
    const TextBufferQueueMap& text_map = cluster_parser_->GetTextBuffers();

    bool cluster_ended = cluster_parser_->cluster_ended();

    if ((!audio_buffers.empty() || !video_buffers.empty() || !text_map.empty()) && !new_buffers_cb_.Run(audio_buffers, video_buffers, text_map)) {
        return -1;
    }

    if (cluster_ended) {
        ChangeState(kParsingHeaders);
        end_of_segment_cb_.Run();
    }

    return bytes_parsed;
}

void WebMStreamParser::OnEncryptedMediaInitData(const std::string& key_id)
{
    //std::vector<uint8> key_id_vector(key_id.begin(), key_id.end());
    std::vector<uint8> key_id_vector(key_id.size());
    memcpy(key_id_vector.data(), key_id.c_str(), key_id.size());
    encrypted_media_init_data_cb_.Run(EmeInitDataType::WEBM, key_id_vector);
}

} // namespace media
