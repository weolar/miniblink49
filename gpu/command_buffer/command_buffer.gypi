# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'target_defaults': {
    'variables': {
      'gles2_utils_target': 0,
    },
    'target_conditions': [
      # This part is shared between the targets defined below.
      ['gles2_utils_target==1', {
        'defines': [
          'GLES2_UTILS_IMPLEMENTATION',
        ],
        'sources': [
          'common/gles2_cmd_format.h',
          'common/gles2_cmd_utils.cc',
          'common/gles2_cmd_utils.h',
          'common/gles2_utils_export.h',
        ],
      }],
    ],
  },
}
