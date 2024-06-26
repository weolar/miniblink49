// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_IMAGE_IMAGE_UTIL_H_
#define UI_GFX_IMAGE_IMAGE_UTIL_H_

#include <stddef.h>

#include <vector>

#include "ui/gfx/gfx_export.h"

namespace gfx {
class Image;
class ImageSkia;
}

namespace gfx {

// Creates an image from the given JPEG-encoded input. If there was an error
// creating the image, returns an IsEmpty() Image.
GFX_EXPORT Image ImageFrom1xJPEGEncodedData(const unsigned char* input,
    size_t input_size);

// Fills the |dst| vector with JPEG-encoded bytes of the 1x representation of
// the given image.
// Returns true if the image has a 1x representation and the 1x representation
// was encoded successfully.
// |quality| determines the compression level, 0 == lowest, 100 == highest.
// Returns true if the Image was encoded successfully.
GFX_EXPORT bool JPEG1xEncodedDataFromImage(const Image& image,
    int quality,
    std::vector<unsigned char>* dst);

// Returns the visible (non-transparent) width of the 1x rep of the given
// image. If the image has no transparency, the leading value will be 0 and
// the trailing will be the image width. Return values are in the 1x width
// pixel units. Margins are given in 0-based column format. So if the image
// has only transparent pixels in columns 0, 1, 2, 3, then the leading value
// will be 4. Similarly, if there are all transparent pixels in column
// width-2, width-1, then the trailing margin value will be width-3.
// Returns true if the value is computed from opacity, false if it is a
// default value because of null image, missing Rep, etc.
// This method is only suitable for fairly small images (i.e. 16x16).
// The margins for a completely transparent image will be w/2-1, w/2, but this
// will be an expensive operation: it isn't expected that it will be frequently
// calculated.
GFX_EXPORT bool VisibleMargins(const ImageSkia& image,
    int* leading, int* trailing);

} // namespace gfx

#endif // UI_GFX_IMAGE_IMAGE_UTIL_H_
