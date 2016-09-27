// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/ReplicaPainter.h"

#include "core/layout/LayoutReplica.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/DeprecatedPaintLayerPainter.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"

namespace blink {

void ReplicaPainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (paintInfo.phase != PaintPhaseForeground && paintInfo.phase != PaintPhaseMask)
        return;

    LayoutPoint adjustedPaintOffset = paintOffset + m_layoutReplica.location();

    if (paintInfo.phase == PaintPhaseForeground) {
        // Turn around and paint the parent layer. Use temporary clipRects, so that the layer doesn't end up caching clip rects
        // computing using the wrong rootLayer
        DeprecatedPaintLayer* rootPaintingLayer = m_layoutReplica.layer()->transform() ? m_layoutReplica.layer()->parent() : m_layoutReplica.layer()->enclosingTransformedAncestor();
        DeprecatedPaintLayerPaintingInfo paintingInfo(rootPaintingLayer, LayoutRect(paintInfo.rect), PaintBehaviorNormal, LayoutSize(), 0);
        PaintLayerFlags flags = PaintLayerHaveTransparency | PaintLayerAppliedTransform | PaintLayerUncachedClipRects | PaintLayerPaintingReflection;
        DeprecatedPaintLayerPainter(*m_layoutReplica.layer()->parent()).paintLayer(paintInfo.context, paintingInfo, flags);
    } else if (paintInfo.phase == PaintPhaseMask) {
        m_layoutReplica.paintMask(paintInfo, adjustedPaintOffset);
    }
}

} // namespace blink
