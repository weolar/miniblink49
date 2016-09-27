/*
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Google, Inc.
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

#ifndef LayoutSVGGradientStop_h
#define LayoutSVGGradientStop_h

#include "core/layout/LayoutObject.h"

namespace blink {

class SVGGradientElement;
class SVGStopElement;

// This class exists mostly so we can hear about gradient stop style changes
class LayoutSVGGradientStop final : public LayoutObject {
public:
    explicit LayoutSVGGradientStop(SVGStopElement*);
    virtual ~LayoutSVGGradientStop();

    virtual const char* name() const override { return "LayoutSVGGradientStop"; }
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectSVG || type == LayoutObjectSVGGradientStop || LayoutObject::isOfType(type); }

    virtual void layout() override;

    // This overrides are needed to prevent ASSERTs on <svg><stop /></svg>
    // LayoutObject's default implementations ASSERT_NOT_REACHED()
    // https://bugs.webkit.org/show_bug.cgi?id=20400
    virtual LayoutRect clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject*, const PaintInvalidationState* = nullptr) const override { return LayoutRect(); }
    virtual FloatRect objectBoundingBox() const override { return FloatRect(); }
    virtual FloatRect strokeBoundingBox() const override { return FloatRect(); }
    virtual FloatRect paintInvalidationRectInLocalCoordinates() const override { return FloatRect(); }

protected:
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;

private:
    SVGGradientElement* gradientElement() const;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutSVGGradientStop, isSVGGradientStop());

}

#endif // LayoutSVGGradientStop_h
