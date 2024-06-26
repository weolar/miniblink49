// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_BIT_READER_H_
#define MEDIA_BASE_BIT_READER_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "media/base/bit_reader_core.h"
#include "media/base/media_export.h"

namespace media {

class MEDIA_EXPORT BitReader
    : NON_EXPORTED_BASE(private BitReaderCore::ByteStreamProvider) {
public:
    // Initialize the reader to start reading at |data|, |size| being size
    // of |data| in bytes.
    BitReader(const uint8* data, int size);
    ~BitReader() override;

    template <typename T>
    bool ReadBits(int num_bits, T* out)
    {
        return bit_reader_core_.ReadBits(num_bits, out);
    }

    bool ReadFlag(bool* flag)
    {
        return bit_reader_core_.ReadFlag(flag);
    }

    bool SkipBits(int num_bits)
    {
        return bit_reader_core_.SkipBits(num_bits);
    }

    int bits_available() const
    {
        return initial_size_ * 8 - bits_read();
    }

    int bits_read() const
    {
        return bit_reader_core_.bits_read();
    }

private:
    // BitReaderCore::ByteStreamProvider implementation.
    int GetBytes(int max_n, const uint8** out) override;

    // Total number of bytes that was initially passed to BitReader.
    const int initial_size_;

    // Pointer to the next unread byte in the stream.
    const uint8* data_;

    // Bytes left in the stream.
    int bytes_left_;

    BitReaderCore bit_reader_core_;

    DISALLOW_COPY_AND_ASSIGN(BitReader);
};

} // namespace media

#endif // MEDIA_BASE_BIT_READER_H_
