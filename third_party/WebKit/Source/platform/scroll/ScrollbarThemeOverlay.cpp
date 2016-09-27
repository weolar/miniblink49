/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/scroll/ScrollbarThemeOverlay.h"

#include "platform/PlatformMouseEvent.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DrawingRecorder.h"
#include "platform/scroll/ScrollbarThemeClient.h"
#include "platform/transforms/TransformationMatrix.h"
#include "public/platform/Platform.h"
#include "public/platform/WebRect.h"
#include "public/platform/WebThemeEngine.h"

#include <algorithm>

namespace blink {

ScrollbarThemeOverlay::ScrollbarThemeOverlay(int thumbThickness, int scrollbarMargin, HitTestBehavior allowHitTest, Color color)
    : ScrollbarTheme()
    , m_thumbThickness(thumbThickness)
    , m_scrollbarMargin(scrollbarMargin)
    , m_allowHitTest(allowHitTest)
    , m_color(color)
    , m_useSolidColor(true)
{
}

ScrollbarThemeOverlay::ScrollbarThemeOverlay(int thumbThickness, int scrollbarMargin, HitTestBehavior allowHitTest)
    : ScrollbarTheme()
    , m_thumbThickness(thumbThickness)
    , m_scrollbarMargin(scrollbarMargin)
    , m_allowHitTest(allowHitTest)
    , m_useSolidColor(false)
{
}

int ScrollbarThemeOverlay::scrollbarThickness(ScrollbarControlSize controlSize)
{
    return m_thumbThickness + m_scrollbarMargin;
}

int ScrollbarThemeOverlay::scrollbarMargin() const
{
    return m_scrollbarMargin;
}

bool ScrollbarThemeOverlay::usesOverlayScrollbars() const
{
    return true;
}

int ScrollbarThemeOverlay::thumbPosition(ScrollbarThemeClient* scrollbar)
{
    if (!scrollbar->totalSize())
        return 0;

    int trackLen = trackLength(scrollbar);
    float proportion = static_cast<float>(scrollbar->currentPos()) / scrollbar->totalSize();
    return round(proportion * trackLen);
}

int ScrollbarThemeOverlay::thumbLength(ScrollbarThemeClient* scrollbar)
{
    int trackLen = trackLength(scrollbar);

    if (!scrollbar->totalSize())
        return trackLen;

    float proportion = static_cast<float>(scrollbar->visibleSize()) / scrollbar->totalSize();
    int length = round(proportion * trackLen);
    length = std::min(std::max(length, minimumThumbLength(scrollbar)), trackLen);
    return length;
}

bool ScrollbarThemeOverlay::hasThumb(ScrollbarThemeClient* scrollbar)
{
    return true;
}

IntRect ScrollbarThemeOverlay::backButtonRect(ScrollbarThemeClient*, ScrollbarPart, bool)
{
    return IntRect();
}

IntRect ScrollbarThemeOverlay::forwardButtonRect(ScrollbarThemeClient*, ScrollbarPart, bool)
{
    return IntRect();
}

IntRect ScrollbarThemeOverlay::trackRect(ScrollbarThemeClient* scrollbar, bool)
{
    IntRect rect = scrollbar->frameRect();
    if (scrollbar->orientation() == HorizontalScrollbar)
        rect.inflateX(-m_scrollbarMargin);
    else
        rect.inflateY(-m_scrollbarMargin);
    return rect;
}

int ScrollbarThemeOverlay::thumbThickness(ScrollbarThemeClient*)
{
    return m_thumbThickness;
}

void ScrollbarThemeOverlay::paintThumb(GraphicsContext* context, ScrollbarThemeClient* scrollbar, const IntRect& rect)
{
    if (DrawingRecorder::useCachedDrawingIfPossible(*context, *scrollbar, DisplayItem::ScrollbarThumb))
        return;

    DrawingRecorder recorder(*context, *scrollbar, DisplayItem::ScrollbarThumb, rect);

    IntRect thumbRect = rect;
    if (scrollbar->orientation() == HorizontalScrollbar) {
        thumbRect.setHeight(thumbRect.height() - m_scrollbarMargin);
    } else {
        thumbRect.setWidth(thumbRect.width() - m_scrollbarMargin);
        if (scrollbar->isLeftSideVerticalScrollbar())
            thumbRect.setX(thumbRect.x() + m_scrollbarMargin);
    }

    if (m_useSolidColor) {
        context->fillRect(thumbRect, m_color);
        return;
    }

    WebThemeEngine::State state = WebThemeEngine::StateNormal;
    if (scrollbar->pressedPart() == ThumbPart)
        state = WebThemeEngine::StatePressed;
    else if (scrollbar->hoveredPart() == ThumbPart)
        state = WebThemeEngine::StateHover;

    WebCanvas* canvas = context->canvas();

    WebThemeEngine::Part part = WebThemeEngine::PartScrollbarHorizontalThumb;
    if (scrollbar->orientation() == VerticalScrollbar)
        part = WebThemeEngine::PartScrollbarVerticalThumb;

    Platform::current()->themeEngine()->paint(canvas, part, state, WebRect(rect), 0);
}

ScrollbarPart ScrollbarThemeOverlay::hitTest(ScrollbarThemeClient* scrollbar, const IntPoint& position)
{
    if (m_allowHitTest == DisallowHitTest)
        return NoPart;

    return ScrollbarTheme::hitTest(scrollbar, position);
}

} // namespace blink
