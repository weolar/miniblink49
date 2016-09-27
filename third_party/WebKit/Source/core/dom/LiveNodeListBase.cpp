/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2006, 2007, 2008, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
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
#include "core/dom/LiveNodeListBase.h"

#include "core/dom/LiveNodeList.h"
#include "core/html/HTMLCollection.h"

namespace blink {

void LiveNodeListBase::invalidateCacheForAttribute(const QualifiedName* attrName) const
{
    if (isLiveNodeListType(type()))
        toLiveNodeList(this)->invalidateCacheForAttribute(attrName);
    else
        toHTMLCollection(this)->invalidateCacheForAttribute(attrName);
}

ContainerNode& LiveNodeListBase::rootNode() const
{
    if (isRootedAtDocument() && m_ownerNode->inDocument())
        return m_ownerNode->document();
    return *m_ownerNode;
}

void LiveNodeListBase::didMoveToDocument(Document& oldDocument, Document& newDocument)
{
    invalidateCache(&oldDocument);
    oldDocument.unregisterNodeList(this);
    newDocument.registerNodeList(this);
}

} // namespace blink
