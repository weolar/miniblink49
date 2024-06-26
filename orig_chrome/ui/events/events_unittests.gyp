# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      # GN version: //ui/events:events_unittests
      'target_name': 'events_unittests',
      'type': '<(gtest_target_type)',
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/base/base.gyp:run_all_unittests',
        '<(DEPTH)/base/base.gyp:test_support_base',
        '<(DEPTH)/ipc/ipc.gyp:test_support_ipc',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(DEPTH)/testing/gtest.gyp:gtest',
        '<(DEPTH)/third_party/mesa/mesa.gyp:osmesa',
        '../gfx/gfx.gyp:gfx',
        '../gfx/gfx.gyp:gfx_geometry',
        '../gfx/gfx.gyp:gfx_test_support',
        'devices/events_devices.gyp:events_devices',
        'events.gyp:dom_keycode_converter',
        'events.gyp:events',
        'events.gyp:events_base',
        'events.gyp:events_ipc',
        'events.gyp:events_test_support',
        'events.gyp:gesture_detection',
        'events.gyp:gestures_blink',
        'platform/events_platform.gyp:events_platform',
      ],
      'sources': [
        # Note: sources list duplicated in GN build.
        'android/scroller_unittest.cc',
        'cocoa/events_mac_unittest.mm',
        'devices/x11/device_data_manager_x11_unittest.cc',
        'event_dispatcher_unittest.cc',
        'event_processor_unittest.cc',
        'event_rewriter_unittest.cc',
        'event_unittest.cc',
        'gesture_detection/bitset_32_unittest.cc',
        'gesture_detection/filtered_gesture_provider_unittest.cc',
        'gesture_detection/gesture_event_data_packet_unittest.cc',
        'gesture_detection/gesture_provider_unittest.cc',
        'gesture_detection/motion_event_buffer_unittest.cc',
        'gesture_detection/motion_event_generic_unittest.cc',
        'gesture_detection/snap_scroll_controller_unittest.cc',
        'gesture_detection/touch_disposition_gesture_filter_unittest.cc',
        'gesture_detection/velocity_tracker_unittest.cc',
        'gestures/blink/web_gesture_curve_impl_unittest.cc',
        'gestures/fling_curve_unittest.cc',
        'gestures/gesture_provider_aura_unittest.cc',
        'gestures/motion_event_aura_unittest.cc',
        'ipc/latency_info_param_traits_unittest.cc',
        'keycodes/dom/keycode_converter_unittest.cc',
        'keycodes/keyboard_code_conversion_unittest.cc',
        'keycodes/platform_key_map_win_unittest.cc',
        'latency_info_unittest.cc',
        'platform/platform_event_source_unittest.cc',
        'scoped_target_handler_unittest.cc',
        'x/events_x_unittest.cc',
      ],
      'include_dirs': [
        '../../testing/gmock/include',
      ],
      'conditions': [
        ['use_x11==1', {
          'dependencies': [
            '../../build/linux/system.gyp:x11',
            '../gfx/x/gfx_x11.gyp:gfx_x11',
	    'x/events_x.gyp:events_x',
          ],
        }],
        ['use_ozone==1', {
          'sources': [
            'ozone/chromeos/cursor_controller_unittest.cc',
            'ozone/evdev/event_converter_evdev_impl_unittest.cc',
            'ozone/evdev/event_converter_test_util.cc',
            'ozone/evdev/event_device_info_unittest.cc',
            'ozone/evdev/event_device_test_util.cc',
            'ozone/evdev/input_injector_evdev_unittest.cc',
            'ozone/evdev/tablet_event_converter_evdev_unittest.cc',
            'ozone/evdev/touch_event_converter_evdev_unittest.cc',
            'ozone/evdev/touch_noise/touch_noise_finder_unittest.cc',
          ],
          'dependencies': [
            'ozone/events_ozone.gyp:events_ozone',
            'ozone/events_ozone.gyp:events_ozone_evdev',
            'ozone/events_ozone.gyp:events_ozone_layout',
          ]
        }],
        ['use_xkbcommon==1', {
          'sources': [
            'ozone/layout/keyboard_layout_engine_unittest.cc',
            'ozone/layout/xkb/xkb_keyboard_layout_engine_unittest.cc',
          ]
        }],
        ['use_aura==0', {
          'sources!': [
            'gestures/gesture_provider_aura_unittest.cc',
            'gestures/motion_event_aura_unittest.cc',
          ],
        }],
        # Exclude tests that rely on event_utils.h for platforms that do not
        # provide native cracking, i.e., platforms that use events_stub.cc.
        ['OS!="win" and use_x11!=1 and use_ozone!=1', {
          'sources!': [
            'event_unittest.cc',
          ],
        }],
        ['OS == "android"', {
          'sources': [
            'android/motion_event_android_unittest.cc',
          ],
          'dependencies': [
            '../../testing/android/native_test.gyp:native_test_native_code',
          ],
        }],
        ['OS!="ios"', {
          'sources': [
            'blink/blink_event_util_unittest.cc',
            'blink/input_handler_proxy_unittest.cc',
            'blink/input_scroll_elasticity_controller_unittest.cc',
          ],
          'dependencies': [
            'blink/events_blink.gyp:events_blink',
          ],
        }],
      ],
    },
  ],
  'conditions': [
    ['OS == "android"', {
      'targets': [
        {
          'target_name': 'events_unittests_apk',
          'type': 'none',
          'dependencies': [
            'events_unittests',
          ],
          'variables': {
            'test_suite_name': 'events_unittests',
          },
          'includes': [ '../../build/apk_test.gypi' ],
        },
      ],
      'conditions': [
        ['test_isolation_mode != "noop"', {
          'targets': [
            {
              'target_name': 'events_unittests_apk_run',
              'type': 'none',
              'dependencies': [
                'events_unittests_apk',
              ],
              'includes': [
                '../../build/isolate.gypi',
              ],
              'sources': [
                'events_unittests_apk.isolate',
              ],
            },
          ],
        }],
      ],
    }],
    ['test_isolation_mode != "noop"', {
      'targets': [
        {
          'target_name': 'events_unittests_run',
          'type': 'none',
          'dependencies': [
            'events_unittests',
          ],
          'includes': [
            '../../build/isolate.gypi',
          ],
          'sources': [
            'events_unittests.isolate',
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
