// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_VECTOR_ICONS_PUBLIC_H_
#define UI_GFX_VECTOR_ICONS_PUBLIC_H_

#include "build/build_config.h"

#if defined(USE_AURA) || (!defined(OS_IOS) && !defined(OS_ANDROID))
#include "ui/gfx/vector_icons.h"
#else
// Define this type so non-views platforms will compile.
namespace gfx {
enum class VectorIconId {
    VECTOR_ICON_NONE,
};
} // namespace gfx
#endif

#endif // UI_GFX_VECTOR_ICONS_PUBLIC_H_
