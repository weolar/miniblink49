/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 2004-2005 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2006, 2007 Nicholas Shanks (webkit@nickshanks.com)
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Alexey Proskuryakov <ap@webkit.org>
 * Copyright (C) 2007, 2008 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 * Copyright (C) Research In Motion Limited 2011. All rights reserved.
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
#include "core/css/resolver/TransformBuilder.h"

#include "core/css/CSSFunctionValue.h"
#include "core/css/CSSPrimitiveValueMappings.h"
#include "platform/heap/Handle.h"
#include "platform/transforms/Matrix3DTransformOperation.h"
#include "platform/transforms/MatrixTransformOperation.h"
#include "platform/transforms/PerspectiveTransformOperation.h"
#include "platform/transforms/RotateTransformOperation.h"
#include "platform/transforms/ScaleTransformOperation.h"
#include "platform/transforms/SkewTransformOperation.h"
#include "platform/transforms/TransformationMatrix.h"
#include "platform/transforms/TranslateTransformOperation.h"

namespace blink {

static Length convertToFloatLength(CSSPrimitiveValue* primitiveValue, const CSSToLengthConversionData& conversionData)
{
    ASSERT(primitiveValue);
    return primitiveValue->convertToLength(conversionData);
}

static TransformOperation::OperationType getTransformOperationType(CSSValueID type)
{
    switch (type) {
    case CSSValueScale: return TransformOperation::Scale;
    case CSSValueScaleX: return TransformOperation::ScaleX;
    case CSSValueScaleY: return TransformOperation::ScaleY;
    case CSSValueScaleZ: return TransformOperation::ScaleZ;
    case CSSValueScale3d: return TransformOperation::Scale3D;
    case CSSValueTranslate: return TransformOperation::Translate;
    case CSSValueTranslateX: return TransformOperation::TranslateX;
    case CSSValueTranslateY: return TransformOperation::TranslateY;
    case CSSValueTranslateZ: return TransformOperation::TranslateZ;
    case CSSValueTranslate3d: return TransformOperation::Translate3D;
    case CSSValueRotate: return TransformOperation::Rotate;
    case CSSValueRotateX: return TransformOperation::RotateX;
    case CSSValueRotateY: return TransformOperation::RotateY;
    case CSSValueRotateZ: return TransformOperation::RotateZ;
    case CSSValueRotate3d: return TransformOperation::Rotate3D;
    case CSSValueSkew: return TransformOperation::Skew;
    case CSSValueSkewX: return TransformOperation::SkewX;
    case CSSValueSkewY: return TransformOperation::SkewY;
    case CSSValueMatrix: return TransformOperation::Matrix;
    case CSSValueMatrix3d: return TransformOperation::Matrix3D;
    case CSSValuePerspective: return TransformOperation::Perspective;
    default:
        ASSERT_NOT_REACHED();
        // FIXME: We shouldn't have a type None since we never create them
        return TransformOperation::None;
    }
}

void TransformBuilder::createTransformOperations(CSSValue& inValue, const CSSToLengthConversionData& conversionData, TransformOperations& outOperations)
{
    ASSERT(!outOperations.size());
    if (!inValue.isValueList()) {
        ASSERT(inValue.isPrimitiveValue() && toCSSPrimitiveValue(inValue).getValueID() == CSSValueNone);
        return;
    }

    float zoomFactor = conversionData.zoom();
    for (auto& value : toCSSValueList(inValue)) {
        CSSFunctionValue* transformValue = toCSSFunctionValue(value.get());
        TransformOperation::OperationType transformType = getTransformOperationType(transformValue->functionType());

        CSSPrimitiveValue* firstValue = toCSSPrimitiveValue(transformValue->item(0));

        switch (transformType) {
        case TransformOperation::Scale:
        case TransformOperation::ScaleX:
        case TransformOperation::ScaleY: {
            double sx = 1.0;
            double sy = 1.0;
            if (transformType == TransformOperation::ScaleY) {
                sy = firstValue->getDoubleValue();
            } else {
                sx = firstValue->getDoubleValue();
                if (transformType != TransformOperation::ScaleX) {
                    if (transformValue->length() > 1) {
                        CSSPrimitiveValue* secondValue = toCSSPrimitiveValue(transformValue->item(1));
                        sy = secondValue->getDoubleValue();
                    } else {
                        sy = sx;
                    }
                }
            }
            outOperations.operations().append(ScaleTransformOperation::create(sx, sy, 1.0, transformType));
            break;
        }
        case TransformOperation::ScaleZ:
        case TransformOperation::Scale3D: {
            double sx = 1.0;
            double sy = 1.0;
            double sz = 1.0;
            if (transformType == TransformOperation::ScaleZ) {
                sz = firstValue->getDoubleValue();
            } else {
                sx = firstValue->getDoubleValue();
                sy = toCSSPrimitiveValue(transformValue->item(1))->getDoubleValue();
                sz = toCSSPrimitiveValue(transformValue->item(2))->getDoubleValue();
            }
            outOperations.operations().append(ScaleTransformOperation::create(sx, sy, sz, transformType));
            break;
        }
        case TransformOperation::Translate:
        case TransformOperation::TranslateX:
        case TransformOperation::TranslateY: {
            Length tx = Length(0, Fixed);
            Length ty = Length(0, Fixed);
            if (transformType == TransformOperation::TranslateY)
                ty = convertToFloatLength(firstValue, conversionData);
            else {
                tx = convertToFloatLength(firstValue, conversionData);
                if (transformType != TransformOperation::TranslateX) {
                    if (transformValue->length() > 1) {
                        CSSPrimitiveValue* secondValue = toCSSPrimitiveValue(transformValue->item(1));
                        ty = convertToFloatLength(secondValue, conversionData);
                    }
                }
            }

            outOperations.operations().append(TranslateTransformOperation::create(tx, ty, 0, transformType));
            break;
        }
        case TransformOperation::TranslateZ:
        case TransformOperation::Translate3D: {
            Length tx = Length(0, Fixed);
            Length ty = Length(0, Fixed);
            double tz = 0;
            if (transformType == TransformOperation::TranslateZ) {
                tz = firstValue->computeLength<double>(conversionData);
            } else {
                tx = convertToFloatLength(firstValue, conversionData);
                ty = convertToFloatLength(toCSSPrimitiveValue(transformValue->item(1)), conversionData);
                tz = toCSSPrimitiveValue(transformValue->item(2))->computeLength<double>(conversionData);
            }

            outOperations.operations().append(TranslateTransformOperation::create(tx, ty, tz, transformType));
            break;
        }
        case TransformOperation::RotateX:
        case TransformOperation::RotateY:
        case TransformOperation::RotateZ: {
            double angle = firstValue->computeDegrees();
            double x = transformType == TransformOperation::RotateX;
            double y = transformType == TransformOperation::RotateY;
            double z = transformType == TransformOperation::RotateZ;
            outOperations.operations().append(RotateTransformOperation::create(x, y, z, angle, transformType));
            break;
        }
        case TransformOperation::Rotate3D: {
            CSSPrimitiveValue* secondValue = toCSSPrimitiveValue(transformValue->item(1));
            CSSPrimitiveValue* thirdValue = toCSSPrimitiveValue(transformValue->item(2));
            CSSPrimitiveValue* fourthValue = toCSSPrimitiveValue(transformValue->item(3));
            double x = firstValue->getDoubleValue();
            double y = secondValue->getDoubleValue();
            double z = thirdValue->getDoubleValue();
            double angle = fourthValue->computeDegrees();
            outOperations.operations().append(RotateTransformOperation::create(x, y, z, angle, transformType));
            break;
        }
        case TransformOperation::Skew:
        case TransformOperation::SkewX:
        case TransformOperation::SkewY: {
            double angleX = 0;
            double angleY = 0;
            double angle = firstValue->computeDegrees();
            if (transformType == TransformOperation::SkewY)
                angleY = angle;
            else {
                angleX = angle;
                if (transformType == TransformOperation::Skew) {
                    if (transformValue->length() > 1) {
                        CSSPrimitiveValue* secondValue = toCSSPrimitiveValue(transformValue->item(1));
                        angleY = secondValue->computeDegrees();
                    }
                }
            }
            outOperations.operations().append(SkewTransformOperation::create(angleX, angleY, transformType));
            break;
        }
        case TransformOperation::Matrix: {
            double a = firstValue->getDoubleValue();
            double b = toCSSPrimitiveValue(transformValue->item(1))->getDoubleValue();
            double c = toCSSPrimitiveValue(transformValue->item(2))->getDoubleValue();
            double d = toCSSPrimitiveValue(transformValue->item(3))->getDoubleValue();
            double e = zoomFactor * toCSSPrimitiveValue(transformValue->item(4))->getDoubleValue();
            double f = zoomFactor * toCSSPrimitiveValue(transformValue->item(5))->getDoubleValue();
            outOperations.operations().append(MatrixTransformOperation::create(a, b, c, d, e, f));
            break;
        }
        case TransformOperation::Matrix3D: {
            TransformationMatrix matrix(toCSSPrimitiveValue(transformValue->item(0))->getDoubleValue(),
                toCSSPrimitiveValue(transformValue->item(1))->getDoubleValue(),
                toCSSPrimitiveValue(transformValue->item(2))->getDoubleValue(),
                toCSSPrimitiveValue(transformValue->item(3))->getDoubleValue(),
                toCSSPrimitiveValue(transformValue->item(4))->getDoubleValue(),
                toCSSPrimitiveValue(transformValue->item(5))->getDoubleValue(),
                toCSSPrimitiveValue(transformValue->item(6))->getDoubleValue(),
                toCSSPrimitiveValue(transformValue->item(7))->getDoubleValue(),
                toCSSPrimitiveValue(transformValue->item(8))->getDoubleValue(),
                toCSSPrimitiveValue(transformValue->item(9))->getDoubleValue(),
                toCSSPrimitiveValue(transformValue->item(10))->getDoubleValue(),
                toCSSPrimitiveValue(transformValue->item(11))->getDoubleValue(),
                zoomFactor * toCSSPrimitiveValue(transformValue->item(12))->getDoubleValue(),
                zoomFactor * toCSSPrimitiveValue(transformValue->item(13))->getDoubleValue(),
                toCSSPrimitiveValue(transformValue->item(14))->getDoubleValue(),
                toCSSPrimitiveValue(transformValue->item(15))->getDoubleValue());
            outOperations.operations().append(Matrix3DTransformOperation::create(matrix));
            break;
        }
        case TransformOperation::Perspective: {
            double p = firstValue->computeLength<double>(conversionData);
            ASSERT(p >= 0);
            outOperations.operations().append(PerspectiveTransformOperation::create(p));
            break;
        }
        default:
            ASSERT_NOT_REACHED();
            break;
        }
    }
}

} // namespace blink
