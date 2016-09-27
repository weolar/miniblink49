// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/SVGClipPainter.h"

#include "core/dom/ElementTraversal.h"
#include "core/layout/svg/LayoutSVGResourceClipper.h"
#include "core/layout/svg/SVGResources.h"
#include "core/layout/svg/SVGResourcesCache.h"
#include "core/paint/CompositingRecorder.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/TransformRecorder.h"
#include "platform/graphics/paint/ClipPathDisplayItem.h"
#include "platform/graphics/paint/CompositingDisplayItem.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include "platform/graphics/paint/DrawingDisplayItem.h"
#include "wtf/TemporaryChange.h"

namespace blink {

bool SVGClipPainter::applyStatefulResource(const LayoutObject& object, GraphicsContext* context, ClipperState& clipperState)
{
    ASSERT(context);

    m_clip.clearInvalidationMask();

    return applyClippingToContext(object, object.objectBoundingBox(), object.paintInvalidationRectInLocalCoordinates(), context, clipperState);
}

class SVGClipExpansionCycleHelper {
public:
    SVGClipExpansionCycleHelper(LayoutSVGResourceClipper& clip) : m_clip(clip) { clip.beginClipExpansion(); }
    ~SVGClipExpansionCycleHelper() { m_clip.endClipExpansion(); }
private:
    LayoutSVGResourceClipper& m_clip;
};

bool SVGClipPainter::applyClippingToContext(const LayoutObject& target, const FloatRect& targetBoundingBox,
    const FloatRect& paintInvalidationRect, GraphicsContext* context, ClipperState& clipperState)
{
    ASSERT(context);
    ASSERT(clipperState == ClipperNotApplied);
    ASSERT_WITH_SECURITY_IMPLICATION(!m_clip.needsLayout());

    if (paintInvalidationRect.isEmpty() || m_clip.hasCycle())
        return false;

    SVGClipExpansionCycleHelper inClipExpansionChange(m_clip);

    AffineTransform animatedLocalTransform = toSVGClipPathElement(m_clip.element())->calculateAnimatedLocalTransform();
    // When drawing a clip for non-SVG elements, the CTM does not include the zoom factor.
    // In this case, we need to apply the zoom scale explicitly - but only for clips with
    // userSpaceOnUse units (the zoom is accounted for objectBoundingBox-resolved lengths).
    if (!target.isSVG() && m_clip.clipPathUnits() == SVGUnitTypes::SVG_UNIT_TYPE_USERSPACEONUSE) {
        ASSERT(m_clip.style());
        animatedLocalTransform.scale(m_clip.style()->effectiveZoom());
    }

    // First, try to apply the clip as a clipPath.
    if (m_clip.tryPathOnlyClipping(target, context, animatedLocalTransform, targetBoundingBox)) {
        clipperState = ClipperAppliedPath;
        return true;
    }

    // Fall back to masking.
    clipperState = ClipperAppliedMask;

    // Begin compositing the clip mask.
    CompositingRecorder::beginCompositing(*context, target, SkXfermode::kSrcOver_Mode, 1, &paintInvalidationRect);
    {
        TransformRecorder recorder(*context, target, animatedLocalTransform);

        // clipPath can also be clipped by another clipPath.
        SVGResources* resources = SVGResourcesCache::cachedResourcesForLayoutObject(&m_clip);
        LayoutSVGResourceClipper* clipPathClipper = resources ? resources->clipper() : 0;
        ClipperState clipPathClipperState = ClipperNotApplied;
        if (clipPathClipper && !SVGClipPainter(*clipPathClipper).applyClippingToContext(m_clip, targetBoundingBox, paintInvalidationRect, context, clipPathClipperState)) {
            // End the clip mask's compositor.
            CompositingRecorder::endCompositing(*context, target);
            return false;
        }

        drawClipMaskContent(context, target, targetBoundingBox, paintInvalidationRect);

        if (clipPathClipper)
            SVGClipPainter(*clipPathClipper).postApplyStatefulResource(m_clip, context, clipPathClipperState);
    }

    // Masked content layer start.
    CompositingRecorder::beginCompositing(*context, target, SkXfermode::kSrcIn_Mode, 1, &paintInvalidationRect);

    return true;
}

void SVGClipPainter::postApplyStatefulResource(const LayoutObject& target, GraphicsContext* context, ClipperState& clipperState)
{
    switch (clipperState) {
    case ClipperAppliedPath:
        // Path-only clipping, no layers to restore but we need to emit an end to the clip path display item.
        if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
            if (!context->displayItemList()->displayItemConstructionIsDisabled()) {
                if (context->displayItemList()->lastDisplayItemIsNoopBegin())
                    context->displayItemList()->removeLastDisplayItem();
                else
                    context->displayItemList()->createAndAppend<EndClipPathDisplayItem>(target);
            }
        } else {
            EndClipPathDisplayItem endClipPathDisplayItem(target);
            endClipPathDisplayItem.replay(*context);
        }
        break;
    case ClipperAppliedMask:
        // Transfer content -> clip mask (SrcIn)
        CompositingRecorder::endCompositing(*context, target);

        // Transfer clip mask -> bg (SrcOver)
        CompositingRecorder::endCompositing(*context, target);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

void SVGClipPainter::drawClipMaskContent(GraphicsContext* context, const LayoutObject& layoutObject, const FloatRect& targetBoundingBox, const FloatRect& targetPaintInvalidationRect)
{
    ASSERT(context);

    AffineTransform contentTransformation;
    RefPtr<const SkPicture> clipContentPicture = m_clip.createContentPicture(contentTransformation, targetBoundingBox, context);

    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*context, layoutObject, DisplayItem::SVGClip))
        return;

    LayoutObjectDrawingRecorder drawingRecorder(*context, layoutObject, DisplayItem::SVGClip, targetPaintInvalidationRect);
    context->save();
    context->concatCTM(contentTransformation);
    context->drawPicture(clipContentPicture.get());
    context->restore();
}

}
