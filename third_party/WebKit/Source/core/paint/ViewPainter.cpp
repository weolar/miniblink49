// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/ViewPainter.h"

#include "core/frame/FrameView.h"
#include "core/frame/Settings.h"
#include "core/layout/LayoutBox.h"
#include "core/layout/LayoutView.h"
#include "core/paint/BlockPainter.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

void ViewPainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    // If we ever require layout but receive a paint anyway, something has gone horribly wrong.
    ASSERT(!m_layoutView.needsLayout());
    // LayoutViews should never be called to paint with an offset not on device pixels.
    ASSERT(LayoutPoint(IntPoint(paintOffset.x(), paintOffset.y())) == paintOffset);

    m_layoutView.paintObject(paintInfo, paintOffset);
    BlockPainter(m_layoutView).paintOverflowControlsIfNeeded(paintInfo, paintOffset);
}

void ViewPainter::paintBoxDecorationBackground(const PaintInfo& paintInfo)
{
    if (!paintInfo.shouldPaintWithinRoot(&m_layoutView))
        return;

    if (paintInfo.skipRootBackground())
        return;

    // This function overrides background painting for the LayoutView.
    // View background painting is special in the following ways:
    // 1. The view paints background for the root element, the background positioning respects
    //    the positioning and transformation of the root element.
    // 2. CSS background-clip is ignored, the background layers always expand to cover the whole
    //    canvas. None of the stacking context effects (except transformation) on the root element
    //    affects the background.
    // 3. The main frame is also responsible for painting the user-agent-defined base background
    //    color. Conceptually it should be painted by the embedder but painting it here allows
    //    culling and pre-blending optimization when possible.

    GraphicsContext& context = *paintInfo.context;
    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(context, m_layoutView, DisplayItem::BoxDecorationBackground))
        return;

    IntRect documentRect = m_layoutView.unscaledDocumentRect();
    const Document& document = m_layoutView.document();
    const FrameView& frameView = *m_layoutView.frameView();
    bool isMainFrame = !document.ownerElement();
    bool paintsBaseBackground = isMainFrame && !frameView.isTransparent();
    bool shouldClearCanvas = paintsBaseBackground && (document.settings() && document.settings()->shouldClearDocumentBackground());
    Color baseBackgroundColor = paintsBaseBackground ? frameView.baseBackgroundColor() : Color();
    Color rootBackgroundColor = m_layoutView.style()->visitedDependentColor(CSSPropertyBackgroundColor);
    const LayoutObject* rootObject = document.documentElement() ? document.documentElement()->layoutObject() : nullptr;

    LayoutObjectDrawingRecorder recorder(context, m_layoutView, DisplayItem::BoxDecorationBackground, documentRect);

    // Special handling for print economy mode.
    bool forceBackgroundToWhite = BoxPainter::shouldForceWhiteBackgroundForPrintEconomy(m_layoutView.styleRef(), document);
    if (forceBackgroundToWhite) {
        // If for any reason the view background is not transparent, paint white instead, otherwise keep transparent as is.
        if (paintsBaseBackground || rootBackgroundColor.alpha() || m_layoutView.style()->backgroundLayers().image())
            context.fillRect(documentRect, Color::white, SkXfermode::kSrc_Mode);
        return;
    }

    // Compute the enclosing rect of the view, in root element space.
    //
    // For background colors we can simply paint the document rect in the default space.
    // However for background image, the root element transform applies. The strategy is to apply
    // root element transform on the context and issue draw commands in the local space, therefore
    // we need to apply inverse transform on the document rect to get to the root element space.
    bool backgroundRenderable = true;
    TransformationMatrix transform;
    IntRect paintRect = documentRect;
    if (!rootObject || !rootObject->isBox()) {
        backgroundRenderable = false;
    } else if (rootObject->hasLayer()) {
        const DeprecatedPaintLayer& rootLayer = *toLayoutBoxModelObject(rootObject)->layer();
        LayoutPoint offset;
        rootLayer.convertToLayerCoords(nullptr, offset);
        transform.translate(offset.x(), offset.y());
        transform.multiply(rootLayer.renderableTransform(paintInfo.paintBehavior));

        if (!transform.isInvertible()) {
            backgroundRenderable = false;
        } else {
            bool isClamped;
            paintRect = transform.inverse().projectQuad(FloatQuad(documentRect), &isClamped).enclosingBoundingBox();
            backgroundRenderable = !isClamped;
        }
    }

    if (!backgroundRenderable) {
        if (baseBackgroundColor.alpha())
            context.fillRect(documentRect, baseBackgroundColor, shouldClearCanvas ? SkXfermode::kSrc_Mode : SkXfermode::kSrcOver_Mode);
        else if (shouldClearCanvas)
            context.fillRect(documentRect, Color(), SkXfermode::kClear_Mode);
        return;
    }

    BoxPainter::FillLayerOcclusionOutputList reversedPaintList;
    bool shouldDrawBackgroundInSeparateBuffer = BoxPainter(m_layoutView).calculateFillLayerOcclusionCulling(reversedPaintList, m_layoutView.style()->backgroundLayers());
    ASSERT(reversedPaintList.size());

    // If the root background color is opaque, isolation group can be skipped because the canvas
    // will be cleared by root background color.
    if (!rootBackgroundColor.hasAlpha())
        shouldDrawBackgroundInSeparateBuffer = false;

    // We are going to clear the canvas with transparent pixels, isolation group can be skipped.
    if (!baseBackgroundColor.alpha() && shouldClearCanvas)
        shouldDrawBackgroundInSeparateBuffer = false;

    if (shouldDrawBackgroundInSeparateBuffer) {
        if (baseBackgroundColor.alpha())
            context.fillRect(documentRect, baseBackgroundColor, shouldClearCanvas ? SkXfermode::kSrc_Mode : SkXfermode::kSrcOver_Mode);
        context.beginLayer();
    }

    Color combinedBackgroundColor = shouldDrawBackgroundInSeparateBuffer ? rootBackgroundColor : baseBackgroundColor.blend(rootBackgroundColor);
    if (combinedBackgroundColor.alpha()) // weolar
        context.fillRect(documentRect, combinedBackgroundColor, (shouldDrawBackgroundInSeparateBuffer || shouldClearCanvas) ? SkXfermode::kSrc_Mode : SkXfermode::kSrcOver_Mode);
    else if (shouldClearCanvas && !shouldDrawBackgroundInSeparateBuffer)
        context.fillRect(documentRect, Color(), SkXfermode::kClear_Mode);

    context.save();
    // TODO(trchen): We should be able to handle 3D-transformed root
    // background with slimming paint by using transform display items.
    context.concatCTM(transform.toAffineTransform());
    for (auto it = reversedPaintList.rbegin(); it != reversedPaintList.rend(); ++it) {
        ASSERT((*it)->clip() == BorderFillBox);
        BoxPainter::paintFillLayerExtended(m_layoutView, paintInfo, Color(), **it, LayoutRect(paintRect), BackgroundBleedNone);
    }
    context.restore();

    if (shouldDrawBackgroundInSeparateBuffer)
        context.endLayer();
}

} // namespace blink
