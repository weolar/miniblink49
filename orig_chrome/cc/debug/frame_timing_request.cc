// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/debug/frame_timing_request.h"

namespace cc {

FrameTimingRequest::FrameTimingRequest()
    : id_(0)
{
}

FrameTimingRequest::FrameTimingRequest(int64_t request_id,
    const gfx::Rect& rect)
    : id_(request_id)
    , rect_(rect)
{
}

} // namespace cc
