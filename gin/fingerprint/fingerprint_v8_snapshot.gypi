# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# This file is meant to be included into a target to provide a rule that
# fingerprints the v8 snapshot and generates a .cc file which includes this
# fingerprint.
#
# To use this, create a gyp target with the following form:
#  {
#    'target_name': 'gin_v8_snapshot_fingerprint',
#    'type': 'none',
#    'variables': {
#      'snapshot_file': 'snapshot blob file to be fingerprinted',
#      'natives_file': 'natives blob file to be fingerprinted',
#      'output_file': 'output .cc file to generate with fingerprints',
#    },
#    'includes': [ '../gin/fingerprint/fingerprint_v8_snapshot.gypi' ],
#  },
#

{
  'conditions': [
    ['v8_use_external_startup_data==1', {
      'actions': [
        {
          'action_name': 'Generate V8 snapshot fingerprint',
          'message': 'Generating V8 snapshot fingerprint',
          'inputs': [
            '<(DEPTH)/gin/fingerprint/fingerprint_v8_snapshot.py',
            '<(snapshot_file)',
            '<(natives_file)',
          ],
          'outputs': [
            '<(output_file)',
          ],
          'action': [
            'python', '<(DEPTH)/gin/fingerprint/fingerprint_v8_snapshot.py',
            '--snapshot_file=<(snapshot_file)',
            '--natives_file=<(natives_file)',
            '--output_file=<(output_file)',
          ],
        }
      ],
    }],
  ],
}