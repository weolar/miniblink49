/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebCompositorSupport_h
#define WebCompositorSupport_h

#include "WebCommon.h"
#include "WebCompositorAnimation.h"
#include "WebCompositorAnimationCurve.h"
#include "WebFloatPoint.h"
#include "WebLayerTreeView.h"
#include "WebScrollbar.h"
#include "WebScrollbarThemePainter.h"

namespace blink {

class WebCompositorAnimationCurve;
class WebCompositorAnimationPlayer;
class WebCompositorAnimationTimeline;
class WebCompositorOutputSurface;
class WebContentLayer;
class WebContentLayerClient;
class WebDisplayItemList;
class WebExternalTextureLayer;
class WebExternalTextureLayerClient;
class WebFilterAnimationCurve;
class WebFilterOperations;
class WebFloatAnimationCurve;
class WebGraphicsContext3D;
class WebImageLayer;
class WebNinePatchLayer;
class WebLayer;
class WebScrollbarLayer;
class WebScrollbarThemeGeometry;
class WebScrollOffsetAnimationCurve;
class WebTransformAnimationCurve;
class WebTransformOperations;

class WebCompositorSupport {
public:
    // Creates an output surface for the compositor backed by a 3d context.
    virtual WebCompositorOutputSurface* createOutputSurfaceFor3D(WebGraphicsContext3D*) { return nullptr; }

    // Creates an output surface for the compositor backed by a software device.
    virtual WebCompositorOutputSurface* createOutputSurfaceForSoftware() { return nullptr; }

    // Layers -------------------------------------------------------

    virtual WebLayer* createLayer() { return nullptr; }

    virtual WebContentLayer* createContentLayer(WebContentLayerClient*) { return nullptr; }

    virtual WebExternalTextureLayer* createExternalTextureLayer(WebExternalTextureLayerClient*) { return nullptr; }

    virtual WebImageLayer* createImageLayer() { return nullptr; }

    virtual WebNinePatchLayer* createNinePatchLayer() { return nullptr; }

    // The ownership of the WebScrollbarThemeGeometry pointer is passed to Chromium.
    virtual WebScrollbarLayer* createScrollbarLayer(WebScrollbar*, WebScrollbarThemePainter, WebScrollbarThemeGeometry*) { return nullptr; }

    virtual WebScrollbarLayer* createSolidColorScrollbarLayer(WebScrollbar::Orientation, int thumbThickness, int trackStart, bool isLeftSideVerticalScrollbar) { return nullptr; }

    // Animation ----------------------------------------------------

    virtual WebCompositorAnimation* createAnimation(const WebCompositorAnimationCurve&, WebCompositorAnimation::TargetProperty, int groupId = 0, int animationId = 0) { return nullptr; }

    virtual WebFilterAnimationCurve* createFilterAnimationCurve() { return nullptr; }

    virtual WebFloatAnimationCurve* createFloatAnimationCurve() { return nullptr; }

    virtual WebScrollOffsetAnimationCurve* createScrollOffsetAnimationCurve(WebFloatPoint targetValue, WebCompositorAnimationCurve::TimingFunctionType) { return nullptr; }

    virtual WebTransformAnimationCurve* createTransformAnimationCurve() { return nullptr; }

    virtual WebTransformOperations* createTransformOperations() { return nullptr; }

    virtual WebFilterOperations* createFilterOperations() { return nullptr; }

    virtual WebCompositorAnimationPlayer* createAnimationPlayer() { return nullptr; }

    virtual WebCompositorAnimationTimeline* createAnimationTimeline() { return nullptr; }

    virtual WebDisplayItemList* createDisplayItemList() { return nullptr; }

protected:
    virtual ~WebCompositorSupport() { }
};

}

#endif // WebCompositorSupport_h
