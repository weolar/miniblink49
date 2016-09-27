/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/LayoutScrollbarTheme.h"

#include "core/layout/LayoutScrollbar.h"
#include "core/paint/ScrollbarPainter.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DrawingRecorder.h"
#include "platform/scroll/ScrollbarThemeClient.h"
#include "wtf/StdLibExtras.h"

namespace blink {

LayoutScrollbarTheme* LayoutScrollbarTheme::layoutScrollbarTheme()
{
    DEFINE_STATIC_LOCAL(LayoutScrollbarTheme, theme, ());
    return &theme;
}

void LayoutScrollbarTheme::buttonSizesAlongTrackAxis(ScrollbarThemeClient* scrollbar, int& beforeSize, int& afterSize)
{
    IntRect firstButton = backButtonRect(scrollbar, BackButtonStartPart);
    IntRect secondButton = forwardButtonRect(scrollbar, ForwardButtonStartPart);
    IntRect thirdButton = backButtonRect(scrollbar, BackButtonEndPart);
    IntRect fourthButton = forwardButtonRect(scrollbar, ForwardButtonEndPart);
    if (scrollbar->orientation() == HorizontalScrollbar) {
        beforeSize = firstButton.width() + secondButton.width();
        afterSize = thirdButton.width() + fourthButton.width();
    } else {
        beforeSize = firstButton.height() + secondButton.height();
        afterSize = thirdButton.height() + fourthButton.height();
    }
}

bool LayoutScrollbarTheme::hasButtons(ScrollbarThemeClient* scrollbar)
{
    int startSize;
    int endSize;
    buttonSizesAlongTrackAxis(scrollbar, startSize, endSize);
    return (startSize + endSize) <= (scrollbar->orientation() == HorizontalScrollbar ? scrollbar->width() : scrollbar->height());
}

bool LayoutScrollbarTheme::hasThumb(ScrollbarThemeClient* scrollbar)
{
    return trackLength(scrollbar) - thumbLength(scrollbar) >= 0;
}

int LayoutScrollbarTheme::minimumThumbLength(ScrollbarThemeClient* scrollbar)
{
    return toLayoutScrollbar(scrollbar)->minimumThumbLength();
}

IntRect LayoutScrollbarTheme::backButtonRect(ScrollbarThemeClient* scrollbar, ScrollbarPart partType, bool)
{
    return toLayoutScrollbar(scrollbar)->buttonRect(partType);
}

IntRect LayoutScrollbarTheme::forwardButtonRect(ScrollbarThemeClient* scrollbar, ScrollbarPart partType, bool)
{
    return toLayoutScrollbar(scrollbar)->buttonRect(partType);
}

IntRect LayoutScrollbarTheme::trackRect(ScrollbarThemeClient* scrollbar, bool)
{
    if (!hasButtons(scrollbar))
        return scrollbar->frameRect();

    int startLength;
    int endLength;
    buttonSizesAlongTrackAxis(scrollbar, startLength, endLength);

    return toLayoutScrollbar(scrollbar)->trackRect(startLength, endLength);
}

IntRect LayoutScrollbarTheme::constrainTrackRectToTrackPieces(ScrollbarThemeClient* scrollbar, const IntRect& rect)
{
    IntRect backRect = toLayoutScrollbar(scrollbar)->trackPieceRectWithMargins(BackTrackPart, rect);
    IntRect forwardRect = toLayoutScrollbar(scrollbar)->trackPieceRectWithMargins(ForwardTrackPart, rect);
    IntRect result = rect;
    if (scrollbar->orientation() == HorizontalScrollbar) {
        result.setX(backRect.x());
        result.setWidth(forwardRect.maxX() - backRect.x());
    } else {
        result.setY(backRect.y());
        result.setHeight(forwardRect.maxY() - backRect.y());
    }
    return result;
}

void LayoutScrollbarTheme::paintScrollCorner(GraphicsContext* context, const DisplayItemClientWrapper& displayItemClient, const IntRect& cornerRect)
{
    if (DrawingRecorder::useCachedDrawingIfPossible(*context, displayItemClient, DisplayItem::ScrollbarCorner))
        return;

    DrawingRecorder recorder(*context, displayItemClient, DisplayItem::ScrollbarCorner, cornerRect);
    // FIXME: Implement.
    context->fillRect(cornerRect, Color::white);
}

void LayoutScrollbarTheme::paintScrollbarBackground(GraphicsContext* context, ScrollbarThemeClient* scrollbar)
{
    ScrollbarPainter(*toLayoutScrollbar(scrollbar)).paintPart(context, ScrollbarBGPart, scrollbar->frameRect());
}

void LayoutScrollbarTheme::paintTrackBackground(GraphicsContext* context, ScrollbarThemeClient* scrollbar, const IntRect& rect)
{
    ScrollbarPainter(*toLayoutScrollbar(scrollbar)).paintPart(context, TrackBGPart, rect);
}

void LayoutScrollbarTheme::paintTrackPiece(GraphicsContext* context, ScrollbarThemeClient* scrollbar, const IntRect& rect, ScrollbarPart part)
{
    ScrollbarPainter(*toLayoutScrollbar(scrollbar)).paintPart(context, part, rect);
}

void LayoutScrollbarTheme::paintButton(GraphicsContext* context, ScrollbarThemeClient* scrollbar, const IntRect& rect, ScrollbarPart part)
{
    ScrollbarPainter(*toLayoutScrollbar(scrollbar)).paintPart(context, part, rect);
}

void LayoutScrollbarTheme::paintThumb(GraphicsContext* context, ScrollbarThemeClient* scrollbar, const IntRect& rect)
{
    ScrollbarPainter(*toLayoutScrollbar(scrollbar)).paintPart(context, ThumbPart, rect);
}

void LayoutScrollbarTheme::paintTickmarks(GraphicsContext* context, ScrollbarThemeClient* scrollbar, const IntRect& rect)
{
    ScrollbarTheme::theme()->paintTickmarks(context, scrollbar, rect);
}

}
