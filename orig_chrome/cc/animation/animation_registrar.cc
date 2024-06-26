// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/animation_registrar.h"

#include "base/trace_event/trace_event_argument.h"
#include "cc/animation/layer_animation_controller.h"

namespace cc {

AnimationRegistrar::AnimationRegistrar()
    : supports_scroll_animations_(false)
{
}

AnimationRegistrar::~AnimationRegistrar()
{
    AnimationControllerMap copy = all_animation_controllers_;
    for (AnimationControllerMap::iterator iter = copy.begin();
         iter != copy.end();
         ++iter)
        (*iter).second->SetAnimationRegistrar(nullptr);
}

scoped_refptr<LayerAnimationController>
AnimationRegistrar::GetAnimationControllerForId(int id)
{
    scoped_refptr<LayerAnimationController> to_return;
    if (!ContainsKey(all_animation_controllers_, id)) {
        to_return = LayerAnimationController::Create(id);
        to_return->SetAnimationRegistrar(this);
        all_animation_controllers_[id] = to_return.get();
    } else {
        to_return = all_animation_controllers_[id];
    }
    return to_return;
}

void AnimationRegistrar::DidActivateAnimationController(
    LayerAnimationController* controller)
{
    active_animation_controllers_[controller->id()] = controller;
}

void AnimationRegistrar::DidDeactivateAnimationController(
    LayerAnimationController* controller)
{
    if (ContainsKey(active_animation_controllers_, controller->id()))
        active_animation_controllers_.erase(controller->id());
}

void AnimationRegistrar::RegisterAnimationController(
    LayerAnimationController* controller)
{
    all_animation_controllers_[controller->id()] = controller;
}

void AnimationRegistrar::UnregisterAnimationController(
    LayerAnimationController* controller)
{
    if (ContainsKey(all_animation_controllers_, controller->id()))
        all_animation_controllers_.erase(controller->id());
    DidDeactivateAnimationController(controller);
}

bool AnimationRegistrar::ActivateAnimations()
{
    if (!needs_animate_layers())
        return false;

    //TRACE_EVENT0("cc", "AnimationRegistrar::ActivateAnimations");
    AnimationControllerMap active_controllers_copy = active_animation_controllers_;
    for (auto& it : active_controllers_copy)
        it.second->ActivateAnimations();

    return true;
}

bool AnimationRegistrar::AnimateLayers(base::TimeTicks monotonic_time)
{
    if (!needs_animate_layers())
        return false;

    //TRACE_EVENT0("cc", "AnimationRegistrar::AnimateLayers");
    AnimationControllerMap controllers_copy = active_animation_controllers_;
    for (auto& it : controllers_copy)
        it.second->Animate(monotonic_time);

    return true;
}

bool AnimationRegistrar::UpdateAnimationState(bool start_ready_animations,
    AnimationEventsVector* events)
{
    if (!needs_animate_layers())
        return false;

    //TRACE_EVENT0("cc", "AnimationRegistrar::UpdateAnimationState");
    AnimationControllerMap active_controllers_copy = active_animation_controllers_;
    for (auto& it : active_controllers_copy)
        it.second->UpdateState(start_ready_animations, events);

    return true;
}

void AnimationRegistrar::SetAnimationEvents(
    scoped_ptr<AnimationEventsVector> events)
{
    for (size_t event_index = 0; event_index < events->size(); ++event_index) {
        int event_layer_id = (*events)[event_index].layer_id;

        // Use the map of all controllers, not just active ones, since non-active
        // controllers may still receive events for impl-only animations.
        const AnimationRegistrar::AnimationControllerMap& animation_controllers = all_animation_controllers_;
        auto iter = animation_controllers.find(event_layer_id);
        if (iter != animation_controllers.end()) {
            switch ((*events)[event_index].type) {
            case AnimationEvent::STARTED:
                (*iter).second->NotifyAnimationStarted((*events)[event_index]);
                break;

            case AnimationEvent::FINISHED:
                (*iter).second->NotifyAnimationFinished((*events)[event_index]);
                break;

            case AnimationEvent::ABORTED:
                (*iter).second->NotifyAnimationAborted((*events)[event_index]);
                break;

            case AnimationEvent::PROPERTY_UPDATE:
                (*iter).second->NotifyAnimationPropertyUpdate((*events)[event_index]);
                break;
            }
        }
    }
}

} // namespace cc
