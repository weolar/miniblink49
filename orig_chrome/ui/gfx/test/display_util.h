// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_TEST_DISPLAY_UTIL_H_
#define UI_GFX_TEST_DISPLAY_UTIL_H_

#include "ui/gfx/display.h"

namespace gfx {

inline bool operator==(const gfx::Display& lhs, const gfx::Display& rhs)
{
    return lhs.id() == rhs.id() && lhs.bounds() == rhs.bounds() && lhs.work_area() == rhs.work_area() && lhs.device_scale_factor() == rhs.device_scale_factor() && lhs.rotation() == rhs.rotation() && lhs.touch_support() == rhs.touch_support();
}

} // namespace gfx

#endif // UI_GFX_TEST_DISPLAY_UTIL_H_
