# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# Common media variables.
{
  'variables': {
    'conditions': [
      ['OS == "android" or OS == "ios"', {
        # Android and iOS don't use FFmpeg, libvpx nor libwebm by default.
        # Set media_use_ffmpeg=1 for Android builds to compile experimental
        # support for FFmpeg and the desktop media pipeline.
        'media_use_ffmpeg%': 0,
        'media_use_libvpx%': 0,
        'media_use_libwebm%': 0,
      }, {
        'media_use_ffmpeg%': 1,
        'media_use_libvpx%': 1,
        'media_use_libwebm%': 1,
      }],
    ],
  },
}
