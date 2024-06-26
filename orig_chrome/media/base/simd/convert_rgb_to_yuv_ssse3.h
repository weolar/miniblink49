// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_SIMD_CONVERT_RGB_TO_YUV_SSSE3_H_
#define MEDIA_BASE_SIMD_CONVERT_RGB_TO_YUV_SSSE3_H_

#ifdef __cplusplus
extern "C" {
#endif

// The header file for ASM functions that convert a row of RGB pixels with SSSE3
// instructions so we can call them from C++ code. These functions are
// implemented in "convert_rgb_to_yuv_ssse3.asm".

// We use ptrdiff_t instead of int for yasm routine parameters to portably
// sign-extend int. On Win64, MSVC does not sign-extend the value in the stack
// home of int function parameters, and yasm routines are unaware of this lack
// of extension and fault.  ptrdiff_t is portably sign-extended and fixes this
// issue on at least Win64.

// Convert a row of 24-bit RGB pixels to YV12 pixels.
void ConvertRGBToYUVRow_SSSE3(const uint8* rgb,
    uint8* y,
    uint8* u,
    uint8* v,
    ptrdiff_t width);

// Convert a row of 32-bit RGB pixels to YV12 pixels.
void ConvertARGBToYUVRow_SSSE3(const uint8* argb,
    uint8* y,
    uint8* u,
    uint8* v,
    ptrdiff_t width);

#ifdef __cplusplus
}
#endif

#endif // MEDIA_BASE_SIMD_CONVERT_RGB_TO_YUV_SSSE3_H_
