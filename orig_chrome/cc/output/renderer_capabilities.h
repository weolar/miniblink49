// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_RENDERER_CAPABILITIES_H_
#define CC_OUTPUT_RENDERER_CAPABILITIES_H_

#include "cc/base/cc_export.h"
#include "cc/resources/resource_format.h"

namespace cc {

// Represents the set of capabilities that a particular Renderer has.
struct CC_EXPORT RendererCapabilities {
    RendererCapabilities(ResourceFormat best_texture_format,
        bool allow_partial_texture_updates,
        int max_texture_size,
        bool using_shared_memory_resources);

    RendererCapabilities();
    ~RendererCapabilities();

    // Duplicate any modification to this list to RendererCapabilitiesImpl.
    ResourceFormat best_texture_format;
    bool allow_partial_texture_updates;
    int max_texture_size;
    bool using_shared_memory_resources;
};

} // namespace cc

#endif // CC_OUTPUT_RENDERER_CAPABILITIES_H_
