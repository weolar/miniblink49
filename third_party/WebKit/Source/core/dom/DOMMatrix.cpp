// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/DOMMatrix.h"

namespace blink {

DOMMatrix* DOMMatrix::create()
{
    return new DOMMatrix(TransformationMatrix());
}

DOMMatrix* DOMMatrix::create(DOMMatrixReadOnly* other)
{
    return new DOMMatrix(other->matrix(), other->is2D());
}

DOMMatrix::DOMMatrix(const TransformationMatrix& matrix, bool is2D)
{
    m_matrix = matrix;
    m_is2D = is2D;
}

void DOMMatrix::setIs2D(bool value)
{
    if (m_is2D)
        m_is2D = value;
}

DOMMatrix* DOMMatrix::multiplySelf(DOMMatrix* other)
{
    if (!other->is2D())
        m_is2D = false;

    m_matrix = m_matrix * other->matrix();

    return this;
}

DOMMatrix* DOMMatrix::preMultiplySelf(DOMMatrix* other)
{
    if (!other->is2D())
        m_is2D = false;

    m_matrix = other->matrix() * m_matrix;

    return this;
}

DOMMatrix* DOMMatrix::translateSelf(double tx, double ty, double tz)
{
    if (!tx && !ty && !tz)
        return this;

    if (tz)
        m_is2D = false;

    if (m_is2D)
        m_matrix.translate(tx, ty);
    else
        m_matrix.translate3d(tx, ty, tz);

    return this;
}

DOMMatrix* DOMMatrix::scaleSelf(double scale, double ox, double oy)
{
    return scaleNonUniformSelf(scale, scale, 1, ox, oy);
}

DOMMatrix* DOMMatrix::scale3dSelf(double scale, double ox, double oy, double oz)
{
    return scaleNonUniformSelf(scale, scale, scale, ox, oy, oz);
}

DOMMatrix* DOMMatrix::scaleNonUniformSelf(double sx, double sy, double sz,
    double ox, double oy, double oz)
{
    if (sz != 1 || oz)
        m_is2D = false;

    if (sx == 1 && sy == 1 && sz == 1)
        return this;

    bool hasTranslation = (ox || oy || oz);

    if (hasTranslation)
        translateSelf(ox, oy, oz);

    if (m_is2D)
        m_matrix.scaleNonUniform(sx, sy);
    else
        m_matrix.scale3d(sx, sy, sz);

    if (hasTranslation)
        translateSelf(-ox, -oy, -oz);

    return this;
}

} // namespace blink
