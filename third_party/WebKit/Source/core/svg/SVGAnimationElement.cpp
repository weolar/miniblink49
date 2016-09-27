/*
 * Copyright (C) 2004, 2005 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Cameron McCormack <cam@mcc.id.au>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
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
#include "core/svg/SVGAnimationElement.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/CSSPropertyNames.h"
#include "core/SVGNames.h"
#include "core/css/CSSComputedStyleDeclaration.h"
#include "core/css/parser/CSSParser.h"
#include "core/frame/UseCounter.h"
#include "core/svg/SVGAnimateElement.h"
#include "core/svg/SVGElement.h"
#include "core/svg/SVGParserUtilities.h"
#include "platform/FloatConversion.h"
#include "wtf/MathExtras.h"

namespace blink {

SVGAnimationElement::SVGAnimationElement(const QualifiedName& tagName, Document& document)
    : SVGSMILElement(tagName, document)
    , m_fromPropertyValueType(RegularPropertyValue)
    , m_toPropertyValueType(RegularPropertyValue)
    , m_animationValid(false)
    , m_attributeType(AttributeTypeAuto)
    , m_hasInvalidCSSAttributeType(false)
    , m_calcMode(CalcModeLinear)
    , m_animationMode(NoAnimation)
{
    UseCounter::count(document, UseCounter::SVGAnimationElement);
}

bool SVGAnimationElement::parseValues(const String& value, Vector<String>& result)
{
    // Per the SMIL specification, leading and trailing white space,
    // and white space before and after semicolon separators, is allowed and will be ignored.
    // http://www.w3.org/TR/SVG11/animate.html#ValuesAttribute
    result.clear();
    Vector<String> parseList;
    value.split(';', true, parseList);
    unsigned last = parseList.size() - 1;
    for (unsigned i = 0; i <= last; ++i) {
        if (parseList[i].isEmpty()) {
            // Tolerate trailing ';'
            if (i < last)
                goto fail;
        } else {
            parseList[i] = parseList[i].stripWhiteSpace();
            result.append(parseList[i]);
        }
    }

    return true;
fail:
    result.clear();
    return false;
}

static bool parseKeyTimes(const String& string, Vector<float>& result, bool verifyOrder)
{
    result.clear();
    Vector<String> parseList;
    string.split(';', true, parseList);
    for (unsigned n = 0; n < parseList.size(); ++n) {
        String timeString = parseList[n].stripWhiteSpace();
        bool ok;
        float time = timeString.toFloat(&ok);
        if (!ok || time < 0 || time > 1)
            goto fail;
        if (verifyOrder) {
            if (!n) {
                if (time)
                    goto fail;
            } else if (time < result.last())
                goto fail;
        }
        result.append(time);
    }
    return true;
fail:
    result.clear();
    return false;
}

template<typename CharType>
static bool parseKeySplinesInternal(const String& string, Vector<UnitBezier>& result)
{
    const CharType* ptr = string.getCharacters<CharType>();
    const CharType* end = ptr + string.length();

    skipOptionalSVGSpaces(ptr, end);

    while (ptr < end) {
        float posA = 0;
        if (!parseNumber(ptr, end, posA))
            return false;

        float posB = 0;
        if (!parseNumber(ptr, end, posB))
            return false;

        float posC = 0;
        if (!parseNumber(ptr, end, posC))
            return false;

        float posD = 0;
        if (!parseNumber(ptr, end, posD, DisallowWhitespace))
            return false;

        skipOptionalSVGSpaces(ptr, end);

        if (ptr < end && *ptr == ';')
            ptr++;
        skipOptionalSVGSpaces(ptr, end);

        result.append(UnitBezier(posA, posB, posC, posD));
    }

    return ptr == end;
}

static bool parseKeySplines(const String& string, Vector<UnitBezier>& result)
{
    result.clear();
    if (string.isEmpty())
        return true;
    bool parsed = true;
    if (string.is8Bit())
        parsed = parseKeySplinesInternal<LChar>(string, result);
    else
        parsed = parseKeySplinesInternal<UChar>(string, result);
    if (!parsed) {
        result.clear();
        return false;
    }
    return true;
}

void SVGAnimationElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == SVGNames::valuesAttr) {
        if (!parseValues(value, m_values)) {
            reportAttributeParsingError(ParsingAttributeFailedError, name, value);
            return;
        }
        updateAnimationMode();
        return;
    }

    if (name == SVGNames::keyTimesAttr) {
        if (!parseKeyTimes(value, m_keyTimes, true))
            reportAttributeParsingError(ParsingAttributeFailedError, name, value);
        return;
    }

    if (name == SVGNames::keyPointsAttr) {
        if (isSVGAnimateMotionElement(*this)) {
            // This is specified to be an animateMotion attribute only but it is simpler to put it here
            // where the other timing calculatations are.
            if (!parseKeyTimes(value, m_keyPoints, false))
                reportAttributeParsingError(ParsingAttributeFailedError, name, value);
        }
        return;
    }

    if (name == SVGNames::keySplinesAttr) {
        if (!parseKeySplines(value, m_keySplines))
            reportAttributeParsingError(ParsingAttributeFailedError, name, value);
        return;
    }

    if (name == SVGNames::attributeTypeAttr) {
        setAttributeType(value);
        return;
    }

    if (name == SVGNames::calcModeAttr) {
        setCalcMode(value);
        return;
    }

    if (name == SVGNames::fromAttr || name == SVGNames::toAttr || name == SVGNames::byAttr) {
        updateAnimationMode();
        return;
    }

    SVGSMILElement::parseAttribute(name, value);
}

void SVGAnimationElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::valuesAttr
        || attrName == SVGNames::byAttr
        || attrName == SVGNames::fromAttr
        || attrName == SVGNames::toAttr
        || attrName == SVGNames::calcModeAttr
        || attrName == SVGNames::attributeTypeAttr
        || attrName == SVGNames::keySplinesAttr
        || attrName == SVGNames::keyPointsAttr
        || attrName == SVGNames::keyTimesAttr) {
        animationAttributeChanged();
        return;
    }

    SVGSMILElement::svgAttributeChanged(attrName);
}

void SVGAnimationElement::animationAttributeChanged()
{
    // Assumptions may not hold after an attribute change.
    m_animationValid = false;
    m_lastValuesAnimationFrom = String();
    m_lastValuesAnimationTo = String();
    setInactive();
}

float SVGAnimationElement::getStartTime(ExceptionState& exceptionState) const
{
    SMILTime startTime = intervalBegin();
    if (!startTime.isFinite()) {
        exceptionState.throwDOMException(InvalidStateError, "No current interval.");
        return 0;
    }
    return narrowPrecisionToFloat(startTime.value());
}

float SVGAnimationElement::getCurrentTime() const
{
    return narrowPrecisionToFloat(elapsed().value());
}

float SVGAnimationElement::getSimpleDuration(ExceptionState& exceptionState) const
{
    SMILTime duration = simpleDuration();
    if (!duration.isFinite()) {
        exceptionState.throwDOMException(NotSupportedError, "No simple duration defined.");
        return 0;
    }
    return narrowPrecisionToFloat(duration.value());
}

void SVGAnimationElement::beginElement()
{
    beginElementAt(0);
}

void SVGAnimationElement::beginElementAt(float offset)
{
    ASSERT(std::isfinite(offset));
    SMILTime elapsed = this->elapsed();
    addBeginTime(elapsed, elapsed + offset, SMILTimeWithOrigin::ScriptOrigin);
}

void SVGAnimationElement::endElement()
{
    endElementAt(0);
}

void SVGAnimationElement::endElementAt(float offset)
{
    ASSERT(std::isfinite(offset));
    SMILTime elapsed = this->elapsed();
    addEndTime(elapsed, elapsed + offset, SMILTimeWithOrigin::ScriptOrigin);
}

void SVGAnimationElement::updateAnimationMode()
{
    // http://www.w3.org/TR/2001/REC-smil-animation-20010904/#AnimFuncValues
    if (hasAttribute(SVGNames::valuesAttr))
        setAnimationMode(ValuesAnimation);
    else if (!toValue().isEmpty())
        setAnimationMode(fromValue().isEmpty() ? ToAnimation : FromToAnimation);
    else if (!byValue().isEmpty())
        setAnimationMode(fromValue().isEmpty() ? ByAnimation : FromByAnimation);
    else
        setAnimationMode(NoAnimation);
}

void SVGAnimationElement::setCalcMode(const AtomicString& calcMode)
{
    DEFINE_STATIC_LOCAL(const AtomicString, discrete, ("discrete", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, linear, ("linear", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, paced, ("paced", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, spline, ("spline", AtomicString::ConstructFromLiteral));
    if (calcMode == discrete)
        setCalcMode(CalcModeDiscrete);
    else if (calcMode == linear)
        setCalcMode(CalcModeLinear);
    else if (calcMode == paced)
        setCalcMode(CalcModePaced);
    else if (calcMode == spline)
        setCalcMode(CalcModeSpline);
    else
        setCalcMode(isSVGAnimateMotionElement(*this) ? CalcModePaced : CalcModeLinear);
}

void SVGAnimationElement::setAttributeType(const AtomicString& attributeType)
{
    DEFINE_STATIC_LOCAL(const AtomicString, css, ("CSS", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, xml, ("XML", AtomicString::ConstructFromLiteral));
    if (attributeType == css)
        m_attributeType = AttributeTypeCSS;
    else if (attributeType == xml)
        m_attributeType = AttributeTypeXML;
    else
        m_attributeType = AttributeTypeAuto;
    checkInvalidCSSAttributeType();
}

String SVGAnimationElement::toValue() const
{
    return fastGetAttribute(SVGNames::toAttr);
}

String SVGAnimationElement::byValue() const
{
    return fastGetAttribute(SVGNames::byAttr);
}

String SVGAnimationElement::fromValue() const
{
    return fastGetAttribute(SVGNames::fromAttr);
}

bool SVGAnimationElement::isAdditive()
{
    DEFINE_STATIC_LOCAL(const AtomicString, sum, ("sum", AtomicString::ConstructFromLiteral));
    const AtomicString& value = fastGetAttribute(SVGNames::additiveAttr);
    return value == sum || animationMode() == ByAnimation;
}

bool SVGAnimationElement::isAccumulated() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, sum, ("sum", AtomicString::ConstructFromLiteral));
    const AtomicString& value = fastGetAttribute(SVGNames::accumulateAttr);
    return value == sum && animationMode() != ToAnimation;
}

bool SVGAnimationElement::isTargetAttributeCSSProperty(SVGElement* targetElement, const QualifiedName& attributeName)
{
    ASSERT(targetElement);

    return SVGElement::isAnimatableCSSProperty(attributeName) || targetElement->isPresentationAttribute(attributeName);
}

SVGAnimationElement::ShouldApplyAnimation SVGAnimationElement::shouldApplyAnimation(SVGElement* targetElement, const QualifiedName& attributeName)
{
    if (!hasValidAttributeType() || !targetElement || attributeName == anyQName() || !targetElement->inActiveDocument())
        return DontApplyAnimation;

    // Always animate CSS properties, using the ApplyCSSAnimation code path, regardless of the attributeType value.
    if (isTargetAttributeCSSProperty(targetElement, attributeName)) {
        if (targetElement->isPresentationAttributeWithSVGDOM(attributeName))
            return ApplyXMLandCSSAnimation;

        return ApplyCSSAnimation;
    }
    // If attributeType="CSS" and attributeName doesn't point to a CSS property, ignore the animation.
    if (attributeType() == AttributeTypeCSS)
        return DontApplyAnimation;

    return ApplyXMLAnimation;
}

void SVGAnimationElement::calculateKeyTimesForCalcModePaced()
{
    ASSERT(calcMode() == CalcModePaced);
    ASSERT(animationMode() == ValuesAnimation);

    unsigned valuesCount = m_values.size();
    ASSERT(valuesCount >= 1);
    if (valuesCount == 1)
        return;

    // FIXME, webkit.org/b/109010: m_keyTimes should not be modified in this function.
    m_keyTimes.clear();

    Vector<float> keyTimesForPaced;
    float totalDistance = 0;
    keyTimesForPaced.append(0);
    for (unsigned n = 0; n < valuesCount - 1; ++n) {
        // Distance in any units
        float distance = calculateDistance(m_values[n], m_values[n + 1]);
        if (distance < 0)
            return;
        totalDistance += distance;
        keyTimesForPaced.append(distance);
    }
    if (!totalDistance)
        return;

    // Normalize.
    for (unsigned n = 1; n < keyTimesForPaced.size() - 1; ++n)
        keyTimesForPaced[n] = keyTimesForPaced[n - 1] + keyTimesForPaced[n] / totalDistance;
    keyTimesForPaced[keyTimesForPaced.size() - 1] = 1;

    // Use key times calculated based on pacing instead of the user provided ones.
    m_keyTimes = keyTimesForPaced;
}

static inline double solveEpsilon(double duration) { return 1 / (200 * duration); }

unsigned SVGAnimationElement::calculateKeyTimesIndex(float percent) const
{
    unsigned index;
    unsigned keyTimesCount = m_keyTimes.size();
    // For linear and spline animations, the last value must be '1'. In those
    // cases we don't need to consider the last value, since |percent| is never
    // greater than one.
    if (keyTimesCount && calcMode() != CalcModeDiscrete)
        keyTimesCount--;
    for (index = 1; index < keyTimesCount; ++index) {
        if (m_keyTimes[index] > percent)
            break;
    }
    return --index;
}

float SVGAnimationElement::calculatePercentForSpline(float percent, unsigned splineIndex) const
{
    ASSERT(calcMode() == CalcModeSpline);
    ASSERT_WITH_SECURITY_IMPLICATION(splineIndex < m_keySplines.size());
    UnitBezier bezier = m_keySplines[splineIndex];
    SMILTime duration = simpleDuration();
    if (!duration.isFinite())
        duration = 100.0;
    return narrowPrecisionToFloat(bezier.solve(percent, solveEpsilon(duration.value())));
}

float SVGAnimationElement::calculatePercentFromKeyPoints(float percent) const
{
    ASSERT(!m_keyPoints.isEmpty());
    ASSERT(calcMode() != CalcModePaced);
    ASSERT(m_keyTimes.size() > 1);
    ASSERT(m_keyPoints.size() == m_keyTimes.size());

    if (percent == 1)
        return m_keyPoints[m_keyPoints.size() - 1];

    unsigned index = calculateKeyTimesIndex(percent);
    float fromKeyPoint = m_keyPoints[index];

    if (calcMode() == CalcModeDiscrete)
        return fromKeyPoint;

    ASSERT(index + 1 < m_keyTimes.size());
    float fromPercent = m_keyTimes[index];
    float toPercent = m_keyTimes[index + 1];
    float toKeyPoint = m_keyPoints[index + 1];
    float keyPointPercent = (percent - fromPercent) / (toPercent - fromPercent);

    if (calcMode() == CalcModeSpline) {
        ASSERT(m_keySplines.size() == m_keyPoints.size() - 1);
        keyPointPercent = calculatePercentForSpline(keyPointPercent, index);
    }
    return (toKeyPoint - fromKeyPoint) * keyPointPercent + fromKeyPoint;
}

float SVGAnimationElement::calculatePercentForFromTo(float percent) const
{
    if (calcMode() == CalcModeDiscrete && m_keyTimes.size() == 2)
        return percent > m_keyTimes[1] ? 1 : 0;

    return percent;
}

void SVGAnimationElement::currentValuesFromKeyPoints(float percent, float& effectivePercent, String& from, String& to) const
{
    ASSERT(!m_keyPoints.isEmpty());
    ASSERT(m_keyPoints.size() == m_keyTimes.size());
    ASSERT(calcMode() != CalcModePaced);
    effectivePercent = calculatePercentFromKeyPoints(percent);
    unsigned index = effectivePercent == 1 ? m_values.size() - 2 : static_cast<unsigned>(effectivePercent * (m_values.size() - 1));
    from = m_values[index];
    to = m_values[index + 1];
}

void SVGAnimationElement::currentValuesForValuesAnimation(float percent, float& effectivePercent, String& from, String& to)
{
    unsigned valuesCount = m_values.size();
    ASSERT(m_animationValid);
    ASSERT(valuesCount >= 1);

    if (percent == 1 || valuesCount == 1) {
        from = m_values[valuesCount - 1];
        to = m_values[valuesCount - 1];
        effectivePercent = 1;
        return;
    }

    CalcMode calcMode = this->calcMode();
    if (isSVGAnimateElement(*this)) {
        SVGAnimateElement& animateElement = toSVGAnimateElement(*this);
        if (!animateElement.animatedPropertyTypeSupportsAddition()) {
            ASSERT(animateElement.animatedPropertyType() != AnimatedTransformList || isSVGAnimateTransformElement(*this));
            ASSERT(animateElement.animatedPropertyType() != AnimatedUnknown);
            calcMode = CalcModeDiscrete;
        }
    }
    if (!m_keyPoints.isEmpty() && calcMode != CalcModePaced)
        return currentValuesFromKeyPoints(percent, effectivePercent, from, to);

    unsigned keyTimesCount = m_keyTimes.size();
    ASSERT(!keyTimesCount || valuesCount == keyTimesCount);
    ASSERT(!keyTimesCount || (keyTimesCount > 1 && !m_keyTimes[0]));

    unsigned index = calculateKeyTimesIndex(percent);
    if (calcMode == CalcModeDiscrete) {
        if (!keyTimesCount)
            index = static_cast<unsigned>(percent * valuesCount);
        from = m_values[index];
        to = m_values[index];
        effectivePercent = 0;
        return;
    }

    float fromPercent;
    float toPercent;
    if (keyTimesCount) {
        fromPercent = m_keyTimes[index];
        toPercent = m_keyTimes[index + 1];
    } else {
        index = static_cast<unsigned>(floorf(percent * (valuesCount - 1)));
        fromPercent =  static_cast<float>(index) / (valuesCount - 1);
        toPercent =  static_cast<float>(index + 1) / (valuesCount - 1);
    }

    if (index == valuesCount - 1)
        --index;
    from = m_values[index];
    to = m_values[index + 1];
    ASSERT(toPercent > fromPercent);
    effectivePercent = (percent - fromPercent) / (toPercent - fromPercent);

    if (calcMode == CalcModeSpline) {
        ASSERT(m_keySplines.size() == m_values.size() - 1);
        effectivePercent = calculatePercentForSpline(effectivePercent, index);
    }
}

void SVGAnimationElement::startedActiveInterval()
{
    m_animationValid = false;

    if (!isValid())
        return;

    if (!hasValidAttributeType())
        return;

    // These validations are appropriate for all animation modes.
    if (fastHasAttribute(SVGNames::keyPointsAttr) && m_keyPoints.size() != m_keyTimes.size())
        return;

    AnimationMode animationMode = this->animationMode();
    CalcMode calcMode = this->calcMode();
    if (calcMode == CalcModeSpline) {
        unsigned splinesCount = m_keySplines.size();
        if (!splinesCount
            || (fastHasAttribute(SVGNames::keyPointsAttr) && m_keyPoints.size() - 1 != splinesCount)
            || (animationMode == ValuesAnimation && m_values.size() - 1 != splinesCount)
            || (fastHasAttribute(SVGNames::keyTimesAttr) && m_keyTimes.size() - 1 != splinesCount))
            return;
    }

    String from = fromValue();
    String to = toValue();
    String by = byValue();
    if (animationMode == NoAnimation)
        return;
    if ((animationMode == FromToAnimation || animationMode == FromByAnimation || animationMode == ToAnimation || animationMode == ByAnimation)
        && (fastHasAttribute(SVGNames::keyPointsAttr) && fastHasAttribute(SVGNames::keyTimesAttr) && (m_keyTimes.size() < 2 || m_keyTimes.size() != m_keyPoints.size())))
        return;
    if (animationMode == FromToAnimation)
        m_animationValid = calculateFromAndToValues(from, to);
    else if (animationMode == ToAnimation) {
        // For to-animations the from value is the current accumulated value from lower priority animations.
        // The value is not static and is determined during the animation.
        m_animationValid = calculateFromAndToValues(emptyString(), to);
    } else if (animationMode == FromByAnimation)
        m_animationValid = calculateFromAndByValues(from, by);
    else if (animationMode == ByAnimation)
        m_animationValid = calculateFromAndByValues(emptyString(), by);
    else if (animationMode == ValuesAnimation) {
        m_animationValid = m_values.size() >= 1
            && (calcMode == CalcModePaced || !fastHasAttribute(SVGNames::keyTimesAttr) || fastHasAttribute(SVGNames::keyPointsAttr) || (m_values.size() == m_keyTimes.size()))
            && (calcMode == CalcModeDiscrete || !m_keyTimes.size() || m_keyTimes.last() == 1)
            && (calcMode != CalcModeSpline || ((m_keySplines.size() && (m_keySplines.size() == m_values.size() - 1)) || m_keySplines.size() == m_keyPoints.size() - 1))
            && (!fastHasAttribute(SVGNames::keyPointsAttr) || (m_keyTimes.size() > 1 && m_keyTimes.size() == m_keyPoints.size()));
        if (m_animationValid)
            m_animationValid = calculateToAtEndOfDurationValue(m_values.last());
        if (calcMode == CalcModePaced && m_animationValid)
            calculateKeyTimesForCalcModePaced();
    } else if (animationMode == PathAnimation)
        m_animationValid = calcMode == CalcModePaced || !fastHasAttribute(SVGNames::keyPointsAttr) || (m_keyTimes.size() > 1 && m_keyTimes.size() == m_keyPoints.size());
}

void SVGAnimationElement::updateAnimation(float percent, unsigned repeatCount, SVGSMILElement* resultElement)
{
    if (!m_animationValid)
        return;

    float effectivePercent;
    CalcMode calcMode = this->calcMode();
    AnimationMode animationMode = this->animationMode();
    if (animationMode == ValuesAnimation) {
        String from;
        String to;
        currentValuesForValuesAnimation(percent, effectivePercent, from, to);
        if (from != m_lastValuesAnimationFrom || to != m_lastValuesAnimationTo) {
            m_animationValid = calculateFromAndToValues(from, to);
            if (!m_animationValid)
                return;
            m_lastValuesAnimationFrom = from;
            m_lastValuesAnimationTo = to;
        }
    } else if (!m_keyPoints.isEmpty() && calcMode != CalcModePaced)
        effectivePercent = calculatePercentFromKeyPoints(percent);
    else if (m_keyPoints.isEmpty() && calcMode == CalcModeSpline && m_keyTimes.size() > 1)
        effectivePercent = calculatePercentForSpline(percent, calculateKeyTimesIndex(percent));
    else if (animationMode == FromToAnimation || animationMode == ToAnimation)
        effectivePercent = calculatePercentForFromTo(percent);
    else
        effectivePercent = percent;

    calculateAnimatedValue(effectivePercent, repeatCount, resultElement);
}

void SVGAnimationElement::computeCSSPropertyValue(SVGElement* element, CSSPropertyID id, String& value)
{
    ASSERT(element);
    // FIXME: StyleEngine doesn't support document without a frame.
    // Refer to comment in Element::computedStyle.
    ASSERT(element->inActiveDocument());

    // Don't include any properties resulting from CSS Transitions/Animations or SMIL animations, as we want to retrieve the "base value".
    element->setUseOverrideComputedStyle(true);
    value = CSSComputedStyleDeclaration::create(element)->getPropertyValue(id);
    element->setUseOverrideComputedStyle(false);
}

void SVGAnimationElement::adjustForInheritance(SVGElement* targetElement, const QualifiedName& attributeName, String& value)
{
    // FIXME: At the moment the computed style gets returned as a String and needs to get parsed again.
    // In the future we might want to work with the value type directly to avoid the String parsing.
    ASSERT(targetElement);

    Element* parent = targetElement->parentElement();
    if (!parent || !parent->isSVGElement())
        return;

    SVGElement* svgParent = toSVGElement(parent);
    computeCSSPropertyValue(svgParent, cssPropertyID(attributeName.localName()), value);
}

static bool inheritsFromProperty(SVGElement* targetElement, const QualifiedName& attributeName, const String& value)
{
    ASSERT(targetElement);
    DEFINE_STATIC_LOCAL(const AtomicString, inherit, ("inherit", AtomicString::ConstructFromLiteral));

    if (value.isEmpty() || value != inherit)
        return false;
    return SVGElement::isAnimatableCSSProperty(attributeName);
}

void SVGAnimationElement::determinePropertyValueTypes(const String& from, const String& to)
{
    SVGElement* targetElement = this->targetElement();
    ASSERT(targetElement);

    const QualifiedName& attributeName = this->attributeName();
    if (inheritsFromProperty(targetElement, attributeName, from))
        m_fromPropertyValueType = InheritValue;
    if (inheritsFromProperty(targetElement, attributeName, to))
        m_toPropertyValueType = InheritValue;
}

void SVGAnimationElement::setTargetElement(SVGElement* target)
{
    SVGSMILElement::setTargetElement(target);
    checkInvalidCSSAttributeType();
}

void SVGAnimationElement::setAttributeName(const QualifiedName& attributeName)
{
    SVGSMILElement::setAttributeName(attributeName);
    checkInvalidCSSAttributeType();
}

void SVGAnimationElement::checkInvalidCSSAttributeType()
{
    bool hasInvalidCSSAttributeType = targetElement() && hasValidAttributeName() && attributeType() == AttributeTypeCSS && !isTargetAttributeCSSProperty(targetElement(), attributeName());

    if (hasInvalidCSSAttributeType != m_hasInvalidCSSAttributeType) {
        if (hasInvalidCSSAttributeType)
            unscheduleIfScheduled();

        m_hasInvalidCSSAttributeType = hasInvalidCSSAttributeType;

        if (!hasInvalidCSSAttributeType)
            schedule();
    }

    // Clear values that may depend on the previous target.
    if (targetElement())
        clearAnimatedType();
}

}
