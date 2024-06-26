// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/formats/mp2t/mp2t_stream_parser.h"

#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/stl_util.h"
#include "media/base/stream_parser_buffer.h"
#include "media/base/text_track_config.h"
#include "media/base/timestamp_constants.h"
#include "media/formats/mp2t/es_parser.h"
#include "media/formats/mp2t/es_parser_adts.h"
#include "media/formats/mp2t/es_parser_h264.h"
#include "media/formats/mp2t/es_parser_mpeg1audio.h"
#include "media/formats/mp2t/mp2t_common.h"
#include "media/formats/mp2t/ts_packet.h"
#include "media/formats/mp2t/ts_section.h"
#include "media/formats/mp2t/ts_section_pat.h"
#include "media/formats/mp2t/ts_section_pes.h"
#include "media/formats/mp2t/ts_section_pmt.h"

namespace media {
namespace mp2t {

    enum StreamType {
        // ISO-13818.1 / ITU H.222 Table 2.34 "Stream type assignments"
        kStreamTypeMpeg1Audio = 0x3,
        kStreamTypeAAC = 0xf,
        kStreamTypeAVC = 0x1b,
    };

    class PidState {
    public:
        enum PidType {
            kPidPat,
            kPidPmt,
            kPidAudioPes,
            kPidVideoPes,
        };

        PidState(int pid, PidType pid_tyoe,
            scoped_ptr<TsSection> section_parser);

        // Extract the content of the TS packet and parse it.
        // Return true if successful.
        bool PushTsPacket(const TsPacket& ts_packet);

        // Flush the PID state (possibly emitting some pending frames)
        // and reset its state.
        void Flush();

        // Enable/disable the PID.
        // Disabling a PID will reset its state and ignore any further incoming TS
        // packets.
        void Enable();
        void Disable();
        bool IsEnabled() const;

        PidType pid_type() const { return pid_type_; }

    private:
        void ResetState();

        int pid_;
        PidType pid_type_;
        scoped_ptr<TsSection> section_parser_;

        bool enable_;

        int continuity_counter_;
    };

    PidState::PidState(int pid, PidType pid_type,
        scoped_ptr<TsSection> section_parser)
        : pid_(pid)
        , pid_type_(pid_type)
        , section_parser_(section_parser.Pass())
        , enable_(false)
        , continuity_counter_(-1)
    {
        DCHECK(section_parser_);
    }

    bool PidState::PushTsPacket(const TsPacket& ts_packet)
    {
        DCHECK_EQ(ts_packet.pid(), pid_);

        // The current PID is not part of the PID filter,
        // just discard the incoming TS packet.
        if (!enable_)
            return true;

        int expected_continuity_counter = (continuity_counter_ + 1) % 16;
        if (continuity_counter_ >= 0 && ts_packet.continuity_counter() != expected_continuity_counter) {
            DVLOG(1) << "TS discontinuity detected for pid: " << pid_;
            return false;
        }

        bool status = section_parser_->Parse(
            ts_packet.payload_unit_start_indicator(),
            ts_packet.payload(),
            ts_packet.payload_size());

        // At the minimum, when parsing failed, auto reset the section parser.
        // Components that use the StreamParser can take further action if needed.
        if (!status) {
            DVLOG(1) << "Parsing failed for pid = " << pid_;
            ResetState();
        }

        return status;
    }

    void PidState::Flush()
    {
        section_parser_->Flush();
        ResetState();
    }

    void PidState::Enable()
    {
        enable_ = true;
    }

    void PidState::Disable()
    {
        if (!enable_)
            return;

        ResetState();
        enable_ = false;
    }

    bool PidState::IsEnabled() const
    {
        return enable_;
    }

    void PidState::ResetState()
    {
        section_parser_->Reset();
        continuity_counter_ = -1;
    }

    Mp2tStreamParser::BufferQueueWithConfig::BufferQueueWithConfig(
        bool is_cfg_sent,
        const AudioDecoderConfig& audio_cfg,
        const VideoDecoderConfig& video_cfg)
        : is_config_sent(is_cfg_sent)
        , audio_config(audio_cfg)
        , video_config(video_cfg)
    {
    }

