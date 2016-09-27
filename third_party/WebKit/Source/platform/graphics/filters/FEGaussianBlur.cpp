/*
 * Copyright (C) 2004, 2005, 2006, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
 * Copyright (C) 2010 Igalia, S.L.
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
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

#include "platform/graphics/filters/FEGaussianBlur.h"

#include "platform/graphics/filters/Filter.h"
#include "platform/graphics/filters/SkiaImageFilterBuilder.h"
#include "platform/text/TextStream.h"

#include "SkBlurImageFilter.h"

static inline float gaussianKernelFactor()
{
    return 3 / 4.f * sqrtf(twoPiFloat);
}

namespace blink {

FEGaussianBlur::FEGaussianBlur(Filter* filter, float x, float y)
    : FilterEffect(filter)
    , m_stdX(x)
    , m_stdY(y)
{
}

PassRefPtrWillBeRawPtr<FEGaussianBlur> FEGaussianBlur::create(Filter* filter, float x, float y)
{
    return adoptRefWillBeNoop(new FEGaussianBlur(filter, x, y));
}

IntSize FEGaussianBlur::calculateUnscaledKernelSize(const FloatPoint& std)
{
    ASSERT(std.x() >= 0 && std.y() >= 0);

    IntSize kernelSize;
    // Limit the kernel size to 1000. A bigger radius won't make a big difference for the result image but
    // inflates the absolute paint rect to much. This is compatible with Firefox' behavior.
    if (std.x()) {
        int size = std::max<unsigned>(2, static_cast<unsigned>(floorf(std.x() * gaussianKernelFactor() + 0.5f)));
        kernelSize.setWidth(size);
    }

    if (std.y()) {
        int size = std::max<unsigned>(2, static_cast<unsigned>(floorf(std.y() * gaussianKernelFactor() + 0.5f)));
        kernelSize.setHeight(size);
    }

    return kernelSize;
}

IntSize FEGaussianBlur::calculateKernelSize(Filter* filter, const FloatPoint& std)
{
    FloatPoint stdError(filter->applyHorizontalScale(std.x()), filter->applyVerticalScale(std.y()));

    return calculateUnscaledKernelSize(stdError);
}

FloatRect FEGaussianBlur::mapRect(const FloatRect& rect, bool)
{
    FloatRect result = rect;
    IntSize kernelSize = calculateKernelSize(filter(), FloatPoint(m_stdX, m_stdY));

    // We take the half kernel size and multiply it with three, because we run box blur three times.
    result.inflateX(3 * kernelSize.width() * 0.5f);
    result.inflateY(3 * kernelSize.height() * 0.5f);
    return result;
}

FloatRect FEGaussianBlur::determineAbsolutePaintRect(const FloatRect& originalRequestedRect)
{
    FloatRect requestedRect = originalRequestedRect;
    if (clipsToBounds())
        requestedRect.intersect(maxEffectRect());

    FilterEffect* input = inputEffect(0);
    FloatRect inputRect = input->determineAbsolutePaintRect(mapRect(requestedRect, false));
    FloatRect outputRect = mapRect(inputRect, true);
    outputRect.intersect(requestedRect);
    addAbsolutePaintRect(outputRect);

    // Blur needs space for both input and output pixels in the paint area.
    // Input is also clipped to subregion.
    if (clipsToBounds())
        inputRect.intersect(maxEffectRect());
    addAbsolutePaintRect(inputRect);
    return outputRect;
}

PassRefPtr<SkImageFilter> FEGaussianBlur::createImageFilter(SkiaImageFilterBuilder* builder)
{
    RefPtr<SkImageFilter> input(builder->build(inputEffect(0), operatingColorSpace()));
    float stdX = filter()->applyHorizontalScale(m_stdX);
    float stdY = filter()->applyVerticalScale(m_stdY);
    SkImageFilter::CropRect rect = getCropRect(builder->cropOffset());
    return adoptRef(SkBlurImageFilter::Create(SkFloatToScalar(stdX), SkFloatToScalar(stdY), input.get(), &rect));
}

TextStream& FEGaussianBlur::externalRepresentation(TextStream& ts, int indent) const
{
    writeIndent(ts, indent);
    ts << "[feGaussianBlur";
    FilterEffect::externalRepresentation(ts);
    ts << " stdDeviation=\"" << m_stdX << ", " << m_stdY << "\"]\n";
    inputEffect(0)->externalRepresentation(ts, indent + 1);
    return ts;
}

} // namespace blink
