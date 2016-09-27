// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WindowNameCollection_h
#define WindowNameCollection_h

#include "core/html/HTMLNameCollection.h"

namespace blink {

class WindowNameCollection final : public HTMLNameCollection {
public:
    static PassRefPtrWillBeRawPtr<WindowNameCollection> create(ContainerNode& document, CollectionType type, const AtomicString& name)
    {
        ASSERT_UNUSED(type, type == WindowNamedItems);
        return adoptRefWillBeNoop(new WindowNameCollection(document, name));
    }

    bool elementMatches(const Element&) const;

private:
    WindowNameCollection(ContainerNode& document, const AtomicString& name);
};

DEFINE_TYPE_CASTS(WindowNameCollection, LiveNodeListBase, collection, collection->type() == WindowNamedItems, collection.type() == WindowNamedItems);

} // namespace blink

#endif // WindowNameCollection_h
