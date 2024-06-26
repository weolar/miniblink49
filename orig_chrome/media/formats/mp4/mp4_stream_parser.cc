// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/formats/mp4/mp4_stream_parser.h"

#include <vector>

#include "base/callback_helpers.h"
#include "base/logging.h"
#include "base/time/time.h"
#include "media/base/audio_decoder_config.h"
#include "media/base/stream_parser_buffer.h"
#include "media/base/text_track_config.h"
#include "media/base/timestamp_constants.h"
#include "media/base/video_decoder_config.h"
#include "media/base/video_util.h"
#include "media/formats/mp4/box_definitions.h"
#include "media/formats/mp4/box_reader.h"
#include "media/formats/mp4/es_descriptor.h"
#include "media/formats/mp4/rcheck.h"
#include "media/formats/mpeg/adts_constants.h"

namespace media {
namespace mp4 {

    MP4StreamParser::MP4StreamParser(const std::set<int>& audio_object_types,
        bool has_sbr)
        : state_(kWaitingForInit)
        , moof_head_(0)
        , mdat_tail_(0)
        , highest_end_offset_(0)
        , has_audio_(false)
        , has_video_(false)
        , audio_track_id_(0)
        , video_track_id_(0)
        , audio_object_types_(audio_object_types)
        , has_sbr_(has_sbr)
        , is_audio_track_encrypted_(false)
        , is_video_track_encrypted_(false)
        , num_top_level_box_skipped_(0)
    {
    }

    MP4StreamParser::~MP4StreamParser() { }

    void MP4StreamParser::Init(
        const InitCB& init_cb,
        const NewConfigCB& config_cb,
        const NewBuffersCB& new_buffers_cb,
        bool /* ignore_text_tracks */,
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

        ChangeState(kParsingBoxes);
        init_cb_ = init_cb;
        config_cb_ = config_cb;
        new_buffers_cb_ = new_buffers_cb;
        encrypted_media_init_data_cb_ = encrypted_media_init_data_cb;
        new_segment_cb_ = new_segment_cb;
        end_of_segment_cb_ = end_of_segment_cb;
        media_log_ = media_log;
    }

    void MP4StreamParser::Reset()
    {
        queue_.Reset();
        runs_.reset();
        moof_head_ = 0;
        mdat_tail_ = 0;
    }

    void MP4StreamParser::Flush()
    {
        DCHECK_NE(state_, kWaitingForInit);
        Reset();
        ChangeState(kParsingBoxes);
    }

    bool MP4StreamParser::Parse(const uint8* buf, int size)
    {
        DCHECK_NE(state_, kWaitingForInit);

        if (state_ == kError)
            return false;

        queue_.Push(buf, size);

        BufferQueue audio_buffers;
        BufferQueue video_buffers;

        bool result = false;
        bool err = false;

        do {
            switch (state_) {
            case kWaitingForInit:
            case kError:
                NOTREACHED();
                return false;

            case kParsingBoxes:
                result = ParseBox(&err);
                break;

            case kWaitingForSampleData:
                result = HaveEnoughDataToEnqueueSamples();
                if (result)
                    ChangeState(kEmittingSamples);
                break;

            case kEmittingSamples:
                result = EnqueueSample(&audio_buffers, &video_buffers, &err);
                if (result) {
                    int64 max_clear = runs_->GetMaxClearOffset() + moof_head_;
                    err = !ReadAndDiscardMDATsUntil(max_clear);
                }
                break;
            }
        } while (result && !err);

        if (!err)
            err = !SendAndFlushSamples(&audio_buffers, &video_buffers);

        if (err) {
            DLOG(ERROR) << "Error while parsing MP4";
            moov_.reset();
            Reset();
            ChangeState(kError);
            return false;
        }

        return true;
    }

