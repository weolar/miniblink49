// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FORMATS_MP2T_ES_PARSER_ADTS_H_
#define MEDIA_FORMATS_MP2T_ES_PARSER_ADTS_H_

#include <list>
#include <utility>

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "media/base/audio_decoder_config.h"
#include "media/base/media_export.h"
#include "media/formats/mp2t/es_parser.h"

namespace media {
class AudioTimestampHelper;
class BitReader;
class OffsetByteQueue;
class StreamParserBuffer;
}

namespace media {
namespace mp2t {

    class MEDIA_EXPORT EsParserAdts : public EsParser {
    public:
        typedef base::Callback<void(const AudioDecoderConfig&)> NewAudioConfigCB;

        EsParserAdts(const NewAudioConfigCB& new_audio_config_cb,
            const EmitBufferCB& emit_buffer_cb,
            bool sbr_in_mimetype);
        ~EsParserAdts() override;

        // EsParser implementation.
        void Flush() override;

    private:
        struct AdtsFrame;

        // EsParser implementation.
        bool ParseFromEsQueue() override;
        void ResetInternal() override;

        // Synchronize the stream on an ADTS syncword (consuming bytes from
        // |es_queue_| if needed).
        // Returns true when a full ADTS frame has been found: in that case
        // |adts_frame| structure is filled up accordingly.
        // Returns false otherwise (no ADTS syncword found or partial ADTS frame).
        bool LookForAdtsFrame(AdtsFrame* adts_frame);

        // Skip an ADTS frame in the ES queue.
        void SkipAdtsFrame(const AdtsFrame& adts_frame);

        // Signal any audio configuration change (if any).
        // Return false if the current audio config is not
        // a supported ADTS audio config.
        bool UpdateAudioConfiguration(const uint8* adts_header);

        // Callbacks:
        // - to signal a new audio configuration,
        // - to send ES buffers.
        NewAudioConfigCB new_audio_config_cb_;
        EmitBufferCB emit_buffer_cb_;

        // True when AAC SBR extension is signalled in the mimetype
        // (mp4a.40.5 in the codecs parameter).
        bool sbr_in_mimetype_;

        // Interpolated PTS for frames that don't have one.
        scoped_ptr<AudioTimestampHelper> audio_timestamp_helper_;

        // Last audio config.
        AudioDecoderConfig last_audio_decoder_config_;

        DISALLOW_COPY_AND_ASSIGN(EsParserAdts);
    };

} // namespace mp2t
} // namespace media

#endif
