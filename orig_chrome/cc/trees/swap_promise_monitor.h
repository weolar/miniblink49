// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_SWAP_PROMISE_MONITOR_H_
#define CC_TREES_SWAP_PROMISE_MONITOR_H_

#include "cc/base/cc_export.h"

namespace cc {

class LayerTreeHost;
class LayerTreeHostImpl;

// A SwapPromiseMonitor is used to monitor compositor state change that
// should be associated with a SwapPromise, e.g. SetNeedsCommit() is
// called on main thread or SetNeedsRedraw() is called on impl thread.
// Creating a SwapPromiseMonitor will insert itself into a LayerTreeHost
// or LayerTreeHostImpl. You must provide a pointer to the appropriate
// structure to the monitor (and only one of the two). Notification of
// compositor state change will be sent through OnSetNeedsCommitOnMain()
// or OnSetNeedsRedrawOnImpl(). When SwapPromiseMonitor is destroyed, it
// will unregister itself from LayerTreeHost or LayerTreeHostImpl.
class CC_EXPORT SwapPromiseMonitor {
public:
    // If the monitor lives on the main thread, pass in layer_tree_host
    // and set layer_tree_host_impl to nullptr.
    // If the monitor lives on the impl thread, pass in layer_tree_host_impl
    // and set layer_tree_host to nullptr.
    SwapPromiseMonitor(LayerTreeHost* layer_tree_host,
        LayerTreeHostImpl* layer_tree_host_impl);
    virtual ~SwapPromiseMonitor();

    virtual void OnSetNeedsCommitOnMain() = 0;
    virtual void OnSetNeedsRedrawOnImpl() = 0;
    virtual void OnForwardScrollUpdateToMainThreadOnImpl() = 0;

protected:
    LayerTreeHost* layer_tree_host_;
    LayerTreeHostImpl* layer_tree_host_impl_;
};

} // namespace cc

#endif // CC_TREES_SWAP_PROMISE_MONITOR_H_
