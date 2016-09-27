# Copyright (C) 2011 Google Inc.  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

from contextlib import contextmanager
import filecmp
import fnmatch
import os
import re
import shutil
import sys
import tempfile

from webkitpy.common.system.executive import Executive

# Source/ path is needed both to find input IDL files, and to import other
# Python modules.
module_path = os.path.dirname(__file__)
source_path = os.path.normpath(os.path.join(module_path, os.pardir, os.pardir,
                                            os.pardir, os.pardir, 'Source'))
sys.path.append(source_path)  # for Source/bindings imports

from bindings.scripts.code_generator_v8 import CodeGeneratorUnionType
import bindings.scripts.compute_interfaces_info_individual
from bindings.scripts.compute_interfaces_info_individual import InterfaceInfoCollector
import bindings.scripts.compute_interfaces_info_overall
from bindings.scripts.compute_interfaces_info_overall import compute_interfaces_info_overall, interfaces_info
from bindings.scripts.idl_compiler import IdlCompilerDictionaryImpl, IdlCompilerV8
from bindings.scripts.idl_reader import IdlReader
from bindings.scripts.utilities import ComponentInfoProviderCore, ComponentInfoProviderModules, idl_filename_to_component, write_file


PASS_MESSAGE = 'All tests PASS!'
FAIL_MESSAGE = """Some tests FAIL!
To update the reference files, execute:
    run-bindings-tests --reset-results

If the failures are not due to your changes, test results may be out of sync;
please rebaseline them in a separate CL, after checking that tests fail in ToT.
In CL, please set:
NOTRY=true
TBR=(someone in Source/bindings/OWNERS or WATCHLISTS:bindings)
"""

DEPENDENCY_IDL_FILES = frozenset([
    'TestImplements.idl',
    'TestImplements2.idl',
    'TestImplements3.idl',
    'TestPartialInterface.idl',
    'TestPartialInterface2.idl',
    'TestPartialInterface3.idl',
    'TestPartialInterface4.idl',
])

# core/inspector/InspectorInstrumentation.idl is not a valid Blink IDL.
NON_BLINK_IDL_FILES = frozenset([
    'InspectorInstrumentation.idl',
])

COMPONENT_DIRECTORY = frozenset(['core', 'modules'])

test_input_directory = os.path.join(source_path, 'bindings', 'tests', 'idls')
reference_directory = os.path.join(source_path, 'bindings', 'tests', 'results')

# component -> ComponentInfoProvider.
# Note that this dict contains information about testing idl files, which live
# in Source/bindings/tests/idls/{core,modules}, not in Source/{core,modules}.
component_info_providers = {}

@contextmanager
def TemporaryDirectory():
    """Wrapper for tempfile.mkdtemp() so it's usable with 'with' statement.

    Simple backport of tempfile.TemporaryDirectory from Python 3.2.
    """
    name = tempfile.mkdtemp()
    try:
        yield name
    finally:
        shutil.rmtree(name)


