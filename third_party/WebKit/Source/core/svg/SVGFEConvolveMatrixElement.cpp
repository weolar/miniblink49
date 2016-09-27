/*
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
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
#include "core/svg/SVGFEConvolveMatrixElement.h"

#include "core/SVGNames.h"
#include "core/dom/Document.h"
#include "core/svg/SVGDocumentExtensions.h"
#include "core/svg/SVGParserUtilities.h"
#include "core/svg/graphics/filters/SVGFilterBuilder.h"
#include "platform/geometry/FloatPoint.h"
#include "platform/geometry/IntPoint.h"
#include "platform/geometry/IntSize.h"
#include "platform/graphics/filters/FilterEffect.h"

namespace blink {

template<> const SVGEnumerationStringEntries& getStaticStringEntries<EdgeModeType>()
{
    DEFINE_STATIC_LOCAL(SVGEnumerationStringEntries, entries, ());
    if (entries.isEmpty()) {
        entries.append(SVGEnumerationBase::StringEntry(EDGEMODE_DUPLICATE, "duplicate"));
        entries.append(SVGEnumerationBase::StringEntry(EDGEMODE_WRAP, "wrap"));
        entries.append(SVGEnumerationBase::StringEntry(EDGEMODE_NONE, "none"));
    }
    return entries;
}

class SVGAnimatedOrder : public SVGAnimatedIntegerOptionalInteger {
public:
    static PassRefPtrWillBeRawPtr<SVGAnimatedOrder> create(SVGElement* contextElement)
    {
        return adoptRefWillBeNoop(new SVGAnimatedOrder(contextElement));
    }

    void setBaseValueAsString(const String&, SVGParsingError&) override;

protected:
    SVGAnimatedOrder(SVGElement* contextElement)
        : SVGAnimatedIntegerOptionalInteger(contextElement, SVGNames::orderAttr, 0, 0)
    {
    }
};

void SVGAnimatedOrder::setBaseValueAsString(const String& value, SVGParsingError& parseError)
{
    SVGAnimatedIntegerOptionalInteger::setBaseValueAsString(value, parseError);

    ASSERT(contextElement());
    if (parseError == NoError && (firstInteger()->baseValue()->value() < 1 || secondInteger()->baseValue()->value() < 1)) {
        contextElement()->document().accessSVGExtensions().reportWarning(
            "feConvolveMatrix: problem parsing order=\"" + value
            + "\". Filtered element will not be displayed.");
    }
}

inline SVGFEConvolveMatrixElement::SVGFEConvolveMatrixElement(Document& document)
    : SVGFilterPrimitiveStandardAttributes(SVGNames::feConvolveMatrixTag, document)
    , m_bias(SVGAnimatedNumber::create(this, SVGNames::biasAttr, SVGNumber::create()))
    , m_divisor(SVGAnimatedNumber::create(this, SVGNames::divisorAttr, SVGNumber::create()))
    , m_in1(SVGAnimatedString::create(this, SVGNames::inAttr, SVGString::create()))
    , m_edgeMode(SVGAnimatedEnumeration<EdgeModeType>::create(this, SVGNames::edgeModeAttr, EDGEMODE_DUPLICATE))
    , m_kernelMatrix(SVGAnimatedNumberList::create(this, SVGNames::kernelMatrixAttr, SVGNumberList::create()))
    , m_kernelUnitLength(SVGAnimatedNumberOptionalNumber::create(this, SVGNames::kernelUnitLengthAttr))
    , m_order(SVGAnimatedOrder::create(this))
    , m_preserveAlpha(SVGAnimatedBoolean::create(this, SVGNames::preserveAlphaAttr, SVGBoolean::create()))
    , m_targetX(SVGAnimatedInteger::create(this, SVGNames::targetXAttr, SVGInteger::create()))
    , m_targetY(SVGAnimatedInteger::create(this, SVGNames::targetYAttr, SVGInteger::create()))
{
    addToPropertyMap(m_preserveAlpha);
    addToPropertyMap(m_divisor);
    addToPropertyMap(m_bias);
    addToPropertyMap(m_kernelUnitLength);
    addToPropertyMap(m_kernelMatrix);
    addToPropertyMap(m_in1);
    addToPropertyMap(m_edgeMode);
    addToPropertyMap(m_order);
    addToPropertyMap(m_targetX);
    addToPropertyMap(m_targetY);
}

DEFINE_TRACE(SVGFEConvolveMatrixElement)
{
    visitor->trace(m_bias);
    visitor->trace(m_divisor);
    visitor->trace(m_in1);
    visitor->trace(m_edgeMode);
    visitor->trace(m_kernelMatrix);
    visitor->trace(m_kernelUnitLength);
    visitor->trace(m_order);
    visitor->trace(m_preserveAlpha);
    visitor->trace(m_targetX);
    visitor->trace(m_targetY);
    SVGFilterPrimitiveStandardAttributes::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGFEConvolveMatrixElement)

bool SVGFEConvolveMatrixElement::setFilterEffectAttribute(FilterEffect* effect, const QualifiedName& attrName)
{
    FEConvolveMatrix* convolveMatrix = static_cast<FEConvolveMatrix*>(effect);
    if (attrName == SVGNames::edgeModeAttr)
        return convolveMatrix->setEdgeMode(m_edgeMode->currentValue()->enumValue());
    if (attrName == SVGNames::divisorAttr)
        return convolveMatrix->setDivisor(m_divisor->currentValue()->value());
    if (attrName == SVGNames::biasAttr)
        return convolveMatrix->setBias(m_bias->currentValue()->value());
    if (attrName == SVGNames::targetXAttr)
        return convolveMatrix->setTargetOffset(IntPoint(m_targetX->currentValue()->value(), m_targetY->currentValue()->value()));
    if (attrName == SVGNames::targetYAttr)
        return convolveMatrix->setTargetOffset(IntPoint(m_targetX->currentValue()->value(), m_targetY->currentValue()->value()));
    if (attrName == SVGNames::kernelUnitLengthAttr)
        return convolveMatrix->setKernelUnitLength(FloatPoint(kernelUnitLengthX()->currentValue()->value(), kernelUnitLengthY()->currentValue()->value()));
    if (attrName == SVGNames::preserveAlphaAttr)
        return convolveMatrix->setPreserveAlpha(m_preserveAlpha->currentValue()->value());

    ASSERT_NOT_REACHED();
    return false;
}

void SVGFEConvolveMatrixElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::edgeModeAttr
        || attrName == SVGNames::divisorAttr
        || attrName == SVGNames::biasAttr
        || attrName == SVGNames::targetXAttr
        || attrName == SVGNames::targetYAttr
        || attrName == SVGNames::kernelUnitLengthAttr
        || attrName == SVGNames::preserveAlphaAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        primitiveAttributeChanged(attrName);
        return;
    }

    if (attrName == SVGNames::inAttr
        || attrName == SVGNames::orderAttr
        || attrName == SVGNames::kernelMatrixAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        invalidate();
        return;
    }

    SVGFilterPrimitiveStandardAttributes::svgAttributeChanged(attrName);
}

PassRefPtrWillBeRawPtr<FilterEffect> SVGFEConvolveMatrixElement::build(SVGFilterBuilder* filterBuilder, Filter* filter)
{
    FilterEffect* input1 = filterBuilder->getEffectById(AtomicString(m_in1->currentValue()->value()));

    if (!input1)
        return nullptr;

    int orderXValue = orderX()->currentValue()->value();
    int orderYValue = orderY()->currentValue()->value();
    if (!hasAttribute(SVGNames::orderAttr)) {
        orderXValue = 3;
        orderYValue = 3;
    }
    // Spec says order must be > 0. Bail if it is not.
    if (orderXValue < 1 || orderYValue < 1)
        return nullptr;
    RefPtrWillBeRawPtr<SVGNumberList> kernelMatrix = this->m_kernelMatrix->currentValue();
    size_t kernelMatrixSize = kernelMatrix->length();
    // The spec says this is a requirement, and should bail out if fails
    if (orderXValue * orderYValue != static_cast<int>(kernelMatrixSize))
        return nullptr;

    int targetXValue = m_targetX->currentValue()->value();
    int targetYValue = m_targetY->currentValue()->value();
    if (hasAttribute(SVGNames::targetXAttr) && (targetXValue < 0 || targetXValue >= orderXValue))
        return nullptr;
    // The spec says the default value is: targetX = floor ( orderX / 2 ))
    if (!hasAttribute(SVGNames::targetXAttr))
        targetXValue = static_cast<int>(floorf(orderXValue / 2));
    if (hasAttribute(SVGNames::targetYAttr) && (targetYValue < 0 || targetYValue >= orderYValue))
        return nullptr;
    // The spec says the default value is: targetY = floor ( orderY / 2 ))
    if (!hasAttribute(SVGNames::targetYAttr))
        targetYValue = static_cast<int>(floorf(orderYValue / 2));

    // Spec says default kernelUnitLength is 1.0, and a specified length cannot be 0.
    // FIXME: Why is this cast from float -> int -> float?
    int kernelUnitLengthXValue = kernelUnitLengthX()->currentValue()->value();
    int kernelUnitLengthYValue = kernelUnitLengthY()->currentValue()->value();
    if (!hasAttribute(SVGNames::kernelUnitLengthAttr)) {
        kernelUnitLengthXValue = 1;
        kernelUnitLengthYValue = 1;
    }
    if (kernelUnitLengthXValue <= 0 || kernelUnitLengthYValue <= 0)
        return nullptr;

    float divisorValue = m_divisor->currentValue()->value();
    if (hasAttribute(SVGNames::divisorAttr) && !divisorValue)
        return nullptr;
    if (!hasAttribute(SVGNames::divisorAttr)) {
        for (size_t i = 0; i < kernelMatrixSize; ++i)
            divisorValue += kernelMatrix->at(i)->value();
        if (!divisorValue)
            divisorValue = 1;
    }

    RefPtrWillBeRawPtr<FilterEffect> effect = FEConvolveMatrix::create(filter,
                    IntSize(orderXValue, orderYValue), divisorValue,
                    m_bias->currentValue()->value(), IntPoint(targetXValue, targetYValue), m_edgeMode->currentValue()->enumValue(),
                    FloatPoint(kernelUnitLengthXValue, kernelUnitLengthYValue), m_preserveAlpha->currentValue()->value(), m_kernelMatrix->currentValue()->toFloatVector());
    effect->inputEffects().append(input1);
    return effect.release();
}

} // namespace blink
