// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/formats/mp2t/es_parser_h264.h"

#include "base/logging.h"
#include "base/numerics/safe_conversions.h"
#include "media/base/stream_parser_buffer.h"
#include "media/base/timestamp_constants.h"
#include "media/base/video_frame.h"
#include "media/filters/h264_parser.h"
#include "media/formats/common/offset_byte_queue.h"
#include "media/formats/mp2t/mp2t_common.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace media {
namespace mp2t {

    // An AUD NALU is at least 4 bytes:
    // 3 bytes for the start code + 1 byte for the NALU type.
    const int kMinAUDSize = 4;

    EsParserH264::EsParserH264(
        const NewVideoConfigCB& new_video_config_cb,
        const EmitBufferCB& emit_buffer_cb)
        : es_adapter_(new_video_config_cb, emit_buffer_cb)
        , h264_parser_(new H264Parser())
        , current_access_unit_pos_(0)
        , next_access_unit_pos_(0)
    {
    }

    EsParserH264::~EsParserH264()
    {
    }

    void EsParserH264::Flush()
    {
        DVLOG(1) << __FUNCTION__;
        if (!FindAUD(&current_access_unit_pos_))
            return;

        // Simulate an additional AUD to force emitting the last access unit
        // which is assumed to be complete at this point.
        uint8 aud[] = { 0x00, 0x00, 0x01, 0x09 };
        es_queue_->Push(aud, sizeof(aud));
        ParseFromEsQueue();

        es_adapter_.Flush();
    }

    void EsParserH264::ResetInternal()
    {
        DVLOG(1) << __FUNCTION__;
        h264_parser_.reset(new H264Parser());
        current_access_unit_pos_ = 0;
        next_access_unit_pos_ = 0;
        last_video_decoder_config_ = VideoDecoderConfig();
        es_adapter_.Reset();
    }

    bool EsParserH264::FindAUD(int64* stream_pos)
    {
        while (true) {
            const uint8* es;
            int size;
            es_queue_->PeekAt(*stream_pos, &es, &size);

            // Find a start code and move the stream to the start code parser position.
            off_t start_code_offset;
            off_t start_code_size;
            bool start_code_found = H264Parser::FindStartCode(
                es, size, &start_code_offset, &start_code_size);
            *stream_pos += start_code_offset;

            // No H264 start code found or NALU type not available yet.
            if (!start_code_found || start_code_offset + start_code_size >= size)
                return false;

            // Exit the parser loop when an AUD is found.
            // Note: NALU header for an AUD:
            // - nal_ref_idc must be 0
            // - nal_unit_type must be H264NALU::kAUD
            if (es[start_code_offset + start_code_size] == H264NALU::kAUD)
                break;

            // The current NALU is not an AUD, skip the start code
            // and continue parsing the stream.
            *stream_pos += start_code_size;
        }

        return true;
    }

