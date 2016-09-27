/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
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

#include "core/svg/SVGFEMergeElement.h"

#include "core/SVGNames.h"
#include "core/dom/ElementTraversal.h"
#include "core/svg/SVGFEMergeNodeElement.h"
#include "core/svg/graphics/filters/SVGFilterBuilder.h"
#include "platform/graphics/filters/FilterEffect.h"

namespace blink {

inline SVGFEMergeElement::SVGFEMergeElement(Document& document)
    : SVGFilterPrimitiveStandardAttributes(SVGNames::feMergeTag, document)
{
}

DEFINE_NODE_FACTORY(SVGFEMergeElement)

PassRefPtrWillBeRawPtr<FilterEffect> SVGFEMergeElement::build(SVGFilterBuilder* filterBuilder, Filter* filter)
{
    RefPtrWillBeRawPtr<FilterEffect> effect = FEMerge::create(filter);
    FilterEffectVector& mergeInputs = effect->inputEffects();
    for (SVGFEMergeNodeElement* element = Traversal<SVGFEMergeNodeElement>::firstChild(*this); element; element = Traversal<SVGFEMergeNodeElement>::nextSibling(*element)) {
        FilterEffect* mergeEffect = filterBuilder->getEffectById(AtomicString(element->in1()->currentValue()->value()));
        if (!mergeEffect)
            return nullptr;
        mergeInputs.append(mergeEffect);
    }

    if (mergeInputs.isEmpty())
        return nullptr;

    return effect.release();
}

}
