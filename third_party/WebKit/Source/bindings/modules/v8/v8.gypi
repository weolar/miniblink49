# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
    'includes': [
        'custom/custom.gypi',
    ],
    'variables': {
        # Note: list duplicated in v8.gni.
        'bindings_modules_v8_files': [
            '<@(bindings_modules_v8_custom_files)',
            'DictionaryHelperForModules.cpp',
            'ModuleBindingsInitializer.cpp',
            'ModuleBindingsInitializer.h',
            'ScriptValueSerializerForModules.cpp',
            'ScriptValueSerializerForModules.h',
            'SerializedScriptValueForModulesFactory.cpp',
            'SerializedScriptValueForModulesFactory.h',
            'ToV8ForModules.h',
            'V8BindingForModules.cpp',
            'V8BindingForModules.h',
            'WebGLAny.cpp',
            'WebGLAny.h',
        ],
        # Note: list duplicated in v8.gni.
        'bindings_modules_v8_unittest_files': [
            'V8BindingForModulesTest.cpp',
        ],
    },
}
