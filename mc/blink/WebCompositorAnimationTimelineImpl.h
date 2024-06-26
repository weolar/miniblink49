// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_blink_WebCompositorAnimationTimelineImpl_h
#define mc_blink_WebCompositorAnimationTimelineImpl_h

#include "third_party/WebKit/public/platform/WebCompositorAnimationTimeline.h"
#include "third_party/WebKit/Source/wtf/RefPtr.h"

namespace blink {
class WebCompositorAnimationPlayerClient;
}

namespace mc {
class AnimationTimeline;
class LayerTreeHost;
}

namespace mc_blink {

class WebCompositorAnimationTimelineImpl : public blink::WebCompositorAnimationTimeline {
public:
    explicit WebCompositorAnimationTimelineImpl();
    ~WebCompositorAnimationTimelineImpl() override;

    mc::AnimationTimeline* getAnimationTimeline() const;

    void setHost(mc::LayerTreeHost* host)
    {
        m_host = host;
    }

    // blink::WebCompositorAnimationTimeline implementation
    void playerAttached(const blink::WebCompositorAnimationPlayerClient& client) override;
    void playerDestroyed(const blink::WebCompositorAnimationPlayerClient& client) override;

private:
    //WTF::RefPtr<mc::AnimationTimeline> m_animationTimeline;
    mc::LayerTreeHost* m_host;
};

}  // namespace mc_blink

#endif  // mc_blink_WebCompositorAnimationTimelineImpl_h