    bool MP4StreamParser::ParseBox(bool* err)
    {
        const uint8* buf;
        int size;
        queue_.Peek(&buf, &size);
        if (!size)
            return false;

        scoped_ptr<BoxReader> reader(
            BoxReader::ReadTopLevelBox(buf, size, media_log_, err));
        if (reader.get() == NULL)
            return false;

        if (reader->type() == FOURCC_MOOV) {
            *err = !ParseMoov(reader.get());
        } else if (reader->type() == FOURCC_MOOF) {
            moof_head_ = queue_.head();
            *err = !ParseMoof(reader.get());

            // Set up first mdat offset for ReadMDATsUntil().
            mdat_tail_ = queue_.head() + reader->size();

            // Return early to avoid evicting 'moof' data from queue. Auxiliary info may
            // be located anywhere in the file, including inside the 'moof' itself.
            // (Since 'default-base-is-moof' is mandated, no data references can come
            // before the head of the 'moof', so keeping this box around is sufficient.)
            return !(*err);
        } else {
            // TODO(wolenetz,chcunningham): Enforce more strict adherence to MSE byte
            // stream spec for ftyp and styp. See http://crbug.com/504514.
            DVLOG(2) << "Skipping unrecognized top-level box: "
                     << FourCCToString(reader->type());
        }

        queue_.Pop(reader->size());
        return !(*err);
    }

