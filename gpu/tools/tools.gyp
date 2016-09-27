# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
  ],
  'conditions': [
    ['OS == "linux" and target_arch != "arm" and use_x11==1', {
      'targets': [
        {
          'target_name': 'compositor_model_bench',
          'type': 'executable',
          'dependencies': [
            '../../base/base.gyp:base',
            '../../build/linux/system.gyp:x11',
            '../../ui/gl/gl.gyp:gl',
          ],
          'libraries': [
            '-lGL',
            '-ldl',
          ],
          'sources': [
            'compositor_model_bench/compositor_model_bench.cc',
            'compositor_model_bench/render_tree.cc',
            'compositor_model_bench/shaders.cc',
            'compositor_model_bench/render_models.cc',
            'compositor_model_bench/render_model_utils.cc',
            'compositor_model_bench/forward_render_model.cc',
          ],
        },
      ],
    }],
  ],
}
