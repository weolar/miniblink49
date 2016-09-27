# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
    'includes': [
        'core/v8/v8.gypi',
        'modules/v8/v8.gypi',
    ],
    'variables': {
        'bindings_dir': '.',
        'bindings_output_dir': '<(SHARED_INTERMEDIATE_DIR)/blink/bindings/',
        'bindings_unittest_files': [
            '<@(bindings_core_v8_unittest_files)',
            '<@(bindings_modules_v8_unittest_files)',
        ],
    },
}
