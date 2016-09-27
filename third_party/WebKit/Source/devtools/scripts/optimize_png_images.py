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
import os
import os.path
import subprocess
import sys

try:
    import json
except ImportError:
    import simplejson as json

scripts_path = os.path.dirname(os.path.abspath(__file__))
devtools_path = os.path.dirname(scripts_path)
blink_source_path = os.path.dirname(devtools_path)
blink_path = os.path.dirname(blink_source_path)
chromium_src_path = os.path.dirname(os.path.dirname(blink_path))
devtools_frontend_path = os.path.join(devtools_path, "front_end")
images_path = os.path.join(devtools_frontend_path, "Images")
image_sources_path = os.path.join(images_path, "src")
hashes_file_name = "optimize_png.hashes"
hashes_file_path = os.path.join(image_sources_path, hashes_file_name)

file_names = os.listdir(image_sources_path)
svg_file_paths = [os.path.join(image_sources_path, file_name) for file_name in file_names if file_name.endswith(".svg")]
svg_file_paths_to_optimize = devtools_file_hashes.files_with_invalid_hashes(hashes_file_path, svg_file_paths)
svg_file_names = [os.path.basename(file_path) for file_path in svg_file_paths_to_optimize]

optimize_script_path = os.path.join("tools", "resources", "optimize-png-files.sh")


def check_installed(app_name, package, how_to):
    proc = subprocess.Popen("which %s" % app_name, stdout=subprocess.PIPE, shell=True)
    proc.communicate()
    if proc.returncode != 0:
        print "This script needs \"%s\" to be installed." % app_name
        if how_to:
            print how_to
        else:
            print "To install execute the following command: sudo apt-get install %s" % package
        sys.exit(1)

check_installed("pngcrush", "pngcrush", None)
check_installed("optipng", "optipng", None)
check_installed("advdef", "advancecomp", None)
check_installed("pngout", None, "Utility can be downloaded here: http://www.jonof.id.au/kenutils")


def optimize_png(file_name):
    png_full_path = os.path.join(images_path, file_name + ".png")
    optimize_command = "bash %s -o2 %s" % (optimize_script_path, png_full_path)
    proc = subprocess.Popen(optimize_command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True, cwd=chromium_src_path)
    return proc

if len(svg_file_names):
    print "%d unoptimized png files found." % len(svg_file_names)
else:
    print "All png files are already optimized."
    sys.exit()

processes = {}
for file_name in svg_file_names:
    name = os.path.splitext(file_name)[0]
    name2x = name + "_2x"
    processes[name] = optimize_png(name)
    processes[name2x] = optimize_png(name2x)

for file_name, proc in processes.items():
    (optimize_out, _) = proc.communicate()
    print("Optimization of %s finished: %s" % (file_name, optimize_out))

devtools_file_hashes.update_file_hashes(hashes_file_path, svg_file_paths)
