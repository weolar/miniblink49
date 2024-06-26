// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_INPUT_SCROLL_STATE_H_
#define CC_INPUT_SCROLL_STATE_H_

#include <list>

#include "cc/base/cc_export.h"

namespace cc {

class LayerImpl;

// ScrollState is based on the proposal for scroll customization in blink, found
// here: https://goo.gl/1ipTpP.
class CC_EXPORT ScrollState {
public:
    ScrollState(double delta_x,
        double delta_y,
        int start_position_x,
        int start_position_y,
        bool should_propagate,
        bool delta_consumed_for_scroll_sequence,
        bool is_direct_manipulation);
    ~ScrollState();

    // Reduce deltas by x, y.
    void ConsumeDelta(double x, double y);
    // Pops the first layer off of |scroll_chain_| and calls
    // |DistributeScroll| on it.
    void DistributeToScrollChainDescendant();
    // Positive when scrolling left.
    double delta_x() const { return delta_x_; }
    // Positive when scrolling up.
    double delta_y() const { return delta_y_; }
    // The location the scroll started at. For touch, the starting
    // position of the finger. For mouse, the location of the cursor.
    int start_position_x() const { return start_position_x_; }
    int start_position_y() const { return start_position_y_; }

    // True if this scroll is allowed to bubble upwards.
    bool should_propagate() const { return should_propagate_; }
    // True if the user interacts directly with the screen, e.g., via touch.
    bool is_direct_manipulation() const { return is_direct_manipulation_; }

    void set_scroll_chain(const std::list<LayerImpl*>& scroll_chain)
    {
        scroll_chain_ = scroll_chain;
    }

    void set_current_native_scrolling_layer(LayerImpl* layer)
    {
        current_native_scrolling_layer_ = layer;
    }

    LayerImpl* current_native_scrolling_layer() const
    {
        return current_native_scrolling_layer_;
    }

    bool delta_consumed_for_scroll_sequence() const
    {
        return delta_consumed_for_scroll_sequence_;
    }

    bool FullyConsumed() const { return !delta_x_ && !delta_y_; }

    void set_caused_scroll(bool x, bool y)
    {
        caused_scroll_x_ |= x;
        caused_scroll_y_ |= y;
    }

    bool caused_scroll_x() const { return caused_scroll_x_; }
    bool caused_scroll_y() const { return caused_scroll_y_; }

private:
    ScrollState();
    double delta_x_;
    double delta_y_;
    double start_position_x_;
    double start_position_y_;

    bool should_propagate_;

    // The last layer to respond to a scroll, or null if none exists.
    LayerImpl* current_native_scrolling_layer_;
    // Whether the scroll sequence has had any delta consumed, in the
    // current frame, or any child frames.
    bool delta_consumed_for_scroll_sequence_;
    // True if the user interacts directly with the display, e.g., via
    // touch.
    bool is_direct_manipulation_;
    // TODO(tdresser): ScrollState shouldn't need to keep track of whether or not
    // this ScrollState object has caused a scroll. Ideally, any native scroller
    // consuming delta has caused a scroll. Currently, there are some cases where
    // we consume delta without scrolling, such as in
    // |Viewport::AdjustOverscroll|. Once these cases are fixed, we should get rid
    // of |caused_scroll_*_|. See crbug.com/510045 for details.
    bool caused_scroll_x_;
    bool caused_scroll_y_;

    // TODO(tdresser): Change LayerImpl* to an abstract scrollable type. See
    // crbug.com/476553 for detail on the effort to unify impl and main thread
    // scrolling, which will require an abstract scrollable type.
    std::list<LayerImpl*> scroll_chain_;
};

} // namespace cc

#endif // CC_INPUT_SCROLL_STATE_H_
