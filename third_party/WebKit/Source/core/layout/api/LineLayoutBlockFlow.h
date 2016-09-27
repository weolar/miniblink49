// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LineLayoutBlockFlow_h
#define LineLayoutBlockFlow_h

#include "core/layout/FloatingObjects.h"
#include "core/layout/LayoutBlockFlow.h"
#include "core/layout/api/LineLayoutBox.h"
#include "platform/LayoutUnit.h"

namespace blink {

class LayoutBlockFlow;
class FloatingObject;
class LineInfo;
class LineWidth;

class LineLayoutBlockFlow : public LineLayoutBox {
public:
    explicit LineLayoutBlockFlow(LayoutBlockFlow* blockFlow)
        : LineLayoutBox(blockFlow)
    {
    }

    LineLayoutBlockFlow(const LineLayoutItem& item)
        : LineLayoutBox(item)
    {
        ASSERT(!item || item.isLayoutBlockFlow());
    }

    LineLayoutBlockFlow() { }

    LineLayoutItem firstChild() const
    {
        return LineLayoutItem(toBlockFlow()->firstChild());
    }
    LineLayoutItem lastChild() const
    {
        return LineLayoutItem(toBlockFlow()->lastChild());
    }

    LayoutUnit startAlignedOffsetForLine(LayoutUnit position, bool shouldIndentText)
    {
        return toBlockFlow()->startAlignedOffsetForLine(position, shouldIndentText);
    }

    LayoutUnit textIndentOffset() const
    {
        return toBlockFlow()->textIndentOffset();
    }

    LayoutUnit logicalWidthForChild(const LayoutBox& child) const
    {
        return toBlockFlow()->logicalWidthForChild(child);
    }

    LayoutUnit marginStartForChild(const LayoutBoxModelObject& child) const
    {
        return toBlockFlow()->marginStartForChild(child);
    }

    LayoutUnit marginEndForChild(const LayoutBoxModelObject& child) const
    {
        return toBlockFlow()->marginEndForChild(child);
    }

    LayoutUnit marginBeforeForChild(const LayoutBoxModelObject& child) const
    {
        return toBlockFlow()->marginBeforeForChild(child);
    }

    LayoutUnit startOffsetForContent() const
    {
        return toBlockFlow()->startOffsetForContent();
    }

    LayoutUnit lineHeight(bool firstLine, LineDirectionMode direction, LinePositionMode linePositionMode) const
    {
        return toBlockFlow()->lineHeight(firstLine, direction, linePositionMode);
    }

    LayoutUnit minLineHeightForReplacedObject(bool isFirstLine, LayoutUnit replacedHeight) const
    {
        return toBlockFlow()->minLineHeightForReplacedObject(isFirstLine, replacedHeight);
    }

    void setStaticInlinePositionForChild(LayoutBox& box, LayoutUnit inlinePosition)
    {
        toBlockFlow()->setStaticInlinePositionForChild(box, inlinePosition);
    }

    void updateStaticInlinePositionForChild(LayoutBox& box, LayoutUnit logicalTop)
    {
        toBlockFlow()->updateStaticInlinePositionForChild(box, logicalTop);
    }

    FloatingObject* insertFloatingObject(LayoutBox& box)
    {
        return toBlockFlow()->insertFloatingObject(box);
    }

    bool positionNewFloats(LineWidth* width)
    {
        return toBlockFlow()->positionNewFloats(width);
    }

    bool positionNewFloatOnLine(FloatingObject& newFloat, FloatingObject* lastFloatFromPreviousLine, LineInfo& lineInfo, LineWidth& lineWidth)
    {
        return toBlockFlow()->positionNewFloatOnLine(newFloat, lastFloatFromPreviousLine, lineInfo, lineWidth);
    }

    LayoutUnit nextFloatLogicalBottomBelow(LayoutUnit logicalHeight, ShapeOutsideFloatOffsetMode offsetMode = ShapeOutsideFloatMarginBoxOffset) const
    {
        return toBlockFlow()->nextFloatLogicalBottomBelow(logicalHeight, offsetMode);
    }

    FloatingObject* lastFloatFromPreviousLine() const
    {
        return toBlockFlow()->lastFloatFromPreviousLine();
    }

    LayoutUnit logicalTopForFloat(const FloatingObject& floatingObject) const
    {
        return toBlockFlow()->logicalTopForFloat(floatingObject);
    }

    LayoutUnit logicalBottomForFloat(const FloatingObject& floatingObject) const
    {
        return toBlockFlow()->logicalBottomForFloat(floatingObject);
    }

    LayoutUnit logicalLeftForFloat(const FloatingObject& floatingObject) const
    {
        return toBlockFlow()->logicalLeftForFloat(floatingObject);
    }

    LayoutUnit logicalRightForFloat(const FloatingObject& floatingObject) const
    {
        return toBlockFlow()->logicalRightForFloat(floatingObject);
    }

    LayoutUnit logicalWidthForFloat(const FloatingObject& floatingObject) const
    {
        return toBlockFlow()->logicalWidthForFloat(floatingObject);
    }

    LayoutUnit logicalRightOffsetForLine(LayoutUnit position, bool shouldIndentText, LayoutUnit logicalHeight = 0) const
    {
        return toBlockFlow()->logicalRightOffsetForLine(position, shouldIndentText, logicalHeight);
    }

    LayoutUnit logicalLeftOffsetForLine(LayoutUnit position, bool shouldIndentText, LayoutUnit logicalHeight = 0) const
    {
        return toBlockFlow()->logicalLeftOffsetForLine(position, shouldIndentText, logicalHeight);
    }

private:
    LayoutBlockFlow* toBlockFlow() { return toLayoutBlockFlow(layoutObject()); };
    const LayoutBlockFlow* toBlockFlow() const { return toLayoutBlockFlow(layoutObject()); };
};

} // namespace blink

#endif // LineLayoutBlockFlow_h
