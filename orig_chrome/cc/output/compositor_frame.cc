// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/compositor_frame.h"

namespace cc {

CompositorFrame::CompositorFrame() { }

CompositorFrame::~CompositorFrame() { }

void CompositorFrame::AssignTo(CompositorFrame* target)
{
    target->delegated_frame_data = delegated_frame_data.Pass();
    target->gl_frame_data = gl_frame_data.Pass();
    target->metadata = metadata;
}

} // namespace cc
