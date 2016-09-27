# Copyright (c) 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Top-level presubmit script for gpu/.

See http://dev.chromium.org/developers/how-tos/depottools/presubmit-scripts
for more details about the presubmit API built into depot_tools.
"""

def GetPreferredTryMasters(project, change):
  return {
    'tryserver.chromium.gpu': {
      'linux_gpu': set(['defaulttests']),
      'mac_gpu': set(['defaulttests']),
      'win_gpu': set(['defaulttests']),
    }
  }
