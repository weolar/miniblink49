// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/swap_promise_monitor.h"
#include "base/logging.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/trees/layer_tree_host_impl.h"

namespace cc {

SwapPromiseMonitor::SwapPromiseMonitor(LayerTreeHost* layer_tree_host,
    LayerTreeHostImpl* layer_tree_host_impl)
    : layer_tree_host_(layer_tree_host)
    , layer_tree_host_impl_(layer_tree_host_impl)
{
    DCHECK((layer_tree_host && !layer_tree_host_impl) || (!layer_tree_host && layer_tree_host_impl));
    if (layer_tree_host_)
        layer_tree_host_->InsertSwapPromiseMonitor(this);
    if (layer_tree_host_impl_)
        layer_tree_host_impl_->InsertSwapPromiseMonitor(this);
}

SwapPromiseMonitor::~SwapPromiseMonitor()
{
    if (layer_tree_host_)
        layer_tree_host_->RemoveSwapPromiseMonitor(this);
    if (layer_tree_host_impl_)
        layer_tree_host_impl_->RemoveSwapPromiseMonitor(this);
}

} // namespace cc
