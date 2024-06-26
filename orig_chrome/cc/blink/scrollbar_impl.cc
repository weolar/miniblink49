// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/scrollbar_impl.h"

#include "base/logging.h"
#include "third_party/WebKit/public/platform/WebScrollbar.h"
#include "third_party/WebKit/public/platform/WebScrollbarThemeGeometry.h"

using blink::WebScrollbar;

namespace cc_blink {

ScrollbarImpl::ScrollbarImpl(
    scoped_ptr<WebScrollbar> scrollbar,
    blink::WebScrollbarThemePainter painter,
    scoped_ptr<blink::WebScrollbarThemeGeometry> geometry)
    : scrollbar_(scrollbar.Pass())
    , painter_(painter)
    , geometry_(geometry.Pass())
{
}

ScrollbarImpl::~ScrollbarImpl()
{
}

cc::ScrollbarOrientation ScrollbarImpl::Orientation() const
{
    if (scrollbar_->orientation() == WebScrollbar::Horizontal)
        return cc::HORIZONTAL;
    return cc::VERTICAL;
}

bool ScrollbarImpl::IsLeftSideVerticalScrollbar() const
{
    return scrollbar_->isLeftSideVerticalScrollbar();
}

bool ScrollbarImpl::HasThumb() const
{
    return geometry_->hasThumb(scrollbar_.get());
}

bool ScrollbarImpl::IsOverlay() const
{
    return scrollbar_->isOverlay();
}

gfx::Point ScrollbarImpl::Location() const
{
    return scrollbar_->location();
}

int ScrollbarImpl::ThumbThickness() const
{
    gfx::Rect thumb_rect = geometry_->thumbRect(scrollbar_.get());
    if (scrollbar_->orientation() == WebScrollbar::Horizontal)
        return thumb_rect.height();
    return thumb_rect.width();
}

int ScrollbarImpl::ThumbLength() const
{
    gfx::Rect thumb_rect = geometry_->thumbRect(scrollbar_.get());
    if (scrollbar_->orientation() == WebScrollbar::Horizontal)
        return thumb_rect.width();
    return thumb_rect.height();
}

gfx::Rect ScrollbarImpl::TrackRect() const
{
    return geometry_->trackRect(scrollbar_.get());
}

void ScrollbarImpl::PaintPart(SkCanvas* canvas,
    cc::ScrollbarPart part,
    const gfx::Rect& content_rect)
{
    if (part == cc::THUMB) {
        painter_.paintThumb(canvas, content_rect);
        return;
    }

    // The following is a simplification of ScrollbarThemeComposite::paint.
    painter_.paintScrollbarBackground(canvas, content_rect);

    if (geometry_->hasButtons(scrollbar_.get())) {
        gfx::Rect back_button_start_paint_rect = geometry_->backButtonStartRect(scrollbar_.get());
        painter_.paintBackButtonStart(canvas, back_button_start_paint_rect);

        gfx::Rect back_button_end_paint_rect = geometry_->backButtonEndRect(scrollbar_.get());
        painter_.paintBackButtonEnd(canvas, back_button_end_paint_rect);

        gfx::Rect forward_button_start_paint_rect = geometry_->forwardButtonStartRect(scrollbar_.get());
        painter_.paintForwardButtonStart(canvas, forward_button_start_paint_rect);

        gfx::Rect forward_button_end_paint_rect = geometry_->forwardButtonEndRect(scrollbar_.get());
        painter_.paintForwardButtonEnd(canvas, forward_button_end_paint_rect);
    }

    gfx::Rect track_paint_rect = geometry_->trackRect(scrollbar_.get());
    painter_.paintTrackBackground(canvas, track_paint_rect);

    bool thumb_present = geometry_->hasThumb(scrollbar_.get());
    if (thumb_present) {
        painter_.paintForwardTrackPart(canvas, track_paint_rect);
        painter_.paintBackTrackPart(canvas, track_paint_rect);
    }

    painter_.paintTickmarks(canvas, track_paint_rect);
}

} // namespace cc_blink
