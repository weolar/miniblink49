/*
 * Copyright (C) 2004, 2005 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 * Copyright (C) 2009 Google, Inc.
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

#include "core/layout/svg/LayoutSVGTransformableContainer.h"

#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/svg/SVGGElement.h"
#include "core/svg/SVGGraphicsElement.h"
#include "core/svg/SVGUseElement.h"

namespace blink {

LayoutSVGTransformableContainer::LayoutSVGTransformableContainer(SVGGraphicsElement* node)
    : LayoutSVGContainer(node)
    , m_needsTransformUpdate(true)
    , m_didTransformToRootUpdate(false)
{
}

static bool hasValidPredecessor(const Node* node)
{
    ASSERT(node);
    for (node = node->previousSibling(); node; node = node->previousSibling()) {
        if (node->isSVGElement() && toSVGElement(node)->isValid())
            return true;
    }
    return false;
}

bool LayoutSVGTransformableContainer::isChildAllowed(LayoutObject* child, const ComputedStyle& style) const
{
    ASSERT(element());
    if (isSVGSwitchElement(*element())) {
        Node* node = child->node();
        // Reject non-SVG/non-valid elements.
        if (!node->isSVGElement() || !toSVGElement(node)->isValid())
            return false;
        // Reject this child if it isn't the first valid node.
        if (hasValidPredecessor(node))
            return false;
    } else if (isSVGAElement(*element())) {
        // http://www.w3.org/2003/01/REC-SVG11-20030114-errata#linking-text-environment
        // The 'a' element may contain any element that its parent may contain, except itself.
        if (isSVGAElement(*child->node()))
            return false;
        if (parent() && parent()->isSVG())
            return parent()->isChildAllowed(child, style);
    }
    return LayoutSVGContainer::isChildAllowed(child, style);
}

bool LayoutSVGTransformableContainer::calculateLocalTransform()
{
    SVGGraphicsElement* element = toSVGGraphicsElement(this->element());
    ASSERT(element);

    // If we're either the layoutObject for a <use> element, or for any <g> element inside the shadow
    // tree, that was created during the use/symbol/svg expansion in SVGUseElement. These containers
    // need to respect the translations induced by their corresponding use elements x/y attributes.
    SVGUseElement* useElement = nullptr;
    if (isSVGUseElement(*element)) {
        useElement = toSVGUseElement(element);
    } else if (isSVGGElement(*element) && toSVGGElement(element)->inUseShadowTree()) {
        SVGElement* correspondingElement = element->correspondingElement();
        if (isSVGUseElement(correspondingElement))
            useElement = toSVGUseElement(correspondingElement);
    }

    if (useElement) {
        SVGLengthContext lengthContext(useElement);
        FloatSize translation(
            useElement->x()->currentValue()->value(lengthContext),
            useElement->y()->currentValue()->value(lengthContext));
        if (translation != m_lastTranslation)
            m_needsTransformUpdate = true;
        m_lastTranslation = translation;
    }

    m_didTransformToRootUpdate = m_needsTransformUpdate || SVGLayoutSupport::transformToRootChanged(parent());
    if (!m_needsTransformUpdate)
        return false;

    m_localTransform = element->calculateAnimatedLocalTransform();
    m_localTransform.translate(m_lastTranslation.width(), m_lastTranslation.height());
    m_needsTransformUpdate = false;
    return true;
}

}
