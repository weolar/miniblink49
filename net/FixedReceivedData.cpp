// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/FixedReceivedData.h"

namespace net {

FixedReceivedData::FixedReceivedData(const char* data, size_t length, int encodedLength)
    : m_data(data, data + length), m_encodedLength(encodedLength)
{
}

FixedReceivedData::FixedReceivedData(ReceivedData* data)
    : FixedReceivedData(data->payload(), data->length(), data->encodedLength())
{
}

FixedReceivedData::FixedReceivedData(const std::vector<char>& data, int encodedLength)
    : m_data(data), m_encodedLength(encodedLength)
{
}

FixedReceivedData::~FixedReceivedData()
{
}

const char* FixedReceivedData::payload() const
{
    return m_data.empty() ? nullptr : &m_data[0];
}

int FixedReceivedData::length() const
{
    return static_cast<int>(m_data.size());
}

int FixedReceivedData::encodedLength() const
{
    return m_encodedLength;
}

}  // namespace content
