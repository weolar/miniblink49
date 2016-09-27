/**
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Nokia Inc. All rights reserved.
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
#include "core/html/LabelsNodeList.h"

#include "core/HTMLNames.h"
#include "core/dom/Element.h"
#include "core/dom/NodeRareData.h"
#include "core/html/HTMLLabelElement.h"

namespace blink {

using namespace HTMLNames;

LabelsNodeList::LabelsNodeList(ContainerNode& ownerNode)
    : LiveNodeList(ownerNode, LabelsNodeListType, InvalidateOnForAttrChange, NodeListIsRootedAtDocument)
{
}

LabelsNodeList::~LabelsNodeList()
{
#if !ENABLE(OILPAN)
    ownerNode().nodeLists()->removeCache(this, LabelsNodeListType);
#endif
}

bool LabelsNodeList::elementMatches(const Element& element) const
{
    return isHTMLLabelElement(element) && toHTMLLabelElement(element).control() == ownerNode();
}

} // namespace blink
