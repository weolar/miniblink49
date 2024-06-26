// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FORMATS_MP4_BOX_DEFINITIONS_H_
#define MEDIA_FORMATS_MP4_BOX_DEFINITIONS_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "media/base/media_export.h"
#include "media/base/media_log.h"
#include "media/base/video_codecs.h"
#include "media/formats/mp4/aac.h"
#include "media/formats/mp4/avc.h"
#include "media/formats/mp4/box_reader.h"
#include "media/formats/mp4/fourccs.h"

namespace media {
namespace mp4 {

    enum TrackType {
        kInvalid = 0,
        kVideo,
        kAudio,
        kHint
    };

    enum SampleFlags {
        kSampleIsNonSyncSample = 0x10000
    };

#define DECLARE_BOX_METHODS(T)              \
    T();                                    \
    ~T() override;                          \
    bool Parse(BoxReader* reader) override; \
    FourCC BoxType() const override;

    struct MEDIA_EXPORT FileType : Box {
        DECLARE_BOX_METHODS(FileType);

        FourCC major_brand;
        uint32 minor_version;
    };

    // If only copying the 'pssh' boxes, use ProtectionSystemSpecificHeader.
    // If access to the individual fields is needed, use
    // FullProtectionSystemSpecificHeader.
    struct MEDIA_EXPORT ProtectionSystemSpecificHeader : Box {
        DECLARE_BOX_METHODS(ProtectionSystemSpecificHeader);

        std::vector<uint8> raw_box;
    };

    struct MEDIA_EXPORT FullProtectionSystemSpecificHeader : Box {
        DECLARE_BOX_METHODS(FullProtectionSystemSpecificHeader);

        std::vector<uint8> system_id;
        std::vector<std::vector<uint8>> key_ids;
        std::vector<uint8> data;
    };

    struct MEDIA_EXPORT SampleAuxiliaryInformationOffset : Box {
        DECLARE_BOX_METHODS(SampleAuxiliaryInformationOffset);

        std::vector<uint64> offsets;
    };

    struct MEDIA_EXPORT SampleAuxiliaryInformationSize : Box {
        DECLARE_BOX_METHODS(SampleAuxiliaryInformationSize);

        uint8 default_sample_info_size;
        uint32 sample_count;
        std::vector<uint8> sample_info_sizes;
    };

    struct MEDIA_EXPORT OriginalFormat : Box {
        DECLARE_BOX_METHODS(OriginalFormat);

        FourCC format;
    };

    struct MEDIA_EXPORT SchemeType : Box {
        DECLARE_BOX_METHODS(SchemeType);

        FourCC type;
        uint32 version;
    };

    struct MEDIA_EXPORT TrackEncryption : Box {
        DECLARE_BOX_METHODS(TrackEncryption);

        // Note: this definition is specific to the CENC protection type.
        bool is_encrypted;
        uint8 default_iv_size;
        std::vector<uint8> default_kid;
    };

    struct MEDIA_EXPORT SchemeInfo : Box {
        DECLARE_BOX_METHODS(SchemeInfo);

        TrackEncryption track_encryption;
    };

    struct MEDIA_EXPORT ProtectionSchemeInfo : Box {
        DECLARE_BOX_METHODS(ProtectionSchemeInfo);

        OriginalFormat format;
        SchemeType type;
        SchemeInfo info;
    };

    struct MEDIA_EXPORT MovieHeader : Box {
        DECLARE_BOX_METHODS(MovieHeader);

        uint64 creation_time;
        uint64 modification_time;
        uint32 timescale;
        uint64 duration;
        int32 rate;
        int16 volume;
        uint32 next_track_id;
    };

    struct MEDIA_EXPORT TrackHeader : Box {
        DECLARE_BOX_METHODS(TrackHeader);

        uint64 creation_time;
        uint64 modification_time;
        uint32 track_id;
        uint64 duration;
        int16 layer;
        int16 alternate_group;
        int16 volume;
        uint32 width;
        uint32 height;
    };

    struct MEDIA_EXPORT EditListEntry {
        uint64 segment_duration;
        int64 media_time;
        int16 media_rate_integer;
        int16 media_rate_fraction;
    };

    struct MEDIA_EXPORT EditList : Box {
        DECLARE_BOX_METHODS(EditList);

        std::vector<EditListEntry> edits;
    };

    struct MEDIA_EXPORT Edit : Box {
        DECLARE_BOX_METHODS(Edit);

        EditList list;
    };

