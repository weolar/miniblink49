# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'include_dirs': [
    '..',
  ],
  'all_dependent_settings': {
    'include_dirs': [
      '..',
    ],
  },
  'dependencies': [
    '../third_party/khronos/khronos.gyp:khronos_headers',
  ],
  'sources': [
    'command_buffer/client/cmd_buffer_helper.cc',
    'command_buffer/client/cmd_buffer_helper.h',
    'command_buffer/client/fenced_allocator.cc',
    'command_buffer/client/fenced_allocator.h',
    'command_buffer/client/gpu_control.h',
    'command_buffer/client/mapped_memory.cc',
    'command_buffer/client/mapped_memory.h',
    'command_buffer/client/ring_buffer.cc',
    'command_buffer/client/ring_buffer.h',
    'command_buffer/client/transfer_buffer.cc',
    'command_buffer/client/transfer_buffer.h',
  ],
}
