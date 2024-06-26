// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/input/scroll_state.h"

#include "cc/layers/layer_impl.h"

namespace cc {

ScrollState::ScrollState(double delta_x,
    double delta_y,
    int start_position_x,
    int start_position_y,
    bool should_propagate,
    bool delta_consumed_for_scroll_sequence,
    bool is_direct_manipulation)
    : delta_x_(delta_x)
    , delta_y_(delta_y)
    , start_position_x_(start_position_x)
    , start_position_y_(start_position_y)
    , should_propagate_(should_propagate)
    , delta_consumed_for_scroll_sequence_(delta_consumed_for_scroll_sequence)
    , is_direct_manipulation_(is_direct_manipulation)
    , caused_scroll_x_(false)
    , caused_scroll_y_(false)
{
}

ScrollState::~ScrollState() { }

void ScrollState::ConsumeDelta(double x, double y)
{
    delta_x_ -= x;
    delta_y_ -= y;

    if (x || y)
        delta_consumed_for_scroll_sequence_ = true;
}

void ScrollState::DistributeToScrollChainDescendant()
{
    if (!scroll_chain_.empty()) {
        LayerImpl* next = scroll_chain_.front();
        scroll_chain_.pop_front();
        next->DistributeScroll(this);
    }
}

} // namespace cc
