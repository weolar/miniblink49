/*
 * Copyright (C) 2004, 2005, 2006, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
 * Copyright (C) 2010 Renata Hodovan <reni@inf.u-szeged.hu>
 * Copyright (C) 2011 Gabor Loki <loki@webkit.org>
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

#include "config.h"
#include "platform/graphics/filters/FETurbulence.h"

#include "SkPerlinNoiseShader.h"
#include "SkRectShaderImageFilter.h"
#include "platform/graphics/filters/Filter.h"
#include "platform/graphics/filters/SkiaImageFilterBuilder.h"
#include "platform/text/TextStream.h"

namespace blink {

FETurbulence::FETurbulence(Filter* filter, TurbulenceType type, float baseFrequencyX, float baseFrequencyY, int numOctaves, float seed, bool stitchTiles)
    : FilterEffect(filter)
    , m_type(type)
    , m_baseFrequencyX(baseFrequencyX)
    , m_baseFrequencyY(baseFrequencyY)
    , m_numOctaves(numOctaves)
    , m_seed(seed)
    , m_stitchTiles(stitchTiles)
{
}

PassRefPtrWillBeRawPtr<FETurbulence> FETurbulence::create(Filter* filter, TurbulenceType type, float baseFrequencyX, float baseFrequencyY, int numOctaves, float seed, bool stitchTiles)
{
    return adoptRefWillBeNoop(new FETurbulence(filter, type, baseFrequencyX, baseFrequencyY, numOctaves, seed, stitchTiles));
}

TurbulenceType FETurbulence::type() const
{
    return m_type;
}

bool FETurbulence::setType(TurbulenceType type)
{
    if (m_type == type)
        return false;
    m_type = type;
    return true;
}

float FETurbulence::baseFrequencyY() const
{
    return m_baseFrequencyY;
}

bool FETurbulence::setBaseFrequencyY(float baseFrequencyY)
{
    if (m_baseFrequencyY == baseFrequencyY)
        return false;
    m_baseFrequencyY = baseFrequencyY;
    return true;
}

float FETurbulence::baseFrequencyX() const
{
    return m_baseFrequencyX;
}

bool FETurbulence::setBaseFrequencyX(float baseFrequencyX)
{
    if (m_baseFrequencyX == baseFrequencyX)
        return false;
    m_baseFrequencyX = baseFrequencyX;
    return true;
}

float FETurbulence::seed() const
{
    return m_seed;
}

bool FETurbulence::setSeed(float seed)
{
    if (m_seed == seed)
        return false;
    m_seed = seed;
    return true;
}

int FETurbulence::numOctaves() const
{
    return m_numOctaves;
}

bool FETurbulence::setNumOctaves(int numOctaves)
{
    if (m_numOctaves == numOctaves)
        return false;
    m_numOctaves = numOctaves;
    return true;
}

bool FETurbulence::stitchTiles() const
{
    return m_stitchTiles;
}

bool FETurbulence::setStitchTiles(bool stitch)
{
    if (m_stitchTiles == stitch)
        return false;
    m_stitchTiles = stitch;
    return true;
}

SkShader* FETurbulence::createShader()
{
    const SkISize size = SkISize::Make(effectBoundaries().width(), effectBoundaries().height());
    // Frequency should be scaled by page zoom, but not by primitiveUnits.
    // So we apply only the transform scale (as Filter::apply*Scale() do)
    // and not the target bounding box scale (as SVGFilter::apply*Scale()
    // would do). Note also that we divide by the scale since this is
    // a frequency, not a period.
    float baseFrequencyX = m_baseFrequencyX / filter()->scale();
    float baseFrequencyY = m_baseFrequencyY / filter()->scale();
    return (type() == FETURBULENCE_TYPE_FRACTALNOISE) ?
        SkPerlinNoiseShader::CreateFractalNoise(SkFloatToScalar(baseFrequencyX),
            SkFloatToScalar(baseFrequencyY), numOctaves(), SkFloatToScalar(seed()),
            stitchTiles() ? &size : 0) :
        SkPerlinNoiseShader::CreateTurbulence(SkFloatToScalar(baseFrequencyX),
            SkFloatToScalar(baseFrequencyY), numOctaves(), SkFloatToScalar(seed()),
            stitchTiles() ? &size : 0);
}

PassRefPtr<SkImageFilter> FETurbulence::createImageFilter(SkiaImageFilterBuilder* builder)
{
    SkAutoTUnref<SkShader> shader(createShader());
    SkImageFilter::CropRect rect = getCropRect(builder->cropOffset());
    return adoptRef(SkRectShaderImageFilter::Create(shader, &rect));
}

static TextStream& operator<<(TextStream& ts, const TurbulenceType& type)
{
    switch (type) {
    case FETURBULENCE_TYPE_UNKNOWN:
        ts << "UNKNOWN";
        break;
    case FETURBULENCE_TYPE_TURBULENCE:
        ts << "TURBULENCE";
        break;
    case FETURBULENCE_TYPE_FRACTALNOISE:
        ts << "NOISE";
        break;
    }
    return ts;
}

TextStream& FETurbulence::externalRepresentation(TextStream& ts, int indent) const
{
    writeIndent(ts, indent);
    ts << "[feTurbulence";
    FilterEffect::externalRepresentation(ts);
    ts << " type=\"" << type() << "\" "
       << "baseFrequency=\"" << baseFrequencyX() << ", " << baseFrequencyY() << "\" "
       << "seed=\"" << seed() << "\" "
       << "numOctaves=\"" << numOctaves() << "\" "
       << "stitchTiles=\"" << stitchTiles() << "\"]\n";
    return ts;
}

} // namespace blink
