// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_COMPOSITOR_BINDINGS_WEB_CONTENT_LAYER_IMPL_H_
#define CONTENT_RENDERER_COMPOSITOR_BINDINGS_WEB_CONTENT_LAYER_IMPL_H_

#include "cc/blink/WebLayerImpl.h"
#include "third_party/WebKit/public/platform/WebContentLayer.h"

class SkBitmap;

namespace cc {
class SkBitmapRefWrap;
class RecordDrawHelp;
class TileGrid;
}

namespace blink {
class IntRect;
class FloatRect;
class WebContentLayerClient;
}

namespace cc_blink {

class WebContentLayerImpl : public blink::WebContentLayer, public WebLayerImplClient {
public:
    explicit WebContentLayerImpl(blink::WebContentLayerClient*);

    // WebContentLayer implementation.
    virtual blink::WebLayer* layer() override;
    virtual void setDoubleSided(bool double_sided) override;
    virtual void setDrawCheckerboardForMissingTiles(bool checkerboard) override;

    // WebLayerImplClient implementation.
    virtual void updataAndPaintContents(blink::WebCanvas*, const blink::IntRect& clip) override;
    virtual void recordDraw(cc::RasterTaskGroup* taskGroup) override;
    virtual void drawToCanvas(blink::WebCanvas* canvas, const blink::IntRect& clip) override;

    void paintContents(blink::WebCanvas* canvas, const blink::IntRect& clip);

    virtual WebLayerImplClient::Type type() const OVERRIDE { return ContentLayerType; }

protected:
    virtual ~WebContentLayerImpl();

    WebLayerImpl* m_layer;
    blink::WebContentLayerClient* m_client;
    bool m_drawsContent;

//     cc::SkBitmapRefWrap* m_bitmapWrap;
//     RecursiveMutex m_mutex;
//     blink::IntSize m_bounds;
    cc::RecordDrawHelp* m_recordDrawHelp;

    //DisplayListRecordingSource* m_recordingSource;

private:
    DISALLOW_COPY_AND_ASSIGN(WebContentLayerImpl);
};

}  // namespace cc_blink

#endif  // CONTENT_RENDERER_COMPOSITOR_BINDINGS_WEB_CONTENT_LAYER_IMPL_H_

