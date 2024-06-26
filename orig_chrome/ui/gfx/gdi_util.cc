// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/gdi_util.h"

#include <stddef.h>

#include <algorithm>

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"

namespace {

void CreateBitmapHeaderWithColorDepth(LONG width,
    LONG height,
    WORD color_depth,
    BITMAPINFOHEADER* hdr)
{
    // These values are shared with gfx::PlatformDevice
    hdr->biSize = sizeof(BITMAPINFOHEADER);
    hdr->biWidth = width;
    hdr->biHeight = -height; // minus means top-down bitmap
    hdr->biPlanes = 1;
    hdr->biBitCount = color_depth;
    hdr->biCompression = BI_RGB; // no compression
    hdr->biSizeImage = 0;
    hdr->biXPelsPerMeter = 1;
    hdr->biYPelsPerMeter = 1;
    hdr->biClrUsed = 0;
    hdr->biClrImportant = 0;
}

} // namespace

namespace gfx {

void CreateBitmapHeader(int width, int height, BITMAPINFOHEADER* hdr)
{
    CreateBitmapHeaderWithColorDepth(width, height, 32, hdr);
}

void CreateBitmapV4Header(int width, int height, BITMAPV4HEADER* hdr)
{
    // Because bmp v4 header is just an extension, we just create a v3 header and
    // copy the bits over to the v4 header.
    BITMAPINFOHEADER header_v3;
    CreateBitmapHeader(width, height, &header_v3);
    memset(hdr, 0, sizeof(BITMAPV4HEADER));
    memcpy(hdr, &header_v3, sizeof(BITMAPINFOHEADER));

    // Correct the size of the header and fill in the mask values.
    hdr->bV4Size = sizeof(BITMAPV4HEADER);
    hdr->bV4RedMask = 0x00ff0000;
    hdr->bV4GreenMask = 0x0000ff00;
    hdr->bV4BlueMask = 0x000000ff;
    hdr->bV4AlphaMask = 0xff000000;
}

// Creates a monochrome bitmap header.
void CreateMonochromeBitmapHeader(int width,
    int height,
    BITMAPINFOHEADER* hdr)
{
    CreateBitmapHeaderWithColorDepth(width, height, 1, hdr);
}

void SubtractRectanglesFromRegion(HRGN hrgn,
    const std::vector<gfx::Rect>& cutouts)
{
    if (cutouts.size()) {
        HRGN cutout = ::CreateRectRgn(0, 0, 0, 0);
        for (size_t i = 0; i < cutouts.size(); i++) {
            ::SetRectRgn(cutout,
                cutouts[i].x(),
                cutouts[i].y(),
                cutouts[i].right(),
                cutouts[i].bottom());
            ::CombineRgn(hrgn, hrgn, cutout, RGN_DIFF);
        }
        ::DeleteObject(cutout);
    }
}

HRGN ConvertPathToHRGN(const gfx::Path& path)
{
    int point_count = path.getPoints(NULL, 0);
    scoped_ptr<SkPoint[]> points(new SkPoint[point_count]);
    path.getPoints(points.get(), point_count);
    scoped_ptr<POINT[]> windows_points(new POINT[point_count]);
    for (int i = 0; i < point_count; ++i) {
        windows_points[i].x = SkScalarRoundToInt(points[i].fX);
        windows_points[i].y = SkScalarRoundToInt(points[i].fY);
    }

    return ::CreatePolygonRgn(windows_points.get(), point_count, ALTERNATE);
}

float CalculatePageScale(HDC dc, int page_width, int page_height)
{
    int dc_width = GetDeviceCaps(dc, HORZRES);
    int dc_height = GetDeviceCaps(dc, VERTRES);

    // If page fits DC - no scaling needed.
    if (dc_width >= page_width && dc_height >= page_height)
        return 1.0;

    float x_factor = static_cast<float>(dc_width) / static_cast<float>(page_width);
    float y_factor = static_cast<float>(dc_height) / static_cast<float>(page_height);
    return std::min(x_factor, y_factor);
}

// Apply scaling to the DC.
bool ScaleDC(HDC dc, float scale_factor)
{
    SetGraphicsMode(dc, GM_ADVANCED);
    XFORM xform = { 0 };
    xform.eM11 = xform.eM22 = scale_factor;
    return !!ModifyWorldTransform(dc, &xform, MWT_LEFTMULTIPLY);
}

void StretchDIBits(HDC hdc, int dest_x, int dest_y, int dest_w, int dest_h,
    int src_x, int src_y, int src_w, int src_h, void* pixels,
    const BITMAPINFO* bitmap_info)
{
    // When blitting a rectangle that touches the bottom, left corner of the
    // bitmap, StretchDIBits looks at it top-down!  For more details, see
    // http://wiki.allegro.cc/index.php?title=StretchDIBits.
    int rv;
    int bitmap_h = -bitmap_info->bmiHeader.biHeight;
    int bottom_up_src_y = bitmap_h - src_y - src_h;
    if (bottom_up_src_y == 0 && src_x == 0 && src_h != bitmap_h) {
        rv = ::StretchDIBits(hdc,
            dest_x, dest_h + dest_y - 1, dest_w, -dest_h,
            src_x, bitmap_h - src_y + 1, src_w, -src_h,
            pixels, bitmap_info, DIB_RGB_COLORS, SRCCOPY);
    } else {
        rv = ::StretchDIBits(hdc,
            dest_x, dest_y, dest_w, dest_h,
            src_x, bottom_up_src_y, src_w, src_h,
            pixels, bitmap_info, DIB_RGB_COLORS, SRCCOPY);
    }
    DCHECK(rv != static_cast<int>(GDI_ERROR));
}

} // namespace gfx
