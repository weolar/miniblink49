// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/PathSVGInterpolation.h"

#include "core/svg/SVGPathElement.h"
#include "core/svg/SVGPathSegArcAbs.h"
#include "core/svg/SVGPathSegArcRel.h"
#include "core/svg/SVGPathSegClosePath.h"
#include "core/svg/SVGPathSegCurvetoCubicAbs.h"
#include "core/svg/SVGPathSegCurvetoCubicRel.h"
#include "core/svg/SVGPathSegCurvetoCubicSmoothAbs.h"
#include "core/svg/SVGPathSegCurvetoCubicSmoothRel.h"
#include "core/svg/SVGPathSegCurvetoQuadraticAbs.h"
#include "core/svg/SVGPathSegCurvetoQuadraticRel.h"
#include "core/svg/SVGPathSegCurvetoQuadraticSmoothAbs.h"
#include "core/svg/SVGPathSegCurvetoQuadraticSmoothRel.h"
#include "core/svg/SVGPathSegLinetoAbs.h"
#include "core/svg/SVGPathSegLinetoHorizontalAbs.h"
#include "core/svg/SVGPathSegLinetoHorizontalRel.h"
#include "core/svg/SVGPathSegLinetoRel.h"
#include "core/svg/SVGPathSegLinetoVerticalAbs.h"
#include "core/svg/SVGPathSegLinetoVerticalRel.h"
#include "core/svg/SVGPathSegMovetoAbs.h"
#include "core/svg/SVGPathSegMovetoRel.h"

