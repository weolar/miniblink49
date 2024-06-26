// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_WebContentLayerImpl_h
#define mc_WebContentLayerImpl_h

#include "mc/blink/WebLayerImpl.h"
#include "third_party/WebKit/public/platform/WebContentLayer.h"

class SkBitmap;

namespace mc {
class SkBitmapRefWrap;
class RecordDrawHelp;
class TileGrid;
}

namespace blink {
class IntRect;
class FloatRect;
class WebContentLayerClient;
}

namespace mc_blink {

class WebContentLayerImpl : public blink::WebContentLayer, public WebLayerImplClient {
public:
    explicit WebContentLayerImpl(blink::WebContentLayerClient*);

    // WebContentLayer implementation.
    virtual blink::WebLayer* layer() override;
    virtual void setDoubleSided(bool double_sided) override;
    virtual void setDrawCheckerboardForMissingTiles(bool checkerboard) override;

    // WebLayerImplClient implementation.
    virtual void updataAndPaintContents(blink::WebCanvas*, const blink::IntRect& clip) override;
    virtual void recordDraw(mc::RasterTaskGroup* taskGroup) override;
    virtual void drawToCanvas(blink::WebCanvas* canvas, const blink::IntRect& clip) override;

    void paintContents(blink::WebCanvas* canvas, const blink::IntRect& clip);

    virtual WebLayerImplClient::Type type() const OVERRIDE { return ContentLayerType; }

protected:
    virtual ~WebContentLayerImpl();

    WebLayerImpl* m_layer;
    blink::WebContentLayerClient* m_client;
    bool m_drawsContent;

    mc::RecordDrawHelp* m_recordDrawHelp;

private:
    DISALLOW_COPY_AND_ASSIGN(WebContentLayerImpl);
};

}  // namespace mc_blink

#endif  // CONTENT_RENDERER_COMPOSITOR_BINDINGS_WEB_CONTENT_LAYER_IMPL_H_

