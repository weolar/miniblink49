# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# Generate IDL interfaces info for modules, used to generate bindings
#
# Design doc: http://www.chromium.org/developers/design-documents/idl-build

{
  'includes': [
    # ../.. == Source
    '../../bindings/core/core.gypi',
    '../../bindings/scripts/scripts.gypi',
    '../../build/scripts/scripts.gypi',  # FIXME: Needed for event files, should be in modules, not bindings_modules http://crbug.com/358074
    '../../modules/modules.gypi',
    'generated.gypi',
    'idl.gypi',
    'modules.gypi',
  ],

  'targets': [
################################################################################
  {
    # GN version: //third_party/WebKit/Source/bindings/modules:bindings_modules_generated
    # FIXME: Should be in modules, not bindings_modules http://crbug.com/358074
    'target_name': 'modules_event_generated',
    'type': 'none',
    'actions': [
      {
        # GN version: //third_party/WebKit/Source/bindings/modules:modules_bindings_generated_event_interfaces
        'action_name': 'event_interfaces',
        'variables': {
          'event_idl_files': [
            '<@(modules_event_idl_files)',
          ],
          'event_idl_files_list':
              '<|(event_idl_files_list.tmp <@(event_idl_files))',
        },
        'inputs': [
          '<(bindings_scripts_dir)/generate_event_interfaces.py',
          '<(bindings_scripts_dir)/utilities.py',
          '<(event_idl_files_list)',
          '<@(event_idl_files)',
        ],
        'outputs': [
          '<(blink_modules_output_dir)/EventModulesInterfaces.in',
        ],
        'action': [
          'python',
          '<(bindings_scripts_dir)/generate_event_interfaces.py',
          '--event-idl-files-list',
          '<(event_idl_files_list)',
          '--event-interfaces-file',
          '<(blink_modules_output_dir)/EventModulesInterfaces.in',
          '--write-file-only-if-changed',
          '<(write_file_only_if_changed)',
          '--suffix',
          'Modules',
        ],
      },
      {
        # GN version: //third_party/WebKit/Source/bindings/modules:bindings_modules_generated_event_modules_factory
        'action_name': 'EventModulesFactory',
        'inputs': [
          '<@(make_event_factory_files)',
          '<(blink_modules_output_dir)/EventModulesInterfaces.in',
        ],
        'outputs': [
          '<(blink_modules_output_dir)/EventModules.cpp',
          '<(blink_modules_output_dir)/EventModulesHeaders.h',
        ],
        'action': [
          'python',
          '../../build/scripts/make_event_factory.py',
          '<(blink_modules_output_dir)/EventModulesInterfaces.in',
          '--output_dir',
          '<(blink_modules_output_dir)',
        ],
      },
      {
        # GN version: //third_party/WebKit/Source/bindings/modules:bindings_modules_generated_event_modules_names
        'action_name': 'EventModulesNames',
        'inputs': [
          '<@(make_names_files)',
          '<(blink_modules_output_dir)/EventModulesInterfaces.in',
        ],
        'outputs': [
          '<(blink_modules_output_dir)/EventModulesNames.cpp',
          '<(blink_modules_output_dir)/EventModulesNames.h',
        ],
        'action': [
          'python',
          '../../build/scripts/make_names.py',
          '<(blink_modules_output_dir)/EventModulesInterfaces.in',
          '--output_dir',
          '<(blink_modules_output_dir)',
        ],
      },
      {
        # GN version: //third_party/WebKit/Source/bindings/modules:bindings_modules_generated_event_target_modules_names
        'action_name': 'EventTargetModulesNames',
        'inputs': [
          '<@(make_names_files)',
          '../../modules/EventTargetModulesFactory.in',
        ],
        'outputs': [
          '<(blink_modules_output_dir)/EventTargetModulesNames.cpp',
          '<(blink_modules_output_dir)/EventTargetModulesNames.h',
        ],
        'action': [
          'python',
          '../../build/scripts/make_names.py',
          '../../modules/EventTargetModulesFactory.in',
          '--output_dir',
          '<(blink_modules_output_dir)',
        ],
      },
    ],
  },
################################################################################
  {
    'target_name': 'modules_global_objects',
    'dependencies': [
      '../core/generated.gyp:core_global_objects',
    ],
    'variables': {
      'idl_files': '<(modules_idl_files)',
      'input_files': [
        '<(bindings_core_output_dir)/GlobalObjectsCore.pickle',
      ],
      'output_file':
        '<(bindings_modules_output_dir)/GlobalObjectsModules.pickle',
    },
    'includes': ['../../bindings/scripts/global_objects.gypi'],
  },
################################################################################
  {
    # Global constructors for global objects in modules (ServiceWorker)
    # but interfaces in core.
    'target_name': 'modules_core_global_constructors_idls',
    'dependencies': [
      'modules_global_objects',
    ],
    'variables': {
      'idl_files': [
        '<@(core_idl_files)',
        '<@(core_idl_with_modules_dependency_files)',
      ],
      'global_objects_file':
        '<(bindings_modules_output_dir)/GlobalObjectsModules.pickle',
      'global_names_idl_files': [
        'CompositorWorkerGlobalScope',
        '<(blink_modules_output_dir)/CompositorWorkerGlobalScopeCoreConstructors.idl',
        'ServiceWorkerGlobalScope',
        '<(blink_modules_output_dir)/ServiceWorkerGlobalScopeCoreConstructors.idl',
      ],
      'outputs': [
        '<@(modules_core_global_constructors_generated_idl_files)',
        '<@(modules_core_global_constructors_generated_header_files)',
      ],
    },
    'includes': ['../../bindings/scripts/global_constructors.gypi'],
  },
################################################################################
  {
    'target_name': 'modules_global_constructors_idls',
    'dependencies': [
      'modules_global_objects',
    ],
    'variables': {
      'idl_files': '<(modules_idl_files)',
      'global_objects_file':
        '<(bindings_modules_output_dir)/GlobalObjectsModules.pickle',
      'global_names_idl_files': [
        'Window',
        '<(blink_modules_output_dir)/WindowModulesConstructors.idl',
        'CompositorWorkerGlobalScope',
        '<(blink_modules_output_dir)/CompositorWorkerGlobalScopeModulesConstructors.idl',
        'DedicatedWorkerGlobalScope',
        '<(blink_modules_output_dir)/DedicatedWorkerGlobalScopeModulesConstructors.idl',
        'ServiceWorkerGlobalScope',
        '<(blink_modules_output_dir)/ServiceWorkerGlobalScopeModulesConstructors.idl',
        'SharedWorkerGlobalScope',
        '<(blink_modules_output_dir)/SharedWorkerGlobalScopeModulesConstructors.idl',
      ],
      'outputs': [
        '<@(modules_global_constructors_generated_idl_files)',
        '<@(modules_global_constructors_generated_header_files)',
      ],
    },
    'includes': ['../../bindings/scripts/global_constructors.gypi'],
  },
################################################################################
  {
    'target_name': 'interfaces_info_individual_modules',
    'dependencies': [
      '<(bindings_scripts_dir)/scripts.gyp:cached_lex_yacc_tables',
      'modules_core_global_constructors_idls',
      'modules_global_constructors_idls',
    ],
    'variables': {
      'cache_directory': '<(bindings_modules_output_dir)/../scripts',
      'static_idl_files': '<(modules_static_idl_files)',
      'generated_idl_files': '<(modules_generated_idl_files)',
      'interfaces_info_file':
        '<(bindings_modules_output_dir)/InterfacesInfoOverallIndividual.pickle',
      'component_info_file':
        '<(bindings_modules_output_dir)/ComponentInfoModules.pickle',
    },
    'includes': ['../../bindings/scripts/interfaces_info_individual.gypi'],
  },
################################################################################
  {
    # GN version: //third_party/WebKit/Source/bindings/modules:interfaces_info
    'target_name': 'interfaces_info',
    'dependencies': [
        '../core/generated.gyp:interfaces_info_individual_core',
        'interfaces_info_individual_modules',
    ],
    'variables': {
      'input_files': [
        '<(bindings_core_output_dir)/InterfacesInfoCoreIndividual.pickle',
        '<(bindings_modules_output_dir)/InterfacesInfoOverallIndividual.pickle',
      ],
      'output_file':
        '<(bindings_modules_output_dir)/InterfacesInfoOverall.pickle',
    },
    'includes': ['../../bindings/scripts/interfaces_info_overall.gypi'],
  },
################################################################################
  ],  # targets
}
