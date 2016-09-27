#!/usr/bin/env python
#
# Copyright (C) 2010 Google Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#         * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#         * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#         * Neither the name of Google Inc. nor the names of its
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
#

# This script concatenates CSS files in the order specified
# as @import url(...) in the input stylesheet.

from cStringIO import StringIO
import os.path
import re
import sys


import_regex = re.compile(r"@import\s*\url\(\s*\"([^\"]+)\"\s*\)")


def extract_css_files(stylesheet_name):
    result = []
    line_number = 1
    with open(stylesheet_name, 'r') as input:
        for line in input.readlines():
            match = re.search(import_regex, line)
            if match:
                result.append((match.group(1), line_number))
            line_number += 1
    return result


def main(argv):

    if len(argv) != 3:
        print('usage: %s input.css output.css' % argv[0])
        return 1

    input_stylesheet_name = argv[1]
    output_file_name = argv[2]
    input_directory = os.path.dirname(input_stylesheet_name)
    output = StringIO()

    for input_file_name, line_number in extract_css_files(input_stylesheet_name):
        full_path = os.path.join(input_directory, input_file_name)
        if not os.path.isfile(full_path):
            raise Exception('File %s referenced in %s:%d was not found, '
                            'check source tree for consistency' %
                            (input_file_name, input_stylesheet_name, line_number))
        output.write('/* %s */\n\n' % input_file_name)
        with open(full_path, 'r') as input_file:
            output.write(input_file.read())
            output.write('\n')

    if os.path.exists(output_file_name):
        os.remove(output_file_name);
    with open(output_file_name, 'w') as output_file:
        value = output.getvalue()
        # Strip newlines and indentation.
        value = re.sub('\r?\n(\r?\n|\s)*', '', value)
        output_file.write(value)
    output.close()

    # Touch output file directory to make sure that Xcode will copy
    # modified resource files.
    if sys.platform == 'darwin':
        output_dir_name = os.path.dirname(output_file_name)
        os.utime(output_dir_name, None)


if __name__ == '__main__':
    sys.exit(main(sys.argv))
