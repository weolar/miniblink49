# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
    # These are defined here because we need to build this library both for
    # the general media pipeline and again for the untrusted NaCl target.
    'shared_memory_support_sources': [
      'audio/audio_parameters.cc',
      'audio/audio_parameters.h',
      'audio/point.cc',
      'audio/point.h',
      'base/audio_bus.cc',
      'base/audio_bus.h',
      'base/channel_layout.cc',
      'base/channel_layout.h',
      'base/limits.h',
      'base/media_export.h',
      'base/vector_math.cc',
      'base/vector_math.h',
    ],
  },
}