    Mp2tStreamParser::BufferQueueWithConfig::~BufferQueueWithConfig()
    {
    }

    Mp2tStreamParser::Mp2tStreamParser(bool sbr_in_mimetype)
        : sbr_in_mimetype_(sbr_in_mimetype)
        , selected_audio_pid_(-1)
        , selected_video_pid_(-1)
        , is_initialized_(false)
        , segment_started_(false)
    {
    }

    Mp2tStreamParser::~Mp2tStreamParser()
    {
        STLDeleteValues(&pids_);
    }

    void Mp2tStreamParser::Init(
        const InitCB& init_cb,
        const NewConfigCB& config_cb,
        const NewBuffersCB& new_buffers_cb,
        bool /* ignore_text_tracks */,
        const EncryptedMediaInitDataCB& encrypted_media_init_data_cb,
        const NewMediaSegmentCB& new_segment_cb,
        const base::Closure& end_of_segment_cb,
        const scoped_refptr<MediaLog>& media_log)
    {
        DCHECK(!is_initialized_);
        DCHECK(init_cb_.is_null());
        DCHECK(!init_cb.is_null());
        DCHECK(!config_cb.is_null());
        DCHECK(!new_buffers_cb.is_null());
        DCHECK(!new_segment_cb.is_null());
        DCHECK(!end_of_segment_cb.is_null());

        init_cb_ = init_cb;
        config_cb_ = config_cb;
        new_buffers_cb_ = new_buffers_cb;
        encrypted_media_init_data_cb_ = encrypted_media_init_data_cb;
        new_segment_cb_ = new_segment_cb;
        end_of_segment_cb_ = end_of_segment_cb;
        media_log_ = media_log;
    }

    void Mp2tStreamParser::Flush()
    {
        DVLOG(1) << "Mp2tStreamParser::Flush";

        // Flush the buffers and reset the pids.
        for (std::map<int, PidState*>::iterator it = pids_.begin();
             it != pids_.end(); ++it) {
            DVLOG(1) << "Flushing PID: " << it->first;
            PidState* pid_state = it->second;
            pid_state->Flush();
            delete pid_state;
        }
        pids_.clear();

        // Flush is invoked from SourceBuffer.abort/SourceState::ResetParserState, and
        // MSE spec prohibits emitting new configs in ResetParserState algorithm (see
        // https://w3c.github.io/media-source/#sourcebuffer-reset-parser-state,
        // 3.5.2 Reset Parser State states that new frames might be processed only in
        // PARSING_MEDIA_SEGMENT and therefore doesn't allow emitting new configs,
        // since that might need to run "init segment received" algorithm).
        // So before we emit remaining buffers here, we need to trim our buffer queue
        // so that we leave only buffers with configs that were already sent.
        for (auto buffer_queue_iter = buffer_queue_chain_.begin();
             buffer_queue_iter != buffer_queue_chain_.end(); ++buffer_queue_iter) {
            const BufferQueueWithConfig& queue_with_config = *buffer_queue_iter;
            if (!queue_with_config.is_config_sent) {
                DVLOG(LOG_LEVEL_ES) << "Flush: dropping buffers with unsent new configs.";
                buffer_queue_chain_.erase(buffer_queue_iter, buffer_queue_chain_.end());
                break;
            }
        }

        EmitRemainingBuffers();
        buffer_queue_chain_.clear();

        // End of the segment.
        // Note: does not need to invoke |end_of_segment_cb_| since flushing the
        // stream parser already involves the end of the current segment.
        segment_started_ = false;

        // Remove any bytes left in the TS buffer.
        // (i.e. any partial TS packet => less than 188 bytes).
        ts_byte_queue_.Reset();

        // Reset the selected PIDs.
        selected_audio_pid_ = -1;
        selected_video_pid_ = -1;

        // Reset the timestamp unroller.
        timestamp_unroller_.Reset();
    }

