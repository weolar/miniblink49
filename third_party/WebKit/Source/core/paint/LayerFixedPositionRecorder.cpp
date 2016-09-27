// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/LayerFixedPositionRecorder.h"

#include "core/layout/LayoutBoxModelObject.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include "platform/graphics/paint/FixedPositionContainerDisplayItem.h"
#include "platform/graphics/paint/FixedPositionDisplayItem.h"

namespace blink {

LayerFixedPositionRecorder::LayerFixedPositionRecorder(GraphicsContext& graphicsContext, const LayoutBoxModelObject& layoutObject)
    : m_graphicsContext(graphicsContext)
    , m_layoutObject(layoutObject)
    , m_isFixedPosition(layoutObject.style()->position() == FixedPosition)
    , m_isFixedPositionContainer(layoutObject.canContainFixedPositionObjects())
{
    if (!RuntimeEnabledFeatures::slimmingPaintCompositorLayerizationEnabled())
        return;

    if (m_graphicsContext.displayItemList()->displayItemConstructionIsDisabled())
        return;

    if (m_isFixedPosition)
        m_graphicsContext.displayItemList()->createAndAppend<BeginFixedPositionDisplayItem>(m_layoutObject);

    // TODO(trchen): Adding a pair of display items on every transformed
    // element can be expensive. Investigate whether we can optimize out some
    // of them if applicable.
    if (m_isFixedPositionContainer)
        m_graphicsContext.displayItemList()->createAndAppend<BeginFixedPositionContainerDisplayItem>(m_layoutObject);
}

LayerFixedPositionRecorder::~LayerFixedPositionRecorder()
{
    if (!RuntimeEnabledFeatures::slimmingPaintCompositorLayerizationEnabled())
        return;

    if (m_graphicsContext.displayItemList()->displayItemConstructionIsDisabled())
        return;

    if (m_isFixedPositionContainer) {
        if (m_graphicsContext.displayItemList()->lastDisplayItemIsNoopBegin())
            m_graphicsContext.displayItemList()->removeLastDisplayItem();
        else
            m_graphicsContext.displayItemList()->createAndAppend<EndFixedPositionDisplayItem>(m_layoutObject);
    }

    if (m_isFixedPosition) {
        if (m_graphicsContext.displayItemList()->lastDisplayItemIsNoopBegin())
            m_graphicsContext.displayItemList()->removeLastDisplayItem();
        else
            m_graphicsContext.displayItemList()->createAndAppend<EndFixedPositionDisplayItem>(m_layoutObject);
    }
}

} // namespace blink
