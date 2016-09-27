# Copyright (C) 2013 Google Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"""DevTools JSDoc validator presubmit script

See http://dev.chromium.org/developers/how-tos/depottools/presubmit-scripts
for more details about the presubmit API built into gcl.
"""

import sys


def _ValidateHashes(input_api, output_api):
    sys.path.append(input_api.PresubmitLocalPath())
    import build_jsdoc_validator_jar
    hashes_modified = build_jsdoc_validator_jar.hashes_modified()
    if not hashes_modified:
        return []

    results = '\n'.join(['%s (%s != %s)' % (name, expected, actual) for (name, expected, actual) in hashes_modified])
    return [output_api.PresubmitError('DevTools frontend JSDoc validator Java code, "%s" and "%s" must always be updated together. Please rebuild.\nModifications found:\n%s' %
            (build_jsdoc_validator_jar.jar_name, build_jsdoc_validator_jar.hashes_name, results))]


def CheckChangeOnUpload(input_api, output_api):
    return _ValidateHashes(input_api, output_api)


def CheckChangeOnCommit(input_api, output_api):
    return _ValidateHashes(input_api, output_api)
