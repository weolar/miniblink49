// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/screen.h"

#include "base/logging.h"

namespace gfx {

Screen* CreateNativeScreen()
{
    NOTREACHED() << "Implementation should be installed at higher level.";
    return NULL;
}

} // namespace gfx
