# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# Generate IDL bindings for modules, plus aggregate bindings files.
#
# Design doc: http://www.chromium.org/developers/design-documents/idl-build

{
  'includes': [
    # ../../.. == Source
    '../../../bindings/bindings.gypi',
    '../../../bindings/modules/idl.gypi',
    '../../../bindings/modules/modules.gypi',
    '../../../bindings/scripts/scripts.gypi',
    # Need to know core idl files list to generate parital interfaces
    # defined in modules.
    '../../../core/core.gypi',
    '../../../modules/modules.gypi',
    'generated.gypi',
  ],

  'targets': [
################################################################################
  {
    'target_name': 'bindings_modules_v8_generated_individual',
    'type': 'none',
    # The 'binding' rule generates .h files, so mark as hard_dependency, per:
    # https://code.google.com/p/gyp/wiki/InputFormatReference#Linking_Dependencies
    'hard_dependency': 1,
    'dependencies': [
      '../../core/generated.gyp:core_global_constructors_idls',
      '../generated.gyp:modules_global_constructors_idls',
      '../generated.gyp:interfaces_info',
      '<(bindings_scripts_dir)/scripts.gyp:cached_jinja_templates',
      '<(bindings_scripts_dir)/scripts.gyp:cached_lex_yacc_tables',
    ],
    'sources': [
      '<@(modules_interface_idl_files)',
    ],
    'rules': [{
      'rule_name': 'binding',
      'extension': 'idl',
      'msvs_external_rule': 1,
      'inputs': [
        '<@(idl_lexer_parser_files)',  # to be explicit (covered by parsetab)
        '<@(idl_cache_files)',
        '<@(idl_compiler_files)',
        '<(bindings_dir)/IDLExtendedAttributes.txt',
        # If the dependency structure or public interface info (e.g.,
        # [ImplementedAs]) changes, we rebuild all files, since we're not
        # computing dependencies file-by-file in the build.
        # This data is generally stable.
        '<(bindings_modules_output_dir)/InterfacesInfoOverall.pickle',
        '<(bindings_modules_output_dir)/ComponentInfoModules.pickle',
        # Further, if any dependency (partial interface or implemented
        # interface) changes, rebuild everything, since every IDL potentially
        # depends on them, because we're not computing dependencies
        # file-by-file.
        # FIXME: This is too conservative, and causes excess rebuilds:
        # compute this file-by-file.  http://crbug.com/341748
        '<@(all_dependency_idl_files)',
      ],
      'outputs': [
        '<(bindings_modules_v8_output_dir)/V8<(RULE_INPUT_ROOT).cpp',
        '<(bindings_modules_v8_output_dir)/V8<(RULE_INPUT_ROOT).h',
      ],
      # sanitize-win-build-log.sed uses a regex which matches this command
      # line (Python script + .idl file being processed).
      # Update that regex if command line changes (other than changing flags)
      'action': [
        'python',
        '<(bindings_scripts_dir)/idl_compiler.py',
        '--cache-dir',
        '<(bindings_scripts_output_dir)',
        '--output-dir',
        '<(bindings_modules_v8_output_dir)',
        '--info-dir',
        '<(bindings_output_dir)',
        '--target-component',
        'modules',
        '--write-file-only-if-changed',
        '<(write_file_only_if_changed)',
        '<(RULE_INPUT_PATH)',
      ],
      'message': 'Generating binding from <(RULE_INPUT_PATH)',
    }],
  },
################################################################################
  {
    'target_name': 'bindings_modules_v8_generated_aggregate',
    'type': 'none',
    'actions': [{
      'action_name': 'generate_aggregate_bindings_modules_v8',
      'inputs': [
        '<(bindings_scripts_dir)/aggregate_generated_bindings.py',
        '<(modules_idl_files_list)',
      ],
      'outputs': [
        '<@(bindings_modules_v8_generated_aggregate_files)',
      ],
      'action': [
        'python',
        '<(bindings_scripts_dir)/aggregate_generated_bindings.py',
        'modules',
        '<(modules_idl_files_list)',
        '--',
        '<@(bindings_modules_v8_generated_aggregate_files)',
      ],
      'message': 'Generating aggregate generated modules V8 bindings files',
    }],
  },
################################################################################
  {
    # GN version: //third_party/WebKit/Source/bindings/modules/v8:bindings_modules_impl_generated
    # http://crbug.com/358074; See comments on
    # 'bindings_core_v8_generated_individual' target
    'target_name': 'bindings_modules_impl_generated',
    'type': 'none',
    'hard_dependency': 1,
    'dependencies': [
      '<(bindings_scripts_dir)/scripts.gyp:cached_jinja_templates',
      '<(bindings_scripts_dir)/scripts.gyp:cached_lex_yacc_tables',
      '../../modules/generated.gyp:interfaces_info',
      '../../modules/generated.gyp:interfaces_info_individual_modules',
    ],
    'sources': [
      '<@(modules_dictionary_idl_files)',
    ],
    'actions': [{
      'action_name': 'idl_dictionary',
      # See comment on bindings_core_impl_generated
      'explicit_idl_action': 1,
      'msvs_cygwin_shell': 0,
      'inputs': [
        '<@(modules_dictionary_idl_files)',
        '<@(idl_lexer_parser_files)',
        '<@(idl_cache_files)',
        '<@(idl_compiler_files)',
        '<(bindings_dir)/IDLExtendedAttributes.txt',
        '<(bindings_modules_output_dir)/InterfacesInfoOverall.pickle',
        '<(bindings_modules_output_dir)/ComponentInfoModules.pickle',
      ],
      'outputs': [
        '<@(bindings_modules_v8_generated_union_type_files)',
        '<@(generated_modules_dictionary_files)',
      ],
      'action': [
        'python',
        '<(bindings_scripts_dir)/idl_compiler.py',
        '--cache-dir',
        '<(bindings_scripts_output_dir)',
        '--output-dir',
        '<(bindings_modules_v8_output_dir)',
        '--impl-output-dir',
        '<(SHARED_INTERMEDIATE_DIR)/blink/',
        '--info-dir',
        '<(bindings_output_dir)',
        '--target-component',
        'modules',
        '--write-file-only-if-changed',
        '<(write_file_only_if_changed)',
        '--generate-impl',
        '<(modules_dictionary_idl_files_list)',
      ],
      'message': 'Generating modules IDL dictionary impl classes',
    }],
  },
################################################################################
  {
    'target_name': 'bindings_modules_v8_generated_partial_individual',
    'type': 'none',
    # The 'partial_binding' rule generates .h files, so mark as hard_dependency, per:
    # https://code.google.com/p/gyp/wiki/InputFormatReference#Linking_Dependencies
    'hard_dependency': 1,
    'dependencies': [
      '../../core/generated.gyp:core_global_constructors_idls',
      '../generated.gyp:modules_global_constructors_idls',
      '../generated.gyp:interfaces_info',
      '<(bindings_scripts_dir)/scripts.gyp:cached_jinja_templates',
      '<(bindings_scripts_dir)/scripts.gyp:cached_lex_yacc_tables',
    ],
    # We need to generate partial interface code for normal modules and also for testing.
    # i.e. parital interface Internals.
    'sources': [
      '<@(core_idl_with_modules_dependency_files)',
      '<@(webcore_testing_idl_with_modules_dependency_files)',
    ],
    'rules': [{
      'rule_name': 'partial_binding',
      'extension': 'idl',
      'msvs_external_rule': 1,
      'inputs': [
        '<@(idl_lexer_parser_files)',  # to be explicit (covered by parsetab)
        '<@(idl_compiler_files)',
        '<(bindings_scripts_output_dir)/lextab.py',
        '<(bindings_scripts_output_dir)/parsetab.pickle',
        '<(bindings_scripts_output_dir)/cached_jinja_templates.stamp',
        '<(bindings_dir)/IDLExtendedAttributes.txt',
        # If the dependency structure or public interface info (e.g.,
        # [ImplementedAs]) changes, we rebuild all files, since we're not
        # computing dependencies file-by-file in the build.
        # This data is generally stable.
        '<(bindings_modules_output_dir)/InterfacesInfoOverall.pickle',
        '<(bindings_modules_output_dir)/ComponentInfoModules.pickle',
        # Further, if any dependency (partial interface or implemented
        # interface) changes, rebuild everything, since every IDL potentially
        # depends on them, because we're not computing dependencies
        # file-by-file.
        # FIXME: This is too conservative, and causes excess rebuilds:
        # compute this file-by-file.  http://crbug.com/341748
        '<@(all_dependency_idl_files)',
        '<@(webcore_testing_idl_files)',
      ],
      'outputs': [
        '<(bindings_modules_v8_output_dir)/V8<(RULE_INPUT_ROOT)Partial.cpp',
        '<(bindings_modules_v8_output_dir)/V8<(RULE_INPUT_ROOT)Partial.h',
      ],
      # sanitize-win-build-log.sed uses a regex which matches this command
      # line (Python script + .idl file being processed).
      # Update that regex if command line changes (other than changing flags)
      'action': [
        'python',
        '<(bindings_scripts_dir)/idl_compiler.py',
        '--cache-dir',
        '<(bindings_scripts_output_dir)',
        '--output-dir',
        '<(bindings_modules_v8_output_dir)',
        '--info-dir',
        '<(bindings_output_dir)',
        '--target-component',
        'modules',
        '--write-file-only-if-changed',
        '<(write_file_only_if_changed)',
        '<(RULE_INPUT_PATH)',
      ],
      'message': 'Generating partial binding from <(RULE_INPUT_PATH)',
    }],
  },
################################################################################
  {
    'target_name': 'bindings_modules_v8_generated_partial_aggregate',
    'type': 'none',
    'actions': [{
      'action_name': 'generate_aggregate_bindings_modules_v8_partial',
      'inputs': [
        '<(bindings_scripts_dir)/aggregate_generated_bindings.py',
        '<(core_idl_with_modules_dependency_files_list)',
      ],
      'outputs': [
        '<@(bindings_modules_v8_generated_partial_aggregate_files)',
      ],
      'action': [
        'python',
        '<(bindings_scripts_dir)/aggregate_generated_bindings.py',
        'modules',
        '<(core_idl_with_modules_dependency_files_list)',
        '--',
        '<@(bindings_modules_v8_generated_partial_aggregate_files)',
      ],
      'message': 'Generating aggregate generated modules V8 partial bindings files',
    }],
  },
################################################################################
  {
    'target_name': 'bindings_modules_v8_generated_init_partial',
    'type': 'none',
    'actions': [{
      'action_name': 'generate_bindings_modules_v8_init_partial',
      'inputs': [
        '<(bindings_scripts_dir)/generate_init_partial_interfaces.py',
        '<(core_idl_with_modules_dependency_files_list)',
        '<(bindings_modules_output_dir)/InterfacesInfoOverall.pickle',
      ],
      'outputs': [
        '<(bindings_modules_v8_output_dir)/initPartialInterfacesInModules.cpp',
      ],
      'action': [
        'python',
        '<(bindings_scripts_dir)/generate_init_partial_interfaces.py',
        '--idl-files-list',
        '<(core_idl_with_modules_dependency_files_list)',
        '--output',
        '<(bindings_modules_v8_output_dir)/initPartialInterfacesInModules.cpp',
        '--write-file-only-if-changed',
        '<(write_file_only_if_changed)',
      ],
    }],
  },
################################################################################
  {
    'target_name': 'bindings_modules_v8_generated',
    'type': 'none',
    'dependencies': [
      'bindings_modules_impl_generated',
      'bindings_modules_v8_generated_aggregate',
      'bindings_modules_v8_generated_individual',
      'bindings_modules_v8_generated_init_partial',
      'bindings_modules_v8_generated_partial_aggregate',
      'bindings_modules_v8_generated_partial_individual',
    ],
  },
################################################################################
  ],  # targets
}
