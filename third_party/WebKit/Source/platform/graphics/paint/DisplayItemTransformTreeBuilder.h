// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DisplayItemTransformTreeBuilder_h
#define DisplayItemTransformTreeBuilder_h

#include "platform/PlatformExport.h"
#include "platform/geometry/FloatSize.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Vector.h"

namespace blink {

class DisplayItem;
class DisplayItemTransformTree;

class PLATFORM_EXPORT DisplayItemTransformTreeBuilder {
public:
    DisplayItemTransformTreeBuilder();
    ~DisplayItemTransformTreeBuilder();

    // Detach the built transform tree.
    // This can be used to construct a WebDisplayItemTransformTree.
    PassOwnPtr<DisplayItemTransformTree> releaseTransformTree();

    // Process another display item from the list.
    void processDisplayItem(const DisplayItem&);

private:
    struct CurrentTransformNodeData {
        CurrentTransformNodeData(size_t nodeIndex, const FloatSize& offsetFromOrigin)
            : transformNode(nodeIndex), offset(offsetFromOrigin), ignoredBeginCount(0) { }

        // Index of the current transform node.
        size_t transformNode;

        // Offset of the current origin (i.e. where a drawing at (0, 0) would
        // be) from that node's origin.
        FloatSize offset;

        // Number of "begin" display items that have been ignored, whose
        // corresponding "end" display items should be skipped.
        unsigned ignoredBeginCount;
    };

    // Emit a range record, unless it would be empty.
    void finishRange();

    // Used to manipulate the current transform node stack.
    CurrentTransformNodeData& currentTransformNodeData() { return m_currentTransformNodeStack.last(); }
    void pushCurrentTransformNode(size_t nodeIndex, const FloatSize& offsetFromOrigin)
    {
        m_currentTransformNodeStack.append(CurrentTransformNodeData(nodeIndex, offsetFromOrigin));
    }
    void popCurrentTransformNode() { m_currentTransformNodeStack.removeLast(); }

    OwnPtr<DisplayItemTransformTree> m_transformTree;
    Vector<CurrentTransformNodeData, 16> m_currentTransformNodeStack;
    size_t m_rangeBeginIndex;
    size_t m_currentIndex;
};

} // namespace

#endif // DisplayItemTransformTreeBuilder_h
