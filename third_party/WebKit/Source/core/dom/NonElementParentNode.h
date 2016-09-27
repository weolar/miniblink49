// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NonElementParentNode_h
#define NonElementParentNode_h

#include "core/dom/Document.h"
#include "core/dom/DocumentFragment.h"

namespace blink {

class NonElementParentNode {
public:
    static Element* getElementById(Document& document, const AtomicString& id)
    {
        return document.getElementById(id);
    }

    static Element* getElementById(DocumentFragment& fragment, const AtomicString& id)
    {
        return fragment.getElementById(id);
    }
};

} // namespace blink

#endif // NonElementParentNode_h
