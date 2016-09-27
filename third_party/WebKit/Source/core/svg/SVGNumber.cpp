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
#include "core/svg/SVGNumber.h"

#include "core/svg/SVGAnimationElement.h"
#include "core/svg/SVGParserUtilities.h"

namespace blink {

SVGNumber::SVGNumber(float value)
    : m_value(value)
{
}

PassRefPtrWillBeRawPtr<SVGNumber> SVGNumber::clone() const
{
    return create(m_value);
}

String SVGNumber::valueAsString() const
{
    return String::number(m_value);
}

template<typename CharType>
bool SVGNumber::parse(const CharType*& ptr, const CharType* end)
{
    if (!parseNumber(ptr, end, m_value, AllowLeadingAndTrailingWhitespace)) {
        m_value = 0;
        return false;
    }

    if (ptr != end) {
        m_value = 0;
        return false;
    }

    return true;
}

void SVGNumber::setValueAsString(const String& string, ExceptionState& exceptionState)
{
    if (string.isEmpty()) {
        m_value = 0;
        return;
    }

    bool valid = false;
    if (string.is8Bit()) {
        const LChar* ptr = string.characters8();
        const LChar* end = ptr + string.length();
        valid = parse(ptr, end);
    } else {
        const UChar* ptr = string.characters16();
        const UChar* end = ptr + string.length();
        valid = parse(ptr, end);
    }

    if (!valid) {
        exceptionState.throwDOMException(SyntaxError, "The value provided ('" + string + "') is invalid.");
        m_value = 0;
    }
}

void SVGNumber::add(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement*)
{
    setValue(m_value + toSVGNumber(other)->value());
}

void SVGNumber::calculateAnimatedValue(SVGAnimationElement* animationElement, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> from, PassRefPtrWillBeRawPtr<SVGPropertyBase> to, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDuration, SVGElement*)
{
    ASSERT(animationElement);

    RefPtrWillBeRawPtr<SVGNumber> fromNumber = toSVGNumber(from);
    RefPtrWillBeRawPtr<SVGNumber> toNumber = toSVGNumber(to);
    RefPtrWillBeRawPtr<SVGNumber> toAtEndOfDurationNumber = toSVGNumber(toAtEndOfDuration);

    animationElement->animateAdditiveNumber(percentage, repeatCount, fromNumber->value(), toNumber->value(), toAtEndOfDurationNumber->value(), m_value);
}

float SVGNumber::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement*)
{
    return fabsf(m_value - toSVGNumber(other)->value());
}

PassRefPtrWillBeRawPtr<SVGNumber> SVGNumberAcceptPercentage::clone() const
{
    return create(m_value);
}

void SVGNumberAcceptPercentage::setValueAsString(const String& string, ExceptionState& exceptionState)
{
    bool valid = parseNumberOrPercentage(string, m_value);

    if (!valid) {
        exceptionState.throwDOMException(SyntaxError, "The value provided ('" + string + "') is invalid.");
        m_value = 0;
    }
}

SVGNumberAcceptPercentage::SVGNumberAcceptPercentage(float value)
    : SVGNumber(value)
{
}

}
