# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'includes': [
    '../../modules/modules_generated.gypi',
  ],

  'variables': {
    # Global constructors
    # Global object in modules, constructors for interfaces in core
    'modules_core_global_constructors_generated_idl_files': [
      '<(blink_modules_output_dir)/CompositorWorkerGlobalScopeCoreConstructors.idl',
      '<(blink_modules_output_dir)/ServiceWorkerGlobalScopeCoreConstructors.idl',
    ],
    'modules_core_global_constructors_generated_header_files': [
      '<(blink_modules_output_dir)/CompositorWorkerGlobalScopeCoreConstructors.h',
      '<(blink_modules_output_dir)/ServiceWorkerGlobalScopeCoreConstructors.h',
    ],

    # Global object in modules, constructors for interfaces in modules
    'modules_global_constructors_generated_idl_files': [
      '<(blink_modules_output_dir)/WindowModulesConstructors.idl',
      '<(blink_modules_output_dir)/CompositorWorkerGlobalScopeModulesConstructors.idl',
      '<(blink_modules_output_dir)/SharedWorkerGlobalScopeModulesConstructors.idl',
      '<(blink_modules_output_dir)/DedicatedWorkerGlobalScopeModulesConstructors.idl',
      '<(blink_modules_output_dir)/ServiceWorkerGlobalScopeModulesConstructors.idl',
    ],
    'modules_global_constructors_generated_header_files': [
      '<(blink_modules_output_dir)/WindowModulesConstructors.h',
      '<(blink_modules_output_dir)/CompositorWorkerGlobalScopeModulesConstructors.h',
      '<(blink_modules_output_dir)/SharedWorkerGlobalScopeModulesConstructors.h',
      '<(blink_modules_output_dir)/DedicatedWorkerGlobalScopeModulesConstructors.h',
      '<(blink_modules_output_dir)/ServiceWorkerGlobalScopeModulesConstructors.h',
    ],
  },
}
