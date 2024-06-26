// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FORMATS_MP2T_ES_PARSER_H264_H_
#define MEDIA_FORMATS_MP2T_ES_PARSER_H264_H_

#include <list>
#include <utility>

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "media/base/media_export.h"
#include "media/base/video_decoder_config.h"
#include "media/formats/mp2t/es_adapter_video.h"
#include "media/formats/mp2t/es_parser.h"

namespace media {
class H264Parser;
struct H264SPS;
class OffsetByteQueue;
}

namespace media {
namespace mp2t {

    // A few remarks:
    // - In this h264 parser, frame splitting is based on AUD nals.
    // Mpeg2 TS spec: "2.14 Carriage of Rec. ITU-T H.264 | ISO/IEC 14496-10 video"
    // "Each AVC access unit shall contain an access unit delimiter NAL Unit;"
    // - PES packets do not necessarily map to an H264 access unit although the HLS
    // recommendation is to use one PES for each access unit. In this parser,
    // we handle the general case and do not make any assumption about the access
    // unit organization within PES packets.
    //
    class MEDIA_EXPORT EsParserH264 : public EsParser {
    public:
        typedef base::Callback<void(const VideoDecoderConfig&)> NewVideoConfigCB;

        EsParserH264(const NewVideoConfigCB& new_video_config_cb,
            const EmitBufferCB& emit_buffer_cb);
        ~EsParserH264() override;

        // EsParser implementation.
        void Flush() override;

    private:
        // EsParser implementation.
        bool ParseFromEsQueue() override;
        void ResetInternal() override;

        // Find the AUD located at or after |*stream_pos|.
        // Return true if an AUD is found.
        // If found, |*stream_pos| corresponds to the position of the AUD start code
        // in the stream. Otherwise, |*stream_pos| corresponds to the last position
        // of the start code parser.
        bool FindAUD(int64* stream_pos);

        // Emit a frame whose position in the ES queue starts at |access_unit_pos|.
        // Returns true if successful, false if no PTS is available for the frame.
        bool EmitFrame(int64 access_unit_pos, int access_unit_size,
            bool is_key_frame, int pps_id);

        // Update the video decoder config based on an H264 SPS.
        // Return true if successful.
        bool UpdateVideoDecoderConfig(const H264SPS* sps);

        EsAdapterVideo es_adapter_;

        // H264 parser state.
        // - |current_access_unit_pos_| is pointing to an annexB syncword
        // representing the first NALU of an H264 access unit.
        scoped_ptr<H264Parser> h264_parser_;
        int64 current_access_unit_pos_;
        int64 next_access_unit_pos_;

        // Last video decoder config.
        VideoDecoderConfig last_video_decoder_config_;

        DISALLOW_COPY_AND_ASSIGN(EsParserH264);
    };

} // namespace mp2t
} // namespace media

#endif
