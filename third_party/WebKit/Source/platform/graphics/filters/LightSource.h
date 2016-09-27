/*
 * Copyright (C) 2008 Alex Mathews <possessedpenguinbob@gmail.com>
 * Copyright (C) 2004, 2005, 2006, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2010 Zoltan Herczeg <zherczeg@webkit.org>
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

#ifndef LightSource_h
#define LightSource_h

#include "platform/PlatformExport.h"
#include "platform/geometry/FloatPoint3D.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

enum LightType {
    LS_DISTANT,
    LS_POINT,
    LS_SPOT
};

class TextStream;

class PLATFORM_EXPORT LightSource : public RefCounted<LightSource> {
public:
    LightSource(LightType type)
        : m_type(type)
    { }

    virtual ~LightSource();

    LightType type() const { return m_type; }
    virtual TextStream& externalRepresentation(TextStream&) const = 0;

    virtual PassRefPtr<LightSource> create(const FloatPoint3D& scale, const FloatSize& offset) const = 0;

    virtual bool setAzimuth(float) { return false; }
    virtual bool setElevation(float) { return false; }
    virtual bool setPosition(const FloatPoint3D&) { return false; }
    virtual bool setPointsAt(const FloatPoint3D&) { return false; }
    virtual bool setSpecularExponent(float) { return false; }
    virtual bool setLimitingConeAngle(float) { return false; }

private:
    LightType m_type;
};

} // namespace blink

#endif // LightSource_h
