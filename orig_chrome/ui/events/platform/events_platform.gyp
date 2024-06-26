# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [{
    'target_name': 'events_platform',
    'type': '<(component)',
    'dependencies': [
      '../../../base/base.gyp:base',
    ],
    'defines': [
      'EVENTS_IMPLEMENTATION',
    ],
    'sources': [
      'platform_event_dispatcher.h',
      'platform_event_observer.h',
      'platform_event_source.cc',
      'platform_event_source.h',
      'platform_event_source_stub.cc',
      'platform_event_types.h',
      'scoped_event_dispatcher.cc',
      'scoped_event_dispatcher.h',
    ],
    'conditions': [
      ['use_x11==1', {
        'sources!': [
          'platform_event_source_stub.cc',
        ],
      }],
    ],
  }],
}
