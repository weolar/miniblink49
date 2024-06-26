// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/formats/mp4/avc.h"

#include <algorithm>

#include "base/logging.h"
#include "media/base/decrypt_config.h"
#include "media/filters/h264_parser.h"
#include "media/formats/mp4/box_definitions.h"
#include "media/formats/mp4/box_reader.h"

namespace media {
namespace mp4 {

    static const uint8 kAnnexBStartCode[] = { 0, 0, 0, 1 };
    static const int kAnnexBStartCodeSize = 4;

    static bool ConvertAVCToAnnexBInPlaceForLengthSize4(std::vector<uint8>* buf)
    {
        const int kLengthSize = 4;
        size_t pos = 0;
        while (pos + kLengthSize < buf->size()) {
            uint32 nal_length = (*buf)[pos];
            nal_length = (nal_length << 8) + (*buf)[pos + 1];
            nal_length = (nal_length << 8) + (*buf)[pos + 2];
            nal_length = (nal_length << 8) + (*buf)[pos + 3];

            if (nal_length == 0) {
                DVLOG(1) << "nal_length is 0";
                return false;
            }

            std::copy(kAnnexBStartCode, kAnnexBStartCode + kAnnexBStartCodeSize,
                buf->begin() + pos);
            pos += kLengthSize + nal_length;
        }
        return pos == buf->size();
    }

    // static
    int AVC::FindSubsampleIndex(const std::vector<uint8>& buffer,
        const std::vector<SubsampleEntry>* subsamples,
        const uint8* ptr)
    {
        DCHECK(ptr >= &buffer[0]);
        DCHECK(ptr <= &buffer[buffer.size() - 1]);
        if (!subsamples || subsamples->empty())
            return 0;

        const uint8* p = &buffer[0];
        for (size_t i = 0; i < subsamples->size(); ++i) {
            p += (*subsamples)[i].clear_bytes + (*subsamples)[i].cypher_bytes;
            if (p > ptr)
                return i;
        }
        NOTREACHED();
        return 0;
    }

    // static
    bool AVC::ConvertFrameToAnnexB(int length_size, std::vector<uint8>* buffer,
        std::vector<SubsampleEntry>* subsamples)
    {
        RCHECK(length_size == 1 || length_size == 2 || length_size == 4);

        if (length_size == 4)
            return ConvertAVCToAnnexBInPlaceForLengthSize4(buffer);

        std::vector<uint8> temp;
        temp.swap(*buffer);
        buffer->reserve(temp.size() + 32);

        size_t pos = 0;
        while (pos + length_size < temp.size()) {
            int nal_length = temp[pos];
            if (length_size == 2)
                nal_length = (nal_length << 8) + temp[pos + 1];
            pos += length_size;

            if (nal_length == 0) {
                DVLOG(1) << "nal_length is 0";
                return false;
            }

            RCHECK(pos + nal_length <= temp.size());
            buffer->insert(buffer->end(), kAnnexBStartCode,
                kAnnexBStartCode + kAnnexBStartCodeSize);
            if (subsamples && !subsamples->empty()) {
                uint8* buffer_pos = &(*(buffer->end() - kAnnexBStartCodeSize));
                int subsample_index = FindSubsampleIndex(*buffer, subsamples, buffer_pos);
                // We've replaced NALU size value with an AnnexB start code.
                int size_adjustment = kAnnexBStartCodeSize - length_size;
                (*subsamples)[subsample_index].clear_bytes += size_adjustment;
            }
            buffer->insert(buffer->end(), temp.begin() + pos,
                temp.begin() + pos + nal_length);
            pos += nal_length;
        }
        return pos == temp.size();
    }

