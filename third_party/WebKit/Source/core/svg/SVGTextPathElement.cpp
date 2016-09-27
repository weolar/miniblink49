/*
 * Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2010 Rob Buis <rwlbuis@gmail.com>
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
#include "core/svg/SVGTextPathElement.h"

#include "core/XLinkNames.h"
#include "core/layout/svg/LayoutSVGTextPath.h"
#include "core/svg/SVGDocumentExtensions.h"

namespace blink {

template<> const SVGEnumerationStringEntries& getStaticStringEntries<SVGTextPathMethodType>()
{
    DEFINE_STATIC_LOCAL(SVGEnumerationStringEntries, entries, ());
    if (entries.isEmpty()) {
        entries.append(SVGEnumerationBase::StringEntry(SVGTextPathMethodAlign, "align"));
        entries.append(SVGEnumerationBase::StringEntry(SVGTextPathMethodStretch, "stretch"));
    }
    return entries;
}

template<> const SVGEnumerationStringEntries& getStaticStringEntries<SVGTextPathSpacingType>()
{
    DEFINE_STATIC_LOCAL(SVGEnumerationStringEntries, entries, ());
    if (entries.isEmpty()) {
        entries.append(SVGEnumerationBase::StringEntry(SVGTextPathSpacingAuto, "auto"));
        entries.append(SVGEnumerationBase::StringEntry(SVGTextPathSpacingExact, "exact"));
    }
    return entries;
}

inline SVGTextPathElement::SVGTextPathElement(Document& document)
    : SVGTextContentElement(SVGNames::textPathTag, document)
    , SVGURIReference(this)
    , m_startOffset(SVGAnimatedLength::create(this, SVGNames::startOffsetAttr, SVGLength::create(SVGLengthMode::Width), AllowNegativeLengths))
    , m_method(SVGAnimatedEnumeration<SVGTextPathMethodType>::create(this, SVGNames::methodAttr, SVGTextPathMethodAlign))
    , m_spacing(SVGAnimatedEnumeration<SVGTextPathSpacingType>::create(this, SVGNames::spacingAttr, SVGTextPathSpacingExact))
{
    addToPropertyMap(m_startOffset);
    addToPropertyMap(m_method);
    addToPropertyMap(m_spacing);
}

DEFINE_NODE_FACTORY(SVGTextPathElement)

SVGTextPathElement::~SVGTextPathElement()
{
#if !ENABLE(OILPAN)
    clearResourceReferences();
#endif
}

DEFINE_TRACE(SVGTextPathElement)
{
    visitor->trace(m_startOffset);
    visitor->trace(m_method);
    visitor->trace(m_spacing);
    SVGTextContentElement::trace(visitor);
    SVGURIReference::trace(visitor);
}

void SVGTextPathElement::clearResourceReferences()
{
    removeAllOutgoingReferences();
}

void SVGTextPathElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (SVGURIReference::isKnownAttribute(attrName)) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        buildPendingResource();
        return;
    }

    if (attrName == SVGNames::startOffsetAttr)
        updateRelativeLengthsInformation();

    if (attrName == SVGNames::startOffsetAttr
        || attrName == SVGNames::methodAttr
        || attrName == SVGNames::spacingAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        if (LayoutObject* object = layoutObject())
            markForLayoutAndParentResourceInvalidation(object);

        return;
    }

    SVGTextContentElement::svgAttributeChanged(attrName);
}

LayoutObject* SVGTextPathElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutSVGTextPath(this);
}

bool SVGTextPathElement::layoutObjectIsNeeded(const ComputedStyle& style)
{
    if (parentNode() && (isSVGAElement(*parentNode()) || isSVGTextElement(*parentNode())))
        return Element::layoutObjectIsNeeded(style);

    return false;
}

void SVGTextPathElement::buildPendingResource()
{
    clearResourceReferences();
    if (!inDocument())
        return;

    AtomicString id;
    Element* target = SVGURIReference::targetElementFromIRIString(hrefString(), treeScope(), &id);
    if (!target) {
        // Do not register as pending if we are already pending this resource.
        if (document().accessSVGExtensions().isElementPendingResource(this, id))
            return;

        if (!id.isEmpty()) {
            document().accessSVGExtensions().addPendingResource(id, this);
            ASSERT(hasPendingResources());
        }
    } else if (isSVGPathElement(*target)) {
        // Register us with the target in the dependencies map. Any change of hrefElement
        // that leads to relayout/repainting now informs us, so we can react to it.
        addReferenceTo(toSVGElement((target)));
    }
}

Node::InsertionNotificationRequest SVGTextPathElement::insertedInto(ContainerNode* rootParent)
{
    SVGTextContentElement::insertedInto(rootParent);
    buildPendingResource();
    return InsertionDone;
}

void SVGTextPathElement::removedFrom(ContainerNode* rootParent)
{
    SVGTextContentElement::removedFrom(rootParent);
    if (rootParent->inDocument())
        clearResourceReferences();
}

bool SVGTextPathElement::selfHasRelativeLengths() const
{
    return m_startOffset->currentValue()->isRelative()
        || SVGTextContentElement::selfHasRelativeLengths();
}

} // namespace blink
