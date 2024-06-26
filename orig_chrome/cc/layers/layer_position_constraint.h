// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_LAYER_POSITION_CONSTRAINT_H_
#define CC_LAYERS_LAYER_POSITION_CONSTRAINT_H_

#include "cc/base/cc_export.h"

namespace cc {

class CC_EXPORT LayerPositionConstraint {
public:
    LayerPositionConstraint();

    void set_is_fixed_position(bool fixed) { is_fixed_position_ = fixed; }
    bool is_fixed_position() const { return is_fixed_position_; }
    void set_is_fixed_to_right_edge(bool fixed)
    {
        is_fixed_to_right_edge_ = fixed;
    }
    bool is_fixed_to_right_edge() const { return is_fixed_to_right_edge_; }
    void set_is_fixed_to_bottom_edge(bool fixed)
    {
        is_fixed_to_bottom_edge_ = fixed;
    }
    bool is_fixed_to_bottom_edge() const { return is_fixed_to_bottom_edge_; }

    bool operator==(const LayerPositionConstraint&) const;
    bool operator!=(const LayerPositionConstraint&) const;

private:
    bool is_fixed_position_ : 1;
    bool is_fixed_to_right_edge_ : 1;
    bool is_fixed_to_bottom_edge_ : 1;
};

} // namespace cc

#endif // CC_LAYERS_LAYER_POSITION_CONSTRAINT_H_
