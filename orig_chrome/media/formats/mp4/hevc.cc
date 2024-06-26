// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/formats/mp4/hevc.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "base/logging.h"
#include "media/base/decrypt_config.h"
#include "media/filters/h265_parser.h"
#include "media/formats/mp4/avc.h"
#include "media/formats/mp4/box_definitions.h"
#include "media/formats/mp4/box_reader.h"

namespace media {
namespace mp4 {

    HEVCDecoderConfigurationRecord::HEVCDecoderConfigurationRecord()
        : configurationVersion(0)
        , general_profile_space(0)
        , general_tier_flag(0)
        , general_profile_idc(0)
        , general_profile_compatibility_flags(0)
        , general_constraint_indicator_flags(0)
        , general_level_idc(0)
        , min_spatial_segmentation_idc(0)
        , parallelismType(0)
        , chromaFormat(0)
        , bitDepthLumaMinus8(0)
        , bitDepthChromaMinus8(0)
        , avgFrameRate(0)
        , constantFrameRate(0)
        , numTemporalLayers(0)
        , temporalIdNested(0)
        , lengthSizeMinusOne(0)
        , numOfArrays(0)
    {
    }

    HEVCDecoderConfigurationRecord::~HEVCDecoderConfigurationRecord() { }
    FourCC HEVCDecoderConfigurationRecord::BoxType() const { return FOURCC_HVCC; }

    bool HEVCDecoderConfigurationRecord::Parse(BoxReader* reader)
    {
        return ParseInternal(reader, reader->media_log());
    }

    bool HEVCDecoderConfigurationRecord::Parse(const uint8_t* data, int data_size)
    {
        BufferReader reader(data, data_size);
        return ParseInternal(&reader, new MediaLog());
    }

    HEVCDecoderConfigurationRecord::HVCCNALArray::HVCCNALArray()
        : first_byte(0)
    {
    }

    HEVCDecoderConfigurationRecord::HVCCNALArray::~HVCCNALArray() { }

    bool HEVCDecoderConfigurationRecord::ParseInternal(
        BufferReader* reader,
        const scoped_refptr<MediaLog>& media_log)
    {
        uint8_t profile_indication = 0;
        uint32_t general_constraint_indicator_flags_hi = 0;
        uint16_t general_constraint_indicator_flags_lo = 0;
        uint8_t misc = 0;
        RCHECK(reader->Read1(&configurationVersion) && configurationVersion == 1 && reader->Read1(&profile_indication) && reader->Read4(&general_profile_compatibility_flags) && reader->Read4(&general_constraint_indicator_flags_hi) && reader->Read2(&general_constraint_indicator_flags_lo) && reader->Read1(&general_level_idc) && reader->Read2(&min_spatial_segmentation_idc) && reader->Read1(&parallelismType) && reader->Read1(&chromaFormat) && reader->Read1(&bitDepthLumaMinus8) && reader->Read1(&bitDepthChromaMinus8) && reader->Read2(&avgFrameRate) && reader->Read1(&misc) && reader->Read1(&numOfArrays));

        general_profile_space = profile_indication >> 6;
        general_tier_flag = (profile_indication >> 5) & 1;
        general_profile_idc = profile_indication & 0x1f;

        general_constraint_indicator_flags = general_constraint_indicator_flags_hi;
        general_constraint_indicator_flags <<= 16;
        general_constraint_indicator_flags |= general_constraint_indicator_flags_lo;

        min_spatial_segmentation_idc &= 0xfff;
        parallelismType &= 3;
        chromaFormat &= 3;
        bitDepthLumaMinus8 &= 7;
        bitDepthChromaMinus8 &= 7;

        constantFrameRate = misc >> 6;
        numTemporalLayers = (misc >> 3) & 7;
        temporalIdNested = (misc >> 2) & 1;
        lengthSizeMinusOne = misc & 3;

        DVLOG(2) << __FUNCTION__ << " numOfArrays=" << (int)numOfArrays;
        arrays.resize(numOfArrays);
        for (uint32_t j = 0; j < numOfArrays; j++) {
            RCHECK(reader->Read1(&arrays[j].first_byte));
            uint16_t numNalus = 0;
            RCHECK(reader->Read2(&numNalus));
            arrays[j].units.resize(numNalus);
            for (uint32_t i = 0; i < numNalus; ++i) {
                uint16_t naluLength = 0;
                RCHECK(reader->Read2(&naluLength) && reader->ReadVec(&arrays[j].units[i], naluLength));
                DVLOG(4) << __FUNCTION__ << " naluType="
                         << (int)(arrays[j].first_byte & 0x3f)
                         << " size=" << arrays[j].units[i].size();
            }
        }

        if (media_log.get()) {
            MEDIA_LOG(INFO, media_log) << "Video codec: hevc";
        }

        return true;
    }

