// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FloatRectOutsets_h
#define FloatRectOutsets_h

#include "platform/PlatformExport.h"
#include <algorithm>

namespace blink {

// Specifies floating-point lengths to be used to expand a rectangle.
// For example, |top()| returns the distance the top edge should be moved
// upward.
//
// Negative lengths can be used to express insets.
class PLATFORM_EXPORT FloatRectOutsets {
public:
    FloatRectOutsets() : m_top(0), m_right(0), m_bottom(0), m_left(0) { }

    FloatRectOutsets(float top, float right, float bottom, float left)
        : m_top(top)
        , m_right(right)
        , m_bottom(bottom)
        , m_left(left)
    {
    }

    float top() const { return m_top; }
    float right() const { return m_right; }
    float bottom() const { return m_bottom; }
    float left() const { return m_left; }

    void setTop(float top) { m_top = top; }
    void setRight(float right) { m_right = right; }
    void setBottom(float bottom) { m_bottom = bottom; }
    void setLeft(float left) { m_left = left; }

    // Change outsets to be at least as large as |other|.
    void unite(const FloatRectOutsets& other)
    {
        m_top = std::max(m_top, other.m_top);
        m_right = std::max(m_right, other.m_right);
        m_bottom = std::max(m_bottom, other.m_bottom);
        m_left = std::max(m_left, other.m_left);
    }

private:
    float m_top;
    float m_right;
    float m_bottom;
    float m_left;
};

} // namespace blink

#endif // FloatRectOutsets_h
