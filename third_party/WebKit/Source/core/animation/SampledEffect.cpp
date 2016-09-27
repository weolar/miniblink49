// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/SampledEffect.h"

#include "core/animation/SVGInterpolation.h"
#include "core/animation/StyleInterpolation.h"
#include "core/svg/SVGElement.h"

namespace blink {

SampledEffect::SampledEffect(KeyframeEffect* effect, PassOwnPtrWillBeRawPtr<WillBeHeapVector<RefPtrWillBeMember<Interpolation>>> interpolations)
    : m_effect(effect)
    , m_animation(effect->animation())
    , m_interpolations(interpolations)
    , m_sequenceNumber(effect->animation()->sequenceNumber())
    , m_priority(effect->priority())
{
    ASSERT(m_interpolations && !m_interpolations->isEmpty());
}

void SampledEffect::clear()
{
    m_effect = nullptr;
    m_animation = nullptr;
    m_interpolations->clear();
}

DEFINE_TRACE(SampledEffect)
{
    visitor->trace(m_effect);
    visitor->trace(m_animation);
#if ENABLE(OILPAN)
    visitor->trace(m_interpolations);
#endif
}

void SampledEffect::applySVGUpdate(SVGElement& targetElement)
{
    for (const auto& interpolation : *m_interpolations) {
        if (interpolation->isSVGInterpolation()) {
            toSVGInterpolation(interpolation.get())->apply(targetElement);
        }
    }
}

} // namespace blink
