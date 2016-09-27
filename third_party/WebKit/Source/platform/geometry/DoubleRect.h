// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DoubleRect_h
#define DoubleRect_h

#include "platform/geometry/DoublePoint.h"
#include "platform/geometry/DoubleSize.h"

namespace blink {

class FloatRect;
class IntRect;
class LayoutRect;

class PLATFORM_EXPORT DoubleRect {
public:
    DoubleRect() { }
    DoubleRect(const DoublePoint& location, const DoubleSize& size)
        : m_location(location), m_size(size) { }
    DoubleRect(double x, double y, double width, double height)
        : m_location(DoublePoint(x, y)), m_size(DoubleSize(width, height)) { }
    DoubleRect(const IntRect&);
    DoubleRect(const FloatRect&);
    DoubleRect(const LayoutRect&);

    DoublePoint location() const { return m_location; }
    DoubleSize size() const { return m_size; }

    void setLocation(const DoublePoint& location) { m_location = location; }
    void setSize(const DoubleSize& size) { m_size = size; }

    double x() const { return m_location.x(); }
    double y() const { return m_location.y(); }
    double maxX() const { return x() + width(); }
    double maxY() const { return y() + height(); }
    double width() const { return m_size.width(); }
    double height() const { return m_size.height(); }

    void setX(double x) { m_location.setX(x); }
    void setY(double y) { m_location.setY(y); }
    void setWidth(double width) { m_size.setWidth(width); }
    void setHeight(double height) { m_size.setHeight(height); }

    bool isEmpty() const { return m_size.isEmpty(); }
    bool isZero() const { return m_size.isZero(); }

    void move(const DoubleSize& delta) { m_location += delta; }
    void move(double dx, double dy) { m_location.move(dx, dy); }
    void moveBy(const DoublePoint& delta) { m_location.move(delta.x(), delta.y()); }

    DoublePoint minXMinYCorner() const { return m_location; } // typically topLeft
    DoublePoint maxXMinYCorner() const { return DoublePoint(m_location.x() + m_size.width(), m_location.y()); } // typically topRight
    DoublePoint minXMaxYCorner() const { return DoublePoint(m_location.x(), m_location.y() + m_size.height()); } // typically bottomLeft
    DoublePoint maxXMaxYCorner() const { return DoublePoint(m_location.x() + m_size.width(), m_location.y() + m_size.height()); } // typically bottomRight

    void scale(float s) { scale(s, s); }
    void scale(float sx, float sy);
private:
    DoublePoint m_location;
    DoubleSize m_size;
};

PLATFORM_EXPORT IntRect enclosingIntRect(const DoubleRect&);

// Returns a valid IntRect contained within the given DoubleRect.
PLATFORM_EXPORT IntRect enclosedIntRect(const DoubleRect&);

PLATFORM_EXPORT IntRect roundedIntRect(const DoubleRect&);

}

#endif
