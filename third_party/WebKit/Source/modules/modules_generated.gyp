# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
{
  'includes': [
    'modules_generated.gypi',
    '../bindings/scripts/scripts.gypi',
    '../build/features.gypi',
    '../build/scripts/scripts.gypi',
  ],
  'targets': [
    {
      # GN version: //third_party/WebKit/Source/modules:make_modules_generated
      'target_name': 'make_modules_generated',
      'type': 'none',
      'hard_dependency': 1,
      'dependencies': [
        '../bindings/modules/generated.gyp:modules_event_generated',
        '../config.gyp:config',
      ],
      'actions': [
        {
          'action_name': 'IndexedDBNames',
          'inputs': [
            '<@(make_names_files)',
            'indexeddb/IndexedDBNames.in',
          ],
          'outputs': [
            '<(blink_modules_output_dir)/IndexedDBNames.cpp',
            '<(blink_modules_output_dir)/IndexedDBNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_names.py',
            'indexeddb/IndexedDBNames.in',
            '--output_dir',
            '<(blink_modules_output_dir)',
          ],
        },
      ],
    },
  ],
}
