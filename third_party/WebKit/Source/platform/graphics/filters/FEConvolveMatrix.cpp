/*
 * Copyright (C) 2004, 2005, 2006, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
 * Copyright (C) 2010 Zoltan Herczeg <zherczeg@webkit.org>
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
#include "platform/graphics/filters/FEConvolveMatrix.h"

#include "SkMatrixConvolutionImageFilter.h"
#include "platform/graphics/filters/SkiaImageFilterBuilder.h"
#include "platform/text/TextStream.h"
#include "wtf/OwnPtr.h"

namespace blink {

FEConvolveMatrix::FEConvolveMatrix(Filter* filter, const IntSize& kernelSize,
    float divisor, float bias, const IntPoint& targetOffset, EdgeModeType edgeMode,
    const FloatPoint& kernelUnitLength, bool preserveAlpha, const Vector<float>& kernelMatrix)
    : FilterEffect(filter)
    , m_kernelSize(kernelSize)
    , m_divisor(divisor)
    , m_bias(bias)
    , m_targetOffset(targetOffset)
    , m_edgeMode(edgeMode)
    , m_kernelUnitLength(kernelUnitLength)
    , m_preserveAlpha(preserveAlpha)
    , m_kernelMatrix(kernelMatrix)
{
    ASSERT(m_kernelSize.width() > 0);
    ASSERT(m_kernelSize.height() > 0);
}

PassRefPtrWillBeRawPtr<FEConvolveMatrix> FEConvolveMatrix::create(Filter* filter, const IntSize& kernelSize,
    float divisor, float bias, const IntPoint& targetOffset, EdgeModeType edgeMode,
    const FloatPoint& kernelUnitLength, bool preserveAlpha, const Vector<float>& kernelMatrix)
{
    return adoptRefWillBeNoop(new FEConvolveMatrix(filter, kernelSize, divisor, bias, targetOffset, edgeMode, kernelUnitLength,
        preserveAlpha, kernelMatrix));
}

FloatRect FEConvolveMatrix::mapPaintRect(const FloatRect& rect, bool forward)
{
    FloatRect result = rect;

    result.moveBy(forward ? -m_targetOffset : m_targetOffset - m_kernelSize);
    result.expand(m_kernelSize);
    return result;
}

IntSize FEConvolveMatrix::kernelSize() const
{
    return m_kernelSize;
}

void FEConvolveMatrix::setKernelSize(const IntSize& kernelSize)
{
    ASSERT(kernelSize.width() > 0);
    ASSERT(kernelSize.height() > 0);
    m_kernelSize = kernelSize;
}

const Vector<float>& FEConvolveMatrix::kernel() const
{
    return m_kernelMatrix;
}

void FEConvolveMatrix::setKernel(const Vector<float>& kernel)
{
    m_kernelMatrix = kernel;
}

float FEConvolveMatrix::divisor() const
{
    return m_divisor;
}

bool FEConvolveMatrix::setDivisor(float divisor)
{
    ASSERT(divisor);
    if (m_divisor == divisor)
        return false;
    m_divisor = divisor;
    return true;
}

float FEConvolveMatrix::bias() const
{
    return m_bias;
}

bool FEConvolveMatrix::setBias(float bias)
{
    if (m_bias == bias)
        return false;
    m_bias = bias;
    return true;
}

IntPoint FEConvolveMatrix::targetOffset() const
{
    return m_targetOffset;
}

bool FEConvolveMatrix::setTargetOffset(const IntPoint& targetOffset)
{
    if (m_targetOffset == targetOffset)
        return false;
    m_targetOffset = targetOffset;
    return true;
}

EdgeModeType FEConvolveMatrix::edgeMode() const
{
    return m_edgeMode;
}

bool FEConvolveMatrix::setEdgeMode(EdgeModeType edgeMode)
{
    if (m_edgeMode == edgeMode)
        return false;
    m_edgeMode = edgeMode;
    return true;
}

FloatPoint FEConvolveMatrix::kernelUnitLength() const
{
    return m_kernelUnitLength;
}

bool FEConvolveMatrix::setKernelUnitLength(const FloatPoint& kernelUnitLength)
{
    ASSERT(kernelUnitLength.x() > 0);
    ASSERT(kernelUnitLength.y() > 0);
    if (m_kernelUnitLength == kernelUnitLength)
        return false;
    m_kernelUnitLength = kernelUnitLength;
    return true;
}

bool FEConvolveMatrix::preserveAlpha() const
{
    return m_preserveAlpha;
}

bool FEConvolveMatrix::setPreserveAlpha(bool preserveAlpha)
{
    if (m_preserveAlpha == preserveAlpha)
        return false;
    m_preserveAlpha = preserveAlpha;
    return true;
}

SkMatrixConvolutionImageFilter::TileMode toSkiaTileMode(EdgeModeType edgeMode)
{
    switch (edgeMode) {
    case EDGEMODE_DUPLICATE:
        return SkMatrixConvolutionImageFilter::kClamp_TileMode;
    case EDGEMODE_WRAP:
        return SkMatrixConvolutionImageFilter::kRepeat_TileMode;
    case EDGEMODE_NONE:
        return SkMatrixConvolutionImageFilter::kClampToBlack_TileMode;
    default:
        return SkMatrixConvolutionImageFilter::kClamp_TileMode;
    }
}

PassRefPtr<SkImageFilter> FEConvolveMatrix::createImageFilter(SkiaImageFilterBuilder* builder)
{
    RefPtr<SkImageFilter> input(builder->build(inputEffect(0), operatingColorSpace()));

    SkISize kernelSize(SkISize::Make(m_kernelSize.width(), m_kernelSize.height()));
    int numElements = kernelSize.width() * kernelSize.height();
    SkScalar gain = SkFloatToScalar(1.0f / m_divisor);
    SkScalar bias = SkFloatToScalar(m_bias * 255);
    SkIPoint target = SkIPoint::Make(m_targetOffset.x(), m_targetOffset.y());
    SkMatrixConvolutionImageFilter::TileMode tileMode = toSkiaTileMode(m_edgeMode);
    bool convolveAlpha = !m_preserveAlpha;
    OwnPtr<SkScalar[]> kernel = adoptArrayPtr(new SkScalar[numElements]);
    for (int i = 0; i < numElements; ++i)
        kernel[i] = SkFloatToScalar(m_kernelMatrix[numElements - 1 - i]);
    SkImageFilter::CropRect cropRect = getCropRect(builder->cropOffset());
    return adoptRef(SkMatrixConvolutionImageFilter::Create(kernelSize, kernel.get(), gain, bias, target, tileMode, convolveAlpha, input.get(), &cropRect));
}

static TextStream& operator<<(TextStream& ts, const EdgeModeType& type)
{
    switch (type) {
    case EDGEMODE_UNKNOWN:
        ts << "UNKNOWN";
        break;
    case EDGEMODE_DUPLICATE:
        ts << "DUPLICATE";
        break;
    case EDGEMODE_WRAP:
        ts << "WRAP";
        break;
    case EDGEMODE_NONE:
        ts << "NONE";
        break;
    }
    return ts;
}

TextStream& FEConvolveMatrix::externalRepresentation(TextStream& ts, int indent) const
{
    writeIndent(ts, indent);
    ts << "[feConvolveMatrix";
    FilterEffect::externalRepresentation(ts);
    ts << " order=\"" << m_kernelSize << "\" "
       << "kernelMatrix=\"" << m_kernelMatrix  << "\" "
       << "divisor=\"" << m_divisor << "\" "
       << "bias=\"" << m_bias << "\" "
       << "target=\"" << m_targetOffset << "\" "
       << "edgeMode=\"" << m_edgeMode << "\" "
       << "kernelUnitLength=\"" << m_kernelUnitLength << "\" "
       << "preserveAlpha=\"" << m_preserveAlpha << "\"]\n";
    inputEffect(0)->externalRepresentation(ts, indent + 1);
    return ts;
}

} // namespace blink
