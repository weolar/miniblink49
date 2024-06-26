// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_FRAME_TIMING_REQUEST_H_
#define CC_DEBUG_FRAME_TIMING_REQUEST_H_

#include "cc/base/cc_export.h"
#include "ui/gfx/geometry/rect.h"

namespace cc {

// This class represents a request to record frame timing information about the
// given rect (in layer space) and an associated request id. When this request
// is propagated to the active LayerImpl, it will cause events to be saved in
// FrameTimingTracker, which in turn can be consumed by the requester.
class CC_EXPORT FrameTimingRequest {
public:
    FrameTimingRequest();
    FrameTimingRequest(int64_t request_id, const gfx::Rect& rect);

    // Return the ID for the request.
    int64_t id() const { return id_; }

    // Return the layer space rect for this request.
    const gfx::Rect& rect() const { return rect_; }

    bool operator==(const FrameTimingRequest& other) const
    {
        return (id_ == other.id_) && (rect_ == other.rect_);
    }

private:
    int64_t id_;
    gfx::Rect rect_;
};

} // namespace cc

#endif // CC_DEBUG_FRAME_TIMING_REQUEST_H_
