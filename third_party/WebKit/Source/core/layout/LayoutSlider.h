/*
 * Copyright (C) 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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

#ifndef LayoutSlider_h
#define LayoutSlider_h

#include "core/layout/LayoutFlexibleBox.h"

namespace blink {

class HTMLInputElement;
class SliderThumbElement;

class LayoutSlider final : public LayoutFlexibleBox {
public:
    static const int defaultTrackLength;

    explicit LayoutSlider(HTMLInputElement*);
    virtual ~LayoutSlider();

    bool inDragMode() const;

    virtual const char* name() const override { return "LayoutSlider"; }

private:
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectSlider || LayoutFlexibleBox::isOfType(type); }

    virtual int baselinePosition(FontBaseline, bool firstLine, LineDirectionMode, LinePositionMode = PositionOnContainingLine) const override;
    virtual void computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const override;
    virtual void layout() override;

    SliderThumbElement* sliderThumbElement() const;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutSlider, isSlider());

} // namespace blink

#endif // LayoutSlider_h
