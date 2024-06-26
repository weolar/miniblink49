# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'conditions': [
    ['OS != "ios"', {
      'targets': [
        {
          # GN version: //gpu/blink
          'target_name': 'gpu_blink',
          'type': '<(component)',
          'variables': {
            'chromium_code': 1,
            'enable_wexit_time_destructors': 1,
          },
          'dependencies': [
            '<(DEPTH)/base/base.gyp:base',
            '<(DEPTH)/cc/cc.gyp:cc',
            '<(DEPTH)/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
            '<(DEPTH)/gpu/command_buffer/command_buffer.gyp:gles2_utils',
            '<(DEPTH)/gpu/gpu.gyp:command_buffer_service',
            '<(DEPTH)/gpu/gpu.gyp:gles2_c_lib',
            '<(DEPTH)/gpu/gpu.gyp:gles2_implementation',
            '<(DEPTH)/gpu/gpu.gyp:gl_in_process_context',
            '<(DEPTH)/gpu/skia_bindings/skia_bindings.gyp:gpu_skia_bindings',
            '<(DEPTH)/skia/skia.gyp:skia',
            '<(DEPTH)/third_party/WebKit/public/blink.gyp:blink_minimal',
            '<(angle_path)/src/angle.gyp:translator',
            '<(DEPTH)/ui/gl/gl.gyp:gl',
            '<(DEPTH)/ui/gfx/gfx.gyp:gfx',
            '<(DEPTH)/ui/gfx/gfx.gyp:gfx_geometry',
          ],
          'sources': [
            'gpu_blink_export.h',
            'webgraphicscontext3d_impl.cc',
            'webgraphicscontext3d_impl.h',
            'webgraphicscontext3d_in_process_command_buffer_impl.cc',
            'webgraphicscontext3d_in_process_command_buffer_impl.h',
          ],
          'defines': [
            'GPU_BLINK_IMPLEMENTATION',
          ],
          # TODO(jschuh): crbug.com/167187 fix size_t to int truncations.
          'msvs_disabled_warnings': [ 4267, ],
        },
      ],
    }],
  ],
}
