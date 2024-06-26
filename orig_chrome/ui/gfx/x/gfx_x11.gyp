# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
    'use_x11': 1,  # It is necessary to explicitly set use_x11 here to make sure
                   # that the exclusion rules in filename_rules.gypi do not
                   # exclude the x11* files.
  },

  'targets': [
    {
      # GN version: //ui/gfx/x
      'target_name': 'gfx_x11',
      'type': '<(component)',
      'dependencies': [
        '../../../base/base.gyp:base',
        '../../../build/linux/system.gyp:x11',
      ],
      'defines': [
        'GFX_IMPLEMENTATION',
      ],
      'sources': [
        'x11_atom_cache.cc',
        'x11_atom_cache.h',
        'x11_connection.cc',
        'x11_connection.h',
        'x11_error_tracker.cc',
        'x11_error_tracker.h',
        'x11_switches.cc',
        'x11_switches.h',
        'x11_types.cc',
        'x11_types.h',
      ],
    },
  ]
}
