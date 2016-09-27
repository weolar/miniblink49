# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
    # nacl_win64_target is for building the trusted Win64 NaCl broker.
    'nacl_win64_target': 0,
  },
  'includes': [
    '../build/common_untrusted.gypi',
    'gpu_common.gypi',
  ],
  'conditions': [
    ['disable_nacl==0 and disable_nacl_untrusted==0', {
      'targets': [
        {
          'target_name': 'gles2_implementation_nacl',
          'type': 'none',
          'variables': {
            'nacl_untrusted_build': 1,
            'nlib_target': 'libgles2_implementation_nacl.a',
            'build_glibc': 0,
            'build_newlib': 0,
            'build_irt': 1,
          },
          'defines': [
            'GLES2_IMPL_IMPLEMENTATION',
          ],
          'sources': [
            '<@(gles2_implementation_source_files)',
          ],
          'dependencies': [
            '../native_client/tools.gyp:prep_toolchain',
            '../base/base_nacl.gyp:base_nacl',
            '../third_party/khronos/khronos.gyp:khronos_headers',
            'command_buffer/command_buffer_nacl.gyp:gles2_utils_nacl',
            'gles2_cmd_helper_nacl',
          ],
        },
        {
          'target_name': 'command_buffer_common_nacl',
          'type': 'none',
          'variables': {
            'nacl_untrusted_build': 1,
            'nlib_target': 'libcommand_buffer_common_nacl.a',
            'build_glibc': 0,
            'build_newlib': 0,
            'build_irt': 1,
          },
          'includes': [
            'command_buffer_common.gypi',
          ],
          'dependencies': [
            '../native_client/tools.gyp:prep_toolchain',
            '../base/base_nacl.gyp:base_nacl',
            'command_buffer/command_buffer_nacl.gyp:gles2_utils_nacl',
          ],
        },
        {
          'target_name': 'gles2_cmd_helper_nacl',
          'type': 'none',
          'variables': {
            'nacl_untrusted_build': 1,
            'nlib_target': 'libgles2_cmd_helper_nacl.a',
            'build_glibc': 0,
            'build_newlib': 0,
            'build_irt': 1,
          },
          'includes': [
            'gles2_cmd_helper.gypi',
          ],
          'dependencies': [
            '../native_client/tools.gyp:prep_toolchain',
            '../base/base_nacl.gyp:base_nacl',
            'command_buffer_client_nacl',
          ],
        },
        {
          'target_name': 'command_buffer_client_nacl',
          'type': 'none',
          'variables': {
            'nacl_untrusted_build': 1,
            'nlib_target': 'libcommand_buffer_client_nacl.a',
            'build_glibc': 0,
            'build_newlib': 0,
            'build_irt': 1,
          },
          'includes': [
            'command_buffer_client.gypi',
          ],
          'dependencies': [
            '../native_client/tools.gyp:prep_toolchain',
            '../base/base_nacl.gyp:base_nacl',
            'command_buffer_common_nacl',
          ],
        },
        {
          'target_name': 'gpu_ipc_nacl',
          'type': 'none',
          'variables': {
            'nacl_untrusted_build': 1,
            'nlib_target': 'libgpu_ipc_nacl.a',
            'build_glibc': 0,
            'build_newlib': 0,
            'build_irt': 1,
          },
          'includes': [
            'gpu_ipc.gypi',
          ],
          'dependencies': [
            '../native_client/tools.gyp:prep_toolchain',
            '../base/base_nacl.gyp:base_nacl',
            'command_buffer_common_nacl',
          ],
        },
      ],
    }],
  ],
}
