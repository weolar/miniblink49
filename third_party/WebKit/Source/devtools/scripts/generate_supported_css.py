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

try:
    import simplejson as json
except ImportError:
    import json

import sys


def properties_from_file(file_name):
    properties = []
    propertyNames = set()
    with open(file_name, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("//") or "alias_for" in line:
                continue
            name = line.partition(" ")[0]
            entry = {"name": name}
            propertyNames.add(name)
            longhands = line.partition("longhands=")[2].partition(",")[0]
            if longhands:
                entry["longhands"] = longhands.split(";")
            properties.append(entry)

    # Filter out unsupported longhands.
    for property in properties:
        if "longhands" not in property:
            continue
        longhands = property["longhands"]
        longhands = [longhand for longhand in longhands if longhand in propertyNames]
        if not longhands:
            del property["longhands"]
        else:
            property["longhands"] = longhands
    return properties

properties = properties_from_file(sys.argv[1])
with open(sys.argv[2], "w") as f:
    f.write("WebInspector.CSSMetadata.initializeWithSupportedProperties(%s);" % json.dumps(properties))
