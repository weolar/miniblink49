// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/LayoutSubtreeRootList.h"

#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutView.h"

namespace blink {

unsigned LayoutSubtreeRootList::LayoutSubtree::determineDepth(LayoutObject* object)
{
    unsigned depth = 1;
    for (LayoutObject* parent = object->parent(); parent; parent = parent->parent())
        ++depth;
    return depth;
}

void LayoutSubtreeRootList::removeRoot(LayoutObject& object)
{
    if (!m_roots.isEmpty()) {
        m_roots.remove(&object);
        ASSERT(m_orderedRoots.isEmpty());
    } else {
        // When removed during layout, we have to search the vector
        int rootCount = m_orderedRoots.size();
        for (int i = 0; i < rootCount; ++i) {
            if (m_orderedRoots[i].object == &object) {
                m_orderedRoots.remove(i);
                return;
            }
        }
    }
}

void LayoutSubtreeRootList::clearAndMarkContainingBlocksForLayout()
{
    for (auto& iter : m_roots)
        iter->markContainerChainForLayout(false);
    m_roots.clear();
}

LayoutObject* LayoutSubtreeRootList::randomRoot()
{
    ASSERT(!isEmpty());
    return *m_roots.begin();
}

LayoutObject* LayoutSubtreeRootList::takeDeepestRoot()
{
    if (m_orderedRoots.isEmpty()) {
        if (m_roots.isEmpty())
            return 0;

        copyToVector(m_roots, m_orderedRoots);
        std::sort(m_orderedRoots.begin(), m_orderedRoots.end());
        m_roots.clear();
    }
    LayoutObject* root = m_orderedRoots.last().object;
    m_orderedRoots.removeLast();
    return root;
}

void LayoutSubtreeRootList::countObjectsNeedingLayoutInRoot(const LayoutObject* object, unsigned& needsLayoutObjects, unsigned& totalObjects)
{
    for (const LayoutObject* o = object; o; o = o->nextInPreOrder(object)) {
        ++totalObjects;
        if (o->needsLayout())
            ++needsLayoutObjects;
    }
}

void LayoutSubtreeRootList::countObjectsNeedingLayout(unsigned& needsLayoutObjects, unsigned& totalObjects)
{
    // TODO(leviw): This will double-count nested roots crbug.com/509141
    for (auto& root : m_roots)
        countObjectsNeedingLayoutInRoot(root, needsLayoutObjects, totalObjects);
}

} // namespace blink
