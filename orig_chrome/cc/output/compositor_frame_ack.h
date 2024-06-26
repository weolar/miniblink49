// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_COMPOSITOR_FRAME_ACK_H_
#define CC_OUTPUT_COMPOSITOR_FRAME_ACK_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/output/gl_frame_data.h"
#include "cc/resources/returned_resource.h"

namespace cc {

class CC_EXPORT CompositorFrameAck {
public:
    CompositorFrameAck();
    ~CompositorFrameAck();

    ReturnedResourceArray resources;
    scoped_ptr<GLFrameData> gl_frame_data;

private:
    DISALLOW_COPY_AND_ASSIGN(CompositorFrameAck);
};

} // namespace cc

#endif // CC_OUTPUT_COMPOSITOR_FRAME_ACK_H_
