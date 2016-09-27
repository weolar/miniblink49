# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
    # These are defined here because we need to build this library twice. Once
    # with extra parameter checking. Once with no parameter checking to be 100%
    # OpenGL ES 2.0 compliant for the conformance tests.
    'gles2_c_lib_source_files': [
      'command_buffer/client/gles2_c_lib.cc',
      'command_buffer/client/gles2_c_lib_autogen.h',
      'command_buffer/client/gles2_c_lib_export.h',
      'command_buffer/client/gles2_lib.h',
      'command_buffer/client/gles2_lib.cc',
    ],
    # These are defined here because we need to build this library twice. Once
    # with without support for client side arrays and once with for pepper and
    # the OpenGL ES 2.0 compliant for the conformance tests.
    'gles2_implementation_source_files': [
      'command_buffer/client/buffer_tracker.cc',
      'command_buffer/client/buffer_tracker.h',
      'command_buffer/client/client_context_state.h',
      'command_buffer/client/client_context_state.cc',
      'command_buffer/client/client_context_state_autogen.h',
      'command_buffer/client/client_context_state_impl_autogen.h',
      'command_buffer/client/gles2_impl_export.h',
      'command_buffer/client/gles2_implementation_autogen.h',
      'command_buffer/client/gles2_implementation.cc',
      'command_buffer/client/gles2_implementation.h',
      'command_buffer/client/gles2_implementation_impl_autogen.h',
      'command_buffer/client/gles2_interface.h',
      'command_buffer/client/gles2_trace_implementation_autogen.h',
      'command_buffer/client/gles2_trace_implementation.cc',
      'command_buffer/client/gles2_trace_implementation.h',
      'command_buffer/client/gles2_trace_implementation_impl_autogen.h',
      'command_buffer/client/gpu_memory_buffer_factory.h',
      'command_buffer/client/gpu_memory_buffer_tracker.cc',
      'command_buffer/client/gpu_memory_buffer_tracker.h',
      'command_buffer/client/program_info_manager.cc',
      'command_buffer/client/program_info_manager.h',
      'command_buffer/client/query_tracker.cc',
      'command_buffer/client/query_tracker.h',
      'command_buffer/client/share_group.cc',
      'command_buffer/client/share_group.h',
      'command_buffer/client/vertex_array_object_manager.cc',
      'command_buffer/client/vertex_array_object_manager.h',
    ]
  },
}
