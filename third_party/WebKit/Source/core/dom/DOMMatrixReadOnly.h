// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMMatrixReadOnly_h
#define DOMMatrixReadOnly_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/DOMTypedArray.h"
#include "platform/heap/Handle.h"
#include "platform/transforms/TransformationMatrix.h"

namespace blink {

class DOMMatrix;

class DOMMatrixReadOnly : public GarbageCollected<DOMMatrixReadOnly>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    double a() const { return m_matrix.m11(); }
    double b() const { return m_matrix.m12(); }
    double c() const { return m_matrix.m21(); }
    double d() const { return m_matrix.m22(); }
    double e() const { return m_matrix.m41(); }
    double f() const { return m_matrix.m42(); }

    double m11() const { return m_matrix.m11(); }
    double m12() const { return m_matrix.m12(); }
    double m13() const { return m_matrix.m13(); }
    double m14() const { return m_matrix.m14(); }
    double m21() const { return m_matrix.m21(); }
    double m22() const { return m_matrix.m22(); }
    double m23() const { return m_matrix.m23(); }
    double m24() const { return m_matrix.m24(); }
    double m31() const { return m_matrix.m31(); }
    double m32() const { return m_matrix.m32(); }
    double m33() const { return m_matrix.m33(); }
    double m34() const { return m_matrix.m34(); }
    double m41() const { return m_matrix.m41(); }
    double m42() const { return m_matrix.m42(); }
    double m43() const { return m_matrix.m43(); }
    double m44() const { return m_matrix.m44(); }

    bool is2D() const;
    bool isIdentity() const;

    DOMMatrix* multiply(DOMMatrix*);
    DOMMatrix* translate(double tx, double ty, double tz = 0);
    DOMMatrix* scale(double scale, double ox = 0, double oy = 0);
    DOMMatrix* scale3d(double scale, double ox = 0, double oy = 0, double oz = 0);
    DOMMatrix* scaleNonUniform(double sx, double sy = 1, double sz = 1,
        double ox = 0, double oy = 0, double oz = 0);

    PassRefPtr<DOMFloat32Array> toFloat32Array() const;
    PassRefPtr<DOMFloat64Array> toFloat64Array() const;

    const TransformationMatrix& matrix() const { return m_matrix; }

    DEFINE_INLINE_TRACE() { }

protected:
    TransformationMatrix m_matrix;
    bool m_is2D;
};

} // namespace blink

#endif
