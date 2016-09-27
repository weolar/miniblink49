# Copyright (c) 2009 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'targets': [
    {
      'target_name': 'npapi',
      'type': 'none',
      'direct_dependent_settings': {
        'include_dirs': [
          # Some things #include "bindings/npapi.h" and others just #include
          # "npapi.h".  Account for both flavors.
          '.',
          'bindings',
        ],
      },
      # Even though these are just headers and aren't compiled, adding them to
      # the project makes it possible to open them in various IDEs.
      'sources': [
        'bindings/npapi.h',
        'bindings/npapi_extensions.h',
        'bindings/npfunctions.h',
        'bindings/nphostapi.h',
        'bindings/nptypes.h',
        'bindings/npruntime.h',
      ],
      'conditions': [
        ['use_x11 == 1', {
          'sources': [
            'bindings/npapi_x11.h',
          ],
        }],
      ],
    },
  ],
}
