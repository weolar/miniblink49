/*
 * Copyright (C) 2004, 2005, 2006, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
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

#include "platform/graphics/filters/FEComposite.h"

#include "SkArithmeticMode.h"
#include "SkXfermodeImageFilter.h"

#include "platform/graphics/filters/SkiaImageFilterBuilder.h"
#include "platform/text/TextStream.h"

namespace blink {

FEComposite::FEComposite(Filter* filter, const CompositeOperationType& type, float k1, float k2, float k3, float k4)
    : FilterEffect(filter)
    , m_type(type)
    , m_k1(k1)
    , m_k2(k2)
    , m_k3(k3)
    , m_k4(k4)
{
}

PassRefPtrWillBeRawPtr<FEComposite> FEComposite::create(Filter* filter, const CompositeOperationType& type, float k1, float k2, float k3, float k4)
{
    return adoptRefWillBeNoop(new FEComposite(filter, type, k1, k2, k3, k4));
}

CompositeOperationType FEComposite::operation() const
{
    return m_type;
}

bool FEComposite::setOperation(CompositeOperationType type)
{
    if (m_type == type)
        return false;
    m_type = type;
    return true;
}

float FEComposite::k1() const
{
    return m_k1;
}

bool FEComposite::setK1(float k1)
{
    if (m_k1 == k1)
        return false;
    m_k1 = k1;
    return true;
}

float FEComposite::k2() const
{
    return m_k2;
}

bool FEComposite::setK2(float k2)
{
    if (m_k2 == k2)
        return false;
    m_k2 = k2;
    return true;
}

float FEComposite::k3() const
{
    return m_k3;
}

bool FEComposite::setK3(float k3)
{
    if (m_k3 == k3)
        return false;
    m_k3 = k3;
    return true;
}

float FEComposite::k4() const
{
    return m_k4;
}

bool FEComposite::setK4(float k4)
{
    if (m_k4 == k4)
        return false;
    m_k4 = k4;
    return true;
}

FloatRect FEComposite::determineAbsolutePaintRect(const FloatRect& originalRequestedRect)
{
    FloatRect requestedRect = originalRequestedRect;
    if (clipsToBounds())
        requestedRect.intersect(maxEffectRect());

    // We may be called multiple times if result is used more than once. Return
    // quickly if nothing new is required.
    if (absolutePaintRect().contains(enclosingIntRect(requestedRect)))
        return requestedRect;

    // No mapPaintRect required for FEComposite.
    FloatRect input1Rect = inputEffect(1)->determineAbsolutePaintRect(requestedRect);
    FloatRect affectedRect;
    switch (m_type) {
    case FECOMPOSITE_OPERATOR_IN:
        // 'in' has output only in the intersection of both inputs.
        affectedRect = intersection(input1Rect, inputEffect(0)->determineAbsolutePaintRect(input1Rect));
        break;
    case FECOMPOSITE_OPERATOR_ATOP:
        // 'atop' has output only in the extents of the second input.
        // Make sure first input knows where it needs to produce output.
        inputEffect(0)->determineAbsolutePaintRect(input1Rect);
        affectedRect = input1Rect;
        break;
    case FECOMPOSITE_OPERATOR_ARITHMETIC:
        if (k4() > 0) {
            // Make sure first input knows where it needs to produce output.
            inputEffect(0)->determineAbsolutePaintRect(requestedRect);
            // Arithmetic with non-zero k4 may influnce the complete filter primitive
            // region. So we can't optimize the paint region here.
            affectedRect = requestedRect;
            break;
        }
        if (k2() <= 0) {
            // Input 0 does not appear where input 1 is not present.
            FloatRect input0Rect = inputEffect(0)->determineAbsolutePaintRect(input1Rect);
            if (k3() > 0) {
                affectedRect = input1Rect;
            } else {
                // Just k1 is positive. Use intersection.
                affectedRect = intersection(input1Rect, input0Rect);
            }
            break;
        }
        // else fall through to use union
    default:
        // Take the union of both input effects.
        affectedRect = unionRect(input1Rect, inputEffect(0)->determineAbsolutePaintRect(requestedRect));
        break;
    }

    affectedRect.intersect(requestedRect);
    addAbsolutePaintRect(affectedRect);
    return affectedRect;
}

SkXfermode::Mode toXfermode(CompositeOperationType mode)
{
    switch (mode) {
    case FECOMPOSITE_OPERATOR_OVER:
        return SkXfermode::kSrcOver_Mode;
    case FECOMPOSITE_OPERATOR_IN:
        return SkXfermode::kSrcIn_Mode;
    case FECOMPOSITE_OPERATOR_OUT:
        return SkXfermode::kSrcOut_Mode;
    case FECOMPOSITE_OPERATOR_ATOP:
        return SkXfermode::kSrcATop_Mode;
    case FECOMPOSITE_OPERATOR_XOR:
        return SkXfermode::kXor_Mode;
    case FECOMPOSITE_OPERATOR_LIGHTER:
        return SkXfermode::kPlus_Mode;
    default:
        ASSERT_NOT_REACHED();
        return SkXfermode::kSrcOver_Mode;
    }
}

PassRefPtr<SkImageFilter> FEComposite::createImageFilter(SkiaImageFilterBuilder* builder)
{
    return createImageFilterInternal(builder, true);
}

PassRefPtr<SkImageFilter> FEComposite::createImageFilterWithoutValidation(SkiaImageFilterBuilder* builder)
{
    return createImageFilterInternal(builder, false);
}

PassRefPtr<SkImageFilter> FEComposite::createImageFilterInternal(SkiaImageFilterBuilder* builder, bool requiresPMColorValidation)
{
    RefPtr<SkImageFilter> foreground(builder->build(inputEffect(0), operatingColorSpace(), !mayProduceInvalidPreMultipliedPixels()));
    RefPtr<SkImageFilter> background(builder->build(inputEffect(1), operatingColorSpace(), !mayProduceInvalidPreMultipliedPixels()));
    SkImageFilter::CropRect cropRect = getCropRect(builder->cropOffset());
    RefPtr<SkXfermode> mode;
    if (m_type == FECOMPOSITE_OPERATOR_ARITHMETIC)
        mode = adoptRef(SkArithmeticMode::Create(SkFloatToScalar(m_k1), SkFloatToScalar(m_k2), SkFloatToScalar(m_k3), SkFloatToScalar(m_k4), requiresPMColorValidation));
    else
        mode = adoptRef(SkXfermode::Create(toXfermode(m_type)));
    return adoptRef(SkXfermodeImageFilter::Create(mode.get(), background.get(), foreground.get(), &cropRect));
}

static TextStream& operator<<(TextStream& ts, const CompositeOperationType& type)
{
    switch (type) {
    case FECOMPOSITE_OPERATOR_UNKNOWN:
        ts << "UNKNOWN";
        break;
    case FECOMPOSITE_OPERATOR_OVER:
        ts << "OVER";
        break;
    case FECOMPOSITE_OPERATOR_IN:
        ts << "IN";
        break;
    case FECOMPOSITE_OPERATOR_OUT:
        ts << "OUT";
        break;
    case FECOMPOSITE_OPERATOR_ATOP:
        ts << "ATOP";
        break;
    case FECOMPOSITE_OPERATOR_XOR:
        ts << "XOR";
        break;
    case FECOMPOSITE_OPERATOR_ARITHMETIC:
        ts << "ARITHMETIC";
        break;
    case FECOMPOSITE_OPERATOR_LIGHTER:
        ts << "LIGHTER";
        break;
    }
    return ts;
}

TextStream& FEComposite::externalRepresentation(TextStream& ts, int indent) const
{
    writeIndent(ts, indent);
    ts << "[feComposite";
    FilterEffect::externalRepresentation(ts);
    ts << " operation=\"" << m_type << "\"";
    if (m_type == FECOMPOSITE_OPERATOR_ARITHMETIC)
        ts << " k1=\"" << m_k1 << "\" k2=\"" << m_k2 << "\" k3=\"" << m_k3 << "\" k4=\"" << m_k4 << "\"";
    ts << "]\n";
    inputEffect(0)->externalRepresentation(ts, indent + 1);
    inputEffect(1)->externalRepresentation(ts, indent + 1);
    return ts;
}

} // namespace blink
