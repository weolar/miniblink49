// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/ModuleProxy.h"

#include "wtf/StdLibExtras.h"

namespace blink {

ModuleProxy& ModuleProxy::moduleProxy()
{
    DEFINE_STATIC_LOCAL(ModuleProxy, moduleProxy, ());
    return moduleProxy;
}

} // namespace blink
