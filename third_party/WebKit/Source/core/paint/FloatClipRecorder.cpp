// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/FloatClipRecorder.h"

#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include "platform/graphics/paint/FloatClipDisplayItem.h"

namespace blink {

FloatClipRecorder::FloatClipRecorder(GraphicsContext& context, const DisplayItemClientWrapper& client, PaintPhase paintPhase, const FloatRect& clipRect)
    : m_context(context)
    , m_client(client)
    , m_clipType(DisplayItem::paintPhaseToFloatClipType(paintPhase))
{
    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        ASSERT(m_context.displayItemList());
        if (m_context.displayItemList()->displayItemConstructionIsDisabled())
            return;
        m_context.displayItemList()->createAndAppend<FloatClipDisplayItem>(m_client, m_clipType, clipRect);
    } else {
        FloatClipDisplayItem floatClipDisplayItem(m_client, m_clipType, clipRect);
        floatClipDisplayItem.replay(m_context);
    }
}

FloatClipRecorder::~FloatClipRecorder()
{
    DisplayItem::Type endType = DisplayItem::floatClipTypeToEndFloatClipType(m_clipType);
    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        ASSERT(m_context.displayItemList());
        if (!m_context.displayItemList()->displayItemConstructionIsDisabled()) {
            if (m_context.displayItemList()->lastDisplayItemIsNoopBegin())
                m_context.displayItemList()->removeLastDisplayItem();
            else
                m_context.displayItemList()->createAndAppend<EndFloatClipDisplayItem>(m_client, endType);
        }
    } else {
        EndFloatClipDisplayItem endClipDisplayItem(m_client, endType);
        endClipDisplayItem.replay(m_context);
    }
}

} // namespace blink
