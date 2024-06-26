// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/input/layer_selection_bound.h"

namespace cc {

LayerSelectionBound::LayerSelectionBound()
    : type(SELECTION_BOUND_EMPTY)
    , layer_id(0)
{
}

LayerSelectionBound::~LayerSelectionBound()
{
}

bool operator==(const LayerSelectionBound& lhs,
    const LayerSelectionBound& rhs)
{
    return lhs.type == rhs.type && lhs.layer_id == rhs.layer_id && lhs.edge_top == rhs.edge_top && lhs.edge_bottom == rhs.edge_bottom;
}

bool operator!=(const LayerSelectionBound& lhs,
    const LayerSelectionBound& rhs)
{
    return !(lhs == rhs);
}

} // namespace cc