    bool MP4StreamParser::ParseMoov(BoxReader* reader)
    {
        moov_.reset(new Movie);
        RCHECK(moov_->Parse(reader));
        runs_.reset();

        has_audio_ = false;
        has_video_ = false;

        AudioDecoderConfig audio_config;
        VideoDecoderConfig video_config;

        for (std::vector<Track>::const_iterator track = moov_->tracks.begin();
             track != moov_->tracks.end(); ++track) {
            // TODO(strobe): Only the first audio and video track present in a file are
            // used. (Track selection is better accomplished via Source IDs, though, so
            // adding support for track selection within a stream is low-priority.)
            const SampleDescription& samp_descr = track->media.information.sample_table.description;

            // TODO(strobe): When codec reconfigurations are supported, detect and send
            // a codec reconfiguration for fragments using a sample description index
            // different from the previous one
            size_t desc_idx = 0;
            for (size_t t = 0; t < moov_->extends.tracks.size(); t++) {
                const TrackExtends& trex = moov_->extends.tracks[t];
                if (trex.track_id == track->header.track_id) {
                    desc_idx = trex.default_sample_description_index;
                    break;
                }
            }
            RCHECK(desc_idx > 0);
            desc_idx -= 1; // BMFF descriptor index is one-based

            if (track->media.handler.type == kAudio && !audio_config.IsValidConfig()) {
                RCHECK(!samp_descr.audio_entries.empty());

                // It is not uncommon to find otherwise-valid files with incorrect sample
                // description indices, so we fail gracefully in that case.
                if (desc_idx >= samp_descr.audio_entries.size())
                    desc_idx = 0;
                const AudioSampleEntry& entry = samp_descr.audio_entries[desc_idx];
                const AAC& aac = entry.esds.aac;

                if (!(entry.format == FOURCC_MP4A || (entry.format == FOURCC_ENCA && entry.sinf.format.format == FOURCC_MP4A))) {
                    MEDIA_LOG(ERROR, media_log_) << "Unsupported audio format 0x"
                                                 << std::hex << entry.format
                                                 << " in stsd box.";
                    return false;
                }

                uint8 audio_type = entry.esds.object_type;
                DVLOG(1) << "audio_type " << std::hex << static_cast<int>(audio_type);
                if (audio_object_types_.find(audio_type) == audio_object_types_.end()) {
                    MEDIA_LOG(ERROR, media_log_)
                        << "audio object type 0x" << std::hex << audio_type
                        << " does not match what is specified in the"
                        << " mimetype.";
                    return false;
                }

                AudioCodec codec = kUnknownAudioCodec;
                ChannelLayout channel_layout = CHANNEL_LAYOUT_NONE;
                int sample_per_second = 0;
                std::vector<uint8> extra_data;
                // Check if it is MPEG4 AAC defined in ISO 14496 Part 3 or
                // supported MPEG2 AAC varients.
                if (ESDescriptor::IsAAC(audio_type)) {
                    codec = kCodecAAC;
                    channel_layout = aac.GetChannelLayout(has_sbr_);
                    sample_per_second = aac.GetOutputSamplesPerSecond(has_sbr_);
#if defined(OS_ANDROID)
                    extra_data = aac.codec_specific_data();
#endif
                } else {
                    MEDIA_LOG(ERROR, media_log_) << "Unsupported audio object type 0x"
                                                 << std::hex << audio_type << " in esds.";
                    return false;
                }

                SampleFormat sample_format;
                if (entry.samplesize == 8) {
                    sample_format = kSampleFormatU8;
                } else if (entry.samplesize == 16) {
                    sample_format = kSampleFormatS16;
                } else if (entry.samplesize == 32) {
                    sample_format = kSampleFormatS32;
                } else {
                    LOG(ERROR) << "Unsupported sample size.";
                    return false;
                }

                is_audio_track_encrypted_ = entry.sinf.info.track_encryption.is_encrypted;
                DVLOG(1) << "is_audio_track_encrypted_: " << is_audio_track_encrypted_;
                audio_config.Initialize(codec, sample_format, channel_layout,
                    sample_per_second, extra_data,
                    is_audio_track_encrypted_, base::TimeDelta(), 0);
                has_audio_ = true;
                audio_track_id_ = track->header.track_id;
            }
            if (track->media.handler.type == kVideo && !video_config.IsValidConfig()) {
                RCHECK(!samp_descr.video_entries.empty());
                if (desc_idx >= samp_descr.video_entries.size())
                    desc_idx = 0;
                const VideoSampleEntry& entry = samp_descr.video_entries[desc_idx];

                if (!entry.IsFormatValid()) {
                    MEDIA_LOG(ERROR, media_log_) << "Unsupported video format 0x"
                                                 << std::hex << entry.format
                                                 << " in stsd box.";
                    return false;
                }

                // TODO(strobe): Recover correct crop box
                gfx::Size coded_size(entry.width, entry.height);
                gfx::Rect visible_rect(coded_size);

                // If PASP is available, use the coded size and PASP to calculate the
                // natural size. Otherwise, use the size in track header for natural size.
                gfx::Size natural_size(visible_rect.size());
                if (entry.pixel_aspect.h_spacing != 1 || entry.pixel_aspect.v_spacing != 1) {
                    natural_size = GetNaturalSize(visible_rect.size(), entry.pixel_aspect.h_spacing,
                        entry.pixel_aspect.v_spacing);
                } else if (track->header.width && track->header.height) {
                    natural_size = gfx::Size(track->header.width, track->header.height);
                }

                is_video_track_encrypted_ = entry.sinf.info.track_encryption.is_encrypted;
                DVLOG(1) << "is_video_track_encrypted_: " << is_video_track_encrypted_;
                video_config.Initialize(
                    entry.video_codec, entry.video_codec_profile, PIXEL_FORMAT_YV12,
                    COLOR_SPACE_HD_REC709, coded_size, visible_rect, natural_size,
                    // No decoder-specific buffer needed for AVC;
                    // SPS/PPS are embedded in the video stream
                    std::vector<uint8_t>(), is_video_track_encrypted_);
                has_video_ = true;
                video_track_id_ = track->header.track_id;
            }
        }

        RCHECK(config_cb_.Run(audio_config, video_config, TextTrackConfigMap()));

        StreamParser::InitParameters params(kInfiniteDuration());
        if (moov_->extends.header.fragment_duration > 0) {
            params.duration = TimeDeltaFromRational(
                moov_->extends.header.fragment_duration, moov_->header.timescale);
            params.liveness = DemuxerStream::LIVENESS_RECORDED;
        } else if (moov_->header.duration > 0 && moov_->header.duration != kuint64max) {
            params.duration = TimeDeltaFromRational(moov_->header.duration, moov_->header.timescale);
            params.liveness = DemuxerStream::LIVENESS_RECORDED;
        } else {
            // In ISO/IEC 14496-12:2005(E), 8.30.2: ".. If an MP4 file is created in
            // real-time, such as used in live streaming, it is not likely that the
            // fragment_duration is known in advance and this (mehd) box may be
            // omitted."
            // TODO(wolenetz): Investigate gating liveness detection on timeline_offset
            // when it's populated. See http://crbug.com/312699
            params.liveness = DemuxerStream::LIVENESS_LIVE;
        }

        DVLOG(1) << "liveness: " << params.liveness;

        if (!init_cb_.is_null())
            base::ResetAndReturn(&init_cb_).Run(params);

        if (!moov_->pssh.empty())
            OnEncryptedMediaInitData(moov_->pssh);

        return true;
    }