    bool EsParserH264::ParseFromEsQueue()
    {
        DCHECK_LE(es_queue_->head(), current_access_unit_pos_);
        DCHECK_LE(current_access_unit_pos_, next_access_unit_pos_);
        DCHECK_LE(next_access_unit_pos_, es_queue_->tail());

        // Find the next AUD located at or after |current_access_unit_pos_|. This is
        // needed since initially |current_access_unit_pos_| might not point to
        // an AUD.
        // Discard all the data before the updated |current_access_unit_pos_|
        // since it won't be used again.
        bool aud_found = FindAUD(&current_access_unit_pos_);
        es_queue_->Trim(current_access_unit_pos_);
        if (next_access_unit_pos_ < current_access_unit_pos_)
            next_access_unit_pos_ = current_access_unit_pos_;

        // Resume parsing later if no AUD was found.
        if (!aud_found)
            return true;

        // Find the next AUD to make sure we have a complete access unit.
        if (next_access_unit_pos_ < current_access_unit_pos_ + kMinAUDSize) {
            next_access_unit_pos_ = current_access_unit_pos_ + kMinAUDSize;
            DCHECK_LE(next_access_unit_pos_, es_queue_->tail());
        }
        if (!FindAUD(&next_access_unit_pos_))
            return true;

        // At this point, we know we have a full access unit.
        bool is_key_frame = false;
        int pps_id_for_access_unit = -1;

        const uint8* es;
        int size;
        es_queue_->PeekAt(current_access_unit_pos_, &es, &size);
        int access_unit_size = base::checked_cast<int, int64>(
            next_access_unit_pos_ - current_access_unit_pos_);
        DCHECK_LE(access_unit_size, size);
        h264_parser_->SetStream(es, access_unit_size);

        while (true) {
            bool is_eos = false;
            H264NALU nalu;
            switch (h264_parser_->AdvanceToNextNALU(&nalu)) {
            case H264Parser::kOk:
                break;
            case H264Parser::kInvalidStream:
            case H264Parser::kUnsupportedStream:
                return false;
            case H264Parser::kEOStream:
                is_eos = true;
                break;
            }
            if (is_eos)
                break;

            switch (nalu.nal_unit_type) {
            case H264NALU::kAUD: {
                DVLOG(LOG_LEVEL_ES) << "NALU: AUD";
                break;
            }
            case H264NALU::kSPS: {
                DVLOG(LOG_LEVEL_ES) << "NALU: SPS";
                int sps_id;
                if (h264_parser_->ParseSPS(&sps_id) != H264Parser::kOk)
                    return false;
                break;
            }
            case H264NALU::kPPS: {
                DVLOG(LOG_LEVEL_ES) << "NALU: PPS";
                int pps_id;
                if (h264_parser_->ParsePPS(&pps_id) != H264Parser::kOk)
                    return false;
                break;
            }
            case H264NALU::kIDRSlice:
            case H264NALU::kNonIDRSlice: {
                is_key_frame = (nalu.nal_unit_type == H264NALU::kIDRSlice);
                DVLOG(LOG_LEVEL_ES) << "NALU: slice IDR=" << is_key_frame;
                H264SliceHeader shdr;
                if (h264_parser_->ParseSliceHeader(nalu, &shdr) != H264Parser::kOk) {
                    // Only accept an invalid SPS/PPS at the beginning when the stream
                    // does not necessarily start with an SPS/PPS/IDR.
                    // TODO(damienv): Should be able to differentiate a missing SPS/PPS
                    // from a slice header parsing error.
                    if (last_video_decoder_config_.IsValidConfig())
                        return false;
                } else {
                    pps_id_for_access_unit = shdr.pic_parameter_set_id;
                }
                break;
            }
            default: {
                DVLOG(LOG_LEVEL_ES) << "NALU: " << nalu.nal_unit_type;
            }
            }
        }

        // Emit a frame and move the stream to the next AUD position.
        RCHECK(EmitFrame(current_access_unit_pos_, access_unit_size,
            is_key_frame, pps_id_for_access_unit));
        current_access_unit_pos_ = next_access_unit_pos_;
        es_queue_->Trim(current_access_unit_pos_);

        return true;
    }