def generate_interface_dependencies(output_directory):
    def idl_paths_recursive(directory):
        # This is slow, especially on Windows, due to os.walk making
        # excess stat() calls. Faster versions may appear in Python 3.5 or
        # later:
        # https://github.com/benhoyt/scandir
        # http://bugs.python.org/issue11406
        idl_paths = []
        for dirpath, _, files in os.walk(directory):
            idl_paths.extend(os.path.join(dirpath, filename)
                             for filename in fnmatch.filter(files, '*.idl'))
        return idl_paths

    def collect_blink_idl_paths():
        """Returns IDL file paths which blink actually uses."""
        idl_paths = []
        for component in COMPONENT_DIRECTORY:
            directory = os.path.join(source_path, component)
            idl_paths.extend(idl_paths_recursive(directory))
        return idl_paths

    def collect_interfaces_info(idl_path_list):
        info_collector = InterfaceInfoCollector()
        for idl_path in idl_path_list:
            if os.path.basename(idl_path) in NON_BLINK_IDL_FILES:
                continue
            info_collector.collect_info(idl_path)
        info = info_collector.get_info_as_dict()
        # TestDictionary.{h,cpp} are placed under
        # Source/bindings/tests/idls/core. However, IdlCompiler generates
        # TestDictionary.{h,cpp} by using relative_dir.
        # So the files will be generated under
        # output_dir/core/bindings/tests/idls/core.
        # To avoid this issue, we need to clear relative_dir here.
        for value in info['interfaces_info'].itervalues():
            value['relative_dir'] = ''
        component_info = info_collector.get_component_info_as_dict()
        return info, component_info

    # We compute interfaces info for *all* IDL files, not just test IDL
    # files, as code generator output depends on inheritance (both ancestor
    # chain and inherited extended attributes), and some real interfaces
    # are special-cased, such as Node.
    #
    # For example, when testing the behavior of interfaces that inherit
    # from Node, we also need to know that these inherit from EventTarget,
    # since this is also special-cased and Node inherits from EventTarget,
    # but this inheritance information requires computing dependencies for
    # the real Node.idl file.
    non_test_idl_paths = collect_blink_idl_paths()
    # For bindings test IDL files, we collect interfaces info for each
    # component so that we can generate union type containers separately.
    test_idl_paths = {}
    for component in COMPONENT_DIRECTORY:
        test_idl_paths[component] = idl_paths_recursive(
            os.path.join(test_input_directory, component))
    # 2nd-stage computation: individual, then overall
    #
    # Properly should compute separately by component (currently test
    # includes are invalid), but that's brittle (would need to update this file
    # for each new component) and doesn't test the code generator any better
    # than using a single component.
    non_test_interfaces_info, non_test_component_info = collect_interfaces_info(non_test_idl_paths)
    test_interfaces_info = {}
    test_component_info = {}
    for component, paths in test_idl_paths.iteritems():
        test_interfaces_info[component], test_component_info[component] = collect_interfaces_info(paths)
    # In order to allow test IDL files to override the production IDL files if
    # they have the same interface name, process the test IDL files after the
    # non-test IDL files.
    info_individuals = [non_test_interfaces_info] + test_interfaces_info.values()
    compute_interfaces_info_overall(info_individuals)
    # Add typedefs which are specified in the actual IDL files to the testing
    # component info.
    test_component_info['core']['typedefs'].update(
        non_test_component_info['typedefs'])
    component_info_providers['core'] = ComponentInfoProviderCore(
        interfaces_info, test_component_info['core'])
    component_info_providers['modules'] = ComponentInfoProviderModules(
        interfaces_info, test_component_info['core'],
        test_component_info['modules'])


