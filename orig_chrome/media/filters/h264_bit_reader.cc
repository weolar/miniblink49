// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/h264_bit_reader.h"
#include "base/logging.h"

namespace media {

H264BitReader::H264BitReader()
    : data_(NULL)
    , bytes_left_(0)
    , curr_byte_(0)
    , num_remaining_bits_in_curr_byte_(0)
    , prev_two_bytes_(0)
    , emulation_prevention_bytes_(0)
{
}

H264BitReader::~H264BitReader() { }

bool H264BitReader::Initialize(const uint8* data, off_t size)
{
    DCHECK(data);

    if (size < 1)
        return false;

    data_ = data;
    bytes_left_ = size;
    num_remaining_bits_in_curr_byte_ = 0;
    // Initially set to 0xffff to accept all initial two-byte sequences.
    prev_two_bytes_ = 0xffff;
    emulation_prevention_bytes_ = 0;

    return true;
}

bool H264BitReader::UpdateCurrByte()
{
    if (bytes_left_ < 1)
        return false;

    // Emulation prevention three-byte detection.
    // If a sequence of 0x000003 is found, skip (ignore) the last byte (0x03).
    if (*data_ == 0x03 && (prev_two_bytes_ & 0xffff) == 0) {
        // Detected 0x000003, skip last byte.
        ++data_;
        --bytes_left_;
        ++emulation_prevention_bytes_;
        // Need another full three bytes before we can detect the sequence again.
        prev_two_bytes_ = 0xffff;

        if (bytes_left_ < 1)
            return false;
    }

    // Load a new byte and advance pointers.
    curr_byte_ = *data_++ & 0xff;
    --bytes_left_;
    num_remaining_bits_in_curr_byte_ = 8;

    prev_two_bytes_ = (prev_two_bytes_ << 8) | curr_byte_;

    return true;
}

// Read |num_bits| (1 to 31 inclusive) from the stream and return them
// in |out|, with first bit in the stream as MSB in |out| at position
// (|num_bits| - 1).
bool H264BitReader::ReadBits(int num_bits, int* out)
{
    int bits_left = num_bits;
    *out = 0;
    DCHECK(num_bits <= 31);

    while (num_remaining_bits_in_curr_byte_ < bits_left) {
        // Take all that's left in current byte, shift to make space for the rest.
        *out |= (curr_byte_ << (bits_left - num_remaining_bits_in_curr_byte_));
        bits_left -= num_remaining_bits_in_curr_byte_;

        if (!UpdateCurrByte())
            return false;
    }

    *out |= (curr_byte_ >> (num_remaining_bits_in_curr_byte_ - bits_left));
    *out &= ((1 << num_bits) - 1);
    num_remaining_bits_in_curr_byte_ -= bits_left;

    return true;
}

off_t H264BitReader::NumBitsLeft()
{
    return (num_remaining_bits_in_curr_byte_ + bytes_left_ * 8);
}

bool H264BitReader::HasMoreRBSPData()
{
    // Make sure we have more bits, if we are at 0 bits in current byte
    // and updating current byte fails, we don't have more data anyway.
    if (num_remaining_bits_in_curr_byte_ == 0 && !UpdateCurrByte())
        return false;

    // On last byte?
    if (bytes_left_)
        return true;

    // Last byte, look for stop bit;
    // We have more RBSP data if the last non-zero bit we find is not the
    // first available bit.
    return (curr_byte_ & ((1 << (num_remaining_bits_in_curr_byte_ - 1)) - 1)) != 0;
}

size_t H264BitReader::NumEmulationPreventionBytesRead()
{
    return emulation_prevention_bytes_;
}

} // namespace media
