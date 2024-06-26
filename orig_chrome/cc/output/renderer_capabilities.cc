// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/renderer_capabilities.h"

namespace cc {

RendererCapabilities::RendererCapabilities(ResourceFormat best_texture_format,
    bool allow_partial_texture_updates,
    int max_texture_size,
    bool using_shared_memory_resources)
    : best_texture_format(best_texture_format)
    , allow_partial_texture_updates(allow_partial_texture_updates)
    , max_texture_size(max_texture_size)
    , using_shared_memory_resources(using_shared_memory_resources)
{
}

RendererCapabilities::RendererCapabilities()
    : best_texture_format(RGBA_8888)
    , allow_partial_texture_updates(false)
    , max_texture_size(0)
    , using_shared_memory_resources(false)
{
}

RendererCapabilities::~RendererCapabilities()
{
}

} // namespace cc
