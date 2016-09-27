/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/scroll/ScrollbarThemeMacNonOverlayAPI.h"

#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/ImageBuffer.h"
#include "platform/graphics/paint/DrawingRecorder.h"
#include "platform/mac/ThemeMac.h"
#include "platform/scroll/ScrollbarThemeClient.h"
#include "public/platform/Platform.h"
#include "public/platform/WebRect.h"
#include "public/platform/WebThemeEngine.h"
#include "skia/ext/skia_utils_mac.h"
#include <Carbon/Carbon.h>

namespace blink {

// FIXME: Get these numbers from CoreUI.
static int cRealButtonLength[] = { 28, 21 };
static int cButtonHitInset[] = { 3, 2 };
// cRealButtonLength - cButtonInset
static int cButtonLength[] = { 14, 10 };
static int cScrollbarThickness[] = { 15, 11 };
static int cButtonInset[] = { 14, 11 };
static int cThumbMinLength[] = { 26, 20 };

static int cOuterButtonLength[] = { 16, 14 }; // The outer button in a double button pair is a bit bigger.
static int cOuterButtonOverlap = 2;

static ScrollbarButtonsPlacement gButtonPlacement = ScrollbarButtonsDoubleEnd;

void ScrollbarThemeMacNonOverlayAPI::updateButtonPlacement()
{
    NSString *buttonPlacement = [[NSUserDefaults standardUserDefaults] objectForKey:@"AppleScrollBarVariant"];
    if ([buttonPlacement isEqualToString:@"Single"])
        gButtonPlacement = ScrollbarButtonsSingle;
    else if ([buttonPlacement isEqualToString:@"DoubleMin"])
        gButtonPlacement = ScrollbarButtonsDoubleStart;
    else if ([buttonPlacement isEqualToString:@"DoubleBoth"])
        gButtonPlacement = ScrollbarButtonsDoubleBoth;
    else {
        gButtonPlacement = ScrollbarButtonsDoubleEnd;
    }
}

// Override ScrollbarThemeMacCommon::paint() to add support for the following:
//     - drawing using WebThemeEngine functions
//     - drawing tickmarks
//     - Skia specific changes
bool ScrollbarThemeMacNonOverlayAPI::paint(ScrollbarThemeClient* scrollbar, GraphicsContext* context, const IntRect& damageRect)
{
    DisplayItem::Type displayItemType = scrollbar->orientation() == HorizontalScrollbar ? DisplayItem::ScrollbarHorizontal : DisplayItem::ScrollbarVertical;
    if (DrawingRecorder::useCachedDrawingIfPossible(*context, *scrollbar, displayItemType))
        return true;

    DrawingRecorder recorder(*context, *scrollbar, displayItemType, scrollbar->frameRect());

    // Get the tickmarks for the frameview.
    Vector<IntRect> tickmarks;
    scrollbar->getTickmarks(tickmarks);

    HIThemeTrackDrawInfo trackInfo;
    trackInfo.version = 0;
    trackInfo.kind = scrollbar->controlSize() == RegularScrollbar ? kThemeMediumScrollBar : kThemeSmallScrollBar;
    trackInfo.bounds = scrollbar->frameRect();
    trackInfo.min = 0;
    trackInfo.max = scrollbar->maximum();
    trackInfo.value = scrollbar->currentPos();
    trackInfo.trackInfo.scrollbar.viewsize = scrollbar->visibleSize();
    trackInfo.attributes = hasThumb(scrollbar) ? kThemeTrackShowThumb : 0;

    if (scrollbar->orientation() == HorizontalScrollbar)
        trackInfo.attributes |= kThemeTrackHorizontal;

    if (!scrollbar->enabled())
        trackInfo.enableState = kThemeTrackDisabled;
    else
        trackInfo.enableState = scrollbar->isScrollableAreaActive() ? kThemeTrackActive : kThemeTrackInactive;

    if (!hasButtons(scrollbar))
        trackInfo.enableState = kThemeTrackNothingToScroll;
    trackInfo.trackInfo.scrollbar.pressState = scrollbarPartToHIPressedState(scrollbar->pressedPart());

    SkCanvas* canvas = context->canvas();
    CGAffineTransform currentCTM = gfx::SkMatrixToCGAffineTransform(canvas->getTotalMatrix());

    // The Aqua scrollbar is buggy when rotated and scaled.  We will just draw into a bitmap if we detect a scale or rotation.
    bool canDrawDirectly = currentCTM.a == 1.0f && currentCTM.b == 0.0f && currentCTM.c == 0.0f && (currentCTM.d == 1.0f || currentCTM.d == -1.0f);
    OwnPtr<ImageBuffer> imageBuffer;
    SkCanvas* drawingCanvas;
    if (!canDrawDirectly) {
        trackInfo.bounds = IntRect(IntPoint(), scrollbar->frameRect().size());

        IntRect bufferRect(scrollbar->frameRect());
        bufferRect.intersect(damageRect);
        bufferRect.move(-scrollbar->frameRect().x(), -scrollbar->frameRect().y());

        imageBuffer = ImageBuffer::create(bufferRect.size());
        if (!imageBuffer)
            return true;

        drawingCanvas = imageBuffer->canvas();
    } else {
        drawingCanvas = canvas;
    }

    // Draw the track and its thumb.
    gfx::SkiaBitLocker bitLocker(
        drawingCanvas,
        ThemeMac::inflateRectForAA(scrollbar->frameRect()),
        canDrawDirectly ? context->deviceScaleFactor() : 1.0f);
    CGContextRef cgContext = bitLocker.cgContext();
    HIThemeDrawTrack(&trackInfo, 0, cgContext, kHIThemeOrientationNormal);

    IntRect tickmarkTrackRect = trackRect(scrollbar, false);
    if (!canDrawDirectly) {
        tickmarkTrackRect.setX(0);
        tickmarkTrackRect.setY(0);
    }
    // The ends are rounded and the thumb doesn't go there.
    tickmarkTrackRect.inflateY(-tickmarkTrackRect.width());
    // Inset a bit.
    tickmarkTrackRect.setX(tickmarkTrackRect.x() + 2);
    tickmarkTrackRect.setWidth(tickmarkTrackRect.width() - 5);
    paintGivenTickmarks(drawingCanvas, scrollbar, tickmarkTrackRect, tickmarks);

    if (!canDrawDirectly) {
        ASSERT(imageBuffer);
        if (!context->contextDisabled()) {
            imageBuffer->draw(context, FloatRect(scrollbar->frameRect().location(), imageBuffer->size()),
                nullptr, SkXfermode::kSrcOver_Mode);
        }
    }

    return true;
}

int ScrollbarThemeMacNonOverlayAPI::scrollbarThickness(ScrollbarControlSize controlSize)
{
    return cScrollbarThickness[controlSize];
}

ScrollbarButtonsPlacement ScrollbarThemeMacNonOverlayAPI::buttonsPlacement() const
{
    return gButtonPlacement;
}

bool ScrollbarThemeMacNonOverlayAPI::hasButtons(ScrollbarThemeClient* scrollbar)
{
    return scrollbar->enabled() && buttonsPlacement() != ScrollbarButtonsNone
             && (scrollbar->orientation() == HorizontalScrollbar
             ? scrollbar->width()
             : scrollbar->height()) >= 2 * (cRealButtonLength[scrollbar->controlSize()] - cButtonHitInset[scrollbar->controlSize()]);
}

bool ScrollbarThemeMacNonOverlayAPI::hasThumb(ScrollbarThemeClient* scrollbar)
{
    int minLengthForThumb = 2 * cButtonInset[scrollbar->controlSize()] + cThumbMinLength[scrollbar->controlSize()] + 1;
    return scrollbar->enabled() && (scrollbar->orientation() == HorizontalScrollbar ?
             scrollbar->width() :
             scrollbar->height()) >= minLengthForThumb;
}

static IntRect buttonRepaintRect(const IntRect& buttonRect, ScrollbarOrientation orientation, ScrollbarControlSize controlSize, bool start)
{
    ASSERT(gButtonPlacement != ScrollbarButtonsNone);

    IntRect paintRect(buttonRect);
    if (orientation == HorizontalScrollbar) {
        paintRect.setWidth(cRealButtonLength[controlSize]);
        if (!start)
            paintRect.setX(buttonRect.x() - (cRealButtonLength[controlSize] - buttonRect.width()));
    } else {
        paintRect.setHeight(cRealButtonLength[controlSize]);
        if (!start)
            paintRect.setY(buttonRect.y() - (cRealButtonLength[controlSize] - buttonRect.height()));
    }

    return paintRect;
}

IntRect ScrollbarThemeMacNonOverlayAPI::backButtonRect(ScrollbarThemeClient* scrollbar, ScrollbarPart part, bool painting)
{
    IntRect result;

    if (part == BackButtonStartPart && (buttonsPlacement() == ScrollbarButtonsNone || buttonsPlacement() == ScrollbarButtonsDoubleEnd))
        return result;

    if (part == BackButtonEndPart && (buttonsPlacement() == ScrollbarButtonsNone || buttonsPlacement() == ScrollbarButtonsDoubleStart || buttonsPlacement() == ScrollbarButtonsSingle))
        return result;

    int thickness = scrollbarThickness(scrollbar->controlSize());
    bool outerButton = part == BackButtonStartPart && (buttonsPlacement() == ScrollbarButtonsDoubleStart || buttonsPlacement() == ScrollbarButtonsDoubleBoth);
    if (outerButton) {
        if (scrollbar->orientation() == HorizontalScrollbar)
            result = IntRect(scrollbar->x(), scrollbar->y(), cOuterButtonLength[scrollbar->controlSize()] + (painting ? cOuterButtonOverlap : 0), thickness);
        else
            result = IntRect(scrollbar->x(), scrollbar->y(), thickness, cOuterButtonLength[scrollbar->controlSize()] + (painting ? cOuterButtonOverlap : 0));
        return result;
    }

    // Our repaint rect is slightly larger, since we are a button that is adjacent to the track.
    if (scrollbar->orientation() == HorizontalScrollbar) {
        int start = part == BackButtonStartPart ? scrollbar->x() : scrollbar->x() + scrollbar->width() - cOuterButtonLength[scrollbar->controlSize()] - cButtonLength[scrollbar->controlSize()];
        result = IntRect(start, scrollbar->y(), cButtonLength[scrollbar->controlSize()], thickness);
    } else {
        int start = part == BackButtonStartPart ? scrollbar->y() : scrollbar->y() + scrollbar->height() - cOuterButtonLength[scrollbar->controlSize()] - cButtonLength[scrollbar->controlSize()];
        result = IntRect(scrollbar->x(), start, thickness, cButtonLength[scrollbar->controlSize()]);
    }

    if (painting)
        return buttonRepaintRect(result, scrollbar->orientation(), scrollbar->controlSize(), part == BackButtonStartPart);
    return result;
}

IntRect ScrollbarThemeMacNonOverlayAPI::forwardButtonRect(ScrollbarThemeClient* scrollbar, ScrollbarPart part, bool painting)
{
    IntRect result;

    if (part == ForwardButtonEndPart && (buttonsPlacement() == ScrollbarButtonsNone || buttonsPlacement() == ScrollbarButtonsDoubleStart))
        return result;

    if (part == ForwardButtonStartPart && (buttonsPlacement() == ScrollbarButtonsNone || buttonsPlacement() == ScrollbarButtonsDoubleEnd || buttonsPlacement() == ScrollbarButtonsSingle))
        return result;

    int thickness = scrollbarThickness(scrollbar->controlSize());
    int outerButtonLength = cOuterButtonLength[scrollbar->controlSize()];
    int buttonLength = cButtonLength[scrollbar->controlSize()];

    bool outerButton = part == ForwardButtonEndPart && (buttonsPlacement() == ScrollbarButtonsDoubleEnd || buttonsPlacement() == ScrollbarButtonsDoubleBoth);
    if (outerButton) {
        if (scrollbar->orientation() == HorizontalScrollbar) {
            result = IntRect(scrollbar->x() + scrollbar->width() - outerButtonLength, scrollbar->y(), outerButtonLength, thickness);
            if (painting)
                result.inflateX(cOuterButtonOverlap);
        } else {
            result = IntRect(scrollbar->x(), scrollbar->y() + scrollbar->height() - outerButtonLength, thickness, outerButtonLength);
            if (painting)
                result.inflateY(cOuterButtonOverlap);
        }
        return result;
    }

    if (scrollbar->orientation() == HorizontalScrollbar) {
        int start = part == ForwardButtonEndPart ? scrollbar->x() + scrollbar->width() - buttonLength : scrollbar->x() + outerButtonLength;
        result = IntRect(start, scrollbar->y(), buttonLength, thickness);
    } else {
        int start = part == ForwardButtonEndPart ? scrollbar->y() + scrollbar->height() - buttonLength : scrollbar->y() + outerButtonLength;
        result = IntRect(scrollbar->x(), start, thickness, buttonLength);
    }
    if (painting)
        return buttonRepaintRect(result, scrollbar->orientation(), scrollbar->controlSize(), part == ForwardButtonStartPart);
    return result;
}

IntRect ScrollbarThemeMacNonOverlayAPI::trackRect(ScrollbarThemeClient* scrollbar, bool painting)
{
    if (painting || !hasButtons(scrollbar))
        return scrollbar->frameRect();

    IntRect result;
    int thickness = scrollbarThickness(scrollbar->controlSize());
    int startWidth = 0;
    int endWidth = 0;
    int outerButtonLength = cOuterButtonLength[scrollbar->controlSize()];
    int buttonLength = cButtonLength[scrollbar->controlSize()];
    int doubleButtonLength = outerButtonLength + buttonLength;
    switch (buttonsPlacement()) {
        case ScrollbarButtonsSingle:
            startWidth = buttonLength;
            endWidth = buttonLength;
            break;
        case ScrollbarButtonsDoubleStart:
            startWidth = doubleButtonLength;
            break;
        case ScrollbarButtonsDoubleEnd:
            endWidth = doubleButtonLength;
            break;
        case ScrollbarButtonsDoubleBoth:
            startWidth = doubleButtonLength;
            endWidth = doubleButtonLength;
            break;
        default:
            break;
    }

    int totalWidth = startWidth + endWidth;
    if (scrollbar->orientation() == HorizontalScrollbar)
        return IntRect(scrollbar->x() + startWidth, scrollbar->y(), scrollbar->width() - totalWidth, thickness);
    return IntRect(scrollbar->x(), scrollbar->y() + startWidth, thickness, scrollbar->height() - totalWidth);
}

int ScrollbarThemeMacNonOverlayAPI::minimumThumbLength(ScrollbarThemeClient* scrollbar)
{
    return cThumbMinLength[scrollbar->controlSize()];
}

}
