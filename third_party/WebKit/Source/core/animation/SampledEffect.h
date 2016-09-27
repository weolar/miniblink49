// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SampledEffect_h
#define SampledEffect_h

#include "core/animation/Animation.h"
#include "core/animation/Interpolation.h"
#include "core/animation/KeyframeEffect.h"
#include "wtf/BitArray.h"
#include "wtf/Vector.h"

namespace blink {

class SVGElement;

class SampledEffect : public NoBaseWillBeGarbageCollected<SampledEffect> {
public:
    static PassOwnPtrWillBeRawPtr<SampledEffect> create(KeyframeEffect* animation, PassOwnPtrWillBeRawPtr<WillBeHeapVector<RefPtrWillBeMember<Interpolation>>> interpolations)
    {
        return adoptPtrWillBeNoop(new SampledEffect(animation, interpolations));
    }

    void clear();

    const WillBeHeapVector<RefPtrWillBeMember<Interpolation>>& interpolations() const { return *m_interpolations; }
#if ENABLE(OILPAN)
    RawPtr<WillBeHeapVector<RefPtrWillBeMember<Interpolation>>> mutableInterpolations() { return m_interpolations.get(); }
#else
    PassOwnPtr<WillBeHeapVector<RefPtrWillBeMember<Interpolation>>> mutableInterpolations() { return m_interpolations.release(); }
#endif

    void setInterpolations(PassOwnPtrWillBeRawPtr<WillBeHeapVector<RefPtrWillBeMember<Interpolation>>> interpolations) { m_interpolations = interpolations; }

    KeyframeEffect* effect() const { return m_effect; }
    unsigned sequenceNumber() const { return m_sequenceNumber; }
    KeyframeEffect::Priority priority() const { return m_priority; }

    DECLARE_TRACE();

    void applySVGUpdate(SVGElement&);

private:
    SampledEffect(KeyframeEffect*, PassOwnPtrWillBeRawPtr<WillBeHeapVector<RefPtrWillBeMember<Interpolation>>>);

    RawPtrWillBeWeakMember<KeyframeEffect> m_effect;
    RefPtrWillBeMember<Animation> m_animation;
    OwnPtrWillBeMember<WillBeHeapVector<RefPtrWillBeMember<Interpolation>>> m_interpolations;
    const unsigned m_sequenceNumber;
    KeyframeEffect::Priority m_priority;
};

} // namespace blink

#endif // SampledEffect_h
