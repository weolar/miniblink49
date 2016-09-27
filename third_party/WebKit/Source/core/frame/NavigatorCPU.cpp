// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/NavigatorCPU.h"

#include "public/platform/Platform.h"

namespace blink {

unsigned NavigatorCPU::hardwareConcurrency() const
{
    return Platform::current()->numberOfProcessors();
}

} // namespace blink
