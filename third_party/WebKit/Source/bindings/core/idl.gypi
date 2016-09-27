# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# IDL file lists; see: http://www.chromium.org/developers/web-idl-interfaces

{
  'includes': [
    '../../core/core.gypi',
    'generated.gypi',
  ],

  'variables': {
    # IDL files that are processed by the blink's code generator. The code
    # generator will generate corresponding .h/.cpp files. Do not add IDL files
    # which won't have .h/.cpp files.
    'core_definition_idl_files': [
      '<@(core_dictionary_idl_files)',
      '<@(core_idl_files)',
      '<@(core_idl_with_modules_dependency_files)',
    ],
    'core_testing_definition_idl_files': [
      '<@(core_testing_dictionary_idl_files)',
      '<@(webcore_testing_idl_files)',
      '<@(webcore_testing_idl_with_modules_dependency_files)',
    ],

    # IDL file lists; see: http://www.chromium.org/developers/web-idl-interfaces
    # Interface IDL files: generate individual bindings (includes testing)
    'core_interface_idl_files': [
      '<@(core_definition_idl_files)',
      '<@(core_testing_definition_idl_files)',
      '<@(generated_webcore_testing_idl_files)',
    ],

    # Write lists of main IDL files to a file, so that the command lines don't
    # exceed OS length limits.
    'core_idl_files_list': '<|(core_idl_files_list.tmp <@(core_definition_idl_files))',
    'core_dictionary_idl_files_list': '<|(core_dictionary_idl_files_list.tmp <@(core_dictionary_idl_files) <@(core_testing_dictionary_idl_files))',

    # Write a list of core IDL files which have dependency IDL files in
    #  modules.
    'core_idl_with_modules_dependency_files_list': '<|(core_idl_with_modules_dependency_files_list.tmp <@(core_idl_with_modules_dependency_files))',

    # Dependency IDL files: don't generate individual bindings, but do process
    # in IDL dependency computation, and count as build dependencies
    # 'core_dependency_idl_files' is already used in Source/core, so avoid
    # collision
    'core_all_dependency_idl_files': [
      '<@(core_static_dependency_idl_files)',
      '<@(core_generated_dependency_idl_files)',
    ],

    # Static IDL files / Generated IDL files
    # Paths need to be passed separately for static and generated files, as
    # static files are listed in a temporary file (b/c too long for command
    # line), but generated files must be passed at the command line, as their
    # paths are not fixed at GYP time, when the temporary file is generated,
    # because their paths depend on the build directory, which varies.
    'core_static_idl_files': [
      '<@(core_static_interface_idl_files)',
      '<@(core_static_dependency_idl_files)',
    ],
    'core_static_idl_files_list':
      '<|(core_static_idl_files_list.tmp <@(core_static_idl_files))',

    'core_generated_idl_files': [
      '<@(core_generated_interface_idl_files)',
      '<@(core_generated_dependency_idl_files)',
    ],

    # Static IDL files
    'core_static_interface_idl_files': [
      '<@(core_definition_idl_files)',
      '<@(core_testing_definition_idl_files)',
      '<@(core_typedefs_enums_only_idl_files)',
    ],
    'core_static_dependency_idl_files': [
      '<@(core_dependency_idl_files)',
      '<@(webcore_testing_dependency_idl_files)',
    ],

    # Generated IDL files
    'core_generated_interface_idl_files': [
      '<@(generated_webcore_testing_idl_files)',  # interfaces
    ],

    'core_generated_dependency_idl_files': [
      '<@(core_global_constructors_generated_idl_files)',  # partial interfaces
    ],
  },
}
