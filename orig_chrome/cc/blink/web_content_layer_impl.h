// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_WEB_CONTENT_LAYER_IMPL_H_
#define CC_BLINK_WEB_CONTENT_LAYER_IMPL_H_

#include "base/memory/scoped_ptr.h"
#include "cc/blink/cc_blink_export.h"
#include "cc/blink/web_layer_impl.h"
#include "cc/layers/content_layer_client.h"
#include "third_party/WebKit/public/platform/WebContentLayer.h"

namespace cc {
class IntRect;
class FloatRect;
}

namespace blink {
class WebContentLayerClient;
}

namespace cc_blink {

class WebContentLayerImpl : public blink::WebContentLayer,
                            public cc::ContentLayerClient {
public:
    CC_BLINK_EXPORT explicit WebContentLayerImpl(blink::WebContentLayerClient*);

    // WebContentLayer implementation.
    blink::WebLayer* layer() override;
    void setDoubleSided(bool double_sided) override;
    void setDrawCheckerboardForMissingTiles(bool) override;

protected:
    ~WebContentLayerImpl() override;

    // ContentLayerClient implementation.
    void PaintContents(SkCanvas* canvas,
        const gfx::Rect& clip,
        PaintingControlSetting painting_control) override;
    scoped_refptr<cc::DisplayItemList> PaintContentsToDisplayList(
        const gfx::Rect& clip,
        PaintingControlSetting painting_control) override;
    bool FillsBoundsCompletely() const override;
    size_t GetApproximateUnsharedMemoryUsage() const override;

    scoped_ptr<WebLayerImpl> layer_;
    blink::WebContentLayerClient* client_;
    bool draws_content_;

private:
    DISALLOW_COPY_AND_ASSIGN(WebContentLayerImpl);
};

} // namespace cc_blink

#endif // CC_BLINK_WEB_CONTENT_LAYER_IMPL_H_
