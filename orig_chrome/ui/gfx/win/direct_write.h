// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_WIN_DIRECT_WRITE_H_
#define UI_GFX_WIN_DIRECT_WRITE_H_

#include <dwrite.h>

#include "ui/gfx/gfx_export.h"

namespace gfx {
namespace win {

    // Returns whether DirectWrite font rendering should be used.
    GFX_EXPORT bool ShouldUseDirectWrite();

    GFX_EXPORT void MaybeInitializeDirectWrite();

    // Returns true if we are using DirectWrite for font metrics and rendering.
    GFX_EXPORT bool IsDirectWriteEnabled();

    // Creates a DirectWrite factory, if using DirectWrite.
    GFX_EXPORT void CreateDWriteFactory(IDWriteFactory** factory);

} // namespace win
} // namespace gfx

#endif // UI_GFX_WIN_DIRECT_WRITE_H_
