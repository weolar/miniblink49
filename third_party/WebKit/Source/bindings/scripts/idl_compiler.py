#!/usr/bin/python
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

"""Compile an .idl file to Blink V8 bindings (.h and .cpp files).

Design doc: http://www.chromium.org/developers/design-documents/idl-compiler
"""

import abc
from optparse import OptionParser
import os
import cPickle as pickle
import sys

from code_generator_v8 import CodeGeneratorDictionaryImpl, CodeGeneratorV8, CodeGeneratorUnionType
from idl_reader import IdlReader
from utilities import create_component_info_provider, read_idl_files_list_from_file, write_file, idl_filename_to_component


def parse_options():
    parser = OptionParser()
    parser.add_option('--cache-directory',
                      help='cache directory, defaults to output directory')
    parser.add_option('--generate-impl',
                      action="store_true", default=False)
    parser.add_option('--output-directory')
    parser.add_option('--impl-output-directory')
    parser.add_option('--info-dir')
    parser.add_option('--write-file-only-if-changed', type='int')
    # FIXME: We should always explicitly specify --target-component and
    # remove the default behavior.
    parser.add_option('--target-component',
                      help='target component to generate code, defaults to '
                      'component of input idl file')
    # ensure output comes last, so command line easy to parse via regexes
    parser.disable_interspersed_args()

    options, args = parser.parse_args()
    if options.output_directory is None:
        parser.error('Must specify output directory using --output-directory.')
    options.write_file_only_if_changed = bool(options.write_file_only_if_changed)
    if len(args) != 1:
        parser.error('Must specify exactly 1 input file as argument, but %d given.' % len(args))
    idl_filename = os.path.realpath(args[0])
    return options, idl_filename


def idl_filename_to_interface_name(idl_filename):
    basename = os.path.basename(idl_filename)
    interface_name, _ = os.path.splitext(basename)
    return interface_name


class IdlCompiler(object):
    """Abstract Base Class for IDL compilers.

    In concrete classes:
    * self.code_generator must be set, implementing generate_code()
      (returning a list of output code), and
    * compile_file() must be implemented (handling output filenames).
    """
    __metaclass__ = abc.ABCMeta

    def __init__(self, output_directory, cache_directory=None,
                 code_generator=None, info_provider=None,
                 only_if_changed=False, target_component=None):
        """
        Args:
          output_directory: directory to put output files.
          cache_directory: directory which contains PLY caches.
          code_generator: code generator to be used.
          info_provider: component-specific information provider.
          only_if_changed: True when the compiler should only write output files
            when the contents are changed.
          target_component: component to be processed.
        """
        self.cache_directory = cache_directory
        self.code_generator = code_generator
        self.info_provider = info_provider
        self.only_if_changed = only_if_changed
        self.output_directory = output_directory
        self.target_component = target_component
        self.reader = IdlReader(info_provider.interfaces_info, cache_directory)

    def compile_and_write(self, idl_filename):
        interface_name = idl_filename_to_interface_name(idl_filename)
        definitions = self.reader.read_idl_definitions(idl_filename)
        target_component = self.target_component or idl_filename_to_component(idl_filename)
        target_definitions = definitions[target_component]
        output_code_list = self.code_generator.generate_code(
            target_definitions, interface_name)
        for output_path, output_code in output_code_list:
            write_file(output_code, output_path, self.only_if_changed)

    @abc.abstractmethod
    def compile_file(self, idl_filename):
        pass


class IdlCompilerV8(IdlCompiler):
    def __init__(self, *args, **kwargs):
        IdlCompiler.__init__(self, *args, **kwargs)
        self.code_generator = CodeGeneratorV8(self.info_provider,
                                              self.cache_directory,
                                              self.output_directory)

    def compile_file(self, idl_filename):
        self.compile_and_write(idl_filename)


class IdlCompilerDictionaryImpl(IdlCompiler):
    def __init__(self, *args, **kwargs):
        IdlCompiler.__init__(self, *args, **kwargs)
        self.code_generator = CodeGeneratorDictionaryImpl(
            self.info_provider, self.cache_directory, self.output_directory)

    def compile_file(self, idl_filename):
        self.compile_and_write(idl_filename)


def generate_bindings(options, input_filename):
    info_provider = create_component_info_provider(
        options.info_dir, options.target_component)
    idl_compiler = IdlCompilerV8(
        options.output_directory,
        cache_directory=options.cache_directory,
        info_provider=info_provider,
        only_if_changed=options.write_file_only_if_changed,
        target_component=options.target_component)
    idl_compiler.compile_file(input_filename)


def generate_dictionary_impl(options, input_filename):
    info_provider = create_component_info_provider(
        options.info_dir, options.target_component)
    idl_compiler = IdlCompilerDictionaryImpl(
        options.impl_output_directory,
        cache_directory=options.cache_directory,
        info_provider=info_provider,
        only_if_changed=options.write_file_only_if_changed)

    idl_filenames = read_idl_files_list_from_file(input_filename)
    for idl_filename in idl_filenames:
        idl_compiler.compile_file(idl_filename)


def generate_union_type_containers(options):
    info_provider = create_component_info_provider(
        options.info_dir, options.target_component)
    if not info_provider.interfaces_info:
        raise Exception('Interfaces info is required to generate '
                        'union types containers')
    generator = CodeGeneratorUnionType(
        info_provider,
        options.cache_directory,
        options.output_directory,
        options.target_component)
    output_code_list = generator.generate_code()
    for output_path, output_code in output_code_list:
        write_file(output_code, output_path, options.write_file_only_if_changed)


def main():
    options, input_filename = parse_options()
    if options.generate_impl:
        # |input_filename| should be a file which contains a list of IDL
        # dictionary paths.
        generate_dictionary_impl(options, input_filename)
        generate_union_type_containers(options)
    else:
        # |input_filename| should be a path of an IDL file.
        generate_bindings(options, input_filename)


if __name__ == '__main__':
    sys.exit(main())
