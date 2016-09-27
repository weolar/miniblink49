# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'dependencies': [
    '../ipc/ipc.gyp:ipc',
    '../third_party/khronos/khronos.gyp:khronos_headers',
  ],
  'include_dirs': [
    '..',
  ],
  'sources': [
    'ipc/gpu_command_buffer_traits.cc',
    'ipc/gpu_command_buffer_traits.h',
  ],
  'conditions': [
    # This section applies to gpu_ipc_win64, used by the NaCl Win64 helper
    # (nacl64.exe).
    ['nacl_win64_target==1', {
      # gpu_ipc_win64 must only link against the 64-bit ipc target.
      'dependencies!': [
        '../ipc/ipc.gyp:ipc',
      ],
    }],
  ],
}
