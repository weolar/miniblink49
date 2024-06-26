// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_SIMD_FILTER_YUV_H_
#define MEDIA_BASE_SIMD_FILTER_YUV_H_

#include "base/basictypes.h"
#include "media/base/media_export.h"

namespace media {

// These methods are exported for testing purposes only.  Library users should
// only call the methods listed in yuv_convert.h.

MEDIA_EXPORT void FilterYUVRows_C(uint8* ybuf,
    const uint8* y0_ptr,
    const uint8* y1_ptr,
    int source_width,
    uint8 source_y_fraction);

MEDIA_EXPORT void FilterYUVRows_SSE2(uint8* ybuf,
    const uint8* y0_ptr,
    const uint8* y1_ptr,
    int source_width,
    uint8 source_y_fraction);

} // namespace media

#endif // MEDIA_BASE_SIMD_FILTER_YUV_H_