    struct MEDIA_EXPORT HandlerReference : Box {
        DECLARE_BOX_METHODS(HandlerReference);

        TrackType type;
    };

    struct MEDIA_EXPORT AVCDecoderConfigurationRecord : Box {
        DECLARE_BOX_METHODS(AVCDecoderConfigurationRecord);

        // Parses AVCDecoderConfigurationRecord data encoded in |data|.
        // Note: This method is intended to parse data outside the MP4StreamParser
        //       context and therefore the box header is not expected to be present
        //       in |data|.
        // Returns true if |data| was successfully parsed.
        bool Parse(const uint8* data, int data_size);

        uint8 version;
        uint8 profile_indication;
        uint8 profile_compatibility;
        uint8 avc_level;
        uint8 length_size;

        typedef std::vector<uint8> SPS;
        typedef std::vector<uint8> PPS;

        std::vector<SPS> sps_list;
        std::vector<PPS> pps_list;

    private:
        bool ParseInternal(BufferReader* reader,
            const scoped_refptr<MediaLog>& media_log);
    };

    struct MEDIA_EXPORT PixelAspectRatioBox : Box {
        DECLARE_BOX_METHODS(PixelAspectRatioBox);

        uint32 h_spacing;
        uint32 v_spacing;
    };

    struct MEDIA_EXPORT VideoSampleEntry : Box {
        DECLARE_BOX_METHODS(VideoSampleEntry);

        FourCC format;
        uint16 data_reference_index;
        uint16 width;
        uint16 height;

        PixelAspectRatioBox pixel_aspect;
        ProtectionSchemeInfo sinf;

        VideoCodec video_codec;
        VideoCodecProfile video_codec_profile;

        bool IsFormatValid() const;

        scoped_refptr<BitstreamConverter> frame_bitstream_converter;
    };

    struct MEDIA_EXPORT ElementaryStreamDescriptor : Box {
        DECLARE_BOX_METHODS(ElementaryStreamDescriptor);

        uint8 object_type;
        AAC aac;
    };

    struct MEDIA_EXPORT AudioSampleEntry : Box {
        DECLARE_BOX_METHODS(AudioSampleEntry);

        FourCC format;
        uint16 data_reference_index;
        uint16 channelcount;
        uint16 samplesize;
        uint32 samplerate;

        ProtectionSchemeInfo sinf;
        ElementaryStreamDescriptor esds;
    };

    struct MEDIA_EXPORT SampleDescription : Box {
        DECLARE_BOX_METHODS(SampleDescription);

        TrackType type;
        std::vector<VideoSampleEntry> video_entries;
        std::vector<AudioSampleEntry> audio_entries;
    };

    struct MEDIA_EXPORT CencSampleEncryptionInfoEntry {
        CencSampleEncryptionInfoEntry();
        ~CencSampleEncryptionInfoEntry();

        bool is_encrypted;
        uint8 iv_size;
        std::vector<uint8> key_id;
    };

    struct MEDIA_EXPORT SampleGroupDescription : Box { // 'sgpd'.
        DECLARE_BOX_METHODS(SampleGroupDescription);

        uint32 grouping_type;
        std::vector<CencSampleEncryptionInfoEntry> entries;
    };

    struct MEDIA_EXPORT SampleTable : Box {
        DECLARE_BOX_METHODS(SampleTable);

        // Media Source specific: we ignore many of the sub-boxes in this box,
        // including some that are required to be present in the BMFF spec. This
        // includes the 'stts', 'stsc', and 'stco' boxes, which must contain no
        // samples in order to be compliant files.
        SampleDescription description;
        SampleGroupDescription sample_group_description;
    };

    struct MEDIA_EXPORT MediaHeader : Box {
        DECLARE_BOX_METHODS(MediaHeader);

        uint64 creation_time;
        uint64 modification_time;
        uint32 timescale;
        uint64 duration;
    };

    struct MEDIA_EXPORT MediaInformation : Box {
        DECLARE_BOX_METHODS(MediaInformation);

        SampleTable sample_table;
    };

    struct MEDIA_EXPORT Media : Box {
        DECLARE_BOX_METHODS(Media);

        MediaHeader header;
        HandlerReference handler;
        MediaInformation information;
    };

    struct MEDIA_EXPORT Track : Box {
        DECLARE_BOX_METHODS(Track);

        TrackHeader header;
        Media media;
        Edit edit;
    };

    struct MEDIA_EXPORT MovieExtendsHeader : Box {
        DECLARE_BOX_METHODS(MovieExtendsHeader);

