// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_WebCompositorSupportImpl_h
#define mc_WebCompositorSupportImpl_h

#include "third_party/WebKit/public/platform/WebCompositorSupport.h"
#include "third_party/WebKit/public/platform/WebLayer.h"
#include "third_party/WebKit/public/platform/WebTransformOperations.h"

namespace blink {
class WebGraphicsContext3D;
}

namespace mc_blink {

class WebCompositorSupportImpl : public NON_EXPORTED_BASE(blink::WebCompositorSupport) {
public:
    WebCompositorSupportImpl();
    ~WebCompositorSupportImpl();

    virtual blink::WebLayer* createLayer() OVERRIDE;
    virtual blink::WebContentLayer* createContentLayer(blink::WebContentLayerClient* client) OVERRIDE;
    virtual blink::WebScrollbarLayer* createScrollbarLayer(blink::WebScrollbar*, blink::WebScrollbarThemePainter, blink::WebScrollbarThemeGeometry*) OVERRIDE;
    virtual blink::WebScrollbarLayer* createSolidColorScrollbarLayer(blink::WebScrollbar::Orientation, int thumbThickness, int trackStart, bool isLeftSideVerticalScrollbar) OVERRIDE;
    virtual blink::WebImageLayer* createImageLayer() OVERRIDE;

    blink::WebCompositorAnimation* createAnimation(
        const blink::WebCompositorAnimationCurve& curve, 
        blink::WebCompositorAnimation::TargetProperty target,
        int group_id, int animation_id
        ) override;
    blink::WebFilterAnimationCurve* createFilterAnimationCurve() override;
    blink::WebFloatAnimationCurve* createFloatAnimationCurve() override;
    blink::WebScrollOffsetAnimationCurve* createScrollOffsetAnimationCurve(
        blink::WebFloatPoint target_value,
        blink::WebCompositorAnimationCurve::TimingFunctionType timing_function) override;
    blink::WebTransformAnimationCurve* createTransformAnimationCurve() override;
    blink::WebTransformOperations* createTransformOperations() override;
    blink::WebFilterOperations* createFilterOperations() override;

    blink::WebCompositorAnimationPlayer* createAnimationPlayer() override;
    blink::WebCompositorAnimationTimeline* createAnimationTimeline() override;
};

} // mc_blink

#endif // CONTENT_RENDERER_COMPOSITOR_BINDINGS_WEB_COMPOSITOR_SUPPORT_IMPL_H_