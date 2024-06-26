// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_SENDER_PERFORMANCE_METRICS_OVERLAY_H_
#define MEDIA_CAST_SENDER_PERFORMANCE_METRICS_OVERLAY_H_

// This module provides a display of frame-level performance metrics, rendered
// in the lower-right corner of a VideoFrame.  It looks like this:
//
// +----------------------------------------------------------------+
// |                         @@@@@@@@@@@@@@@@@@@@@@@                |
// |                 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                |
// |              @@@@@@@@@@@@@@@@@@@@@@@ @@@@@@@@@@@@              |
// |             @@@@@@@@@@@@@                    @@@@              |
// |            @@@@@@@@@@                        @@@@              |
// |           @@@@@  @@@               @@@       @@@@              |
// |           @@@     @    @@@         @@@@      @@@@              |
// |          @@@@          @@@@                  @@@@              |
// |          @@@@                  @@@           @@@               |
// |            @@@@                 @@           @@@               |
// |             @@@@@      @@@            @@@   @@@                |
// |              @@@@@     @@@@@        @@@@   @@@@                |
// |               @@@@@      @@@@@@@@@@@@@    @@@@                 |
// |                @@@@@@                    @@@@      1  45%  75% |
// |                    @@@@@@@@         @@@@@@      22  16.7  4000 |
// |                         @@@@@@@@@@@@@@@@      1280x720 0:15.12 |
// +----------------------------------------------------------------+
//
// Line 1: Reads as, "1 frame ago, the encoder deadline utilization for the
// frame was 45% and the lossy utilization was 75%."  Encoder deadline
// utilization is in terms the amount of real-world time it took to encode the
// frame, divided by the maximum amount of time allowed.  Lossy utilization is
// the amount of "complexity" in the frame's content versus the target encoded
// byte size, where a value over 100% means the frame's content is too complex
// to encode within the target number of bytes.
//
// Line 2: Reads as, "Capture of this frame took 22 ms.  The expected duration
// of this frame is 16.7 ms.  The target bitrate for this frame is 4000 kbps."
//
// Line 3: Contains the frame's resolution and media timestamp in
// minutes:seconds.hundredths format.

namespace media {

class VideoFrame;

namespace cast {

    // Renders an overlay of frame-level performance metrics in the lower-right
    // corner of the |frame|, as described above.  The verbose logging level for
    // video_frame_overlay.cc determines which lines, if any, are rendered: VLOG
    // level 1 renders the bottom line only, level 2 renders the bottom and middle
    // lines, and level 3 renders all three lines.  So, use the
    // --vmodule=performance_metrics_overlay=3 command line argument to turn on
    // rendering of the entire overlay.
    void MaybeRenderPerformanceMetricsOverlay(int target_bitrate,
        int frames_ago,
        double deadline_utilization,
        double lossy_utilization,
        VideoFrame* frame);

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_SENDER_PERFORMANCE_METRICS_OVERLAY_H_
