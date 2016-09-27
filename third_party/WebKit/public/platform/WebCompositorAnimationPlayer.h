// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebCompositorAnimationPlayer_h
#define WebCompositorAnimationPlayer_h

namespace blink {

class WebCompositorAnimation;
class WebCompositorAnimationDelegate;
class WebLayer;

// A compositor representation for AnimationPlayer.
class WebCompositorAnimationPlayer {
public:
    virtual ~WebCompositorAnimationPlayer() { }

    // An animation delegate is notified when animations are started and
    // stopped. The WebCompositorAnimationPlayer does not take ownership of the delegate, and it is
    // the responsibility of the client to reset the layer's delegate before
    // deleting the delegate.
    virtual void setAnimationDelegate(WebCompositorAnimationDelegate*) = 0;

    virtual void attachLayer(WebLayer*) = 0;
    virtual void detachLayer() = 0;
    virtual bool isLayerAttached() const = 0;

    virtual void addAnimation(WebCompositorAnimation*) = 0;
    virtual void removeAnimation(int animationId) = 0;
    virtual void pauseAnimation(int animationId, double timeOffset) = 0;
};

} // namespace blink

#endif // WebCompositorAnimationPlayer_h
