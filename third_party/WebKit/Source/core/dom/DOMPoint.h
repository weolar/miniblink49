// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMPoint_h
#define DOMPoint_h

#include "core/CoreExport.h"
#include "core/dom/DOMPointReadOnly.h"

namespace blink {

class DOMPointInit;

class CORE_EXPORT DOMPoint final : public DOMPointReadOnly {
    DEFINE_WRAPPERTYPEINFO();
public:
    static DOMPoint* create(const DOMPointInit&);
    static DOMPoint* create(double x, double y, double z = 0, double w = 1);

    void setX(double x) { m_x = x; }
    void setY(double y) { m_y = y; }
    void setZ(double z) { m_z = z; }
    void setW(double w) { m_w = w; }

protected:
    DOMPoint(double x, double y, double z, double w);
};

} // namespace blink

#endif
