// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_SCROLLBAR_H_
#define CC_TEST_FAKE_SCROLLBAR_H_

#include "base/compiler_specific.h"
#include "cc/input/scrollbar.h"
#include "third_party/skia/include/core/SkColor.h"

namespace cc {

class FakeScrollbar : public Scrollbar {
public:
    FakeScrollbar();
    FakeScrollbar(bool paint, bool has_thumb, bool is_overlay);
    ~FakeScrollbar() override;

    // Scrollbar implementation.
    ScrollbarOrientation Orientation() const override;
    bool IsLeftSideVerticalScrollbar() const override;
    gfx::Point Location() const override;
    bool IsOverlay() const override;
    bool HasThumb() const override;
    int ThumbThickness() const override;
    int ThumbLength() const override;
    gfx::Rect TrackRect() const override;
    void PaintPart(SkCanvas* canvas,
        ScrollbarPart part,
        const gfx::Rect& content_rect) override;

    void set_location(const gfx::Point& location) { location_ = location; }
    void set_track_rect(const gfx::Rect& track_rect) { track_rect_ = track_rect; }
    void set_thumb_thickness(int thumb_thickness)
    {
        thumb_thickness_ = thumb_thickness;
    }
    void set_thumb_length(int thumb_length) { thumb_length_ = thumb_length; }
    void set_has_thumb(bool has_thumb) { has_thumb_ = has_thumb; }
    SkColor paint_fill_color() const { return SK_ColorBLACK | fill_color_; }

private:
    bool paint_;
    bool has_thumb_;
    bool is_overlay_;
    int thumb_thickness_;
    int thumb_length_;
    gfx::Point location_;
    gfx::Rect track_rect_;
    SkColor fill_color_;

    DISALLOW_COPY_AND_ASSIGN(FakeScrollbar);
};

} // namespace cc

#endif // CC_TEST_FAKE_SCROLLBAR_H_
