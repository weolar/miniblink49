/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
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

#include "core/svg/SVGCursorElement.h"

#include "core/SVGNames.h"
#include "core/XLinkNames.h"

namespace blink {

inline SVGCursorElement::SVGCursorElement(Document& document)
    : SVGElement(SVGNames::cursorTag, document)
    , SVGTests(this)
    , SVGURIReference(this)
    , m_x(SVGAnimatedLength::create(this, SVGNames::xAttr, SVGLength::create(SVGLengthMode::Width), AllowNegativeLengths))
    , m_y(SVGAnimatedLength::create(this, SVGNames::yAttr, SVGLength::create(SVGLengthMode::Height), AllowNegativeLengths))
{
    addToPropertyMap(m_x);
    addToPropertyMap(m_y);
}

DEFINE_NODE_FACTORY(SVGCursorElement)

SVGCursorElement::~SVGCursorElement()
{
    // The below teardown is all handled by weak pointer processing in oilpan.
#if !ENABLE(OILPAN)
    for (const auto& client : m_clients)
        client->cursorElementRemoved();
#endif
}

void SVGCursorElement::addClient(SVGElement* element)
{
    m_clients.add(element);
    element->setCursorElement(this);
}

#if !ENABLE(OILPAN)
void SVGCursorElement::removeClient(SVGElement* element)
{
    HashSet<RawPtr<SVGElement>>::iterator it = m_clients.find(element);
    if (it != m_clients.end()) {
        m_clients.remove(it);
        element->cursorElementRemoved();
    }
}
#endif

void SVGCursorElement::removeReferencedElement(SVGElement* element)
{
    m_clients.remove(element);
}

void SVGCursorElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::xAttr || attrName == SVGNames::yAttr
        || SVGTests::isKnownAttribute(attrName)
        || SVGURIReference::isKnownAttribute(attrName)) {
        SVGElement::InvalidationGuard invalidationGuard(this);

        // Any change of a cursor specific attribute triggers this recalc.
        for (const auto& client : m_clients)
            client->setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::SVGCursor));

        return;
    }

    SVGElement::svgAttributeChanged(attrName);
}

DEFINE_TRACE(SVGCursorElement)
{
#if ENABLE(OILPAN)
    visitor->trace(m_x);
    visitor->trace(m_y);
    visitor->trace(m_clients);
#endif
    SVGElement::trace(visitor);
    SVGTests::trace(visitor);
    SVGURIReference::trace(visitor);
}

} // namespace blink
