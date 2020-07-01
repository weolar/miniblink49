// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef net_FixedReceivedData_h
#define net_FixedReceivedData_h

#include <vector>

#include "net/RequestPeer.h"

namespace net {

class FixedReceivedData final : public RequestPeer::ThreadSafeReceivedData {
public:
    FixedReceivedData(const char* data, size_t length, int encodedLength);
    explicit FixedReceivedData(ReceivedData* data);
    FixedReceivedData(const std::vector<char>& data, int encodedLength);
    ~FixedReceivedData() override;

    const char* payload() const override;
    int length() const override;
    int encodedLength() const override;

private:
    std::vector<char> data_;
    int m_encodedLength;

    DISALLOW_COPY_AND_ASSIGN(FixedReceivedData);
};

}  // namespace net

#endif  // net_FixedReceivedData_h
