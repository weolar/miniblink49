// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InlineFlowBoxPainter_h
#define InlineFlowBoxPainter_h

#include "core/style/ShadowData.h"
#include "platform/graphics/GraphicsTypes.h"
#include "platform/text/TextDirection.h"

namespace blink {

class Color;
class FillLayer;
class InlineFlowBox;
class LayoutPoint;
class LayoutRect;
class LayoutSize;
class LayoutUnit;
struct PaintInfo;
class ComputedStyle;

class InlineFlowBoxPainter {
public:
    InlineFlowBoxPainter(InlineFlowBox& inlineFlowBox) : m_inlineFlowBox(inlineFlowBox) { }
    void paint(const PaintInfo&, const LayoutPoint& paintOffset, const LayoutUnit lineTop, const LayoutUnit lineBottom);

private:
    void paintBoxDecorationBackground(const PaintInfo&, const LayoutPoint& paintOffset, const LayoutRect& cullRect);
    void paintMask(const PaintInfo&, const LayoutPoint& paintOffset);
    void paintFillLayers(const PaintInfo&, const Color&, const FillLayer&, const LayoutRect&, SkXfermode::Mode op = SkXfermode::kSrcOver_Mode);
    void paintFillLayer(const PaintInfo&, const Color&, const FillLayer&, const LayoutRect&, SkXfermode::Mode op);
    void paintBoxShadow(const PaintInfo&, const ComputedStyle&, ShadowStyle, const LayoutRect& paintRect);
    LayoutRect roundedFrameRectClampedToLineTopAndBottomIfNeeded() const;
    LayoutRect paintRectForImageStrip(const LayoutPoint& paintOffset, const LayoutSize& frameSize, TextDirection) const;

    enum BorderPaintingType {
        DontPaintBorders,
        PaintBordersWithoutClip,
        PaintBordersWithClip
    };
    BorderPaintingType getBorderPaintType(const LayoutRect& adjustedFrameRect, LayoutRect& adjustedClipRect) const;

    InlineFlowBox& m_inlineFlowBox;
};

} // namespace blink

#endif // InlineFlowBoxPainter_h
