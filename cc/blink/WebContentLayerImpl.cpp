// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/WebContentLayerImpl.h"

#include "third_party/WebKit/public/platform/WebContentLayerClient.h"
#include "third_party/WebKit/public/platform/WebFloatPoint.h"
#include "third_party/WebKit/public/platform/WebFloatRect.h"
#include "third_party/WebKit/public/platform/WebRect.h"
#include "third_party/WebKit/public/platform/WebSize.h"
#include "third_party/skia/include/utils/SkMatrix44.h"
#include "third_party/skia/include/core/SkBBHFactory.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "skia/ext/refptr.h"
#include "cc/raster/RasterResouce.h"
#include "cc/trees/LayerTreeHost.h"
#include "cc/trees/DrawProperties.h"
#include "cc/tiles/TileGrid.h"

namespace cc_blink {

WebContentLayerImpl::WebContentLayerImpl(blink::WebContentLayerClient* client)
    : m_client(client)
    //, m_recordDrawHelp(new cc::RecordDrawHelp())
{
    m_layer = new WebLayerImpl(this);
    m_layer->setDrawsContent(true);
    m_recordDrawHelp = nullptr;
}

WebContentLayerImpl::~WebContentLayerImpl()
{
//     if (m_recordDrawHelp)
//         delete m_recordDrawHelp;
    m_recordDrawHelp = nullptr;

    delete m_layer;
}

blink::WebLayer* WebContentLayerImpl::layer()
{
    return m_layer;
}

void WebContentLayerImpl::setDoubleSided(bool doubleSided)
{
    m_layer->setDoubleSided(doubleSided);
}

void WebContentLayerImpl::setDrawCheckerboardForMissingTiles(bool enable)
{
    //m_layer->setDrawCheckerboardForMissingTiles(enable);
}

void WebContentLayerImpl::updataAndPaintContents(blink::WebCanvas* canvas, const blink::IntRect& clip)
{
    if (!m_client)
        return;

    m_client->paintContents(canvas, clip);
}

void WebContentLayerImpl::paintContents(blink::WebCanvas* canvas, const blink::IntRect& clip)
{
    m_client->paintContents(canvas, blink::WebRect(0, 0, clip.width(), clip.height()));
}

void WebContentLayerImpl::recordDraw(cc::RasterTaskGroup* taskGroup)
{
//     String outString = String::format("WebContentLayerImpl::recordDraw:%d \n", m_layer->id());
//     OutputDebugStringW(outString.charactersWithNullTermination().data());

    if (!m_client || !m_layer)
        return;

    cc::LayerTreeHost* layerTreeHost = m_layer->layerTreeHost();
    if (!layerTreeHost || !layerTreeHost->needTileRender()) {
        //m_recordDrawHelp->recordDraw(m_layer, WTF::bind<SkCanvas*, const blink::IntRect&>(&WebContentLayerImpl::paintContents, this));
        return;
    }

    if (!m_layer->tileGrid())
        return;

    // 第一步：更新tile相关信息，检查哪些tile需要被裁剪掉，哪些需要被刷新出来
    blink::IntRect screenRect = blink::IntRect(blink::IntPoint(), layerTreeHost->deviceViewportSize());
    screenRect = m_layer->mapRectFromRootLayerCoordinateToCurrentLayer(screenRect);

    blink::IntSize bounds = m_layer->bounds();
    m_layer->tileGrid()->update(m_client, taskGroup, bounds, screenRect);
}

void WebContentLayerImpl::drawToCanvas(blink::WebCanvas* canvas, const blink::IntRect& clip)
{
    if (!m_layer)
        return;

    DebugBreak();

//     blink::IntSize bounds = m_layer->drawToCanvasProperties()->bounds;
//     cc::LayerTreeHost* layerTreeHost = m_layer->layerTreeHost();
//     if (layerTreeHost && !layerTreeHost->needTileRender()) {
//         //m_recordDrawHelp->drawToCanvas(m_layer, canvas, clip);
//         paintContents(canvas, blink::WebRect(0, 0, bounds.width(), bounds.height()));
//         return;
//     }
//     
//     if (!m_layer->tileGrid())
//         return;
//     m_layer->tileGrid()->drawToCanvas(m_layer, canvas, clip);
}

}  // namespace cc_blink

