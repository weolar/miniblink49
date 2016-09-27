/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Graham Dennis (graham.dennis@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef ShadowData_h
#define ShadowData_h

#include "core/css/StyleColor.h"
#include "platform/geometry/FloatPoint.h"
#include "platform/geometry/FloatRectOutsets.h"

namespace blink {

enum ShadowStyle { Normal, Inset };

// This class holds information about shadows for the text-shadow and box-shadow properties.
class ShadowData {
    WTF_MAKE_FAST_ALLOCATED(ShadowData);
public:
    ShadowData(const FloatPoint& location, float blur, float spread, ShadowStyle style, StyleColor color)
        : m_location(location)
        , m_blur(blur)
        , m_spread(spread)
        , m_color(color)
        , m_style(style)
    {
    }

    bool operator==(const ShadowData&) const;
    bool operator!=(const ShadowData& o) const { return !(*this == o); }

    ShadowData blend(const ShadowData& from, double progress, const Color& currentColor) const;

    float x() const { return m_location.x(); }
    float y() const { return m_location.y(); }
    FloatPoint location() const { return m_location; }
    float blur() const { return m_blur; }
    float spread() const { return m_spread; }
    ShadowStyle style() const { return m_style; }
    StyleColor color() const { return m_color; }

    // Outsets needed to adjust a source rectangle to the one cast by this
    // shadow.
    FloatRectOutsets rectOutsets() const
    {
        float blurAndSpread = blur() + spread();
        return FloatRectOutsets(
            blurAndSpread - y() /* top */,
            blurAndSpread + x() /* right */,
            blurAndSpread + y() /* bottom */,
            blurAndSpread - x() /* left */);
    }

private:
    FloatPoint m_location;
    float m_blur;
    float m_spread;
    StyleColor m_color;
    ShadowStyle m_style;
};

} // namespace blink

#endif // ShadowData_h
