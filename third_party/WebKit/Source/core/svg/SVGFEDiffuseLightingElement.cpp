/*
 * Copyright (C) 2005 Oliver Hunt <ojh16@student.canterbury.ac.nz>
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
#include "core/svg/SVGFEDiffuseLightingElement.h"

#include "core/layout/LayoutObject.h"
#include "core/style/ComputedStyle.h"
#include "core/svg/SVGParserUtilities.h"
#include "core/svg/graphics/filters/SVGFilterBuilder.h"
#include "platform/graphics/filters/FEDiffuseLighting.h"
#include "platform/graphics/filters/FilterEffect.h"

namespace blink {

inline SVGFEDiffuseLightingElement::SVGFEDiffuseLightingElement(Document& document)
    : SVGFilterPrimitiveStandardAttributes(SVGNames::feDiffuseLightingTag, document)
    , m_diffuseConstant(SVGAnimatedNumber::create(this, SVGNames::diffuseConstantAttr, SVGNumber::create(1)))
    , m_surfaceScale(SVGAnimatedNumber::create(this, SVGNames::surfaceScaleAttr, SVGNumber::create(1)))
    , m_kernelUnitLength(SVGAnimatedNumberOptionalNumber::create(this, SVGNames::kernelUnitLengthAttr))
    , m_in1(SVGAnimatedString::create(this, SVGNames::inAttr, SVGString::create()))
{
    addToPropertyMap(m_diffuseConstant);
    addToPropertyMap(m_surfaceScale);
    addToPropertyMap(m_kernelUnitLength);
    addToPropertyMap(m_in1);
}

DEFINE_TRACE(SVGFEDiffuseLightingElement)
{
    visitor->trace(m_diffuseConstant);
    visitor->trace(m_surfaceScale);
    visitor->trace(m_kernelUnitLength);
    visitor->trace(m_in1);
    SVGFilterPrimitiveStandardAttributes::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGFEDiffuseLightingElement)

bool SVGFEDiffuseLightingElement::setFilterEffectAttribute(FilterEffect* effect, const QualifiedName& attrName)
{
    FEDiffuseLighting* diffuseLighting = static_cast<FEDiffuseLighting*>(effect);

    if (attrName == SVGNames::lighting_colorAttr) {
        LayoutObject* layoutObject = this->layoutObject();
        ASSERT(layoutObject);
        ASSERT(layoutObject->style());
        return diffuseLighting->setLightingColor(layoutObject->style()->svgStyle().lightingColor());
    }
    if (attrName == SVGNames::surfaceScaleAttr)
        return diffuseLighting->setSurfaceScale(m_surfaceScale->currentValue()->value());
    if (attrName == SVGNames::diffuseConstantAttr)
        return diffuseLighting->setDiffuseConstant(m_diffuseConstant->currentValue()->value());
    if (attrName == SVGNames::kernelUnitLengthAttr) {
        bool changedX = diffuseLighting->setKernelUnitLengthX(m_kernelUnitLength->firstNumber()->currentValue()->value());
        bool changedY = diffuseLighting->setKernelUnitLengthY(m_kernelUnitLength->secondNumber()->currentValue()->value());
        return changedX || changedY;
    }

    LightSource* lightSource = const_cast<LightSource*>(diffuseLighting->lightSource());
    const SVGFELightElement* lightElement = SVGFELightElement::findLightElement(*this);
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

void SVGFEDiffuseLightingElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::surfaceScaleAttr
        || attrName == SVGNames::diffuseConstantAttr
        || attrName == SVGNames::kernelUnitLengthAttr
        || attrName == SVGNames::lighting_colorAttr) {
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

void SVGFEDiffuseLightingElement::lightElementAttributeChanged(const SVGFELightElement* lightElement, const QualifiedName& attrName)
{
    if (SVGFELightElement::findLightElement(*this) != lightElement)
        return;

    // The light element has different attribute names.
    primitiveAttributeChanged(attrName);
}

PassRefPtrWillBeRawPtr<FilterEffect> SVGFEDiffuseLightingElement::build(SVGFilterBuilder* filterBuilder, Filter* filter)
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
    RefPtrWillBeRawPtr<FilterEffect> effect = FEDiffuseLighting::create(filter, color, m_surfaceScale->currentValue()->value(), m_diffuseConstant->currentValue()->value(),
        kernelUnitLengthX()->currentValue()->value(), kernelUnitLengthY()->currentValue()->value(), lightSource.release());
    effect->inputEffects().append(input1);
    return effect.release();
}

} // namespace blink
