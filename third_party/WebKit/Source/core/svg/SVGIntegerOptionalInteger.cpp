/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
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
#include "core/svg/SVGIntegerOptionalInteger.h"

#include "core/svg/SVGAnimationElement.h"
#include "core/svg/SVGParserUtilities.h"

namespace blink {

SVGIntegerOptionalInteger::SVGIntegerOptionalInteger(PassRefPtrWillBeRawPtr<SVGInteger> firstInteger, PassRefPtrWillBeRawPtr<SVGInteger> secondInteger)
    : SVGPropertyBase(classType())
    , m_firstInteger(firstInteger)
    , m_secondInteger(secondInteger)
{
}

DEFINE_TRACE(SVGIntegerOptionalInteger)
{
    visitor->trace(m_firstInteger);
    visitor->trace(m_secondInteger);
    SVGPropertyBase::trace(visitor);
}

PassRefPtrWillBeRawPtr<SVGIntegerOptionalInteger> SVGIntegerOptionalInteger::clone() const
{
    return SVGIntegerOptionalInteger::create(m_firstInteger->clone(), m_secondInteger->clone());
}

PassRefPtrWillBeRawPtr<SVGPropertyBase> SVGIntegerOptionalInteger::cloneForAnimation(const String& value) const
{
    float floatX, floatY;
    if (!parseNumberOptionalNumber(value, floatX, floatY)) {
        return SVGIntegerOptionalInteger::create(SVGInteger::create(0), SVGInteger::create(0));
    }

    int x = static_cast<int>(roundf(floatX));
    int y = static_cast<int>(roundf(floatY));

    return SVGIntegerOptionalInteger::create(SVGInteger::create(x), SVGInteger::create(y));
}

String SVGIntegerOptionalInteger::valueAsString() const
{
    if (m_firstInteger->value() == m_secondInteger->value()) {
        return String::number(m_firstInteger->value());
    }

    return String::number(m_firstInteger->value()) + " " + String::number(m_secondInteger->value());
}

void SVGIntegerOptionalInteger::setValueAsString(const String& value, ExceptionState& exceptionState)
{
    float x, y;
    if (!parseNumberOptionalNumber(value, x, y)) {
        exceptionState.throwDOMException(SyntaxError, "The value provided ('" + value + "') is invalid.");
        x = y = 0;
    }

    m_firstInteger->setValue(x);
    m_secondInteger->setValue(y);
}

void SVGIntegerOptionalInteger::add(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement*)
{
    RefPtrWillBeRawPtr<SVGIntegerOptionalInteger> otherIntegerOptionalInteger = toSVGIntegerOptionalInteger(other);

    m_firstInteger->setValue(m_firstInteger->value() + otherIntegerOptionalInteger->m_firstInteger->value());
    m_secondInteger->setValue(m_secondInteger->value() + otherIntegerOptionalInteger->m_secondInteger->value());
}

void SVGIntegerOptionalInteger::calculateAnimatedValue(SVGAnimationElement* animationElement, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> from, PassRefPtrWillBeRawPtr<SVGPropertyBase> to, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDuration, SVGElement*)
{
    ASSERT(animationElement);

    RefPtrWillBeRawPtr<SVGIntegerOptionalInteger> fromInteger = toSVGIntegerOptionalInteger(from);
    RefPtrWillBeRawPtr<SVGIntegerOptionalInteger> toInteger = toSVGIntegerOptionalInteger(to);
    RefPtrWillBeRawPtr<SVGIntegerOptionalInteger> toAtEndOfDurationInteger = toSVGIntegerOptionalInteger(toAtEndOfDuration);

    float x = m_firstInteger->value();
    float y = m_secondInteger->value();
    animationElement->animateAdditiveNumber(percentage, repeatCount, fromInteger->firstInteger()->value(), toInteger->firstInteger()->value(), toAtEndOfDurationInteger->firstInteger()->value(), x);
    animationElement->animateAdditiveNumber(percentage, repeatCount, fromInteger->secondInteger()->value(), toInteger->secondInteger()->value(), toAtEndOfDurationInteger->secondInteger()->value(), y);
    m_firstInteger->setValue(static_cast<int>(roundf(x)));
    m_secondInteger->setValue(static_cast<int>(roundf(y)));
}

float SVGIntegerOptionalInteger::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement*)
{
    // FIXME: Distance calculation is not possible for SVGIntegerOptionalInteger right now. We need the distance for every single value.
    return -1;
}

}
