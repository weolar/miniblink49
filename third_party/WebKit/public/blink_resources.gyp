# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'targets': [
    {
      'target_name': 'blink_resources',
      'type': 'none',
      'variables': {
        'grit_out_dir': '<(SHARED_INTERMEDIATE_DIR)/blink/public/resources',
      },
      'actions': [
        {
          'action_name': 'generate_blink_resources',
          'variables': {
            'grit_grd_file': 'blink_resources.grd',
          },
          'includes': [ '../../../build/grit_action.gypi' ],
        },
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '<(SHARED_INTERMEDIATE_DIR)',
        ],
      },
    },
    {
      'target_name': 'blink_image_resources',
      'type': 'none',
      'variables': {
        'grit_out_dir': '<(SHARED_INTERMEDIATE_DIR)/blink/public/resources',
      },
      'actions': [
        {
          'action_name': 'generate_blink_image_resources',
          'variables': {
            'grit_grd_file': 'blink_image_resources.grd',
          },
          'includes': [ '../../../build/grit_action.gypi' ],
        },
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '<(SHARED_INTERMEDIATE_DIR)',
        ],
      },
    },
  ],
}
