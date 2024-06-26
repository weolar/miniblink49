// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_COLOR_UTILS_H_
#define UI_GFX_COLOR_UTILS_H_

#include "third_party/skia/include/core/SkColor.h"
#include "ui/gfx/gfx_export.h"

class SkBitmap;

namespace color_utils {

// Represents an HSL color.
struct HSL {
    double h;
    double s;
    double l;
};

// The minimum contrast between text and background that is still readable.
// This value is taken from w3c accessibility guidelines.
const double kMinimumReadableContrastRatio = 4.5f;

// Determines the contrast ratio of two colors.
GFX_EXPORT double GetContrastRatio(SkColor color_a, SkColor color_b);

GFX_EXPORT unsigned char GetLuminanceForColor(SkColor color);

// Calculated according to http://www.w3.org/TR/WCAG20/#relativeluminancedef
GFX_EXPORT double RelativeLuminance(SkColor color);

// Note: these transformations assume sRGB as the source color space
GFX_EXPORT void SkColorToHSL(SkColor c, HSL* hsl);
GFX_EXPORT SkColor HSLToSkColor(const HSL& hsl, SkAlpha alpha);

// Determines whether the given |hsl| falls within the given range for each
// component. All components of |hsl| are expected to be in the range [0, 1].
//
// If a component is negative in either |lower_bound| or |upper_bound|, that
// component will be ignored.
//
// For hue, the lower bound should be in the range [0, 1] and the upper bound
// should be in the range [(lower bound), (lower bound + 1)].
// For saturation and value, bounds should be specified in the range [0, 1],
// with the lower bound less than the upper bound.
GFX_EXPORT bool IsWithinHSLRange(const HSL& hsl,
    const HSL& lower_bound,
    const HSL& upper_bound);

// Makes |hsl| valid input for HSLShift(). Sets values of hue, saturation
// and luminosity which are outside of the valid range [0, 1] to -1.
// -1 is a special value which indicates 'no change'.
GFX_EXPORT void MakeHSLShiftValid(HSL* hsl);

// HSL-Shift an SkColor. The shift values are in the range of 0-1, with the
// option to specify -1 for 'no change'. The shift values are defined as:
// hsl_shift[0] (hue): The absolute hue value - 0 and 1 map
//    to 0 and 360 on the hue color wheel (red).
// hsl_shift[1] (saturation): A saturation shift, with the
//    following key values:
//    0 = remove all color.
//    0.5 = leave unchanged.
//    1 = fully saturate the image.
// hsl_shift[2] (lightness): A lightness shift, with the
//    following key values:
//    0 = remove all lightness (make all pixels black).
//    0.5 = leave unchanged.
//    1 = full lightness (make all pixels white).
GFX_EXPORT SkColor HSLShift(SkColor color, const HSL& shift);

// Builds a histogram based on the Y' of the Y'UV representation of
// this image.
GFX_EXPORT void BuildLumaHistogram(const SkBitmap& bitmap, int histogram[256]);

// Calculates how "boring" an image is. The boring score is the
// 0,1 ranged percentage of pixels that are the most common
// luma. Higher boring scores indicate that a higher percentage of a
// bitmap are all the same brightness.
GFX_EXPORT double CalculateBoringScore(const SkBitmap& bitmap);

// Returns a blend of the supplied colors, ranging from |background| (for
// |alpha| == 0) to |foreground| (for |alpha| == 255). The alpha channels of
// the supplied colors are also taken into account, so the returned color may
// be partially transparent.
GFX_EXPORT SkColor AlphaBlend(SkColor foreground, SkColor background,
    SkAlpha alpha);

// Returns true if the luminance of |color| is closer to black than white.
GFX_EXPORT bool IsDark(SkColor color);

// Makes a dark color lighter or a light color darker by blending |color| with
// white or black depending on its current luminance.  |alpha| controls the
// amount of white or black that will be alpha-blended into |color|.
GFX_EXPORT SkColor BlendTowardOppositeLuminance(SkColor color, SkAlpha alpha);

// Given an opaque foreground and background color, try to return a foreground
// color that is "readable" over the background color by luma-inverting the
// foreground color and then picking whichever foreground color has higher
// contrast against the background color.  You should not pass colors with
// non-255 alpha to this routine, since determining the correct behavior in such
// cases can be impossible.
//
// NOTE: This won't do anything but waste time if the supplied foreground color
// has a luma value close to the midpoint (0.5 in the HSL representation).
GFX_EXPORT SkColor GetReadableColor(SkColor foreground, SkColor background);

// Invert a color.
GFX_EXPORT SkColor InvertColor(SkColor color);

// Gets a Windows system color as a SkColor
GFX_EXPORT SkColor GetSysSkColor(int which);

// Returns true only if Chrome should use an inverted color scheme - which is
// only true if the system has high-contrast mode enabled and and is using a
// light-on-dark color scheme.
GFX_EXPORT bool IsInvertedColorScheme();

// Derives a color for icons on a UI surface based on the text color on the same
// surface.
GFX_EXPORT SkColor DeriveDefaultIconColor(SkColor text_color);

} // namespace color_utils

#endif // UI_GFX_COLOR_UTILS_H_
