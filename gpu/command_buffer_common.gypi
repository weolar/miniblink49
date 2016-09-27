# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'dependencies': [
    '../third_party/khronos/khronos.gyp:khronos_headers',
  ],
  'sources': [
    'command_buffer/common/bitfield_helpers.h',
    'command_buffer/common/buffer.cc',
    'command_buffer/common/buffer.h',
    'command_buffer/common/capabilities.cc',
    'command_buffer/common/capabilities.h',
    'command_buffer/common/cmd_buffer_common.cc',
    'command_buffer/common/cmd_buffer_common.h',
    'command_buffer/common/command_buffer.h',
    'command_buffer/common/constants.h',
    'command_buffer/common/debug_marker_manager.cc',
    'command_buffer/common/debug_marker_manager.h',
    'command_buffer/common/gles2_cmd_format.cc',
    'command_buffer/common/gles2_cmd_format.h',
    'command_buffer/common/gles2_cmd_format_autogen.h',
    'command_buffer/common/gles2_cmd_ids.h',
    'command_buffer/common/gles2_cmd_ids_autogen.h',
    'command_buffer/common/id_allocator.cc',
    'command_buffer/common/id_allocator.h',
    'command_buffer/common/mailbox.cc',
    'command_buffer/common/mailbox.h',
    'command_buffer/common/mailbox_holder.cc',
    'command_buffer/common/mailbox_holder.h',
    'command_buffer/common/thread_local.h',
    'command_buffer/common/time.h',
  ],
}
