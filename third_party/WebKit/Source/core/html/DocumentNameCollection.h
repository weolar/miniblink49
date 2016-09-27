// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DocumentNameCollection_h
#define DocumentNameCollection_h

#include "core/html/HTMLElement.h"
#include "core/html/HTMLNameCollection.h"

namespace blink {

class DocumentNameCollection final : public HTMLNameCollection {
public:
    static PassRefPtrWillBeRawPtr<DocumentNameCollection> create(ContainerNode& document, CollectionType type, const AtomicString& name)
    {
        ASSERT_UNUSED(type, type == DocumentNamedItems);
        return adoptRefWillBeNoop(new DocumentNameCollection(document, name));
    }

    HTMLElement* item(unsigned offset) const { return toHTMLElement(HTMLNameCollection::item(offset)); }

    bool elementMatches(const HTMLElement&) const;

private:
    DocumentNameCollection(ContainerNode& document, const AtomicString& name);
};

DEFINE_TYPE_CASTS(DocumentNameCollection, LiveNodeListBase, collection, collection->type() == DocumentNamedItems, collection.type() == DocumentNamedItems);

} // namespace blink

#endif // DocumentNameCollection_h
