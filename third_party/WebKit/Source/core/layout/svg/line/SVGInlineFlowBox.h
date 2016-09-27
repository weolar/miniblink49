/*
 * Copyright (C) 2006 Oliver Hunt <ojh16@student.canterbury.ac.nz>
 * Copyright (C) 2006 Apple Computer Inc.
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

#ifndef SVGInlineFlowBox_h
#define SVGInlineFlowBox_h

#include "core/layout/line/InlineFlowBox.h"

namespace blink {

class SVGInlineFlowBox final : public InlineFlowBox {
public:
    SVGInlineFlowBox(LayoutObject& obj)
        : InlineFlowBox(obj)
        , m_logicalHeight(0)
    {
    }

    virtual bool isSVGInlineFlowBox() const override { return true; }
    virtual LayoutUnit virtualLogicalHeight() const override { return m_logicalHeight; }
    void setLogicalHeight(LayoutUnit h) { m_logicalHeight = h; }

    virtual void paint(const PaintInfo&, const LayoutPoint&, LayoutUnit lineTop, LayoutUnit lineBottom) override;

    virtual LayoutRect calculateBoundaries() const override;

private:
    LayoutUnit m_logicalHeight;
};

DEFINE_INLINE_BOX_TYPE_CASTS(SVGInlineFlowBox);

} // namespace blink

#endif // SVGInlineFlowBox_h
