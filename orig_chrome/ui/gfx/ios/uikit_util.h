// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_IOS_UIKIT_UTIL_H_
#define UI_GFX_IOS_UIKIT_UTIL_H_

#import <UIKit/UIKit.h>

#include "base/compiler_specific.h"

// UI Util containing functions that require UIKit.

namespace ui {

// Returns the closest pixel-aligned value higher than |value|, taking the scale
// factor into account. At a scale of 1, equivalent to ceil().
CGFloat AlignValueToUpperPixel(CGFloat value) WARN_UNUSED_RESULT;

// Returns the size resulting from applying AlignToUpperPixel to both
// components.
CGSize AlignSizeToUpperPixel(CGSize size) WARN_UNUSED_RESULT;

} // namespace ui

#endif // UI_GFX_IOS_UIKIT_UTIL_H_
