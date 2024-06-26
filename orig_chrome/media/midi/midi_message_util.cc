// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/midi/midi_message_util.h"
#include "base/logging.h"

namespace media {
namespace midi {

    size_t GetMidiMessageLength(uint8 status_byte)
    {
        if (status_byte < 0x80)
            return 0;
        if (0x80 <= status_byte && status_byte <= 0xbf)
            return 3;
        if (0xc0 <= status_byte && status_byte <= 0xdf)
            return 2;
        if (0xe0 <= status_byte && status_byte <= 0xef)
            return 3;

        switch (status_byte) {
        case 0xf0:
            return 0;
        case 0xf1:
            return 2;
        case 0xf2:
            return 3;
        case 0xf3:
            return 2;
        case 0xf4: // Reserved
        case 0xf5: // Reserved
            return 0;
        case 0xf6:
            return 1;
        case 0xf7:
            return 0;
        case 0xf8:
        case 0xf9:
        case 0xfa:
        case 0xfb:
        case 0xfc:
        case 0xfd:
        case 0xfe:
        case 0xff:
            return 1;
        default:
            NOTREACHED();
            return 0;
        }
    }

} // namespace midi
} // namespace media
