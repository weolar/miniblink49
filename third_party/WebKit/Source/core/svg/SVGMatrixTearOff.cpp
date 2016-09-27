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
#include "core/svg/SVGMatrixTearOff.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/svg/SVGElement.h"
#include "core/svg/SVGTransformTearOff.h"

namespace blink {

SVGMatrixTearOff::SVGMatrixTearOff(const AffineTransform& staticValue)
    : m_staticValue(staticValue)
    , m_contextTransform(nullptr)
{
}

SVGMatrixTearOff::SVGMatrixTearOff(SVGTransformTearOff* transform)
    : m_contextTransform(transform)
{
    ASSERT(transform);
}

SVGMatrixTearOff::~SVGMatrixTearOff()
{
}

DEFINE_TRACE(SVGMatrixTearOff)
{
    visitor->trace(m_contextTransform);
}

const AffineTransform& SVGMatrixTearOff::value() const
{
    return m_contextTransform ? m_contextTransform->target()->matrix() : m_staticValue;
}

AffineTransform* SVGMatrixTearOff::mutableValue()
{
    return m_contextTransform ? m_contextTransform->target()->mutableMatrix() : &m_staticValue;
}

void SVGMatrixTearOff::commitChange()
{
    if (!m_contextTransform)
        return;

    m_contextTransform->target()->onMatrixChange();
    m_contextTransform->commitChange();
}

#define DEFINE_SETTER(ATTRIBUTE) \
    void SVGMatrixTearOff::set##ATTRIBUTE(double f, ExceptionState& exceptionState) \
    { \
        if (m_contextTransform && m_contextTransform->isImmutable()) { \
            exceptionState.throwDOMException(NoModificationAllowedError, "The attribute is read-only."); \
            return; \
        } \
        mutableValue()->set##ATTRIBUTE(f); \
        commitChange(); \
    }

DEFINE_SETTER(A);
DEFINE_SETTER(B);
DEFINE_SETTER(C);
DEFINE_SETTER(D);
DEFINE_SETTER(E);
DEFINE_SETTER(F);

#undef DEFINE_SETTER

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGMatrixTearOff::translate(double tx, double ty)
{
    RefPtrWillBeRawPtr<SVGMatrixTearOff> matrix = create(value());
    matrix->mutableValue()->translate(tx, ty);
    return matrix.release();
}

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGMatrixTearOff::scale(double s)
{
    RefPtrWillBeRawPtr<SVGMatrixTearOff> matrix = create(value());
    matrix->mutableValue()->scale(s, s);
    return matrix.release();
}

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGMatrixTearOff::scaleNonUniform(double sx, double sy)
{
    RefPtrWillBeRawPtr<SVGMatrixTearOff> matrix = create(value());
    matrix->mutableValue()->scale(sx, sy);
    return matrix.release();
}

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGMatrixTearOff::rotate(double d)
{
    RefPtrWillBeRawPtr<SVGMatrixTearOff> matrix = create(value());
    matrix->mutableValue()->rotate(d);
    return matrix.release();
}

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGMatrixTearOff::flipX()
{
    RefPtrWillBeRawPtr<SVGMatrixTearOff> matrix = create(value());
    matrix->mutableValue()->flipX();
    return matrix.release();
}

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGMatrixTearOff::flipY()
{
    RefPtrWillBeRawPtr<SVGMatrixTearOff> matrix = create(value());
    matrix->mutableValue()->flipY();
    return matrix.release();
}

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGMatrixTearOff::skewX(double angle)
{
    RefPtrWillBeRawPtr<SVGMatrixTearOff> matrix = create(value());
    matrix->mutableValue()->skewX(angle);
    return matrix.release();
}

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGMatrixTearOff::skewY(double angle)
{
    RefPtrWillBeRawPtr<SVGMatrixTearOff> matrix = create(value());
    matrix->mutableValue()->skewY(angle);
    return matrix.release();
}

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGMatrixTearOff::multiply(PassRefPtrWillBeRawPtr<SVGMatrixTearOff> other)
{
    RefPtrWillBeRawPtr<SVGMatrixTearOff> matrix = create(value());
    *matrix->mutableValue() *= other->value();
    return matrix.release();
}

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGMatrixTearOff::inverse(ExceptionState& exceptionState)
{
    AffineTransform transform = value().inverse();
    if (!value().isInvertible())
        exceptionState.throwDOMException(InvalidStateError, "The matrix is not invertible.");

    return create(transform);
}

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGMatrixTearOff::rotateFromVector(double x, double y, ExceptionState& exceptionState)
{
    if (!x || !y)
        exceptionState.throwDOMException(InvalidAccessError, "Arguments cannot be zero.");

    AffineTransform copy = value();
    copy.rotateFromVector(x, y);
    return create(copy);
}

}