    bool Mp2tStreamParser::Parse(const uint8* buf, int size)
    {
        DVLOG(1) << "Mp2tStreamParser::Parse size=" << size;

        // Add the data to the parser state.
        ts_byte_queue_.Push(buf, size);

        while (true) {
            const uint8* ts_buffer;
            int ts_buffer_size;
            ts_byte_queue_.Peek(&ts_buffer, &ts_buffer_size);
            if (ts_buffer_size < TsPacket::kPacketSize)
                break;

            // Synchronization.
            int skipped_bytes = TsPacket::Sync(ts_buffer, ts_buffer_size);
            if (skipped_bytes > 0) {
                DVLOG(1) << "Packet not aligned on a TS syncword:"
                         << " skipped_bytes=" << skipped_bytes;
                ts_byte_queue_.Pop(skipped_bytes);
                continue;
            }

            // Parse the TS header, skipping 1 byte if the header is invalid.
            scoped_ptr<TsPacket> ts_packet(TsPacket::Parse(ts_buffer, ts_buffer_size));
            if (!ts_packet) {
                DVLOG(1) << "Error: invalid TS packet";
                ts_byte_queue_.Pop(1);
                continue;
            }
            DVLOG(LOG_LEVEL_TS)
                << "Processing PID=" << ts_packet->pid()
                << " start_unit=" << ts_packet->payload_unit_start_indicator();

            // Parse the section.
            std::map<int, PidState*>::iterator it = pids_.find(ts_packet->pid());
            if (it == pids_.end() && ts_packet->pid() == TsSection::kPidPat) {
                // Create the PAT state here if needed.
                scoped_ptr<TsSection> pat_section_parser(
                    new TsSectionPat(
                        base::Bind(&Mp2tStreamParser::RegisterPmt,
                            base::Unretained(this))));
                scoped_ptr<PidState> pat_pid_state(
                    new PidState(ts_packet->pid(), PidState::kPidPat,
                        pat_section_parser.Pass()));
                pat_pid_state->Enable();
                it = pids_.insert(
                              std::pair<int, PidState*>(ts_packet->pid(),
                                  pat_pid_state.release()))
                         .first;
            }

            if (it != pids_.end()) {
                if (!it->second->PushTsPacket(*ts_packet))
                    return false;
            } else {
                DVLOG(LOG_LEVEL_TS) << "Ignoring TS packet for pid: " << ts_packet->pid();
            }

            // Go to the next packet.
            ts_byte_queue_.Pop(TsPacket::kPacketSize);
        }

        RCHECK(FinishInitializationIfNeeded());

        // Emit the A/V buffers that kept accumulating during TS parsing.
        return EmitRemainingBuffers();
    }

    void Mp2tStreamParser::RegisterPmt(int program_number, int pmt_pid)
    {
        DVLOG(1) << "RegisterPmt:"
                 << " program_number=" << program_number
                 << " pmt_pid=" << pmt_pid;

        // Only one TS program is allowed. Ignore the incoming program map table,
        // if there is already one registered.
        for (std::map<int, PidState*>::iterator it = pids_.begin();
             it != pids_.end(); ++it) {
            PidState* pid_state = it->second;
            if (pid_state->pid_type() == PidState::kPidPmt) {
                DVLOG_IF(1, pmt_pid != it->first) << "More than one program is defined";
                return;
            }
        }

        // Create the PMT state here if needed.
        DVLOG(1) << "Create a new PMT parser";
        scoped_ptr<TsSection> pmt_section_parser(
            new TsSectionPmt(
                base::Bind(&Mp2tStreamParser::RegisterPes,
                    base::Unretained(this), pmt_pid)));
        scoped_ptr<PidState> pmt_pid_state(
            new PidState(pmt_pid, PidState::kPidPmt, pmt_section_parser.Pass()));
        pmt_pid_state->Enable();
        pids_.insert(std::pair<int, PidState*>(pmt_pid, pmt_pid_state.release()));
    }

