# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'includes': [
    '../../core/core_generated.gypi',
  ],

  'variables': {
    # Global constructors
    'core_global_constructors_generated_idl_files': [
      '<(blink_core_output_dir)/WindowCoreConstructors.idl',
      '<(blink_core_output_dir)/SharedWorkerGlobalScopeCoreConstructors.idl',
      '<(blink_core_output_dir)/DedicatedWorkerGlobalScopeCoreConstructors.idl',
    ],

    'core_global_constructors_generated_header_files': [
      '<(blink_core_output_dir)/WindowCoreConstructors.h',
      '<(blink_core_output_dir)/SharedWorkerGlobalScopeCoreConstructors.h',
      '<(blink_core_output_dir)/DedicatedWorkerGlobalScopeCoreConstructors.h',
    ],
  },
}
