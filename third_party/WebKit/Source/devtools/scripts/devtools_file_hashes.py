#!/usr/bin/env python
# Copyright (c) 2014 Google Inc. All rights reserved.
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

import hashlib
import os.path

try:
    import json
except ImportError:
    import simplejson as json


def save_hashes(hashes_file_path, hashes):
    try:
        with open(hashes_file_path, "wt") as hashes_file:
            json.dump(hashes, hashes_file, indent=4, separators=(",", ": "))
    except:
        print "ERROR: Failed to write %s" % hashes_file_path
        raise


def load_hashes(hashes_file_path):
    try:
        with open(hashes_file_path, "r") as hashes_file:
            hashes = json.load(hashes_file)
    except:
        return {}
    return hashes


def calculate_file_hash(file_path):
    with open(file_path) as file:
        data = file.read()
        md5_hash = hashlib.md5(data).hexdigest()
    return md5_hash


def files_with_invalid_hashes(hash_file_path, file_paths):
    hashes = load_hashes(hash_file_path)
    result = []
    for file_path in file_paths:
        file_name = os.path.basename(file_path)
        if calculate_file_hash(file_path) != hashes.get(file_name, ""):
            result.append(file_path)
    return result


def update_file_hashes(hash_file_path, file_paths):
    hashes = {}
    for file_path in file_paths:
        file_name = os.path.basename(file_path)
        hashes[file_name] = calculate_file_hash(file_path)
    save_hashes(hash_file_path, hashes)
