// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/sender/sender_encoded_frame.h"

namespace media {
namespace cast {

    SenderEncodedFrame::SenderEncodedFrame()
        : EncodedFrame()
        , deadline_utilization(-1.0)
        , lossy_utilization(-1.0)
    {
    }

    SenderEncodedFrame::~SenderEncodedFrame() { }

} //  namespace cast
} //  namespace media