def bindings_tests(output_directory, verbose):
    executive = Executive()

    def list_files(directory):
        files = []
        for component in os.listdir(directory):
            if component not in COMPONENT_DIRECTORY:
                continue
            directory_with_component = os.path.join(directory, component)
            for filename in os.listdir(directory_with_component):
                files.append(os.path.join(directory_with_component, filename))
        return files

    def diff(filename1, filename2):
        # Python's difflib module is too slow, especially on long output, so
        # run external diff(1) command
        cmd = ['diff',
               '-u',  # unified format
               '-N',  # treat absent files as empty
               filename1,
               filename2]
        # Return output and don't raise exception, even though diff(1) has
        # non-zero exit if files differ.
        return executive.run_command(cmd, error_handler=lambda x: None)

    def is_cache_file(filename):
        return filename.endswith('.cache')

    def delete_cache_files():
        # FIXME: Instead of deleting cache files, don't generate them.
        cache_files = [path for path in list_files(output_directory)
                       if is_cache_file(os.path.basename(path))]
        for cache_file in cache_files:
            os.remove(cache_file)

    def identical_file(reference_filename, output_filename):
        reference_basename = os.path.basename(reference_filename)

        if not os.path.isfile(reference_filename):
            print 'Missing reference file!'
            print '(if adding new test, update reference files)'
            print reference_basename
            print
            return False

        if not filecmp.cmp(reference_filename, output_filename):
            # cmp is much faster than diff, and usual case is "no differance",
            # so only run diff if cmp detects a difference
            print 'FAIL: %s' % reference_basename
            print diff(reference_filename, output_filename)
            return False

        if verbose:
            print 'PASS: %s' % reference_basename
        return True

    def identical_output_files(output_files):
        reference_files = [os.path.join(reference_directory,
                                        os.path.relpath(path, output_directory))
                           for path in output_files]
        return all([identical_file(reference_filename, output_filename)
                    for (reference_filename, output_filename) in zip(reference_files, output_files)])

    def no_excess_files(output_files):
        generated_files = set([os.path.relpath(path, output_directory)
                               for path in output_files])
        # Add subversion working copy directories in core and modules.
        for component in COMPONENT_DIRECTORY:
            generated_files.add(os.path.join(component, '.svn'))

        excess_files = []
        for path in list_files(reference_directory):
            relpath = os.path.relpath(path, reference_directory)
            if relpath not in generated_files:
                excess_files.append(relpath)
        if excess_files:
            print ('Excess reference files! '
                  '(probably cruft from renaming or deleting):\n' +
                  '\n'.join(excess_files))
            return False
        return True

    def generate_union_type_containers(output_directory, component):
        generator = CodeGeneratorUnionType(
            component_info_providers[component], cache_dir=None,
            output_dir=output_directory, target_component=component)
        outputs = generator.generate_code()
        for output_path, output_code in outputs:
            write_file(output_code, output_path, only_if_changed=True)

    try:
        generate_interface_dependencies(output_directory)
        for component in COMPONENT_DIRECTORY:
            output_dir = os.path.join(output_directory, component)
            if not os.path.exists(output_dir):
                os.makedirs(output_dir)

            generate_union_type_containers(output_dir, component)

            idl_compiler = IdlCompilerV8(
                output_dir,
                info_provider=component_info_providers[component],
                only_if_changed=True)
            if component == 'core':
                partial_interface_output_dir = os.path.join(output_directory,
                                                            'modules')
                if not os.path.exists(partial_interface_output_dir):
                    os.makedirs(partial_interface_output_dir)
                idl_partial_interface_compiler = IdlCompilerV8(
                    partial_interface_output_dir,
                    info_provider=component_info_providers['modules'],
                    only_if_changed=True,
                    target_component='modules')
            else:
                idl_partial_interface_compiler = None

            dictionary_impl_compiler = IdlCompilerDictionaryImpl(
                output_dir, info_provider=component_info_providers[component],
                only_if_changed=True)

            idl_filenames = []
            input_directory = os.path.join(test_input_directory, component)
            for filename in os.listdir(input_directory):
                if (filename.endswith('.idl') and
                    # Dependencies aren't built
                    # (they are used by the dependent)
                    filename not in DEPENDENCY_IDL_FILES):
                    idl_filenames.append(
                        os.path.realpath(
                            os.path.join(input_directory, filename)))
            for idl_path in idl_filenames:
                idl_basename = os.path.basename(idl_path)
                idl_compiler.compile_file(idl_path)
                definition_name, _ = os.path.splitext(idl_basename)
                if definition_name in interfaces_info:
                    interface_info = interfaces_info[definition_name]
                    if interface_info['is_dictionary']:
                        dictionary_impl_compiler.compile_file(idl_path)
                    if component == 'core' and interface_info['dependencies_other_component_full_paths']:
                        idl_partial_interface_compiler.compile_file(idl_path)
                if verbose:
                    print 'Compiled: %s' % idl_path
    finally:
        delete_cache_files()

    # Detect all changes
    output_files = list_files(output_directory)
    passed = identical_output_files(output_files)
    passed &= no_excess_files(output_files)

    if passed:
        if verbose:
            print
            print PASS_MESSAGE
        return 0
    print
    print FAIL_MESSAGE
    return 1


def run_bindings_tests(reset_results, verbose):
    # Generate output into the reference directory if resetting results, or
    # a temp directory if not.
    if reset_results:
        print 'Resetting results'
        return bindings_tests(reference_directory, verbose)
    with TemporaryDirectory() as temp_dir:
        return bindings_tests(temp_dir, verbose)
