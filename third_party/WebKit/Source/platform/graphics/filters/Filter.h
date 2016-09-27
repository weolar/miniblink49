/*
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
 */

#ifndef Filter_h
#define Filter_h

#include "platform/PlatformExport.h"
#include "platform/geometry/FloatPoint3D.h"
#include "platform/geometry/FloatRect.h"
#include "platform/geometry/IntRect.h"
#include "platform/heap/Handle.h"
#include "wtf/RefCounted.h"

namespace blink {

class PLATFORM_EXPORT Filter : public RefCountedWillBeGarbageCollectedFinalized<Filter> {
public:
    virtual ~Filter() { }
    DEFINE_INLINE_VIRTUAL_TRACE() { }

    float scale() const { return m_scale; }
    FloatRect mapLocalRectToAbsoluteRect(const FloatRect& rect) const { FloatRect result(rect); result.scale(m_scale); return result; }
    FloatRect mapAbsoluteRectToLocalRect(const FloatRect& rect) const { FloatRect result(rect); result.scale(1.0f / m_scale); return result; }
    virtual float applyHorizontalScale(float value) const { return m_scale * value; }
    virtual float applyVerticalScale(float value) const { return m_scale * value; }

    virtual FloatPoint3D resolve3dPoint(const FloatPoint3D& point) const { return point; }

    virtual IntRect sourceImageRect() const = 0;

    FloatRect absoluteFilterRegion() const { return m_absoluteFilterRegion; }

    FloatRect filterRegion() const { return m_filterRegion; }
    void setFilterRegion(const FloatRect& rect)
    {
        m_filterRegion = rect;
        m_absoluteFilterRegion = rect;
        m_absoluteFilterRegion.scale(m_scale);
    }

protected:
    explicit Filter(float scale)
        : m_scale(scale)
    {
    }

private:
    float m_scale;
    FloatRect m_absoluteFilterRegion;
    FloatRect m_filterRegion;
};

} // namespace blink

#endif // Filter_h
