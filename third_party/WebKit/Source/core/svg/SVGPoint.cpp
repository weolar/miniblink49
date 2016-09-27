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

#include "core/svg/SVGPoint.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/svg/SVGAnimationElement.h"
#include "core/svg/SVGParserUtilities.h"
#include "platform/transforms/AffineTransform.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"

namespace blink {

SVGPoint::SVGPoint()
{
}

SVGPoint::SVGPoint(const FloatPoint& point)
    : m_value(point)
{
}

PassRefPtrWillBeRawPtr<SVGPoint> SVGPoint::clone() const
{
    return SVGPoint::create(m_value);
}

template<typename CharType>
void SVGPoint::parse(const CharType*& ptr, const CharType* end, ExceptionState& exceptionState)
{
    const CharType* start = ptr;

    skipOptionalSVGSpaces(ptr, end);

    float x = 0.0f;
    float y = 0.0f;
    bool valid = parseNumber(ptr, end, x) && parseNumber(ptr, end, y, DisallowWhitespace);

    if (!valid) {
        exceptionState.throwDOMException(SyntaxError, "Problem parsing point \"" + String(start, end - start) + "\"");
        return;
    }

    skipOptionalSVGSpaces(ptr, end);
    if (ptr < end) { // nothing should come after the last, fourth number
        exceptionState.throwDOMException(SyntaxError, "Problem parsing point \"" + String(start, end - start) + "\"");
        return;
    }

    m_value = FloatPoint(x, y);
}

FloatPoint SVGPoint::matrixTransform(const AffineTransform& transform) const
{
    double newX, newY;
    transform.map(static_cast<double>(x()), static_cast<double>(y()), newX, newY);
    return FloatPoint::narrowPrecision(newX, newY);
}

void SVGPoint::setValueAsString(const String& string, ExceptionState& exceptionState)
{
    if (string.isEmpty()) {
        m_value = FloatPoint(0.0f, 0.0f);
        return;
    }

    if (string.is8Bit()) {
        const LChar* ptr = string.characters8();
        const LChar* end = ptr + string.length();
        parse(ptr, end, exceptionState);
        return;
    }

    const UChar* ptr = string.characters16();
    const UChar* end = ptr + string.length();
    parse(ptr, end, exceptionState);
}

String SVGPoint::valueAsString() const
{
    StringBuilder builder;
    builder.appendNumber(x());
    builder.append(' ');
    builder.appendNumber(y());
    return builder.toString();
}

void SVGPoint::add(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement*)
{
    // SVGPoint is not animated by itself
    ASSERT_NOT_REACHED();
}

void SVGPoint::calculateAnimatedValue(SVGAnimationElement* animationElement, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> fromValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement*)
{
    // SVGPoint is not animated by itself
    ASSERT_NOT_REACHED();
}

float SVGPoint::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement* contextElement)
{
    // SVGPoint is not animated by itself
    ASSERT_NOT_REACHED();
    return 0.0f;
}

}
