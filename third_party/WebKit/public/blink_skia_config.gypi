# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
{
  'variables': {

    # Place defines here that require significant Blink rebaselining, or that
    # are otherwise best removed in Blink and then rolled into Chromium.
    # Defines should be in single quotes and a comma must appear after every one.
    # DO NOT remove the define until you are ready to rebaseline, and
    # AFTER the flag has been removed from skia.gyp in Chromium.
    'blink_skia_defines': [
    ],
  },
}
