// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_SENDER_VP8_QUANTIZER_PARSER_H_
#define MEDIA_CAST_SENDER_VP8_QUANTIZER_PARSER_H_

#include "media/cast/cast_config.h"

namespace media {
namespace cast {

    // Partially parse / skip data in the header and the first partition,
    // and return the base quantizer in the range [0,63], or -1 on parse error.
    int ParseVp8HeaderQuantizer(const uint8* data, size_t size);

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_SENDER_VP8_QUANTIZER_PARSER_H_
