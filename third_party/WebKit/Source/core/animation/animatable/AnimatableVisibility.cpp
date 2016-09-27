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
#include "core/animation/animatable/AnimatableVisibility.h"

namespace blink {

bool AnimatableVisibility::usesDefaultInterpolationWith(const AnimatableValue* value) const
{
    EVisibility from = m_visibility;
    EVisibility to = toAnimatableVisibility(value)->m_visibility;
    return from != VISIBLE && to != VISIBLE;
}

PassRefPtrWillBeRawPtr<AnimatableValue> AnimatableVisibility::interpolateTo(const AnimatableValue* value, double fraction) const
{
    EVisibility from = m_visibility;
    EVisibility to = toAnimatableVisibility(value)->m_visibility;
    if (from != VISIBLE && to != VISIBLE)
        return defaultInterpolateTo(this, value, fraction);
    if (fraction <= 0)
        return takeConstRef(this);
    if (fraction >= 1)
        return takeConstRef(value);
    return takeConstRef(from == VISIBLE ? this : value);
}

bool AnimatableVisibility::equalTo(const AnimatableValue* value) const
{
    return m_visibility == toAnimatableVisibility(value)->m_visibility;
}

} // namespace blink
