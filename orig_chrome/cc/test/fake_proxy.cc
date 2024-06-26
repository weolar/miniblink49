// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_proxy.h"

namespace cc {

void FakeProxy::SetLayerTreeHost(LayerTreeHost* host)
{
    layer_tree_host_ = host;
}

bool FakeProxy::IsStarted() const { return true; }

bool FakeProxy::CommitToActiveTree() const
{
    return false;
}

const RendererCapabilities& FakeProxy::GetRendererCapabilities() const
{
    return capabilities_;
}

RendererCapabilities& FakeProxy::GetRendererCapabilities()
{
    return capabilities_;
}

void FakeProxy::ReleaseOutputSurface() { }

bool FakeProxy::BeginMainFrameRequested() const { return false; }

bool FakeProxy::CommitRequested() const { return false; }

bool FakeProxy::SupportsImplScrolling() const
{
    return true;
}

bool FakeProxy::MainFrameWillHappenForTesting()
{
    return false;
}

} // namespace cc
