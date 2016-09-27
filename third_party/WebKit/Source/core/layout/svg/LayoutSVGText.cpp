/*
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2006 Alexander Kellett <lypanov@kde.org>
 * Copyright (C) 2006 Oliver Hunt <ojh16@student.canterbury.ac.nz>
 * Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2008 Rob Buis <buis@kde.org>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
 * Copyright (C) Research In Motion Limited 2010-2012. All rights reserved.
 * Copyright (C) 2012 Google Inc.
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

#include "config.h"

#include "core/layout/svg/LayoutSVGText.h"

#include "core/editing/PositionWithAffinity.h"
#include "core/layout/HitTestRequest.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutAnalyzer.h"
#include "core/layout/LayoutState.h"
#include "core/layout/PointerEventsHitRules.h"
#include "core/layout/svg/LayoutSVGInline.h"
#include "core/layout/svg/LayoutSVGInlineText.h"
#include "core/layout/svg/LayoutSVGRoot.h"
#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/layout/svg/SVGResourcesCache.h"
#include "core/layout/svg/line/SVGRootInlineBox.h"
#include "core/paint/SVGTextPainter.h"
#include "core/style/ShadowList.h"
#include "core/svg/SVGLengthList.h"
#include "core/svg/SVGTextElement.h"
#include "core/svg/SVGTransformList.h"
#include "core/svg/SVGURIReference.h"
#include "platform/FloatConversion.h"
#include "platform/fonts/FontCache.h"
#include "platform/fonts/SimpleFontData.h"
#include "platform/geometry/FloatQuad.h"
#include "platform/geometry/TransformState.h"

namespace blink {

LayoutSVGText::LayoutSVGText(SVGTextElement* node)
    : LayoutSVGBlock(node)
    , m_needsReordering(false)
    , m_needsPositioningValuesUpdate(false)
    , m_needsTransformUpdate(true)
    , m_needsTextMetricsUpdate(false)
{
}

LayoutSVGText::~LayoutSVGText()
{
    ASSERT(m_layoutAttributes.isEmpty());
}

bool LayoutSVGText::isChildAllowed(LayoutObject* child, const ComputedStyle&) const
{
    return child->isSVGInline() || (child->isText() && SVGLayoutSupport::isLayoutableTextNode(child));
}

LayoutSVGText* LayoutSVGText::locateLayoutSVGTextAncestor(LayoutObject* start)
{
    ASSERT(start);
    while (start && !start->isSVGText())
        start = start->parent();
    if (!start || !start->isSVGText())
        return nullptr;
    return toLayoutSVGText(start);
}

const LayoutSVGText* LayoutSVGText::locateLayoutSVGTextAncestor(const LayoutObject* start)
{
    ASSERT(start);
    while (start && !start->isSVGText())
        start = start->parent();
    if (!start || !start->isSVGText())
        return nullptr;
    return toLayoutSVGText(start);
}

static inline void collectLayoutAttributes(LayoutObject* text, Vector<SVGTextLayoutAttributes*>& attributes)
{
    for (LayoutObject* descendant = text; descendant; descendant = descendant->nextInPreOrder(text)) {
        if (descendant->isSVGInlineText())
            attributes.append(toLayoutSVGInlineText(descendant)->layoutAttributes());
    }
}

static inline bool findPreviousAndNextAttributes(LayoutSVGText* root, LayoutSVGInlineText* locateElement, SVGTextLayoutAttributes*& previous, SVGTextLayoutAttributes*& next)
{
    ASSERT(root);
    ASSERT(locateElement);
    bool stopAfterNext = false;
    LayoutObject* current = root->firstChild();
    while (current) {
        if (current->isSVGInlineText()) {
            LayoutSVGInlineText* text = toLayoutSVGInlineText(current);
            if (locateElement != text) {
                if (stopAfterNext) {
                    next = text->layoutAttributes();
                    return true;
                }

                previous = text->layoutAttributes();
            } else {
                stopAfterNext = true;
            }
        } else if (current->isSVGInline()) {
            // Descend into text content (if possible).
            if (LayoutObject* child = toLayoutSVGInline(current)->firstChild()) {
                current = child;
                continue;
            }
        }

        current = current->nextInPreOrderAfterChildren(root);
    }
    return false;
}

inline bool LayoutSVGText::shouldHandleSubtreeMutations() const
{
    if (beingDestroyed() || !everHadLayout()) {
        ASSERT(m_layoutAttributes.isEmpty());
        ASSERT(!m_layoutAttributesBuilder.numberOfTextPositioningElements());
        return false;
    }
    return true;
}

void LayoutSVGText::subtreeChildWasAdded(LayoutObject* child)
{
    ASSERT(child);
    if (!shouldHandleSubtreeMutations() || documentBeingDestroyed())
        return;

    // Always protect the cache before clearing text positioning elements when the cache will subsequently be rebuilt.
    FontCachePurgePreventer fontCachePurgePreventer;

    // The positioning elements cache doesn't include the new 'child' yet. Clear the
    // cache, as the next buildLayoutAttributesForText() call rebuilds it.
    m_layoutAttributesBuilder.clearTextPositioningElements();

    if (!child->isSVGInlineText() && !child->isSVGInline())
        return;

    // Detect changes in layout attributes and only measure those text parts that have changed!
    Vector<SVGTextLayoutAttributes*> newLayoutAttributes;
    collectLayoutAttributes(this, newLayoutAttributes);
    if (newLayoutAttributes.isEmpty()) {
        ASSERT(m_layoutAttributes.isEmpty());
        return;
    }

    // Compare m_layoutAttributes with newLayoutAttributes to figure out which attribute got added.
    size_t size = newLayoutAttributes.size();
    SVGTextLayoutAttributes* attributes = nullptr;
    for (size_t i = 0; i < size; ++i) {
        attributes = newLayoutAttributes[i];
        if (m_layoutAttributes.find(attributes) == kNotFound) {
            // Every time this is invoked, there's only a single new entry in the newLayoutAttributes list, compared to the old in m_layoutAttributes.
            SVGTextLayoutAttributes* previous = nullptr;
            SVGTextLayoutAttributes* next = nullptr;
            ASSERT_UNUSED(child, attributes->context() == child);
            findPreviousAndNextAttributes(this, attributes->context(), previous, next);

            if (previous)
                m_layoutAttributesBuilder.buildLayoutAttributesForText(previous->context());
            m_layoutAttributesBuilder.buildLayoutAttributesForText(attributes->context());
            if (next)
                m_layoutAttributesBuilder.buildLayoutAttributesForText(next->context());
            break;
        }
    }

#if ENABLE(ASSERT)
    // Verify that m_layoutAttributes only differs by a maximum of one entry.
    for (size_t i = 0; i < size; ++i)
        ASSERT(m_layoutAttributes.find(newLayoutAttributes[i]) != kNotFound || newLayoutAttributes[i] == attributes);
#endif

    m_layoutAttributes = newLayoutAttributes;
}

static inline void checkLayoutAttributesConsistency(LayoutSVGText* text, Vector<SVGTextLayoutAttributes*>& expectedLayoutAttributes)
{
#if ENABLE(ASSERT)
    Vector<SVGTextLayoutAttributes*> newLayoutAttributes;
    collectLayoutAttributes(text, newLayoutAttributes);
    ASSERT(newLayoutAttributes == expectedLayoutAttributes);
#endif
}

void LayoutSVGText::willBeDestroyed()
{
    m_layoutAttributes.clear();
    m_layoutAttributesBuilder.clearTextPositioningElements();

    LayoutSVGBlock::willBeDestroyed();
}

void LayoutSVGText::subtreeChildWillBeRemoved(LayoutObject* child, Vector<SVGTextLayoutAttributes*, 2>& affectedAttributes)
{
    ASSERT(child);
    if (!shouldHandleSubtreeMutations())
        return;

    checkLayoutAttributesConsistency(this, m_layoutAttributes);

    // The positioning elements cache depends on the size of each text layoutObject in the
    // subtree. If this changes, clear the cache. It's going to be rebuilt below.
    m_layoutAttributesBuilder.clearTextPositioningElements();
    if (m_layoutAttributes.isEmpty() || !child->isSVGInlineText())
        return;

    // This logic requires that the 'text' child is still inserted in the tree.
    LayoutSVGInlineText* text = toLayoutSVGInlineText(child);
    SVGTextLayoutAttributes* previous = nullptr;
    SVGTextLayoutAttributes* next = nullptr;
    if (!documentBeingDestroyed())
        findPreviousAndNextAttributes(this, text, previous, next);

    if (previous)
        affectedAttributes.append(previous);
    if (next)
        affectedAttributes.append(next);

    size_t position = m_layoutAttributes.find(text->layoutAttributes());
    ASSERT(position != kNotFound);
    m_layoutAttributes.remove(position);
}

void LayoutSVGText::subtreeChildWasRemoved(const Vector<SVGTextLayoutAttributes*, 2>& affectedAttributes)
{
    if (!shouldHandleSubtreeMutations() || documentBeingDestroyed()) {
        ASSERT(affectedAttributes.isEmpty());
        return;
    }

    // This is called immediately after subtreeChildWillBeDestroyed, once the LayoutSVGInlineText::willBeDestroyed() method
    // passes on to the base class, which removes us from the layout tree. At this point we can update the layout attributes.
    unsigned size = affectedAttributes.size();
    for (unsigned i = 0; i < size; ++i)
        m_layoutAttributesBuilder.buildLayoutAttributesForText(affectedAttributes[i]->context());
}

void LayoutSVGText::subtreeStyleDidChange()
{
    if (!shouldHandleSubtreeMutations() || documentBeingDestroyed())
        return;

    checkLayoutAttributesConsistency(this, m_layoutAttributes);

    // Only update the metrics cache, but not the text positioning element cache
    // nor the layout attributes cached in the leaf #text layoutObjects.
    FontCachePurgePreventer fontCachePurgePreventer;
    for (LayoutObject* descendant = firstChild(); descendant; descendant = descendant->nextInPreOrder(this)) {
        if (descendant->isSVGInlineText())
            m_layoutAttributesBuilder.rebuildMetricsForTextLayoutObject(toLayoutSVGInlineText(descendant));
    }
}

void LayoutSVGText::subtreeTextDidChange(LayoutSVGInlineText* text)
{
    ASSERT(text);
    ASSERT(!beingDestroyed());
    if (!everHadLayout()) {
        ASSERT(m_layoutAttributes.isEmpty());
        ASSERT(!m_layoutAttributesBuilder.numberOfTextPositioningElements());
        return;
    }

    // Always protect the cache before clearing text positioning elements when the cache will subsequently be rebuilt.
    FontCachePurgePreventer fontCachePurgePreventer;

    // The positioning elements cache depends on the size of each text layoutObject in the
    // subtree. If this changes, clear the cache. It's going to be rebuilt below.
    m_layoutAttributesBuilder.clearTextPositioningElements();

    for (LayoutObject* descendant = text; descendant; descendant = descendant->nextInPreOrder(text)) {
        if (descendant->isSVGInlineText())
            m_layoutAttributesBuilder.buildLayoutAttributesForText(toLayoutSVGInlineText(descendant));
    }
}

static inline void updateFontInAllDescendants(LayoutObject* start, SVGTextLayoutAttributesBuilder* builder = nullptr)
{
    for (LayoutObject* descendant = start; descendant; descendant = descendant->nextInPreOrder(start)) {
        if (!descendant->isSVGInlineText())
            continue;
        LayoutSVGInlineText* text = toLayoutSVGInlineText(descendant);
        text->updateScaledFont();
        if (builder)
            builder->rebuildMetricsForTextLayoutObject(text);
    }
}

void LayoutSVGText::layout()
{
    ASSERT(needsLayout());
    LayoutAnalyzer::Scope analyzer(*this);

    subtreeStyleDidChange();

    bool updateCachedBoundariesInParents = false;
    if (m_needsTransformUpdate) {
        m_localTransform = toSVGTextElement(node())->calculateAnimatedLocalTransform();
        m_needsTransformUpdate = false;
        updateCachedBoundariesInParents = true;
    }

    if (!everHadLayout()) {
        // When laying out initially, collect all layout attributes, build the character data map,
        // and propogate resulting SVGLayoutAttributes to all LayoutSVGInlineText children in the subtree.
        ASSERT(m_layoutAttributes.isEmpty());
        collectLayoutAttributes(this, m_layoutAttributes);
        updateFontInAllDescendants(this);
        m_layoutAttributesBuilder.buildLayoutAttributesForForSubtree(*this);

        m_needsReordering = true;
        m_needsTextMetricsUpdate = false;
        m_needsPositioningValuesUpdate = false;
        updateCachedBoundariesInParents = true;
    } else if (m_needsPositioningValuesUpdate) {
        // When the x/y/dx/dy/rotate lists change, recompute the layout attributes, and eventually
        // update the on-screen font objects as well in all descendants.
        if (m_needsTextMetricsUpdate) {
            updateFontInAllDescendants(this);
            m_needsTextMetricsUpdate = false;
        }

        m_layoutAttributesBuilder.buildLayoutAttributesForForSubtree(*this);
        m_needsReordering = true;
        m_needsPositioningValuesUpdate = false;
        updateCachedBoundariesInParents = true;
    } else if (m_needsTextMetricsUpdate || SVGLayoutSupport::findTreeRootObject(this)->isLayoutSizeChanged()) {
        // If the root layout size changed (eg. window size changes) or the transform to the root
        // context has changed then recompute the on-screen font size.
        updateFontInAllDescendants(this, &m_layoutAttributesBuilder);

        ASSERT(!m_needsReordering);
        ASSERT(!m_needsPositioningValuesUpdate);
        m_needsTextMetricsUpdate = false;
        updateCachedBoundariesInParents = true;
    }

    checkLayoutAttributesConsistency(this, m_layoutAttributes);

    // Reduced version of LayoutBlock::layoutBlock(), which only takes care of SVG text.
    // All if branches that could cause early exit in LayoutBlocks layoutBlock() method are turned into assertions.
    ASSERT(!isInline());
    ASSERT(!simplifiedLayout());
    ASSERT(!scrollsOverflow());
    ASSERT(!hasControlClip());
    ASSERT(!positionedObjects());
    ASSERT(!isAnonymousBlock());

    if (!firstChild())
        setChildrenInline(true);

    // FIXME: We need to find a way to only layout the child boxes, if needed.
    FloatRect oldBoundaries = objectBoundingBox();
    ASSERT(childrenInline());

    rebuildFloatsFromIntruding();

    LayoutUnit beforeEdge = borderBefore() + paddingBefore();
    LayoutUnit afterEdge = borderAfter() + paddingAfter() + scrollbarLogicalHeight();
    setLogicalHeight(beforeEdge);

    LayoutState state(*this, locationOffset());
    LayoutUnit paintInvalidationLogicalTop = 0;
    LayoutUnit paintInvalidationLogicalBottom = 0;
    layoutInlineChildren(true, paintInvalidationLogicalTop, paintInvalidationLogicalBottom, afterEdge);

    if (m_needsReordering)
        m_needsReordering = false;

    // If we don't have any line boxes, then make sure the frame rect is still cleared.
    if (!firstLineBox())
        setFrameRect(LayoutRect());

    m_overflow.clear();
    addVisualEffectOverflow();

    if (!updateCachedBoundariesInParents)
        updateCachedBoundariesInParents = oldBoundaries != objectBoundingBox();

    // Invalidate all resources of this client if our layout changed.
    if (everHadLayout() && selfNeedsLayout())
        SVGResourcesCache::clientLayoutChanged(this);

    // If our bounds changed, notify the parents.
    if (updateCachedBoundariesInParents)
        LayoutSVGBlock::setNeedsBoundariesUpdate();

    clearNeedsLayout();
}

RootInlineBox* LayoutSVGText::createRootInlineBox()
{
    RootInlineBox* box = new SVGRootInlineBox(*this);
    box->setHasVirtualLogicalHeight();
    return box;
}

bool LayoutSVGText::nodeAtFloatPoint(HitTestResult& result, const FloatPoint& pointInParent, HitTestAction hitTestAction)
{
    PointerEventsHitRules hitRules(PointerEventsHitRules::SVG_TEXT_HITTESTING, result.hitTestRequest(), style()->pointerEvents());
    bool isVisible = (style()->visibility() == VISIBLE);
    if (isVisible || !hitRules.requireVisible) {
        if ((hitRules.canHitBoundingBox && !objectBoundingBox().isEmpty())
            || (hitRules.canHitStroke && (style()->svgStyle().hasStroke() || !hitRules.requireStroke))
            || (hitRules.canHitFill && (style()->svgStyle().hasFill() || !hitRules.requireFill))) {
            FloatPoint localPoint;
            if (!SVGLayoutSupport::transformToUserSpaceAndCheckClipping(this, localToParentTransform(), pointInParent, localPoint))
                return false;

            if (hitRules.canHitBoundingBox && !objectBoundingBox().contains(localPoint))
                return false;

            HitTestLocation hitTestLocation(LayoutPoint(flooredIntPoint(localPoint)));
            return LayoutBlock::nodeAtPoint(result, hitTestLocation, LayoutPoint(), hitTestAction);
        }
    }

    return false;
}

PositionWithAffinity LayoutSVGText::positionForPoint(const LayoutPoint& pointInContents)
{
    RootInlineBox* rootBox = firstRootBox();
    if (!rootBox)
        return createPositionWithAffinity(0, DOWNSTREAM);

    ASSERT(!rootBox->nextRootBox());
    ASSERT(childrenInline());

    InlineBox* closestBox = toSVGRootInlineBox(rootBox)->closestLeafChildForPosition(pointInContents);
    if (!closestBox)
        return createPositionWithAffinity(0, DOWNSTREAM);

    return closestBox->layoutObject().positionForPoint(LayoutPoint(pointInContents.x(), closestBox->y()));
}

void LayoutSVGText::absoluteQuads(Vector<FloatQuad>& quads, bool* wasFixed) const
{
    quads.append(localToAbsoluteQuad(strokeBoundingBox(), 0 /* mode */, wasFixed));
}

