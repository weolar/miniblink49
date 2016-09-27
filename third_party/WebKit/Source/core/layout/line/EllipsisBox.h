/**
 * Copyright (C) 2003, 2006 Apple Computer, Inc.
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
 */

#ifndef EllipsisBox_h
#define EllipsisBox_h

#include "core/layout/line/InlineBox.h"

namespace blink {

class HitTestRequest;
class HitTestResult;

class EllipsisBox final : public InlineBox {
public:
    EllipsisBox(LayoutObject& obj, const AtomicString& ellipsisStr, InlineFlowBox* parent,
        int width, int height, int x, int y, bool firstLine, bool isVertical)
        : InlineBox(obj, LayoutPoint(x, y), width, firstLine, true, false, false, isVertical, 0, 0, parent)
        , m_height(height)
        , m_str(ellipsisStr)
        , m_selectionState(LayoutObject::SelectionNone)
    {
        setHasVirtualLogicalHeight();
    }

    virtual void paint(const PaintInfo&, const LayoutPoint&, LayoutUnit lineTop, LayoutUnit lineBottom) override;
    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, LayoutUnit lineTop, LayoutUnit lineBottom) override;
    void setSelectionState(LayoutObject::SelectionState s) { m_selectionState = s; }
    IntRect selectionRect();

    virtual LayoutUnit virtualLogicalHeight() const override { return m_height; }
    virtual LayoutObject::SelectionState selectionState() const override { return m_selectionState; }
    const AtomicString& ellipsisStr() { return m_str; }

    virtual const char* boxName() const override;

private:

    int m_height;
    AtomicString m_str;
    LayoutObject::SelectionState m_selectionState;
};

} // namespace blink

#endif // EllipsisBox_h
