# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Utility functions (file reading, simple IDL parsing by regexes) for IDL build.

Design doc: http://www.chromium.org/developers/design-documents/idl-build
"""

import os
import cPickle as pickle
import re
import string
import subprocess


KNOWN_COMPONENTS = frozenset(['core', 'modules'])
KNOWN_COMPONENTS_WITH_TESTING = frozenset(['core', 'modules', 'testing'])


def idl_filename_to_interface_name(idl_filename):
    # interface name is the root of the basename: InterfaceName.idl
    return os.path.splitext(os.path.basename(idl_filename))[0]


def idl_filename_to_component_with_known_components(idl_filename, known_components):
    path = os.path.dirname(os.path.realpath(idl_filename))
    while path:
        dirname, basename = os.path.split(path)
        if not basename:
            break
        if basename.lower() in known_components:
            return basename.lower()
        path = dirname
    raise Exception('Unknown component type for %s' % idl_filename)


def idl_filename_to_component(idl_filename):
    return idl_filename_to_component_with_known_components(idl_filename, KNOWN_COMPONENTS)


def is_testing_target(idl_filename):
    component = idl_filename_to_component_with_known_components(idl_filename, KNOWN_COMPONENTS_WITH_TESTING)
    return component == 'testing'


# See whether "component" can depend on "dependency" or not:
# Suppose that we have interface X and Y:
# - if X is a partial interface and Y is the original interface,
#   use is_valid_component_dependency(X, Y).
# - if X implements Y, use is_valid_component_dependency(X, Y)
# Suppose that X is a cpp file and Y is a header file:
# - if X includes Y, use is_valid_component_dependency(X, Y)
def is_valid_component_dependency(component, dependency):
    assert component in KNOWN_COMPONENTS
    assert dependency in KNOWN_COMPONENTS
    if component == 'core' and dependency == 'modules':
        return False
    return True


class ComponentInfoProvider(object):
    """Base class of information provider which provides component-specific
    information.
    """
    def __init__(self):
        pass

    @property
    def interfaces_info(self):
        return {}

    @property
    def component_info(self):
        return {}

    @property
    def enumerations(self):
        return {}

    @property
    def typedefs(self):
        return {}

    @property
    def union_types(self):
        return set()

    @property
    def include_path_for_union_types(self):
        return None


class ComponentInfoProviderCore(ComponentInfoProvider):
    def __init__(self, interfaces_info, component_info):
        super(ComponentInfoProviderCore, self).__init__()
        self._interfaces_info = interfaces_info
        self._component_info = component_info

    @property
    def interfaces_info(self):
        return self._interfaces_info

    @property
    def component_info(self):
        return self._component_info

    @property
    def enumerations(self):
        return self._component_info['enumerations']

    @property
    def typedefs(self):
        return self._component_info['typedefs']

    @property
    def union_types(self):
        return self._component_info['union_types']

    @property
    def include_path_for_union_types(self):
        return 'bindings/core/v8/UnionTypesCore.h'

    @property
    def specifier_for_export(self):
        return 'CORE_EXPORT '

    @property
    def include_path_for_export(self):
        return 'core/CoreExport.h'


class ComponentInfoProviderModules(ComponentInfoProvider):
    def __init__(self, interfaces_info, component_info_core,
                 component_info_modules):
        super(ComponentInfoProviderModules, self).__init__()
        self._interfaces_info = interfaces_info
        self._component_info_core = component_info_core
        self._component_info_modules = component_info_modules

    @property
    def interfaces_info(self):
        return self._interfaces_info

    @property
    def component_info(self):
        return self._component_info_modules

    @property
    def enumerations(self):
        enums = self._component_info_core['enumerations'].copy()
        enums.update(self._component_info_modules['enumerations'])
        return enums

    @property
    def typedefs(self):
        typedefs = self._component_info_core['typedefs'].copy()
        typedefs.update(self._component_info_modules['typedefs'])
        return typedefs

    @property
    def union_types(self):
        # Remove duplicate union types from component_info_modules to avoid
        # generating multiple container generation.
        return self._component_info_modules['union_types'] - self._component_info_core['union_types']

    @property
    def include_path_for_union_types(self):
        return 'bindings/modules/v8/UnionTypesModules.h'

    @property
    def specifier_for_export(self):
        return 'MODULES_EXPORT '

    @property
    def include_path_for_export(self):
        return 'modules/ModulesExport.h'


def load_interfaces_info_overall_pickle(info_dir):
    with open(os.path.join(info_dir, 'modules', 'InterfacesInfoOverall.pickle')) as interface_info_file:
        return pickle.load(interface_info_file)


def create_component_info_provider_core(info_dir):
    interfaces_info = load_interfaces_info_overall_pickle(info_dir)
    with open(os.path.join(info_dir, 'core', 'ComponentInfoCore.pickle')) as component_info_file:
        component_info = pickle.load(component_info_file)
    return ComponentInfoProviderCore(interfaces_info, component_info)


def create_component_info_provider_modules(info_dir):
    interfaces_info = load_interfaces_info_overall_pickle(info_dir)
    with open(os.path.join(info_dir, 'core', 'ComponentInfoCore.pickle')) as component_info_file:
        component_info_core = pickle.load(component_info_file)
    with open(os.path.join(info_dir, 'modules', 'ComponentInfoModules.pickle')) as component_info_file:
        component_info_modules = pickle.load(component_info_file)
    return ComponentInfoProviderModules(
        interfaces_info, component_info_core, component_info_modules)


def create_component_info_provider(info_dir, component):
    if component == 'core':
        return create_component_info_provider_core(info_dir)
    elif component == 'modules':
        return create_component_info_provider_modules(info_dir)
    else:
        return ComponentInfoProvider()


################################################################################
# Basic file reading/writing
################################################################################

def get_file_contents(filename):
    with open(filename) as f:
        return f.read()


def read_file_to_list(filename):
    """Returns a list of (stripped) lines for a given filename."""
    with open(filename) as f:
        return [line.rstrip('\n') for line in f]


def resolve_cygpath(cygdrive_names):
    if not cygdrive_names:
        return []
    cmd = ['cygpath', '-f', '-', '-wa']
    process = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    idl_file_names = []
    for file_name in cygdrive_names:
        process.stdin.write('%s\n' % file_name)
        process.stdin.flush()
        idl_file_names.append(process.stdout.readline().rstrip())
    process.stdin.close()
    process.wait()
    return idl_file_names


def read_idl_files_list_from_file(filename):
    """Similar to read_file_to_list, but also resolves cygpath."""
    with open(filename) as input_file:
        file_names = sorted([os.path.realpath(line.rstrip('\n'))
                             for line in input_file])
        idl_file_names = [file_name for file_name in file_names
                          if not file_name.startswith('/cygdrive')]
        cygdrive_names = [file_name for file_name in file_names
                          if file_name.startswith('/cygdrive')]
        idl_file_names.extend(resolve_cygpath(cygdrive_names))
        return idl_file_names


def read_pickle_files(pickle_filenames):
    for pickle_filename in pickle_filenames:
        with open(pickle_filename) as pickle_file:
            yield pickle.load(pickle_file)


def write_file(new_text, destination_filename, only_if_changed):
    if only_if_changed and os.path.isfile(destination_filename):
        with open(destination_filename) as destination_file:
            if destination_file.read() == new_text:
                return
    destination_dirname = os.path.dirname(destination_filename)
    if not os.path.exists(destination_dirname):
        os.makedirs(destination_dirname)
    with open(destination_filename, 'w') as destination_file:
        destination_file.write(new_text)


def write_pickle_file(pickle_filename, data, only_if_changed):
    if only_if_changed and os.path.isfile(pickle_filename):
        with open(pickle_filename) as pickle_file:
            try:
                if pickle.load(pickle_file) == data:
                    return
            except Exception:
                # If trouble unpickling, overwrite
                pass
    with open(pickle_filename, 'w') as pickle_file:
        pickle.dump(data, pickle_file)


################################################################################
# IDL parsing
#
# We use regular expressions for parsing; this is incorrect (Web IDL is not a
# regular language), but simple and sufficient in practice.
# Leading and trailing context (e.g. following '{') used to avoid false matches.
################################################################################

def is_callback_interface_from_idl(file_contents):
    match = re.search(r'callback\s+interface\s+\w+\s*{', file_contents)
    return bool(match)


def should_generate_impl_file_from_idl(file_contents):
    """True when a given IDL file contents could generate .h/.cpp files."""
    # FIXME: This would be error-prone and we should use AST rather than
    # improving the regexp pattern.
    match = re.search(r'(interface|dictionary|exception)\s+\w+', file_contents)
    return bool(match)


def match_interface_extended_attributes_from_idl(file_contents):
    # Strip comments
    # re.compile needed b/c Python 2.6 doesn't support flags in re.sub
    single_line_comment_re = re.compile(r'//.*$', flags=re.MULTILINE)
    block_comment_re = re.compile(r'/\*.*?\*/', flags=re.MULTILINE | re.DOTALL)
    file_contents = re.sub(single_line_comment_re, '', file_contents)
    file_contents = re.sub(block_comment_re, '', file_contents)

    match = re.search(r'\[(.*)\]\s*'
                      r'((callback|partial)\s+)?'
                      r'(interface|exception)\s+'
                      r'\w+\s*'
                      r'(:\s*\w+\s*)?'
                      r'{',
                      file_contents, flags=re.DOTALL)
    return match


def get_interface_extended_attributes_from_idl(file_contents):
    match = match_interface_extended_attributes_from_idl(file_contents)
    if not match:
        return {}

    extended_attributes_string = match.group(1)
    extended_attributes = {}
    # FIXME: this splitting is WRONG: it fails on extended attributes where lists of
    # multiple values are used, which are seperated by a comma and a space.
    parts = [extended_attribute.strip()
             for extended_attribute in re.split(',\s+', extended_attributes_string)
             # Discard empty parts, which may exist due to trailing comma
             if extended_attribute.strip()]
    for part in parts:
        name, _, value = map(string.strip, part.partition('='))
        extended_attributes[name] = value
    return extended_attributes


def get_interface_exposed_arguments(file_contents):
    match = match_interface_extended_attributes_from_idl(file_contents)
    if not match:
        return None

    extended_attributes_string = match.group(1)
    match = re.search(r'[^=]\bExposed\(([^)]*)\)', file_contents)
    if not match:
        return None
    arguments = []
    for argument in map(string.strip, match.group(1).split(',')):
        exposed, runtime_enabled = argument.split()
        arguments.append({'exposed': exposed, 'runtime_enabled': runtime_enabled})

    return arguments
