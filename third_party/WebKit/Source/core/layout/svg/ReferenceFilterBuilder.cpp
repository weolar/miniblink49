/*
 * Copyright (C) 2013 Adobe Systems Inc. All rights reserved.
 * Copyright (C) 2013 Google Inc.  All rights reserved.
 * Copyright (C) 2011 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "core/layout/svg/ReferenceFilterBuilder.h"

#include "core/css/CSSPrimitiveValue.h"
#include "core/css/CSSPrimitiveValueMappings.h"
#include "core/css/StylePropertySet.h"
#include "core/dom/Element.h"
#include "core/dom/ElementTraversal.h"
#include "core/fetch/DocumentResource.h"
#include "core/layout/svg/LayoutSVGResourceFilter.h"
#include "core/svg/SVGDocumentExtensions.h"
#include "core/svg/SVGFilterPrimitiveStandardAttributes.h"
#include "core/svg/graphics/filters/SVGFilterBuilder.h"

namespace blink {

HashMap<const FilterOperation*, OwnPtr<DocumentResourceReference>>* ReferenceFilterBuilder::documentResourceReferences = nullptr;

DocumentResourceReference* ReferenceFilterBuilder::documentResourceReference(const FilterOperation* filterOperation)
{
    if (!documentResourceReferences)
        return nullptr;

    return documentResourceReferences->get(filterOperation);
}

void ReferenceFilterBuilder::setDocumentResourceReference(const FilterOperation* filterOperation, PassOwnPtr<DocumentResourceReference> documentResourceReference)
{
    if (!documentResourceReferences)
        documentResourceReferences = new HashMap<const FilterOperation*, OwnPtr<DocumentResourceReference>>;
    documentResourceReferences->add(filterOperation, documentResourceReference);
}

void ReferenceFilterBuilder::clearDocumentResourceReference(const FilterOperation* filterOperation)
{
    if (!documentResourceReferences)
        return;

    documentResourceReferences->remove(filterOperation);
}

// Returns the color-interpolation-filters property of the element.
static EColorInterpolation colorInterpolationForElement(SVGElement& element, EColorInterpolation parentColorInterpolation)
{
    if (const LayoutObject* layoutObject = element.layoutObject())
        return layoutObject->styleRef().svgStyle().colorInterpolationFilters();

    // No layout has been performed, try to determine the property value
    // "manually" (used by external SVG files.)
    if (const StylePropertySet* propertySet = element.presentationAttributeStyle()) {
        RefPtrWillBeRawPtr<CSSValue> cssValue = propertySet->getPropertyCSSValue(CSSPropertyColorInterpolationFilters);
        if (cssValue && cssValue->isPrimitiveValue()) {
            const CSSPrimitiveValue& primitiveValue = *((CSSPrimitiveValue*)cssValue.get());
            return static_cast<EColorInterpolation>(primitiveValue);
        }
    }
    // 'auto' is the default (per Filter Effects), but since the property is
    // inherited, propagate the parent's value.
    return parentColorInterpolation;
}

PassRefPtrWillBeRawPtr<FilterEffect> ReferenceFilterBuilder::build(Filter* parentFilter, Element* element, FilterEffect* previousEffect, const ReferenceFilterOperation& filterOperation)
{
    TreeScope* treeScope = &element->treeScope();

    if (DocumentResourceReference* documentResourceRef = documentResourceReference(&filterOperation)) {
        DocumentResource* cachedSVGDocument = documentResourceRef->document();

        // If we have an SVG document, this is an external reference. Otherwise
        // we look up the referenced node in the current document.
        if (cachedSVGDocument)
            treeScope = cachedSVGDocument->document();
    }

    if (!treeScope)
        return nullptr;

    Element* filter = treeScope->getElementById(filterOperation.fragment());

    if (!filter) {
        // Although we did not find the referenced filter, it might exist later
        // in the document.
        treeScope->document().accessSVGExtensions().addPendingResource(filterOperation.fragment(), element);
        return nullptr;
    }

    if (!isSVGFilterElement(*filter))
        return nullptr;

    SVGFilterElement& filterElement = toSVGFilterElement(*filter);

    RefPtrWillBeRawPtr<SVGFilterBuilder> builder = SVGFilterBuilder::create(previousEffect);

    EColorInterpolation filterColorInterpolation = colorInterpolationForElement(filterElement, CI_AUTO);

    for (SVGElement* element = Traversal<SVGElement>::firstChild(filterElement); element; element = Traversal<SVGElement>::nextSibling(*element)) {
        if (!element->isFilterEffect())
            continue;

        SVGFilterPrimitiveStandardAttributes* effectElement = static_cast<SVGFilterPrimitiveStandardAttributes*>(element);
        RefPtrWillBeRawPtr<FilterEffect> effect = effectElement->build(builder.get(), parentFilter);
        if (!effect)
            continue;

        effectElement->setStandardAttributes(effect.get());
        effect->setEffectBoundaries(SVGLengthContext::resolveRectangle<SVGFilterPrimitiveStandardAttributes>(effectElement, filterElement.primitiveUnits()->currentValue()->enumValue(), parentFilter->sourceImageRect()));
        EColorInterpolation colorInterpolation = colorInterpolationForElement(*effectElement, filterColorInterpolation);
        effect->setOperatingColorSpace(colorInterpolation == CI_LINEARRGB ? ColorSpaceLinearRGB : ColorSpaceDeviceRGB);
        builder->add(AtomicString(effectElement->result()->currentValue()->value()), effect);
    }
    return builder->lastEffect();
}

} // namespace blink
