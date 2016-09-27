#!/usr/bin/python
#
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

"""Compute global interface information for individual IDL files.

Auxiliary module for compute_interfaces_info_overall, which consolidates
this individual information, computing info that spans multiple files
(dependencies and ancestry).

This distinction is so that individual interface info can be computed
separately for each component (avoiding duplicated reading of individual
files), then consolidated using *only* the info visible to a given component.

Design doc: http://www.chromium.org/developers/design-documents/idl-build
"""

from collections import defaultdict
import optparse
import os
import posixpath
import sys

from idl_definitions import Visitor
from idl_reader import IdlReader
from utilities import get_file_contents, read_file_to_list, idl_filename_to_interface_name, idl_filename_to_component, write_pickle_file, get_interface_extended_attributes_from_idl, is_callback_interface_from_idl

module_path = os.path.dirname(__file__)
source_path = os.path.normpath(os.path.join(module_path, os.pardir, os.pardir))


class IdlBadFilenameError(Exception):
    """Raised if an IDL filename disagrees with the interface name in the file."""
    pass


def parse_options():
    usage = 'Usage: %prog [options] [generated1.idl]...'
    parser = optparse.OptionParser(usage=usage)
    parser.add_option('--cache-directory', help='cache directory')
    parser.add_option('--idl-files-list', help='file listing IDL files')
    parser.add_option('--interfaces-info-file', help='interface info pickle file')
    parser.add_option('--component-info-file', help='component wide info pickle file')
    parser.add_option('--write-file-only-if-changed', type='int', help='if true, do not write an output file if it would be identical to the existing one, which avoids unnecessary rebuilds in ninja')

    options, args = parser.parse_args()
    if options.interfaces_info_file is None:
        parser.error('Must specify an output file using --interfaces-info-file.')
    if options.idl_files_list is None:
        parser.error('Must specify a file listing IDL files using --idl-files-list.')
    if options.write_file_only_if_changed is None:
        parser.error('Must specify whether file is only written if changed using --write-file-only-if-changed.')
    options.write_file_only_if_changed = bool(options.write_file_only_if_changed)
    return options, args


################################################################################
# Computations
################################################################################

def relative_dir_posix(idl_filename):
    """Returns relative path to the directory of idl_file in POSIX format."""
    relative_path_local = os.path.relpath(idl_filename, source_path)
    relative_dir_local = os.path.dirname(relative_path_local)
    return relative_dir_local.replace(os.path.sep, posixpath.sep)


def include_path(idl_filename, implemented_as=None):
    """Returns relative path to header file in POSIX format; used in includes.

    POSIX format is used for consistency of output, so reference tests are
    platform-independent.
    """
    relative_dir = relative_dir_posix(idl_filename)

    # IDL file basename is used even if only a partial interface file
    idl_file_basename, _ = os.path.splitext(os.path.basename(idl_filename))
    cpp_class_name = implemented_as or idl_file_basename

    return posixpath.join(relative_dir, cpp_class_name + '.h')


def get_implements_from_definitions(definitions, definition_name):
    left_interfaces = []
    right_interfaces = []
    for implement in definitions.implements:
        if definition_name == implement.left_interface:
            right_interfaces.append(implement.right_interface)
        elif definition_name == implement.right_interface:
            left_interfaces.append(implement.left_interface)
        else:
            raise IdlBadFilenameError(
                'implements statement found in unrelated IDL file.\n'
                'Statement is:\n'
                '    %s implements %s;\n'
                'but filename is unrelated "%s.idl"' %
                (implement.left_interface, implement.right_interface, definition_name))
    return left_interfaces, right_interfaces


def get_put_forward_interfaces_from_definition(definition):
    return sorted(set(attribute.idl_type.base_type
                      for attribute in definition.attributes
                      if 'PutForwards' in attribute.extended_attributes))


def collect_union_types_from_definitions(definitions):
    """Traverse definitions and collect all union types."""
    class UnionTypeCollector(Visitor):
        def collect(self, definitions):
            self._union_types = set()
            definitions.accept(self)
            return self._union_types

        def visit_typed_object(self, typed_object):
            for attribute_name in typed_object.idl_type_attributes:
                attribute = getattr(typed_object, attribute_name, None)
                if not attribute:
                    continue
                for idl_type in attribute.idl_types():
                    if idl_type.is_union_type:
                        self._union_types.add(idl_type)

    return UnionTypeCollector().collect(definitions)


