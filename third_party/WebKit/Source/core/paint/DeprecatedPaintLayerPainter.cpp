// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/DeprecatedPaintLayerPainter.h"

#include "core/frame/Settings.h"
#include "core/layout/ClipPathOperation.h"
#include "core/layout/LayoutBlock.h"
#include "core/layout/LayoutView.h"
#include "core/layout/svg/LayoutSVGResourceClipper.h"
#include "core/page/Page.h"
#include "core/paint/CompositingRecorder.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/FilterPainter.h"
#include "core/paint/LayerClipRecorder.h"
#include "core/paint/LayerFixedPositionRecorder.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/SVGClipPainter.h"
#include "core/paint/ScopeRecorder.h"
#include "core/paint/ScrollRecorder.h"
#include "core/paint/ScrollableAreaPainter.h"
#include "core/paint/Transform3DRecorder.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/graphics/paint/ClipPathRecorder.h"
#include "platform/graphics/paint/ClipRecorder.h"
#include "platform/graphics/paint/CompositingDisplayItem.h"
#include "platform/graphics/paint/Transform3DDisplayItem.h"
#include "wtf/Optional.h"

namespace blink {

static inline bool shouldSuppressPaintingLayer(DeprecatedPaintLayer* layer)
{
    // Avoid painting descendants of the root layer when stylesheets haven't loaded. This eliminates FOUC.
    // It's ok not to draw, because later on, when all the stylesheets do load, updateStyleSelector on the Document
    // will do a full paintInvalidationForWholeLayoutObject().
    if (layer->layoutObject()->document().didLayoutWithPendingStylesheets() && !layer->isRootLayer() && !layer->layoutObject()->isDocumentElement())
        return true;

    return false;
}

void DeprecatedPaintLayerPainter::paint(GraphicsContext* context, const LayoutRect& damageRect, PaintBehavior paintBehavior, LayoutObject* paintingRoot, PaintLayerFlags paintFlags)
{
    DeprecatedPaintLayerPaintingInfo paintingInfo(&m_paintLayer, LayoutRect(enclosingIntRect(damageRect)), paintBehavior, LayoutSize(), paintingRoot);
    if (shouldPaintLayerInSoftwareMode(paintingInfo, paintFlags))
        paintLayer(context, paintingInfo, paintFlags);
}

static ShouldRespectOverflowClip shouldRespectOverflowClip(PaintLayerFlags paintFlags, const LayoutObject* layoutObject)
{
    return (paintFlags & PaintLayerPaintingOverflowContents || (paintFlags & PaintLayerPaintingChildClippingMaskPhase && layoutObject->hasClipPath())) ? IgnoreOverflowClip : RespectOverflowClip;
}

void DeprecatedPaintLayerPainter::paintLayer(GraphicsContext* context, const DeprecatedPaintLayerPaintingInfo& paintingInfo, PaintLayerFlags paintFlags)
{
    // https://code.google.com/p/chromium/issues/detail?id=343772
    DisableCompositingQueryAsserts disabler;

    if (m_paintLayer.compositingState() != NotComposited) {
        if (paintingInfo.paintBehavior & PaintBehaviorFlattenCompositingLayers) {
            // FIXME: ok, but what about PaintBehaviorFlattenCompositingLayers? That's for printing.
            // FIXME: why isn't the code here global, as opposed to being set on each paintLayer() call?
            paintFlags |= PaintLayerUncachedClipRects;
        }
    }

    // Non self-painting leaf layers don't need to be painted as their layoutObject() should properly paint itself.
    if (!m_paintLayer.isSelfPaintingLayer() && !m_paintLayer.hasSelfPaintingLayerDescendant())
        return;

    if (shouldSuppressPaintingLayer(&m_paintLayer))
        return;

    // If this layer is totally invisible then there is nothing to paint.
    if (!m_paintLayer.layoutObject()->opacity())
        return;

    if (m_paintLayer.paintsWithTransparency(paintingInfo.paintBehavior))
        paintFlags |= PaintLayerHaveTransparency;

    LayerFixedPositionRecorder fixedPositionRecorder(*context, *m_paintLayer.layoutObject());

    // PaintLayerAppliedTransform is used in LayoutReplica, to avoid applying the transform twice.
    if (m_paintLayer.paintsWithTransform(paintingInfo.paintBehavior) && !(paintFlags & PaintLayerAppliedTransform)) {
        paintLayerWithTransform(context, paintingInfo, paintFlags);
        return;
    }

    paintLayerContentsAndReflection(context, paintingInfo, paintFlags);
}

void DeprecatedPaintLayerPainter::paintLayerContentsAndReflection(GraphicsContext* context, const DeprecatedPaintLayerPaintingInfo& paintingInfo, PaintLayerFlags paintFlags, FragmentPolicy fragmentPolicy)
{
    ASSERT(m_paintLayer.isSelfPaintingLayer() || m_paintLayer.hasSelfPaintingLayerDescendant());

    PaintLayerFlags localPaintFlags = paintFlags & ~(PaintLayerAppliedTransform);

    // Paint the reflection first if we have one.
    if (m_paintLayer.reflectionInfo()) {
        ScopeRecorder scopeRecorder(*context, *m_paintLayer.layoutObject());
        m_paintLayer.reflectionInfo()->paint(context, paintingInfo, localPaintFlags | PaintLayerPaintingReflection);
    }

    localPaintFlags |= PaintLayerPaintingCompositingAllPhases;
    paintLayerContents(context, paintingInfo, localPaintFlags, fragmentPolicy);
}

class ClipPathHelper {
public:
    ClipPathHelper(GraphicsContext* context, const DeprecatedPaintLayer& paintLayer, const DeprecatedPaintLayerPaintingInfo& paintingInfo, LayoutRect& rootRelativeBounds, bool& rootRelativeBoundsComputed,
        const LayoutPoint& offsetFromRoot, PaintLayerFlags paintFlags)
        : m_resourceClipper(0), m_paintLayer(paintLayer), m_context(context)
    {
        const ComputedStyle& style = paintLayer.layoutObject()->styleRef();

        // Clip-path, like border radius, must not be applied to the contents of a composited-scrolling container.
        // It must, however, still be applied to the mask layer, so that the compositor can properly mask the
        // scrolling contents and scrollbars.
        if (!paintLayer.layoutObject()->hasClipPath() || (paintLayer.needsCompositedScrolling() && !(paintFlags & PaintLayerPaintingChildClippingMaskPhase)))
            return;

        m_clipperState = SVGClipPainter::ClipperNotApplied;

        ASSERT(style.clipPath());
        if (style.clipPath()->type() == ClipPathOperation::SHAPE) {
            ShapeClipPathOperation* clipPath = toShapeClipPathOperation(style.clipPath());
            if (clipPath->isValid()) {
                if (!rootRelativeBoundsComputed) {
                    rootRelativeBounds = paintLayer.physicalBoundingBoxIncludingReflectionAndStackingChildren(paintingInfo.rootLayer, offsetFromRoot);
                    rootRelativeBoundsComputed = true;
                }
                m_clipPathRecorder.emplace(*context, *paintLayer.layoutObject(), clipPath->path(rootRelativeBounds));
            }
        } else if (style.clipPath()->type() == ClipPathOperation::REFERENCE) {
            ReferenceClipPathOperation* referenceClipPathOperation = toReferenceClipPathOperation(style.clipPath());
            Document& document = paintLayer.layoutObject()->document();
            // FIXME: It doesn't work with forward or external SVG references (https://bugs.webkit.org/show_bug.cgi?id=90405)
            Element* element = document.getElementById(referenceClipPathOperation->fragment());
            if (isSVGClipPathElement(element) && element->layoutObject()) {
                if (!rootRelativeBoundsComputed) {
                    rootRelativeBounds = paintLayer.physicalBoundingBoxIncludingReflectionAndStackingChildren(paintingInfo.rootLayer, offsetFromRoot);
                    rootRelativeBoundsComputed = true;
                }

                m_resourceClipper = toLayoutSVGResourceClipper(toLayoutSVGResourceContainer(element->layoutObject()));
                if (!SVGClipPainter(*m_resourceClipper).applyClippingToContext(*paintLayer.layoutObject(), rootRelativeBounds,
                    paintingInfo.paintDirtyRect, context, m_clipperState)) {
                    // No need to post-apply the clipper if this failed.
                    m_resourceClipper = 0;
                }
            }
        }
    }

