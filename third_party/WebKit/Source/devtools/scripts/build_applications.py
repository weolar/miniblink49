#!/usr/bin/env python
#
# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""
Invokes concatenate_application_code for applications specified on the command line.
"""

from os import path
import concatenate_application_code
import modular_build
import sys

try:
    import simplejson as json
except ImportError:
    import json


def main(argv):
    try:
        input_path_flag_index = argv.index('--input_path')
        input_path = argv[input_path_flag_index + 1]
        output_path_flag_index = argv.index('--output_path')
        output_path = argv[output_path_flag_index + 1]
        application_names = argv[1:input_path_flag_index]
        debug_flag_index = argv.index('--debug')
        minify = argv[debug_flag_index + 1] == '0'
    except:
        print('Usage: %s app_1 app_2 ... app_N --input_path <input_path> --output_path <output_path> --debug <0_or_1>' % argv[0])
        raise

    loader = modular_build.DescriptorLoader(input_path)
    for app in application_names:
        concatenate_application_code.build_application(app, loader, input_path, output_path, minify)

if __name__ == '__main__':
    sys.exit(main(sys.argv))
