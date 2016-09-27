// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/BoxClipper.h"

#include "core/layout/LayoutBox.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/PaintInfo.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/graphics/paint/ClipDisplayItem.h"
#include "platform/graphics/paint/DisplayItemList.h"

namespace blink {

BoxClipper::BoxClipper(const LayoutBox& box, const PaintInfo& paintInfo, const LayoutPoint& accumulatedOffset, ContentsClipBehavior contentsClipBehavior)
    : m_box(box)
    , m_paintInfo(paintInfo)
    , m_clipType(DisplayItem::UninitializedType)
{
    if (m_paintInfo.phase == PaintPhaseBlockBackground || m_paintInfo.phase == PaintPhaseSelfOutline || m_paintInfo.phase == PaintPhaseMask)
        return;

    bool isControlClip = m_box.hasControlClip();
    bool isOverflowClip = m_box.hasOverflowClip() && !m_box.layer()->isSelfPaintingLayer();

    if (!isControlClip && !isOverflowClip)
        return;

    LayoutRect clipRect = isControlClip ? m_box.controlClipRect(accumulatedOffset) : m_box.overflowClipRect(accumulatedOffset);
    FloatRoundedRect clipRoundedRect(0, 0, 0, 0);
    bool hasBorderRadius = m_box.style()->hasBorderRadius();
    if (hasBorderRadius)
        clipRoundedRect = m_box.style()->getRoundedInnerBorderFor(LayoutRect(accumulatedOffset, m_box.size()));

    if (contentsClipBehavior == SkipContentsClipIfPossible) {
        LayoutRect contentsVisualOverflow = m_box.contentsVisualOverflowRect();
        if (contentsVisualOverflow.isEmpty())
            return;

        LayoutRect conservativeClipRect = clipRect;
        if (hasBorderRadius)
            conservativeClipRect.intersect(LayoutRect(clipRoundedRect.radiusCenterRect()));
        conservativeClipRect.moveBy(-accumulatedOffset);
        if (m_box.hasLayer())
            conservativeClipRect.move(m_box.scrolledContentOffset());
        if (conservativeClipRect.contains(contentsVisualOverflow))
            return;
    }

    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        ASSERT(m_paintInfo.context->displayItemList());
        if (!m_paintInfo.context->displayItemList()->displayItemConstructionIsDisabled()) {
            m_clipType = m_paintInfo.displayItemTypeForClipping();
            Vector<FloatRoundedRect> roundedRects;
            if (hasBorderRadius)
                roundedRects.append(clipRoundedRect);
            m_paintInfo.context->displayItemList()->createAndAppend<ClipDisplayItem>(m_box, m_clipType, pixelSnappedIntRect(clipRect), roundedRects);
        }
    } else {
        m_clipType = m_paintInfo.displayItemTypeForClipping();
        Vector<FloatRoundedRect> roundedRects;
        if (hasBorderRadius)
            roundedRects.append(clipRoundedRect);
        ClipDisplayItem clipDisplayItem(m_box, m_clipType, pixelSnappedIntRect(clipRect), roundedRects);
        clipDisplayItem.replay(*paintInfo.context);
    }
}

BoxClipper::~BoxClipper()
{
    if (m_clipType == DisplayItem::UninitializedType)
        return;

    ASSERT(m_box.hasControlClip() || (m_box.hasOverflowClip() && !m_box.layer()->isSelfPaintingLayer()));

    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        ASSERT(m_paintInfo.context->displayItemList());
        if (!m_paintInfo.context->displayItemList()->displayItemConstructionIsDisabled()) {
            if (m_paintInfo.context->displayItemList()->lastDisplayItemIsNoopBegin())
                m_paintInfo.context->displayItemList()->removeLastDisplayItem();
            else
                m_paintInfo.context->displayItemList()->createAndAppend<EndClipDisplayItem>(m_box, DisplayItem::clipTypeToEndClipType(m_clipType));
        }
    } else {
        EndClipDisplayItem endClipDisplayItem(m_box, DisplayItem::clipTypeToEndClipType(m_clipType));
        endClipDisplayItem.replay(*m_paintInfo.context);
    }
}

} // namespace blink
