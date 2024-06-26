// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAILURE_OUTPUT_SURFACE_H_
#define CC_TEST_FAILURE_OUTPUT_SURFACE_H_

#include "cc/test/fake_output_surface.h"

namespace cc {

class FailureOutputSurface : public FakeOutputSurface {
public:
    explicit FailureOutputSurface(bool is_delegating);

    bool BindToClient(OutputSurfaceClient* client) override;

private:
    DISALLOW_COPY_AND_ASSIGN(FailureOutputSurface);
};

} // namespace cc

#endif // CC_TEST_FAILURE_OUTPUT_SURFACE_H_
