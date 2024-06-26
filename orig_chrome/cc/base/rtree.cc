// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/base/rtree.h"

#include <cmath>

#include "base/logging.h"

namespace cc {

RTree::RTree()
    : num_data_elements_(0u)
{
}

RTree::~RTree() { }

RTree::Node* RTree::AllocateNodeAtLevel(int level)
{
    nodes_.push_back(Node());
    Node& node = nodes_.back();
    node.num_children = 0;
    node.level = level;
    return &node;
}

RTree::Branch RTree::BuildRecursive(std::vector<Branch>* branches, int level)
{
    // Only one branch.  It will be the root.
    if (branches->size() == 1)
        return (*branches)[0];

    // TODO(vmpstr): Investigate if branches should be sorted in y.
    // The comment from Skia reads:
    // We might sort our branches here, but we expect Blink gives us a reasonable
    // x,y order. Skipping a call to sort (in Y) here resulted in a 17% win for
    // recording with negligible difference in playback speed.
    int num_branches = static_cast<int>(branches->size() / MAX_CHILDREN);
    int remainder = static_cast<int>(branches->size() % MAX_CHILDREN);

    if (remainder > 0) {
        ++num_branches;
        // If the remainder isn't enough to fill a node, we'll add fewer nodes to
        // other branches.
        if (remainder >= MIN_CHILDREN)
            remainder = 0;
        else
            remainder = MIN_CHILDREN - remainder;
    }

    int num_strips = static_cast<int>(std::ceil(std::sqrt(num_branches)));
    int num_tiles = static_cast<int>(
        std::ceil(num_branches / static_cast<float>(num_strips)));
    size_t current_branch = 0;

    size_t new_branch_index = 0;
    for (int i = 0; i < num_strips; ++i) {
        // Might be worth sorting by X here too.
        for (int j = 0; j < num_tiles && current_branch < branches->size(); ++j) {
            int increment_by = MAX_CHILDREN;
            if (remainder != 0) {
                // if need be, omit some nodes to make up for remainder
                if (remainder <= MAX_CHILDREN - MIN_CHILDREN) {
                    increment_by -= remainder;
                    remainder = 0;
                } else {
                    increment_by = MIN_CHILDREN;
                    remainder -= MAX_CHILDREN - MIN_CHILDREN;
                }
            }
            Node* node = AllocateNodeAtLevel(level);
            node->num_children = 1;
            node->children[0] = (*branches)[current_branch];

            Branch branch;
            branch.bounds = (*branches)[current_branch].bounds;
            branch.subtree = node;
            ++current_branch;
            for (int k = 1; k < increment_by && current_branch < branches->size();
                 ++k) {
                branch.bounds.Union((*branches)[current_branch].bounds);
                node->children[k] = (*branches)[current_branch];
                ++node->num_children;
                ++current_branch;
            }
            DCHECK_LT(new_branch_index, current_branch);
            (*branches)[new_branch_index] = branch;
            ++new_branch_index;
        }
    }
    branches->resize(new_branch_index);
    return BuildRecursive(branches, level + 1);
}

void RTree::Search(const gfx::RectF& query,
    std::vector<size_t>* results) const
{
    if (num_data_elements_ > 0 && query.Intersects(root_.bounds))
        SearchRecursive(root_.subtree, query, results);
}

void RTree::SearchRecursive(Node* node,
    const gfx::RectF& query,
    std::vector<size_t>* results) const
{
    for (uint16_t i = 0; i < node->num_children; ++i) {
        if (query.Intersects(node->children[i].bounds)) {
            if (node->level == 0)
                results->push_back(node->children[i].index);
            else
                SearchRecursive(node->children[i].subtree, query, results);
        }
    }
}

} // namespace cc