    static const uint8_t kAnnexBStartCode[] = { 0, 0, 0, 1 };
    static const int kAnnexBStartCodeSize = 4;

    bool HEVC::InsertParamSetsAnnexB(
        const HEVCDecoderConfigurationRecord& hevc_config,
        std::vector<uint8_t>* buffer,
        std::vector<SubsampleEntry>* subsamples)
    {
        DCHECK(HEVC::IsValidAnnexB(*buffer, *subsamples));

        scoped_ptr<H265Parser> parser(new H265Parser());
        const uint8_t* start = &(*buffer)[0];
        parser->SetEncryptedStream(start, buffer->size(), *subsamples);

        H265NALU nalu;
        if (parser->AdvanceToNextNALU(&nalu) != H265Parser::kOk)
            return false;

        std::vector<uint8_t>::iterator config_insert_point = buffer->begin();

        if (nalu.nal_unit_type == H265NALU::AUD_NUT) {
            // Move insert point to just after the AUD.
            config_insert_point += (nalu.data + nalu.size) - start;
        }

        // Clear |parser| and |start| since they aren't needed anymore and
        // will hold stale pointers once the insert happens.
        parser.reset();
        start = NULL;

        std::vector<uint8_t> param_sets;
        RCHECK(HEVC::ConvertConfigToAnnexB(hevc_config, &param_sets));
        DVLOG(4) << __FUNCTION__ << " converted hvcC to AnnexB "
                 << " size=" << param_sets.size() << " inserted at "
                 << (int)(config_insert_point - buffer->begin());

        if (subsamples && !subsamples->empty()) {
            int subsample_index = AVC::FindSubsampleIndex(*buffer, subsamples,
                &(*config_insert_point));
            // Update the size of the subsample where SPS/PPS is to be inserted.
            (*subsamples)[subsample_index].clear_bytes += param_sets.size();
        }

        buffer->insert(config_insert_point,
            param_sets.begin(), param_sets.end());

        DCHECK(HEVC::IsValidAnnexB(*buffer, *subsamples));
        return true;
    }

    bool HEVC::ConvertConfigToAnnexB(
        const HEVCDecoderConfigurationRecord& hevc_config,
        std::vector<uint8_t>* buffer)
    {
        DCHECK(buffer->empty());
        buffer->clear();

        for (size_t j = 0; j < hevc_config.arrays.size(); j++) {
            uint8_t naluType = hevc_config.arrays[j].first_byte & 0x3f;
            for (size_t i = 0; i < hevc_config.arrays[j].units.size(); ++i) {
                DVLOG(3) << __FUNCTION__ << " naluType=" << (int)naluType
                         << " size=" << hevc_config.arrays[j].units[i].size();
                buffer->insert(buffer->end(), kAnnexBStartCode,
                    kAnnexBStartCode + kAnnexBStartCodeSize);
                buffer->insert(buffer->end(), hevc_config.arrays[j].units[i].begin(),
                    hevc_config.arrays[j].units[i].end());
            }
        }

        return true;
    }

    // Verifies AnnexB NALU order according to section 7.4.2.4.4 of ISO/IEC 23008-2.
    bool HEVC::IsValidAnnexB(const std::vector<uint8_t>& buffer,
        const std::vector<SubsampleEntry>& subsamples)
    {
        return IsValidAnnexB(&buffer[0], buffer.size(), subsamples);
    }

    bool HEVC::IsValidAnnexB(const uint8_t* buffer,
        size_t size,
        const std::vector<SubsampleEntry>& subsamples)
    {
        DCHECK(buffer);

        if (size == 0)
            return true;

        // TODO(servolk): Implement this, see crbug.com/527595
        return true;
    }

    HEVCBitstreamConverter::HEVCBitstreamConverter(
        scoped_ptr<HEVCDecoderConfigurationRecord> hevc_config)
        : hevc_config_(std::move(hevc_config))
    {
        DCHECK(hevc_config_);
    }

    HEVCBitstreamConverter::~HEVCBitstreamConverter()
    {
    }

    bool HEVCBitstreamConverter::ConvertFrame(
        std::vector<uint8_t>* frame_buf,
        bool is_keyframe,
        std::vector<SubsampleEntry>* subsamples) const
    {
        RCHECK(AVC::ConvertFrameToAnnexB(hevc_config_->lengthSizeMinusOne + 1,
            frame_buf, subsamples));

        if (is_keyframe) {
            // If this is a keyframe, we (re-)inject HEVC params headers at the start of
            // a frame. If subsample info is present, we also update the clear byte
            // count for that first subsample.
            RCHECK(HEVC::InsertParamSetsAnnexB(*hevc_config_, frame_buf, subsamples));
        }

        DCHECK(HEVC::IsValidAnnexB(*frame_buf, *subsamples));
        return true;
    }

} // namespace mp4
} // namespace media
