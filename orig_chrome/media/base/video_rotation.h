// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_VIDEO_ROTATION_H_
#define MEDIA_BASE_VIDEO_ROTATION_H_

namespace media {

// Enumeration to represent 90 degree video rotation for MP4 videos
// where it can be rotated by 90 degree intervals.
enum VideoRotation {
    VIDEO_ROTATION_0 = 0,
    VIDEO_ROTATION_90,
    VIDEO_ROTATION_180,
    VIDEO_ROTATION_270,
    VIDEO_ROTATION_MAX = VIDEO_ROTATION_270
};

} // namespace media

#endif // MEDIA_BASE_VIDEO_ROTATION_H_
