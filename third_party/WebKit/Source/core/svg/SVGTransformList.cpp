/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 * Copyright (C) Research In Motion Limited 2012. All rights reserved.
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

#include "core/svg/SVGTransformList.h"

#include "core/SVGNames.h"
#include "core/svg/SVGAnimateTransformElement.h"
#include "core/svg/SVGAnimatedNumber.h"
#include "core/svg/SVGParserUtilities.h"
#include "core/svg/SVGTransformDistance.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"

namespace blink {

inline PassRefPtrWillBeRawPtr<SVGTransformList> toSVGTransformList(PassRefPtrWillBeRawPtr<SVGPropertyBase> passBase)
{
    RefPtrWillBeRawPtr<SVGPropertyBase> base = passBase;
    ASSERT(base->type() == SVGTransformList::classType());
    return static_pointer_cast<SVGTransformList>(base.release());
}

SVGTransformList::SVGTransformList()
{
}

SVGTransformList::~SVGTransformList()
{
}

PassRefPtrWillBeRawPtr<SVGTransform> SVGTransformList::consolidate()
{
    AffineTransform matrix;
    if (!concatenate(matrix))
        return SVGTransform::create();

    RefPtrWillBeRawPtr<SVGTransform> transform = SVGTransform::create(matrix);
    clear();
    return appendItem(transform);
}

bool SVGTransformList::concatenate(AffineTransform& result) const
{
    if (isEmpty())
        return false;

    ConstIterator it = begin();
    ConstIterator itEnd = end();
    for (; it != itEnd; ++it)
        result *= it->matrix();

    return true;
}

namespace {

template<typename CharType>
int parseTransformParamList(const CharType*& ptr, const CharType* end, float* values, int required, int optional)
{
    int parsedParams = 0;
    int maxPossibleParams = required + optional;

    bool trailingDelimiter = false;

    skipOptionalSVGSpaces(ptr, end);
    while (parsedParams < maxPossibleParams) {
        if (!parseNumber(ptr, end, values[parsedParams], DisallowWhitespace))
            break;

        ++parsedParams;

        if (skipOptionalSVGSpaces(ptr, end) && *ptr == ',') {
            ++ptr;
            skipOptionalSVGSpaces(ptr, end);

            trailingDelimiter = true;
        } else {
            trailingDelimiter = false;
        }
    }

    if (trailingDelimiter || !(parsedParams == required || parsedParams == maxPossibleParams))
        return -1;

    return parsedParams;
}

// These should be kept in sync with enum SVGTransformType
static const int requiredValuesForType[] =  {0, 6, 1, 1, 1, 1, 1};
static const int optionalValuesForType[] =  {0, 0, 1, 1, 2, 0, 0};

template<typename CharType>
PassRefPtrWillBeRawPtr<SVGTransform> parseTransformOfType(unsigned type, const CharType*& ptr, const CharType* end)
{
    if (type == SVG_TRANSFORM_UNKNOWN)
        return nullptr;

    int valueCount = 0;
    float values[] = {0, 0, 0, 0, 0, 0};
    if ((valueCount = parseTransformParamList(ptr, end, values, requiredValuesForType[type], optionalValuesForType[type])) < 0) {
        return nullptr;
    }

    RefPtrWillBeRawPtr<SVGTransform> transform = SVGTransform::create();

    switch (type) {
    case SVG_TRANSFORM_SKEWX:
        transform->setSkewX(values[0]);
        break;
    case SVG_TRANSFORM_SKEWY:
        transform->setSkewY(values[0]);
        break;
    case SVG_TRANSFORM_SCALE:
        if (valueCount == 1) // Spec: if only one param given, assume uniform scaling
            transform->setScale(values[0], values[0]);
        else
            transform->setScale(values[0], values[1]);
        break;
    case SVG_TRANSFORM_TRANSLATE:
        if (valueCount == 1) // Spec: if only one param given, assume 2nd param to be 0
            transform->setTranslate(values[0], 0);
        else
            transform->setTranslate(values[0], values[1]);
        break;
    case SVG_TRANSFORM_ROTATE:
        if (valueCount == 1)
            transform->setRotate(values[0], 0, 0);
        else
            transform->setRotate(values[0], values[1], values[2]);
        break;
    case SVG_TRANSFORM_MATRIX:
        transform->setMatrix(AffineTransform(values[0], values[1], values[2], values[3], values[4], values[5]));
        break;
    }

    return transform.release();
}

}

template<typename CharType>
bool SVGTransformList::parseInternal(const CharType*& ptr, const CharType* end)
{
    clear();

    bool delimParsed = false;
    while (ptr < end) {
        delimParsed = false;
        SVGTransformType transformType = SVG_TRANSFORM_UNKNOWN;
        skipOptionalSVGSpaces(ptr, end);

        if (!parseAndSkipTransformType(ptr, end, transformType))
            return false;

        if (!skipOptionalSVGSpaces(ptr, end) || *ptr != '(')
            return false;
        ptr++;

        RefPtrWillBeRawPtr<SVGTransform> transform = parseTransformOfType(transformType, ptr, end);
        if (!transform)
            return false;

        if (!skipOptionalSVGSpaces(ptr, end) || *ptr != ')')
            return false;
        ptr++;

        append(transform.release());

        skipOptionalSVGSpaces(ptr, end);
        if (ptr < end && *ptr == ',') {
            delimParsed = true;
            ++ptr;
            skipOptionalSVGSpaces(ptr, end);
        }
    }

    return !delimParsed;
}

bool SVGTransformList::parse(const UChar*& ptr, const UChar* end)
{
    return parseInternal(ptr, end);
}

bool SVGTransformList::parse(const LChar*& ptr, const LChar* end)
{
    return parseInternal(ptr, end);
}

String SVGTransformList::valueAsString() const
{
    StringBuilder builder;

    ConstIterator it = begin();
    ConstIterator itEnd = end();
    while (it != itEnd) {
        builder.append(it->valueAsString());
        ++it;
        if (it != itEnd)
            builder.append(' ');
    }

    return builder.toString();
}

void SVGTransformList::setValueAsString(const String& value, ExceptionState& exceptionState)
{
    if (value.isEmpty()) {
        clear();
        return;
    }

    bool valid = false;
    if (value.is8Bit()) {
        const LChar* ptr = value.characters8();
        const LChar* end = ptr + value.length();
        valid = parse(ptr, end);
    } else {
        const UChar* ptr = value.characters16();
        const UChar* end = ptr + value.length();
        valid = parse(ptr, end);
    }

    if (!valid) {
        clear();
        exceptionState.throwDOMException(SyntaxError, "Problem parsing transform list=\""+value+"\"");
    }
}

PassRefPtrWillBeRawPtr<SVGPropertyBase> SVGTransformList::cloneForAnimation(const String& value) const
{
    ASSERT(RuntimeEnabledFeatures::webAnimationsSVGEnabled());
    return SVGListPropertyHelper::cloneForAnimation(value);
}

PassRefPtrWillBeRawPtr<SVGTransformList> SVGTransformList::create(SVGTransformType transformType, const String& value)
{
    RefPtrWillBeRawPtr<SVGTransform> transform = nullptr;
    if (value.isEmpty()) {
    } else if (value.is8Bit()) {
        const LChar* ptr = value.characters8();
        const LChar* end = ptr + value.length();
        transform = parseTransformOfType(transformType, ptr, end);
    } else {
        const UChar* ptr = value.characters16();
        const UChar* end = ptr + value.length();
        transform = parseTransformOfType(transformType, ptr, end);
    }

    RefPtrWillBeRawPtr<SVGTransformList> svgTransformList = SVGTransformList::create();
    if (transform)
        svgTransformList->append(transform);
    return svgTransformList.release();
}

void SVGTransformList::add(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement* contextElement)
{
    if (isEmpty())
        return;

    RefPtrWillBeRawPtr<SVGTransformList> otherList = toSVGTransformList(other);
    if (length() != otherList->length())
        return;

    ASSERT(length() == 1);
    RefPtrWillBeRawPtr<SVGTransform> fromTransform = at(0);
    RefPtrWillBeRawPtr<SVGTransform> toTransform = otherList->at(0);

    ASSERT(fromTransform->transformType() == toTransform->transformType());
    clear();
    append(SVGTransformDistance::addSVGTransforms(fromTransform, toTransform));
}

void SVGTransformList::calculateAnimatedValue(SVGAnimationElement* animationElement, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> fromValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement* contextElement)
{
    ASSERT(animationElement);
    bool isToAnimation = animationElement->animationMode() == ToAnimation;

    // Spec: To animations provide specific functionality to get a smooth change from the underlying value to the
    // 'to' attribute value, which conflicts mathematically with the requirement for additive transform animations
    // to be post-multiplied. As a consequence, in SVG 1.1 the behavior of to animations for 'animateTransform' is undefined
    // FIXME: This is not taken into account yet.
    RefPtrWillBeRawPtr<SVGTransformList> fromList = isToAnimation ? PassRefPtrWillBeRawPtr<SVGTransformList>(this) : toSVGTransformList(fromValue);
    RefPtrWillBeRawPtr<SVGTransformList> toList = toSVGTransformList(toValue);
    RefPtrWillBeRawPtr<SVGTransformList> toAtEndOfDurationList = toSVGTransformList(toAtEndOfDurationValue);

    size_t toListSize = toList->length();
    if (!toListSize)
        return;

    // Get a reference to the from value before potentially cleaning it out (in the case of a To animation.)
    RefPtrWillBeRawPtr<SVGTransform> toTransform = toList->at(0);
    RefPtrWillBeRawPtr<SVGTransform> effectiveFrom = nullptr;
    // If there's an existing 'from'/underlying value of the same type use that, else use a "zero transform".
    if (fromList->length() && fromList->at(0)->transformType() == toTransform->transformType())
        effectiveFrom = fromList->at(0);
    else
        effectiveFrom = SVGTransform::create(toTransform->transformType(), SVGTransform::ConstructZeroTransform);

    // Never resize the animatedTransformList to the toList size, instead either clear the list or append to it.
    if (!isEmpty() && (!animationElement->isAdditive() || isToAnimation))
        clear();

    RefPtrWillBeRawPtr<SVGTransform> currentTransform = SVGTransformDistance(effectiveFrom, toTransform).scaledDistance(percentage).addToSVGTransform(effectiveFrom);
    if (animationElement->isAccumulated() && repeatCount) {
        RefPtrWillBeRawPtr<SVGTransform> effectiveToAtEnd = !toAtEndOfDurationList->isEmpty() ? PassRefPtrWillBeRawPtr<SVGTransform>(toAtEndOfDurationList->at(0)) : SVGTransform::create(toTransform->transformType(), SVGTransform::ConstructZeroTransform);
        append(SVGTransformDistance::addSVGTransforms(currentTransform, effectiveToAtEnd, repeatCount));
    } else {
        append(currentTransform);
    }
}

float SVGTransformList::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, SVGElement*)
{
    // FIXME: This is not correct in all cases. The spec demands that each component (translate x and y for example)
    // is paced separately. To implement this we need to treat each component as individual animation everywhere.

    RefPtrWillBeRawPtr<SVGTransformList> toList = toSVGTransformList(toValue);
    if (isEmpty() || length() != toList->length())
        return -1;

    ASSERT(length() == 1);
    if (at(0)->transformType() == toList->at(0)->transformType())
        return -1;

    // Spec: http://www.w3.org/TR/SVG/animate.html#complexDistances
    // Paced animations assume a notion of distance between the various animation values defined by the 'to', 'from', 'by' and 'values' attributes.
    // Distance is defined only for scalar types (such as <length>), colors and the subset of transformation types that are supported by 'animateTransform'.
    return SVGTransformDistance(at(0), toList->at(0)).distance();
}

}
