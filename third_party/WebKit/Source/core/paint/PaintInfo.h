/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 *           (C) 2004 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef PaintInfo_h
#define PaintInfo_h

#include "core/paint/PaintPhase.h"
#include "platform/geometry/IntRect.h"
#include "platform/geometry/LayoutRect.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "platform/transforms/AffineTransform.h"
#include "wtf/HashMap.h"
#include "wtf/ListHashSet.h"

#include <limits>

namespace blink {

class LayoutInline;
class LayoutBoxModelObject;
class LayoutObject;

struct PaintInfo {
    PaintInfo(GraphicsContext* newContext, const IntRect& newRect, PaintPhase newPhase, PaintBehavior newPaintBehavior,
        LayoutObject* newPaintingRoot = 0, ListHashSet<LayoutInline*>* newOutlineObjects = 0,
        const LayoutBoxModelObject* newPaintContainer = 0)
        : context(newContext)
        , rect(newRect)
        , phase(newPhase)
        , paintBehavior(newPaintBehavior)
        , paintingRoot(newPaintingRoot)
        , m_paintContainer(newPaintContainer)
        , m_outlineObjects(newOutlineObjects)
    {
    }

    void updatePaintingRootForChildren(const LayoutObject* layoutObject)
    {
        if (!paintingRoot)
            return;

        // If we're the painting root, kids draw normally, and see root of 0.
        if (paintingRoot == layoutObject) {
            paintingRoot = 0;
            return;
        }
    }

    bool shouldPaintWithinRoot(const LayoutObject* layoutObject) const
    {
        return !paintingRoot || paintingRoot == layoutObject;
    }

    bool isRenderingClipPathAsMaskImage() const { return paintBehavior & PaintBehaviorRenderingClipPathAsMask; }

    bool skipRootBackground() const { return paintBehavior & PaintBehaviorSkipRootBackground; }
    bool paintRootBackgroundOnly() const { return paintBehavior & PaintBehaviorRootBackgroundOnly; }

    DisplayItem::Type displayItemTypeForClipping() const { return DisplayItem::paintPhaseToClipBoxType(phase); }

    const LayoutBoxModelObject* paintContainer() const { return m_paintContainer; }

    ListHashSet<LayoutInline*>* outlineObjects() const { return m_outlineObjects; }
    void setOutlineObjects(ListHashSet<LayoutInline*>* objects) { m_outlineObjects = objects; }

    bool intersectsCullRect(const AffineTransform& transform, const FloatRect& boundingBox) const
    {
        return transform.mapRect(boundingBox).intersects(rect);
    }

    void updateCullRectForSVGTransform(const AffineTransform& localToParentTransform)
    {
        if (rect != LayoutRect::infiniteIntRect())
            rect = localToParentTransform.inverse().mapRect(rect);
    }

    // FIXME: Introduce setters/getters at some point. Requires a lot of changes throughout layout/.
    GraphicsContext* context;
    IntRect rect; // dirty rect used for culling non-intersecting layoutObjects
    PaintPhase phase;
    PaintBehavior paintBehavior;
    LayoutObject* paintingRoot; // used to draw just one element and its visual kids

private:
    const LayoutBoxModelObject* m_paintContainer; // the box model object that originates the current painting
    ListHashSet<LayoutInline*>* m_outlineObjects; // used to list outlines that should be painted by a block with inline children
};

} // namespace blink

#endif // PaintInfo_h
