// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/compositor_frame_metadata.h"

namespace cc {

CompositorFrameMetadata::CompositorFrameMetadata()
    : device_scale_factor(0.f)
    , page_scale_factor(0.f)
    , min_page_scale_factor(0.f)
    , max_page_scale_factor(0.f)
    , root_overflow_x_hidden(false)
    , root_overflow_y_hidden(false)
    , root_background_color(SK_ColorWHITE)
{
}

CompositorFrameMetadata::~CompositorFrameMetadata()
{
}

} // namespace cc
