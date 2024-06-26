// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/formats/webm/webm_tracks_parser.h"

#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "media/base/timestamp_constants.h"
#include "media/formats/webm/webm_constants.h"
#include "media/formats/webm/webm_content_encodings.h"

namespace media {

static TextKind CodecIdToTextKind(const std::string& codec_id)
{
    if (codec_id == kWebMCodecSubtitles)
        return kTextSubtitles;

    if (codec_id == kWebMCodecCaptions)
        return kTextCaptions;

    if (codec_id == kWebMCodecDescriptions)
        return kTextDescriptions;

    if (codec_id == kWebMCodecMetadata)
        return kTextMetadata;

    return kTextNone;
}

static base::TimeDelta PrecisionCappedDefaultDuration(
    const double timecode_scale_in_us, const int64 duration_in_ns)
{
    if (duration_in_ns <= 0)
        return kNoTimestamp();

    int64 mult = duration_in_ns / 1000;
    mult /= timecode_scale_in_us;
    if (mult == 0)
        return kNoTimestamp();

    mult = static_cast<double>(mult) * timecode_scale_in_us;
    return base::TimeDelta::FromMicroseconds(mult);
}

WebMTracksParser::WebMTracksParser(const scoped_refptr<MediaLog>& media_log,
    bool ignore_text_tracks)
    : track_type_(-1)
    , track_num_(-1)
    , seek_preroll_(-1)
    , codec_delay_(-1)
    , default_duration_(-1)
    , audio_track_num_(-1)
    , audio_default_duration_(-1)
    , video_track_num_(-1)
    , video_default_duration_(-1)
    , ignore_text_tracks_(ignore_text_tracks)
    , media_log_(media_log)
    , audio_client_(media_log)
    , video_client_(media_log)
{
}

WebMTracksParser::~WebMTracksParser() { }

int WebMTracksParser::Parse(const uint8* buf, int size)
{
    track_type_ = -1;
    track_num_ = -1;
    default_duration_ = -1;
    track_name_ = "";
    track_language_ = "";
    audio_track_num_ = -1;
    audio_default_duration_ = -1;
    audio_decoder_config_ = AudioDecoderConfig();
    video_track_num_ = -1;
    video_default_duration_ = -1;
    video_decoder_config_ = VideoDecoderConfig();
    text_tracks_.clear();
    ignored_tracks_.clear();

    WebMListParser parser(kWebMIdTracks, this);
    int result = parser.Parse(buf, size);

    if (result <= 0)
        return result;

    // For now we do all or nothing parsing.
    return parser.IsParsingComplete() ? result : 0;
}

base::TimeDelta WebMTracksParser::GetAudioDefaultDuration(
    const double timecode_scale_in_us) const
{
    return PrecisionCappedDefaultDuration(timecode_scale_in_us,
        audio_default_duration_);
}

base::TimeDelta WebMTracksParser::GetVideoDefaultDuration(
    const double timecode_scale_in_us) const
{
    return PrecisionCappedDefaultDuration(timecode_scale_in_us,
        video_default_duration_);
}

WebMParserClient* WebMTracksParser::OnListStart(int id)
{
    if (id == kWebMIdContentEncodings) {
        DCHECK(!track_content_encodings_client_.get());
        track_content_encodings_client_.reset(
            new WebMContentEncodingsClient(media_log_));
        return track_content_encodings_client_->OnListStart(id);
    }

    if (id == kWebMIdTrackEntry) {
        track_type_ = -1;
        track_num_ = -1;
        default_duration_ = -1;
        track_name_ = "";
        track_language_ = "";
        codec_id_ = "";
        codec_private_.clear();
        audio_client_.Reset();
        video_client_.Reset();
        return this;
    }

    if (id == kWebMIdAudio)
        return &audio_client_;

    if (id == kWebMIdVideo)
        return &video_client_;

    return this;
}

bool WebMTracksParser::OnListEnd(int id)
{
    if (id == kWebMIdContentEncodings) {
        DCHECK(track_content_encodings_client_.get());
        return track_content_encodings_client_->OnListEnd(id);
    }

    if (id == kWebMIdTrackEntry) {
        if (track_type_ == -1 || track_num_ == -1) {
            MEDIA_LOG(ERROR, media_log_) << "Missing TrackEntry data for "
                                         << " TrackType " << track_type_
                                         << " TrackNum " << track_num_;
            return false;
        }

        if (track_type_ != kWebMTrackTypeAudio && track_type_ != kWebMTrackTypeVideo && track_type_ != kWebMTrackTypeSubtitlesOrCaptions && track_type_ != kWebMTrackTypeDescriptionsOrMetadata) {
            MEDIA_LOG(ERROR, media_log_) << "Unexpected TrackType " << track_type_;
            return false;
        }

        TextKind text_track_kind = kTextNone;
        if (track_type_ == kWebMTrackTypeSubtitlesOrCaptions) {
            text_track_kind = CodecIdToTextKind(codec_id_);
            if (text_track_kind == kTextNone) {
                MEDIA_LOG(ERROR, media_log_) << "Missing TrackEntry CodecID"
                                             << " TrackNum " << track_num_;
                return false;
            }

            if (text_track_kind != kTextSubtitles && text_track_kind != kTextCaptions) {
                MEDIA_LOG(ERROR, media_log_) << "Wrong TrackEntry CodecID"
                                             << " TrackNum " << track_num_;
                return false;
            }
        } else if (track_type_ == kWebMTrackTypeDescriptionsOrMetadata) {
            text_track_kind = CodecIdToTextKind(codec_id_);
            if (text_track_kind == kTextNone) {
                MEDIA_LOG(ERROR, media_log_) << "Missing TrackEntry CodecID"
                                             << " TrackNum " << track_num_;
                return false;
            }

            if (text_track_kind != kTextDescriptions && text_track_kind != kTextMetadata) {
                MEDIA_LOG(ERROR, media_log_) << "Wrong TrackEntry CodecID"
                                             << " TrackNum " << track_num_;
                return false;
            }
        }

        std::string encryption_key_id;
        if (track_content_encodings_client_) {
            DCHECK(!track_content_encodings_client_->content_encodings().empty());
            // If we have multiple ContentEncoding in one track. Always choose the
            // key id in the first ContentEncoding as the key id of the track.
            encryption_key_id = track_content_encodings_client_->content_encodings()[0]->encryption_key_id();
        }

        if (track_type_ == kWebMTrackTypeAudio) {
            if (audio_track_num_ == -1) {
                audio_track_num_ = track_num_;
                audio_encryption_key_id_ = encryption_key_id;

                if (default_duration_ == 0) {
                    MEDIA_LOG(ERROR, media_log_) << "Illegal 0ns audio TrackEntry "
                                                    "DefaultDuration";
                    return false;
                }
                audio_default_duration_ = default_duration_;

                DCHECK(!audio_decoder_config_.IsValidConfig());
                if (!audio_client_.InitializeConfig(
                        codec_id_, codec_private_, seek_preroll_, codec_delay_,
                        !audio_encryption_key_id_.empty(), &audio_decoder_config_)) {
                    return false;
                }
            } else {
                MEDIA_LOG(DEBUG, media_log_) << "Ignoring audio track " << track_num_;
                ignored_tracks_.insert(track_num_);
            }
        } else if (track_type_ == kWebMTrackTypeVideo) {
            if (video_track_num_ == -1) {
                video_track_num_ = track_num_;
                video_encryption_key_id_ = encryption_key_id;

                if (default_duration_ == 0) {
                    MEDIA_LOG(ERROR, media_log_) << "Illegal 0ns video TrackEntry "
                                                    "DefaultDuration";
                    return false;
                }
                video_default_duration_ = default_duration_;

                DCHECK(!video_decoder_config_.IsValidConfig());
                if (!video_client_.InitializeConfig(
                        codec_id_, codec_private_, !video_encryption_key_id_.empty(),
                        &video_decoder_config_)) {
                    return false;
                }
            } else {
                MEDIA_LOG(DEBUG, media_log_) << "Ignoring video track " << track_num_;
                ignored_tracks_.insert(track_num_);
            }
        } else if (track_type_ == kWebMTrackTypeSubtitlesOrCaptions || track_type_ == kWebMTrackTypeDescriptionsOrMetadata) {
            if (ignore_text_tracks_) {
                MEDIA_LOG(DEBUG, media_log_) << "Ignoring text track " << track_num_;
                ignored_tracks_.insert(track_num_);
            } else {
                std::string track_num = base::Int64ToString(track_num_);
                text_tracks_[track_num_] = TextTrackConfig(
                    text_track_kind, track_name_, track_language_, track_num);
            }
        } else {
            MEDIA_LOG(ERROR, media_log_) << "Unexpected TrackType " << track_type_;
            return false;
        }

        track_type_ = -1;
        track_num_ = -1;
        default_duration_ = -1;
        track_name_ = "";
        track_language_ = "";
        codec_id_ = "";
        codec_private_.clear();
        track_content_encodings_client_.reset();

        audio_client_.Reset();
        video_client_.Reset();
        return true;
    }

    return true;
}

bool WebMTracksParser::OnUInt(int id, int64 val)
{
    int64* dst = NULL;

    switch (id) {
    case kWebMIdTrackNumber:
        dst = &track_num_;
        break;
    case kWebMIdTrackType:
        dst = &track_type_;
        break;
    case kWebMIdSeekPreRoll:
        dst = &seek_preroll_;
        break;
    case kWebMIdCodecDelay:
        dst = &codec_delay_;
        break;
    case kWebMIdDefaultDuration:
        dst = &default_duration_;
        break;
    default:
        return true;
    }

    if (*dst != -1) {
        MEDIA_LOG(ERROR, media_log_) << "Multiple values for id " << std::hex << id
                                     << " specified";
        return false;
    }

    *dst = val;
    return true;
}

bool WebMTracksParser::OnFloat(int id, double val)
{
    return true;
}

bool WebMTracksParser::OnBinary(int id, const uint8* data, int size)
{
    if (id == kWebMIdCodecPrivate) {
        if (!codec_private_.empty()) {
            MEDIA_LOG(ERROR, media_log_)
                << "Multiple CodecPrivate fields in a track.";
            return false;
        }
        codec_private_.assign(data, data + size);
        return true;
    }
    return true;
}

bool WebMTracksParser::OnString(int id, const std::string& str)
{
    if (id == kWebMIdCodecID) {
        if (!codec_id_.empty()) {
            MEDIA_LOG(ERROR, media_log_) << "Multiple CodecID fields in a track";
            return false;
        }

        codec_id_ = str;
        return true;
    }

    if (id == kWebMIdName) {
        track_name_ = str;
        return true;
    }

    if (id == kWebMIdLanguage) {
        track_language_ = str;
        return true;
    }

    return true;
}

} // namespace media
