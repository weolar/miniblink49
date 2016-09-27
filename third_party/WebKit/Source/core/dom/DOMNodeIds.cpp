// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/DOMNodeIds.h"

#include "platform/heap/Handle.h"

namespace blink {

DEFINE_WEAK_IDENTIFIER_MAP(Node);

#if !ENABLE(OILPAN)
void WeakIdentifierMapTraits<Node>::removedFromIdentifierMap(Node* node)
{
    node->clearFlag(Node::HasWeakReferencesFlag);
}

void WeakIdentifierMapTraits<Node>::addedToIdentifierMap(Node* node)
{
    node->setFlag(Node::HasWeakReferencesFlag);
}
#endif

// static
int DOMNodeIds::idForNode(Node* node)
{
    return WeakIdentifierMap<Node>::identifier(node);
}

// static
Node* DOMNodeIds::nodeForId(int id)
{
    return WeakIdentifierMap<Node>::lookup(id);
}

}
