// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/sender/performance_metrics_overlay.h"

#include <algorithm>
#include <string>

#include "base/logging.h"
#include "base/numerics/safe_conversions.h"
#include "base/strings/stringprintf.h"
#include "media/base/video_frame.h"

namespace media {
namespace cast {

    namespace {

        const int kScale = 4; // Physical pixels per one logical pixel.
        const int kCharacterWidth = 3; // Logical pixel width of one character.
        const int kCharacterHeight = 5; // Logical pixel height of one character.
        const int kCharacterSpacing = 1; // Logical pixels between each character.
        const int kLineSpacing = 2; // Logical pixels between each line of characters.
        const int kPlane = 0; // Y-plane in YUV formats.

        // For each pixel in the |rect| (logical coordinates), either decrease the
        // intensity or increase it so that the resulting pixel has a perceivably
        // different value than it did before.  |p_ul| is a pointer to the pixel at
        // coordinate (0,0) in a single-channel 8bpp bitmap.  |stride| is the number of
        // bytes per row in the output bitmap.
        void DivergePixels(const gfx::Rect& rect, uint8* p_ul, int stride)
        {
            DCHECK(p_ul);
            DCHECK_GT(stride, 0);

            // These constants and heuristics were chosen based on experimenting with a
            // wide variety of content, and converging on a readable result.  The amount
            // by which the darker pixels are changed is less because each unit of change
            // has a larger visual impact on the darker end of the spectrum.  Each pixel's
            // intensity value is changed as follows:
            //
            //    [16,31] --> [32,63]   (always a difference of +16)
            //    [32,64] --> 16        (a difference between -16 and -48)
            //   [65,235] --> [17,187]  (always a difference of -48)
            const int kDivergeDownThreshold = 32;
            const int kDivergeDownAmount = 48;
            const int kDivergeUpAmount = 32;
            const int kMinIntensity = 16;

            const int top = rect.y() * kScale;
            const int bottom = rect.bottom() * kScale;
            const int left = rect.x() * kScale;
            const int right = rect.right() * kScale;
            for (int y = top; y < bottom; ++y) {
                uint8* const p_l = p_ul + y * stride;
                for (int x = left; x < right; ++x) {
                    int intensity = p_l[x];
                    if (intensity >= kDivergeDownThreshold)
                        intensity = std::max(kMinIntensity, intensity - kDivergeDownAmount);
                    else
                        intensity += kDivergeUpAmount;
                    p_l[x] = static_cast<uint8>(intensity);
                }
            }
        }

