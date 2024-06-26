// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/DataBuffer.h"
#include <memory>

namespace media {

DataBuffer::DataBuffer(int buffer_size)
    : buffer_size_(buffer_size),
    data_size_(0)
{
    //CHECK_GE(buffer_size, 0);
    data_.reset(new uint8[buffer_size_]);
}

DataBuffer::DataBuffer(std::unique_ptr<uint8[]> buffer, int buffer_size)
    : data_(buffer.release()),
    buffer_size_(buffer_size),
    data_size_(buffer_size)
{
//     CHECK(data_.get());
//     CHECK_GE(buffer_size, 0);
}

DataBuffer::DataBuffer(const uint8* data, int data_size)
    : buffer_size_(data_size),
    data_size_(data_size)
{
    if (!data) {
        //CHECK_EQ(data_size, 0);
        return;
    }

    //CHECK_GE(data_size, 0);
    data_.reset(new uint8[buffer_size_]);
    memcpy(data_.get(), data, data_size_);
}

DataBuffer::~DataBuffer() {}

// static
std::unique_ptr<DataBuffer> DataBuffer::CopyFrom(const uint8* data, int size)
{
    // If you hit this CHECK you likely have a bug in a demuxer. Go fix it.
    //CHECK(data);
    return std::unique_ptr<DataBuffer>(new DataBuffer(data, size));
}

// static
std::unique_ptr<DataBuffer> DataBuffer::CreateEOSBuffer()
{
    return std::unique_ptr<DataBuffer>(new DataBuffer(NULL, 0));
}
}  // namespace media
