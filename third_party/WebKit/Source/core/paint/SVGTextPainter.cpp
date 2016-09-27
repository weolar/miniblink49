// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/SVGTextPainter.h"

#include "core/layout/svg/LayoutSVGText.h"
#include "core/paint/BlockPainter.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/TransformRecorder.h"

namespace blink {

void SVGTextPainter::paint(const PaintInfo& paintInfo)
{
    if (paintInfo.phase != PaintPhaseForeground && paintInfo.phase != PaintPhaseSelection)
        return;

    PaintInfo blockInfo(paintInfo);
    blockInfo.updateCullRectForSVGTransform(m_layoutSVGText.localToParentTransform());
    TransformRecorder transformRecorder(*blockInfo.context, m_layoutSVGText, m_layoutSVGText.localToParentTransform());

    BlockPainter(m_layoutSVGText).paint(blockInfo, LayoutPoint());

    // Paint the outlines, if any
    if (paintInfo.phase == PaintPhaseForeground) {
        blockInfo.phase = PaintPhaseSelfOutline;
        BlockPainter(m_layoutSVGText).paint(blockInfo, LayoutPoint());
    }
}

} // namespace blink
