// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/context_provider.h"

#include <limits>

namespace cc {

ContextProvider::Capabilities::Capabilities()
    : max_transfer_buffer_usage_bytes(std::numeric_limits<size_t>::max())
{
}

} // namespace cc
