// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FORMATS_MPEG_MPEG1_AUDIO_STREAM_PARSER_H_
#define MEDIA_FORMATS_MPEG_MPEG1_AUDIO_STREAM_PARSER_H_

#include "base/basictypes.h"
#include "media/base/media_export.h"
#include "media/formats/mpeg/mpeg_audio_stream_parser_base.h"

namespace media {

// MPEG1AudioStreamParser handles MPEG-1 audio streams (ISO/IEC 11172-3)
// as well as the following extensions:
// - MPEG-2 audio (ISO/IEC 13818-3),
// - and MPEG2.5 (not an ISO standard).
class MEDIA_EXPORT MPEG1AudioStreamParser : public MPEGAudioStreamParserBase {
public:
    // Size of an MPEG-1 frame header in bytes.
    enum {
        kHeaderSize = 4,
    };

    // Versions and layers as defined in ISO/IEC 11172-3.
    enum Version {
        kVersion1 = 3,
        kVersion2 = 2,
        kVersionReserved = 1,
        kVersion2_5 = 0,
    };

    enum Layer {
        kLayer1 = 3,
        kLayer2 = 2,
        kLayer3 = 1,
        kLayerReserved = 0,
    };

    struct Header {
        Version version;

        // Layer as defined in ISO/IEC 11172-3 bitstream specification.
        Layer layer;

        // Frame size in bytes.
        int frame_size;

        // Sample frequency.
        int sample_rate;

        // Channel mode as defined in ISO/IEC 11172-3 bitstream specification.
        int channel_mode;

        // Channel layout.
        ChannelLayout channel_layout;

        // Number of samples per frame.
        int sample_count;
    };

    // Parses the header starting at |data|.
    // Assumption: size of array |data| should be at least |kHeaderSize|.
    // Returns false if the header is not valid.
    static bool ParseHeader(const scoped_refptr<MediaLog>& media_log,
        const uint8* data,
        Header* header);

    MPEG1AudioStreamParser();
    ~MPEG1AudioStreamParser() override;

private:
    // MPEGAudioStreamParserBase overrides.
    int ParseFrameHeader(const uint8* data,
        int size,
        int* frame_size,
        int* sample_rate,
        ChannelLayout* channel_layout,
        int* sample_count,
        bool* metadata_frame) const override;

    DISALLOW_COPY_AND_ASSIGN(MPEG1AudioStreamParser);
};

} // namespace media

#endif // MEDIA_FORMATS_MPEG_MPEG1_AUDIO_STREAM_PARSER_H_
