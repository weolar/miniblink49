# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
{
    'targets': [
        {
          # Depend on this target to use public blink API headers for things
          # like enums and public structures without actually linking against any Blink
          # libraries.
          'target_name': 'blink_headers',
          'type': 'none',
          'direct_dependent_settings': {
            'include_dirs': [ '..' ],
          },
        },
    ],
    'conditions': [
        ['OS=="android"', {
            'targets': [
                {
                    # gn version: //third_party/WebKit/public:blink_headers_java_enums_srcjar
                    'target_name': 'web_input_event_java',
                    'type': 'none',
                    'variables': {
                        'source_file': 'web/WebInputEvent.h',
                    },
                    'includes': [ '../../../build/android/java_cpp_enum.gypi' ],
                },
                {
                    # gn version: //third_party/WebKit/public:blink_headers_java_enums_srcjar
                    'target_name': 'web_text_input_type',
                    'type': 'none',
                    'variables': {
                        'source_file': 'web/WebTextInputType.h',
                    },
                    'includes': [ '../../../build/android/java_cpp_enum.gypi' ],
                },
                {
                    # gn version: //third_party/WebKit/public:blink_headers_java
                    'target_name': 'blink_headers_java',
                    'type': 'none',
                    'dependencies': [
                        'web_input_event_java',
                        'web_text_input_type',
                    ],
                    'variables': {
                        'java_in_dir': '../../../build/android/empty',
                    },
                    'includes': [ '../../../build/java.gypi' ],
                },
            ],
        }],
    ],
}

