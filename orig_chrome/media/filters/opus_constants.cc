// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/opus_constants.h"
#include <stdint.h>

namespace media {

const uint8_t kDefaultOpusChannelLayout[OPUS_MAX_CHANNELS_WITH_DEFAULT_LAYOUT] = { 0, 1 };

const uint8_t kFFmpegChannelDecodingLayouts
    [OPUS_MAX_VORBIS_CHANNELS][OPUS_MAX_VORBIS_CHANNELS]
    = {
          { 0 },

          // Stereo: No reorder.
          { 0, 1 },

          // 3 Channels, from Vorbis order to:
          //  L, R, Center
          { 0, 2, 1 },

          // 4 Channels: No reorder.
          { 0, 1, 2, 3 },

          // 5 Channels, from Vorbis order to:
          //  Front L, Front R, Center, Back L, Back R
          { 0, 2, 1, 3, 4 },

          // 6 Channels (5.1), from Vorbis order to:
          //  Front L, Front R, Center, LFE, Back L, Back R
          { 0, 2, 1, 5, 3, 4 },

          // 7 Channels (6.1), from Vorbis order to:
          //  Front L, Front R, Front Center, LFE, Side L, Side R, Back Center
          { 0, 2, 1, 6, 3, 4, 5 },

          // 8 Channels (7.1), from Vorbis order to:
          //  Front L, Front R, Center, LFE, Back L, Back R, Side L, Side R
          { 0, 2, 1, 7, 5, 6, 3, 4 },
      };

const uint8_t
    kOpusVorbisChannelMap[OPUS_MAX_VORBIS_CHANNELS][OPUS_MAX_VORBIS_CHANNELS]
    = {
          { 0 },
          { 0, 1 },
          { 0, 2, 1 },
          { 0, 1, 2, 3 },
          { 0, 4, 1, 2, 3 },
          { 0, 4, 1, 2, 3, 5 },
          { 0, 4, 1, 2, 3, 5, 6 },
          { 0, 6, 1, 2, 3, 4, 5, 7 },
      };

} // namespace media