namespace blink {

namespace {

struct SubPathCoordinates {
    double initialX = 0;
    double initialY = 0;
    double currentX = 0;
    double currentY = 0;
};

SVGPathSegType absolutePathSegType(const SVGPathSeg& item)
{
    return toAbsolutePathSegType(static_cast<SVGPathSegType>(item.pathSegType()));
}

bool isAbsolutePathSegType(const SVGPathSeg& item)
{
    return isAbsolutePathSegType(static_cast<SVGPathSegType>(item.pathSegType()));
}

PassOwnPtrWillBeRawPtr<InterpolableNumber> controlToInterpolableValue(double value, bool isAbsolute, double currentValue)
{
    if (isAbsolute)
        return InterpolableNumber::create(value);
    return InterpolableNumber::create(currentValue + value);
}

double controlFromInterpolableValue(const InterpolableValue* number, bool isAbsolute, double currentValue)
{
    double value = toInterpolableNumber(number)->value();

    if (isAbsolute)
        return value;
    return value - currentValue;
}

PassOwnPtrWillBeRawPtr<InterpolableNumber> specifiedToInterpolableValue(double value, bool isAbsolute, double& currentValue)
{
    if (isAbsolute)
        currentValue = value;
    else
        currentValue += value;
    return InterpolableNumber::create(currentValue);
}

double specifiedFromInterpolableValue(const InterpolableValue* number, bool isAbsolute, double& currentValue)
{
    double previousValue = currentValue;
    currentValue = toInterpolableNumber(number)->value();

    if (isAbsolute)
        return currentValue;
    return currentValue - previousValue;
}

PassOwnPtrWillBeRawPtr<InterpolableValue> pathSegClosePathToInterpolableValue(const SVGPathSeg& item, SubPathCoordinates& coordinates)
{
    coordinates.currentX = coordinates.initialX;
    coordinates.currentY = coordinates.initialY;

    // arbitrary
    return InterpolableBool::create(false);
}

PassRefPtrWillBeRawPtr<SVGPathSeg> pathSegClosePathFromInterpolableValue(const InterpolableValue&, SVGPathSegType, SubPathCoordinates& coordinates, SVGPathElement* element)
{
    coordinates.currentX = coordinates.initialX;
    coordinates.currentY = coordinates.initialY;

    return SVGPathSegClosePath::create(element);
}

PassOwnPtrWillBeRawPtr<InterpolableValue> pathSegSingleCoordinateToInterpolableValue(const SVGPathSegSingleCoordinate& item, SubPathCoordinates& coordinates)
{
    bool isAbsolute = isAbsolutePathSegType(item);
    OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(2);
    result->set(0, specifiedToInterpolableValue(item.x(), isAbsolute, coordinates.currentX));
    result->set(1, specifiedToInterpolableValue(item.y(), isAbsolute, coordinates.currentY));

    if (absolutePathSegType(item) == PathSegMoveToAbs) {
        // Any upcoming 'closepath' commands bring us back to the location we have just moved to.
        coordinates.initialX = coordinates.currentX;
        coordinates.initialY = coordinates.currentY;
    }

    return result.release();
}

PassRefPtrWillBeRawPtr<SVGPathSeg> pathSegSingleCoordinateFromInterpolableValue(const InterpolableValue& value, SVGPathSegType segType, SubPathCoordinates& coordinates, SVGPathElement* element)
{
    const InterpolableList& list = toInterpolableList(value);
    bool isAbsolute = isAbsolutePathSegType(segType);
    float x = specifiedFromInterpolableValue(list.get(0), isAbsolute, coordinates.currentX);
    float y = specifiedFromInterpolableValue(list.get(1), isAbsolute, coordinates.currentY);

    if (toAbsolutePathSegType(segType) == PathSegMoveToAbs) {
        // Any upcoming 'closepath' commands bring us back to the location we have just moved to.
        coordinates.initialX = coordinates.currentX;
        coordinates.initialY = coordinates.currentY;
    }

    switch (segType) {
    case PathSegMoveToAbs:
        return SVGPathSegMovetoAbs::create(element, x, y);
    case PathSegMoveToRel:
        return SVGPathSegMovetoRel::create(element, x, y);
    case PathSegLineToAbs:
        return SVGPathSegLinetoAbs::create(element, x, y);
    case PathSegLineToRel:
        return SVGPathSegLinetoRel::create(element, x, y);
    case PathSegCurveToQuadraticSmoothAbs:
        return SVGPathSegCurvetoQuadraticSmoothAbs::create(element, x, y);
    case PathSegCurveToQuadraticSmoothRel:
        return SVGPathSegCurvetoQuadraticSmoothRel::create(element, x, y);
    default:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
}

PassOwnPtrWillBeRawPtr<InterpolableValue> pathSegCurvetoCubicToInterpolableValue(const SVGPathSegCurvetoCubic& item, SubPathCoordinates& coordinates)
{
    bool isAbsolute = isAbsolutePathSegType(item);
    OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(6);
    result->set(0, controlToInterpolableValue(item.x1(), isAbsolute, coordinates.currentX));
    result->set(1, controlToInterpolableValue(item.y1(), isAbsolute, coordinates.currentY));
    result->set(2, controlToInterpolableValue(item.x2(), isAbsolute, coordinates.currentX));
    result->set(3, controlToInterpolableValue(item.y2(), isAbsolute, coordinates.currentY));
    result->set(4, specifiedToInterpolableValue(item.x(), isAbsolute, coordinates.currentX));
    result->set(5, specifiedToInterpolableValue(item.y(), isAbsolute, coordinates.currentY));
    return result.release();
}

PassRefPtrWillBeRawPtr<SVGPathSeg> pathSegCurvetoCubicFromInterpolableValue(const InterpolableValue& value, SVGPathSegType segType, SubPathCoordinates& coordinates, SVGPathElement* element)
{
    const InterpolableList& list = toInterpolableList(value);
    bool isAbsolute = isAbsolutePathSegType(segType);
    float x1 = controlFromInterpolableValue(list.get(0), isAbsolute, coordinates.currentX);
    float y1 = controlFromInterpolableValue(list.get(1), isAbsolute, coordinates.currentY);
    float x2 = controlFromInterpolableValue(list.get(2), isAbsolute, coordinates.currentX);
    float y2 = controlFromInterpolableValue(list.get(3), isAbsolute, coordinates.currentY);
    float x = specifiedFromInterpolableValue(list.get(4), isAbsolute, coordinates.currentX);
    float y = specifiedFromInterpolableValue(list.get(5), isAbsolute, coordinates.currentY);

    switch (segType) {
    case PathSegCurveToCubicAbs:
        return SVGPathSegCurvetoCubicAbs::create(element, x, y, x1, y1, x2, y2);
    case PathSegCurveToCubicRel:
        return SVGPathSegCurvetoCubicRel::create(element, x, y, x1, y1, x2, y2);
    default:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
}

PassOwnPtrWillBeRawPtr<InterpolableValue> pathSegCurvetoQuadraticToInterpolableValue(const SVGPathSegCurvetoQuadratic& item, SubPathCoordinates& coordinates)
{
    bool isAbsolute = isAbsolutePathSegType(item);
    OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(4);
    result->set(0, controlToInterpolableValue(item.x1(), isAbsolute, coordinates.currentX));
    result->set(1, controlToInterpolableValue(item.y1(), isAbsolute, coordinates.currentY));
    result->set(2, specifiedToInterpolableValue(item.x(), isAbsolute, coordinates.currentX));
    result->set(3, specifiedToInterpolableValue(item.y(), isAbsolute, coordinates.currentY));
    return result.release();
}

PassRefPtrWillBeRawPtr<SVGPathSeg> pathSegCurvetoQuadraticFromInterpolableValue(const InterpolableValue& value, SVGPathSegType segType, SubPathCoordinates& coordinates, SVGPathElement* element)
{
    const InterpolableList& list = toInterpolableList(value);
    bool isAbsolute = isAbsolutePathSegType(segType);
    float x1 = controlFromInterpolableValue(list.get(0), isAbsolute, coordinates.currentX);
    float y1 = controlFromInterpolableValue(list.get(1), isAbsolute, coordinates.currentY);
    float x = specifiedFromInterpolableValue(list.get(2), isAbsolute, coordinates.currentX);
    float y = specifiedFromInterpolableValue(list.get(3), isAbsolute, coordinates.currentY);
    switch (segType) {
    case PathSegCurveToQuadraticAbs:
        return SVGPathSegCurvetoQuadraticAbs::create(element, x, y, x1, y1);
    case PathSegCurveToQuadraticRel:
        return SVGPathSegCurvetoQuadraticRel::create(element, x, y, x1, y1);
    default:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
}

PassOwnPtrWillBeRawPtr<InterpolableValue> pathSegArcToInterpolableValue(const SVGPathSegArc& item, SubPathCoordinates& coordinates)
{
    bool isAbsolute = isAbsolutePathSegType(item);
    OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(7);
    result->set(0, specifiedToInterpolableValue(item.x(), isAbsolute, coordinates.currentX));
    result->set(1, specifiedToInterpolableValue(item.y(), isAbsolute, coordinates.currentY));
    result->set(2, InterpolableNumber::create(item.r1()));
    result->set(3, InterpolableNumber::create(item.r2()));
    result->set(4, InterpolableNumber::create(item.angle()));
    result->set(5, InterpolableBool::create(item.largeArcFlag()));
    result->set(6, InterpolableBool::create(item.sweepFlag()));
    return result.release();
}

PassRefPtrWillBeRawPtr<SVGPathSeg> pathSegArcFromInterpolableValue(const InterpolableValue& value, SVGPathSegType segType, SubPathCoordinates& coordinates, SVGPathElement* element)
{
    const InterpolableList& list = toInterpolableList(value);
    bool isAbsolute = isAbsolutePathSegType(segType);
    float x = specifiedFromInterpolableValue(list.get(0), isAbsolute, coordinates.currentX);
    float y = specifiedFromInterpolableValue(list.get(1), isAbsolute, coordinates.currentY);
    float r1 = toInterpolableNumber(list.get(2))->value();
    float r2 = toInterpolableNumber(list.get(3))->value();
    float angle = toInterpolableNumber(list.get(4))->value();
    bool largeArcFlag = toInterpolableBool(list.get(5))->value();
    bool sweepFlag = toInterpolableBool(list.get(6))->value();
    switch (segType) {
    case PathSegArcAbs:
        return SVGPathSegArcAbs::create(element, x, y, r1, r2, angle, largeArcFlag, sweepFlag);
    case PathSegArcRel:
        return SVGPathSegArcRel::create(element, x, y, r1, r2, angle, largeArcFlag, sweepFlag);
    default:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
}

PassOwnPtrWillBeRawPtr<InterpolableValue> pathSegLinetoHorizontalToInterpolableValue(const SVGPathSegLinetoHorizontal& item, SubPathCoordinates& coordinates)
{
    bool isAbsolute = isAbsolutePathSegType(item);
    return specifiedToInterpolableValue(item.x(), isAbsolute, coordinates.currentX);
}

PassRefPtrWillBeRawPtr<SVGPathSeg> pathSegLinetoHorizontalFromInterpolableValue(const InterpolableValue& value, SVGPathSegType segType, SubPathCoordinates& coordinates, SVGPathElement* element)
{
    bool isAbsolute = isAbsolutePathSegType(segType);
    float x = specifiedFromInterpolableValue(&value, isAbsolute, coordinates.currentX);

    switch (segType) {
    case PathSegLineToHorizontalAbs:
        return SVGPathSegLinetoHorizontalAbs::create(element, x);
    case PathSegLineToHorizontalRel:
        return SVGPathSegLinetoHorizontalRel::create(element, x);
    default:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
}

PassOwnPtrWillBeRawPtr<InterpolableValue> pathSegLinetoVerticalToInterpolableValue(const SVGPathSegLinetoVertical& item, SubPathCoordinates& coordinates)
{
    bool isAbsolute = isAbsolutePathSegType(item);
    return specifiedToInterpolableValue(item.y(), isAbsolute, coordinates.currentY);
}

PassRefPtrWillBeRawPtr<SVGPathSeg> pathSegLinetoVerticalFromInterpolableValue(const InterpolableValue& value, SVGPathSegType segType, SubPathCoordinates& coordinates, SVGPathElement* element)
{
    bool isAbsolute = isAbsolutePathSegType(segType);
    float y = specifiedFromInterpolableValue(&value, isAbsolute, coordinates.currentY);

    switch (segType) {
    case PathSegLineToVerticalAbs:
        return SVGPathSegLinetoVerticalAbs::create(element, y);
    case PathSegLineToVerticalRel:
        return SVGPathSegLinetoVerticalRel::create(element, y);
    default:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
}

PassOwnPtrWillBeRawPtr<InterpolableValue> pathSegCurvetoCubicSmoothToInterpolableValue(const SVGPathSegCurvetoCubicSmooth& item, SubPathCoordinates& coordinates)
{
    bool isAbsolute = isAbsolutePathSegType(item);
    OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(4);
    result->set(0, controlToInterpolableValue(item.x2(), isAbsolute, coordinates.currentX));
    result->set(1, controlToInterpolableValue(item.y2(), isAbsolute, coordinates.currentY));
    result->set(2, specifiedToInterpolableValue(item.x(), isAbsolute, coordinates.currentX));
    result->set(3, specifiedToInterpolableValue(item.y(), isAbsolute, coordinates.currentY));
    return result.release();
}

PassRefPtrWillBeRawPtr<SVGPathSeg> pathSegCurvetoCubicSmoothFromInterpolableValue(const InterpolableValue& value, SVGPathSegType segType, SubPathCoordinates& coordinates, SVGPathElement* element)
{
    const InterpolableList& list = toInterpolableList(value);
    bool isAbsolute = isAbsolutePathSegType(segType);
    float x2 = controlFromInterpolableValue(list.get(0), isAbsolute, coordinates.currentX);
    float y2 = controlFromInterpolableValue(list.get(1), isAbsolute, coordinates.currentY);
    float x = specifiedFromInterpolableValue(list.get(2), isAbsolute, coordinates.currentX);
    float y = specifiedFromInterpolableValue(list.get(3), isAbsolute, coordinates.currentY);
    switch (segType) {
    case PathSegCurveToCubicSmoothAbs:
        return SVGPathSegCurvetoCubicSmoothAbs::create(element, x, y, x2, y2);
    case PathSegCurveToCubicSmoothRel:
        return SVGPathSegCurvetoCubicSmoothRel::create(element, x, y, x2, y2);
    default:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
}

PassOwnPtrWillBeRawPtr<InterpolableValue> pathSegToInterpolableValue(const SVGPathSeg& item, SubPathCoordinates& coordinates, SVGPathSegType* ptrSegType)
{
    SVGPathSegType segType = static_cast<SVGPathSegType>(item.pathSegType());

    if (ptrSegType)
        *ptrSegType = segType;

    switch (segType) {
    case PathSegClosePath:
        return pathSegClosePathToInterpolableValue(item, coordinates);

    case PathSegMoveToAbs:
    case PathSegMoveToRel:
    case PathSegLineToAbs:
    case PathSegLineToRel:
    case PathSegCurveToQuadraticSmoothAbs:
    case PathSegCurveToQuadraticSmoothRel:
        return pathSegSingleCoordinateToInterpolableValue(static_cast<const SVGPathSegSingleCoordinate&>(item), coordinates);

    case PathSegCurveToCubicAbs:
    case PathSegCurveToCubicRel:
        return pathSegCurvetoCubicToInterpolableValue(static_cast<const SVGPathSegCurvetoCubic&>(item), coordinates);

    case PathSegCurveToQuadraticAbs:
    case PathSegCurveToQuadraticRel:
        return pathSegCurvetoQuadraticToInterpolableValue(static_cast<const SVGPathSegCurvetoQuadratic&>(item), coordinates);

    case PathSegArcAbs:
    case PathSegArcRel:
        return pathSegArcToInterpolableValue(static_cast<const SVGPathSegArc&>(item), coordinates);

    case PathSegLineToHorizontalAbs:
    case PathSegLineToHorizontalRel:
        return pathSegLinetoHorizontalToInterpolableValue(static_cast<const SVGPathSegLinetoHorizontal&>(item), coordinates);

    case PathSegLineToVerticalAbs:
    case PathSegLineToVerticalRel:
        return pathSegLinetoVerticalToInterpolableValue(static_cast<const SVGPathSegLinetoVertical&>(item), coordinates);

    case PathSegCurveToCubicSmoothAbs:
    case PathSegCurveToCubicSmoothRel:
        return pathSegCurvetoCubicSmoothToInterpolableValue(static_cast<const SVGPathSegCurvetoCubicSmooth&>(item), coordinates);

    case PathSegUnknown:
        ASSERT_NOT_REACHED();
    }
    ASSERT_NOT_REACHED();
    return nullptr;
}

PassRefPtrWillBeRawPtr<SVGPathSeg> pathSegFromInterpolableValue(const InterpolableValue& value, SVGPathSegType segType, SubPathCoordinates& coordinates, SVGPathElement* element)
{
    switch (segType) {
    case PathSegClosePath:
        return pathSegClosePathFromInterpolableValue(value, segType, coordinates, element);

    case PathSegMoveToAbs:
    case PathSegMoveToRel:
    case PathSegLineToAbs:
    case PathSegLineToRel:
    case PathSegCurveToQuadraticSmoothAbs:
    case PathSegCurveToQuadraticSmoothRel:
        return pathSegSingleCoordinateFromInterpolableValue(value, segType, coordinates, element);

    case PathSegCurveToCubicAbs:
    case PathSegCurveToCubicRel:
        return pathSegCurvetoCubicFromInterpolableValue(value, segType, coordinates, element);

    case PathSegCurveToQuadraticAbs:
    case PathSegCurveToQuadraticRel:
        return pathSegCurvetoQuadraticFromInterpolableValue(value, segType, coordinates, element);

    case PathSegArcAbs:
    case PathSegArcRel:
        return pathSegArcFromInterpolableValue(value, segType, coordinates, element);

    case PathSegLineToHorizontalAbs:
    case PathSegLineToHorizontalRel:
        return pathSegLinetoHorizontalFromInterpolableValue(value, segType, coordinates, element);

    case PathSegLineToVerticalAbs:
    case PathSegLineToVerticalRel:
        return pathSegLinetoVerticalFromInterpolableValue(value, segType, coordinates, element);

    case PathSegCurveToCubicSmoothAbs:
    case PathSegCurveToCubicSmoothRel:
        return pathSegCurvetoCubicSmoothFromInterpolableValue(value, segType, coordinates, element);

    case PathSegUnknown:
        ASSERT_NOT_REACHED();
    }
    ASSERT_NOT_REACHED();
    return nullptr;
}

} // namespace

PassRefPtrWillBeRawPtr<PathSVGInterpolation> PathSVGInterpolation::maybeCreate(SVGPropertyBase* start, SVGPropertyBase* end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute)
{
    ASSERT(start->type() == SVGPathSegList::classType());
    ASSERT(end->type() == SVGPathSegList::classType());

    SVGPathSegList* startList = static_cast<SVGPathSegList*>(start);
    SVGPathSegList* endList = static_cast<SVGPathSegList*>(end);
    size_t length = startList->length();
    if (length != endList->length())
        return nullptr;

    Vector<SVGPathSegType> pathSegTypes(length);
    OwnPtrWillBeRawPtr<InterpolableList> startValue = InterpolableList::create(length);
    OwnPtrWillBeRawPtr<InterpolableList> endValue = InterpolableList::create(length);
    SubPathCoordinates startCoordinates;
    SubPathCoordinates endCoordinates;
    for (size_t i = 0; i < length; i++) {
        if (absolutePathSegType(*startList->at(i)) != absolutePathSegType(*endList->at(i)))
            return nullptr;

        // Like Firefox SMIL, we use the final path seg type.
        startValue->set(i, pathSegToInterpolableValue(*startList->at(i), startCoordinates, nullptr));
        endValue->set(i, pathSegToInterpolableValue(*endList->at(i), endCoordinates, &pathSegTypes.at(i)));
    }

    return adoptRefWillBeNoop(new PathSVGInterpolation(startValue.release(), endValue.release(), attribute, pathSegTypes));
}

PassRefPtrWillBeRawPtr<SVGPropertyBase> PathSVGInterpolation::fromInterpolableValue(const InterpolableValue& value, const Vector<SVGPathSegType>& pathSegTypes, SVGPathElement* element)
{
    const InterpolableList& listValue = toInterpolableList(value);
    RefPtrWillBeRawPtr<SVGPathSegList> result = SVGPathSegList::create(element);
    SubPathCoordinates coordinates;
    for (size_t i = 0; i < listValue.length(); i++)
        result->append(pathSegFromInterpolableValue(*listValue.get(i), pathSegTypes.at(i), coordinates, element));
    return result.release();
}

PassRefPtrWillBeRawPtr<SVGPropertyBase> PathSVGInterpolation::interpolatedValue(SVGElement& element) const
{
    return fromInterpolableValue(*m_cachedValue, m_pathSegTypes, toSVGPathElement(&element));
}

}
