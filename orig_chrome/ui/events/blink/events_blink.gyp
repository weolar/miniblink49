# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      # GN version: //ui/events/blink
      'target_name': 'events_blink',
      'type': 'static_library',
      'dependencies': [
        '../../../cc/cc.gyp:cc',
        '../../../third_party/WebKit/public/blink_headers.gyp:blink_headers',
        '../../gfx/gfx.gyp:gfx_geometry',
        '../events.gyp:events',
        '../events.gyp:gesture_detection',
      ],
      'sources': [
        # Note: sources list duplicated in GN build.
        'blink_event_util.cc',
        'blink_event_util.h',
        'input_handler_proxy_client.h',
        'input_handler_proxy.cc',
        'input_handler_proxy.h',
        'input_scroll_elasticity_controller.cc',
        'input_scroll_elasticity_controller.h',
        'synchronous_input_handler_proxy.h',
      ],
    },
  ],
}