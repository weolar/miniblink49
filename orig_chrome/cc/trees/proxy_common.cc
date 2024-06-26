// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/proxy_common.h"
#include "cc/trees/layer_tree_host.h"

namespace cc {

BeginMainFrameAndCommitState::BeginMainFrameAndCommitState()
    : memory_allocation_limit_bytes(0)
    , evicted_ui_resources(false)
{
}

BeginMainFrameAndCommitState::~BeginMainFrameAndCommitState() { }

} // namespace cc
