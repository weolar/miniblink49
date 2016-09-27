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
#include "core/svg/SVGNumberOptionalNumber.h"

#include "core/svg/SVGAnimationElement.h"
#include "core/svg/SVGParserUtilities.h"

namespace blink {

SVGNumberOptionalNumber::SVGNumberOptionalNumber(PassRefPtrWillBeRawPtr<SVGNumber> firstNumber, PassRefPtrWillBeRawPtr<SVGNumber> secondNumber)
    : SVGPropertyBase(classType())
    , m_firstNumber(firstNumber)
    , m_secondNumber(secondNumber)
{
}

DEFINE_TRACE(SVGNumberOptionalNumber)
{
    visitor->trace(m_firstNumber);
    visitor->trace(m_secondNumber);
    SVGPropertyBase::trace(visitor);
}

PassRefPtrWillBeRawPtr<SVGNumberOptionalNumber> SVGNumberOptionalNumber::clone() const
{
    return SVGNumberOptionalNumber::create(m_firstNumber->clone(), m_secondNumber->clone());
}

PassRefPtrWillBeRawPtr<SVGPropertyBase> SVGNumberOptionalNumber::cloneForAnimation(const String& value) const
{
    float x, y;
    if (!parseNumberOptionalNumber(value, x, y)) {
        x = y = 0;
    }

    return SVGNumberOptionalNumber::create(SVGNumber::create(x), SVGNumber::create(y));
}

String SVGNumberOptionalNumber::valueAsString() const
{
    if (m_firstNumber->value() == m_secondNumber->value()) {
        return String::number(m_firstNumber->value());
    }

    return String::number(m_firstNumber->value()) + " " + String::number(m_secondNumber->value());
}

void SVGNumberOptionalNumber::setValueAsString(const String& value, ExceptionState& exceptionState)
{
    float x, y;
    if (!parseNumberOptionalNumber(value, x, y)) {
        exceptionState.throwDOMException(SyntaxError, "The value provided ('" + value + "') is invalid.");
        x = y = 0;
    }

    m_firstNumber->setValue(x);
    m_secondNumber->setValue(y);
}

void SVGNumberOptionalNumber::add(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement*)
{
    RefPtrWillBeRawPtr<SVGNumberOptionalNumber> otherNumberOptionalNumber = toSVGNumberOptionalNumber(other);

    m_firstNumber->setValue(m_firstNumber->value() + otherNumberOptionalNumber->m_firstNumber->value());
    m_secondNumber->setValue(m_secondNumber->value() + otherNumberOptionalNumber->m_secondNumber->value());
}

void SVGNumberOptionalNumber::calculateAnimatedValue(SVGAnimationElement* animationElement, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> from, PassRefPtrWillBeRawPtr<SVGPropertyBase> to, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDuration, SVGElement*)
{
    ASSERT(animationElement);

    RefPtrWillBeRawPtr<SVGNumberOptionalNumber> fromNumber = toSVGNumberOptionalNumber(from);
    RefPtrWillBeRawPtr<SVGNumberOptionalNumber> toNumber = toSVGNumberOptionalNumber(to);
    RefPtrWillBeRawPtr<SVGNumberOptionalNumber> toAtEndOfDurationNumber = toSVGNumberOptionalNumber(toAtEndOfDuration);

    float x = m_firstNumber->value();
    float y = m_secondNumber->value();
    animationElement->animateAdditiveNumber(percentage, repeatCount, fromNumber->firstNumber()->value(), toNumber->firstNumber()->value(), toAtEndOfDurationNumber->firstNumber()->value(), x);
    animationElement->animateAdditiveNumber(percentage, repeatCount, fromNumber->secondNumber()->value(), toNumber->secondNumber()->value(), toAtEndOfDurationNumber->secondNumber()->value(), y);
    m_firstNumber->setValue(x);
    m_secondNumber->setValue(y);
}

float SVGNumberOptionalNumber::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement*)
{
    // FIXME: Distance calculation is not possible for SVGNumberOptionalNumber right now. We need the distance for every single value.
    return -1;
}

}
