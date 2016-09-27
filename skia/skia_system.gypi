# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.


# This gypi file contains the shim header generation and other settings to use
# the system version of skia on Android.
{
  'direct_dependent_settings': {
    # This makes the Android build system set the include path appropriately.
    'libraries': [ '-lskia' ],
    # Some Chrome code uses non-public header files (http://crbug.com/274425),
    # so we need to add this include path for now to make it build. The system
    # version of skia is already required to be the same as the chromium version
    # so using the bundled headers shouldn't break anything.
    'include_dirs': [
      '../third_party/skia/src/core',
    ],
  },
  'link_settings': {
    # This actually causes the final binary to be linked against skia.
    'libraries': [ '-lskia' ],
  },
  'variables': {
    'headers_root_path': '../third_party/skia/include',
  },
  'includes': [
    '../third_party/skia/gyp/public_headers.gypi',
    '../build/shim_headers.gypi',
  ],
}
