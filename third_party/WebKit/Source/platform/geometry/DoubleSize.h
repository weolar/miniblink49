// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DoubleSize_h
#define DoubleSize_h

#include "platform/geometry/FloatSize.h"
#include "platform/geometry/IntSize.h"
#include "wtf/MathExtras.h"

namespace blink {

class LayoutSize;

class PLATFORM_EXPORT DoubleSize {
public:
    DoubleSize() : m_width(0), m_height(0) { }
    DoubleSize(double width, double height) : m_width(width), m_height(height) { }
    DoubleSize(const IntSize& p) : m_width(p.width()), m_height(p.height()) { }
    DoubleSize(const FloatSize& s) : m_width(s.width()), m_height(s.height()) { }
    explicit DoubleSize(const LayoutSize&);

    double width() const { return m_width; }
    double height() const { return m_height; }

    void setWidth(double width) { m_width = width; }
    void setHeight(double height) { m_height = height; }

    bool isEmpty() const { return m_width <= 0 || m_height <= 0; }

    bool isZero() const;

    void expand(float width, float height)
    {
        m_width += width;
        m_height += height;
    }

    void scale(float widthScale, float heightScale)
    {
        m_width = m_width * widthScale;
        m_height = m_height * heightScale;
    }

    void scale(float scale)
    {
        this->scale(scale, scale);
    }

private:
    double m_width, m_height;
};

inline DoubleSize& operator+=(DoubleSize& a, const DoubleSize& b)
{
    a.setWidth(a.width() + b.width());
    a.setHeight(a.height() + b.height());
    return a;
}

inline DoubleSize& operator-=(DoubleSize& a, const DoubleSize& b)
{
    a.setWidth(a.width() - b.width());
    a.setHeight(a.height() - b.height());
    return a;
}

inline DoubleSize operator+(const DoubleSize& a, const DoubleSize& b)
{
    return DoubleSize(a.width() + b.width(), a.height() + b.height());
}

inline DoubleSize operator-(const DoubleSize& a, const DoubleSize& b)
{
    return DoubleSize(a.width() - b.width(), a.height() - b.height());
}

inline bool operator==(const DoubleSize& a, const DoubleSize& b)
{
    return a.width() == b.width() && a.height() == b.height();
}

inline bool operator!=(const DoubleSize& a, const DoubleSize& b)
{
    return a.width() != b.width() || a.height() != b.height();
}

inline IntSize flooredIntSize(const DoubleSize& p)
{
    return IntSize(clampTo<int>(floor(p.width())), clampTo<int>(floor(p.height())));
}

inline IntSize roundedIntSize(const DoubleSize& p)
{
    return IntSize(clampTo<int>(roundf(p.width())), clampTo<int>(roundf(p.height())));
}

inline IntSize expandedIntSize(const DoubleSize& p)
{
    return IntSize(clampTo<int>(ceilf(p.width())), clampTo<int>(ceilf(p.height())));
}

inline FloatSize toFloatSize(const DoubleSize& p)
{
    return FloatSize(p.width(), p.height());
}

} // namespace blink

#endif // DoubleSize_h
