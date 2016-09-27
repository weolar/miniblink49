# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'includes': [
    '../../build/common_untrusted.gypi',
    'command_buffer.gypi',
  ],
  'conditions': [
    ['disable_nacl==0 and disable_nacl_untrusted==0', {
      'targets': [
        {
          'target_name': 'gles2_utils_nacl',
          'type': 'none',
          'variables': {
            'gles2_utils_target': 1,
            'nacl_untrusted_build': 1,
            'nlib_target': 'libgles2_utils_nacl.a',
            'build_glibc': 0,
            'build_newlib': 0,
            'build_irt': 1,
          },
          'dependencies': [
            '../../native_client/tools.gyp:prep_toolchain',
            '../../base/base_nacl.gyp:base_nacl',
            '../../third_party/khronos/khronos.gyp:khronos_headers',
          ],
        },
      ],
    }],
  ],
}