    bool EsParserH264::EmitFrame(int64 access_unit_pos, int access_unit_size,
        bool is_key_frame, int pps_id)
    {
        // Get the access unit timing info.
        // Note: |current_timing_desc.pts| might be |kNoTimestamp()| at this point
        // if:
        // - the stream is not fully MPEG-2 compliant.
        // - or if the stream relies on H264 VUI parameters to compute the timestamps.
        //   See H.222 spec: section 2.7.5 "Conditional coding of timestamps".
        //   This part is not yet implemented in EsParserH264.
        // |es_adapter_| will take care of the missing timestamps.
        TimingDesc current_timing_desc = GetTimingDescriptor(access_unit_pos);
        DVLOG_IF(1, current_timing_desc.pts == kNoTimestamp())
            << "Missing timestamp";

        // If only the PTS is provided, copy the PTS into the DTS.
        if (current_timing_desc.dts == kNoDecodeTimestamp()) {
            current_timing_desc.dts = DecodeTimestamp::FromPresentationTime(current_timing_desc.pts);
        }

        // Update the video decoder configuration if needed.
        const H264PPS* pps = h264_parser_->GetPPS(pps_id);
        if (!pps) {
            // Only accept an invalid PPS at the beginning when the stream
            // does not necessarily start with an SPS/PPS/IDR.
            // In this case, the initial frames are conveyed to the upper layer with
            // an invalid VideoDecoderConfig and it's up to the upper layer
            // to process this kind of frame accordingly.
            if (last_video_decoder_config_.IsValidConfig())
                return false;
        } else {
            const H264SPS* sps = h264_parser_->GetSPS(pps->seq_parameter_set_id);
            if (!sps)
                return false;
            RCHECK(UpdateVideoDecoderConfig(sps));
        }

        // Emit a frame.
        DVLOG(LOG_LEVEL_ES) << "Emit frame: stream_pos=" << current_access_unit_pos_
                            << " size=" << access_unit_size;
        int es_size;
        const uint8* es;
        es_queue_->PeekAt(current_access_unit_pos_, &es, &es_size);
        CHECK_GE(es_size, access_unit_size);

        // TODO(wolenetz/acolwell): Validate and use a common cross-parser TrackId
        // type and allow multiple video tracks. See https://crbug.com/341581.
        scoped_refptr<StreamParserBuffer> stream_parser_buffer = StreamParserBuffer::CopyFrom(
            es,
            access_unit_size,
            is_key_frame,
            DemuxerStream::VIDEO,
            0);
        stream_parser_buffer->SetDecodeTimestamp(current_timing_desc.dts);
        stream_parser_buffer->set_timestamp(current_timing_desc.pts);
        return es_adapter_.OnNewBuffer(stream_parser_buffer);
    }

    bool EsParserH264::UpdateVideoDecoderConfig(const H264SPS* sps)
    {
        // Set the SAR to 1 when not specified in the H264 stream.
        int sar_width = (sps->sar_width == 0) ? 1 : sps->sar_width;
        int sar_height = (sps->sar_height == 0) ? 1 : sps->sar_height;

        // TODO(damienv): a MAP unit can be either 16 or 32 pixels.
        // although it's 16 pixels for progressive non MBAFF frames.
        gfx::Size coded_size((sps->pic_width_in_mbs_minus1 + 1) * 16,
            (sps->pic_height_in_map_units_minus1 + 1) * 16);
        gfx::Rect visible_rect(
            sps->frame_crop_left_offset,
            sps->frame_crop_top_offset,
            (coded_size.width() - sps->frame_crop_right_offset) - sps->frame_crop_left_offset,
            (coded_size.height() - sps->frame_crop_bottom_offset) - sps->frame_crop_top_offset);
        if (visible_rect.width() <= 0 || visible_rect.height() <= 0)
            return false;
        gfx::Size natural_size(
            (visible_rect.width() * sar_width) / sar_height,
            visible_rect.height());
        if (natural_size.width() == 0)
            return false;

        VideoDecoderConfig video_decoder_config(
            kCodecH264, VIDEO_CODEC_PROFILE_UNKNOWN, PIXEL_FORMAT_YV12,
            COLOR_SPACE_HD_REC709, coded_size, visible_rect, natural_size,
            std::vector<uint8_t>(), false);

        if (!video_decoder_config.Matches(last_video_decoder_config_)) {
            DVLOG(1) << "Profile IDC: " << sps->profile_idc;
            DVLOG(1) << "Level IDC: " << sps->level_idc;
            DVLOG(1) << "Pic width: " << coded_size.width();
            DVLOG(1) << "Pic height: " << coded_size.height();
            DVLOG(1) << "log2_max_frame_num_minus4: "
                     << sps->log2_max_frame_num_minus4;
            DVLOG(1) << "SAR: width=" << sps->sar_width
                     << " height=" << sps->sar_height;
            last_video_decoder_config_ = video_decoder_config;
            es_adapter_.OnConfigChanged(video_decoder_config);
        }

        return true;
    }

} // namespace mp2t
} // namespace media
