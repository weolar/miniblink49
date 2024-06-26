# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Chromium presubmit script for src/ui/events

See http://dev.chromium.org/developers/how-tos/depottools/presubmit-scripts
for more details on the presubmit API built into depot_tools.
"""

def GetPreferredTryMasters(project, change):
  return {
    'tryserver.chromium.linux': {
      'linux_chromium_chromeos_rel_ng': set(['defaulttests']),
      'linux_chromium_chromeos_asan_rel_ng': set(['defaulttests']),
    },
    'tryserver.chromium.win': {
      'win_chromium_compile_dbg_ng': set(['defaulttests']),
    }
  }
