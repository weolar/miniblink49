// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_COMPOSITOR_BINDINGS_WEB_COMPOSITOR_SUPPORT_IMPL_H_
#define CONTENT_RENDERER_COMPOSITOR_BINDINGS_WEB_COMPOSITOR_SUPPORT_IMPL_H_

//#include "third_party/WebKit/public/platform/WebAnimationCurve.h"
#include "third_party/WebKit/public/platform/WebCompositorSupport.h"
#include "third_party/WebKit/public/platform/WebLayer.h"
#include "third_party/WebKit/public/platform/WebTransformOperations.h"

namespace blink {
    class WebGraphicsContext3D;
}

namespace cc_blink {

class WebCompositorSupportImpl : public NON_EXPORTED_BASE(blink::WebCompositorSupport) {
public:
    WebCompositorSupportImpl();
    ~WebCompositorSupportImpl();

    virtual blink::WebLayer* createLayer() OVERRIDE;
    virtual blink::WebContentLayer* createContentLayer(blink::WebContentLayerClient* client) OVERRIDE;
    virtual blink::WebScrollbarLayer* createScrollbarLayer(blink::WebScrollbar*, blink::WebScrollbarThemePainter, blink::WebScrollbarThemeGeometry*) OVERRIDE;
    virtual blink::WebScrollbarLayer* createSolidColorScrollbarLayer(blink::WebScrollbar::Orientation, int thumbThickness, int trackStart, bool isLeftSideVerticalScrollbar) OVERRIDE;
    virtual blink::WebFilterOperations* createFilterOperations() OVERRIDE;
    virtual blink::WebImageLayer* createImageLayer() OVERRIDE;
};

} // cc_blink

#endif // CONTENT_RENDERER_COMPOSITOR_BINDINGS_WEB_COMPOSITOR_SUPPORT_IMPL_H_