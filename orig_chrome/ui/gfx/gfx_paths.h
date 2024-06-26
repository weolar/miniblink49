// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_GFX_PATHS_H_
#define UI_GFX_GFX_PATHS_H_

#include "ui/gfx/gfx_export.h"

// This file declares path keys for the app module.  These can be used with
// the PathService to access various special directories and files.

namespace gfx {

enum {
    PATH_START = 2000,

    // Valid only in development environment; TODO(darin): move these
    DIR_TEST_DATA, // Directory where unit test data resides.

    PATH_END
};

// Call once to register the provider for the path keys defined above.
GFX_EXPORT void RegisterPathProvider();

} // namespace gfx

#endif // UI_GFX_GFX_PATHS_H_
