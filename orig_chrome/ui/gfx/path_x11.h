// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_PATH_X11_H_
#define UI_GFX_PATH_X11_H_

#include "ui/gfx/gfx_export.h"

class SkPath;
class SkRegion;

typedef struct _XRegion REGION;

namespace gfx {

// Creates a new REGION given |region|. The caller is responsible for destroying
// the returned region.
GFX_EXPORT REGION* CreateRegionFromSkRegion(const SkRegion& region);

// Creates a new REGION given |path|. The caller is responsible for destroying
// the returned region.
GFX_EXPORT REGION* CreateRegionFromSkPath(const SkPath& path);

} // namespace gfx

#endif // UI_GFX_PATH_X11_H_
