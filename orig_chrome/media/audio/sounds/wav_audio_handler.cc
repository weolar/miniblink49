// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/sounds/wav_audio_handler.h"

#include <algorithm>
#include <cstring>

#include "base/logging.h"
#include "base/sys_byteorder.h"
#include "media/base/audio_bus.h"

namespace {

const char kChunkId[] = "RIFF";
const char kFormat[] = "WAVE";
const char kSubchunk1Id[] = "fmt ";
const char kSubchunk2Id[] = "data";

// The size of the header of a wav file. The header consists of 'RIFF', 4 bytes
// of total data length, and 'WAVE'.
const size_t kWavFileHeaderSize = 12;

// The size of a chunk header in wav file format. A chunk header consists of a
// tag ('fmt ' or 'data') and 4 bytes of chunk length.
const size_t kChunkHeaderSize = 8;

// The minimum size of 'fmt' chunk.
const size_t kFmtChunkMinimumSize = 16;

// The offsets of 'fmt' fields.
const size_t kAudioFormatOffset = 0;
const size_t kChannelOffset = 2;
const size_t kSampleRateOffset = 4;
const size_t kBitsPerSampleOffset = 14;

// Some constants for audio format.
const int kAudioFormatPCM = 1;

// Reads an integer from |data| with |offset|.
template <typename T>
T ReadInt(const base::StringPiece& data, size_t offset)
{
    CHECK_LE(offset + sizeof(T), data.size());
    T result;
    memcpy(&result, data.data() + offset, sizeof(T));
#if !defined(ARCH_CPU_LITTLE_ENDIAN)
    result = base::ByteSwap(result);
#endif
    return result;
}

} // namespace

namespace media {

WavAudioHandler::WavAudioHandler(const base::StringPiece& wav_data)
    : num_channels_(0)
    , sample_rate_(0)
    , bits_per_sample_(0)
    , total_frames_(0)
{
    CHECK_LE(kWavFileHeaderSize, wav_data.size()) << "wav data is too small";
    CHECK(wav_data.starts_with(kChunkId) && memcmp(wav_data.data() + 8, kFormat, 4) == 0)
        << "incorrect wav header";

    uint32 total_length = std::min(ReadInt<uint32>(wav_data, 4),
        static_cast<uint32>(wav_data.size()));
    uint32 offset = kWavFileHeaderSize;
    while (offset < total_length) {
        const int length = ParseSubChunk(wav_data.substr(offset));
        CHECK_LE(0, length) << "can't parse wav sub-chunk";
        offset += length;
    }

    total_frames_ = data_.size() * 8 / num_channels_ / bits_per_sample_;
}

WavAudioHandler::~WavAudioHandler() { }

bool WavAudioHandler::AtEnd(size_t cursor) const
{
    return data_.size() <= cursor;
}

bool WavAudioHandler::CopyTo(AudioBus* bus,
    size_t cursor,
    size_t* bytes_written) const
{
    if (!bus)
        return false;
    if (bus->channels() != num_channels_) {
        DVLOG(1) << "Number of channels mismatch.";
        return false;
    }
    if (AtEnd(cursor)) {
        bus->Zero();
        return true;
    }
    const int bytes_per_frame = num_channels_ * bits_per_sample_ / 8;
    const int remaining_frames = (data_.size() - cursor) / bytes_per_frame;
    const int frames = std::min(bus->frames(), remaining_frames);

    bus->FromInterleaved(data_.data() + cursor, frames, bits_per_sample_ / 8);
    *bytes_written = frames * bytes_per_frame;
    bus->ZeroFramesPartial(frames, bus->frames() - frames);
    return true;
}

base::TimeDelta WavAudioHandler::GetDuration() const
{
    return base::TimeDelta::FromSecondsD(total_frames_ / static_cast<double>(sample_rate_));
}

int WavAudioHandler::ParseSubChunk(const base::StringPiece& data)
{
    if (data.size() < kChunkHeaderSize)
        return data.size();
    uint32 chunk_length = ReadInt<uint32>(data, 4);
    if (data.starts_with(kSubchunk1Id)) {
        if (!ParseFmtChunk(data.substr(kChunkHeaderSize, chunk_length)))
            return -1;
    } else if (data.starts_with(kSubchunk2Id)) {
        if (!ParseDataChunk(data.substr(kChunkHeaderSize, chunk_length)))
            return -1;
    } else {
        DVLOG(1) << "Unknown data chunk: " << data.substr(0, 4) << ".";
    }
    return chunk_length + kChunkHeaderSize;
}

bool WavAudioHandler::ParseFmtChunk(const base::StringPiece& data)
{
    if (data.size() < kFmtChunkMinimumSize) {
        DLOG(ERROR) << "Data size " << data.size() << " is too short.";
        return false;
    }
    DCHECK_EQ(ReadInt<uint16>(data, kAudioFormatOffset), kAudioFormatPCM);
    num_channels_ = ReadInt<uint16>(data, kChannelOffset);
    sample_rate_ = ReadInt<uint32>(data, kSampleRateOffset);
    bits_per_sample_ = ReadInt<uint16>(data, kBitsPerSampleOffset);
    return true;
}

bool WavAudioHandler::ParseDataChunk(const base::StringPiece& data)
{
    data_ = data;
    return true;
}

} // namespace media
