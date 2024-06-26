# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
    'conditions': [
      ['(OS=="linux" or OS=="freebsd" or OS=="solaris") and (embedded!=1 or (chromecast==1 and target_arch!="arm"))', {
        'use_alsa%': 1,
      }, {
        'use_alsa%': 0,
      }],
    ],
    # Source files that are used in production code for Android, and in tests
    # for others.
    'usb_midi_sources': [
      'midi_manager_usb.cc',
      'midi_manager_usb.h',
      'usb_midi_descriptor_parser.cc',
      'usb_midi_descriptor_parser.h',
      'usb_midi_device.h',
      'usb_midi_export.h',
      'usb_midi_input_stream.cc',
      'usb_midi_input_stream.h',
      'usb_midi_jack.h',
      'usb_midi_output_stream.cc',
      'usb_midi_output_stream.h',
    ],
  },
  'targets': [
    {
      # GN version: //media/midi
      'target_name': 'midi',
      'type': '<(component)',
      'dependencies': [
        '../../base/base.gyp:base',
      ],
      'defines': [
        'MIDI_IMPLEMENTATION',
      ],
      'include_dirs': [
        '../..',
      ],
      'sources': [
        'midi_export.h',
        'midi_manager.cc',
        'midi_manager.h',
        'midi_manager_android.cc',
        'midi_manager_android.h',
        'midi_manager_mac.cc',
        'midi_manager_mac.h',
        'midi_manager_win.cc',
        'midi_manager_win.h',
        'midi_message_queue.cc',
        'midi_message_queue.h',
        'midi_message_util.cc',
        'midi_message_util.h',
        'midi_port_info.cc',
        'midi_port_info.h',
        'midi_scheduler.cc',
        'midi_scheduler.h',
        'midi_switches.cc',
        'midi_switches.h',
        'usb_midi_device_android.cc',
        'usb_midi_device_android.h',
        'usb_midi_device_factory_android.cc',
        'usb_midi_device_factory_android.h',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '../..',
        ],
      },
      'conditions': [
        ['use_alsa==1 and use_udev==1', {
          'dependencies': [
            '../../crypto/crypto.gyp:crypto',
            '../../device/udev_linux/udev.gyp:udev_linux',
          ],
          'defines': [
            'USE_ALSA',
            'USE_UDEV',
          ],
          'sources': [
            'midi_manager_alsa.cc',
            'midi_manager_alsa.h',
          ],
          'link_settings': {
            'libraries': [
              '-lasound',
            ],
          },
        }],
        ['OS=="android"', {
          'dependencies': [
            '../../base/base.gyp:base_i18n',
            'midi_jni_headers',
            'midi_java',
          ],
          'sources': [
            '<@(usb_midi_sources)',
            'midi_device_android.cc',
            'midi_device_android.h',
            'midi_input_port_android.cc',
            'midi_input_port_android.h',
            'midi_jni_registrar.cc',
            'midi_jni_registrar.h',
            'midi_output_port_android.cc',
            'midi_output_port_android.h',
          ],
          'defines': [
            'EXPORT_USB_MIDI',
          ],
        }],
        ['OS=="mac"', {
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/CoreAudio.framework',
              '$(SDKROOT)/System/Library/Frameworks/CoreMIDI.framework',
            ],
          },
        }],
        ['OS=="win"', {
          'dependencies': [
            '../../device/usb/usb.gyp:device_usb',
          ],
        }],
      ],  # conditions
    },
    {
      # GN version: //media/midi:midi_unittests
      'target_name': 'midi_unittests',
      'type': '<(gtest_target_type)',
      'dependencies': [
        'midi',
        '../../base/base.gyp:base',
        '../../base/base.gyp:run_all_unittests',
      ],
      'include_dirs': [
        '../..',
      ],
      'sources': [
        'midi_manager_unittest.cc',
        'midi_manager_usb_unittest.cc',
        'midi_message_queue_unittest.cc',
        'midi_message_util_unittest.cc',
        'usb_midi_descriptor_parser_unittest.cc',
        'usb_midi_input_stream_unittest.cc',
        'usb_midi_output_stream_unittest.cc',
      ],
      'conditions': [
        ['use_alsa==1 and use_udev==1', {
          'defines': [
            'USE_ALSA',
            'USE_UDEV',
          ],
          'sources': [
            'midi_manager_alsa_unittest.cc',
          ],
        }],
        ['use_x11==1', {
          'dependencies': [
            '../../tools/xdisplaycheck/xdisplaycheck.gyp:xdisplaycheck',
          ],
        }],
        ['OS=="android"', {
          'dependencies': [
            '../../testing/android/native_test.gyp:native_test_native_code',
          ],
        }, {
          'sources': [
            '<@(usb_midi_sources)',
          ],
        }],
        ['OS=="mac"', {
          'sources': [
            'midi_manager_mac_unittest.cc',
          ],
        }],
      ],
    },
  ],
  'conditions': [
    ['test_isolation_mode != "noop"', {
      'targets': [
        {
          'target_name': 'midi_unittests_run',
          'type': 'none',
          'dependencies': [
            'midi_unittests',
          ],
          'includes': [
            '../../build/isolate.gypi',
          ],
          'sources': [
            'midi_unittests.isolate',
          ],
        },
      ],
    }],
  ],
}
