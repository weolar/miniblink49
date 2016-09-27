/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2006, 2007, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Holger Hans Peter Freyther
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

#ifndef GlyphOverflow_h
#define GlyphOverflow_h

#include "platform/geometry/FloatRect.h"
#include <math.h>

namespace blink {

struct GlyphOverflow {
    GlyphOverflow()
        : left(0)
        , right(0)
        , top(0)
        , bottom(0)
        , computeBounds(false)
    {
    }

    bool isZero() const
    {
        return !left && !right && !top && !bottom;
    }

    void setFromBounds(const FloatRect& bounds, float ascent, float descent, float textWidth)
    {
        top = ceilf(computeBounds ? -bounds.y() : std::max(0.0f, -bounds.y() - ascent));
        bottom = ceilf(computeBounds ? bounds.maxY() : std::max(0.0f, bounds.maxY() - descent));
        left = ceilf(std::max(0.0f, -bounds.x()));
        right = ceilf(std::max(0.0f, bounds.maxX() - textWidth));
    }

    // If computeBounds, top and bottom are the maximum heights of the glyphs above and below the baseline, respectively.
    // Otherwise they are the amounts of glyph overflows exceeding the font metrics' ascent and descent, respectively.
    // Left and right are the amounts of glyph overflows exceeding the left and right edge of normal layout boundary, respectively.
    // All fields are in absolute number of pixels rounded up to the nearest integer.
    int left;
    int right;
    int top;
    int bottom;
    bool computeBounds;
};

} // namespace blink

#endif // GlyphOverflow_h
