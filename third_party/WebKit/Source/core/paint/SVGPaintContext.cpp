/*
 * Copyright (C) 2007, 2008 Rob Buis <buis@kde.org>
 * Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Google, Inc.  All rights reserved.
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
 * Copyright (C) Research In Motion Limited 2009-2010. All rights reserved.
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
#include "core/paint/SVGPaintContext.h"

#include "core/frame/FrameHost.h"
#include "core/layout/svg/LayoutSVGResourceFilter.h"
#include "core/layout/svg/LayoutSVGResourceMasker.h"
#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/layout/svg/SVGResources.h"
#include "core/layout/svg/SVGResourcesCache.h"
#include "core/paint/SVGMaskPainter.h"
#include "platform/FloatConversion.h"

namespace blink {

SVGPaintContext::~SVGPaintContext()
{
    if (m_filter) {
        ASSERT(SVGResourcesCache::cachedResourcesForLayoutObject(m_object));
        ASSERT(SVGResourcesCache::cachedResourcesForLayoutObject(m_object)->filter() == m_filter);
        ASSERT(m_filterRecordingContext);
        SVGFilterPainter(*m_filter).finishEffect(*m_object, *m_filterRecordingContext);

        // Reset the paint info after the filter effect has been completed.
        // This isn't strictly required (e.g., m_paintInfo.rect is not used
        // after this).
        m_paintInfo.context = m_originalPaintInfo->context;
        m_paintInfo.rect = m_originalPaintInfo->rect;
    }

    if (m_masker) {
        ASSERT(SVGResourcesCache::cachedResourcesForLayoutObject(m_object));
        ASSERT(SVGResourcesCache::cachedResourcesForLayoutObject(m_object)->masker() == m_masker);
        SVGMaskPainter(*m_masker).finishEffect(*m_object, m_paintInfo.context);
    }

    if (m_clipper) {
        ASSERT(SVGResourcesCache::cachedResourcesForLayoutObject(m_object));
        ASSERT(SVGResourcesCache::cachedResourcesForLayoutObject(m_object)->clipper() == m_clipper);
        SVGClipPainter(*m_clipper).postApplyStatefulResource(*m_object, m_paintInfo.context, m_clipperState);
    }
}

bool SVGPaintContext::applyClipMaskAndFilterIfNecessary()
{
#if ENABLE(ASSERT)
    ASSERT(!m_applyClipMaskAndFilterIfNecessaryCalled);
    m_applyClipMaskAndFilterIfNecessaryCalled = true;
#endif

    SVGResources* resources = SVGResourcesCache::cachedResourcesForLayoutObject(m_object);

    // When rendering clip paths as masks, only geometric operations should be included so skip
    // non-geometric operations such as compositing, masking, and filtering.
    if (m_paintInfo.isRenderingClipPathAsMaskImage()) {
        if (!applyClipIfNecessary(resources))
            return false;
        return true;
    }

    applyCompositingIfNecessary();

    if (!applyClipIfNecessary(resources))
        return false;

    if (!applyMaskIfNecessary(resources))
        return false;

    if (!applyFilterIfNecessary(resources))
        return false;

    if (!isIsolationInstalled() && SVGLayoutSupport::isIsolationRequired(m_object))
        m_compositingRecorder = adoptPtr(new CompositingRecorder(*m_paintInfo.context, *m_object, SkXfermode::kSrcOver_Mode, 1));

    return true;
}

void SVGPaintContext::applyCompositingIfNecessary()
{
    ASSERT(!m_paintInfo.isRenderingClipPathAsMaskImage());

    // Layer takes care of root opacity and blend mode.
    if (m_object->isSVGRoot())
        return;

    const ComputedStyle& style = m_object->styleRef();
    float opacity = style.opacity();
    WebBlendMode blendMode = style.hasBlendMode() && m_object->isBlendingAllowed() ?
        style.blendMode() : WebBlendModeNormal;
    if (opacity < 1 || blendMode != WebBlendModeNormal) {
        m_clipRecorder = adoptPtr(new FloatClipRecorder(*m_paintInfo.context, *m_object, m_paintInfo.phase, m_object->paintInvalidationRectInLocalCoordinates()));
        m_compositingRecorder = adoptPtr(new CompositingRecorder(*m_paintInfo.context, *m_object,
            WebCoreCompositeToSkiaComposite(CompositeSourceOver, blendMode), opacity));
    }
}

bool SVGPaintContext::applyClipIfNecessary(SVGResources* resources)
{
    // resources->clipper() corresponds to the non-prefixed 'clip-path' whereas
    // m_object->style()->clipPath() corresponds to '-webkit-clip-path'.
    // FIXME: We should unify the clip-path and -webkit-clip-path codepaths.
    if (LayoutSVGResourceClipper* clipper = resources ? resources->clipper() : nullptr) {
        if (!SVGClipPainter(*clipper).applyStatefulResource(*m_object, m_paintInfo.context, m_clipperState))
            return false;
        m_clipper = clipper;
    } else {
        ClipPathOperation* clipPathOperation = m_object->style()->clipPath();
        if (clipPathOperation && clipPathOperation->type() == ClipPathOperation::SHAPE) {
            ShapeClipPathOperation* clipPath = toShapeClipPathOperation(clipPathOperation);
            if (!clipPath->isValid())
                return false;
            m_clipPathRecorder = adoptPtr(new ClipPathRecorder(*m_paintInfo.context, *m_object, clipPath->path(m_object->objectBoundingBox())));
        }
    }
    return true;
}

bool SVGPaintContext::applyMaskIfNecessary(SVGResources* resources)
{
    if (LayoutSVGResourceMasker* masker = resources ? resources->masker() : nullptr) {
        if (!SVGMaskPainter(*masker).prepareEffect(*m_object, m_paintInfo.context))
            return false;
        m_masker = masker;
    }
    return true;
}

bool SVGPaintContext::applyFilterIfNecessary(SVGResources* resources)
{
    if (!resources) {
        if (m_object->style()->svgStyle().hasFilter())
            return false;
    } else if (LayoutSVGResourceFilter* filter = resources->filter()) {
        m_filterRecordingContext = adoptPtr(new SVGFilterRecordingContext(m_paintInfo.context));
        m_filter = filter;
        GraphicsContext* filterContext = SVGFilterPainter(*filter).prepareEffect(*m_object, *m_filterRecordingContext);
        if (!filterContext)
            return false;

        // Because the filter needs to cache its contents we replace the context
        // during filtering with the filter's context.
        m_paintInfo.context = filterContext;

        // Because we cache the filter contents and do not invalidate on paint
        // invalidation rect changes, we need to paint the entire filter region
        // so elements outside the initial paint (due to scrolling, etc) paint.
        m_paintInfo.rect = LayoutRect::infiniteIntRect();
    }
    return true;
}

bool SVGPaintContext::isIsolationInstalled() const
{
    if (m_compositingRecorder)
        return true;
    if (m_masker || m_filter)
        return true;
    if (m_clipper && m_clipperState == SVGClipPainter::ClipperAppliedMask)
        return true;
    return false;
}

void SVGPaintContext::paintSubtree(GraphicsContext* context, LayoutObject* item)
{
    ASSERT(context);
    ASSERT(item);
    ASSERT(!item->needsLayout());

    PaintInfo info(context, LayoutRect::infiniteIntRect(), PaintPhaseForeground, PaintBehaviorNormal);
    item->paint(info, IntPoint());
}

bool SVGPaintContext::paintForLayoutObject(const PaintInfo& paintInfo, const ComputedStyle& style, LayoutObject& layoutObject, LayoutSVGResourceMode resourceMode, SkPaint& paint, const AffineTransform* additionalPaintServerTransform)
{
    if (paintInfo.isRenderingClipPathAsMaskImage()) {
        if (resourceMode == ApplyToStrokeMode)
            return false;
        paint.setColor(SVGComputedStyle::initialFillPaintColor().rgb());
        paint.setShader(nullptr);
        return true;
    }

    SVGPaintServer paintServer = SVGPaintServer::requestForLayoutObject(layoutObject, style, resourceMode);
    if (!paintServer.isValid())
        return false;

    if (additionalPaintServerTransform && paintServer.isTransformDependent())
        paintServer.prependTransform(*additionalPaintServerTransform);

    const SVGComputedStyle& svgStyle = style.svgStyle();
    float paintAlpha = resourceMode == ApplyToFillMode ? svgStyle.fillOpacity() : svgStyle.strokeOpacity();
    paintServer.applyToSkPaint(paint, paintAlpha);

    paint.setFilterQuality(WebCoreInterpolationQualityToSkFilterQuality(InterpolationDefault));

    // TODO(fs): The color filter can set when generating a picture for a mask -
    // due to color-interpolation. We could also just apply the
    // color-interpolation property from the the shape itself (which could mean
    // the paintserver if it has it specified), since that would be more in line
    // with the spec for color-interpolation. For now, just steal it from the GC
    // though.
    // Additionally, it's not really safe/guaranteed to be correct, as
    // something down the paint pipe may want to farther tweak the color
    // filter, which could yield incorrect results. (Consider just using
    // saveLayer() w/ this color filter explicitly instead.)
    paint.setColorFilter(paintInfo.context->colorFilter());
    return true;
}

} // namespace blink
