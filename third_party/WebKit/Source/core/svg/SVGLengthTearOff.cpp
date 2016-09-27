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

#include "core/svg/SVGLengthTearOff.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/svg/SVGElement.h"

namespace blink {

namespace {

inline SVGLengthType toSVGLengthType(unsigned short type)
{
    ASSERT(type >= LengthTypeUnknown && type <= LengthTypePC);
    return static_cast<SVGLengthType>(type);
}

inline bool canResolveRelativeUnits(const SVGElement* contextElement)
{
    return contextElement && contextElement->inDocument();
}

} // namespace

SVGLengthType SVGLengthTearOff::unitType()
{
    return hasExposedLengthUnit() ? target()->unitType() : LengthTypeUnknown;
}

SVGLengthMode SVGLengthTearOff::unitMode()
{
    return target()->unitMode();
}

float SVGLengthTearOff::value(ExceptionState& exceptionState)
{
    if (target()->isRelative() && !canResolveRelativeUnits(contextElement())) {
        exceptionState.throwDOMException(NotSupportedError, "Could not resolve relative length.");
        return 0;
    }

    SVGLengthContext lengthContext(contextElement());
    return target()->value(lengthContext);
}

void SVGLengthTearOff::setValue(float value, ExceptionState& exceptionState)
{
    if (isImmutable()) {
        exceptionState.throwDOMException(NoModificationAllowedError, "The attribute is read-only.");
        return;
    }

    if (target()->isRelative() && !canResolveRelativeUnits(contextElement())) {
        exceptionState.throwDOMException(NotSupportedError, "Could not resolve relative length.");
        return;
    }

    SVGLengthContext lengthContext(contextElement());
    target()->setValue(value, lengthContext);
    commitChange();
}

float SVGLengthTearOff::valueInSpecifiedUnits()
{
    return target()->valueInSpecifiedUnits();
}

void SVGLengthTearOff::setValueInSpecifiedUnits(float value, ExceptionState& es)
{
    if (isImmutable()) {
        es.throwDOMException(NoModificationAllowedError, "The attribute is read-only.");
        return;
    }
    target()->setValueInSpecifiedUnits(value);
    commitChange();
}

String SVGLengthTearOff::valueAsString()
{
    // TODO(shanmuga.m@samsung.com): Not all <length> properties have 0 (with no unit) as the default (lacuna) value, Need to return default value instead of 0
    return hasExposedLengthUnit() ? target()->valueAsString() : String::number(0);
}

void SVGLengthTearOff::setValueAsString(const String& str, ExceptionState& es)
{
    if (isImmutable()) {
        es.throwDOMException(NoModificationAllowedError, "The attribute is read-only.");
        return;
    }

    String oldValue = target()->valueAsString();

    target()->setValueAsString(str, es);

    if (!es.hadException() && !hasExposedLengthUnit()) {
        target()->setValueAsString(oldValue, ASSERT_NO_EXCEPTION); // rollback to old value
        es.throwDOMException(SyntaxError, "The value provided ('" + str + "') is invalid.");
        return;
    }

    commitChange();
}

void SVGLengthTearOff::newValueSpecifiedUnits(unsigned short unitType, float valueInSpecifiedUnits, ExceptionState& exceptionState)
{
    if (isImmutable()) {
        exceptionState.throwDOMException(NoModificationAllowedError, "The object is read-only.");
        return;
    }

    if (unitType == LengthTypeUnknown || unitType > LengthTypePC) {
        exceptionState.throwDOMException(NotSupportedError, "Cannot set value with unknown or invalid units (" + String::number(unitType) + ").");
        return;
    }

    target()->newValueSpecifiedUnits(toSVGLengthType(unitType), valueInSpecifiedUnits);
    commitChange();
}

void SVGLengthTearOff::convertToSpecifiedUnits(unsigned short unitType, ExceptionState& exceptionState)
{
    if (isImmutable()) {
        exceptionState.throwDOMException(NoModificationAllowedError, "The object is read-only.");
        return;
    }

    if (unitType == LengthTypeUnknown || unitType > LengthTypePC) {
        exceptionState.throwDOMException(NotSupportedError, "Cannot convert to unknown or invalid units (" + String::number(unitType) + ").");
        return;
    }

    if ((target()->isRelative() || SVGLength::isRelativeUnit(toSVGLengthType(unitType)))
        && !canResolveRelativeUnits(contextElement())) {
        exceptionState.throwDOMException(NotSupportedError, "Could not resolve relative length.");
        return;
    }

    SVGLengthContext lengthContext(contextElement());
    target()->convertToSpecifiedUnits(toSVGLengthType(unitType), lengthContext);
    commitChange();
}

SVGLengthTearOff::SVGLengthTearOff(PassRefPtrWillBeRawPtr<SVGLength> target, SVGElement* contextElement, PropertyIsAnimValType propertyIsAnimVal, const QualifiedName& attributeName)
    : SVGPropertyTearOff<SVGLength>(target, contextElement, propertyIsAnimVal, attributeName)
{
}

}
