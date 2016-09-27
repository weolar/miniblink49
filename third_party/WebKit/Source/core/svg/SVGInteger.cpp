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
#include "core/svg/SVGInteger.h"
#include "core/html/parser/HTMLParserIdioms.h"

#include "core/svg/SVGAnimationElement.h"

namespace blink {

SVGInteger::SVGInteger(int value)
    : m_value(value)
{
}

PassRefPtrWillBeRawPtr<SVGInteger> SVGInteger::clone() const
{
    return create(m_value);
}

String SVGInteger::valueAsString() const
{
    return String::number(m_value);
}

void SVGInteger::setValueAsString(const String& string, ExceptionState& exceptionState)
{
    if (string.isEmpty()) {
        m_value = 0;
        return;
    }

    bool valid = true;
    m_value = stripLeadingAndTrailingHTMLSpaces(string).toIntStrict(&valid);

    if (!valid) {
        exceptionState.throwDOMException(SyntaxError, "The value provided ('" + string + "') is invalid.");
        m_value = 0;
    }
}

void SVGInteger::add(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement*)
{
    setValue(m_value + toSVGInteger(other)->value());
}

void SVGInteger::calculateAnimatedValue(SVGAnimationElement* animationElement, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> from, PassRefPtrWillBeRawPtr<SVGPropertyBase> to, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDuration, SVGElement*)
{
    ASSERT(animationElement);

    RefPtrWillBeRawPtr<SVGInteger> fromInteger = toSVGInteger(from);
    RefPtrWillBeRawPtr<SVGInteger> toInteger = toSVGInteger(to);
    RefPtrWillBeRawPtr<SVGInteger> toAtEndOfDurationInteger = toSVGInteger(toAtEndOfDuration);

    float animatedFloat = m_value;
    animationElement->animateAdditiveNumber(percentage, repeatCount, fromInteger->value(), toInteger->value(), toAtEndOfDurationInteger->value(), animatedFloat);
    m_value = static_cast<int>(roundf(animatedFloat));
}

float SVGInteger::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement*)
{
    return abs(m_value - toSVGInteger(other)->value());
}

}
