// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_CHROMEOS_CODEC_JPEG_CODEC_ROBUST_SLOW_H_
#define UI_GFX_CHROMEOS_CODEC_JPEG_CODEC_ROBUST_SLOW_H_

#include <stddef.h>
#include <vector>

#include "ui/gfx/gfx_export.h"

class SkBitmap;

namespace gfx {

// Interface for encoding/decoding JPEG data. This is a wrapper around libjpeg,
// which has an inconvenient interface for callers. This is only used for
// servicing ChromeUtilityMsg_RobustJPEGDecodeImage and is currently unique
// to Chrome OS.
class GFX_EXPORT JPEGCodecRobustSlow {
public:
    enum ColorFormat {
        // 3 bytes per pixel (packed), in RGB order regardless of endianness.
        // This is the native JPEG format.
        FORMAT_RGB,

        // 4 bytes per pixel, in RGBA order in mem regardless of endianness.
        FORMAT_RGBA,

        // 4 bytes per pixel, in BGRA order in mem regardless of endianness.
        // This is the default Windows DIB order.
        FORMAT_BGRA,

        // 4 bytes per pixel, it can be either RGBA or BGRA. It depends on the bit
        // order in kARGB_8888_Config skia bitmap.
        FORMAT_SkBitmap
    };

    // Decodes the JPEG data contained in input of length input_size. The
    // decoded data will be placed in *output with the dimensions in *w and *h
    // on success (returns true). This data will be written in the'format'
    // format. On failure, the values of these output variables is undefined.
    static bool Decode(const unsigned char* input, size_t input_size,
        ColorFormat format, std::vector<unsigned char>* output,
        int* w, int* h);

    // Decodes the JPEG data contained in input of length input_size. If
    // successful, a SkBitmap is created and returned. It is up to the caller
    // to delete the returned bitmap.
    static SkBitmap* Decode(const unsigned char* input, size_t input_size);
};

} // namespace gfx

#endif // UI_GFX_CHROMEOS_CODEC_JPEG_CODEC_ROBUST_SLOW_H_
