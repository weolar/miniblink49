/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2010 Dirk Schulze <krit@webkit.org>
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
#include "core/svg/SVGFEImageElement.h"

#include "core/XLinkNames.h"
#include "core/dom/Document.h"
#include "core/fetch/FetchRequest.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/svg/SVGDocumentExtensions.h"
#include "core/svg/SVGPreserveAspectRatio.h"
#include "core/svg/graphics/filters/SVGFEImage.h"
#include "platform/graphics/Image.h"

namespace blink {

inline SVGFEImageElement::SVGFEImageElement(Document& document)
    : SVGFilterPrimitiveStandardAttributes(SVGNames::feImageTag, document)
    , SVGURIReference(this)
    , m_preserveAspectRatio(SVGAnimatedPreserveAspectRatio::create(this, SVGNames::preserveAspectRatioAttr, SVGPreserveAspectRatio::create()))
{
    addToPropertyMap(m_preserveAspectRatio);
}

DEFINE_NODE_FACTORY(SVGFEImageElement)

SVGFEImageElement::~SVGFEImageElement()
{
#if ENABLE(OILPAN)
    if (m_cachedImage) {
        m_cachedImage->removeClient(this);
        m_cachedImage = 0;
    }
#else
    clearResourceReferences();
#endif
}

DEFINE_TRACE(SVGFEImageElement)
{
    visitor->trace(m_preserveAspectRatio);
    SVGFilterPrimitiveStandardAttributes::trace(visitor);
    SVGURIReference::trace(visitor);
}

bool SVGFEImageElement::currentFrameHasSingleSecurityOrigin() const
{
    if (m_cachedImage && m_cachedImage->image())
        return m_cachedImage->image()->currentFrameHasSingleSecurityOrigin();

    return true;
}

void SVGFEImageElement::clearResourceReferences()
{
    if (m_cachedImage) {
        m_cachedImage->removeClient(this);
        m_cachedImage = 0;
    }

    removeAllOutgoingReferences();
}

void SVGFEImageElement::fetchImageResource()
{
    FetchRequest request(ResourceRequest(ownerDocument()->completeURL(hrefString())), localName());
    m_cachedImage = ImageResource::fetch(request, document().fetcher());

    if (m_cachedImage)
        m_cachedImage->addClient(this);
}

void SVGFEImageElement::buildPendingResource()
{
    clearResourceReferences();
    if (!inDocument())
        return;

    AtomicString id;
    Element* target = SVGURIReference::targetElementFromIRIString(hrefString(), treeScope(), &id);
    if (!target) {
        if (id.isEmpty())
            fetchImageResource();
        else {
            document().accessSVGExtensions().addPendingResource(id, this);
            ASSERT(hasPendingResources());
        }
    } else if (target->isSVGElement()) {
        // Register us with the target in the dependencies map. Any change of hrefElement
        // that leads to relayout/repainting now informs us, so we can react to it.
        addReferenceTo(toSVGElement(target));
    }

    invalidate();
}

void SVGFEImageElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::preserveAspectRatioAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        invalidate();
        return;
    }

    if (SVGURIReference::isKnownAttribute(attrName)) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        buildPendingResource();
        return;
    }

    SVGFilterPrimitiveStandardAttributes::svgAttributeChanged(attrName);
}

Node::InsertionNotificationRequest SVGFEImageElement::insertedInto(ContainerNode* rootParent)
{
    SVGFilterPrimitiveStandardAttributes::insertedInto(rootParent);
    buildPendingResource();
    return InsertionDone;
}

void SVGFEImageElement::removedFrom(ContainerNode* rootParent)
{
    SVGFilterPrimitiveStandardAttributes::removedFrom(rootParent);
    if (rootParent->inDocument())
        clearResourceReferences();
}

void SVGFEImageElement::notifyFinished(Resource*)
{
    if (!inDocument())
        return;

    Element* parent = parentElement();
    if (!parent || !isSVGFilterElement(parent) || !parent->layoutObject())
        return;

    if (LayoutObject* layoutObject = this->layoutObject())
        markForLayoutAndParentResourceInvalidation(layoutObject);
}

PassRefPtrWillBeRawPtr<FilterEffect> SVGFEImageElement::build(SVGFilterBuilder*, Filter* filter)
{
    if (m_cachedImage)
        return FEImage::createWithImage(filter, m_cachedImage->imageForLayoutObject(layoutObject()), m_preserveAspectRatio->currentValue());
    return FEImage::createWithIRIReference(filter, treeScope(), hrefString(), m_preserveAspectRatio->currentValue());
}

}
