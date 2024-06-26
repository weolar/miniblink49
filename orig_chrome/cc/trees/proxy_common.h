// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_PROXY_COMMON_H_
#define CC_TREES_PROXY_COMMON_H_

#include "cc/base/cc_export.h"
#include "cc/output/begin_frame_args.h"
#include "cc/trees/layer_tree_host_common.h"

namespace cc {
class LayerTreeHost;

struct CC_EXPORT BeginMainFrameAndCommitState {
    BeginMainFrameAndCommitState();
    ~BeginMainFrameAndCommitState();

    unsigned int begin_frame_id;
    BeginFrameArgs begin_frame_args;
    scoped_ptr<ScrollAndScaleSet> scroll_info;
    size_t memory_allocation_limit_bytes;
    bool evicted_ui_resources;
};

} // namespace cc

#endif // CC_TREES_PROXY_COMMON_H_
