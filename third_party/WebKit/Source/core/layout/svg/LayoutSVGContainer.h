/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2009 Google, Inc.  All rights reserved.
 * Copyright (C) 2009 Apple Inc. All rights reserved.
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

#ifndef LayoutSVGContainer_h
#define LayoutSVGContainer_h

#include "core/layout/svg/LayoutSVGModelObject.h"

namespace blink {

class SVGElement;

class LayoutSVGContainer : public LayoutSVGModelObject {
public:
    explicit LayoutSVGContainer(SVGElement*);
    virtual ~LayoutSVGContainer();

    // If you have a LayoutSVGContainer, use firstChild or lastChild instead.
    void slowFirstChild() const = delete;
    void slowLastChild() const = delete;

    LayoutObject* firstChild() const { ASSERT(children() == virtualChildren()); return children()->firstChild(); }
    LayoutObject* lastChild() const { ASSERT(children() == virtualChildren()); return children()->lastChild(); }

    virtual void paint(const PaintInfo&, const LayoutPoint&) override;
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;
    virtual void setNeedsBoundariesUpdate() override final { m_needsBoundariesUpdate = true; }
    virtual bool didTransformToRootUpdate() { return false; }
    bool isObjectBoundingBoxValid() const { return m_objectBoundingBoxValid; }

    bool selfWillPaint();

    virtual bool hasNonIsolatedBlendingDescendants() const override final;

    virtual const char* name() const override { return "LayoutSVGContainer"; }

    virtual FloatRect objectBoundingBox() const override final { return m_objectBoundingBox; }

protected:
    virtual LayoutObjectChildList* virtualChildren() override final { return children(); }
    virtual const LayoutObjectChildList* virtualChildren() const override final { return children(); }

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectSVGContainer || LayoutSVGModelObject::isOfType(type); }
    virtual void layout() override;

    virtual void addChild(LayoutObject* child, LayoutObject* beforeChild = nullptr) override final;
    virtual void removeChild(LayoutObject*) override final;
    virtual void addFocusRingRects(Vector<LayoutRect>&, const LayoutPoint& additionalOffset) const override final;

    virtual FloatRect strokeBoundingBox() const override final { return m_strokeBoundingBox; }

    virtual bool nodeAtFloatPoint(HitTestResult&, const FloatPoint& pointInParent, HitTestAction) override;

    // Allow LayoutSVGTransformableContainer to hook in at the right time in layout().
    virtual bool calculateLocalTransform() { return false; }

    // Allow LayoutSVGViewportContainer to hook in at the right times in layout() and nodeAtFloatPoint().
    virtual void calcViewport() { }
    virtual bool pointIsInsideViewportClip(const FloatPoint& /*pointInParent*/) { return true; }

    virtual void determineIfLayoutSizeChanged() { }

    void updateCachedBoundaries();

    virtual void descendantIsolationRequirementsChanged(DescendantIsolationState) override final;

private:
    const LayoutObjectChildList* children() const { return &m_children; }
    LayoutObjectChildList* children() { return &m_children; }

    LayoutObjectChildList m_children;
    FloatRect m_objectBoundingBox;
    FloatRect m_strokeBoundingBox;
    bool m_objectBoundingBoxValid;
    bool m_needsBoundariesUpdate;
    mutable bool m_hasNonIsolatedBlendingDescendants : 1;
    mutable bool m_hasNonIsolatedBlendingDescendantsDirty : 1;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutSVGContainer, isSVGContainer());

} // namespace blink

#endif // LayoutSVGContainer_h
