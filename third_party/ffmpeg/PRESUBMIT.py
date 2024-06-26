# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Presubmit script for FFmpeg repository.

Does the following:
- Warns users that changes must be submitted via Gerrit.
- Warns users when a change is made without updating the README file.
"""

import re
import subprocess


def _WarnIfReadmeIsUnchanged(input_api, output_api):
  """Warn if the README file hasn't been updated with change notes."""
  readme_re = re.compile(r'.*[/\\]?chromium[/\\]patches[/\\]README$')
  for f in input_api.AffectedFiles():
    if readme_re.match(f.LocalPath()):
      return []

  return [output_api.PresubmitPromptWarning('\n'.join([
      'FFmpeg changes detected without any update to chromium/patches/README,',
      'it\'s good practice to update this file with a note about your changes.'
  ]))]


def CheckChangeOnUpload(input_api, output_api):
  results = []
  results.extend(_WarnIfReadmeIsUnchanged(input_api, output_api))
  return results
