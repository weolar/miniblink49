// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/bit_reader.h"

namespace media {

BitReader::BitReader(const uint8* data, int size)
    : initial_size_(size)
    , data_(data)
    , bytes_left_(size)
    , bit_reader_core_(this)
{
    DCHECK(data != NULL);
    DCHECK_GE(size, 0);
}

BitReader::~BitReader() { }

int BitReader::GetBytes(int max_nbytes, const uint8** out)
{
    DCHECK_GE(max_nbytes, 0);
    DCHECK(out);

    int nbytes = max_nbytes;
    if (nbytes > bytes_left_)
        nbytes = bytes_left_;

    *out = data_;
    data_ += nbytes;
    bytes_left_ -= nbytes;
    return nbytes;
}

} // namespace media
