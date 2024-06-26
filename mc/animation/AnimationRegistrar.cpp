// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/animation/AnimationRegistrar.h"

#include "mc/animation/layerAnimationController.h"

namespace mc {

AnimationRegistrar::AnimationRegistrar() : m_supportsScrollAnimations(false) {
}

AnimationRegistrar::~AnimationRegistrar()
{
    AnimationControllerMap copy = m_allAnimationControllers;
    for (AnimationControllerMap::iterator iter = copy.begin(); iter != copy.end(); ++iter)
        iter->value->setAnimationRegistrar(nullptr);
}

WTF::PassRefPtr<LayerAnimationController> AnimationRegistrar::getAnimationControllerForId(int id)
{
    AnimationRegistrar::AnimationControllerMap::iterator it = m_allAnimationControllers.find(id);
    if (it == m_allAnimationControllers.end()) {
        WTF::PassRefPtr<LayerAnimationController> toReturn = LayerAnimationController::create(nullptr, id);
        toReturn->setAnimationRegistrar(this);
        m_allAnimationControllers.add(id, toReturn.get());

        return toReturn;
    } else {
        return adoptRef(it->value);
    }
}

void AnimationRegistrar::didActivateAnimationController(LayerAnimationController* controller)
{
    m_activeAnimationControllers.add(controller->id(), controller);
}

void AnimationRegistrar::didDeactivateAnimationController(LayerAnimationController* controller) 
{
    AnimationRegistrar::AnimationControllerMap::iterator it = m_activeAnimationControllers.find(controller->id());
    if (it != m_allAnimationControllers.end())
        m_activeAnimationControllers.remove(it);
}

void AnimationRegistrar::registerAnimationController(LayerAnimationController* controller) 
{
    m_allAnimationControllers.add(controller->id(), controller);
}

void AnimationRegistrar::unregisterAnimationController(LayerAnimationController* controller) 
{
    AnimationRegistrar::AnimationControllerMap::iterator it = m_allAnimationControllers.find(controller->id());
    if (it != m_allAnimationControllers.end())
        m_allAnimationControllers.remove(it);
    didDeactivateAnimationController(controller);
}

bool AnimationRegistrar::activateAnimations() 
{
    if (!needsAnimateLayers())
        return false;

    //TRACE_EVENT0("cc", "AnimationRegistrar::ActivateAnimations");
    AnimationRegistrar::AnimationControllerMap activeAontrollersCopy = m_activeAnimationControllers;
    AnimationRegistrar::AnimationControllerMap::iterator it = activeAontrollersCopy.begin();
    for (; it != activeAontrollersCopy.end(); ++it)
        it->value->activateAnimations();

    return true;
}

bool AnimationRegistrar::animateLayers(base::TimeTicks monotonicTime) 
{
    if (!needsAnimateLayers())
        return false;

    //TRACE_EVENT0("cc", "AnimationRegistrar::AnimateLayers");
    AnimationRegistrar::AnimationControllerMap controllersCopy = m_activeAnimationControllers;
    AnimationRegistrar::AnimationControllerMap::iterator it = controllersCopy.begin();
    for (; it != controllersCopy.end(); ++it)
        it->value->animate(monotonicTime);

    return true;
}

bool AnimationRegistrar::updateAnimationState(bool startReadyAnimations, AnimationEventsVector* events) 
{
    if (!needsAnimateLayers())
        return false;

    //TRACE_EVENT0("cc", "AnimationRegistrar::UpdateAnimationState");
    AnimationRegistrar::AnimationControllerMap activeAontrollersCopy = m_activeAnimationControllers;
    AnimationRegistrar::AnimationControllerMap::iterator it = activeAontrollersCopy.begin();
    for (; it != activeAontrollersCopy.end(); ++it)
        it->value->updateState(startReadyAnimations, events);

    return true;
}

void AnimationRegistrar::setAnimationEvents(WTF::PassOwnPtr<AnimationEventsVector> events)
{
    for (size_t eventIndex = 0; eventIndex < events->size(); ++eventIndex) {
        int eventLayerId = (*events)[eventIndex].layerId;

        // Use the map of all controllers, not just active ones, since non-active
        // controllers may still receive events for impl-only animations.
        const AnimationRegistrar::AnimationControllerMap& animationControllers = m_allAnimationControllers;
        AnimationRegistrar::AnimationControllerMap::const_iterator iter = animationControllers.find(eventLayerId);
        if (iter != animationControllers.end()) {
            LayerAnimationController* controller = iter->value;
            switch ((*events)[eventIndex].type) {
            case AnimationEvent::STARTED:
                controller->notifyAnimationStarted((*events)[eventIndex]);
                break;

            case AnimationEvent::FINISHED:
                controller->notifyAnimationFinished((*events)[eventIndex]);
                break;

            case AnimationEvent::ABORTED:
                controller->notifyAnimationAborted((*events)[eventIndex]);
                break;

            case AnimationEvent::PROPERTY_UPDATE:
                controller->notifyAnimationPropertyUpdate((*events)[eventIndex]);
                break;
            }
        }
    }
}

}  // namespace mc
