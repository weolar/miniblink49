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

#ifndef ScrollbarTheme_h
#define ScrollbarTheme_h

#include "platform/PlatformExport.h"
#include "platform/geometry/IntRect.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "platform/scroll/ScrollTypes.h"

namespace blink {

class GraphicsContext;
class PlatformMouseEvent;
class ScrollbarThemeClient;

class PLATFORM_EXPORT ScrollbarTheme {
    WTF_MAKE_NONCOPYABLE(ScrollbarTheme); WTF_MAKE_FAST_ALLOCATED(ScrollbarTheme);
public:
    ScrollbarTheme() { }
    virtual ~ScrollbarTheme() { }

    virtual void updateEnabledState(ScrollbarThemeClient*) { }

    virtual bool paint(ScrollbarThemeClient*, GraphicsContext*, const IntRect& damageRect);

    virtual ScrollbarPart hitTest(ScrollbarThemeClient*, const IntPoint&);

    virtual int scrollbarThickness(ScrollbarControlSize = RegularScrollbar) { return 0; }
    virtual int scrollbarMargin() const { return 0; }

    virtual ScrollbarButtonsPlacement buttonsPlacement() const { return ScrollbarButtonsSingle; }

    virtual bool supportsControlTints() const { return false; }
    virtual bool usesOverlayScrollbars() const { return false; }
    virtual void updateScrollbarOverlayStyle(ScrollbarThemeClient*) { }

    virtual bool invalidateOnMouseEnterExit() { return false; }

    void invalidateParts(ScrollbarThemeClient* scrollbar, ScrollbarControlPartMask mask)
    {
        if (mask & BackButtonStartPart)
            invalidatePart(scrollbar, BackButtonStartPart);
        if (mask & ForwardButtonStartPart)
            invalidatePart(scrollbar, ForwardButtonStartPart);
        if (mask & BackTrackPart)
            invalidatePart(scrollbar, BackTrackPart);
        if (mask & ThumbPart)
            invalidatePart(scrollbar, ThumbPart);
        if (mask & ForwardTrackPart)
            invalidatePart(scrollbar, ForwardTrackPart);
        if (mask & BackButtonEndPart)
            invalidatePart(scrollbar, BackButtonEndPart);
        if (mask & ForwardButtonEndPart)
            invalidatePart(scrollbar, ForwardButtonEndPart);
    }

    virtual void invalidatePart(ScrollbarThemeClient*, ScrollbarPart);

    virtual void paintScrollCorner(GraphicsContext*, const DisplayItemClientWrapper&, const IntRect& cornerRect);
    virtual void paintTickmarks(GraphicsContext*, ScrollbarThemeClient*, const IntRect&) { }

    virtual bool shouldCenterOnThumb(ScrollbarThemeClient*, const PlatformMouseEvent&);
    virtual bool shouldSnapBackToDragOrigin(ScrollbarThemeClient*, const PlatformMouseEvent&);
    virtual bool shouldDragDocumentInsteadOfThumb(ScrollbarThemeClient*, const PlatformMouseEvent&) { return false; }

    // The position of the thumb relative to the track.
    virtual int thumbPosition(ScrollbarThemeClient*);
    // The length of the thumb along the axis of the scrollbar.
    virtual int thumbLength(ScrollbarThemeClient*);
    // The position of the track relative to the scrollbar.
    virtual int trackPosition(ScrollbarThemeClient*);
    // The length of the track along the axis of the scrollbar.
    virtual int trackLength(ScrollbarThemeClient*);

    virtual bool hasButtons(ScrollbarThemeClient*) = 0;
    virtual bool hasThumb(ScrollbarThemeClient*) = 0;

    virtual IntRect backButtonRect(ScrollbarThemeClient*, ScrollbarPart, bool painting = false) = 0;
    virtual IntRect forwardButtonRect(ScrollbarThemeClient*, ScrollbarPart, bool painting = false) = 0;
    virtual IntRect trackRect(ScrollbarThemeClient*, bool painting = false) = 0;
    virtual IntRect thumbRect(ScrollbarThemeClient*);
    virtual int thumbThickness(ScrollbarThemeClient*);

    virtual int minimumThumbLength(ScrollbarThemeClient*);

    virtual void splitTrack(ScrollbarThemeClient*, const IntRect& track, IntRect& startTrack, IntRect& thumb, IntRect& endTrack);

    virtual void paintScrollbarBackground(GraphicsContext*, ScrollbarThemeClient*) { }
    virtual void paintTrackBackground(GraphicsContext*, ScrollbarThemeClient*, const IntRect&) { }
    virtual void paintTrackPiece(GraphicsContext*, ScrollbarThemeClient*, const IntRect&, ScrollbarPart) { }
    virtual void paintButton(GraphicsContext*, ScrollbarThemeClient*, const IntRect&, ScrollbarPart) { }
    virtual void paintThumb(GraphicsContext*, ScrollbarThemeClient*, const IntRect&) { }

    virtual int maxOverlapBetweenPages() { return std::numeric_limits<int>::max(); }

    virtual double initialAutoscrollTimerDelay() { return 0.25; }
    virtual double autoscrollTimerDelay() { return 0.05; }

    virtual IntRect constrainTrackRectToTrackPieces(ScrollbarThemeClient*, const IntRect& rect) { return rect; }

    virtual void registerScrollbar(ScrollbarThemeClient*) { }
    virtual void unregisterScrollbar(ScrollbarThemeClient*) { }

    virtual bool isMockTheme() const { return false; }

    static ScrollbarTheme* theme();

    static void setMockScrollbarsEnabled(bool flag);
    static bool mockScrollbarsEnabled();

protected:
    static DisplayItem::Type buttonPartToDisplayItemType(ScrollbarPart);
    static DisplayItem::Type trackPiecePartToDisplayItemType(ScrollbarPart);

private:
    static ScrollbarTheme* nativeTheme(); // Must be implemented to return the correct theme subclass.
    static bool gMockScrollbarsEnabled;
};

}
#endif
