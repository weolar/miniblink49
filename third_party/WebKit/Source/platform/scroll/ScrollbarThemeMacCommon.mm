/*
 * Copyright (C) 2008, 2011 Apple Inc. All Rights Reserved.
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
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/scroll/ScrollbarThemeMacCommon.h"

#include <Carbon/Carbon.h>
#include "platform/PlatformMouseEvent.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DrawingRecorder.h"
#include "platform/mac/ColorMac.h"
#include "platform/mac/LocalCurrentGraphicsContext.h"
#include "platform/mac/NSScrollerImpDetails.h"
#include "platform/mac/ScrollAnimatorMac.h"
#include "platform/scroll/ScrollbarThemeClient.h"
#include "platform/scroll/ScrollbarThemeMacNonOverlayAPI.h"
#include "platform/scroll/ScrollbarThemeMacOverlayAPI.h"
#include "public/platform/WebThemeEngine.h"
#include "public/platform/Platform.h"
#include "public/platform/WebRect.h"
#include "skia/ext/skia_utils_mac.h"
#include "wtf/HashSet.h"
#include "wtf/StdLibExtras.h"
#include "wtf/TemporaryChange.h"

// FIXME: There are repainting problems due to Aqua scroll bar buttons' visual overflow.

using namespace blink;

@interface NSColor (WebNSColorDetails)
+ (NSImage *)_linenPatternImage;
@end

namespace blink {

typedef HashSet<ScrollbarThemeClient*> ScrollbarSet;

static ScrollbarSet& scrollbarSet()
{
    DEFINE_STATIC_LOCAL(ScrollbarSet, set, ());
    return set;
}

static float gInitialButtonDelay = 0.5f;
static float gAutoscrollButtonDelay = 0.05f;
static NSScrollerStyle gPreferredScrollerStyle = NSScrollerStyleLegacy;

ScrollbarTheme* ScrollbarTheme::nativeTheme()
{
    static ScrollbarThemeMacCommon* theme = NULL;
    if (theme)
        return theme;
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable()) {
        DEFINE_STATIC_LOCAL(ScrollbarThemeMacOverlayAPI, overlayTheme, ());
        theme = &overlayTheme;
    } else {
        DEFINE_STATIC_LOCAL(ScrollbarThemeMacNonOverlayAPI, nonOverlayTheme, ());
        theme = &nonOverlayTheme;
    }
    return theme;
}

void ScrollbarThemeMacCommon::registerScrollbar(ScrollbarThemeClient* scrollbar)
{
    scrollbarSet().add(scrollbar);
}

void ScrollbarThemeMacCommon::unregisterScrollbar(ScrollbarThemeClient* scrollbar)
{
    scrollbarSet().remove(scrollbar);
}

void ScrollbarThemeMacCommon::paintGivenTickmarks(SkCanvas* canvas, ScrollbarThemeClient* scrollbar, const IntRect& rect, const Vector<IntRect>& tickmarks)
{
    if (scrollbar->orientation() != VerticalScrollbar)
        return;

    if (rect.height() <= 0 || rect.width() <= 0)
        return;  // nothing to draw on.

    if (!tickmarks.size())
        return;

    SkAutoCanvasRestore stateSaver(canvas, true);

    SkPaint strokePaint;
    strokePaint.setAntiAlias(false);
    strokePaint.setColor(SkColorSetRGB(0xCC, 0xAA, 0x00));
    strokePaint.setStyle(SkPaint::kStroke_Style);

    SkPaint fillPaint;
    fillPaint.setAntiAlias(false);
    fillPaint.setColor(SkColorSetRGB(0xFF, 0xDD, 0x00));
    fillPaint.setStyle(SkPaint::kFill_Style);

    for (Vector<IntRect>::const_iterator i = tickmarks.begin(); i != tickmarks.end(); ++i) {
        // Calculate how far down (in %) the tick-mark should appear.
        const float percent = static_cast<float>(i->y()) / scrollbar->totalSize();
        if (percent < 0.0 || percent > 1.0)
            continue;

        // Calculate how far down (in pixels) the tick-mark should appear.
        const int yPos = rect.y() + (rect.height() * percent);

        // Paint.
        FloatRect tickRect(rect.x(), yPos, rect.width(), 2);
        canvas->drawRect(tickRect, fillPaint);
        canvas->drawRect(tickRect, strokePaint);
    }
}

void ScrollbarThemeMacCommon::paintTickmarks(GraphicsContext* context, ScrollbarThemeClient* scrollbar, const IntRect& rect)
{
    // Note: This is only used for css-styled scrollbars on mac.
    if (scrollbar->orientation() != VerticalScrollbar)
        return;

    if (rect.height() <= 0 || rect.width() <= 0)
        return;

    Vector<IntRect> tickmarks;
    scrollbar->getTickmarks(tickmarks);
    if (!tickmarks.size())
        return;

    if (DrawingRecorder::useCachedDrawingIfPossible(*context, *scrollbar, DisplayItem::ScrollbarTickmarks))
        return;

    DrawingRecorder recorder(*context, *scrollbar, DisplayItem::ScrollbarTickmarks, rect);

    // Inset a bit.
    IntRect tickmarkTrackRect = rect;
    tickmarkTrackRect.setX(tickmarkTrackRect.x() + 1);
    tickmarkTrackRect.setWidth(tickmarkTrackRect.width() - 2);
    paintGivenTickmarks(context->canvas(), scrollbar, tickmarkTrackRect, tickmarks);
}

ScrollbarThemeMacCommon::~ScrollbarThemeMacCommon()
{
}

void ScrollbarThemeMacCommon::preferencesChanged(float initialButtonDelay, float autoscrollButtonDelay, NSScrollerStyle preferredScrollerStyle, bool redraw)
{
    updateButtonPlacement();
    gInitialButtonDelay = initialButtonDelay;
    gAutoscrollButtonDelay = autoscrollButtonDelay;
    bool sendScrollerStyleNotification = gPreferredScrollerStyle != preferredScrollerStyle;
    gPreferredScrollerStyle = preferredScrollerStyle;
    if (redraw && !scrollbarSet().isEmpty()) {
        ScrollbarSet::iterator end = scrollbarSet().end();
        for (ScrollbarSet::iterator it = scrollbarSet().begin(); it != end; ++it) {
            (*it)->styleChanged();
            (*it)->invalidate();
        }
    }
    if (sendScrollerStyleNotification) {
        [[NSNotificationCenter defaultCenter]
            postNotificationName:@"NSPreferredScrollerStyleDidChangeNotification"
                          object:nil
                        userInfo:@{ @"NSScrollerStyle" : @(gPreferredScrollerStyle) }];
    }
}

double ScrollbarThemeMacCommon::initialAutoscrollTimerDelay()
{
    return gInitialButtonDelay;
}

double ScrollbarThemeMacCommon::autoscrollTimerDelay()
{
    return gAutoscrollButtonDelay;
}

bool ScrollbarThemeMacCommon::shouldDragDocumentInsteadOfThumb(ScrollbarThemeClient*, const PlatformMouseEvent& event)
{
    return event.altKey();
}

int ScrollbarThemeMacCommon::scrollbarPartToHIPressedState(ScrollbarPart part)
{
    switch (part) {
        case BackButtonStartPart:
            return kThemeTopOutsideArrowPressed;
        case BackButtonEndPart:
            return kThemeTopOutsideArrowPressed; // This does not make much sense.  For some reason the outside constant is required.
        case ForwardButtonStartPart:
            return kThemeTopInsideArrowPressed;
        case ForwardButtonEndPart:
            return kThemeBottomOutsideArrowPressed;
        case ThumbPart:
            return kThemeThumbPressed;
        default:
            return 0;
    }
}

// static
NSScrollerStyle ScrollbarThemeMacCommon::recommendedScrollerStyle()
{
    if (RuntimeEnabledFeatures::overlayScrollbarsEnabled())
        return NSScrollerStyleOverlay;
    return gPreferredScrollerStyle;
}

// static
bool ScrollbarThemeMacCommon::isOverlayAPIAvailable()
{
    static bool apiAvailable =
        [NSClassFromString(@"NSScrollerImp") respondsToSelector:@selector(scrollerImpWithStyle:controlSize:horizontal:replacingScrollerImp:)]
        && [NSClassFromString(@"NSScrollerImpPair") instancesRespondToSelector:@selector(scrollerStyle)];
    return apiAvailable;
}

} // namespace blink
