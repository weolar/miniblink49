/*
 * Copyright (C) 2004, 2006, 2007 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/LayoutHTMLCanvas.h"

#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLCanvasElement.h"
#include "core/layout/LayoutView.h"
#include "core/page/Page.h"
#include "core/paint/HTMLCanvasPainter.h"

namespace blink {

using namespace HTMLNames;

LayoutHTMLCanvas::LayoutHTMLCanvas(HTMLCanvasElement* element)
    : LayoutReplaced(element, LayoutSize(element->size()))
{
    view()->frameView()->setIsVisuallyNonEmpty();
}

DeprecatedPaintLayerType LayoutHTMLCanvas::layerTypeRequired() const
{
    return NormalDeprecatedPaintLayer;
}

void LayoutHTMLCanvas::paintReplaced(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    HTMLCanvasPainter(*this).paintReplaced(paintInfo, paintOffset);
}

void LayoutHTMLCanvas::canvasSizeChanged()
{
    IntSize canvasSize = toHTMLCanvasElement(node())->size();
    LayoutSize zoomedSize(canvasSize.width() * style()->effectiveZoom(), canvasSize.height() * style()->effectiveZoom());

    if (zoomedSize == intrinsicSize())
        return;

    setIntrinsicSize(zoomedSize);

    if (!parent())
        return;

    if (!preferredLogicalWidthsDirty())
        setPreferredLogicalWidthsDirty();

    LayoutSize oldSize = size();
    updateLogicalWidth();
    updateLogicalHeight();
    if (oldSize == size())
        return;

    if (!selfNeedsLayout())
        setNeedsLayout(LayoutInvalidationReason::SizeChanged);
}

PaintInvalidationReason LayoutHTMLCanvas::invalidatePaintIfNeeded(PaintInvalidationState& paintInvalidationState, const LayoutBoxModelObject& paintInvalidationContainer)
{
    PaintInvalidationReason reason = LayoutBox::invalidatePaintIfNeeded(paintInvalidationState, paintInvalidationContainer);
    HTMLCanvasElement* element = toHTMLCanvasElement(node());
    if (element->isDirty()) {
        element->doDeferredPaintInvalidation();
        if (reason < PaintInvalidationRectangle)
            reason = PaintInvalidationRectangle;
    }
    return reason;
}

CompositingReasons LayoutHTMLCanvas::additionalCompositingReasons() const
{
    if (toHTMLCanvasElement(node())->shouldBeDirectComposited())
        return CompositingReasonCanvas;
    return CompositingReasonNone;
}

} // namespace blink
