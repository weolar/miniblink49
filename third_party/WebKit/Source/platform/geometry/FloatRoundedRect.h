/*
 * Copyright (C) 2013 Adobe Systems Incorporated. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FloatRoundedRect_h
#define FloatRoundedRect_h

#include "platform/geometry/FloatRect.h"
#include "platform/geometry/FloatSize.h"
#include "third_party/skia/include/core/SkRRect.h"

namespace blink {

class FloatQuad;

class PLATFORM_EXPORT FloatRoundedRect {
public:
    class PLATFORM_EXPORT Radii {
    public:
        Radii() { }
        Radii(const FloatSize& topLeft, const FloatSize& topRight, const FloatSize& bottomLeft, const FloatSize& bottomRight)
            : m_topLeft(topLeft)
            , m_topRight(topRight)
            , m_bottomLeft(bottomLeft)
            , m_bottomRight(bottomRight)
        {
        }

        Radii(const FloatRoundedRect::Radii& intRadii)
            : m_topLeft(intRadii.topLeft())
            , m_topRight(intRadii.topRight())
            , m_bottomLeft(intRadii.bottomLeft())
            , m_bottomRight(intRadii.bottomRight())
        {
        }

        void setTopLeft(const FloatSize& size) { m_topLeft = size; }
        void setTopRight(const FloatSize& size) { m_topRight = size; }
        void setBottomLeft(const FloatSize& size) { m_bottomLeft = size; }
        void setBottomRight(const FloatSize& size) { m_bottomRight = size; }
        const FloatSize& topLeft() const { return m_topLeft; }
        const FloatSize& topRight() const { return m_topRight; }
        const FloatSize& bottomLeft() const { return m_bottomLeft; }
        const FloatSize& bottomRight() const { return m_bottomRight; }

        bool isZero() const;

        void scale(float factor);
        // Multiply all radii by |factor| and floor the result to the nearest integer.
        void scaleAndFloor(float factor);

        void expand(float topWidth, float bottomWidth, float leftWidth, float rightWidth);
        void expand(float size) { expand(size, size, size, size); }

        void shrink(float topWidth, float bottomWidth, float leftWidth, float rightWidth);
        void shrink(float size) { shrink(size, size, size, size); }

        void includeLogicalEdges(const Radii& edges, bool isHorizontal, bool includeLogicalLeftEdge, bool includeLogicalRightEdge);

#ifndef NDEBUG
        // Prints debugging information.
        void show();
#endif

    private:
        FloatSize m_topLeft;
        FloatSize m_topRight;
        FloatSize m_bottomLeft;
        FloatSize m_bottomRight;
    };

    FloatRoundedRect() { }
    explicit FloatRoundedRect(const FloatRect&, const Radii& = Radii());
    FloatRoundedRect(float x, float y, float width, float height);
    FloatRoundedRect(const FloatRect&, const FloatSize& topLeft, const FloatSize& topRight, const FloatSize& bottomLeft, const FloatSize& bottomRight);

    const FloatRect& rect() const { return m_rect; }
    const Radii& radii() const { return m_radii; }
    bool isRounded() const { return !m_radii.isZero(); }
    bool isEmpty() const { return m_rect.isEmpty(); }

    void setRect(const FloatRect& rect) { m_rect = rect; }
    void setRadii(const Radii& radii) { m_radii = radii; }

    void move(const FloatSize& size) { m_rect.move(size); }
    void inflateWithRadii(int size);
    void inflate(float size) { m_rect.inflate(size); }

    // expandRadii() does not have any effect on corner radii which have zero width or height. This is because the process of expanding
    // the radius of a corner is not allowed to make sharp corners non-sharp. This applies when "spreading" a shadow or
    // a box shape.
    void expandRadii(float size) { m_radii.expand(size); }
    void shrinkRadii(float size) { m_radii.shrink(size); }

    // Returns a quickly computed rect enclosed by the rounded rect.
    FloatRect radiusCenterRect() const;

    FloatRect topLeftCorner() const
    {
        return FloatRect(m_rect.x(), m_rect.y(), m_radii.topLeft().width(), m_radii.topLeft().height());
    }
    FloatRect topRightCorner() const
    {
        return FloatRect(m_rect.maxX() - m_radii.topRight().width(), m_rect.y(), m_radii.topRight().width(), m_radii.topRight().height());
    }
    FloatRect bottomLeftCorner() const
    {
        return FloatRect(m_rect.x(), m_rect.maxY() - m_radii.bottomLeft().height(), m_radii.bottomLeft().width(), m_radii.bottomLeft().height());
    }
    FloatRect bottomRightCorner() const
    {
        return FloatRect(m_rect.maxX() - m_radii.bottomRight().width(), m_rect.maxY() - m_radii.bottomRight().height(), m_radii.bottomRight().width(), m_radii.bottomRight().height());
    }

    bool xInterceptsAtY(float y, float& minXIntercept, float& maxXIntercept) const;

    void includeLogicalEdges(const Radii& edges, bool isHorizontal, bool includeLogicalLeftEdge, bool includeLogicalRightEdge);

    // Tests whether the quad intersects any part of this rounded rectangle.
    // This only works for convex quads.
    bool intersectsQuad(const FloatQuad&) const;

    void adjustRadii();
    bool isRenderable() const;

#ifndef NDEBUG
    // Prints debugging information.
    void show();
#endif

    // Constrains the radii to be no more than the size of rect(); radii outside of this range are not
    // defined.
    // In addition, the radii of the corners are floored to the nearest integer.
    // FIXME: the flooring should not be necessary. At the moment it causes background bleed in some cases.
    // FIXME: this code is almost the same as adjustRadii()/isRenderable(). Get rid of one of them.
    void constrainRadii();

    operator SkRRect() const;

private:
    FloatRect m_rect;
    Radii m_radii;
};

inline FloatRoundedRect::operator SkRRect() const
{
    SkRRect rrect;

    if (isRounded()) {
        SkVector radii[4];
        radii[SkRRect::kUpperLeft_Corner].set(topLeftCorner().width(), topLeftCorner().height());
        radii[SkRRect::kUpperRight_Corner].set(topRightCorner().width(), topRightCorner().height());
        radii[SkRRect::kLowerRight_Corner].set(bottomRightCorner().width(), bottomRightCorner().height());
        radii[SkRRect::kLowerLeft_Corner].set(bottomLeftCorner().width(), bottomLeftCorner().height());

        rrect.setRectRadii(rect(), radii);
    } else {
        rrect.setRect(rect());
    }

    return rrect;
}

inline bool operator==(const FloatRoundedRect::Radii& a, const FloatRoundedRect::Radii& b)
{
    return a.topLeft() == b.topLeft() && a.topRight() == b.topRight() && a.bottomLeft() == b.bottomLeft() && a.bottomRight() == b.bottomRight();
}


inline bool operator!=(const FloatRoundedRect::Radii& a, const FloatRoundedRect::Radii& b)
{
    return !(a == b);
}


inline bool operator==(const FloatRoundedRect& a, const FloatRoundedRect& b)
{
    return a.rect() == b.rect() && a.radii() == b.radii();
}

} // namespace blink

#endif // FloatRoundedRect_h