    bool MP4StreamParser::ParseMoof(BoxReader* reader)
    {
        RCHECK(moov_.get()); // Must already have initialization segment
        MovieFragment moof;
        RCHECK(moof.Parse(reader));
        if (!runs_)
            runs_.reset(new TrackRunIterator(moov_.get(), media_log_));
        RCHECK(runs_->Init(moof));
        RCHECK(ComputeHighestEndOffset(moof));

        if (!moof.pssh.empty())
            OnEncryptedMediaInitData(moof.pssh);

        new_segment_cb_.Run();
        ChangeState(kWaitingForSampleData);
        return true;
    }

    void MP4StreamParser::OnEncryptedMediaInitData(
        const std::vector<ProtectionSystemSpecificHeader>& headers)
    {
        // TODO(strobe): ensure that the value of init_data (all PSSH headers
        // concatenated in arbitrary order) matches the EME spec.
        // See https://www.w3.org/Bugs/Public/show_bug.cgi?id=17673.
        size_t total_size = 0;
        for (size_t i = 0; i < headers.size(); i++)
            total_size += headers[i].raw_box.size();

        std::vector<uint8> init_data(total_size);
        size_t pos = 0;
        for (size_t i = 0; i < headers.size(); i++) {
            memcpy(&init_data[pos], &headers[i].raw_box[0],
                headers[i].raw_box.size());
            pos += headers[i].raw_box.size();
        }
        encrypted_media_init_data_cb_.Run(EmeInitDataType::CENC, init_data);
    }

    bool MP4StreamParser::PrepareAACBuffer(
        const AAC& aac_config, std::vector<uint8>* frame_buf,
        std::vector<SubsampleEntry>* subsamples) const
    {
        // Append an ADTS header to every audio sample.
        RCHECK(aac_config.ConvertEsdsToADTS(frame_buf));

        // As above, adjust subsample information to account for the headers. AAC is
        // not required to use subsample encryption, so we may need to add an entry.
        if (subsamples->empty()) {
            subsamples->push_back(SubsampleEntry(
                kADTSHeaderMinSize, frame_buf->size() - kADTSHeaderMinSize));
        } else {
            (*subsamples)[0].clear_bytes += kADTSHeaderMinSize;
        }
        return true;
    }

