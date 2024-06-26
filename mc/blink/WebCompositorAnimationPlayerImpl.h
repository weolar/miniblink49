// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_blink_WebCompositorAnimationPlayerImpl_h
#define mc_blink_WebCompositorAnimationPlayerImpl_h

#include "third_party/WebKit/public/platform/WebCompositorAnimationPlayer.h"

#include "mc/base/ScopedPtrVector.h"
#include "third_party/WebKit/Source/wtf/RefPtr.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"

namespace mc {
class AnimationPlayer;
class AnimationObj;
class LayerTreeHost;
}

namespace mc_blink {

class WebToCCAnimationDelegateAdapter;

class WebCompositorAnimationPlayerImpl : public blink::WebCompositorAnimationPlayer {
public:
    WebCompositorAnimationPlayerImpl();
    ~WebCompositorAnimationPlayerImpl() override;

    void setHost(mc::LayerTreeHost* host);
    void bindElementAnimations();

    // blink::WebCompositorAnimationPlayer implementation
    void setAnimationDelegate(blink::WebCompositorAnimationDelegate* delegate) override;
    void attachLayer(blink::WebLayer* webLayer) override;
    void detachLayer() override;
    bool isLayerAttached() const override;
    void addAnimation(blink::WebCompositorAnimation* animation) override;
    void removeAnimation(int animationId) override;
    void pauseAnimation(int animationId, double timeOffset) override;

    void notifyAnimationStarted(double monotonicTime, int group);
    void notifyAnimationFinished(double monotonicTime, int group);

    int getLayerId() const { return m_layerId; }

private:
    //WTF::RefPtr<mc::AnimationPlayer> m_animationPlayer;
    //WTF::OwnPtr<WebToCCAnimationDelegateAdapter> m_animationDelegateAdapter;
    int m_layerId;
    mc::LayerTreeHost* m_host;
    blink::WebCompositorAnimationDelegate* m_animation;
    mc::ScopedPtrVector<mc::AnimationObj> m_animationsCache;
};

}  // namespace mc_blink

#endif  // mc_blink_WebCompositorAnimationPlayerImpl_h
