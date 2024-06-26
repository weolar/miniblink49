# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      # GN version: //gpu/skia_bindings
      'target_name': 'gpu_skia_bindings',
      'type': 'static_library',
      'dependencies': [
        '../gpu.gyp:gles2_c_lib',
        '../../skia/skia.gyp:skia',
      ],
      'sources': [
        'gl_bindings_skia_cmd_buffer.cc',
        'gl_bindings_skia_cmd_buffer.h',
      ],
    }
  ]
}
