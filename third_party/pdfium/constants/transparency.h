// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSTANTS_TRANSPARENCY_H_
#define CONSTANTS_TRANSPARENCY_H_

namespace pdfium {
namespace transparency {

// PDF 1.7 spec, table 7.2.
// Standard separable blend modes.

constexpr char kNormal[] = "Normal";
constexpr char kMultiply[] = "Multiply";
constexpr char kScreen[] = "Screen";
constexpr char kOverlay[] = "Overlay";
constexpr char kDarken[] = "Darken";
constexpr char kLighten[] = "Lighten";
constexpr char kColorDodge[] = "ColorDodge";
constexpr char kColorBurn[] = "ColorBurn";
constexpr char kHardLight[] = "HardLight";
constexpr char kSoftLight[] = "SoftLight";
constexpr char kDifference[] = "Difference";
constexpr char kExclusion[] = "Exclusion";

// PDF 1.7 spec, table 7.3.
// Standard nonseparable blend modes.

constexpr char kHue[] = "Hue";
constexpr char kSaturation[] = "Saturation";
constexpr char kColor[] = "Color";
constexpr char kLuminosity[] = "Luminosity";

// PDF 1.7 spec, table 7.10.
// Entries in a soft-mask dictionary.

constexpr char kSoftMaskSubType[] = "S";
constexpr char kAlpha[] = "Alpha";
constexpr char kG[] = "G";
constexpr char kBC[] = "BC";
constexpr char kTR[] = "TR";

// PDF 1.7 spec, table 7.13.
// Additional entries specific to a transparency group attributes dictionary.

constexpr char kGroupSubType[] = "S";
constexpr char kTransparency[] = "Transparency";
constexpr char kCS[] = "CS";
constexpr char kI[] = "I";

}  // namespace transparency
}  // namespace pdfium

#endif  // CONSTANTS_TRANSPARENCY_H_
