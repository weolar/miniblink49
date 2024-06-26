// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/bsp_tree.h"

#include <vector>

#include "base/memory/scoped_ptr.h"
#include "cc/base/scoped_ptr_deque.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/output/bsp_compare_result.h"
#include "cc/quads/draw_polygon.h"

namespace cc {

BspNode::BspNode(scoped_ptr<DrawPolygon> data)
    : node_data(data.Pass())
{
}

BspNode::~BspNode()
{
}

BspTree::BspTree(ScopedPtrDeque<DrawPolygon>* list)
{
    if (list->size() == 0)
        return;

    root_ = make_scoped_ptr(new BspNode(list->take_front()));
    BuildTree(root_.get(), list);
}

// The idea behind using a deque for BuildTree's input is that we want to be
// able to place polygons that we've decided aren't splitting plane candidates
// at the back of the queue while moving the candidate splitting planes to the
// front when the heuristic decides that they're a better choice. This way we
// can always simply just take from the front of the deque for our node's
// data.
void BspTree::BuildTree(BspNode* node,
    ScopedPtrDeque<DrawPolygon>* polygon_list)
{
    ScopedPtrDeque<DrawPolygon> front_list;
    ScopedPtrDeque<DrawPolygon> back_list;

    // We take in a list of polygons at this level of the tree, and have to
    // find a splitting plane, then classify polygons as either in front of
    // or behind that splitting plane.
    while (polygon_list->size() > 0) {
        // Is this particular polygon in front of or behind our splitting polygon.
        BspCompareResult comparer_result = GetNodePositionRelative(*polygon_list->front(), *(node->node_data));

        // If it's clearly behind or in front of the splitting plane, we use the
        // heuristic to decide whether or not we should put it at the back
        // or front of the list.
        switch (comparer_result) {
        case BSP_FRONT:
            front_list.push_back(polygon_list->take_front().Pass());
            break;
        case BSP_BACK:
            back_list.push_back(polygon_list->take_front().Pass());
            break;
        case BSP_SPLIT: {
            scoped_ptr<DrawPolygon> polygon;
            scoped_ptr<DrawPolygon> new_front;
            scoped_ptr<DrawPolygon> new_back;
            // Time to split this geometry, *it needs to be split by node_data.
            polygon = polygon_list->take_front();
            bool split_result = polygon->Split(*(node->node_data), &new_front, &new_back);
            DCHECK(split_result);
            if (!split_result) {
                break;
            }
            front_list.push_back(new_front.Pass());
            back_list.push_back(new_back.Pass());
            break;
        }
        case BSP_COPLANAR_FRONT:
            node->coplanars_front.push_back(polygon_list->take_front());
            break;
        case BSP_COPLANAR_BACK:
            node->coplanars_back.push_back(polygon_list->take_front());
            break;
        default:
            NOTREACHED();
            break;
        }
    }

    // Build the back subtree using the front of the back_list as our splitter.
    if (back_list.size() > 0) {
        node->back_child = make_scoped_ptr(new BspNode(back_list.take_front()));
        BuildTree(node->back_child.get(), &back_list);
    }

    // Build the front subtree using the front of the front_list as our splitter.
    if (front_list.size() > 0) {
        node->front_child = scoped_ptr<BspNode>(new BspNode(front_list.take_front()));
        BuildTree(node->front_child.get(), &front_list);
    }
}

BspCompareResult BspTree::GetNodePositionRelative(const DrawPolygon& node_a,
    const DrawPolygon& node_b)
{
    return DrawPolygon::SideCompare(node_a, node_b);
}

// The base comparer with 0,0,0 as camera position facing forward
BspCompareResult BspTree::GetCameraPositionRelative(const DrawPolygon& node)
{
    if (node.normal().z() > 0.0f) {
        return BSP_FRONT;
    }
    return BSP_BACK;
}

BspTree::~BspTree()
{
}

} // namespace cc
