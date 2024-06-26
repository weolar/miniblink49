// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "mc/blink/WebCompositorSupportImpl.h"
#include "mc/blink/WebLayerImpl.h"
#include "mc/blink/WebContentLayerImpl.h"
#include "mc/blink/WebScrollbarLayerImpl.h"
#include "mc/blink/WebImageLayerImpl.h"

#include "mc/blink/WebAnimationImpl.h"
#include "mc/blink/WebFilterAnimationCurveImpl.h"
#include "mc/blink/WebFloatAnimationCurveImpl.h"
#include "mc/blink/WebScrollOffsetAnimationCurveImpl.h"
#include "mc/blink/WebTransformAnimationCurveImpl.h"
#include "mc/blink/WebTransformOperationsImpl.h"
#include "mc/blink/WebFilterOperationsImpl.h"
#include "mc/blink/WebCompositorAnimationPlayerImpl.h"
#include "mc/blink/WebCompositorAnimationTimelineImpl.h"

namespace mc_blink {

WebCompositorSupportImpl::WebCompositorSupportImpl() { }

WebCompositorSupportImpl::~WebCompositorSupportImpl() { }

blink::WebLayer* WebCompositorSupportImpl::createLayer() 
{
    return new WebLayerImpl(nullptr);
}

blink::WebContentLayer* WebCompositorSupportImpl::createContentLayer(blink::WebContentLayerClient* client)
{
    return new WebContentLayerImpl(client);
}

blink::WebScrollbarLayer* WebCompositorSupportImpl::createScrollbarLayer(blink::WebScrollbar* scrollbar, 
    blink::WebScrollbarThemePainter painter, blink::WebScrollbarThemeGeometry* themeGeometry)
{
    return new WebScrollbarLayerImpl(scrollbar, painter, themeGeometry);
}

blink::WebScrollbarLayer* WebCompositorSupportImpl::createSolidColorScrollbarLayer(blink::WebScrollbar::Orientation orientation,
    int thumbThickness, int trackStart, bool isLeftSideVerticalScrollbar)
{
    return new WebScrollbarLayerImpl(orientation, thumbThickness, trackStart, isLeftSideVerticalScrollbar);
}

blink::WebImageLayer* WebCompositorSupportImpl::createImageLayer()
{
    return new WebImageLayerImpl();
}

blink::WebCompositorAnimation* WebCompositorSupportImpl::createAnimation(
    const blink::WebCompositorAnimationCurve& curve,
    blink::WebCompositorAnimation::TargetProperty target,
    int group_id,
    int animation_id)
{
    return new WebCompositorAnimationImpl(curve, target, animation_id, group_id);
}

blink::WebFilterAnimationCurve* WebCompositorSupportImpl::createFilterAnimationCurve()
{
    return new WebFilterAnimationCurveImpl();
}

blink::WebFloatAnimationCurve* WebCompositorSupportImpl::createFloatAnimationCurve()
{
    return new WebFloatAnimationCurveImpl();
}

blink::WebScrollOffsetAnimationCurve* WebCompositorSupportImpl::createScrollOffsetAnimationCurve(
    blink::WebFloatPoint target_value,
    blink::WebCompositorAnimationCurve::TimingFunctionType timing_function)
{
    return new WebScrollOffsetAnimationCurveImpl(target_value, timing_function);
}

blink::WebTransformAnimationCurve* WebCompositorSupportImpl::createTransformAnimationCurve()
{
    return new WebTransformAnimationCurveImpl();
}

blink::WebTransformOperations* WebCompositorSupportImpl::createTransformOperations()
{
    return new WebTransformOperationsImpl();
}

blink::WebFilterOperations* WebCompositorSupportImpl::createFilterOperations()
{
    return new WebFilterOperationsImpl();
}

blink::WebCompositorAnimationPlayer* WebCompositorSupportImpl::createAnimationPlayer()
{
    return new WebCompositorAnimationPlayerImpl();
}

blink::WebCompositorAnimationTimeline* WebCompositorSupportImpl::createAnimationTimeline()
{
    return new WebCompositorAnimationTimelineImpl();
}

} // mc_blink