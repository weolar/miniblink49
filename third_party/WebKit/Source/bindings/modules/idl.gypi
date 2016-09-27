# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# IDL file lists; see: http://www.chromium.org/developers/web-idl-interfaces

{
  'includes': [
    '../../modules/modules.gypi',
    '../core/idl.gypi',
    'generated.gypi',
  ],

  'variables': {
    # IDL file lists; see: http://www.chromium.org/developers/web-idl-interfaces

    'modules_definition_idl_files': [
      '<@(modules_dictionary_idl_files)',
      '<@(modules_idl_files)',
    ],

    # Interface IDL files: generate individual bindings (includes testing)
    'modules_interface_idl_files': [
      # No testing or generated interface IDL files in modules currently
      '<@(modules_definition_idl_files)',
    ],

    # Write lists of main IDL files to a file, so that the command lines don't
    # exceed OS length limits.
    'modules_idl_files_list': '<|(modules_idl_files_list.tmp <@(modules_definition_idl_files))',
    'modules_dictionary_idl_files_list': '<|(modules_dictionary_idl_files_list.tmp <@(modules_dictionary_idl_files))',

    # Dependency IDL files: don't generate individual bindings, but do process
    # in IDL dependency computation, and count as build dependencies
    'all_dependency_idl_files': [
      '<@(core_all_dependency_idl_files)',
      '<@(modules_all_dependency_idl_files)',
    ],
    # 'modules_dependency_idl_files' is already used in Source/modules, so avoid
    # collision
    'modules_all_dependency_idl_files': [
      '<@(modules_static_dependency_idl_files)',
      '<@(modules_generated_dependency_idl_files)',
    ],

    # Static IDL files / Generated IDL files
    # Paths need to be passed separately for static and generated files, as
    # static files are listed in a temporary file (b/c too long for command
    # line), but generated files must be passed at the command line, as their
    # paths are not fixed at GYP time, when the temporary file is generated,
    # because their paths depend on the build directory, which varies.
    'modules_static_idl_files': [
      '<@(modules_static_interface_idl_files)',
      '<@(modules_static_dependency_idl_files)',
    ],
    'modules_static_idl_files_list':
      '<|(modules_static_idl_files_list.tmp <@(modules_static_idl_files))',

    'modules_generated_idl_files': [
      '<@(modules_generated_dependency_idl_files)',
    ],

    # Static IDL files
    'modules_static_interface_idl_files': [
      '<@(modules_definition_idl_files)',
    ],
    'modules_static_dependency_idl_files': [
      '<@(modules_dependency_idl_files)',
      '<@(modules_testing_dependency_idl_files)',
    ],

    # Generated IDL files
    'modules_generated_dependency_idl_files': [
      '<@(modules_core_global_constructors_generated_idl_files)',  # partial interfaces
      '<@(modules_global_constructors_generated_idl_files)',  # partial interfaces
    ],
  },
}