    void Mp2tStreamParser::RegisterPes(int pmt_pid,
        int pes_pid,
        int stream_type)
    {
        // TODO(damienv): check there is no mismatch if the entry already exists.
        DVLOG(1) << "RegisterPes:"
                 << " pes_pid=" << pes_pid
                 << " stream_type=" << std::hex << stream_type << std::dec;
        std::map<int, PidState*>::iterator it = pids_.find(pes_pid);
        if (it != pids_.end())
            return;

        // Create a stream parser corresponding to the stream type.
        bool is_audio = false;
        scoped_ptr<EsParser> es_parser;
        if (stream_type == kStreamTypeAVC) {
            es_parser.reset(
                new EsParserH264(
                    base::Bind(&Mp2tStreamParser::OnVideoConfigChanged,
                        base::Unretained(this),
                        pes_pid),
                    base::Bind(&Mp2tStreamParser::OnEmitVideoBuffer,
                        base::Unretained(this),
                        pes_pid)));
        } else if (stream_type == kStreamTypeAAC) {
            es_parser.reset(
                new EsParserAdts(
                    base::Bind(&Mp2tStreamParser::OnAudioConfigChanged,
                        base::Unretained(this),
                        pes_pid),
                    base::Bind(&Mp2tStreamParser::OnEmitAudioBuffer,
                        base::Unretained(this),
                        pes_pid),
                    sbr_in_mimetype_));
            is_audio = true;
        } else if (stream_type == kStreamTypeMpeg1Audio) {
            es_parser.reset(new EsParserMpeg1Audio(
                base::Bind(&Mp2tStreamParser::OnAudioConfigChanged,
                    base::Unretained(this), pes_pid),
                base::Bind(&Mp2tStreamParser::OnEmitAudioBuffer, base::Unretained(this),
                    pes_pid),
                media_log_));
            is_audio = true;
        } else {
            return;
        }

        // Create the PES state here.
        DVLOG(1) << "Create a new PES state";
        scoped_ptr<TsSection> pes_section_parser(
            new TsSectionPes(es_parser.Pass(), &timestamp_unroller_));
        PidState::PidType pid_type = is_audio ? PidState::kPidAudioPes : PidState::kPidVideoPes;
        scoped_ptr<PidState> pes_pid_state(
            new PidState(pes_pid, pid_type, pes_section_parser.Pass()));
        pids_.insert(std::pair<int, PidState*>(pes_pid, pes_pid_state.release()));

        // A new PES pid has been added, the PID filter might change.
        UpdatePidFilter();
    }

    void Mp2tStreamParser::UpdatePidFilter()
    {
        // Applies the HLS rule to select the default audio/video PIDs:
        // select the audio/video streams with the lowest PID.
        // TODO(damienv): this can be changed when the StreamParser interface
        // supports multiple audio/video streams.
        PidMap::iterator lowest_audio_pid = pids_.end();
        PidMap::iterator lowest_video_pid = pids_.end();
        for (PidMap::iterator it = pids_.begin(); it != pids_.end(); ++it) {
            int pid = it->first;
            PidState* pid_state = it->second;
            if (pid_state->pid_type() == PidState::kPidAudioPes && (lowest_audio_pid == pids_.end() || pid < lowest_audio_pid->first))
                lowest_audio_pid = it;
            if (pid_state->pid_type() == PidState::kPidVideoPes && (lowest_video_pid == pids_.end() || pid < lowest_video_pid->first))
                lowest_video_pid = it;
        }

        // Enable both the lowest audio and video PIDs.
        if (lowest_audio_pid != pids_.end()) {
            DVLOG(1) << "Enable audio pid: " << lowest_audio_pid->first;
            lowest_audio_pid->second->Enable();
            selected_audio_pid_ = lowest_audio_pid->first;
        }
        if (lowest_video_pid != pids_.end()) {
            DVLOG(1) << "Enable video pid: " << lowest_video_pid->first;
            lowest_video_pid->second->Enable();
            selected_video_pid_ = lowest_video_pid->first;
        }

        // Disable all the other audio and video PIDs.
        for (PidMap::iterator it = pids_.begin(); it != pids_.end(); ++it) {
            PidState* pid_state = it->second;
            if (it != lowest_audio_pid && it != lowest_video_pid && (pid_state->pid_type() == PidState::kPidAudioPes || pid_state->pid_type() == PidState::kPidVideoPes))
                pid_state->Disable();
        }
    }

