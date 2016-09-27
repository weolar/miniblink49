# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'includes': [
    'v8/generated.gypi',
    'v8/v8.gypi',
  ],

  'variables': {
    'bindings_modules_output_dir': '<(SHARED_INTERMEDIATE_DIR)/blink/bindings/modules',
  },
}
