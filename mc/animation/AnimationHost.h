// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_AnimationHost_h
#define mc_animation_AnimationHost_h

#include "base/time/time.h"
#include "mc/animation/AnimationEvents.h"
#include "mc/animation/MutatorHostClient.h"

#include "third_party/WebKit/Source/wtf/HashMap.h"
#include "third_party/WebKit/Source/wtf/RefPtr.h"
#include <vector>

namespace mc {

class AnimationPlayer;
class AnimationRegistrar;
class AnimationTimeline;
class ElementAnimations;
class LayerAnimationController;
class LayerTreeHost;
class FloatBox;

enum class ThreadInstance { MAIN, IMPL };

typedef std::vector<AnimationTimeline*> AnimationTimelineList;

// An AnimationHost contains all the state required to play animations.
// Specifically, it owns all the AnimationTimelines objects.
// There is just one AnimationHost for LayerTreeHost on main renderer thread
// and just one AnimationHost for LayerTreeHostImpl on impl thread.
// We synchronize them during the commit process in a one-way data flow process
// (PushPropertiesTo).
// An AnimationHost talks to its correspondent LayerTreeHost via
// LayerTreeMutatorsClient interface.
// AnimationHost has it's own instance of AnimationRegistrar,
// we want to merge AnimationRegistrar into AnimationHost.
class AnimationHost {
public:
    static PassOwnPtr<AnimationHost> create(ThreadInstance threadInstance);
    virtual ~AnimationHost();

    void addAnimationTimeline(PassRefPtr<AnimationTimeline> timeline);
    void removeAnimationTimeline(PassRefPtr<AnimationTimeline> timeline);
    AnimationTimeline* getTimelineById(int timeline_id) const;

    void clearTimelines();

    void registerLayer(int layerId, LayerTreeType treeType);
    void unregisterLayer(int layerId, LayerTreeType treeType);

    void registerPlayerForLayer(int layerId, AnimationPlayer* player);
    void unregisterPlayerForLayer(int layerId, AnimationPlayer* player);

    ElementAnimations* getElementAnimationsForLayerId(int layerId) const;

    // TODO(loyso): Get rid of LayerAnimationController.
    LayerAnimationController* getControllerForLayerId(int layerId) const;

    // Parent LayerTreeHost or LayerTreeHostImpl.
    MutatorHostClient* getMutatorHostClient() 
    {
        return m_mutatorHostClient;
    }
    const MutatorHostClient* getMutatorHostClient() const 
    {
        return m_mutatorHostClient;
    }
    void setMutatorHostClient(MutatorHostClient* client);

    void setNeedsCommit();

    void pushPropertiesTo(AnimationHost* host_impl);

    AnimationRegistrar* getAnimationRegistrar() const 
    {
        return m_animationRegistrar.get();
    }

    void setSupportsScrollAnimations(bool supports_scroll_animations);
    bool supportsScrollAnimations() const;
    bool needsAnimateLayers() const;

    bool activateAnimations();
    bool animateLayers(base::TimeTicks monotonic_time);
    bool updateAnimationState(bool start_ready_animations, AnimationEventsVector* events);

    PassOwnPtr<AnimationEventsVector> createEvents();
    void setAnimationEvents(PassOwnPtr<AnimationEventsVector> events);

    bool scrollOffsetAnimationWasInterrupted(int layerId) const;

    bool isAnimatingFilterProperty(int layerId, LayerTreeType treeType) const;
    bool isAnimatingOpacityProperty(int layerId, LayerTreeType treeType) const;
    bool isAnimatingTransformProperty(int layerId, LayerTreeType treeType) const;

    bool hasPotentiallyRunningFilterAnimation(int layerId, LayerTreeType treeType) const;
    bool hasPotentiallyRunningOpacityAnimation(int layerId, LayerTreeType treeType) const;
    bool hasPotentiallyRunningTransformAnimation(int layerId, LayerTreeType treeType) const;

    bool hasAnyAnimationTargetingProperty(int layerId, AnimationObj::TargetProperty property) const;

    bool filterIsAnimatingOnImplOnly(int layerId) const;
    bool opacityIsAnimatingOnImplOnly(int layerId) const;
    bool transformIsAnimatingOnImplOnly(int layerId) const;

    bool hasFilterAnimationThatInflatesBounds(int layerId) const;
    bool hasTransformAnimationThatInflatesBounds(int layerId) const;
    bool hasAnimationThatInflatesBounds(int layerId) const;

    bool filterAnimationBoundsForBox(int layerId, const FloatBox& box, FloatBox* bounds) const;
    bool transformAnimationBoundsForBox(int layerId, const FloatBox& box, FloatBox* bounds) const;

    bool hasOnlyTranslationTransforms(int layerId, LayerTreeType treeType) const;
    bool animationsPreserveAxisAlignment(int layerId) const;

    bool maximumTargetScale(int layerId, LayerTreeType treeType, float* max_scale) const;
    bool animationStartScale(int layerId, LayerTreeType treeType, float* start_scale) const;

    bool hasAnyAnimation(int layerId) const;
    bool hasActiveAnimation(int layerId) const;

    void implOnlyScrollAnimationCreate(int layerId, const blink::FloatPoint& target_offset, const blink::FloatPoint& current_offset);
    bool implOnlyScrollAnimationUpdateTarget(int layerId, const blink::FloatPoint& scroll_delta, const blink::FloatPoint& max_scroll_offset, base::TimeTicks frame_monotonic_time);

private:
    explicit AnimationHost(ThreadInstance thread_instance);

    void pushTimelinesToImplThread(AnimationHost* host_impl) const;
    void removeTimelinesFromImplThread(AnimationHost* host_impl) const;
    void pushPropertiesToImplThread(AnimationHost* host_impl);

    void eraseTimelines(AnimationTimelineList::iterator begin, AnimationTimelineList::iterator end);

    // TODO(loyso): For now AnimationPlayers share LayerAnimationController object
    // if they are attached to the same element(layer). Note that Element can
    // contain many Layers.
    typedef WTF::HashMap<int, ElementAnimations*> LayerToElementAnimationsMap;
    LayerToElementAnimationsMap m_layerToElementAnimationsMap;

    AnimationTimelineList m_timelines;
    PassOwnPtr<AnimationRegistrar> m_animationRegistrar;
    MutatorHostClient* m_mutatorHostClient;

    class ScrollOffsetAnimations;
    WTF::OwnPtr<ScrollOffsetAnimations> m_scrollOffsetAnimations;

    const ThreadInstance m_threadInstance;
};

}  // namespace mc

#endif  // mc_animation_AnimationHost_h