    // static
    bool AVC::InsertParamSetsAnnexB(const AVCDecoderConfigurationRecord& avc_config,
        std::vector<uint8>* buffer,
        std::vector<SubsampleEntry>* subsamples)
    {
        DCHECK(AVC::IsValidAnnexB(*buffer, *subsamples));

        scoped_ptr<H264Parser> parser(new H264Parser());
        const uint8* start = &(*buffer)[0];
        parser->SetEncryptedStream(start, buffer->size(), *subsamples);

        H264NALU nalu;
        if (parser->AdvanceToNextNALU(&nalu) != H264Parser::kOk)
            return false;

        std::vector<uint8>::iterator config_insert_point = buffer->begin();

        if (nalu.nal_unit_type == H264NALU::kAUD) {
            // Move insert point to just after the AUD.
            config_insert_point += (nalu.data + nalu.size) - start;
        }

        // Clear |parser| and |start| since they aren't needed anymore and
        // will hold stale pointers once the insert happens.
        parser.reset();
        start = NULL;

        std::vector<uint8> param_sets;
        RCHECK(AVC::ConvertConfigToAnnexB(avc_config, &param_sets));

        if (subsamples && !subsamples->empty()) {
            int subsample_index = FindSubsampleIndex(*buffer, subsamples,
                &(*config_insert_point));
            // Update the size of the subsample where SPS/PPS is to be inserted.
            (*subsamples)[subsample_index].clear_bytes += param_sets.size();
        }

        buffer->insert(config_insert_point,
            param_sets.begin(), param_sets.end());

        DCHECK(AVC::IsValidAnnexB(*buffer, *subsamples));
        return true;
    }

    // static
    bool AVC::ConvertConfigToAnnexB(
        const AVCDecoderConfigurationRecord& avc_config,
        std::vector<uint8>* buffer)
    {
        DCHECK(buffer->empty());
        buffer->clear();
        int total_size = 0;
        for (size_t i = 0; i < avc_config.sps_list.size(); i++)
            total_size += avc_config.sps_list[i].size() + kAnnexBStartCodeSize;
        for (size_t i = 0; i < avc_config.pps_list.size(); i++)
            total_size += avc_config.pps_list[i].size() + kAnnexBStartCodeSize;
        buffer->reserve(total_size);

        for (size_t i = 0; i < avc_config.sps_list.size(); i++) {
            buffer->insert(buffer->end(), kAnnexBStartCode,
                kAnnexBStartCode + kAnnexBStartCodeSize);
            buffer->insert(buffer->end(), avc_config.sps_list[i].begin(),
                avc_config.sps_list[i].end());
        }

        for (size_t i = 0; i < avc_config.pps_list.size(); i++) {
            buffer->insert(buffer->end(), kAnnexBStartCode,
                kAnnexBStartCode + kAnnexBStartCodeSize);
            buffer->insert(buffer->end(), avc_config.pps_list[i].begin(),
                avc_config.pps_list[i].end());
        }
        return true;
    }

    // Verifies AnnexB NALU order according to ISO/IEC 14496-10 Section 7.4.1.2.3
    bool AVC::IsValidAnnexB(const std::vector<uint8>& buffer,
        const std::vector<SubsampleEntry>& subsamples)
    {
        return IsValidAnnexB(&buffer[0], buffer.size(), subsamples);
    }

