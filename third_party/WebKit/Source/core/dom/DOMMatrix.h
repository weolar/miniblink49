// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMMatrix_h
#define DOMMatrix_h

#include "core/dom/DOMMatrixReadOnly.h"

namespace blink {

class DOMMatrix : public DOMMatrixReadOnly {
    DEFINE_WRAPPERTYPEINFO();
public:
    static DOMMatrix* create();
    static DOMMatrix* create(DOMMatrixReadOnly*);

    void setA(double value) { m_matrix.setM11(value); }
    void setB(double value) { m_matrix.setM12(value); }
    void setC(double value) { m_matrix.setM21(value); }
    void setD(double value) { m_matrix.setM22(value); }
    void setE(double value) { m_matrix.setM41(value); }
    void setF(double value) { m_matrix.setM42(value); }

    void setM11(double value) { m_matrix.setM11(value); }
    void setM12(double value) { m_matrix.setM12(value); }
    void setM13(double value) { m_matrix.setM13(value); setIs2D(!value); }
    void setM14(double value) { m_matrix.setM14(value); setIs2D(!value); }
    void setM21(double value) { m_matrix.setM21(value); }
    void setM22(double value) { m_matrix.setM22(value); }
    void setM23(double value) { m_matrix.setM23(value); setIs2D(!value); }
    void setM24(double value) { m_matrix.setM24(value); setIs2D(!value); }
    void setM31(double value) { m_matrix.setM31(value); setIs2D(!value); }
    void setM32(double value) { m_matrix.setM32(value); setIs2D(!value); }
    void setM33(double value) { m_matrix.setM33(value); setIs2D(value != 1); }
    void setM34(double value) { m_matrix.setM34(value); setIs2D(!value); }
    void setM41(double value) { m_matrix.setM41(value); }
    void setM42(double value) { m_matrix.setM42(value); }
    void setM43(double value) { m_matrix.setM43(value); setIs2D(!value); }
    void setM44(double value) { m_matrix.setM44(value); setIs2D(value != 1); }

    DOMMatrix* multiplySelf(DOMMatrix*);
    DOMMatrix* preMultiplySelf(DOMMatrix*);
    DOMMatrix* translateSelf(double tx, double ty, double tz = 0);
    DOMMatrix* scaleSelf(double scale, double ox = 0, double oy = 0);
    DOMMatrix* scale3dSelf(double scale, double ox = 0, double oy = 0, double oz = 0);
    DOMMatrix* scaleNonUniformSelf(double sx, double sy = 1, double sz = 1,
        double ox = 0, double oy = 0, double oz = 0);

private:
    DOMMatrix(const TransformationMatrix&, bool is2D = true);

    void setIs2D(bool value);
};

} // namespace blink

#endif
