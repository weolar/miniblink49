// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_VIDEO_UTIL_H_
#define MEDIA_BASE_VIDEO_UTIL_H_

#include "base/basictypes.h"
#include "media/base/media_export.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace media {

class VideoFrame;

// Computes the size of |visible_size| for a given aspect ratio.
MEDIA_EXPORT gfx::Size GetNaturalSize(const gfx::Size& visible_size,
    int aspect_ratio_numerator,
    int aspect_ratio_denominator);

// Copies a plane of YUV(A) source into a VideoFrame object, taking into account
// source and destinations dimensions.
//
// NOTE: rows is *not* the same as height!
MEDIA_EXPORT void CopyYPlane(const uint8* source, int stride, int rows,
    VideoFrame* frame);
MEDIA_EXPORT void CopyUPlane(const uint8* source, int stride, int rows,
    VideoFrame* frame);
MEDIA_EXPORT void CopyVPlane(const uint8* source, int stride, int rows,
    VideoFrame* frame);
MEDIA_EXPORT void CopyAPlane(const uint8* source, int stride, int rows,
    VideoFrame* frame);

// Sets alpha plane values to be completely opaque (all 255's).
MEDIA_EXPORT void MakeOpaqueAPlane(int stride, int rows, VideoFrame* frame);

// |plane| is one of VideoFrame::kYPlane, VideoFrame::kUPlane,
// VideoFrame::kVPlane or VideoFrame::kAPlane
MEDIA_EXPORT void CopyPlane(size_t plane, const uint8* source, int stride,
    int rows, VideoFrame* frame);

// Fills |frame| containing YUV data to the given color values.
MEDIA_EXPORT void FillYUV(VideoFrame* frame, uint8 y, uint8 u, uint8 v);

// Fills |frame| containing YUVA data with the given color values.
MEDIA_EXPORT void FillYUVA(VideoFrame* frame,
    uint8 y,
    uint8 u,
    uint8 v,
    uint8 a);

// Creates a border in |frame| such that all pixels outside of
// |view_area| are black. The size and position of |view_area|
// must be even to align correctly with the color planes.
// Only YV12 format video frames are currently supported.
MEDIA_EXPORT void LetterboxYUV(VideoFrame* frame,
    const gfx::Rect& view_area);

// Rotates |src| plane by |rotation| degree with possible flipping vertically
// and horizontally.
// |rotation| is limited to {0, 90, 180, 270}.
// |width| and |height| are expected to be even numbers.
// Both |src| and |dest| planes are packed and have same |width| and |height|.
// When |width| != |height| and rotated by 90/270, only the maximum square
// portion located in the center is rotated. For example, for width=640 and
// height=480, the rotated area is 480x480 located from row 0 through 479 and
// from column 80 through 559. The leftmost and rightmost 80 columns are
// ignored for both |src| and |dest|.
// The caller is responsible for blanking out the margin area.
MEDIA_EXPORT void RotatePlaneByPixels(
    const uint8* src,
    uint8* dest,
    int width,
    int height,
    int rotation, // Clockwise.
    bool flip_vert,
    bool flip_horiz);

// Return the largest centered rectangle with the same aspect ratio of |content|
// that fits entirely inside of |bounds|.  If |content| is empty, its aspect
// ratio would be undefined; and in this case an empty Rect would be returned.
MEDIA_EXPORT gfx::Rect ComputeLetterboxRegion(const gfx::Rect& bounds,
    const gfx::Size& content);

// Return a scaled |size| whose area is less than or equal to |target|, where
// one of its dimensions is equal to |target|'s.  The aspect ratio of |size| is
// preserved as closely as possible.  If |size| is empty, the result will be
// empty.
MEDIA_EXPORT gfx::Size ScaleSizeToFitWithinTarget(const gfx::Size& size,
    const gfx::Size& target);

// Return a scaled |size| whose area is greater than or equal to |target|, where
// one of its dimensions is equal to |target|'s.  The aspect ratio of |size| is
// preserved as closely as possible.  If |size| is empty, the result will be
// empty.
MEDIA_EXPORT gfx::Size ScaleSizeToEncompassTarget(const gfx::Size& size,
    const gfx::Size& target);

// Returns |size| with only one of its dimensions increased such that the result
// matches the aspect ratio of |target|.  This is different from
// ScaleSizeToEncompassTarget() in two ways: 1) The goal is to match the aspect
// ratio of |target| rather than that of |size|.  2) Only one of the dimensions
// of |size| may change, and it may only be increased (padded).  If either
// |size| or |target| is empty, the result will be empty.
MEDIA_EXPORT gfx::Size PadToMatchAspectRatio(const gfx::Size& size,
    const gfx::Size& target);

// Copy an RGB bitmap into the specified |region_in_frame| of a YUV video frame.
// Fills the regions outside |region_in_frame| with black.
MEDIA_EXPORT void CopyRGBToVideoFrame(const uint8* source,
    int stride,
    const gfx::Rect& region_in_frame,
    VideoFrame* frame);

} // namespace media

#endif // MEDIA_BASE_VIDEO_UTIL_H_