        uint64 fragment_duration;
    };

    struct MEDIA_EXPORT TrackExtends : Box {
        DECLARE_BOX_METHODS(TrackExtends);

        uint32 track_id;
        uint32 default_sample_description_index;
        uint32 default_sample_duration;
        uint32 default_sample_size;
        uint32 default_sample_flags;
    };

    struct MEDIA_EXPORT MovieExtends : Box {
        DECLARE_BOX_METHODS(MovieExtends);

        MovieExtendsHeader header;
        std::vector<TrackExtends> tracks;
    };

    struct MEDIA_EXPORT Movie : Box {
        DECLARE_BOX_METHODS(Movie);

        bool fragmented;
        MovieHeader header;
        MovieExtends extends;
        std::vector<Track> tracks;
        std::vector<ProtectionSystemSpecificHeader> pssh;
    };

    struct MEDIA_EXPORT TrackFragmentDecodeTime : Box {
        DECLARE_BOX_METHODS(TrackFragmentDecodeTime);

        uint64 decode_time;
    };

    struct MEDIA_EXPORT MovieFragmentHeader : Box {
        DECLARE_BOX_METHODS(MovieFragmentHeader);

        uint32 sequence_number;
    };

    struct MEDIA_EXPORT TrackFragmentHeader : Box {
        DECLARE_BOX_METHODS(TrackFragmentHeader);

        uint32 track_id;

        uint32 sample_description_index;
        uint32 default_sample_duration;
        uint32 default_sample_size;
        uint32 default_sample_flags;

        // As 'flags' might be all zero, we cannot use zeroness alone to identify
        // when default_sample_flags wasn't specified, unlike the other values.
        bool has_default_sample_flags;
    };

    struct MEDIA_EXPORT TrackFragmentRun : Box {
        DECLARE_BOX_METHODS(TrackFragmentRun);

        uint32 sample_count;
        uint32 data_offset;
        std::vector<uint32> sample_flags;
        std::vector<uint32> sample_sizes;
        std::vector<uint32> sample_durations;
        std::vector<int32> sample_composition_time_offsets;
    };

    // sample_depends_on values in ISO/IEC 14496-12 Section 8.40.2.3.
    enum SampleDependsOn {
        kSampleDependsOnUnknown = 0,
        kSampleDependsOnOthers = 1,
        kSampleDependsOnNoOther = 2,
        kSampleDependsOnReserved = 3,
    };

    class MEDIA_EXPORT IndependentAndDisposableSamples : public Box {
    public:
        DECLARE_BOX_METHODS(IndependentAndDisposableSamples);

        // Returns the SampleDependsOn value for the |i|'th value
        // in the track. If no data was parsed for the |i|'th sample,
        // then |kSampleDependsOnUnknown| is returned.
        SampleDependsOn sample_depends_on(size_t i) const;

    private:
        std::vector<SampleDependsOn> sample_depends_on_;
    };

    struct MEDIA_EXPORT SampleToGroupEntry {
        enum GroupDescriptionIndexBase {
            kTrackGroupDescriptionIndexBase = 0,
            kFragmentGroupDescriptionIndexBase = 0x10000,
        };

        uint32 sample_count;
        uint32 group_description_index;
    };

    struct MEDIA_EXPORT SampleToGroup : Box { // 'sbgp'.
        DECLARE_BOX_METHODS(SampleToGroup);

        uint32 grouping_type;
        uint32 grouping_type_parameter; // Version 1 only.
        std::vector<SampleToGroupEntry> entries;
    };

    struct MEDIA_EXPORT TrackFragment : Box {
        DECLARE_BOX_METHODS(TrackFragment);

        TrackFragmentHeader header;
        std::vector<TrackFragmentRun> runs;
        TrackFragmentDecodeTime decode_time;
        SampleAuxiliaryInformationOffset auxiliary_offset;
        SampleAuxiliaryInformationSize auxiliary_size;
        IndependentAndDisposableSamples sdtp;
        SampleGroupDescription sample_group_description;
        SampleToGroup sample_to_group;
    };

    struct MEDIA_EXPORT MovieFragment : Box {
        DECLARE_BOX_METHODS(MovieFragment);

        MovieFragmentHeader header;
        std::vector<TrackFragment> tracks;
        std::vector<ProtectionSystemSpecificHeader> pssh;
    };

#undef DECLARE_BOX

} // namespace mp4
} // namespace media

#endif // MEDIA_FORMATS_MP4_BOX_DEFINITIONS_H_
