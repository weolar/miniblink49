// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_SCOPED_ABORT_REMAINING_SWAP_PROMISES_H_
#define CC_TREES_SCOPED_ABORT_REMAINING_SWAP_PROMISES_H_

#include "cc/output/swap_promise.h"
#include "cc/trees/layer_tree_host.h"

namespace cc {

class ScopedAbortRemainingSwapPromises {
public:
    explicit ScopedAbortRemainingSwapPromises(LayerTreeHost* layer_tree_host)
        : layer_tree_host_(layer_tree_host)
    {
    }

    ~ScopedAbortRemainingSwapPromises()
    {
        layer_tree_host_->BreakSwapPromises(SwapPromise::COMMIT_FAILS);
    }

private:
    LayerTreeHost* layer_tree_host_;

    DISALLOW_COPY_AND_ASSIGN(ScopedAbortRemainingSwapPromises);
};

} // namespace cc

#endif // CC_TREES_SCOPED_ABORT_REMAINING_SWAP_PROMISES_H_
