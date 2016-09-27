/*
 * Copyright (C) 2010 University of Szeged
 * Copyright (C) 2010 Zoltan Herczeg
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY UNIVERSITY OF SZEGED ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL UNIVERSITY OF SZEGED OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FELighting_h
#define FELighting_h

#include "platform/graphics/Color.h"
#include "platform/graphics/filters/FilterEffect.h"

// Common base class for FEDiffuseLighting and FESpecularLighting

namespace blink {

class LightSource;

class PLATFORM_EXPORT FELighting : public FilterEffect {
public:
    PassRefPtr<SkImageFilter> createImageFilter(SkiaImageFilterBuilder*) override;

protected:
    enum LightingType {
        DiffuseLighting,
        SpecularLighting
    };

    FloatRect mapPaintRect(const FloatRect&, bool forward = true) final;
    bool affectsTransparentPixels() override { return true; }

    FELighting(Filter*, LightingType, const Color&, float, float, float, float, float, float, PassRefPtr<LightSource>);

    LightingType m_lightingType;
    RefPtr<LightSource> m_lightSource;

    Color m_lightingColor;
    float m_surfaceScale;
    float m_diffuseConstant;
    float m_specularConstant;
    float m_specularExponent;
    float m_kernelUnitLengthX;
    float m_kernelUnitLengthY;
};

} // namespace blink

#endif // FELighting_h
