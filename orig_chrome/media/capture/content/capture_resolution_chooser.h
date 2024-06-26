// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAPTURE_CAPTURE_RESOLUTION_CHOOSER_H_
#define MEDIA_CAPTURE_CAPTURE_RESOLUTION_CHOOSER_H_

#include <vector>

#include "media/base/media_export.h"
#include "media/base/video_capture_types.h"
#include "ui/gfx/geometry/size.h"

namespace media {

// Encapsulates the logic that determines the capture frame resolution based on:
//   1. The configured maximum frame resolution and resolution change policy.
//   2. Changes to resolution of the source content.
//   3. Changes to the (externally-computed) target data volume, provided in
//      terms of the number of pixels in the frame.
//
// CaptureResolutionChooser always computes capture sizes less than the maximum
// frame size, and adheres to the configured resolution change policy.  Within
// those hard limits, the capture size is computed to be as close to the
// targeted frame area as possible.
//
// In variable-resolution use cases, the capture sizes are "snapped" to a small
// (i.e., usually less than a dozen) set of possibilities.  This is to prevent
// the end-to-end system from having to deal with rapidly-changing video frame
// resolutions that results from providing a fine-grained range of values.  The
// possibile snapped frame sizes are computed relative to the resolution of the
// source content: They are the same or smaller in size, and are of the same
// aspect ratio.
class MEDIA_EXPORT CaptureResolutionChooser {
public:
    // media::ResolutionChangePolicy determines whether the variable frame
    // resolutions being computed must adhere to a fixed aspect ratio or not, or
    // that there must only be a single fixed resolution.
    CaptureResolutionChooser(const gfx::Size& max_frame_size,
        ResolutionChangePolicy resolution_change_policy);
    ~CaptureResolutionChooser();

    // Returns the current capture frame resolution to use.
    gfx::Size capture_size() const { return capture_size_; }

    // Updates the capture size based on a change in the resolution of the source
    // content.
    void SetSourceSize(const gfx::Size& source_size);

    // Updates the capture size to target the given frame area, in terms of
    // gfx::Size::GetArea().  The initial target frame area is the maximum int
    // (i.e., always target the source size).
    void SetTargetFrameArea(int area);

    // Search functions to, given a frame |area|, return the nearest snapped frame
    // size, or N size steps up/down.  Snapped frame sizes are based on the
    // current source size.
    gfx::Size FindNearestFrameSize(int area) const;
    gfx::Size FindLargerFrameSize(int area, int num_steps_up) const;
    gfx::Size FindSmallerFrameSize(int area, int num_steps_down) const;

private:
    // Called after any update that requires |capture_size_| be re-computed.
    void RecomputeCaptureSize();

    // Recomputes the |snapped_sizes_| cache.
    void UpdateSnappedFrameSizes(const gfx::Size& constrained_size);

    // Hard constraints.
    const gfx::Size max_frame_size_;
    const gfx::Size min_frame_size_; // Computed from the ctor arguments.

    // Specifies the set of heuristics to use.
    const ResolutionChangePolicy resolution_change_policy_;

    // |capture_size_| will be computed such that its area is as close to this
    // value as possible.
    int target_area_;

    // The current computed capture frame resolution.
    gfx::Size capture_size_;

    // Cache of the set of possible values |capture_size_| can have, in order from
    // smallest to largest.  This is recomputed whenever UpdateSnappedFrameSizes()
    // is called.
    std::vector<gfx::Size> snapped_sizes_;
};

} // namespace media

#endif // MEDIA_CAPTURE_RESOLUTION_CHOOSER_H_