    bool MP4StreamParser::EnqueueSample(BufferQueue* audio_buffers,
        BufferQueue* video_buffers,
        bool* err)
    {
        DCHECK_EQ(state_, kEmittingSamples);

        if (!runs_->IsRunValid()) {
            // Flush any buffers we've gotten in this chunk so that buffers don't
            // cross |new_segment_cb_| calls
            *err = !SendAndFlushSamples(audio_buffers, video_buffers);
            if (*err)
                return false;

            // Remain in kEmittingSamples state, discarding data, until the end of
            // the current 'mdat' box has been appended to the queue.
            if (!queue_.Trim(mdat_tail_))
                return false;

            ChangeState(kParsingBoxes);
            end_of_segment_cb_.Run();
            return true;
        }

        if (!runs_->IsSampleValid()) {
            runs_->AdvanceRun();
            return true;
        }

        DCHECK(!(*err));

        const uint8* buf;
        int buf_size;
        queue_.Peek(&buf, &buf_size);
        if (!buf_size)
            return false;

        bool audio = has_audio_ && audio_track_id_ == runs_->track_id();
        bool video = has_video_ && video_track_id_ == runs_->track_id();

        // Skip this entire track if it's not one we're interested in
        if (!audio && !video) {
            runs_->AdvanceRun();
            return true;
        }

        // Attempt to cache the auxiliary information first. Aux info is usually
        // placed in a contiguous block before the sample data, rather than being
        // interleaved. If we didn't cache it, this would require that we retain the
        // start of the segment buffer while reading samples. Aux info is typically
        // quite small compared to sample data, so this pattern is useful on
        // memory-constrained devices where the source buffer consumes a substantial
        // portion of the total system memory.
        if (runs_->AuxInfoNeedsToBeCached()) {
            queue_.PeekAt(runs_->aux_info_offset() + moof_head_, &buf, &buf_size);
            if (buf_size < runs_->aux_info_size())
                return false;
            *err = !runs_->CacheAuxInfo(buf, buf_size);
            return !*err;
        }

        queue_.PeekAt(runs_->sample_offset() + moof_head_, &buf, &buf_size);
        if (buf_size < runs_->sample_size())
            return false;

        scoped_ptr<DecryptConfig> decrypt_config;
        std::vector<SubsampleEntry> subsamples;
        if (runs_->is_encrypted()) {
            decrypt_config = runs_->GetDecryptConfig();
            if (!decrypt_config) {
                *err = true;
                return false;
            }
            subsamples = decrypt_config->subsamples();
        }

        std::vector<uint8> frame_buf(buf, buf + runs_->sample_size());
        if (video) {
            DCHECK(runs_->video_description().frame_bitstream_converter);
            if (!runs_->video_description().frame_bitstream_converter->ConvertFrame(
                    &frame_buf, runs_->is_keyframe(), &subsamples)) {
                MEDIA_LOG(ERROR, media_log_)
                    << "Failed to prepare video sample for decode";
                *err = true;
                return false;
            }
        }

        if (audio) {
            if (ESDescriptor::IsAAC(runs_->audio_description().esds.object_type) && !PrepareAACBuffer(runs_->audio_description().esds.aac, &frame_buf, &subsamples)) {
                MEDIA_LOG(ERROR, media_log_) << "Failed to prepare AAC sample for decode";
                *err = true;
                return false;
            }
        }

        if (decrypt_config) {
            if (!subsamples.empty()) {
                // Create a new config with the updated subsamples.
                decrypt_config.reset(new DecryptConfig(
                    decrypt_config->key_id(),
                    decrypt_config->iv(),
                    subsamples));
            }
            // else, use the existing config.
        } else if ((audio && is_audio_track_encrypted_) || (video && is_video_track_encrypted_)) {
            // The media pipeline requires a DecryptConfig with an empty |iv|.
            // TODO(ddorwin): Refactor so we do not need a fake key ID ("1");
            decrypt_config.reset(
                new DecryptConfig("1", "", std::vector<SubsampleEntry>()));
        }

        StreamParserBuffer::Type buffer_type = audio ? DemuxerStream::AUDIO : DemuxerStream::VIDEO;

        // TODO(wolenetz/acolwell): Validate and use a common cross-parser TrackId
        // type and allow multiple tracks for same media type, if applicable. See
        // https://crbug.com/341581.
        scoped_refptr<StreamParserBuffer> stream_buf = StreamParserBuffer::CopyFrom(&frame_buf[0], frame_buf.size(),
            runs_->is_keyframe(),
            buffer_type, 0);

        if (decrypt_config)
            stream_buf->set_decrypt_config(decrypt_config.Pass());

        stream_buf->set_duration(runs_->duration());
        stream_buf->set_timestamp(runs_->cts());
        stream_buf->SetDecodeTimestamp(runs_->dts());

        DVLOG(3) << "Pushing frame: aud=" << audio
                 << ", key=" << runs_->is_keyframe()
                 << ", dur=" << runs_->duration().InMilliseconds()
                 << ", dts=" << runs_->dts().InMilliseconds()
                 << ", cts=" << runs_->cts().InMilliseconds()
                 << ", size=" << runs_->sample_size();

        if (audio) {
            audio_buffers->push_back(stream_buf);
        } else {
            video_buffers->push_back(stream_buf);
        }

        runs_->AdvanceSample();
        return true;
    }

