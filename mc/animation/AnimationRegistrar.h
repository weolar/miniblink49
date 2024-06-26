// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_ANIMATION_ANIMATION_REGISTRAR_H_
#define CC_ANIMATION_ANIMATION_REGISTRAR_H_

//#include "base/containers/hash_tables.h"
//#include "base/memory/ref_counted.h"
//#include "base/memory/scoped_ptr.h"
#include "mc/animation/AnimationEvents.h"
#include "third_party/WebKit/Source/wtf/HashMap.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"
#include "third_party/WebKit/Source/wtf/PassRefPtr.h"

namespace mc {

class LayerAnimationController;

class AnimationRegistrar {
public:
    typedef WTF::HashMap<int, LayerAnimationController*> AnimationControllerMap;

    static WTF::PassOwnPtr<AnimationRegistrar> create()
    {
        return adoptPtr(new AnimationRegistrar());
    }

    virtual ~AnimationRegistrar();

    // If an animation has been registered for the given id, return it. Otherwise
    // creates a new one and returns a scoped_refptr to it.
    WTF::PassRefPtr<LayerAnimationController> getAnimationControllerForId(int id);

    // Registers the given animation controller as active. An active animation
    // controller is one that has a running animation that needs to be ticked.
    void didActivateAnimationController(LayerAnimationController* controller);

    // Unregisters the given animation controller. When this happens, the
    // animation controller will no longer be ticked (since it's not active). It
    // is not an error to call this function with a deactivated controller.
    void didDeactivateAnimationController(LayerAnimationController* controller);

    // Registers the given controller as alive.
    void registerAnimationController(LayerAnimationController* controller);

    // Unregisters the given controller as alive.
    void unregisterAnimationController(LayerAnimationController* controller);

    const AnimationControllerMap& activeAnimationControllersForTesting() const
    {
        return m_activeAnimationControllers;
    }

    const AnimationControllerMap& allAnimationControllersForTesting() const
    {
        return m_allAnimationControllers;
    }

    void setSupportsScrollAnimations(bool supports_scroll_animations) 
    {
        m_supportsScrollAnimations = supports_scroll_animations;
    }

    bool supportsScrollAnimations()
    {
        return m_supportsScrollAnimations;
    }

    bool needsAnimateLayers() const 
    {
        return !m_activeAnimationControllers.isEmpty();
    }

    bool activateAnimations();
    bool animateLayers(base::TimeTicks monotonic_time);
    bool updateAnimationState(bool start_ready_animations, AnimationEventsVector* events);

    WTF::PassOwnPtr<AnimationEventsVector> createEvents()
    {
        return adoptPtr(new AnimationEventsVector());
    }

    void setAnimationEvents(WTF::PassOwnPtr<AnimationEventsVector> events);

private:
    AnimationRegistrar();

    AnimationControllerMap m_activeAnimationControllers;
    AnimationControllerMap m_allAnimationControllers;

    bool m_supportsScrollAnimations;
};

}  // namespace cc

#endif  // CC_ANIMATION_ANIMATION_REGISTRAR_H_
