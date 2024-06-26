// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_GDI_UTIL_H_
#define UI_GFX_GDI_UTIL_H_

#include <vector>
#include <windows.h>

#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/gfx_export.h"
#include "ui/gfx/path.h"

namespace gfx {

// Creates a BITMAPINFOHEADER structure given the bitmap's size.
GFX_EXPORT void CreateBitmapHeader(int width, int height,
    BITMAPINFOHEADER* hdr);

// Creates a BITMAPV4HEADER structure given the bitmap's size.  You probably
// only need to use BMP V4 if you need transparency (alpha channel). This
// function sets the AlphaMask to 0xff000000.
GFX_EXPORT void CreateBitmapV4Header(int width, int height,
    BITMAPV4HEADER* hdr);

// Creates a monochrome bitmap header.
void CreateMonochromeBitmapHeader(int width, int height, BITMAPINFOHEADER* hdr);

// Modify the given hrgn by subtracting the given rectangles.
GFX_EXPORT void SubtractRectanglesFromRegion(
    HRGN hrgn,
    const std::vector<gfx::Rect>& cutouts);

GFX_EXPORT HRGN ConvertPathToHRGN(const gfx::Path& path);

// Calculate scale to fit an entire page on DC.
GFX_EXPORT float CalculatePageScale(HDC dc, int page_width, int page_height);

// Apply scaling to the DC.
GFX_EXPORT bool ScaleDC(HDC dc, float scale_factor);

GFX_EXPORT void StretchDIBits(HDC hdc,
    int dest_x, int dest_y, int dest_w, int dest_h,
    int src_x, int src_y, int src_w, int src_h,
    void* pixels, const BITMAPINFO* bitmap_info);

} // namespace gfx

#endif // UI_GFX_GDI_UTIL_H_
