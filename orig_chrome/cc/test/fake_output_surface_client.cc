// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_output_surface_client.h"
#include "cc/output/output_surface.h"

namespace cc {

void FakeOutputSurfaceClient::DidSwapBuffers()
{
    swap_count_++;
}

void FakeOutputSurfaceClient::DidLoseOutputSurface()
{
    did_lose_output_surface_called_ = true;
}

void FakeOutputSurfaceClient::SetMemoryPolicy(
    const ManagedMemoryPolicy& policy)
{
    memory_policy_ = policy;
}

} // namespace cc
