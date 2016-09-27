#!/usr/bin/python
# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Generate initPartialInterfacesInModules(), which registers partial interfaces in modules to core interfaces."""

import cPickle as pickle
from optparse import OptionParser
import os
import posixpath
import sys
from utilities import write_file

from aggregate_generated_bindings import extract_meta_data
from utilities import read_idl_files_list_from_file


_COPYRIGHT = """// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

"""

_INIT_PARTIAL_INTERFACE = """%s
#include "config.h"

%s

namespace blink {

void initPartialInterfacesInModules()
{
%s
}

} // namespace blink
"""


def parse_options():
    usage = 'Usage: %prog [options]'
    parser = OptionParser(usage=usage)
    parser.add_option('--idl-files-list', help="a text file containing the IDL file paths, so the command line doesn't exceed OS length limits.")
    parser.add_option('--write-file-only-if-changed', type='int', help='if true, do not write an output file if it would be identical to the existing one, which avoids unnecessary rebuilds in ninja')
    parser.add_option('--output')

    options, args = parser.parse_args()
    if options.output is None:
        parser.error('Must specify output file using --output.')
    if options.idl_files_list is None:
        parser.error('Must specify a list of IDL files using --idl-files-list.')
    if options.write_file_only_if_changed is None:
        parser.error('Must specify whether file is only written if changed using --write-file-only-if-changed.')
    options.write_file_only_if_changed = bool(options.write_file_only_if_changed)
    return options


def main():
    options = parse_options()

    idl_file_names = read_idl_files_list_from_file(options.idl_files_list)

    meta_data_list = extract_meta_data(idl_file_names)
    interface_names = ['V8%sPartial' % meta_data['name']
                       for meta_data in meta_data_list]
    interface_names.sort()

    includes = ['#include "bindings/modules/v8/%s.h"' % interface_name
                for interface_name in interface_names]
    initialize_calls = ['    %s::initialize();' % interface_name
                        for interface_name in interface_names]

    content = _INIT_PARTIAL_INTERFACE % (
        _COPYRIGHT,
        '\n'.join(includes),
        '\n'.join(initialize_calls))

    write_file(content, options.output,
               only_if_changed=options.write_file_only_if_changed)


if __name__ == '__main__':
    sys.exit(main())
