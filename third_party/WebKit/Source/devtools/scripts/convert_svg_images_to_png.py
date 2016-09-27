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

import devtools_file_hashes
import hashlib
import os
import os.path
import re
import subprocess
import sys

try:
    import json
except ImportError:
    import simplejson as json


def check_installed(app_name):
    proc = subprocess.Popen("which %s" % app_name, stdout=subprocess.PIPE, shell=True)
    proc.communicate()
    if proc.returncode != 0:
        print "This script needs \"%s\" to be installed." % app_name
        sys.exit(1)


check_installed("inkscape")

scripts_path = os.path.dirname(os.path.abspath(__file__))
devtools_path = os.path.dirname(scripts_path)
devtools_frontend_path = devtools_path + "/front_end"
images_path = devtools_frontend_path + "/Images"
image_sources_path = images_path + "/src"
hashes_file_name = "svg2png.hashes"
hashes_file_path = image_sources_path + "/" + hashes_file_name

file_names = os.listdir(image_sources_path)
svg_file_paths = [image_sources_path + "/" + file_name for file_name in file_names if file_name.endswith(".svg")]

svg_file_paths_to_convert = devtools_file_hashes.files_with_invalid_hashes(hashes_file_path, svg_file_paths)
svg_file_names = [re.sub(".svg$", "", re.sub(".*/", "", file_path)) for file_path in svg_file_paths_to_convert]


def convert_svg_to_png(svg_file_name, png_file_name, dpi):
    svg_full_path = image_sources_path + "/" + svg_file_name + ".svg"
    png_full_path = images_path + "/" + png_file_name + ".png"
    convert_command = "inkscape -f %s -e %s -d %s" % (svg_full_path, png_full_path, dpi)
    proc = subprocess.Popen(convert_command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
    return proc

processes = {}
for file_name in svg_file_names:
    name = re.sub(".svg$", "", file_name)
    name2x = name + "_2x"
    processes[name] = convert_svg_to_png(name, name, 90)
    processes[name2x] = convert_svg_to_png(name, name2x, 180)

for file_name, proc in processes.items():
    (convert_out, _) = proc.communicate()
    print("Conversion of %s finished: %s" % (file_name, convert_out))

devtools_file_hashes.update_file_hashes(hashes_file_path, svg_file_paths)
