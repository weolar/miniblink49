// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/SVGFilterPainter.h"

#include "core/layout/svg/LayoutSVGResourceFilter.h"
#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/paint/CompositingRecorder.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/TransformRecorder.h"
#include "platform/graphics/filters/SkiaImageFilterBuilder.h"
#include "platform/graphics/filters/SourceGraphic.h"
#include "platform/graphics/paint/CompositingDisplayItem.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include "platform/graphics/paint/DrawingDisplayItem.h"

#define CHECK_CTM_FOR_TRANSFORMED_IMAGEFILTER

namespace blink {

GraphicsContext* SVGFilterRecordingContext::beginContent(FilterData* filterData)
{
    ASSERT(filterData->m_state == FilterData::Initial);

    GraphicsContext* context = paintingContext();

    // For slimming paint we need to create a new context so the contents of the
    // filter can be drawn and cached.
    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        m_displayItemList = DisplayItemList::create();
        m_context = adoptPtr(new GraphicsContext(m_displayItemList.get()));
        context = m_context.get();
    } else {
        context->beginRecording(filterData->filter->filterRegion());
    }

    filterData->m_state = FilterData::RecordingContent;
    return context;
}

void SVGFilterRecordingContext::endContent(FilterData* filterData)
{
    ASSERT(filterData->m_state == FilterData::RecordingContent);

    // FIXME: maybe filterData should just hold onto SourceGraphic after creation?
    SourceGraphic* sourceGraphic = static_cast<SourceGraphic*>(filterData->builder->getEffectById(SourceGraphic::effectName()));
    ASSERT(sourceGraphic);

    GraphicsContext* context = paintingContext();

    // For slimming paint we need to use the context that contains the filtered
    // content.
    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        ASSERT(m_displayItemList);
        ASSERT(m_context);
        context = m_context.get();
        context->beginRecording(filterData->filter->filterRegion());
        m_displayItemList->commitNewDisplayItemsAndReplay(*context);
    }

    sourceGraphic->setPicture(context->endRecording());

    // Content is cached by the source graphic so temporaries can be freed.
    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        m_displayItemList = nullptr;
        m_context = nullptr;
    }

    filterData->m_state = FilterData::ReadyToPaint;
}

static void paintFilteredContent(LayoutObject& object, GraphicsContext* context, FilterData* filterData)
{
    ASSERT(filterData->m_state == FilterData::ReadyToPaint);
    ASSERT(filterData->builder->getEffectById(SourceGraphic::effectName()));

    filterData->m_state = FilterData::PaintingFilter;

    SkiaImageFilterBuilder builder;
    RefPtr<SkImageFilter> imageFilter = builder.build(filterData->builder->lastEffect(), ColorSpaceDeviceRGB);
    FloatRect boundaries = filterData->filter->filterRegion();
    context->save();

    // Clip drawing of filtered image to the minimum required paint rect.
    FilterEffect* lastEffect = filterData->builder->lastEffect();
    context->clipRect(lastEffect->determineAbsolutePaintRect(lastEffect->maxEffectRect()));

#ifdef CHECK_CTM_FOR_TRANSFORMED_IMAGEFILTER
    // TODO: Remove this workaround once skew/rotation support is added in Skia
    // (https://code.google.com/p/skia/issues/detail?id=3288, crbug.com/446935).
    // If the CTM contains rotation or shearing, apply the filter to
    // the unsheared/unrotated matrix, and do the shearing/rotation
    // as a final pass.
    AffineTransform ctm = SVGLayoutSupport::deprecatedCalculateTransformToLayer(&object);
    if (ctm.b() || ctm.c()) {
        AffineTransform scaleAndTranslate;
        scaleAndTranslate.translate(ctm.e(), ctm.f());
        scaleAndTranslate.scale(ctm.xScale(), ctm.yScale());
        ASSERT(scaleAndTranslate.isInvertible());
        AffineTransform shearAndRotate = scaleAndTranslate.inverse();
        shearAndRotate.multiply(ctm);
        context->concatCTM(shearAndRotate.inverse());
        imageFilter = builder.buildTransform(shearAndRotate, imageFilter.get());
    }
#endif

    context->beginLayer(1, SkXfermode::kSrcOver_Mode, &boundaries, ColorFilterNone, imageFilter.get());
    context->endLayer();
    context->restore();

    filterData->m_state = FilterData::ReadyToPaint;
}

