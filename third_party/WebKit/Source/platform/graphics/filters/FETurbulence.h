/*
 * Copyright (C) 2004, 2005, 2006, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
 * Copyright (C) 2010 Renata Hodovan <reni@inf.u-szeged.hu>
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

#ifndef FETurbulence_h
#define FETurbulence_h

#include "platform/graphics/filters/FilterEffect.h"

class SkShader;

namespace blink {

enum TurbulenceType {
    FETURBULENCE_TYPE_UNKNOWN = 0,
    FETURBULENCE_TYPE_FRACTALNOISE = 1,
    FETURBULENCE_TYPE_TURBULENCE = 2
};

class PLATFORM_EXPORT FETurbulence : public FilterEffect {
public:
    static PassRefPtrWillBeRawPtr<FETurbulence> create(Filter*, TurbulenceType, float, float, int, float, bool);

    TurbulenceType type() const;
    bool setType(TurbulenceType);

    float baseFrequencyY() const;
    bool setBaseFrequencyY(float);

    float baseFrequencyX() const;
    bool setBaseFrequencyX(float);

    float seed() const;
    bool setSeed(float);

    int numOctaves() const;
    bool setNumOctaves(int);

    bool stitchTiles() const;
    bool setStitchTiles(bool);

    TextStream& externalRepresentation(TextStream&, int indention) const override;

private:
    FETurbulence(Filter*, TurbulenceType, float, float, int, float, bool);

    PassRefPtr<SkImageFilter> createImageFilter(SkiaImageFilterBuilder*) override;
    SkShader* createShader();

    TurbulenceType m_type;
    float m_baseFrequencyX;
    float m_baseFrequencyY;
    int m_numOctaves;
    float m_seed;
    bool m_stitchTiles;
};

} // namespace blink

#endif // FETurbulence_h
