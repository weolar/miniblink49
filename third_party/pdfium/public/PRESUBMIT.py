# Copyright 2017 The PDFium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Presubmit script for pdfium.

See http://dev.chromium.org/developers/how-tos/depottools/presubmit-scripts
for more details about the presubmit API built into depot_tools.
"""

def _CheckPublicHeaders(input_api, output_api):
  """Checks that the public headers match the API tests."""
  src_path = input_api.os_path.dirname(input_api.PresubmitLocalPath())
  check_script = input_api.os_path.join(
      src_path, 'testing' , 'tools' , 'api_check.py')
  try:
    input_api.subprocess.check_output(check_script)
    return []
  except input_api.subprocess.CalledProcessError as error:
    return [output_api.PresubmitError('api_check.py failed:',
                                      long_text=error.output)]


def CheckChangeOnUpload(input_api, output_api):
  results = []
  results += _CheckPublicHeaders(input_api, output_api)
  return results


def CheckChangeOnCommit(input_api, output_api):
  results = []
  results += _CheckPublicHeaders(input_api, output_api)
  return results
