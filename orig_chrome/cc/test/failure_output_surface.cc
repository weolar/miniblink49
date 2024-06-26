// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/failure_output_surface.h"

namespace cc {

FailureOutputSurface::FailureOutputSurface(bool is_delegating)
    : FakeOutputSurface(static_cast<ContextProvider*>(nullptr), is_delegating)
{
}

bool FailureOutputSurface::BindToClient(OutputSurfaceClient* client)
{
    // This will force this output surface to not initialize in LTHI
    // and eventually get back to LTH::DidFailToInitializeOutputSurface;
    return false;
}

} // namespace cc
