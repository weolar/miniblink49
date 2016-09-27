// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/editing/iterators/FullyClippedStateStack.h"

#include "core/dom/ContainerNode.h"
#include "core/dom/Node.h"
#include "core/editing/htmlediting.h"
#include "core/layout/LayoutBox.h"
#include "core/layout/LayoutObject.h"

namespace blink {

namespace {

inline bool fullyClipsContents(Node* node)
{
    LayoutObject* layoutObject = node->layoutObject();
    if (!layoutObject || !layoutObject->isBox() || !layoutObject->hasOverflowClip() || layoutObject->isLayoutView())
        return false;
    return toLayoutBox(layoutObject)->size().isEmpty();
}

inline bool ignoresContainerClip(Node* node)
{
    LayoutObject* layoutObject = node->layoutObject();
    if (!layoutObject || layoutObject->isText())
        return false;
    return layoutObject->style()->hasOutOfFlowPosition();
}

template <typename Strategy>
unsigned depthCrossingShadowBoundaries(const Node& node)
{
    unsigned depth = 0;
    for (ContainerNode* parent = parentCrossingShadowBoundaries<Strategy>(node); parent; parent = parentCrossingShadowBoundaries<Strategy>(*parent))
        ++depth;
    return depth;
}

} // namespace

template<typename Strategy>
FullyClippedStateStackAlgorithm<Strategy>::FullyClippedStateStackAlgorithm()
{
}

template<typename Strategy>
FullyClippedStateStackAlgorithm<Strategy>::~FullyClippedStateStackAlgorithm()
{
}

template<typename Strategy>
void FullyClippedStateStackAlgorithm<Strategy>::pushFullyClippedState(Node* node)
{
    ASSERT(size() == depthCrossingShadowBoundaries<Strategy>(*node));

    // FIXME: m_fullyClippedStack was added in response to <https://bugs.webkit.org/show_bug.cgi?id=26364>
    // ("Search can find text that's hidden by overflow:hidden"), but the logic here will not work correctly if
    // a shadow tree redistributes nodes. m_fullyClippedStack relies on the assumption that DOM node hierarchy matches
    // the layout tree, which is not necessarily true if there happens to be shadow DOM distribution or other mechanics
    // that shuffle around the layout objects regardless of node tree hierarchy (like CSS flexbox).
    //
    // A more appropriate way to handle this situation is to detect overflow:hidden blocks by using only layout
    // primitives, not with DOM primitives.

    // Push true if this node full clips its contents, or if a parent already has fully
    // clipped and this is not a node that ignores its container's clip.
    push(fullyClipsContents(node) || (top() && !ignoresContainerClip(node)));
}

template<typename Strategy>
void FullyClippedStateStackAlgorithm<Strategy>::setUpFullyClippedStack(Node* node)
{
    // Put the nodes in a vector so we can iterate in reverse order.
    WillBeHeapVector<RawPtrWillBeMember<ContainerNode>, 100> ancestry;
    for (ContainerNode* parent = parentCrossingShadowBoundaries<Strategy>(*node); parent; parent = parentCrossingShadowBoundaries<Strategy>(*parent))
        ancestry.append(parent);

    // Call pushFullyClippedState on each node starting with the earliest ancestor.
    size_t ancestrySize = ancestry.size();
    for (size_t i = 0; i < ancestrySize; ++i)
        pushFullyClippedState(ancestry[ancestrySize - i - 1]);
    pushFullyClippedState(node);

    ASSERT(size() == 1 + depthCrossingShadowBoundaries<Strategy>(*node));
}

template class FullyClippedStateStackAlgorithm<EditingStrategy>;
template class FullyClippedStateStackAlgorithm<EditingInComposedTreeStrategy>;

} // namespace blink
