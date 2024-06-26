// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/data_buffer.h"

namespace media {

DataBuffer::DataBuffer(int buffer_size)
    : buffer_size_(buffer_size)
    , data_size_(0)
{
    CHECK_GE(buffer_size, 0);
    data_.reset(new uint8[buffer_size_]);
}

DataBuffer::DataBuffer(scoped_ptr<uint8[]> buffer, int buffer_size)
    : data_(buffer.Pass())
    , buffer_size_(buffer_size)
    , data_size_(buffer_size)
{
    CHECK(data_.get());
    CHECK_GE(buffer_size, 0);
}

DataBuffer::DataBuffer(const uint8* data, int data_size)
    : buffer_size_(data_size)
    , data_size_(data_size)
{
    if (!data) {
        CHECK_EQ(data_size, 0);
        return;
    }

    CHECK_GE(data_size, 0);
    data_.reset(new uint8[buffer_size_]);
    memcpy(data_.get(), data, data_size_);
}

DataBuffer::~DataBuffer() { }

// static
scoped_refptr<DataBuffer> DataBuffer::CopyFrom(const uint8* data, int size)
{
    // If you hit this CHECK you likely have a bug in a demuxer. Go fix it.
    CHECK(data);
    return make_scoped_refptr(new DataBuffer(data, size));
}

// static
scoped_refptr<DataBuffer> DataBuffer::CreateEOSBuffer()
{
    return make_scoped_refptr(new DataBuffer(NULL, 0));
}
} // namespace media
