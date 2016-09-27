// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebDisplayItemTransformTree_h
#define WebDisplayItemTransformTree_h

#include "public/platform/WebCommon.h"
#include "public/platform/WebFloatSize.h"
#include "public/platform/WebPrivateOwnPtr.h"
#include "third_party/skia/include/utils/SkMatrix44.h"

namespace blink {

class DisplayItemTransformTree;

// Represents the hierarchy of transforms which apply to ranges of a display
// item list and may be of interest to the compositor.
//
// It consists of a tree of "transform nodes", stored in a flattened
// representation in which their order is not guaranteed. Each node has a
// parent, relative to whom its transform should be interpreted (i.e. the
// total transform at a node is the product of its transform and its parent's
// total transform).
//
// These nodes are associated with a display item list through the associated
// "range records", which correspond to non-overlapping ranges of display items
// in the list, in sorted order. Since the begin/end display items that create
// transform nodes are not included in these ranges, and empty ranges are
// omitted, these ranges are not a partition. Rather, they constitute a partial
// map from display item indices to transform node indices.
//
// Similarly, there may be transform nodes with no associated range records.
// This doesn't necessarily mean that it can be ignored -- it may be the parent
// of one or more other transform nodes.
class WebDisplayItemTransformTree {
public:
    enum : size_t { kInvalidIndex = static_cast<size_t>(-1) };

    struct TransformNode {
        TransformNode(size_t parent, const SkMatrix44& matrix44)
            : parentNodeIndex(parent)
            , matrix(matrix44)
        {
        }

        bool isRoot() const { return parentNodeIndex == kInvalidIndex; }

        // Index of parent in m_nodes (kInvalidIndex for root).
        size_t parentNodeIndex;

        // Transformation matrix of this node, relative to its parent.
        SkMatrix44 matrix;
    };

    struct RangeRecord {
        RangeRecord(size_t beginIndex, size_t endIndex, size_t nodeIndex, const WebFloatSize& drawingOffset = WebFloatSize())
            : displayListBeginIndex(beginIndex)
            , displayListEndIndex(endIndex)
            , transformNodeIndex(nodeIndex)
            , offset(drawingOffset)
        {
        }

        bool operator==(const RangeRecord& other) const
        {
            return displayListBeginIndex == other.displayListBeginIndex
                && displayListEndIndex == other.displayListEndIndex
                && transformNodeIndex == other.transformNodeIndex
                && offset == other.offset;
        }
        bool operator!=(const RangeRecord& other) const { return !(*this == other); }

        // Index of first affected display item.
        size_t displayListBeginIndex;

        // Index of first unaffected display item after |displayListBeginIndex|.
        size_t displayListEndIndex;

        // Index of a the applicable transform node (in |m_nodes|).
        size_t transformNodeIndex;

        // The offset of this range's drawing in the coordinate space of the
        // transform node.
        WebFloatSize offset;
    };

    BLINK_PLATFORM_EXPORT WebDisplayItemTransformTree();
#if INSIDE_BLINK
    BLINK_PLATFORM_EXPORT WebDisplayItemTransformTree(
        const WTF::PassOwnPtr<DisplayItemTransformTree>&);
#endif

    BLINK_PLATFORM_EXPORT ~WebDisplayItemTransformTree();

    // Returns the number of nodes in the transform tree.
    BLINK_PLATFORM_EXPORT size_t nodeCount() const;

    // Returns a node in the transform tree by its index (from 0 to nodeCount() - 1).
    BLINK_PLATFORM_EXPORT const TransformNode& nodeAt(size_t index) const;

    // Returns the parent of the given node.
    // Do not call this with the root node.
    BLINK_PLATFORM_EXPORT const TransformNode& parentNode(const TransformNode&) const;

    // Returns the number of display item ranges.
    BLINK_PLATFORM_EXPORT size_t rangeRecordCount() const;

    // Returns the requested display item range, sorted by position in the
    // display item list.
    BLINK_PLATFORM_EXPORT const RangeRecord& rangeRecordAt(size_t index) const;

private:
    WebPrivateOwnPtr<const DisplayItemTransformTree> m_private;
};

} // namespace blink

#endif // WebDisplayItemTransformTree_h