    ~ClipPathHelper()
    {
        if (m_resourceClipper)
            SVGClipPainter(*m_resourceClipper).postApplyStatefulResource(*m_paintLayer.layoutObject(), m_context, m_clipperState);
    }
private:
    LayoutSVGResourceClipper* m_resourceClipper;
    Optional<ClipPathRecorder> m_clipPathRecorder;
    SVGClipPainter::ClipperState m_clipperState;
    const DeprecatedPaintLayer& m_paintLayer;
    GraphicsContext* m_context;
};

void DeprecatedPaintLayerPainter::paintLayerContents(GraphicsContext* context, const DeprecatedPaintLayerPaintingInfo& paintingInfo, PaintLayerFlags paintFlags, FragmentPolicy fragmentPolicy)
{
    ASSERT(m_paintLayer.isSelfPaintingLayer() || m_paintLayer.hasSelfPaintingLayerDescendant());
    ASSERT(!(paintFlags & PaintLayerAppliedTransform));

    bool isSelfPaintingLayer = m_paintLayer.isSelfPaintingLayer();
    bool isPaintingOverlayScrollbars = paintFlags & PaintLayerPaintingOverlayScrollbars;
    bool isPaintingScrollingContent = paintFlags & PaintLayerPaintingCompositingScrollingPhase;
    bool isPaintingCompositedForeground = paintFlags & PaintLayerPaintingCompositingForegroundPhase;
    bool isPaintingCompositedBackground = paintFlags & PaintLayerPaintingCompositingBackgroundPhase;
    bool isPaintingOverflowContents = paintFlags & PaintLayerPaintingOverflowContents;
    // Outline always needs to be painted even if we have no visible content. Also,
    // the outline is painted in the background phase during composited scrolling.
    // If it were painted in the foreground phase, it would move with the scrolled
    // content. When not composited scrolling, the outline is painted in the
    // foreground phase. Since scrolled contents are moved by paint invalidation in this
    // case, the outline won't get 'dragged along'.
    bool shouldPaintOutline = isSelfPaintingLayer && !isPaintingOverlayScrollbars
        && ((isPaintingScrollingContent && isPaintingCompositedBackground)
        || (!isPaintingScrollingContent && isPaintingCompositedForeground));
    bool shouldPaintContent = m_paintLayer.hasVisibleContent() && isSelfPaintingLayer && !isPaintingOverlayScrollbars;

    if (paintFlags & PaintLayerPaintingRootBackgroundOnly && !m_paintLayer.layoutObject()->isLayoutView() && !m_paintLayer.layoutObject()->isDocumentElement())
        return;

    // Ensure our lists are up-to-date.
    m_paintLayer.stackingNode()->updateLayerListsIfNeeded();

    LayoutPoint offsetFromRoot;
    m_paintLayer.convertToLayerCoords(paintingInfo.rootLayer, offsetFromRoot);

    if (m_paintLayer.compositingState() == PaintsIntoOwnBacking)
        offsetFromRoot.move(m_paintLayer.subpixelAccumulation());
    else
        offsetFromRoot.move(paintingInfo.subPixelAccumulation);

    LayoutRect rootRelativeBounds;
    bool rootRelativeBoundsComputed = false;

    // These helpers output clip and compositing operations using a RAII pattern. Stack-allocated-varibles are destructed in the reverse order of construction,
    // so they are nested properly.
    ClipPathHelper clipPathHelper(context, m_paintLayer, paintingInfo, rootRelativeBounds, rootRelativeBoundsComputed, offsetFromRoot, paintFlags);

    Optional<LayerClipRecorder> clipRecorder;
    Optional<CompositingRecorder> compositingRecorder;
    // Blending operations must be performed only with the nearest ancestor stacking context.
    // Note that there is no need to composite if we're painting the root.
    // FIXME: this should be unified further into DeprecatedPaintLayer::paintsWithTransparency().
    bool shouldCompositeForBlendMode = (!m_paintLayer.layoutObject()->isDocumentElement() || m_paintLayer.layoutObject()->isSVGRoot()) && m_paintLayer.stackingNode()->isStackingContext() && m_paintLayer.hasNonIsolatedDescendantWithBlendMode();
    if (shouldCompositeForBlendMode || m_paintLayer.paintsWithTransparency(paintingInfo.paintBehavior)) {
        clipRecorder.emplace(*context, *m_paintLayer.layoutObject(), DisplayItem::TransparencyClip,
            m_paintLayer.paintingExtent(paintingInfo.rootLayer, paintingInfo.paintDirtyRect, paintingInfo.subPixelAccumulation, paintingInfo.paintBehavior),
            &paintingInfo, LayoutPoint(), paintFlags);

        compositingRecorder.emplace(*context, *m_paintLayer.layoutObject(),
            WebCoreCompositeToSkiaComposite(CompositeSourceOver, m_paintLayer.layoutObject()->style()->blendMode()),
            m_paintLayer.layoutObject()->opacity());
    }

    DeprecatedPaintLayerPaintingInfo localPaintingInfo(paintingInfo);

    DeprecatedPaintLayerFragments layerFragments;
    if (shouldPaintContent || shouldPaintOutline || isPaintingOverlayScrollbars) {
        // Collect the fragments. This will compute the clip rectangles and paint offsets for each layer fragment.
        ClipRectsCacheSlot cacheSlot = (paintFlags & PaintLayerUncachedClipRects) ? UncachedClipRects : PaintingClipRects;
        ShouldRespectOverflowClip respectOverflowClip = shouldRespectOverflowClip(paintFlags, m_paintLayer.layoutObject());
        if (fragmentPolicy == ForceSingleFragment)
            m_paintLayer.appendSingleFragmentIgnoringPagination(layerFragments, localPaintingInfo.rootLayer, localPaintingInfo.paintDirtyRect, cacheSlot, IgnoreOverlayScrollbarSize, respectOverflowClip, &offsetFromRoot, localPaintingInfo.subPixelAccumulation);
        else
            m_paintLayer.collectFragments(layerFragments, localPaintingInfo.rootLayer, localPaintingInfo.paintDirtyRect, cacheSlot, IgnoreOverlayScrollbarSize, respectOverflowClip, &offsetFromRoot, localPaintingInfo.subPixelAccumulation);
        if (shouldPaintContent)
            shouldPaintContent = atLeastOneFragmentIntersectsDamageRect(layerFragments, localPaintingInfo, paintFlags, offsetFromRoot);
    }

    bool selectionOnly = localPaintingInfo.paintBehavior & PaintBehaviorSelectionOnly;
    // If this layer's layoutObject is a child of the paintingRoot, we paint unconditionally, which
    // is done by passing a nil paintingRoot down to our layoutObject (as if no paintingRoot was ever set).
    // Else, our layout tree may or may not contain the painting root, so we pass that root along
    // so it will be tested against as we descend through the layoutObjects.
    LayoutObject* paintingRootForLayoutObject = 0;
    if (localPaintingInfo.paintingRoot && !m_paintLayer.layoutObject()->isDescendantOf(localPaintingInfo.paintingRoot))
        paintingRootForLayoutObject = localPaintingInfo.paintingRoot;

    { // Begin block for the lifetime of any filter.
        FilterPainter filterPainter(m_paintLayer, context, offsetFromRoot, layerFragments.isEmpty() ? ClipRect() : layerFragments[0].backgroundRect, localPaintingInfo, paintFlags,
            rootRelativeBounds, rootRelativeBoundsComputed);

        bool shouldPaintBackground = isPaintingCompositedBackground && shouldPaintContent && !selectionOnly;
        bool shouldPaintNegZOrderList = (isPaintingScrollingContent && isPaintingOverflowContents) || (!isPaintingScrollingContent && isPaintingCompositedBackground);
        bool shouldPaintOwnContents = isPaintingCompositedForeground && shouldPaintContent;
        bool shouldPaintNormalFlowAndPosZOrderLists = isPaintingCompositedForeground;
        bool shouldPaintOverlayScrollbars = isPaintingOverlayScrollbars;

        PaintBehavior paintBehavior = PaintBehaviorNormal;
        if (paintFlags & PaintLayerPaintingSkipRootBackground)
            paintBehavior |= PaintBehaviorSkipRootBackground;
        else if (paintFlags & PaintLayerPaintingRootBackgroundOnly)
            paintBehavior |= PaintBehaviorRootBackgroundOnly;

        if (shouldPaintBackground) {
            paintBackgroundForFragments(layerFragments, context, paintingInfo.paintDirtyRect,
                localPaintingInfo, paintBehavior, paintingRootForLayoutObject, paintFlags);
        }

        if (shouldPaintNegZOrderList)
            paintChildren(NegativeZOrderChildren, context, paintingInfo, paintFlags);

        if (shouldPaintOwnContents) {
            paintForegroundForFragments(layerFragments, context, paintingInfo.paintDirtyRect,
                localPaintingInfo, paintBehavior, paintingRootForLayoutObject, selectionOnly, paintFlags);
        }

        if (shouldPaintOutline)
            paintOutlineForFragments(layerFragments, context, localPaintingInfo, paintBehavior, paintingRootForLayoutObject, paintFlags);

        if (shouldPaintNormalFlowAndPosZOrderLists)
            paintChildren(NormalFlowChildren | PositiveZOrderChildren, context, paintingInfo, paintFlags);

        if (shouldPaintOverlayScrollbars)
            paintOverflowControlsForFragments(layerFragments, context, localPaintingInfo, paintFlags);
    } // FilterPainter block

    bool shouldPaintMask = (paintFlags & PaintLayerPaintingCompositingMaskPhase) && shouldPaintContent && m_paintLayer.layoutObject()->hasMask() && !selectionOnly;
    bool shouldPaintClippingMask = (paintFlags & PaintLayerPaintingChildClippingMaskPhase) && shouldPaintContent && !selectionOnly;

    if (shouldPaintMask)
        paintMaskForFragments(layerFragments, context, localPaintingInfo, paintingRootForLayoutObject, paintFlags);
    if (shouldPaintClippingMask) {
        // Paint the border radius mask for the fragments.
        paintChildClippingMaskForFragments(layerFragments, context, localPaintingInfo, paintingRootForLayoutObject, paintFlags);
    }
}

bool DeprecatedPaintLayerPainter::needsToClip(const DeprecatedPaintLayerPaintingInfo& localPaintingInfo, const ClipRect& clipRect)
{
    return clipRect.rect() != localPaintingInfo.paintDirtyRect || clipRect.hasRadius();
}

bool DeprecatedPaintLayerPainter::atLeastOneFragmentIntersectsDamageRect(DeprecatedPaintLayerFragments& fragments, const DeprecatedPaintLayerPaintingInfo& localPaintingInfo, PaintLayerFlags localPaintFlags, const LayoutPoint& offsetFromRoot)
{
    if (m_paintLayer.enclosingPaginationLayer())
        return true; // The fragments created have already been found to intersect with the damage rect.

    if (&m_paintLayer == localPaintingInfo.rootLayer && (localPaintFlags & PaintLayerPaintingOverflowContents))
        return true;

    for (DeprecatedPaintLayerFragment& fragment: fragments) {
        LayoutPoint newOffsetFromRoot = offsetFromRoot + fragment.paginationOffset;
        // Note that this really only works reliably on the first fragment. If the layer has visible
        // overflow and a subsequent fragment doesn't intersect with the border box of the layer
        // (i.e. only contains an overflow portion of the layer), intersection will fail. The reason
        // for this is that fragment.layerBounds is set to the border box, not the bounding box, of
        // the layer.
        if (m_paintLayer.intersectsDamageRect(fragment.layerBounds, fragment.backgroundRect.rect(), localPaintingInfo.rootLayer, &newOffsetFromRoot))
            return true;
    }
    return false;
}

void DeprecatedPaintLayerPainter::paintLayerWithTransform(GraphicsContext* context, const DeprecatedPaintLayerPaintingInfo& paintingInfo, PaintLayerFlags paintFlags)
{
    TransformationMatrix layerTransform = m_paintLayer.renderableTransform(paintingInfo.paintBehavior);
    // If the transform can't be inverted, then don't paint anything.
    if (!layerTransform.isInvertible())
        return;

    // FIXME: We should make sure that we don't walk past paintingInfo.rootLayer here.
    // m_paintLayer may be the "root", and then we should avoid looking at its parent.
    DeprecatedPaintLayer* parentLayer = m_paintLayer.parent();

    ClipRect ancestorBackgroundClipRect;
    if (parentLayer) {
        // Calculate the clip rectangle that the ancestors establish.
        ClipRectsContext clipRectsContext(paintingInfo.rootLayer, (paintFlags & PaintLayerUncachedClipRects) ? UncachedClipRects : PaintingClipRects, IgnoreOverlayScrollbarSize);
        if (shouldRespectOverflowClip(paintFlags, m_paintLayer.layoutObject()) == IgnoreOverflowClip)
            clipRectsContext.setIgnoreOverflowClip();
        ancestorBackgroundClipRect = m_paintLayer.clipper().backgroundClipRect(clipRectsContext);
    }

    DeprecatedPaintLayer* paginationLayer = m_paintLayer.enclosingPaginationLayer();
    DeprecatedPaintLayerFragments fragments;
    if (paginationLayer) {
        // FIXME: This is a mess. Look closely at this code and the code in Layer and fix any
        // issues in it & refactor to make it obvious from code structure what it does and that it's
        // correct.
        ClipRectsCacheSlot cacheSlot = (paintFlags & PaintLayerUncachedClipRects) ? UncachedClipRects : PaintingClipRects;
        ShouldRespectOverflowClip respectOverflowClip = shouldRespectOverflowClip(paintFlags, m_paintLayer.layoutObject());
        // Calculate the transformed bounding box in the current coordinate space, to figure out
        // which fragmentainers (e.g. columns) we need to visit.
        LayoutRect transformedExtent = DeprecatedPaintLayer::transparencyClipBox(&m_paintLayer, paginationLayer, DeprecatedPaintLayer::PaintingTransparencyClipBox, DeprecatedPaintLayer::RootOfTransparencyClipBox, paintingInfo.subPixelAccumulation, paintingInfo.paintBehavior);
        // FIXME: we don't check if paginationLayer is within paintingInfo.rootLayer here.
        paginationLayer->collectFragments(fragments, paintingInfo.rootLayer, paintingInfo.paintDirtyRect, cacheSlot, IgnoreOverlayScrollbarSize, respectOverflowClip, 0, paintingInfo.subPixelAccumulation, &transformedExtent);
    } else {
        // We don't need to collect any fragments in the regular way here. We have already
        // calculated a clip rectangle for the ancestry if it was needed, and clipping this
        // layer is something that can be done further down the path, when the transform has
        // been applied.
        DeprecatedPaintLayerFragment fragment;
        fragment.backgroundRect = paintingInfo.paintDirtyRect;
        fragments.append(fragment);
    }

    bool needsScope = fragments.size() > 1;
    for (const auto& fragment : fragments) {
        Optional<ScopeRecorder> scopeRecorder;
        if (needsScope)
            scopeRecorder.emplace(*context, *m_paintLayer.layoutObject());
        Optional<LayerClipRecorder> clipRecorder;
        if (parentLayer) {
            ClipRect clipRectForFragment(ancestorBackgroundClipRect);
            clipRectForFragment.moveBy(fragment.paginationOffset);
            clipRectForFragment.intersect(fragment.backgroundRect);
            if (clipRectForFragment.isEmpty())
                continue;
            if (needsToClip(paintingInfo, clipRectForFragment))
                clipRecorder.emplace(*context, *parentLayer->layoutObject(), DisplayItem::ClipLayerParent, clipRectForFragment, &paintingInfo, fragment.paginationOffset, paintFlags);
        }

        paintFragmentByApplyingTransform(context, paintingInfo, paintFlags, fragment.paginationOffset);
    }
}

void DeprecatedPaintLayerPainter::paintFragmentByApplyingTransform(GraphicsContext* context, const DeprecatedPaintLayerPaintingInfo& paintingInfo, PaintLayerFlags paintFlags, const LayoutPoint& fragmentTranslation)
{
    // This involves subtracting out the position of the layer in our current coordinate space, but preserving
    // the accumulated error for sub-pixel layout.
    LayoutPoint delta;
    m_paintLayer.convertToLayerCoords(paintingInfo.rootLayer, delta);
    delta.moveBy(fragmentTranslation);
    TransformationMatrix transform(m_paintLayer.renderableTransform(paintingInfo.paintBehavior));
    IntPoint roundedDelta = roundedIntPoint(delta);
    transform.translateRight(roundedDelta.x(), roundedDelta.y());
    LayoutSize adjustedSubPixelAccumulation = paintingInfo.subPixelAccumulation + (delta - roundedDelta);

    Transform3DRecorder transform3DRecorder(*context, *m_paintLayer.layoutObject(), DisplayItem::Transform3DElementTransform, transform);

    // Now do a paint with the root layer shifted to be us.
    DeprecatedPaintLayerPaintingInfo transformedPaintingInfo(&m_paintLayer, LayoutRect(enclosingIntRect(transform.inverse().mapRect(paintingInfo.paintDirtyRect))), paintingInfo.paintBehavior,
        adjustedSubPixelAccumulation, paintingInfo.paintingRoot);
    paintLayerContentsAndReflection(context, transformedPaintingInfo, paintFlags, ForceSingleFragment);
}

void DeprecatedPaintLayerPainter::paintChildren(unsigned childrenToVisit, GraphicsContext* context, const DeprecatedPaintLayerPaintingInfo& paintingInfo, PaintLayerFlags paintFlags)
{
    if (!m_paintLayer.hasSelfPaintingLayerDescendant())
        return;

#if ENABLE(ASSERT)
    LayerListMutationDetector mutationChecker(m_paintLayer.stackingNode());
#endif

    IntSize scrollOffsetAccumulation = paintingInfo.scrollOffsetAccumulation;
    if (m_paintLayer.layoutObject()->hasOverflowClip())
        scrollOffsetAccumulation += m_paintLayer.layoutBox()->scrolledContentOffset();

    DeprecatedPaintLayerStackingNodeIterator iterator(*m_paintLayer.stackingNode(), childrenToVisit);
    while (DeprecatedPaintLayerStackingNode* child = iterator.next()) {
        DeprecatedPaintLayerPainter childPainter(*child->layer());
        // If this Layer should paint into its own backing or a grouped backing, that will be done via CompositedDeprecatedPaintLayerMapping::paintContents()
        // and CompositedDeprecatedPaintLayerMapping::doPaintTask().
        if (!childPainter.shouldPaintLayerInSoftwareMode(paintingInfo, paintFlags))
            continue;

        DeprecatedPaintLayerPaintingInfo childPaintingInfo = paintingInfo;
        childPaintingInfo.scrollOffsetAccumulation = scrollOffsetAccumulation;
        // Rare case: accumulate scroll offset of non-stacking-context ancestors up to m_paintLayer.
        for (DeprecatedPaintLayer* parentLayer = child->layer()->parent(); parentLayer != &m_paintLayer; parentLayer = parentLayer->parent()) {
            if (parentLayer->layoutObject()->hasOverflowClip())
                childPaintingInfo.scrollOffsetAccumulation += parentLayer->layoutBox()->scrolledContentOffset();
        }

        childPainter.paintLayer(context, childPaintingInfo, paintFlags);
    }
}

// FIXME: inline this.
static bool paintForFixedRootBackground(const DeprecatedPaintLayer* layer, PaintLayerFlags paintFlags)
{
    return layer->layoutObject()->isDocumentElement() && (paintFlags & PaintLayerPaintingRootBackgroundOnly);
}

bool DeprecatedPaintLayerPainter::shouldPaintLayerInSoftwareMode(const DeprecatedPaintLayerPaintingInfo& paintingInfo, PaintLayerFlags paintFlags)
{
    DisableCompositingQueryAsserts disabler;
    
    return m_paintLayer.compositingState() == NotComposited
        || (paintingInfo.paintBehavior & PaintBehaviorFlattenCompositingLayers)
        || ((paintFlags & PaintLayerPaintingReflection) && !m_paintLayer.has3DTransform())
        || paintForFixedRootBackground(&m_paintLayer, paintFlags);
}

void DeprecatedPaintLayerPainter::paintOverflowControlsForFragments(const DeprecatedPaintLayerFragments& layerFragments, GraphicsContext* context, const DeprecatedPaintLayerPaintingInfo& localPaintingInfo, PaintLayerFlags paintFlags)
{
    bool needsScope = layerFragments.size() > 1;
    for (auto& fragment : layerFragments) {
        Optional<ScopeRecorder> scopeRecorder;
        if (needsScope)
            scopeRecorder.emplace(*context, *m_paintLayer.layoutObject());

        Optional<LayerClipRecorder> clipRecorder;

        if (needsToClip(localPaintingInfo, fragment.backgroundRect))
            clipRecorder.emplace(*context, *m_paintLayer.layoutObject(), DisplayItem::ClipLayerOverflowControls, fragment.backgroundRect, &localPaintingInfo, fragment.paginationOffset, paintFlags);
        if (DeprecatedPaintLayerScrollableArea* scrollableArea = m_paintLayer.scrollableArea())
            ScrollableAreaPainter(*scrollableArea).paintOverflowControls(context, roundedIntPoint(toPoint(fragment.layerBounds.location() - m_paintLayer.layoutBoxLocation())), pixelSnappedIntRect(fragment.backgroundRect.rect()), true);
    }
}

void DeprecatedPaintLayerPainter::paintFragmentWithPhase(PaintPhase phase, const DeprecatedPaintLayerFragment& fragment, GraphicsContext* context, const ClipRect& clipRect, const DeprecatedPaintLayerPaintingInfo& paintingInfo, PaintBehavior paintBehavior, LayoutObject* paintingRootForLayoutObject, PaintLayerFlags paintFlags, ClipState clipState)
{
    ASSERT(m_paintLayer.isSelfPaintingLayer());

    Optional<LayerClipRecorder> clipRecorder;
    if (clipState != HasClipped && paintingInfo.clipToDirtyRect && needsToClip(paintingInfo, clipRect)) {
        DisplayItem::Type clipType = DisplayItem::paintPhaseToClipLayerFragmentType(phase);
        LayerClipRecorder::BorderRadiusClippingRule clippingRule;
        switch (phase) {
        case PaintPhaseBlockBackground: // Background painting will handle clipping to self.
        case PaintPhaseSelfOutline:
        case PaintPhaseMask: // Mask painting will handle clipping to self.
            clippingRule = LayerClipRecorder::DoNotIncludeSelfForBorderRadius;
            break;
        default:
            clippingRule = LayerClipRecorder::IncludeSelfForBorderRadius;
            break;
        }

        clipRecorder.emplace(*context, *m_paintLayer.layoutObject(), clipType, clipRect, &paintingInfo, fragment.paginationOffset, paintFlags, clippingRule);
    }

    PaintInfo paintInfo(context, pixelSnappedIntRect(clipRect.rect()), phase, paintBehavior, paintingRootForLayoutObject, 0, paintingInfo.rootLayer->layoutObject());
    Optional<ScrollRecorder> scrollRecorder;
    LayoutPoint paintOffset = toPoint(fragment.layerBounds.location() - m_paintLayer.layoutBoxLocation());
    if (!paintingInfo.scrollOffsetAccumulation.isZero()) {
        // As a descendant of the root layer, m_paintLayer's painting is not controlled by the ScrollRecorders
        // created by BlockPainter of the ancestor layers up to the root layer, so we need to issue ScrollRecorder
        // for this layer seperately, with the scroll offset accumulated from the root layer to the parent of this
        // layer, to get the same result as ScrollRecorder in BlockPainter.
        paintOffset += paintingInfo.scrollOffsetAccumulation;
        paintInfo.rect.move(paintingInfo.scrollOffsetAccumulation);
        scrollRecorder.emplace(*paintInfo.context, *m_paintLayer.layoutObject(), paintInfo.phase, paintingInfo.scrollOffsetAccumulation);
    }
    m_paintLayer.layoutObject()->paint(paintInfo, paintOffset);
}

void DeprecatedPaintLayerPainter::paintBackgroundForFragments(const DeprecatedPaintLayerFragments& layerFragments, GraphicsContext* context,
    const LayoutRect& transparencyPaintDirtyRect, const DeprecatedPaintLayerPaintingInfo& localPaintingInfo, PaintBehavior paintBehavior,
    LayoutObject* paintingRootForLayoutObject, PaintLayerFlags paintFlags)
{
    bool needsScope = layerFragments.size() > 1;
    for (auto& fragment : layerFragments) {
        Optional<ScopeRecorder> scopeRecorder;
        if (needsScope)
            scopeRecorder.emplace(*context, *m_paintLayer.layoutObject());
        paintFragmentWithPhase(PaintPhaseBlockBackground, fragment, context, fragment.backgroundRect, localPaintingInfo, paintBehavior, paintingRootForLayoutObject, paintFlags, HasNotClipped);
    }
}

void DeprecatedPaintLayerPainter::paintForegroundForFragments(const DeprecatedPaintLayerFragments& layerFragments, GraphicsContext* context,
    const LayoutRect& transparencyPaintDirtyRect, const DeprecatedPaintLayerPaintingInfo& localPaintingInfo, PaintBehavior paintBehavior,
    LayoutObject* paintingRootForLayoutObject, bool selectionOnly, PaintLayerFlags paintFlags)
{
    // Optimize clipping for the single fragment case.
    bool shouldClip = localPaintingInfo.clipToDirtyRect && layerFragments.size() == 1 && !layerFragments[0].foregroundRect.isEmpty();
    ClipState clipState = HasNotClipped;
    Optional<LayerClipRecorder> clipRecorder;
    if (shouldClip && needsToClip(localPaintingInfo, layerFragments[0].foregroundRect)) {
        clipRecorder.emplace(*context, *m_paintLayer.layoutObject(), DisplayItem::ClipLayerForeground, layerFragments[0].foregroundRect, &localPaintingInfo, layerFragments[0].paginationOffset, paintFlags);
        clipState = HasClipped;
    }

    // We have to loop through every fragment multiple times, since we have to issue paint invalidations in each specific phase in order for
    // interleaving of the fragments to work properly.
    paintForegroundForFragmentsWithPhase(selectionOnly ? PaintPhaseSelection : PaintPhaseChildBlockBackgrounds, layerFragments,
        context, localPaintingInfo, paintBehavior, paintingRootForLayoutObject, paintFlags, clipState);

    if (!selectionOnly) {
        paintForegroundForFragmentsWithPhase(PaintPhaseFloat, layerFragments, context, localPaintingInfo, paintBehavior, paintingRootForLayoutObject, paintFlags, clipState);
        paintForegroundForFragmentsWithPhase(PaintPhaseForeground, layerFragments, context, localPaintingInfo, paintBehavior, paintingRootForLayoutObject, paintFlags, clipState);
        paintForegroundForFragmentsWithPhase(PaintPhaseChildOutlines, layerFragments, context, localPaintingInfo, paintBehavior, paintingRootForLayoutObject, paintFlags, clipState);
    }
}

void DeprecatedPaintLayerPainter::paintForegroundForFragmentsWithPhase(PaintPhase phase, const DeprecatedPaintLayerFragments& layerFragments, GraphicsContext* context,
    const DeprecatedPaintLayerPaintingInfo& localPaintingInfo, PaintBehavior paintBehavior, LayoutObject* paintingRootForLayoutObject, PaintLayerFlags paintFlags, ClipState clipState)
{
    bool needsScope = layerFragments.size() > 1;
    for (auto& fragment : layerFragments) {
        if (!fragment.foregroundRect.isEmpty()) {
            Optional<ScopeRecorder> scopeRecorder;
            if (needsScope)
                scopeRecorder.emplace(*context, *m_paintLayer.layoutObject());
            paintFragmentWithPhase(phase, fragment, context, fragment.foregroundRect, localPaintingInfo, paintBehavior, paintingRootForLayoutObject, paintFlags, clipState);
        }
    }
}

void DeprecatedPaintLayerPainter::paintOutlineForFragments(const DeprecatedPaintLayerFragments& layerFragments, GraphicsContext* context, const DeprecatedPaintLayerPaintingInfo& localPaintingInfo,
    PaintBehavior paintBehavior, LayoutObject* paintingRootForLayoutObject, PaintLayerFlags paintFlags)
{
    bool needsScope = layerFragments.size() > 1;
    for (auto& fragment : layerFragments) {
        if (!fragment.outlineRect.isEmpty()) {
            Optional<ScopeRecorder> scopeRecorder;
            if (needsScope)
                scopeRecorder.emplace(*context, *m_paintLayer.layoutObject());
            paintFragmentWithPhase(PaintPhaseSelfOutline, fragment, context, fragment.outlineRect, localPaintingInfo, paintBehavior, paintingRootForLayoutObject, paintFlags, HasNotClipped);
        }
    }
}

void DeprecatedPaintLayerPainter::paintMaskForFragments(const DeprecatedPaintLayerFragments& layerFragments, GraphicsContext* context, const DeprecatedPaintLayerPaintingInfo& localPaintingInfo,
    LayoutObject* paintingRootForLayoutObject, PaintLayerFlags paintFlags)
{
    bool needsScope = layerFragments.size() > 1;
    for (auto& fragment : layerFragments) {
        Optional<ScopeRecorder> scopeRecorder;
        if (needsScope)
            scopeRecorder.emplace(*context, *m_paintLayer.layoutObject());
        paintFragmentWithPhase(PaintPhaseMask, fragment, context, fragment.backgroundRect, localPaintingInfo, PaintBehaviorNormal, paintingRootForLayoutObject, paintFlags, HasNotClipped);
    }
}

void DeprecatedPaintLayerPainter::paintChildClippingMaskForFragments(const DeprecatedPaintLayerFragments& layerFragments, GraphicsContext* context, const DeprecatedPaintLayerPaintingInfo& localPaintingInfo,
    LayoutObject* paintingRootForLayoutObject, PaintLayerFlags paintFlags)
{
    bool needsScope = layerFragments.size() > 1;
    for (auto& fragment: layerFragments) {
        Optional<ScopeRecorder> scopeRecorder;
        if (needsScope)
            scopeRecorder.emplace(*context, *m_paintLayer.layoutObject());
        paintFragmentWithPhase(PaintPhaseClippingMask, fragment, context, fragment.foregroundRect, localPaintingInfo, PaintBehaviorNormal, paintingRootForLayoutObject, paintFlags, HasNotClipped);
    }
}

void DeprecatedPaintLayerPainter::paintOverlayScrollbars(GraphicsContext* context, const LayoutRect& damageRect, PaintBehavior paintBehavior, LayoutObject* paintingRoot)
{
    if (!m_paintLayer.containsDirtyOverlayScrollbars())
        return;

    DeprecatedPaintLayerPaintingInfo paintingInfo(&m_paintLayer, LayoutRect(enclosingIntRect(damageRect)), paintBehavior, LayoutSize(), paintingRoot);
    paintLayer(context, paintingInfo, PaintLayerPaintingOverlayScrollbars);

    m_paintLayer.setContainsDirtyOverlayScrollbars(false);
}

} // namespace blink
