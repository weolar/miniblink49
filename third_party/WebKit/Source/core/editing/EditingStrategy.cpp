// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/editing/EditingStrategy.h"

#include "core/editing/htmlediting.h"

namespace blink {

template <typename Traversal>
bool EditingAlgorithm<Traversal>::isEmptyNonEditableNodeInEditable(const Node* node)
{
    // Editability is defined the DOM tree rather than the composed tree. For example:
    // DOM:
    //   <host><span>unedittable</span><shadowroot><div ce><content /></div></shadowroot></host>
    // Composed Tree:
    //   <host><div ce><span1>unedittable</span></div></host>
    // e.g. editing/shadow/breaking-editing-boundaries.html
    return !Traversal::hasChildren(*node) && !node->hasEditableStyle() && node->parentNode() && node->parentNode()->hasEditableStyle();
}

template <typename Traversal>
bool EditingAlgorithm<Traversal>::editingIgnoresContent(const Node* node)
{
    return !node->canContainRangeEndPoint() || isEmptyNonEditableNodeInEditable(node);
}

template <typename Traversal>
int EditingAlgorithm<Traversal>::lastOffsetForEditing(const Node* node)
{
    ASSERT(node);
    if (!node)
        return 0;
    if (node->offsetInCharacters())
        return node->maxCharacterOffset();

    if (Traversal::hasChildren(*node))
        return Traversal::countChildren(*node);

    // FIXME: Try return 0 here.

    if (!editingIgnoresContent(node))
        return 0;

    // editingIgnoresContent uses the same logic in
    // isEmptyNonEditableNodeInEditable (htmlediting.cpp). We don't understand
    // why this function returns 1 even when the node doesn't have children.
    return 1;
}

template class EditingAlgorithm<NodeTraversal>;
template class EditingAlgorithm<ComposedTreeTraversal>;

} // namespace blink
