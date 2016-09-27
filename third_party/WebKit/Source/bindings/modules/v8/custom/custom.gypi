# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
    'variables': {
        # Note: list duplicated in custom.gni.
        'bindings_modules_v8_custom_files': [
            'V8CustomSQLStatementErrorCallback.cpp',
            'V8DeviceMotionEventCustom.cpp',
            'V8ServiceWorkerMessageEventCustom.cpp',
        ],
    },
}
