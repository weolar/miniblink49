#!/usr/bin/env python
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

import os.path
import sys

import in_generator
import license
import name_utilities
import template_expander


HEADER_TEMPLATE = """%(license)s

#ifndef %(namespace)s%(suffix)sHeaders_h
#define %(namespace)s%(suffix)sHeaders_h
%(base_header_for_suffix)s
%(includes)s

#endif // %(namespace)s%(suffix)sHeaders_h
"""


def case_insensitive_matching(name):
    return (name == ('HTMLEvents')
            or name == 'Event'
            or name == 'Events'
            or name.startswith('UIEvent')
            or name.startswith('CustomEvent')
            or name.startswith('MouseEvent'))


class EventFactoryWriter(in_generator.Writer):
    defaults = {
        'ImplementedAs': None,
        'Conditional': None,
        'RuntimeEnabled': None,
    }
    default_parameters = {
        'export': '',
        'namespace': '',
        'suffix': '',
    }
    filters = {
        'cpp_name': name_utilities.cpp_name,
        'enable_conditional': name_utilities.enable_conditional_if_endif,
        'lower_first': name_utilities.lower_first,
        'case_insensitive_matching': case_insensitive_matching,
        'script_name': name_utilities.script_name,
    }

    def __init__(self, in_file_path):
        super(EventFactoryWriter, self).__init__(in_file_path)
        self.namespace = self.in_file.parameters['namespace'].strip('"')
        self.suffix = self.in_file.parameters['suffix'].strip('"')
        self._validate_entries()
        self._outputs = {(self.namespace + self.suffix + "Headers.h"): self.generate_headers_header,
                         (self.namespace + self.suffix + ".cpp"): self.generate_implementation,
                        }

    def _validate_entries(self):
        # If there is more than one entry with the same script name, only the first one will ever
        # be hit in practice, and so we'll silently ignore any properties requested for the second
        # (like RuntimeEnabled - see crbug.com/332588).
        entries_by_script_name = dict()
        for entry in self.in_file.name_dictionaries:
            script_name = name_utilities.script_name(entry)
            if script_name in entries_by_script_name:
                self._fatal('Multiple entries with script_name=%(script_name)s: %(name1)s %(name2)s' % {
                    'script_name': script_name,
                    'name1': entry['name'],
                    'name2': entries_by_script_name[script_name]['name']})
            entries_by_script_name[script_name] = entry

    def _fatal(self, message):
        print 'FATAL ERROR: ' + message
        exit(1)

    def _headers_header_include_path(self, entry):
        if entry['ImplementedAs']:
            path = os.path.dirname(entry['name'])
            if len(path):
                path += '/'
            path += entry['ImplementedAs']
        else:
            path = entry['name']
        return path + '.h'

    def _headers_header_includes(self, entries):
        includes = dict()
        for entry in entries:
            cpp_name = name_utilities.cpp_name(entry)
            # Avoid duplicate includes.
            if cpp_name in includes:
                continue
            if self.suffix == 'Modules':
                subdir_name = 'modules'
            else:
                subdir_name = 'core'
            include = '#include "%(path)s"\n#include "bindings/%(subdir_name)s/v8/V8%(script_name)s.h"' % {
                'path': self._headers_header_include_path(entry),
                'script_name': name_utilities.script_name(entry),
                'subdir_name': subdir_name,
            }
            includes[cpp_name] = self.wrap_with_condition(include, entry['Conditional'])
        return includes.values()

    def generate_headers_header(self):
        base_header_for_suffix = ''
        if self.suffix:
            base_header_for_suffix = '\n#include "core/%(namespace)sHeaders.h"\n' % {'namespace': self.namespace}
        return HEADER_TEMPLATE % {
            'license': license.license_for_generated_cpp(),
            'namespace': self.namespace,
            'suffix': self.suffix,
            'base_header_for_suffix': base_header_for_suffix,
            'includes': '\n'.join(self._headers_header_includes(self.in_file.name_dictionaries)),
        }

    @template_expander.use_jinja('EventFactory.cpp.tmpl', filters=filters)
    def generate_implementation(self):
        return {
            'namespace': self.namespace,
            'suffix': self.suffix,
            'events': self.in_file.name_dictionaries,
        }


if __name__ == "__main__":
    in_generator.Maker(EventFactoryWriter).main(sys.argv)
