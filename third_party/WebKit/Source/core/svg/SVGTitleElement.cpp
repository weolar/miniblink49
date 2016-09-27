/*
 * Copyright (C) 2004, 2005 Nikolas Zimmermann <zimmermann@kde.org>
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
#include "core/svg/SVGTitleElement.h"

#include "core/SVGNames.h"
#include "core/dom/Document.h"

namespace blink {

inline SVGTitleElement::SVGTitleElement(Document& document)
    : SVGElement(SVGNames::titleTag, document)
{
}

DEFINE_NODE_FACTORY(SVGTitleElement)

Node::InsertionNotificationRequest SVGTitleElement::insertedInto(ContainerNode* rootParent)
{
    SVGElement::insertedInto(rootParent);
    if (!rootParent->inDocument())
        return InsertionDone;
    if (hasChildren() && document().isSVGDocument())
        document().setTitleElement(this);
    return InsertionDone;
}

void SVGTitleElement::removedFrom(ContainerNode* rootParent)
{
    SVGElement::removedFrom(rootParent);
    if (rootParent->inDocument() && document().isSVGDocument())
        document().removeTitle(this);
}

void SVGTitleElement::childrenChanged(const ChildrenChange& change)
{
    SVGElement::childrenChanged(change);
    if (inDocument() && document().isSVGDocument())
        document().setTitleElement(this);
}

}
