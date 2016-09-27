/*
 * Copyright (C) 2004, 2005 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 * Copyright (C) Research In Motion Limited 2011. All rights reserved.
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

#include "core/svg/SVGAnimateElement.h"

#include "core/CSSPropertyNames.h"
#include "core/css/StylePropertySet.h"
#include "core/css/parser/CSSParser.h"
#include "core/dom/Document.h"
#include "core/dom/QualifiedName.h"
#include "core/svg/SVGAnimatedTypeAnimator.h"
#include "core/svg/SVGDocumentExtensions.h"

namespace blink {

SVGAnimateElement::SVGAnimateElement(const QualifiedName& tagName, Document& document)
    : SVGAnimationElement(tagName, document)
    , m_animator(this)
{
}

PassRefPtrWillBeRawPtr<SVGAnimateElement> SVGAnimateElement::create(Document& document)
{
    return adoptRefWillBeNoop(new SVGAnimateElement(SVGNames::animateTag, document));
}

SVGAnimateElement::~SVGAnimateElement()
{
}

bool SVGAnimateElement::isSVGAnimationAttributeSettingJavaScriptURL(const Attribute& attribute) const
{
    if ((attribute.name() == SVGNames::fromAttr || attribute.name() == SVGNames::toAttr) && attributeValueIsJavaScriptURL(attribute))
        return true;

    if (attribute.name() == SVGNames::valuesAttr) {
        Vector<String> parts;
        if (!parseValues(attribute.value(), parts)) {
            // Assume the worst.
            return true;
        }
        for (const auto& part : parts) {
            if (protocolIsJavaScript(part))
                return true;
        }
    }

    return SVGSMILElement::isSVGAnimationAttributeSettingJavaScriptURL(attribute);
}

AnimatedPropertyType SVGAnimateElement::animatedPropertyType()
{
    if (!targetElement())
        return AnimatedUnknown;

    m_animator.reset(targetElement());
    return m_animator.type();
}

bool SVGAnimateElement::hasValidAttributeType()
{
    SVGElement* targetElement = this->targetElement();
    if (!targetElement)
        return false;

    return animatedPropertyType() != AnimatedUnknown && !hasInvalidCSSAttributeType();
}

void SVGAnimateElement::calculateAnimatedValue(float percentage, unsigned repeatCount, SVGSMILElement* resultElement)
{
    ASSERT(resultElement);
    SVGElement* targetElement = this->targetElement();
    if (!targetElement || !isSVGAnimateElement(*resultElement))
        return;

    ASSERT(percentage >= 0 && percentage <= 1);
    ASSERT(animatedPropertyType() != AnimatedTransformList || isSVGAnimateTransformElement(*this));
    ASSERT(animatedPropertyType() != AnimatedUnknown);
    ASSERT(m_fromProperty);
    ASSERT(m_fromProperty->type() == animatedPropertyType());
    ASSERT(m_toProperty);

    SVGAnimateElement* resultAnimationElement = toSVGAnimateElement(resultElement);
    ASSERT(resultAnimationElement->m_animatedProperty);
    ASSERT(resultAnimationElement->animatedPropertyType() == animatedPropertyType());

    if (isSVGSetElement(*this))
        percentage = 1;

    if (calcMode() == CalcModeDiscrete)
        percentage = percentage < 0.5 ? 0 : 1;

    // Target element might have changed.
    m_animator.setContextElement(targetElement);

    // Values-animation accumulates using the last values entry corresponding to the end of duration time.
    SVGPropertyBase* toAtEndOfDurationProperty = m_toAtEndOfDurationProperty ? m_toAtEndOfDurationProperty.get() : m_toProperty.get();
    m_animator.calculateAnimatedValue(percentage, repeatCount, m_fromProperty.get(), m_toProperty.get(), toAtEndOfDurationProperty, resultAnimationElement->m_animatedProperty.get());
}

bool SVGAnimateElement::calculateToAtEndOfDurationValue(const String& toAtEndOfDurationString)
{
    if (toAtEndOfDurationString.isEmpty())
        return false;
    m_toAtEndOfDurationProperty = m_animator.constructFromString(toAtEndOfDurationString);
    return true;
}

bool SVGAnimateElement::calculateFromAndToValues(const String& fromString, const String& toString)
{
    SVGElement* targetElement = this->targetElement();
    if (!targetElement)
        return false;

    determinePropertyValueTypes(fromString, toString);
    m_animator.calculateFromAndToValues(m_fromProperty, m_toProperty, fromString, toString);
    return true;
}

bool SVGAnimateElement::calculateFromAndByValues(const String& fromString, const String& byString)
{
    SVGElement* targetElement = this->targetElement();
    if (!targetElement)
        return false;

    if (animationMode() == ByAnimation && !isAdditive())
        return false;

    // from-by animation may only be used with attributes that support addition (e.g. most numeric attributes).
    if (animationMode() == FromByAnimation && !animatedPropertyTypeSupportsAddition())
        return false;

    ASSERT(!isSVGSetElement(*this));

    determinePropertyValueTypes(fromString, byString);
    m_animator.calculateFromAndByValues(m_fromProperty, m_toProperty, fromString, byString);
    return true;
}

SVGElementInstances SVGAnimateElement::findElementInstances(SVGElement* targetElement)
{
    ASSERT(targetElement);
    SVGElementInstances animatedElements;

    animatedElements.append(targetElement);

    const auto& instances = targetElement->instancesForElement();
    animatedElements.appendRange(instances.begin(), instances.end());

    return animatedElements;
}

void SVGAnimateElement::resetAnimatedType()
{
    SVGElement* targetElement = this->targetElement();
    const QualifiedName& attributeName = this->attributeName();

    m_animator.reset(targetElement);

    ShouldApplyAnimation shouldApply = shouldApplyAnimation(targetElement, attributeName);

    if (shouldApply == DontApplyAnimation)
        return;
    if (shouldApply == ApplyXMLAnimation || shouldApply == ApplyXMLandCSSAnimation) {
        // SVG DOM animVal animation code-path.
        SVGElementInstances animatedElements = findElementInstances(targetElement);
        ASSERT(!animatedElements.isEmpty());

        for (SVGElement* element : animatedElements)
            addReferenceTo(element);

        if (!m_animatedProperty)
            m_animatedProperty = m_animator.startAnimValAnimation(animatedElements);
        else
            m_animatedProperty = m_animator.resetAnimValToBaseVal(animatedElements);

        return;
    }

    // CSS properties animation code-path.
    String baseValue;

    if (shouldApply == ApplyCSSAnimation) {
        ASSERT(SVGAnimationElement::isTargetAttributeCSSProperty(targetElement, attributeName));
        computeCSSPropertyValue(targetElement, cssPropertyID(attributeName.localName()), baseValue);
    }

    m_animatedProperty = m_animator.constructFromString(baseValue);
}

static inline void applyCSSPropertyToTarget(SVGElement* targetElement, CSSPropertyID id, const String& value)
{
#if !ENABLE(OILPAN)
    ASSERT_WITH_SECURITY_IMPLICATION(!targetElement->m_deletionHasBegun);
#endif

    MutableStylePropertySet* propertySet = targetElement->ensureAnimatedSMILStyleProperties();
    if (!propertySet->setProperty(id, value, false, 0))
        return;

    targetElement->setNeedsStyleRecalc(LocalStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::Animation));
}

static inline void removeCSSPropertyFromTarget(SVGElement* targetElement, CSSPropertyID id)
{
#if !ENABLE(OILPAN)
    ASSERT_WITH_SECURITY_IMPLICATION(!targetElement->m_deletionHasBegun);
#endif
    targetElement->ensureAnimatedSMILStyleProperties()->removeProperty(id);
    targetElement->setNeedsStyleRecalc(LocalStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::Animation));
}

static inline void applyCSSPropertyToTargetAndInstances(SVGElement* targetElement, const QualifiedName& attributeName, const String& valueAsString)
{
    ASSERT(targetElement);
    if (attributeName == anyQName() || !targetElement->inDocument() || !targetElement->parentNode())
        return;

    CSSPropertyID id = cssPropertyID(attributeName.localName());

    SVGElement::InstanceUpdateBlocker blocker(targetElement);
    applyCSSPropertyToTarget(targetElement, id, valueAsString);

    // If the target element has instances, update them as well, w/o requiring the <use> tree to be rebuilt.
    const WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>>& instances = targetElement->instancesForElement();
    for (SVGElement* shadowTreeElement : instances) {
        if (shadowTreeElement)
            applyCSSPropertyToTarget(shadowTreeElement, id, valueAsString);
    }
}

static inline void removeCSSPropertyFromTargetAndInstances(SVGElement* targetElement, const QualifiedName& attributeName)
{
    ASSERT(targetElement);
    if (attributeName == anyQName() || !targetElement->inDocument() || !targetElement->parentNode())
        return;

    CSSPropertyID id = cssPropertyID(attributeName.localName());

    SVGElement::InstanceUpdateBlocker blocker(targetElement);
    removeCSSPropertyFromTarget(targetElement, id);

    // If the target element has instances, update them as well, w/o requiring the <use> tree to be rebuilt.
    const WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>>& instances = targetElement->instancesForElement();
    for (SVGElement* shadowTreeElement : instances) {
        if (shadowTreeElement)
            removeCSSPropertyFromTarget(shadowTreeElement, id);
    }
}

static inline void notifyTargetAboutAnimValChange(SVGElement* targetElement, const QualifiedName& attributeName)
{
#if !ENABLE(OILPAN)
    ASSERT_WITH_SECURITY_IMPLICATION(!targetElement->m_deletionHasBegun);
#endif
    targetElement->invalidateSVGAttributes();
    targetElement->svgAttributeChanged(attributeName);
}

static inline void notifyTargetAndInstancesAboutAnimValChange(SVGElement* targetElement, const QualifiedName& attributeName)
{
    ASSERT(targetElement);
    if (attributeName == anyQName() || !targetElement->inDocument() || !targetElement->parentNode())
        return;

    SVGElement::InstanceUpdateBlocker blocker(targetElement);
    notifyTargetAboutAnimValChange(targetElement, attributeName);

    // If the target element has instances, update them as well, w/o requiring the <use> tree to be rebuilt.
    for (SVGElement* element : targetElement->instancesForElement())
        notifyTargetAboutAnimValChange(element, attributeName);
}

void SVGAnimateElement::clearAnimatedType()
{
    if (!m_animatedProperty)
        return;

    SVGElement* targetElement = this->targetElement();
    if (!targetElement) {
        m_animatedProperty.clear();
        return;
    }

    ShouldApplyAnimation shouldApply = shouldApplyAnimation(targetElement, attributeName());
    if (shouldApply == ApplyXMLandCSSAnimation) {
        removeCSSPropertyFromTargetAndInstances(targetElement, attributeName());
    } else if (m_animator.isAnimatingCSSProperty()) {
        // CSS properties animation code-path.
        removeCSSPropertyFromTargetAndInstances(targetElement, attributeName());
        m_animatedProperty.clear();
        m_animator.clear();
        return;
    }

    // SVG DOM animVal animation code-path.
    if (m_animatedProperty) {
        SVGElementInstances animatedElements = findElementInstances(targetElement);
        m_animator.stopAnimValAnimation(animatedElements);
        notifyTargetAndInstancesAboutAnimValChange(targetElement, attributeName());
    }

    m_animatedProperty.clear();
    m_animator.clear();
}

void SVGAnimateElement::applyResultsToTarget()
{
    ASSERT(animatedPropertyType() != AnimatedTransformList || isSVGAnimateTransformElement(*this));
    ASSERT(animatedPropertyType() != AnimatedUnknown);

    // Early exit if our animated type got destructed by a previous endedActiveInterval().
    if (!m_animatedProperty)
        return;

    // We do update the style and the animation property independent of each other.
    ShouldApplyAnimation shouldApply = shouldApplyAnimation(targetElement(), attributeName());
    if (shouldApply == ApplyXMLandCSSAnimation) {
        applyCSSPropertyToTargetAndInstances(targetElement(), attributeName(), m_animatedProperty->valueAsString());
    } else if (m_animator.isAnimatingCSSProperty()) {
        // CSS properties animation code-path.
        // Convert the result of the animation to a String and apply it as CSS property on the target & all instances.
        applyCSSPropertyToTargetAndInstances(targetElement(), attributeName(), m_animatedProperty->valueAsString());
        return;
    }

    // SVG DOM animVal animation code-path.
    // At this point the SVG DOM values are already changed, unlike for CSS.
    // We only have to trigger update notifications here.
    notifyTargetAndInstancesAboutAnimValChange(targetElement(), attributeName());
}

bool SVGAnimateElement::animatedPropertyTypeSupportsAddition()
{
    // Spec: http://www.w3.org/TR/SVG/animate.html#AnimationAttributesAndProperties.
    switch (animatedPropertyType()) {
    case AnimatedBoolean:
    case AnimatedEnumeration:
    case AnimatedPreserveAspectRatio:
    case AnimatedString:
    case AnimatedUnknown:
        return false;
    default:
        return true;
    }
}

bool SVGAnimateElement::isAdditive()
{
    if (animationMode() == ByAnimation || animationMode() == FromByAnimation)
        if (!animatedPropertyTypeSupportsAddition())
            return false;

    return SVGAnimationElement::isAdditive();
}

float SVGAnimateElement::calculateDistance(const String& fromString, const String& toString)
{
    // FIXME: A return value of float is not enough to support paced animations on lists.
    SVGElement* targetElement = this->targetElement();
    if (!targetElement)
        return -1;

    return m_animator.calculateDistance(fromString, toString);
}

void SVGAnimateElement::setTargetElement(SVGElement* target)
{
    SVGAnimationElement::setTargetElement(target);
    resetAnimatedPropertyType();
}

void SVGAnimateElement::setAttributeName(const QualifiedName& attributeName)
{
    SVGAnimationElement::setAttributeName(attributeName);
    resetAnimatedPropertyType();
}

void SVGAnimateElement::resetAnimatedPropertyType()
{
    ASSERT(!m_animatedProperty);
    m_fromProperty.clear();
    m_toProperty.clear();
    m_toAtEndOfDurationProperty.clear();
    m_animator.clear();
}

DEFINE_TRACE(SVGAnimateElement)
{
    visitor->trace(m_fromProperty);
    visitor->trace(m_toProperty);
    visitor->trace(m_toAtEndOfDurationProperty);
    visitor->trace(m_animatedProperty);
    visitor->trace(m_animator);
    SVGAnimationElement::trace(visitor);
}

}
