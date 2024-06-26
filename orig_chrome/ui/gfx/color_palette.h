// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_COLOR_PALETTE_H_
#define UI_GFX_COLOR_PALETTE_H_

#include "third_party/skia/include/core/SkColor.h"

namespace gfx {

// A placeholder value for unset colors. This should never be visible and is red
// as a visual flag for misbehaving code.
const SkColor kPlaceholderColor = SK_ColorRED;

const SkColor kChromeIconGrey = SkColorSetRGB(0x5A, 0x5A, 0x5A);

// The number refers to the shade of darkness. Each color in the MD
// palette ranges from 100-900.
const SkColor kGoogleBlue300 = SkColorSetRGB(0x7B, 0xAA, 0xF7);
const SkColor kGoogleBlue500 = SkColorSetRGB(0x42, 0x85, 0xF4);
const SkColor kGoogleBlue700 = SkColorSetRGB(0x33, 0x67, 0xD6);
const SkColor kGoogleRed300 = SkColorSetRGB(0xE6, 0x7C, 0x73);
const SkColor kGoogleRed700 = SkColorSetRGB(0xC5, 0x39, 0x29);
const SkColor kGoogleGreen300 = SkColorSetRGB(0x57, 0xBB, 0x8A);
const SkColor kGoogleGreen700 = SkColorSetRGB(0x0B, 0x80, 0x43);
const SkColor kGoogleYellow300 = SkColorSetRGB(0xF7, 0xCB, 0x4D);
const SkColor kGoogleYellow700 = SkColorSetRGB(0xF0, 0x93, 0x00);

} // namespace gfx

#endif // UI_GFX_COLOR_PALETTE_H_
