// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FORMATS_MP2T_MP2T_STREAM_PARSER_H_
#define MEDIA_FORMATS_MP2T_MP2T_STREAM_PARSER_H_

#include <list>
#include <map>

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "media/base/audio_decoder_config.h"
#include "media/base/byte_queue.h"
#include "media/base/media_export.h"
#include "media/base/stream_parser.h"
#include "media/base/video_decoder_config.h"
#include "media/formats/mp2t/timestamp_unroller.h"

namespace media {

class StreamParserBuffer;

namespace mp2t {

    class PidState;

    class MEDIA_EXPORT Mp2tStreamParser : public StreamParser {
    public:
        explicit Mp2tStreamParser(bool sbr_in_mimetype);
        ~Mp2tStreamParser() override;

        // StreamParser implementation.
        void Init(const InitCB& init_cb,
            const NewConfigCB& config_cb,
            const NewBuffersCB& new_buffers_cb,
            bool ignore_text_tracks,
            const EncryptedMediaInitDataCB& encrypted_media_init_data_cb,
            const NewMediaSegmentCB& new_segment_cb,
            const base::Closure& end_of_segment_cb,
            const scoped_refptr<MediaLog>& media_log) override;
        void Flush() override;
        bool Parse(const uint8* buf, int size) override;

    private:
        typedef std::map<int, PidState*> PidMap;

        struct BufferQueueWithConfig {
            BufferQueueWithConfig(bool is_cfg_sent,
                const AudioDecoderConfig& audio_cfg,
                const VideoDecoderConfig& video_cfg);
            ~BufferQueueWithConfig();

            bool is_config_sent;
            AudioDecoderConfig audio_config;
            StreamParser::BufferQueue audio_queue;
            VideoDecoderConfig video_config;
            StreamParser::BufferQueue video_queue;
        };

        // Callback invoked to register a Program Map Table.
        // Note: Does nothing if the PID is already registered.
        void RegisterPmt(int program_number, int pmt_pid);

        // Callback invoked to register a PES pid.
        // Possible values for |stream_type| are defined in:
        // ISO-13818.1 / ITU H.222 Table 2.34 "Stream type assignments".
        // |pes_pid| is part of the Program Map Table refered by |pmt_pid|.
        void RegisterPes(int pmt_pid, int pes_pid, int stream_type);

        // Since the StreamParser interface allows only one audio & video streams,
        // an automatic PID filtering should be applied to select the audio & video
        // streams.
        void UpdatePidFilter();

        // Callback invoked each time the audio/video decoder configuration is
        // changed.
        void OnVideoConfigChanged(int pes_pid,
            const VideoDecoderConfig& video_decoder_config);
        void OnAudioConfigChanged(int pes_pid,
            const AudioDecoderConfig& audio_decoder_config);

        // Invoke the initialization callback if needed.
        bool FinishInitializationIfNeeded();

        // Callback invoked by the ES stream parser
        // to emit a new audio/video access unit.
        void OnEmitAudioBuffer(
            int pes_pid,
            scoped_refptr<StreamParserBuffer> stream_parser_buffer);
        void OnEmitVideoBuffer(
            int pes_pid,
            scoped_refptr<StreamParserBuffer> stream_parser_buffer);
        bool EmitRemainingBuffers();

        // List of callbacks.
        InitCB init_cb_;
        NewConfigCB config_cb_;
        NewBuffersCB new_buffers_cb_;
        EncryptedMediaInitDataCB encrypted_media_init_data_cb_;
        NewMediaSegmentCB new_segment_cb_;
        base::Closure end_of_segment_cb_;
        scoped_refptr<MediaLog> media_log_;

        // True when AAC SBR extension is signalled in the mimetype
        // (mp4a.40.5 in the codecs parameter).
        bool sbr_in_mimetype_;

        // Bytes of the TS stream.
        ByteQueue ts_byte_queue_;

        // List of PIDs and their state.
        PidMap pids_;

        // Selected audio and video PIDs.
        int selected_audio_pid_;
        int selected_video_pid_;

        // Pending audio & video buffers.
        std::list<BufferQueueWithConfig> buffer_queue_chain_;

        // Whether |init_cb_| has been invoked.
        bool is_initialized_;

        // Indicate whether a segment was started.
        bool segment_started_;

        // Timestamp unroller.
        // Timestamps in PES packets must be unrolled using the same offset.
        // So the unroller is global between PES pids.
        TimestampUnroller timestamp_unroller_;

        DISALLOW_COPY_AND_ASSIGN(Mp2tStreamParser);
    };

} // namespace mp2t
} // namespace media

#endif
