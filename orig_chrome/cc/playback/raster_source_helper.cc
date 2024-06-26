// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/raster_source_helper.h"

#include "base/trace_event/trace_event.h"
#include "cc/debug/debug_colors.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/skia_util.h"

namespace cc {

void RasterSourceHelper::PrepareForPlaybackToCanvas(
    SkCanvas* canvas,
    const gfx::Rect& canvas_bitmap_rect,
    const gfx::Rect& canvas_playback_rect,
    const gfx::Rect& source_rect,
    float contents_scale,
    SkColor background_color,
    bool clear_canvas_with_debug_color,
    bool requires_clear)
{
    bool partial_update = canvas_bitmap_rect != canvas_playback_rect;

    if (!partial_update)
        canvas->discard();
    if (clear_canvas_with_debug_color) {
        // Any non-painted areas in the content bounds will be left in this color.
        if (!partial_update) {
            canvas->clear(DebugColors::NonPaintedFillColor());
        } else {
            canvas->save();
            canvas->clipRect(gfx::RectToSkRect(
                canvas_playback_rect - canvas_bitmap_rect.OffsetFromOrigin()));
            canvas->drawColor(DebugColors::NonPaintedFillColor());
            canvas->restore();
        }
    }

    // If this raster source has opaque contents, it is guaranteeing that it will
    // draw an opaque rect the size of the layer.  If it is not, then we must
    // clear this canvas ourselves.
    if (requires_clear) {
        TRACE_EVENT_INSTANT0("cc", "SkCanvas::clear", TRACE_EVENT_SCOPE_THREAD);
        // Clearing is about ~4x faster than drawing a rect even if the content
        // isn't covering a majority of the canvas.
        if (!partial_update) {
            canvas->clear(SK_ColorTRANSPARENT);
        } else {
            canvas->save();
            canvas->clipRect(gfx::RectToSkRect(
                canvas_playback_rect - canvas_bitmap_rect.OffsetFromOrigin()));
            canvas->drawColor(SK_ColorTRANSPARENT, SkXfermode::kClear_Mode);
            canvas->restore();
        }
    } else {
        // Even if completely covered, for rasterizations that touch the edge of the
        // layer, we also need to raster the background color underneath the last
        // texel (since the recording won't cover it) and outside the last texel
        // (due to linear filtering when using this texture).
        gfx::Rect content_rect = gfx::ScaleToEnclosingRect(source_rect, contents_scale);

        // The final texel of content may only be partially covered by a
        // rasterization; this rect represents the content rect that is fully
        // covered by content.
        gfx::Rect deflated_content_rect = content_rect;
        deflated_content_rect.Inset(0, 0, 1, 1);
        deflated_content_rect.Intersect(canvas_playback_rect);
        if (!deflated_content_rect.Contains(canvas_playback_rect)) {
            if (clear_canvas_with_debug_color) {
                // Any non-painted areas outside of the content bounds are left in
                // this color.  If this is seen then it means that cc neglected to
                // rerasterize a tile that used to intersect with the content rect
                // after the content bounds grew.
                canvas->save();
                canvas->translate(-canvas_bitmap_rect.x(), -canvas_bitmap_rect.y());
                canvas->clipRect(gfx::RectToSkRect(content_rect),
                    SkRegion::kDifference_Op);
                canvas->drawColor(DebugColors::MissingResizeInvalidations(),
                    SkXfermode::kSrc_Mode);
                canvas->restore();
            }

            // Drawing at most 2 x 2 x (canvas width + canvas height) texels is 2-3X
            // faster than clearing, so special case this.
            canvas->save();
            canvas->translate(-canvas_bitmap_rect.x(), -canvas_bitmap_rect.y());
            gfx::Rect inflated_content_rect = content_rect;
            // Only clear edges that will be inside the canvas_playback_rect, else we
            // clear things that are still valid from a previous raster.
            inflated_content_rect.Inset(0, 0, -1, -1);
            inflated_content_rect.Intersect(canvas_playback_rect);
            canvas->clipRect(gfx::RectToSkRect(inflated_content_rect),
                SkRegion::kReplace_Op);
            canvas->clipRect(gfx::RectToSkRect(deflated_content_rect),
                SkRegion::kDifference_Op);
            canvas->drawColor(background_color, SkXfermode::kSrc_Mode);
            canvas->restore();
        }
    }
}

} // namespace cc
