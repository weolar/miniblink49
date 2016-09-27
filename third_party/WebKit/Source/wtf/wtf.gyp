# Copyright (C) 2012 Google Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
{
  'includes': [
    '../build/win/precompile.gypi',
    '../build/features.gypi',
    'wtf.gypi',
  ],
  'conditions': [
    ['gcc_version>=46', {
      'target_defaults': {
        # Disable warnings about c++0x compatibility, as some names (such as nullptr) conflict
        # with upcoming c++0x types.
        'cflags_cc': ['-Wno-c++0x-compat'],
      },
    }],
  ],
  'targets': [
    {
      # This target sets up defines and includes that are required by WTF and
      # its dependents.
      'target_name': 'wtf_config',
      'type': 'none',
      'direct_dependent_settings': {
        'defines': [
          # Import features_defines from features.gypi
          '<@(feature_defines)',
        ],
        'conditions': [
          ['OS=="win"', {
            'defines': [
              '__STD_C',
              '_CRT_SECURE_NO_DEPRECATE',
              '_SCL_SECURE_NO_DEPRECATE',
            ],
          }],
        ],
      },
    },
    {
      'target_name': 'wtf',
      'type': '<(component)',
      'include_dirs': [
        '..',
      ],
      'dependencies': [
          'wtf_config',
          '../config.gyp:config',
          '<(DEPTH)/third_party/icu/icu.gyp:icui18n',
          '<(DEPTH)/third_party/icu/icu.gyp:icuuc',
      ],
      'sources': [
        '<@(wtf_files)',
      ],
      'defines': [
        'WTF_IMPLEMENTATION=1',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '..',
        ],
        # Some warnings occur in WTF headers, so they must also be disabled
        # in targets that use WTF.
        'msvs_disabled_warnings': [
          # Don't complain about calling specific versions of templatized
          # functions (e.g. in RefPtrHashMap.h).
          4344,
          # Don't complain about using "this" in an initializer list
          # (e.g. in StringImpl.h).
          4355,
          # Disable c4267 warnings until we fix size_t to int truncations.
          4267,
        ],
      },
      'export_dependent_settings': [
        'wtf_config',
        '<(DEPTH)/third_party/icu/icu.gyp:icui18n',
        '<(DEPTH)/third_party/icu/icu.gyp:icuuc',
      ],
      # Disable c4267 warnings until we fix size_t to int truncations.
      'msvs_disabled_warnings': [4127, 4355, 4510, 4512, 4610, 4706, 4068, 4267],
      'conditions': [
        ['OS=="android"', {
          'link_settings': { 'libraries': [ '-llog' ] },
        }],
        ['OS=="linux"', {
          'link_settings': { 'libraries': [ '-ldl' ] },
        }],
        ['OS=="win"', {
          'sources/': [
            ['exclude', 'ThreadingPthreads\\.cpp$'],
          ],
          'include_dirs!': [
            '<(SHARED_INTERMEDIATE_DIR)/blink',
          ],
          'conditions': [
            ['component=="shared_library"', {
              # Chromium windows multi-dll build enables C++ exception and this
              # causes wtf to generate 4291 warning due to operator new/delete
              # implementations. Disable the warning for chromium windows
              # multi-dll build.
              'msvs_disabled_warnings': [4291],
              'direct_dependent_settings': {
                'msvs_disabled_warnings': [4291],
              },
            }],
          ],
        }, { # OS!="win"
          'sources/': [
            ['exclude', 'Win\\.cpp$'],
          ],
        }],
        ['OS=="mac"', {
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework',
              '$(SDKROOT)/System/Library/Frameworks/Foundation.framework',
            ]
          }
        }, { # OS!="mac"
          'sources/': [
            ['exclude', 'CF\\.cpp$'],
            ['exclude', 'Mac\\.mm$'],
            # mac is the only OS that uses WebKit's copy of TCMalloc.
            ['exclude', 'TC.*\\.(cpp|h)$'],
          ],
        }],
      ],
    },
  ]
}
