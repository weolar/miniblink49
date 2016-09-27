/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/animation/CompositorPendingAnimations.h"

#include "core/animation/AnimationTimeline.h"
#include "core/animation/KeyframeEffect.h"
#include "core/dom/Document.h"
#include "core/frame/FrameView.h"
#include "core/page/Page.h"
#include "platform/TraceEvent.h"

namespace blink {

void CompositorPendingAnimations::add(Animation* animation)
{
    ASSERT(animation);
    ASSERT(m_pending.find(animation) == kNotFound);
    m_pending.append(animation);

    Document* document = animation->timeline()->document();
    if (document->view())
        document->view()->scheduleAnimation();

    bool visible = document->page() && document->page()->visibilityState() == PageVisibilityStateVisible;
    if (!visible && !m_timer.isActive()) {
        m_timer.startOneShot(0, FROM_HERE);
    }
}

bool CompositorPendingAnimations::update(bool startOnCompositor)
{
    WillBeHeapVector<RawPtrWillBeMember<Animation>> waitingForStartTime;
    bool startedSynchronizedOnCompositor = false;

    WillBeHeapVector<RefPtrWillBeMember<Animation>> animations;
    animations.swap(m_pending);
    int compositorGroup = ++m_compositorGroup;
    if (compositorGroup == 0) {
        // Wrap around, skipping 0.
        compositorGroup = ++m_compositorGroup;
    }

    for (auto& animation : animations) {
        bool hadCompositorAnimation = animation->hasActiveAnimationsOnCompositor();
        animation->preCommit(compositorGroup, startOnCompositor);
        if (animation->hasActiveAnimationsOnCompositor() && !hadCompositorAnimation) {
            startedSynchronizedOnCompositor = true;
        }

        if (animation->playing() && !animation->hasStartTime()) {
            waitingForStartTime.append(animation.get());
        }
    }

    // If any synchronized animations were started on the compositor, all
    // remaning synchronized animations need to wait for the synchronized
    // start time. Otherwise they may start immediately.
    if (startedSynchronizedOnCompositor) {
        for (auto& animation : waitingForStartTime) {
            if (!animation->hasStartTime()) {
                m_waitingForCompositorAnimationStart.append(animation);
            }
        }
    } else {
        for (auto& animation : waitingForStartTime) {
            if (!animation->hasStartTime()) {
                animation->notifyCompositorStartTime(animation->timeline()->currentTimeInternal());
            }
        }
    }

    // FIXME: The postCommit should happen *after* the commit, not before.
    for (auto& animation : animations)
        animation->postCommit(animation->timeline()->currentTimeInternal());

    ASSERT(m_pending.isEmpty());

    if (startedSynchronizedOnCompositor)
        return true;

    if (m_waitingForCompositorAnimationStart.isEmpty())
        return false;

    // Check if we're still waiting for any compositor animations to start.
    for (auto& animation : m_waitingForCompositorAnimationStart) {
        if (animation->hasActiveAnimationsOnCompositor())
            return true;
    }

    // If not, go ahead and start any animations that were waiting.
    notifyCompositorAnimationStarted(monotonicallyIncreasingTime());

    ASSERT(m_pending.isEmpty());
    return false;
}

void CompositorPendingAnimations::notifyCompositorAnimationStarted(double monotonicAnimationStartTime, int compositorGroup)
{
    TRACE_EVENT0("blink", "CompositorPendingAnimations::notifyCompositorAnimationStarted");
    WillBeHeapVector<RefPtrWillBeMember<Animation>> animations;
    animations.swap(m_waitingForCompositorAnimationStart);

    for (auto animation : animations) {
        if (animation->hasStartTime() || animation->playStateInternal() != Animation::Pending) {
            // Already started or no longer relevant.
            continue;
        }
        if (compositorGroup && animation->compositorGroup() != compositorGroup) {
            // Still waiting.
            m_waitingForCompositorAnimationStart.append(animation);
            continue;
        }
        animation->notifyCompositorStartTime(monotonicAnimationStartTime - animation->timeline()->zeroTime());
    }

}

DEFINE_TRACE(CompositorPendingAnimations)
{
    visitor->trace(m_pending);
    visitor->trace(m_waitingForCompositorAnimationStart);
}

} // namespace
