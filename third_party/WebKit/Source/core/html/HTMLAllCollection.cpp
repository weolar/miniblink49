/*
 * Copyright (C) 2009, 2011, 2012 Apple Inc. All rights reserved.
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
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/HTMLAllCollection.h"

#include "bindings/core/v8/UnionTypesCore.h"
#include "core/dom/Element.h"
#include "core/dom/StaticNodeList.h"

namespace blink {

PassRefPtrWillBeRawPtr<HTMLAllCollection> HTMLAllCollection::create(ContainerNode& node, CollectionType type)
{
    ASSERT_UNUSED(type, type == DocAll);
    return adoptRefWillBeNoop(new HTMLAllCollection(node));
}

HTMLAllCollection::HTMLAllCollection(ContainerNode& node)
    : HTMLCollection(node, DocAll, DoesNotOverrideItemAfter)
{
}

HTMLAllCollection::~HTMLAllCollection()
{
}

Element* HTMLAllCollection::namedItemWithIndex(const AtomicString& name, unsigned index) const
{
    updateIdNameCache();

    const NamedItemCache& cache = namedItemCache();
    if (WillBeHeapVector<RawPtrWillBeMember<Element>>* elements = cache.getElementsById(name)) {
        if (index < elements->size())
            return elements->at(index);
        index -= elements->size();
    }

    if (WillBeHeapVector<RawPtrWillBeMember<Element>>* elements = cache.getElementsByName(name)) {
        if (index < elements->size())
            return elements->at(index);
    }

    return 0;
}

void HTMLAllCollection::namedGetter(const AtomicString& name, NodeListOrElement& returnValue)
{
    WillBeHeapVector<RefPtrWillBeMember<Element>> namedItems;
    this->namedItems(name, namedItems);

    if (!namedItems.size())
        return;

    if (namedItems.size() == 1) {
        returnValue.setElement(namedItems.at(0));
        return;
    }

    // FIXME: HTML5 specification says this should be a HTMLCollection.
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/common-dom-interfaces.html#htmlallcollection
    returnValue.setNodeList(StaticElementList::adopt(namedItems));
}

} // namespace blink