    void Mp2tStreamParser::OnVideoConfigChanged(
        int pes_pid,
        const VideoDecoderConfig& video_decoder_config)
    {
        DVLOG(1) << "OnVideoConfigChanged for pid=" << pes_pid;
        DCHECK_EQ(pes_pid, selected_video_pid_);
        DCHECK(video_decoder_config.IsValidConfig());

        if (!buffer_queue_chain_.empty() && !buffer_queue_chain_.back().video_config.IsValidConfig()) {
            // No video has been received so far, can reuse the existing video queue.
            DCHECK(buffer_queue_chain_.back().video_queue.empty());
            buffer_queue_chain_.back().video_config = video_decoder_config;
        } else {
            // Create a new entry in |buffer_queue_chain_| with the updated configs.
            BufferQueueWithConfig buffer_queue_with_config(
                false,
                buffer_queue_chain_.empty()
                    ? AudioDecoderConfig()
                    : buffer_queue_chain_.back().audio_config,
                video_decoder_config);
            buffer_queue_chain_.push_back(buffer_queue_with_config);
        }

        // Replace any non valid config with the 1st valid entry.
        // This might happen if there was no available config before.
        for (std::list<BufferQueueWithConfig>::iterator it = buffer_queue_chain_.begin(); it != buffer_queue_chain_.end(); ++it) {
            if (it->video_config.IsValidConfig())
                break;
            it->video_config = video_decoder_config;
        }
    }

    void Mp2tStreamParser::OnAudioConfigChanged(
        int pes_pid,
        const AudioDecoderConfig& audio_decoder_config)
    {
        DVLOG(1) << "OnAudioConfigChanged for pid=" << pes_pid;
        DCHECK_EQ(pes_pid, selected_audio_pid_);
        DCHECK(audio_decoder_config.IsValidConfig());

        if (!buffer_queue_chain_.empty() && !buffer_queue_chain_.back().audio_config.IsValidConfig()) {
            // No audio has been received so far, can reuse the existing audio queue.
            DCHECK(buffer_queue_chain_.back().audio_queue.empty());
            buffer_queue_chain_.back().audio_config = audio_decoder_config;
        } else {
            // Create a new entry in |buffer_queue_chain_| with the updated configs.
            BufferQueueWithConfig buffer_queue_with_config(
                false,
                audio_decoder_config,
                buffer_queue_chain_.empty()
                    ? VideoDecoderConfig()
                    : buffer_queue_chain_.back().video_config);
            buffer_queue_chain_.push_back(buffer_queue_with_config);
        }

        // Replace any non valid config with the 1st valid entry.
        // This might happen if there was no available config before.
        for (std::list<BufferQueueWithConfig>::iterator it = buffer_queue_chain_.begin(); it != buffer_queue_chain_.end(); ++it) {
            if (it->audio_config.IsValidConfig())
                break;
            it->audio_config = audio_decoder_config;
        }
    }

    bool Mp2tStreamParser::FinishInitializationIfNeeded()
    {
        // Nothing to be done if already initialized.
        if (is_initialized_)
            return true;

        // Wait for more data to come to finish initialization.
        if (buffer_queue_chain_.empty())
            return true;

        // Wait for more data to come if one of the config is not available.
        BufferQueueWithConfig& queue_with_config = buffer_queue_chain_.front();
        if (selected_audio_pid_ > 0 && !queue_with_config.audio_config.IsValidConfig())
            return true;
        if (selected_video_pid_ > 0 && !queue_with_config.video_config.IsValidConfig())
            return true;

        // Pass the config before invoking the initialization callback.
        RCHECK(config_cb_.Run(queue_with_config.audio_config,
            queue_with_config.video_config,
            TextTrackConfigMap()));
        queue_with_config.is_config_sent = true;

        // For Mpeg2 TS, the duration is not known.
        DVLOG(1) << "Mpeg2TS stream parser initialization done";
        base::ResetAndReturn(&init_cb_).Run(InitParameters(kInfiniteDuration()));
        is_initialized_ = true;

        return true;
    }

