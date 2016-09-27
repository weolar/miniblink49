// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/paint/DisplayItemTransformTree.h"

namespace blink {

DisplayItemTransformTree::DisplayItemTransformTree()
{
    // There is always a root node.
    m_nodes.append(TransformNode(kInvalidIndex, SkMatrix44::kIdentity_Constructor));
    ASSERT(m_nodes[0].isRoot());
}

DisplayItemTransformTree::~DisplayItemTransformTree()
{
}

} // namespace blink