        // Render |line| into |frame| at physical pixel row |top| and aligned to the
        // right edge.  Only number digits and a smattering of punctuation characters
        // will be rendered.
        void RenderLineOfText(const std::string& line, int top, VideoFrame* frame)
        {
            // Compute number of physical pixels wide the rendered |line| would be,
            // including padding.
            const int line_width = (((kCharacterWidth + kCharacterSpacing) * static_cast<int>(line.size())) + kCharacterSpacing) * kScale;

            // Determine if any characters would render past the left edge of the frame,
            // and compute the index of the first character to be rendered.
            const int pixels_per_char = (kCharacterWidth + kCharacterSpacing) * kScale;
            const size_t first_idx = (line_width < frame->visible_rect().width()) ? 0u : static_cast<size_t>(((line_width - frame->visible_rect().width()) / pixels_per_char) + 1);

            // Compute the pointer to the pixel at the upper-left corner of the first
            // character to be rendered.
            const int stride = frame->stride(kPlane);
            uint8* p_ul =
                // Start at the first pixel in the first row...
                frame->visible_data(kPlane) + (stride * top)
                // ...now move to the right edge of the visible part of the frame...
                + frame->visible_rect().width()
                // ...now move left to where line[0] would be rendered...
                - line_width
                // ...now move right to where line[first_idx] would be rendered.
                + first_idx * pixels_per_char;

            // Render each character.
            for (size_t i = first_idx; i < line.size(); ++i, p_ul += pixels_per_char) {
                switch (line[i]) {
                case '0':
                    DivergePixels(gfx::Rect(0, 0, 3, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 1, 1, 3), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 1, 1, 3), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 4, 3, 1), p_ul, stride);
                    break;
                case '1':
                    DivergePixels(gfx::Rect(1, 0, 1, 5), p_ul, stride);
                    break;
                case '2':
                    DivergePixels(gfx::Rect(0, 0, 3, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 1, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 2, 3, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 3, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 4, 3, 1), p_ul, stride);
                    break;
                case '3':
                    DivergePixels(gfx::Rect(0, 0, 3, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 1, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 2, 3, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 3, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 4, 3, 1), p_ul, stride);
                    break;
                case '4':
                    DivergePixels(gfx::Rect(0, 0, 1, 2), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 0, 1, 5), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 2, 2, 1), p_ul, stride);
                    break;
                case '5':
                    DivergePixels(gfx::Rect(0, 0, 3, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 1, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 2, 3, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 3, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 4, 3, 1), p_ul, stride);
                    break;
                case '6':
                    DivergePixels(gfx::Rect(1, 0, 2, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 1, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 2, 3, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 3, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 3, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 4, 3, 1), p_ul, stride);
                    break;
                case '7':
                    DivergePixels(gfx::Rect(0, 0, 3, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 1, 1, 2), p_ul, stride);
                    DivergePixels(gfx::Rect(1, 3, 1, 2), p_ul, stride);
                    break;
                case '8':
                    DivergePixels(gfx::Rect(0, 0, 3, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 1, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 1, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 2, 3, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 3, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 3, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 4, 3, 1), p_ul, stride);
                    break;
                case '9':
                    DivergePixels(gfx::Rect(0, 0, 3, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 1, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 1, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 2, 3, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 3, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 4, 2, 1), p_ul, stride);
                    break;
                case 'e':
                case 'E':
                    DivergePixels(gfx::Rect(0, 0, 3, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 1, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 2, 2, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 3, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 4, 3, 1), p_ul, stride);
                    break;
                case '.':
                    DivergePixels(gfx::Rect(1, 4, 1, 1), p_ul, stride);
                    break;
                case '+':
                    DivergePixels(gfx::Rect(1, 1, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(1, 3, 1, 1), p_ul, stride);
                    // ...fall through...
                case '-':
                    DivergePixels(gfx::Rect(0, 2, 3, 1), p_ul, stride);
                    break;
                case 'x':
                    DivergePixels(gfx::Rect(0, 1, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 1, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(1, 2, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 3, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 3, 1, 1), p_ul, stride);
                    break;
                case ':':
                    DivergePixels(gfx::Rect(1, 1, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(1, 3, 1, 1), p_ul, stride);
                    break;
                case '%':
                    DivergePixels(gfx::Rect(0, 0, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 1, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(1, 2, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(0, 3, 1, 1), p_ul, stride);
                    DivergePixels(gfx::Rect(2, 4, 1, 1), p_ul, stride);
                    break;
                case ' ':
                default:
                    break;
                }
            }
        }

    } // namespace

    void MaybeRenderPerformanceMetricsOverlay(int target_bitrate,
        int frames_ago,
        double deadline_utilization,
        double lossy_utilization,
        VideoFrame* frame)
    {
        if (VideoFrame::PlaneHorizontalBitsPerPixel(frame->format(), kPlane) != 8) {
            DLOG(WARNING) << "Cannot render overlay: Plane " << kPlane << " not 8bpp.";
            return;
        }

        // Can't render to unmappable memory (DmaBuf, CVPixelBuffer).
        if (!frame->IsMappable()) {
            DVLOG(2) << "Cannot render overlay: frame uses unmappable memory.";
            return;
        }

        // Compute the physical pixel top row for the bottom-most line of text.
        const int line_height = (kCharacterHeight + kLineSpacing) * kScale;
        int top = frame->visible_rect().height() - line_height;
        if (top < 0 || !VLOG_IS_ON(1))
            return;

        // Line 3: Frame resolution and timestamp.
        base::TimeDelta rem = frame->timestamp();
        const int minutes = rem.InMinutes();
        rem -= base::TimeDelta::FromMinutes(minutes);
        const int seconds = static_cast<int>(rem.InSeconds());
        rem -= base::TimeDelta::FromSeconds(seconds);
        const int hundredth_seconds = static_cast<int>(rem.InMilliseconds() / 10);
        RenderLineOfText(base::StringPrintf("%dx%d %d:%02d.%02d",
                             frame->visible_rect().width(),
                             frame->visible_rect().height(),
                             minutes,
                             seconds,
                             hundredth_seconds),
            top,
            frame);

        // Move up one line's worth of pixels.
        top -= line_height;
        if (top < 0 || !VLOG_IS_ON(2))
            return;

        // Line 2: Capture/frame duration and target bitrate.
        int capture_duration_ms = 0;
        base::TimeTicks capture_begin_time, capture_end_time;
        if (frame->metadata()->GetTimeTicks(VideoFrameMetadata::CAPTURE_BEGIN_TIME,
                &capture_begin_time)
            && frame->metadata()->GetTimeTicks(VideoFrameMetadata::CAPTURE_END_TIME,
                &capture_end_time)) {
            capture_duration_ms = base::saturated_cast<int>(
                (capture_end_time - capture_begin_time).InMillisecondsF() + 0.5);
        }
        int frame_duration_ms = 0;
        int frame_duration_ms_frac = 0;
        base::TimeDelta frame_duration;
        if (frame->metadata()->GetTimeDelta(VideoFrameMetadata::FRAME_DURATION,
                &frame_duration)) {
            const int decimilliseconds = base::saturated_cast<int>(
                frame_duration.InMicroseconds() / 100.0 + 0.5);
            frame_duration_ms = decimilliseconds / 10;
            frame_duration_ms_frac = decimilliseconds % 10;
        }
        const int target_kbits = target_bitrate / 1000;
        RenderLineOfText(base::StringPrintf("%3.1d %3.1d.%01d %4.1d",
                             capture_duration_ms,
                             frame_duration_ms,
                             frame_duration_ms_frac,
                             target_kbits),
            top,
            frame);

        // Move up one line's worth of pixels.
        top -= line_height;
        if (top < 0 || !VLOG_IS_ON(3))
            return;

        // Line 1: Recent utilization metrics.
        const int deadline_pct = base::saturated_cast<int>(deadline_utilization * 100.0 + 0.5);
        const int lossy_pct = base::saturated_cast<int>(lossy_utilization * 100.0 + 0.5);
        RenderLineOfText(base::StringPrintf("%d %3.1d%% %3.1d%%",
                             frames_ago,
                             deadline_pct,
                             lossy_pct),
            top,
            frame);
    }

} // namespace cast
} // namespace media
