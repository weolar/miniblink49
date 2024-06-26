# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'events_devices',
      'type': '<(component)',
      'dependencies': [
        '../../../base/base.gyp:base',
        '../../../base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
        '../../../skia/skia.gyp:skia',
        '../../../ui/gfx/gfx.gyp:gfx',
        '../../../ui/gfx/gfx.gyp:gfx_geometry',
      ],
      'defines': [
        'EVENTS_DEVICES_IMPLEMENTATION',
      ],
      'sources': [
        'device_data_manager.cc',
        'device_data_manager.h',
        'device_hotplug_event_observer.h',
        'device_util_linux.cc',
        'device_util_linux.h',
        'events_devices_export.h',
        'input_device.cc',
        'input_device.h',
        'input_device_event_observer.h',
        'keyboard_device.cc',
        'keyboard_device.h',
        'touchscreen_device.cc',
        'touchscreen_device.h',
        'x11/device_data_manager_x11.cc',
        'x11/device_data_manager_x11.h',
        'x11/device_list_cache_x11.cc',
        'x11/device_list_cache_x11.h',
        'x11/touch_factory_x11.cc',
        'x11/touch_factory_x11.h',
      ],
      'export_dependent_settings': [
        '../../../ui/gfx/gfx.gyp:gfx',
      ],
      'conditions': [
        ['use_x11==1', {
          'dependencies': [
            '../../../build/linux/system.gyp:x11',
            '../../../ui/events/events.gyp:events_base',
            '../../../ui/gfx/x/gfx_x11.gyp:gfx_x11',
          ],
        }],
      ],
    },
  ],
}
