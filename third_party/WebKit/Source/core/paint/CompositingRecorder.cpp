// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/CompositingRecorder.h"

#include "core/layout/LayoutObject.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/graphics/paint/CompositingDisplayItem.h"
#include "platform/graphics/paint/DisplayItemList.h"

namespace blink {

CompositingRecorder::CompositingRecorder(GraphicsContext& graphicsContext, const DisplayItemClientWrapper& client, const SkXfermode::Mode xferMode, const float opacity, const FloatRect* bounds, ColorFilter colorFilter)
    : m_client(client)
    , m_graphicsContext(graphicsContext)
{
    beginCompositing(graphicsContext, m_client, xferMode, opacity, bounds, colorFilter);
}

CompositingRecorder::~CompositingRecorder()
{
    endCompositing(m_graphicsContext, m_client);
}

void CompositingRecorder::beginCompositing(GraphicsContext& graphicsContext, const DisplayItemClientWrapper& client, const SkXfermode::Mode xferMode, const float opacity, const FloatRect* bounds, ColorFilter colorFilter)
{
    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        ASSERT(graphicsContext.displayItemList());
        if (graphicsContext.displayItemList()->displayItemConstructionIsDisabled())
            return;
        graphicsContext.displayItemList()->createAndAppend<BeginCompositingDisplayItem>(client, xferMode, opacity, bounds, colorFilter);
    } else {
        BeginCompositingDisplayItem compositingDisplayItem(client, xferMode, opacity, bounds, colorFilter);
        compositingDisplayItem.replay(graphicsContext);
    }
}

void CompositingRecorder::endCompositing(GraphicsContext& graphicsContext, const DisplayItemClientWrapper& client)
{
    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        ASSERT(graphicsContext.displayItemList());
        if (!graphicsContext.displayItemList()->displayItemConstructionIsDisabled()) {
            if (graphicsContext.displayItemList()->lastDisplayItemIsNoopBegin())
                graphicsContext.displayItemList()->removeLastDisplayItem();
            else
                graphicsContext.displayItemList()->createAndAppend<EndCompositingDisplayItem>(client);
        }
    } else {
        EndCompositingDisplayItem endCompositingDisplayItem(client);
        endCompositingDisplayItem.replay(graphicsContext);
    }
}

} // namespace blink