    bool MP4StreamParser::SendAndFlushSamples(BufferQueue* audio_buffers,
        BufferQueue* video_buffers)
    {
        if (audio_buffers->empty() && video_buffers->empty())
            return true;

        TextBufferQueueMap empty_text_map;
        bool success = new_buffers_cb_.Run(*audio_buffers,
            *video_buffers,
            empty_text_map);
        audio_buffers->clear();
        video_buffers->clear();
        return success;
    }

    bool MP4StreamParser::ReadAndDiscardMDATsUntil(int64 max_clear_offset)
    {
        bool err = false;
        int64 upper_bound = std::min(max_clear_offset, queue_.tail());
        while (mdat_tail_ < upper_bound) {
            const uint8* buf = NULL;
            int size = 0;
            queue_.PeekAt(mdat_tail_, &buf, &size);

            FourCC type;
            int box_sz;
            if (!BoxReader::StartTopLevelBox(buf, size, media_log_, &type, &box_sz,
                    &err))
                break;

            if (type != FOURCC_MDAT) {
                MEDIA_LOG(DEBUG, media_log_)
                    << "Unexpected box type while parsing MDATs: "
                    << FourCCToString(type);
            }
            mdat_tail_ += box_sz;
        }
        queue_.Trim(std::min(mdat_tail_, upper_bound));
        return !err;
    }

    void MP4StreamParser::ChangeState(State new_state)
    {
        DVLOG(2) << "Changing state: " << new_state;
        state_ = new_state;
    }

    bool MP4StreamParser::HaveEnoughDataToEnqueueSamples()
    {
        DCHECK_EQ(state_, kWaitingForSampleData);
        // For muxed content, make sure we have data up to |highest_end_offset_|
        // so we can ensure proper enqueuing behavior. Otherwise assume we have enough
        // data and allow per sample offset checks to meter sample enqueuing.
        // TODO(acolwell): Fix trun box handling so we don't have to special case
        // muxed content.
        return !(has_audio_ && has_video_ && queue_.tail() < highest_end_offset_ + moof_head_);
    }

    bool MP4StreamParser::ComputeHighestEndOffset(const MovieFragment& moof)
    {
        highest_end_offset_ = 0;

        TrackRunIterator runs(moov_.get(), media_log_);
        RCHECK(runs.Init(moof));

        while (runs.IsRunValid()) {
            int64 aux_info_end_offset = runs.aux_info_offset() + runs.aux_info_size();
            if (aux_info_end_offset > highest_end_offset_)
                highest_end_offset_ = aux_info_end_offset;

            while (runs.IsSampleValid()) {
                int64 sample_end_offset = runs.sample_offset() + runs.sample_size();
                if (sample_end_offset > highest_end_offset_)
                    highest_end_offset_ = sample_end_offset;

                runs.AdvanceSample();
            }
            runs.AdvanceRun();
        }

        return true;
    }

} // namespace mp4
} // namespace media
