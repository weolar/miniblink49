// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/InterpolableValue.h"

namespace blink {

DEFINE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(InterpolableValue);

void InterpolableNumber::interpolate(const InterpolableValue &to, const double progress, InterpolableValue& result) const
{
    const InterpolableNumber& toNumber = toInterpolableNumber(to);
    InterpolableNumber& resultNumber = toInterpolableNumber(result);

    if (progress == 0 || m_value == toNumber.m_value)
        resultNumber.m_value = m_value;
    else if (progress == 1)
        resultNumber.m_value = toNumber.m_value;
    else
        resultNumber.m_value = m_value * (1 - progress) + toNumber.m_value * progress;
}

void InterpolableBool::interpolate(const InterpolableValue &to, const double progress, InterpolableValue& result) const
{
    const InterpolableBool& toBool = toInterpolableBool(to);
    InterpolableBool& resultBool = toInterpolableBool(result);

    if (progress < 0.5)
        resultBool.m_value = m_value;
    else
        resultBool.m_value = toBool.m_value;
}

void InterpolableList::interpolate(const InterpolableValue& to, const double progress, InterpolableValue& result) const
{
    const InterpolableList& toList = toInterpolableList(to);
    InterpolableList& resultList = toInterpolableList(result);

    ASSERT(toList.m_size == m_size);
    ASSERT(resultList.m_size == m_size);

    for (size_t i = 0; i < m_size; i++) {
        ASSERT(m_values[i]);
        ASSERT(toList.m_values[i]);
        m_values[i]->interpolate(*(toList.m_values[i]), progress, *(resultList.m_values[i]));
    }
}

void InterpolableNumber::add(const InterpolableValue& rhs, InterpolableValue& result) const
{
    const InterpolableNumber& rhsNumber = toInterpolableNumber(rhs);
    InterpolableNumber& resultNumber = toInterpolableNumber(result);

    resultNumber.m_value = m_value + rhsNumber.m_value;
}

void InterpolableNumber::multiply(double scalar, InterpolableValue& result) const
{
    InterpolableNumber& resultNumber = toInterpolableNumber(result);

    resultNumber.m_value = scalar * m_value;
}

void InterpolableBool::add(const InterpolableValue& rhs, InterpolableValue& result) const
{
    const InterpolableBool& rhsBool = toInterpolableBool(rhs);
    InterpolableBool& resultBool = toInterpolableBool(result);

    resultBool.m_value = m_value || rhsBool.m_value;
}

void InterpolableList::add(const InterpolableValue& rhs, InterpolableValue& result) const
{
    const InterpolableList& rhsList = toInterpolableList(rhs);
    InterpolableList& resultList = toInterpolableList(result);

    ASSERT(rhsList.m_size == m_size);
    ASSERT(resultList.m_size == m_size);

    for (size_t i = 0; i < m_size; i++) {
        ASSERT(m_values[i]);
        ASSERT(rhsList.m_values[i]);
        m_values[i]->add(*(rhsList.m_values[i]), *(resultList.m_values[i]));
    }
}

void InterpolableList::multiply(double scalar, InterpolableValue& result) const
{
    InterpolableList& resultList = toInterpolableList(result);

    ASSERT(resultList.m_size == m_size);

    for (size_t i = 0; i < m_size; i++) {
        ASSERT(m_values[i]);
        m_values[i]->multiply(scalar, *(resultList.m_values[i]));
    }
}

DEFINE_TRACE(InterpolableList)
{
    visitor->trace(m_values);
    InterpolableValue::trace(visitor);
}

void InterpolableAnimatableValue::interpolate(const InterpolableValue& to, const double progress, InterpolableValue& result) const
{
    const InterpolableAnimatableValue& toValue = toInterpolableAnimatableValue(to);
    InterpolableAnimatableValue& resultValue = toInterpolableAnimatableValue(result);
    if (progress == 0)
        resultValue.m_value = m_value;
    if (progress == 1)
        resultValue.m_value = toValue.m_value;
    resultValue.m_value = AnimatableValue::interpolate(m_value.get(), toValue.m_value.get(), progress);
}

DEFINE_TRACE(InterpolableAnimatableValue)
{
    visitor->trace(m_value);
    InterpolableValue::trace(visitor);
}

}
