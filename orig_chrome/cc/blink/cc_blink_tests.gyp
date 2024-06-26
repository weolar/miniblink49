# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'targets': [
    {
      # GN version: //cc/blink/cc_blink_unittests
      'target_name': 'cc_blink_unittests',
      'type': '<(gtest_target_type)',
      'dependencies': [
        '../../base/base.gyp:test_support_base',
        '../../base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
        '../../skia/skia.gyp:skia',
        '../../testing/gtest.gyp:gtest',
        '../../ui/gfx/gfx.gyp:gfx_geometry',
        '../../ui/gfx/gfx.gyp:gfx_test_support',
        '../cc.gyp:cc',
        '../cc_tests.gyp:cc_test_support',
        'cc_blink.gyp:cc_blink',
      ],
      'sources': [
        'test/cc_blink_test_suite.cc',
        'test/run_all_unittests.cc',
        'web_animation_unittest.cc',
        'web_float_animation_curve_unittest.cc',
        'web_layer_impl_fixed_bounds_unittest.cc',
      ],
    }
  ],
  'conditions': [
    ['OS == "android"',
      {
        'targets': [
          {
            'target_name': 'cc_blink_unittests_apk',
            'type': 'none',
            'dependencies': [
              'cc_blink_unittests',
            ],
            'variables': {
              'test_suite_name': 'cc_blink_unittests',
            },
            'includes': [ '../../build/apk_test.gypi' ],
          },
        ],
      }
    ]
  ],

}
