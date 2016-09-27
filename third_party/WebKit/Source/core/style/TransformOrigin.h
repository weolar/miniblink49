// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TransformOrigin_h
#define TransformOrigin_h

#include "platform/Length.h"

namespace blink {

class TransformOrigin {
public:
    TransformOrigin(const Length& x, const Length& y, float z) : m_x(x), m_y(y), m_z(z) { }
    bool operator==(const TransformOrigin& o) const { return m_x == o.m_x && m_y == o.m_y && m_z == o.m_z; }
    bool operator!=(const TransformOrigin& o) const { return !(*this == o); }
    const Length& x() const { return m_x; }
    const Length& y() const { return m_y; }
    float z() const { return m_z; }
private:
    Length m_x;
    Length m_y;
    float m_z;
};

} // namespace blink

#endif // TransformOrigin_h
