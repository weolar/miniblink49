// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/formats/mpeg/adts_stream_parser.h"

#include "media/formats/mpeg/adts_constants.h"

namespace media {

static const uint32 kADTSStartCodeMask = 0xfff00000;

ADTSStreamParser::ADTSStreamParser()
    : MPEGAudioStreamParserBase(kADTSStartCodeMask, kCodecAAC, 0)
{
}

ADTSStreamParser::~ADTSStreamParser() { }

int ADTSStreamParser::ParseFrameHeader(const uint8* data,
    int size,
    int* frame_size,
    int* sample_rate,
    ChannelLayout* channel_layout,
    int* sample_count,
    bool* metadata_frame) const
{
    DCHECK(data);
    DCHECK_GE(size, 0);
    DCHECK(frame_size);

    if (size < 8)
        return 0;

    BitReader reader(data, size);
    int sync;
    int version;
    int layer;
    int protection_absent;
    int profile;
    size_t sample_rate_index;
    size_t channel_layout_index;
    int frame_length;
    size_t num_data_blocks;
    int unused;

    if (!reader.ReadBits(12, &sync) || !reader.ReadBits(1, &version) || !reader.ReadBits(2, &layer) || !reader.ReadBits(1, &protection_absent) || !reader.ReadBits(2, &profile) || !reader.ReadBits(4, &sample_rate_index) || !reader.ReadBits(1, &unused) || !reader.ReadBits(3, &channel_layout_index) || !reader.ReadBits(4, &unused) || !reader.ReadBits(13, &frame_length) || !reader.ReadBits(11, &unused) || !reader.ReadBits(2, &num_data_blocks) || (!protection_absent && !reader.ReadBits(16, &unused))) {
        return -1;
    }

    DVLOG(2) << "Header data :" << std::hex
             << " sync 0x" << sync
             << " version 0x" << version
             << " layer 0x" << layer
             << " profile 0x" << profile
             << " sample_rate_index 0x" << sample_rate_index
             << " channel_layout_index 0x" << channel_layout_index;

    const int bytes_read = reader.bits_read() / 8;
    if (sync != 0xfff || layer != 0 || frame_length < bytes_read || sample_rate_index >= kADTSFrequencyTableSize || channel_layout_index >= kADTSChannelLayoutTableSize) {
        MEDIA_LOG(DEBUG, media_log())
            << "Invalid header data :" << std::hex << " sync 0x" << sync
            << " version 0x" << version << " layer 0x" << layer
            << " sample_rate_index 0x" << sample_rate_index
            << " channel_layout_index 0x" << channel_layout_index;
        return -1;
    }

    if (sample_rate)
        *sample_rate = kADTSFrequencyTable[sample_rate_index];

    if (frame_size)
        *frame_size = frame_length;

    if (sample_count)
        *sample_count = (num_data_blocks + 1) * kSamplesPerAACFrame;

    if (channel_layout)
        *channel_layout = kADTSChannelLayoutTable[channel_layout_index];

    if (metadata_frame)
        *metadata_frame = false;

    return bytes_read;
}

} // namespace media
