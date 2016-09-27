/*
 * This file is part of the layout object implementation for KHTML.
 *
 * Copyright (C) 2003 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef LayoutDeprecatedFlexibleBox_h
#define LayoutDeprecatedFlexibleBox_h

#include "core/layout/LayoutBlock.h"

namespace blink {

class FlexBoxIterator;

class LayoutDeprecatedFlexibleBox final : public LayoutBlock {
public:
    LayoutDeprecatedFlexibleBox(Element&);
    virtual ~LayoutDeprecatedFlexibleBox();

    virtual const char* name() const override { return "LayoutDeprecatedFlexibleBox"; }

    virtual void styleWillChange(StyleDifference, const ComputedStyle& newStyle) override;

    virtual void layoutBlock(bool relayoutChildren) override;
    void layoutHorizontalBox(bool relayoutChildren);
    void layoutVerticalBox(bool relayoutChildren);

    virtual bool isDeprecatedFlexibleBox() const override { return true; }
    bool isStretchingChildren() const { return m_stretchingChildren; }
    virtual bool canCollapseAnonymousBlockChild() const override { return false; }

    void placeChild(LayoutBox* child, const LayoutPoint& location);

private:
    virtual void computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const override;

    LayoutUnit allowedChildFlex(LayoutBox* child, bool expanding, unsigned group);

    bool hasMultipleLines() const { return style()->boxLines() == MULTIPLE; }
    bool isVertical() const { return style()->boxOrient() == VERTICAL; }
    bool isHorizontal() const { return style()->boxOrient() == HORIZONTAL; }

    void applyLineClamp(FlexBoxIterator&, bool relayoutChildren);
    void clearLineClamp();

    bool m_stretchingChildren;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutDeprecatedFlexibleBox, isDeprecatedFlexibleBox());

} // namespace blink

#endif // LayoutDeprecatedFlexibleBox_h
