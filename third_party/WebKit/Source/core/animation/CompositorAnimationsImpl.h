/*
 * Copyright (c) 2013, Google Inc. All rights reserved.
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

#ifndef CompositorAnimationsImpl_h
#define CompositorAnimationsImpl_h

#include "core/CoreExport.h"
#include "core/animation/EffectModel.h"
#include "core/animation/KeyframeEffectModel.h"
#include "core/animation/Timing.h"
#include "platform/animation/TimingFunction.h"
#include "public/platform/WebCompositorAnimation.h"

namespace blink {

class WebCompositorAnimationCurve;

class CORE_EXPORT CompositorAnimationsImpl {
private:
    struct CompositorTiming {
        void assertValid() const
        {
            ASSERT(scaledDuration > 0);
            ASSERT(std::isfinite(scaledTimeOffset));
            ASSERT(adjustedIterationCount > 0 || adjustedIterationCount == -1);
            ASSERT(std::isfinite(playbackRate) && playbackRate);
            ASSERT(iterationStart >= 0);
        }

        Timing::PlaybackDirection direction;
        double scaledDuration;
        double scaledTimeOffset;
        double adjustedIterationCount;
        double playbackRate;
        Timing::FillMode fillMode;
        double iterationStart;
    };

    static bool convertTimingForCompositor(const Timing&, double timeOffset, CompositorTiming& out, double animationPlaybackRate);

    static void getAnimationOnCompositor(const Timing&, int group, double startTime, double timeOffset, const KeyframeEffectModelBase&, Vector<OwnPtr<WebCompositorAnimation>>& animations, double animationPlaybackRate);

    static void addKeyframesToCurve(WebCompositorAnimationCurve&, const AnimatableValuePropertySpecificKeyframeVector&, const Timing&);

    friend class CompositorAnimations;
    friend class AnimationCompositorAnimationsTest;
};

} // namespace blink

#endif
