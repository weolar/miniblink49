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

#include "config.h"
#include "platform/graphics/filters/FELighting.h"

#include "SkLightingImageFilter.h"
#include "platform/graphics/filters/DistantLightSource.h"
#include "platform/graphics/filters/PointLightSource.h"
#include "platform/graphics/filters/SkiaImageFilterBuilder.h"
#include "platform/graphics/filters/SpotLightSource.h"

namespace blink {

FELighting::FELighting(Filter* filter, LightingType lightingType, const Color& lightingColor, float surfaceScale,
    float diffuseConstant, float specularConstant, float specularExponent,
    float kernelUnitLengthX, float kernelUnitLengthY, PassRefPtr<LightSource> lightSource)
    : FilterEffect(filter)
    , m_lightingType(lightingType)
    , m_lightSource(lightSource)
    , m_lightingColor(lightingColor)
    , m_surfaceScale(surfaceScale)
    , m_diffuseConstant(std::max(diffuseConstant, 0.0f))
    , m_specularConstant(std::max(specularConstant, 0.0f))
    , m_specularExponent(std::min(std::max(specularExponent, 1.0f), 128.0f))
    , m_kernelUnitLengthX(kernelUnitLengthX)
    , m_kernelUnitLengthY(kernelUnitLengthY)
{
}

FloatRect FELighting::mapPaintRect(const FloatRect& rect, bool)
{
    FloatRect result = rect;
    // The areas affected need to be a pixel bigger to accommodate the Sobel kernel.
    result.inflate(1);
    return result;
}

PassRefPtr<SkImageFilter> FELighting::createImageFilter(SkiaImageFilterBuilder* builder)
{
    SkImageFilter::CropRect rect = getCropRect(builder ? builder->cropOffset() : FloatSize());
    Color lightColor = adaptColorToOperatingColorSpace(m_lightingColor);
    RefPtr<SkImageFilter> input(builder ? builder->build(inputEffect(0), operatingColorSpace()) : nullptr);
    switch (m_lightSource->type()) {
    case LS_DISTANT: {
        DistantLightSource* distantLightSource = static_cast<DistantLightSource*>(m_lightSource.get());
        float azimuthRad = deg2rad(distantLightSource->azimuth());
        float elevationRad = deg2rad(distantLightSource->elevation());
        SkPoint3 direction(cosf(azimuthRad) * cosf(elevationRad),
                           sinf(azimuthRad) * cosf(elevationRad),
                           sinf(elevationRad));
        if (m_specularConstant > 0)
            return adoptRef(SkLightingImageFilter::CreateDistantLitSpecular(direction, lightColor.rgb(), m_surfaceScale, m_specularConstant, m_specularExponent, input.get(), &rect));
        return adoptRef(SkLightingImageFilter::CreateDistantLitDiffuse(direction, lightColor.rgb(), m_surfaceScale, m_diffuseConstant, input.get(), &rect));
    }
    case LS_POINT: {
        PointLightSource* pointLightSource = static_cast<PointLightSource*>(m_lightSource.get());
        FloatPoint3D position = pointLightSource->position();
        SkPoint3 skPosition(position.x(), position.y(), position.z());
        if (m_specularConstant > 0)
            return adoptRef(SkLightingImageFilter::CreatePointLitSpecular(skPosition, lightColor.rgb(), m_surfaceScale, m_specularConstant, m_specularExponent, input.get(), &rect));
        return adoptRef(SkLightingImageFilter::CreatePointLitDiffuse(skPosition, lightColor.rgb(), m_surfaceScale, m_diffuseConstant, input.get(), &rect));
    }
    case LS_SPOT: {
        SpotLightSource* spotLightSource = static_cast<SpotLightSource*>(m_lightSource.get());
        SkPoint3 location(spotLightSource->position().x(), spotLightSource->position().y(), spotLightSource->position().z());
        SkPoint3 target(spotLightSource->direction().x(), spotLightSource->direction().y(), spotLightSource->direction().z());
        float specularExponent = spotLightSource->specularExponent();
        float limitingConeAngle = spotLightSource->limitingConeAngle();
        if (!limitingConeAngle || limitingConeAngle > 90 || limitingConeAngle < -90)
            limitingConeAngle = 90;
        if (m_specularConstant > 0)
            return adoptRef(SkLightingImageFilter::CreateSpotLitSpecular(location, target, specularExponent, limitingConeAngle, lightColor.rgb(), m_surfaceScale, m_specularConstant, m_specularExponent, input.get(), &rect));
        return adoptRef(SkLightingImageFilter::CreateSpotLitDiffuse(location, target, specularExponent, limitingConeAngle, lightColor.rgb(), m_surfaceScale, m_diffuseConstant, input.get(), &rect));
    }
    default:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
}

} // namespace blink
