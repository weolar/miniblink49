# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# Generate IDL interfaces info for core, used to generate bindings
#
# Design doc: http://www.chromium.org/developers/design-documents/idl-build

{
  'includes': [
    # ../.. == Source
    '../../bindings/scripts/scripts.gypi',
    '../../core/core.gypi',
    'core.gypi',
    'generated.gypi',
    'idl.gypi',
  ],

  'targets': [
################################################################################
  {
    'target_name': 'core_global_objects',
    'variables': {
      'idl_files': [
        '<@(core_idl_files)',
        '<@(core_idl_with_modules_dependency_files)',
      ],
      'output_file': '<(bindings_core_output_dir)/GlobalObjectsCore.pickle',
    },
    'includes': ['../../bindings/scripts/global_objects.gypi'],
  },
################################################################################
  {
    'target_name': 'core_global_constructors_idls',
    'dependencies': [
      'core_global_objects',
    ],
    'variables': {
      'idl_files': [
        '<@(core_idl_files)',
        '<@(core_idl_with_modules_dependency_files)',
      ],
      'global_objects_file':
        '<(bindings_core_output_dir)/GlobalObjectsCore.pickle',
      'global_names_idl_files': [
        'Window',
        '<(blink_core_output_dir)/WindowCoreConstructors.idl',
        'SharedWorkerGlobalScope',
        '<(blink_core_output_dir)/SharedWorkerGlobalScopeCoreConstructors.idl',
        'DedicatedWorkerGlobalScope',
        '<(blink_core_output_dir)/DedicatedWorkerGlobalScopeCoreConstructors.idl',
      ],
      'outputs': [
        '<@(core_global_constructors_generated_idl_files)',
        '<@(core_global_constructors_generated_header_files)',
      ],
    },
    'includes': ['../../bindings/scripts/global_constructors.gypi'],
  },
################################################################################
  {
    'target_name': 'interfaces_info_individual_core',
    'dependencies': [
      '<(bindings_scripts_dir)/scripts.gyp:cached_lex_yacc_tables',
      '../../core/core_generated.gyp:generated_testing_idls',
      'core_global_constructors_idls',
    ],
    'variables': {
      'cache_directory': '<(bindings_core_output_dir)/../scripts',
      'static_idl_files': '<(core_static_idl_files)',
      'generated_idl_files': '<(core_generated_idl_files)',
      'interfaces_info_file':
        '<(bindings_core_output_dir)/InterfacesInfoCoreIndividual.pickle',
      'component_info_file':
        '<(bindings_core_output_dir)/ComponentInfoCore.pickle',
    },
    'includes': ['../../bindings/scripts/interfaces_info_individual.gypi'],
  },
################################################################################
  ],  # targets
}
