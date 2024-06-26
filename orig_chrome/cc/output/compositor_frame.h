// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_COMPOSITOR_FRAME_H_
#define CC_OUTPUT_COMPOSITOR_FRAME_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/output/compositor_frame_metadata.h"
#include "cc/output/delegated_frame_data.h"
#include "cc/output/gl_frame_data.h"

namespace cc {

// A CompositorFrame struct contains the complete output of a compositor meant
// for display.
// TODO(fsamuel): Write more here.
class CC_EXPORT CompositorFrame {
public:
    CompositorFrame();
    ~CompositorFrame();

    CompositorFrameMetadata metadata;
    scoped_ptr<DelegatedFrameData> delegated_frame_data;
    scoped_ptr<GLFrameData> gl_frame_data;

    void AssignTo(CompositorFrame* target);

private:
    DISALLOW_COPY_AND_ASSIGN(CompositorFrame);
};

} // namespace cc

#endif // CC_OUTPUT_COMPOSITOR_FRAME_H_
