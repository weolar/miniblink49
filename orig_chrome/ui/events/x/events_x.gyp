# Copyright 2016 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      # GN version: //ui/events/x
      'target_name': 'events_x',
      'type': '<(component)',
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/skia/skia.gyp:skia',
        '../../events/devices/events_devices.gyp:events_devices',
        '../../events/events.gyp:events_base',
        '../../gfx/x/gfx_x11.gyp:gfx_x11',
      ],
      'defines': [
        'EVENTS_X_IMPLEMENTATION',
      ],
      'sources': [
        'events_x_export.h',
        'events_x_utils.cc',
        'events_x_utils.h',
      ],
      'conditions': [
        ['use_x11==1', {
          'dependencies': [
            '<(DEPTH)/build/linux/system.gyp:x11',
          ],
        }],
      ],
    },
  ],
}
