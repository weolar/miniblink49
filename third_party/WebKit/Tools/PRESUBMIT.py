# Copyright (c) 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Top-level presubmit script for bisect/perf trybot.

See http://dev.chromium.org/developers/how-tos/depottools/presubmit-scripts for
details on the presubmit API built into gcl.
"""

import imp
import os


def _ExamineConfigFiles(input_api):
    for f in input_api.AffectedFiles():
        if (not f.LocalPath().endswith('run-perf-test.cfg')):
            continue

        try:
            cfg_file = imp.load_source('config', os.path.basename(f.LocalPath()))

            for k, v in cfg_file.config.iteritems():
                if v:
                    return f.LocalPath()
        except (IOError, AttributeError, TypeError):
            return f.LocalPath()

    return None


def _CheckNoChangesToBisectConfigFile(input_api, output_api):
    results = _ExamineConfigFiles(input_api)
    if results:
        return [output_api.PresubmitError(
            'The perf try config file should only contain a config dict with '
            'empty fields. Changes to this file should never be submitted.',
            items=[results])]

    return []


def CommonChecks(input_api, output_api):
    results = []
    results.extend(_CheckNoChangesToBisectConfigFile(input_api, output_api))
    return results


def CheckChangeOnUpload(input_api, output_api):
    return CommonChecks(input_api, output_api)


def CheckChangeOnCommit(input_api, output_api):
    return CommonChecks(input_api, output_api)