    void Mp2tStreamParser::OnEmitAudioBuffer(
        int pes_pid,
        scoped_refptr<StreamParserBuffer> stream_parser_buffer)
    {
        DCHECK_EQ(pes_pid, selected_audio_pid_);

        DVLOG(LOG_LEVEL_ES)
            << "OnEmitAudioBuffer: "
            << " size="
            << stream_parser_buffer->data_size()
            << " dts="
            << stream_parser_buffer->GetDecodeTimestamp().InMilliseconds()
            << " pts="
            << stream_parser_buffer->timestamp().InMilliseconds()
            << " dur="
            << stream_parser_buffer->duration().InMilliseconds();

        // Ignore the incoming buffer if it is not associated with any config.
        if (buffer_queue_chain_.empty()) {
            NOTREACHED() << "Cannot provide buffers before configs";
            return;
        }

        buffer_queue_chain_.back().audio_queue.push_back(stream_parser_buffer);
    }

    void Mp2tStreamParser::OnEmitVideoBuffer(
        int pes_pid,
        scoped_refptr<StreamParserBuffer> stream_parser_buffer)
    {
        DCHECK_EQ(pes_pid, selected_video_pid_);

        DVLOG(LOG_LEVEL_ES)
            << "OnEmitVideoBuffer"
            << " size="
            << stream_parser_buffer->data_size()
            << " dts="
            << stream_parser_buffer->GetDecodeTimestamp().InMilliseconds()
            << " pts="
            << stream_parser_buffer->timestamp().InMilliseconds()
            << " dur="
            << stream_parser_buffer->duration().InMilliseconds()
            << " is_key_frame="
            << stream_parser_buffer->is_key_frame();

        // Ignore the incoming buffer if it is not associated with any config.
        if (buffer_queue_chain_.empty()) {
            NOTREACHED() << "Cannot provide buffers before configs";
            return;
        }

        buffer_queue_chain_.back().video_queue.push_back(stream_parser_buffer);
    }

    bool Mp2tStreamParser::EmitRemainingBuffers()
    {
        DVLOG(LOG_LEVEL_ES) << "Mp2tStreamParser::EmitRemainingBuffers";

        // No buffer should be sent until fully initialized.
        if (!is_initialized_)
            return true;

        if (buffer_queue_chain_.empty())
            return true;

        // Keep track of the last audio and video config sent.
        AudioDecoderConfig last_audio_config = buffer_queue_chain_.back().audio_config;
        VideoDecoderConfig last_video_config = buffer_queue_chain_.back().video_config;

        // Do not have all the configs, need more data.
        if (selected_audio_pid_ >= 0 && !last_audio_config.IsValidConfig())
            return true;
        if (selected_video_pid_ >= 0 && !last_video_config.IsValidConfig())
            return true;

        // Buffer emission.
        while (!buffer_queue_chain_.empty()) {
            // Start a segment if needed.
            if (!segment_started_) {
                DVLOG(1) << "Starting a new segment";
                segment_started_ = true;
                new_segment_cb_.Run();
            }

            // Update the audio and video config if needed.
            BufferQueueWithConfig& queue_with_config = buffer_queue_chain_.front();
            if (!queue_with_config.is_config_sent) {
                if (!config_cb_.Run(queue_with_config.audio_config,
                        queue_with_config.video_config,
                        TextTrackConfigMap()))
                    return false;
                queue_with_config.is_config_sent = true;
            }

            // Add buffers.
            TextBufferQueueMap empty_text_map;
            if (!queue_with_config.audio_queue.empty() || !queue_with_config.video_queue.empty()) {
                if (!new_buffers_cb_.Run(queue_with_config.audio_queue,
                        queue_with_config.video_queue,
                        empty_text_map)) {
                    return false;
                }
            }

            buffer_queue_chain_.pop_front();
        }

        // Push an empty queue with the last audio/video config
        // so that buffers with the same config can be added later on.
        BufferQueueWithConfig queue_with_config(
            true, last_audio_config, last_video_config);
        buffer_queue_chain_.push_back(queue_with_config);

        return true;
    }

} // namespace mp2t
} // namespace media