class InterfaceInfoCollector(object):
    """A class that collects interface information from idl files."""
    def __init__(self, cache_directory=None):
        self.reader = IdlReader(interfaces_info=None, outputdir=cache_directory)
        self.interfaces_info = {}
        self.partial_interface_files = defaultdict(lambda: {
            'full_paths': [],
            'include_paths': [],
        })
        self.enumerations = set()
        self.union_types = set()
        self.typedefs = {}

    def add_paths_to_partials_dict(self, partial_interface_name, full_path,
                                   include_paths):
        paths_dict = self.partial_interface_files[partial_interface_name]
        paths_dict['full_paths'].append(full_path)
        paths_dict['include_paths'].extend(include_paths)

    def collect_info(self, idl_filename):
        """Reads an idl file and collects information which is required by the
        binding code generation."""
        definitions = self.reader.read_idl_file(idl_filename)

        this_union_types = collect_union_types_from_definitions(definitions)
        self.union_types.update(this_union_types)
        self.typedefs.update(definitions.typedefs)
        # Check enum duplication.
        for enum_name in definitions.enumerations.keys():
            for defined_enum in self.enumerations:
                if defined_enum.name == enum_name:
                    raise Exception('Enumeration %s has multiple definitions' % enum_name)
        self.enumerations.update(definitions.enumerations.values())

        if definitions.interfaces:
            definition = next(definitions.interfaces.itervalues())
            interface_info = {
                'is_callback_interface': definition.is_callback,
                'is_dictionary': False,
                # Interfaces that are referenced (used as types) and that we
                # introspect during code generation (beyond interface-level
                # data ([ImplementedAs], is_callback_interface, ancestors, and
                # inherited extended attributes): deep dependencies.
                # These cause rebuilds of referrers, due to the dependency,
                # so these should be minimized; currently only targets of
                # [PutForwards].
                'referenced_interfaces': get_put_forward_interfaces_from_definition(definition),
            }
        elif definitions.dictionaries:
            definition = next(definitions.dictionaries.itervalues())
            interface_info = {
                'is_callback_interface': False,
                'is_dictionary': True,
                'referenced_interfaces': None,
            }
        else:
            return

        extended_attributes = definition.extended_attributes
        implemented_as = extended_attributes.get('ImplementedAs')
        full_path = os.path.realpath(idl_filename)
        this_include_path = None if 'NoImplHeader' in extended_attributes else include_path(idl_filename, implemented_as)
        if definition.is_partial:
            # We don't create interface_info for partial interfaces, but
            # adds paths to another dict.
            partial_include_paths = []
            if this_include_path:
                partial_include_paths.append(this_include_path)
            if this_union_types:
                component = idl_filename_to_component(idl_filename)
                partial_include_paths.append(
                    'bindings/%s/v8/UnionTypes%s.h' % (component, component.capitalize()))
            self.add_paths_to_partials_dict(definition.name, full_path, partial_include_paths)
            return

        # 'implements' statements can be included in either the file for the
        # implement*ing* interface (lhs of 'implements') or implement*ed* interface
        # (rhs of 'implements'). Store both for now, then merge to implement*ing*
        # interface later.
        left_interfaces, right_interfaces = get_implements_from_definitions(
            definitions, definition.name)

        interface_info.update({
            'extended_attributes': extended_attributes,
            'full_path': full_path,
            'has_union_types': bool(this_union_types),
            'implemented_as': implemented_as,
            'implemented_by_interfaces': left_interfaces,
            'implements_interfaces': right_interfaces,
            'include_path': this_include_path,
            # FIXME: temporary private field, while removing old treatement of
            # 'implements': http://crbug.com/360435
            'is_legacy_treat_as_partial_interface': 'LegacyTreatAsPartialInterface' in extended_attributes,
            'parent': definition.parent,
            'relative_dir': relative_dir_posix(idl_filename),
        })
        self.interfaces_info[definition.name] = interface_info

    def get_info_as_dict(self):
        """Returns info packaged as a dict."""
        return {
            'interfaces_info': self.interfaces_info,
            # Can't pickle defaultdict, convert to dict
            # FIXME: this should be included in get_component_info.
            'partial_interface_files': dict(self.partial_interface_files),
        }

    def get_component_info_as_dict(self):
        """Returns component wide information as a dict."""
        return {
            'enumerations': dict((enum.name, enum.values)
                                 for enum in self.enumerations),
            'typedefs': self.typedefs,
            'union_types': self.union_types,
        }


################################################################################

def main():
    options, args = parse_options()

    # Static IDL files are passed in a file (generated at GYP time), due to OS
    # command line length limits
    idl_files = read_file_to_list(options.idl_files_list)
    # Generated IDL files are passed at the command line, since these are in the
    # build directory, which is determined at build time, not GYP time, so these
    # cannot be included in the file listing static files
    idl_files.extend(args)

    # Compute information for individual files
    # Information is stored in global variables interfaces_info and
    # partial_interface_files.
    info_collector = InterfaceInfoCollector(options.cache_directory)
    for idl_filename in idl_files:
        info_collector.collect_info(idl_filename)

    write_pickle_file(options.interfaces_info_file,
                      info_collector.get_info_as_dict(),
                      options.write_file_only_if_changed)
    write_pickle_file(options.component_info_file,
                      info_collector.get_component_info_as_dict(),
                      options.write_file_only_if_changed)

if __name__ == '__main__':
    sys.exit(main())
