// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_X_X11_SWITCHES_H_
#define UI_GFX_X_X11_SWITCHES_H_

#include "build/build_config.h"
#include "ui/gfx/gfx_export.h"

namespace switches {

#if !defined(OS_CHROMEOS)
GFX_EXPORT extern const char kEnableTransparentVisuals[];
GFX_EXPORT extern const char kWindowDepth[];
GFX_EXPORT extern const char kX11Display[];
#endif

} // namespace switches

#endif // UI_GFX_X_X11_SWITCHES_H_
