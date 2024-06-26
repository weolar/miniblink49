# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'gfx_unittests',
      'type': '<(gtest_target_type)',
      # iOS uses a small subset of ui. common_sources are the only files that
      # are built on iOS.
      'common_sources' : [
        'font_unittest.cc',
        'image/image_family_unittest.cc',
        'image/image_ios_unittest.mm',
        'image/image_skia_unittest.cc',
        'image/image_unittest.cc',
        'ios/NSString+CrStringDrawing_unittest.mm',
        'ios/uikit_util_unittest.mm',
        'screen_unittest.cc',
        'test/run_all_unittests.cc',
        'text_elider_unittest.cc',
        'text_utils_unittest.cc',
      ],
      'all_sources': [
        '<@(_common_sources)',
        'animation/animation_container_unittest.cc',
        'animation/animation_unittest.cc',
        'animation/multi_animation_unittest.cc',
        'animation/slide_animation_unittest.cc',
        'animation/tween_unittest.cc',
        'blit_unittest.cc',
        'break_list_unittest.cc',
        'canvas_unittest.cc',
        'canvas_unittest_mac.mm',
        'codec/jpeg_codec_unittest.cc',
        'codec/png_codec_unittest.cc',
        'color_analysis_unittest.cc',
        'color_profile_mac_unittest.mm',
        'color_utils_unittest.cc',
        'display_change_notifier_unittest.cc',
        'display_unittest.cc',
        'font_fallback_mac_unittest.cc',
        'font_list_unittest.cc',
        'font_render_params_linux_unittest.cc',
        'geometry/box_unittest.cc',
        'geometry/cubic_bezier_unittest.cc',
        'geometry/insets_unittest.cc',
        'geometry/matrix3_unittest.cc',
        'geometry/point3_unittest.cc',
        'geometry/point_unittest.cc',
        'geometry/quad_unittest.cc',
        'geometry/rect_unittest.cc',
        'geometry/safe_integer_conversions_unittest.cc',
        'geometry/scroll_offset_unittest.cc',
        'geometry/size_unittest.cc',
        'geometry/vector2d_unittest.cc',
        'geometry/vector3d_unittest.cc',
        'image/image_mac_unittest.mm',
        'image/image_util_unittest.cc',
        'mac/coordinate_conversion_unittest.mm',
        'nine_image_painter_unittest.cc',
        'path_mac_unittest.mm',
        'platform_font_linux_unittest.cc',
        'platform_font_mac_unittest.mm',
        'range/range_mac_unittest.mm',
        'range/range_unittest.cc',
        'range/range_win_unittest.cc',
        'render_text_unittest.cc',
        'screen_win_unittest.cc',
        'sequential_id_generator_unittest.cc',
        'shadow_value_unittest.cc',
        'skbitmap_operations_unittest.cc',
        'skrect_conversion_unittest.cc',
        'transform_util_unittest.cc',
        'utf16_indexing_unittest.cc',
      ],
      'dependencies': [
        '../../base/base.gyp:base',
        '../../base/base.gyp:test_support_base',
        '../../skia/skia.gyp:skia',
        '../../testing/gtest.gyp:gtest',
        '../../third_party/libpng/libpng.gyp:libpng',
        '../base/ui_base.gyp:ui_base',
        '../resources/ui_resources.gyp:ui_test_pak',
        'gfx.gyp:gfx',
        'gfx.gyp:gfx_geometry',
        'gfx.gyp:gfx_test_support',
      ],
      'conditions': [
        ['OS == "ios"', {
          'sources': ['<@(_common_sources)'],
        }, {  # OS != "ios"
          'sources': ['<@(_all_sources)'],
        }],
        ['OS != "mac" and OS != "ios"', {
          'sources': [
            'interpolated_transform_unittest.cc',
            'transform_unittest.cc',
          ],
        }],
        ['OS == "android"', {
          'dependencies': [
            '../../testing/android/native_test.gyp:native_test_native_code',
          ],
          # Do not run display_change_notifier_unittest.cc on Android because it
          # does not compile display_observer.cc
          'sources!': [
            'display_change_notifier_unittest.cc',
          ],
        }],
        ['OS=="android" or OS=="ios"', {
          'sources!': [
            'render_text_unittest.cc',
          ],
        }],
        ['chromeos==1', {
          'sources': [
            'chromeos/codec/jpeg_codec_robust_slow_unittest.cc',
          ],
        }],
        ['use_aura==1', {
          'sources!': [
            'screen_unittest.cc',
          ],
        },{
          'sources!': [
            'nine_image_painter_unittest.cc',
          ],
        }],
        ['OS == "win"', {
          'sources': [
            'color_profile_win_unittest.cc',
            'font_fallback_win_unittest.cc',
            'icon_util_unittest.cc',
            'icon_util_unittests.rc',
            'path_win_unittest.cc',
            'platform_font_win_unittest.cc',
          ],
          'msvs_settings': {
            'VCLinkerTool': {
              'DelayLoadDLLs': [
                'd2d1.dll',
                'd3d10_1.dll',
              ],
              'AdditionalDependencies': [
                'd2d1.lib',
                'd3d10_1.lib',
              ],
            },
          },
          'link_settings': {
            'libraries': [
              '-limm32.lib',
              '-loleacc.lib',
            ],
          },
          # TODO(jschuh): crbug.com/167187 fix size_t to int truncations.
          'msvs_disabled_warnings': [ 4267, ],
        }],
      ],
    }
  ],
  'conditions': [
    ['OS == "android"', {
      'targets': [
        {
          'target_name': 'gfx_unittests_apk',
          'type': 'none',
          'dependencies': [
            '../android/ui_android.gyp:ui_java',
            'gfx_unittests',
          ],
          'variables': {
            'test_suite_name': 'gfx_unittests',
          },
          'includes': [ '../../build/apk_test.gypi' ],
        },
      ],
    }],
    ['test_isolation_mode != "noop"', {
      'targets': [
        {
          'target_name': 'gfx_unittests_run',
          'type': 'none',
          'dependencies': [
            'gfx_unittests',
          ],
          'includes': [
            '../../build/isolate.gypi',
          ],
          'sources': [
            'gfx_unittests.isolate',
          ],
          'conditions': [
            ['use_x11 == 1', {
              'dependencies': [
                '../../tools/xdisplaycheck/xdisplaycheck.gyp:xdisplaycheck',
              ],
            }],
          ],
        },
      ],
    }],
  ],
}