GraphicsContext* SVGFilterPainter::prepareEffect(LayoutObject& object, SVGFilterRecordingContext& recordingContext)
{
    ASSERT(recordingContext.paintingContext());

    m_filter.clearInvalidationMask();

    if (FilterData* filterData = m_filter.getFilterDataForLayoutObject(&object)) {
        // If the filterData already exists we do not need to record the content
        // to be filtered. This can occur if the content was previously recorded
        // or we are in a cycle.
        if (filterData->m_state == FilterData::PaintingFilter)
            filterData->m_state = FilterData::PaintingFilterCycleDetected;

        if (filterData->m_state == FilterData::RecordingContent)
            filterData->m_state = FilterData::RecordingContentCycleDetected;

        return nullptr;
    }

    OwnPtrWillBeRawPtr<FilterData> filterData = FilterData::create();
    FloatRect targetBoundingBox = object.objectBoundingBox();

    SVGFilterElement* filterElement = toSVGFilterElement(m_filter.element());
    FloatRect filterRegion = SVGLengthContext::resolveRectangle<SVGFilterElement>(filterElement, filterElement->filterUnits()->currentValue()->enumValue(), targetBoundingBox);
    if (filterRegion.isEmpty())
        return nullptr;

    // Create the SVGFilter object.
    FloatRect drawingRegion = object.strokeBoundingBox();
    drawingRegion.intersect(filterRegion);
    bool primitiveBoundingBoxMode = filterElement->primitiveUnits()->currentValue()->enumValue() == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX;
    filterData->filter = SVGFilter::create(enclosingIntRect(drawingRegion), targetBoundingBox, filterRegion, primitiveBoundingBoxMode);

    // Create all relevant filter primitives.
    filterData->builder = m_filter.buildPrimitives(filterData->filter.get());
    if (!filterData->builder)
        return nullptr;

    FilterEffect* lastEffect = filterData->builder->lastEffect();
    if (!lastEffect)
        return nullptr;

    lastEffect->determineFilterPrimitiveSubregion(ClipToFilterRegion);

    FilterData* data = filterData.get();
    m_filter.setFilterDataForLayoutObject(&object, filterData.release());
    return recordingContext.beginContent(data);
}

void SVGFilterPainter::finishEffect(LayoutObject& object, SVGFilterRecordingContext& recordingContext)
{
    FilterData* filterData = m_filter.getFilterDataForLayoutObject(&object);
    if (filterData) {
        // A painting cycle can occur when an FeImage references a source that
        // makes use of the FeImage itself. This is the first place we would hit
        // the cycle so we reset the state and continue.
        if (filterData->m_state == FilterData::PaintingFilterCycleDetected)
            filterData->m_state = FilterData::PaintingFilter;

        // Check for RecordingContent here because we may can be re-painting
        // without re-recording the contents to be filtered.
        if (filterData->m_state == FilterData::RecordingContent)
            recordingContext.endContent(filterData);

        if (filterData->m_state == FilterData::RecordingContentCycleDetected)
            filterData->m_state = FilterData::RecordingContent;
    }

    GraphicsContext* context = recordingContext.paintingContext();
    ASSERT(context);
    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*context, object, DisplayItem::SVGFilter))
        return;

    LayoutObjectDrawingRecorder recorder(*context, object, DisplayItem::SVGFilter, LayoutRect::infiniteIntRect());
    if (filterData && filterData->m_state == FilterData::ReadyToPaint)
        paintFilteredContent(object, context, filterData);
}

}
