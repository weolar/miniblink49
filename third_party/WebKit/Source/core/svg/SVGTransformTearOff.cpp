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
#include "core/svg/SVGTransformTearOff.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/svg/SVGElement.h"

namespace blink {

SVGTransformTearOff::SVGTransformTearOff(PassRefPtrWillBeRawPtr<SVGTransform> target, SVGElement* contextElement, PropertyIsAnimValType propertyIsAnimVal, const QualifiedName& attributeName)
    : SVGPropertyTearOff<SVGTransform>(target, contextElement, propertyIsAnimVal, attributeName)
{
}

SVGTransformTearOff::~SVGTransformTearOff()
{
}

DEFINE_TRACE(SVGTransformTearOff)
{
    visitor->trace(m_matrixTearoff);
    SVGPropertyTearOff<SVGTransform>::trace(visitor);
}

SVGMatrixTearOff* SVGTransformTearOff::matrix()
{
    if (!m_matrixTearoff) {
        m_matrixTearoff = SVGMatrixTearOff::create(this);
    }

    return m_matrixTearoff.get();
}

void SVGTransformTearOff::setMatrix(PassRefPtrWillBeRawPtr<SVGMatrixTearOff> matrix, ExceptionState& exceptionState)
{
    if (isImmutable()) {
        exceptionState.throwDOMException(NoModificationAllowedError, "The attribute is read-only.");
        return;
    }

    target()->setMatrix(matrix->value());
    commitChange();
}

void SVGTransformTearOff::setTranslate(float tx, float ty, ExceptionState& exceptionState)
{
    if (isImmutable()) {
        exceptionState.throwDOMException(NoModificationAllowedError, "The attribute is read-only.");
        return;
    }

    target()->setTranslate(tx, ty);
    commitChange();
}

void SVGTransformTearOff::setScale(float sx, float sy, ExceptionState& exceptionState)
{
    if (isImmutable()) {
        exceptionState.throwDOMException(NoModificationAllowedError, "The attribute is read-only.");
        return;
    }

    target()->setScale(sx, sy);
    commitChange();
}

void SVGTransformTearOff::setRotate(float angle, float cx, float cy, ExceptionState& exceptionState)
{
    if (isImmutable()) {
        exceptionState.throwDOMException(NoModificationAllowedError, "The attribute is read-only.");
        return;
    }

    target()->setRotate(angle, cx, cy);
    commitChange();
}

void SVGTransformTearOff::setSkewX(float x, ExceptionState& exceptionState)
{
    if (isImmutable()) {
        exceptionState.throwDOMException(NoModificationAllowedError, "The attribute is read-only.");
        return;
    }

    target()->setSkewX(x);
    commitChange();
}

void SVGTransformTearOff::setSkewY(float y, ExceptionState& exceptionState)
{
    if (isImmutable()) {
        exceptionState.throwDOMException(NoModificationAllowedError, "The attribute is read-only.");
        return;
    }

    target()->setSkewY(y);
    commitChange();
}

}
