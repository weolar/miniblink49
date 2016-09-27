/*
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) Research In Motion Limited 2010-2012. All rights reserved.
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

#ifndef LayoutSVGText_h
#define LayoutSVGText_h

#include "core/layout/svg/LayoutSVGBlock.h"
#include "core/layout/svg/SVGTextLayoutAttributesBuilder.h"
#include "platform/transforms/AffineTransform.h"

namespace blink {

class LayoutSVGInlineText;
class SVGTextElement;
class LayoutSVGInlineText;

class LayoutSVGText final : public LayoutSVGBlock {
public:
    explicit LayoutSVGText(SVGTextElement*);
    virtual ~LayoutSVGText();

    virtual bool isChildAllowed(LayoutObject*, const ComputedStyle&) const override;

    void setNeedsPositioningValuesUpdate() { m_needsPositioningValuesUpdate = true; }
    virtual void setNeedsTransformUpdate() override { m_needsTransformUpdate = true; }
    void setNeedsTextMetricsUpdate() { m_needsTextMetricsUpdate = true; }
    virtual FloatRect paintInvalidationRectInLocalCoordinates() const override;

    static LayoutSVGText* locateLayoutSVGTextAncestor(LayoutObject*);
    static const LayoutSVGText* locateLayoutSVGTextAncestor(const LayoutObject*);

    bool needsReordering() const { return m_needsReordering; }
    Vector<SVGTextLayoutAttributes*>& layoutAttributes() { return m_layoutAttributes; }

    void subtreeChildWasAdded(LayoutObject*);
    void subtreeChildWillBeRemoved(LayoutObject*, Vector<SVGTextLayoutAttributes*, 2>& affectedAttributes);
    void subtreeChildWasRemoved(const Vector<SVGTextLayoutAttributes*, 2>& affectedAttributes);
    void subtreeStyleDidChange();
    void subtreeTextDidChange(LayoutSVGInlineText*);

    virtual const AffineTransform& localToParentTransform() const override { return m_localTransform; }

    virtual const char* name() const override { return "LayoutSVGText"; }

private:
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectSVGText || LayoutSVGBlock::isOfType(type); }

    virtual void paint(const PaintInfo&, const LayoutPoint&) override;
    virtual bool nodeAtFloatPoint(HitTestResult&, const FloatPoint& pointInParent, HitTestAction) override;
    virtual PositionWithAffinity positionForPoint(const LayoutPoint&) override;

    virtual void layout() override;

    virtual void absoluteQuads(Vector<FloatQuad>&, bool* wasFixed) const override;

    virtual void addChild(LayoutObject* child, LayoutObject* beforeChild = nullptr) override;
    virtual void removeChild(LayoutObject*) override;
    virtual void willBeDestroyed() override;

    virtual FloatRect objectBoundingBox() const override { return frameRect(); }
    virtual FloatRect strokeBoundingBox() const override;

    virtual RootInlineBox* createRootInlineBox() override;

    bool shouldHandleSubtreeMutations() const;

    bool m_needsReordering : 1;
    bool m_needsPositioningValuesUpdate : 1;
    bool m_needsTransformUpdate : 1;
    bool m_needsTextMetricsUpdate : 1;
    SVGTextLayoutAttributesBuilder m_layoutAttributesBuilder;
    Vector<SVGTextLayoutAttributes*> m_layoutAttributes;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutSVGText, isSVGText());

}

#endif
