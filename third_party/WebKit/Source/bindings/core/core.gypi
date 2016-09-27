# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'includes': [
    '../modules/v8/generated.gypi',  # FIXME: remove once core scripts generate qualified includes correctly: http://crbug.com/358074
    'v8/v8.gypi',  # FIXME: should be v8/v8.gypi: http://crbug.com/358074
    'v8/generated.gypi',
  ],

  'variables': {
    'bindings_core_output_dir': '<(SHARED_INTERMEDIATE_DIR)/blink/bindings/core',
  },
}
