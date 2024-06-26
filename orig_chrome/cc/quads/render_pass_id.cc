// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/quads/render_pass_id.h"

namespace cc {

void* RenderPassId::AsTracingId() const
{
    static_assert(sizeof(size_t) <= sizeof(void*), // NOLINT
        "size of size_t should not be greater than that of a pointer");
    return reinterpret_cast<void*>(
        base::HashPair(layer_id, static_cast<int>(index)));
}

} // namespace cc