void LayoutSVGText::paint(const PaintInfo& paintInfo, const LayoutPoint&)
{
    SVGTextPainter(*this).paint(paintInfo);
}

FloatRect LayoutSVGText::strokeBoundingBox() const
{
    FloatRect strokeBoundaries = objectBoundingBox();
    const SVGComputedStyle& svgStyle = style()->svgStyle();
    if (!svgStyle.hasStroke())
        return strokeBoundaries;

    ASSERT(node());
    ASSERT(node()->isSVGElement());
    SVGLengthContext lengthContext(toSVGElement(node()));
    strokeBoundaries.inflate(lengthContext.valueForLength(svgStyle.strokeWidth()));
    return strokeBoundaries;
}

FloatRect LayoutSVGText::paintInvalidationRectInLocalCoordinates() const
{
    FloatRect paintInvalidationRect = strokeBoundingBox();
    SVGLayoutSupport::intersectPaintInvalidationRectWithResources(this, paintInvalidationRect);

    if (const ShadowList* textShadow = style()->textShadow())
        textShadow->adjustRectForShadow(paintInvalidationRect);

    return paintInvalidationRect;
}

void LayoutSVGText::addChild(LayoutObject* child, LayoutObject* beforeChild)
{
    LayoutSVGBlock::addChild(child, beforeChild);

    SVGResourcesCache::clientWasAddedToTree(child, child->styleRef());
    subtreeChildWasAdded(child);
}

void LayoutSVGText::removeChild(LayoutObject* child)
{
    SVGResourcesCache::clientWillBeRemovedFromTree(child);

    Vector<SVGTextLayoutAttributes*, 2> affectedAttributes;
    FontCachePurgePreventer fontCachePurgePreventer;
    subtreeChildWillBeRemoved(child, affectedAttributes);
    LayoutSVGBlock::removeChild(child);
    subtreeChildWasRemoved(affectedAttributes);
}

}
