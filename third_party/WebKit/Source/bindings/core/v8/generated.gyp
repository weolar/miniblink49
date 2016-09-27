# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# Generate IDL bindings for core, plus aggregate bindings files.
#
# Design doc: http://www.chromium.org/developers/design-documents/idl-build

{
  'includes': [
    # ../../.. == Source
    '../../../bindings/bindings.gypi',
    '../../../bindings/core/core.gypi',
    '../../../bindings/core/generated.gypi',
    '../../../bindings/core/idl.gypi',
    # FIXME: need info about modules IDL files because some core IDL files
    # depend on modules IDL files  http://crbug.com/358074
    '../../../bindings/modules/idl.gypi',
    '../../../bindings/modules/modules.gypi',
    '../../../bindings/scripts/scripts.gypi',
    '../../../core/core.gypi',
    'generated.gypi',
  ],

  'targets': [
################################################################################
  {
    # GN version: //third_party/WebKit/Source/bindings/core/v8:bindings_core_v8_generated_individual
    'target_name': 'bindings_core_v8_generated_individual',
    'type': 'none',
    # The 'binding' rule generates .h files, so mark as hard_dependency, per:
    # https://code.google.com/p/gyp/wiki/InputFormatReference#Linking_Dependencies
    'hard_dependency': 1,
    'dependencies': [
      '../../../core/core_generated.gyp:generated_testing_idls',
      '../generated.gyp:core_global_constructors_idls',
      # FIXME: should not depend on modules, but partial interface definitions
      # in modules change bindings for core http://crbug.com/358074
      '../../modules/generated.gyp:modules_global_constructors_idls',
      '<(bindings_scripts_dir)/scripts.gyp:cached_jinja_templates',
      '<(bindings_scripts_dir)/scripts.gyp:cached_lex_yacc_tables',
      # FIXME: should be interfaces_info_core (w/o modules)
      # http://crbug.com/358074
      '../../modules/generated.gyp:interfaces_info',
    ],
    'sources': [
      '<@(core_interface_idl_files)',
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
        '<(bindings_core_output_dir)/ComponentInfoCore.pickle',
        # Further, if any dependency (partial interface or implemented
        # interface) changes, rebuild everything, since every IDL potentially
        # depends on them, because we're not computing dependencies
        # file-by-file.
        # FIXME: This is too conservative, and causes excess rebuilds:
        # compute this file-by-file.  http://crbug.com/341748
        # FIXME: should be core_all_dependency_idl_files only, but some core IDL
        # files depend on modules IDL files  http://crbug.com/358074
        '<@(all_dependency_idl_files)',
      ],
      'outputs': [
        '<(bindings_core_v8_output_dir)/V8<(RULE_INPUT_ROOT).cpp',
        '<(bindings_core_v8_output_dir)/V8<(RULE_INPUT_ROOT).h',
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
        '<(bindings_core_v8_output_dir)',
        '--info-dir',
        '<(bindings_output_dir)',
        '--target-component',
        'core',
        '--write-file-only-if-changed',
        '<(write_file_only_if_changed)',
        '<(RULE_INPUT_PATH)',
      ],
      'message': 'Generating binding from <(RULE_INPUT_PATH)',
    }],
  },
################################################################################
  {
    # GN version: //third_party/WebKit/Source/bindings/core/v8:bindings_core_v8_generated_aggregate
    'target_name': 'bindings_core_v8_generated_aggregate',
    'type': 'none',
    'actions': [{
      'action_name': 'generate_aggregate_bindings_core_v8',
      'inputs': [
        '<(bindings_scripts_dir)/aggregate_generated_bindings.py',
        '<(core_idl_files_list)',
      ],
      'outputs': [
        '<@(bindings_core_v8_generated_aggregate_files)',
      ],
      'action': [
        'python',
        '<(bindings_scripts_dir)/aggregate_generated_bindings.py',
        'core',
        '<(core_idl_files_list)',
        '--',
        '<@(bindings_core_v8_generated_aggregate_files)',
      ],
      'message': 'Generating aggregate generated core V8 bindings files',
    }],
  },
################################################################################
  {
    # GN version: //third_party/WebKit/Source/bindings/core/v8:bindings_core_impl_generated
    # http://crbug.com/358074; See comments on
    # 'bindings_core_v8_generated_individual' target
    'target_name': 'bindings_core_impl_generated',
    'type': 'none',
    'hard_dependency': 1,
    'dependencies': [
      '<(bindings_scripts_dir)/scripts.gyp:cached_jinja_templates',
      '<(bindings_scripts_dir)/scripts.gyp:cached_lex_yacc_tables',
      '../../core/generated.gyp:interfaces_info_individual_core',
      '../../modules/generated.gyp:interfaces_info',
    ],
    'sources': [
      '<@(core_dictionary_idl_files)',
      '<@(core_testing_dictionary_idl_files)',
    ],
    'actions': [{
      'action_name': 'idl_dictionary',
      # Mark as explicit idl action to prevent MSVS emulation on Windows.
      'explicit_idl_action': 1,
      'msvs_cygwin_shell': 0,
      'inputs': [
        '<@(core_dictionary_idl_files)',
        '<@(core_testing_dictionary_idl_files)',
        '<@(idl_lexer_parser_files)',
        '<@(idl_cache_files)',
        '<@(idl_compiler_files)',
        '<(bindings_dir)/IDLExtendedAttributes.txt',
        '<(bindings_modules_output_dir)/InterfacesInfoOverall.pickle',
        '<(bindings_core_output_dir)/ComponentInfoCore.pickle',
      ],
      'outputs': [
        '<@(bindings_core_v8_generated_union_type_files)',
        '<@(generated_core_dictionary_files)',
        '<@(generated_core_testing_dictionary_files)',
      ],
      'action': [
        'python',
        '<(bindings_scripts_dir)/idl_compiler.py',
        '--cache-dir',
        '<(bindings_scripts_output_dir)',
        '--output-dir',
        '<(bindings_core_v8_output_dir)',
        '--impl-output-dir',
        '<(SHARED_INTERMEDIATE_DIR)/blink/',
        '--info-dir',
        '<(bindings_output_dir)',
        '--target-component',
        'core',
        '--write-file-only-if-changed',
        '<(write_file_only_if_changed)',
        '--generate-impl',
        '<(core_dictionary_idl_files_list)',
      ],
      'message': 'Generating core IDL dictionary impl classes',
    }],
  },
################################################################################
  {
    # GN version: //third_party/WebKit/Source/bindings/core/v8:bindings_core_v8_generated
    'target_name': 'bindings_core_v8_generated',
    'type': 'none',
    'dependencies': [
      'bindings_core_impl_generated',
      'bindings_core_v8_generated_aggregate',
      'bindings_core_v8_generated_individual',
    ],
  },
################################################################################
  ],  # targets
}
