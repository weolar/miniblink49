// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/FixedReceivedData.h"

namespace net {

FixedReceivedData::FixedReceivedData(const char* data, size_t length, int encodedLength)
    : data_(data, data + length), m_encodedLength(encodedLength)
{
}

FixedReceivedData::FixedReceivedData(ReceivedData* data)
    : FixedReceivedData(data->payload(), data->length(), data->encodedLength())
{
}

FixedReceivedData::FixedReceivedData(const std::vector<char>& data, int encodedLength)
    : data_(data), m_encodedLength(encodedLength)
{
}

FixedReceivedData::~FixedReceivedData()
{
}

const char* FixedReceivedData::payload() const
{
    return data_.empty() ? nullptr : &data_[0];
}

int FixedReceivedData::length() const
{
    return static_cast<int>(data_.size());
}

int FixedReceivedData::encodedLength() const
{
    return m_encodedLength;
}

}  // namespace content
