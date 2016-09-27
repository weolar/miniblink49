// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/FilterPainter.h"

#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/FilterEffectBuilder.h"
#include "core/paint/LayerClipRecorder.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/graphics/filters/FilterEffect.h"
#include "platform/graphics/filters/SkiaImageFilterBuilder.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include "platform/graphics/paint/FilterDisplayItem.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCompositorSupport.h"
#include "public/platform/WebFilterOperations.h"

namespace blink {

FilterPainter::FilterPainter(DeprecatedPaintLayer& layer, GraphicsContext* context, const LayoutPoint& offsetFromRoot, const ClipRect& clipRect, DeprecatedPaintLayerPaintingInfo& paintingInfo, PaintLayerFlags paintFlags,
    LayoutRect& rootRelativeBounds, bool& rootRelativeBoundsComputed)
    : m_filterInProgress(false)
    , m_context(context)
    , m_layoutObject(layer.layoutObject())
{
    if (!layer.filterEffectBuilder() || !layer.paintsWithFilters())
        return;

    ASSERT(layer.filterInfo());

    SkiaImageFilterBuilder builder;
    RefPtrWillBeRawPtr<FilterEffect> lastEffect = layer.filterEffectBuilder()->lastEffect();
    lastEffect->determineFilterPrimitiveSubregion(MapRectForward);
    RefPtr<SkImageFilter> imageFilter = builder.build(lastEffect.get(), ColorSpaceDeviceRGB);
    if (!imageFilter)
        return;

    if (!rootRelativeBoundsComputed) {
        rootRelativeBounds = layer.physicalBoundingBoxIncludingReflectionAndStackingChildren(paintingInfo.rootLayer, offsetFromRoot);
        rootRelativeBoundsComputed = true;
    }

    // We'll handle clipping to the dirty rect before filter rasterization.
    // Filter processing will automatically expand the clip rect and the offscreen to accommodate any filter outsets.
    // FIXME: It is incorrect to just clip to the damageRect here once multiple fragments are involved.

    // Subsequent code should not clip to the dirty rect, since we've already
    // done it above, and doing it later will defeat the outsets.
    paintingInfo.clipToDirtyRect = false;

    if (clipRect.rect() != paintingInfo.paintDirtyRect || clipRect.hasRadius()) {
        m_clipRecorder = adoptPtr(new LayerClipRecorder(*context, *layer.layoutObject(), DisplayItem::ClipLayerFilter, clipRect, &paintingInfo, LayoutPoint(), paintFlags));
    }

    ASSERT(m_layoutObject);
    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        ASSERT(context->displayItemList());
        if (!context->displayItemList()->displayItemConstructionIsDisabled()) {
            FilterOperations filterOperations(layer.computeFilterOperations(m_layoutObject->styleRef()));
            OwnPtr<WebFilterOperations> webFilterOperations = adoptPtr(Platform::current()->compositorSupport()->createFilterOperations());
            builder.buildFilterOperations(filterOperations, webFilterOperations.get());
            // FIXME: It's possible to have empty WebFilterOperations here even
            // though the SkImageFilter produced above is non-null, since the
            // layer's FilterEffectBuilder can have a stale representation of
            // the layer's filter. See crbug.com/502026.
            if (webFilterOperations->isEmpty())
                return;
            context->displayItemList()->createAndAppend<BeginFilterDisplayItem>(*m_layoutObject, imageFilter, rootRelativeBounds, webFilterOperations.release());
        }
    } else {
        BeginFilterDisplayItem filterDisplayItem(*m_layoutObject, imageFilter, rootRelativeBounds);
        filterDisplayItem.replay(*context);
    }

    m_filterInProgress = true;
}

FilterPainter::~FilterPainter()
{
    if (!m_filterInProgress)
        return;

    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        ASSERT(m_context->displayItemList());
        if (!m_context->displayItemList()->displayItemConstructionIsDisabled()) {
            if (m_context->displayItemList()->lastDisplayItemIsNoopBegin())
                m_context->displayItemList()->removeLastDisplayItem();
            else
                m_context->displayItemList()->createAndAppend<EndFilterDisplayItem>(*m_layoutObject);
        }
    } else {
        EndFilterDisplayItem endFilterDisplayItem(*m_layoutObject);
        endFilterDisplayItem.replay(*m_context);
    }
}

} // namespace blink
