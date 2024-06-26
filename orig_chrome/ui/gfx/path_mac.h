// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_PATH_MAC_H_
#define UI_GFX_PATH_MAC_H_

#include "ui/gfx/gfx_export.h"

@class NSBezierPath;
class SkPath;

namespace gfx {

// Returns an autoreleased NSBezierPath corresponding to |path|. Caller should
// call retain on the returned object, if it wishes to take ownership.
GFX_EXPORT NSBezierPath* CreateNSBezierPathFromSkPath(const SkPath& path);

} // namespace gfx

#endif // UI_GFX_PATH_MAC_H_