    bool AVC::IsValidAnnexB(const uint8* buffer, size_t size,
        const std::vector<SubsampleEntry>& subsamples)
    {
        DVLOG(1) << __FUNCTION__;
        DCHECK(buffer);

        if (size == 0)
            return true;

        H264Parser parser;
        parser.SetEncryptedStream(buffer, size, subsamples);

        typedef enum {
            kAUDAllowed,
            kBeforeFirstVCL, // VCL == nal_unit_types 1-5
            kAfterFirstVCL,
            kEOStreamAllowed,
            kNoMoreDataAllowed,
        } NALUOrderState;

        H264NALU nalu;
        NALUOrderState order_state = kAUDAllowed;
        int last_nalu_type = H264NALU::kUnspecified;
        bool done = false;
        while (!done) {
            switch (parser.AdvanceToNextNALU(&nalu)) {
            case H264Parser::kOk:
                DVLOG(1) << "nal_unit_type " << nalu.nal_unit_type;

                switch (nalu.nal_unit_type) {
                case H264NALU::kAUD:
                    if (order_state > kAUDAllowed) {
                        DVLOG(1) << "Unexpected AUD in order_state " << order_state;
                        return false;
                    }
                    order_state = kBeforeFirstVCL;
                    break;

                case H264NALU::kSEIMessage:
                case H264NALU::kReserved14:
                case H264NALU::kReserved15:
                case H264NALU::kReserved16:
                case H264NALU::kReserved17:
                case H264NALU::kReserved18:
                case H264NALU::kPPS:
                case H264NALU::kSPS:
                    if (order_state > kBeforeFirstVCL) {
                        DVLOG(1) << "Unexpected NALU type " << nalu.nal_unit_type
                                 << " in order_state " << order_state;
                        return false;
                    }
                    order_state = kBeforeFirstVCL;
                    break;

                case H264NALU::kSPSExt:
                    if (last_nalu_type != H264NALU::kSPS) {
                        DVLOG(1) << "SPS extension does not follow an SPS.";
                        return false;
                    }
                    break;

                case H264NALU::kNonIDRSlice:
                case H264NALU::kSliceDataA:
                case H264NALU::kSliceDataB:
                case H264NALU::kSliceDataC:
                case H264NALU::kIDRSlice:
                    if (order_state > kAfterFirstVCL) {
                        DVLOG(1) << "Unexpected VCL in order_state " << order_state;
                        return false;
                    }
                    order_state = kAfterFirstVCL;
                    break;

                case H264NALU::kCodedSliceAux:
                    if (order_state != kAfterFirstVCL) {
                        DVLOG(1) << "Unexpected extension in order_state " << order_state;
                        return false;
                    }
                    break;

                case H264NALU::kEOSeq:
                    if (order_state != kAfterFirstVCL) {
                        DVLOG(1) << "Unexpected EOSeq in order_state " << order_state;
                        return false;
                    }
                    order_state = kEOStreamAllowed;
                    break;

                case H264NALU::kEOStream:
                    if (order_state < kAfterFirstVCL) {
                        DVLOG(1) << "Unexpected EOStream in order_state " << order_state;
                        return false;
                    }
                    order_state = kNoMoreDataAllowed;
                    break;

                case H264NALU::kFiller:
                case H264NALU::kUnspecified:
                    if (!(order_state >= kAfterFirstVCL && order_state < kEOStreamAllowed)) {
                        DVLOG(1) << "Unexpected NALU type " << nalu.nal_unit_type
                                 << " in order_state " << order_state;
                        return false;
                    }
                    break;

                default:
                    DCHECK_GE(nalu.nal_unit_type, 20);
                    if (nalu.nal_unit_type >= 20 && nalu.nal_unit_type <= 31 && order_state != kAfterFirstVCL) {
                        DVLOG(1) << "Unexpected NALU type " << nalu.nal_unit_type
                                 << " in order_state " << order_state;
                        return false;
                    }
                }
                last_nalu_type = nalu.nal_unit_type;
                break;

            case H264Parser::kInvalidStream:
                return false;

            case H264Parser::kUnsupportedStream:
                NOTREACHED() << "AdvanceToNextNALU() returned kUnsupportedStream!";
                return false;

            case H264Parser::kEOStream:
                done = true;
            }
        }

        return order_state >= kAfterFirstVCL;
    }

    AVCBitstreamConverter::AVCBitstreamConverter(
        scoped_ptr<AVCDecoderConfigurationRecord> avc_config)
        : avc_config_(avc_config.Pass())
    {
        DCHECK(avc_config_);
    }

    AVCBitstreamConverter::~AVCBitstreamConverter()
    {
    }

    bool AVCBitstreamConverter::ConvertFrame(
        std::vector<uint8>* frame_buf,
        bool is_keyframe,
        std::vector<SubsampleEntry>* subsamples) const
    {
        // Convert the AVC NALU length fields to Annex B headers, as expected by
        // decoding libraries. Since this may enlarge the size of the buffer, we also
        // update the clear byte count for each subsample if encryption is used to
        // account for the difference in size between the length prefix and Annex B
        // start code.
        RCHECK(AVC::ConvertFrameToAnnexB(avc_config_->length_size, frame_buf,
            subsamples));

        if (is_keyframe) {
            // If this is a keyframe, we (re-)inject SPS and PPS headers at the start of
            // a frame. If subsample info is present, we also update the clear byte
            // count for that first subsample.
            RCHECK(AVC::InsertParamSetsAnnexB(*avc_config_, frame_buf, subsamples));
        }

        DCHECK(AVC::IsValidAnnexB(*frame_buf, *subsamples));
        return true;
    }

} // namespace mp4
} // namespace media
