/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
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

#ifndef LayoutBR_h
#define LayoutBR_h

#include "core/layout/LayoutText.h"

/*
 * The whole class here is a hack to get <br> working, as long as we don't have support for
 * CSS2 :before and :after pseudo elements
 */
namespace blink {

class LayoutBR final : public LayoutText {
public:
    explicit LayoutBR(Node*);
    virtual ~LayoutBR();

    virtual const char* name() const override { return "LayoutBR"; }

    virtual LayoutRect selectionRectForPaintInvalidation(const LayoutBoxModelObject* /* paintInvalidationContainer */) const override { return LayoutRect(); }

    virtual float width(unsigned /* from */, unsigned /* len */, const Font&, LayoutUnit /* xpos */, TextDirection, HashSet<const SimpleFontData*>* = nullptr /* fallbackFonts */ , FloatRect* /* glyphBounds */ = nullptr) const override { return 0; }
    virtual float width(unsigned /* from */, unsigned /* len */, LayoutUnit /* xpos */, TextDirection, bool = false /* firstLine */, HashSet<const SimpleFontData*>* = nullptr /* fallbackFonts */, FloatRect* /* glyphBounds */ = nullptr) const override { return 0; }

    int lineHeight(bool firstLine) const;

    // overrides
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectBr || LayoutText::isOfType(type); }

    virtual int caretMinOffset() const override;
    virtual int caretMaxOffset() const override;

    virtual PositionWithAffinity positionForPoint(const LayoutPoint&) override final;

protected:
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutBR, isBR());

} // namespace blink

#endif // LayoutBR_h
