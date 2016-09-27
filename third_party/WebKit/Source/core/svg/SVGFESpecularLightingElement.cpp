/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Oliver Hunt <oliver@nerget.com>
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
#include "core/svg/SVGFESpecularLightingElement.h"

#include "core/layout/LayoutObject.h"
#include "core/style/ComputedStyle.h"
#include "core/svg/SVGParserUtilities.h"
#include "core/svg/graphics/filters/SVGFilterBuilder.h"
#include "platform/graphics/filters/FilterEffect.h"

namespace blink {

inline SVGFESpecularLightingElement::SVGFESpecularLightingElement(Document& document)
    : SVGFilterPrimitiveStandardAttributes(SVGNames::feSpecularLightingTag, document)
    , m_specularConstant(SVGAnimatedNumber::create(this, SVGNames::specularConstantAttr, SVGNumber::create(1)))
    , m_specularExponent(SVGAnimatedNumber::create(this, SVGNames::specularExponentAttr, SVGNumber::create(1)))
    , m_surfaceScale(SVGAnimatedNumber::create(this, SVGNames::surfaceScaleAttr, SVGNumber::create(1)))
    , m_kernelUnitLength(SVGAnimatedNumberOptionalNumber::create(this, SVGNames::kernelUnitLengthAttr))
    , m_in1(SVGAnimatedString::create(this, SVGNames::inAttr, SVGString::create()))
{
    addToPropertyMap(m_specularConstant);
    addToPropertyMap(m_specularExponent);
    addToPropertyMap(m_surfaceScale);
    addToPropertyMap(m_kernelUnitLength);
    addToPropertyMap(m_in1);
}

DEFINE_TRACE(SVGFESpecularLightingElement)
{
    visitor->trace(m_specularConstant);
    visitor->trace(m_specularExponent);
    visitor->trace(m_surfaceScale);
    visitor->trace(m_kernelUnitLength);
    visitor->trace(m_in1);
    SVGFilterPrimitiveStandardAttributes::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGFESpecularLightingElement)

bool SVGFESpecularLightingElement::setFilterEffectAttribute(FilterEffect* effect, const QualifiedName& attrName)
{
    FESpecularLighting* specularLighting = static_cast<FESpecularLighting*>(effect);

    if (attrName == SVGNames::lighting_colorAttr) {
        LayoutObject* layoutObject = this->layoutObject();
        ASSERT(layoutObject);
        ASSERT(layoutObject->style());
        return specularLighting->setLightingColor(layoutObject->style()->svgStyle().lightingColor());
    }
    if (attrName == SVGNames::surfaceScaleAttr)
        return specularLighting->setSurfaceScale(m_surfaceScale->currentValue()->value());
    if (attrName == SVGNames::specularConstantAttr)
        return specularLighting->setSpecularConstant(m_specularConstant->currentValue()->value());
    if (attrName == SVGNames::specularExponentAttr)
        return specularLighting->setSpecularExponent(m_specularExponent->currentValue()->value());
    if (attrName == SVGNames::kernelUnitLengthAttr) {
        bool changedX = specularLighting->setKernelUnitLengthX(m_kernelUnitLength->firstNumber()->currentValue()->value());
        bool changedY = specularLighting->setKernelUnitLengthY(m_kernelUnitLength->secondNumber()->currentValue()->value());
        return changedX || changedY;
    }

    LightSource* lightSource = const_cast<LightSource*>(specularLighting->lightSource());
    SVGFELightElement* lightElement = SVGFELightElement::findLightElement(*this);
    ASSERT(lightSource);
    ASSERT(lightElement);
    ASSERT(effect->filter());

    if (attrName == SVGNames::azimuthAttr)
        return lightSource->setAzimuth(lightElement->azimuth()->currentValue()->value());
    if (attrName == SVGNames::elevationAttr)
        return lightSource->setElevation(lightElement->elevation()->currentValue()->value());
    if (attrName == SVGNames::xAttr || attrName == SVGNames::yAttr || attrName == SVGNames::zAttr)
        return lightSource->setPosition(effect->filter()->resolve3dPoint(lightElement->position()));
    if (attrName == SVGNames::pointsAtXAttr || attrName == SVGNames::pointsAtYAttr || attrName == SVGNames::pointsAtZAttr)
        return lightSource->setPointsAt(effect->filter()->resolve3dPoint(lightElement->pointsAt()));
    if (attrName == SVGNames::specularExponentAttr)
        return lightSource->setSpecularExponent(lightElement->specularExponent()->currentValue()->value());
    if (attrName == SVGNames::limitingConeAngleAttr)
        return lightSource->setLimitingConeAngle(lightElement->limitingConeAngle()->currentValue()->value());

    ASSERT_NOT_REACHED();
    return false;
}

void SVGFESpecularLightingElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::surfaceScaleAttr
        || attrName == SVGNames::specularConstantAttr
        || attrName == SVGNames::specularExponentAttr
        || attrName == SVGNames::kernelUnitLengthAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        primitiveAttributeChanged(attrName);
        return;
    }

    if (attrName == SVGNames::inAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        invalidate();
        return;
    }

    SVGFilterPrimitiveStandardAttributes::svgAttributeChanged(attrName);
}

void SVGFESpecularLightingElement::lightElementAttributeChanged(const SVGFELightElement* lightElement, const QualifiedName& attrName)
{
    if (SVGFELightElement::findLightElement(*this) != lightElement)
        return;

    // The light element has different attribute names so attrName can identify the requested attribute.
    primitiveAttributeChanged(attrName);
}

PassRefPtrWillBeRawPtr<FilterEffect> SVGFESpecularLightingElement::build(SVGFilterBuilder* filterBuilder, Filter* filter)
{
    FilterEffect* input1 = filterBuilder->getEffectById(AtomicString(m_in1->currentValue()->value()));

    if (!input1)
        return nullptr;

    SVGFELightElement* lightNode = SVGFELightElement::findLightElement(*this);
    if (!lightNode)
        return nullptr;

    LayoutObject* layoutObject = this->layoutObject();
    if (!layoutObject)
        return nullptr;

    ASSERT(layoutObject->style());
    Color color = layoutObject->style()->svgStyle().lightingColor();

    RefPtr<LightSource> lightSource = lightNode->lightSource(filter);
    RefPtrWillBeRawPtr<FilterEffect> effect = FESpecularLighting::create(filter, color, m_surfaceScale->currentValue()->value(), m_specularConstant->currentValue()->value(),
        m_specularExponent->currentValue()->value(), kernelUnitLengthX()->currentValue()->value(), kernelUnitLengthY()->currentValue()->value(), lightSource.release());
    effect->inputEffects().append(input1);
    return effect.release();
}

} // namespace blink
