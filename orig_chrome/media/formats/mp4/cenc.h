// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FORMATS_MP4_CENC_H_
#define MEDIA_FORMATS_MP4_CENC_H_

#include <vector>

#include "base/basictypes.h"
#include "media/base/decrypt_config.h"

namespace media {
namespace mp4 {

    class BufferReader;

    struct FrameCENCInfo {
        uint8 iv[16];
        std::vector<SubsampleEntry> subsamples;

        FrameCENCInfo();
        ~FrameCENCInfo();
        bool Parse(int iv_size, BufferReader* r) WARN_UNUSED_RESULT;
        bool GetTotalSizeOfSubsamples(size_t* total_size) const WARN_UNUSED_RESULT;
    };

} // namespace mp4
} // namespace media

#endif // MEDIA_FORMATS_MP4_CENC_H_
