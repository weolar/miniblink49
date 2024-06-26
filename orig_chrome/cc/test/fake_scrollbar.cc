// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_scrollbar.h"

#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/skia_util.h"

namespace cc {

FakeScrollbar::FakeScrollbar()
    : paint_(false)
    , has_thumb_(false)
    , is_overlay_(false)
    , thumb_thickness_(10)
    , thumb_length_(5)
    , track_rect_(0, 0, 100, 10)
    , fill_color_(SK_ColorGREEN)
{
}

FakeScrollbar::FakeScrollbar(bool paint, bool has_thumb, bool is_overlay)
    : paint_(paint)
    , has_thumb_(has_thumb)
    , is_overlay_(is_overlay)
    , thumb_thickness_(10)
    , thumb_length_(5)
    , track_rect_(0, 0, 100, 10)
    , fill_color_(SK_ColorGREEN)
{
}

FakeScrollbar::~FakeScrollbar() { }

ScrollbarOrientation FakeScrollbar::Orientation() const
{
    return HORIZONTAL;
}

bool FakeScrollbar::IsLeftSideVerticalScrollbar() const
{
    return false;
}

gfx::Point FakeScrollbar::Location() const { return location_; }

bool FakeScrollbar::IsOverlay() const { return is_overlay_; }

bool FakeScrollbar::HasThumb() const { return has_thumb_; }

int FakeScrollbar::ThumbThickness() const
{
    return thumb_thickness_;
}

int FakeScrollbar::ThumbLength() const
{
    return thumb_length_;
}

gfx::Rect FakeScrollbar::TrackRect() const
{
    return track_rect_;
}

void FakeScrollbar::PaintPart(SkCanvas* canvas,
    ScrollbarPart part,
    const gfx::Rect& content_rect)
{
    if (!paint_)
        return;

    // Fill the scrollbar with a different color each time.
    fill_color_++;
    SkPaint paint;
    paint.setAntiAlias(false);
    paint.setColor(paint_fill_color());
    paint.setStyle(SkPaint::kFill_Style);

    // Emulate the how the real scrollbar works by using scrollbar's rect for
    // TRACK and the given content_rect for the THUMB
    SkRect rect = part == TRACK ? RectToSkRect(TrackRect())
                                : RectToSkRect(content_rect);
    canvas->drawRect(rect, paint);
}

} // namespace cc
