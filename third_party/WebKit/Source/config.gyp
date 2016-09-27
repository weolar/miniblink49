#
# Copyright (C) 2013 Google Inc. All rights reserved.
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
#
{
  'variables': {
    # If set to 1, doesn't compile debug symbols into webcore reducing the
    # size of the binary and increasing the speed of gdb.  gcc only.
    'remove_webcore_debug_symbols%': 0,
    'enable_oilpan%': 0,
    # If set to 1 (default) and using clang, the Blink GC plugin will check the
    # usage of the garbage-collection infrastructure during compilation.
    'blink_gc_plugin%': 1,
    # Additional flags for the Blink GC plugin.
    'blink_gc_plugin_flags%': '',
    # If set to 1, the Blink will use the base allocator instead of
    # PartitionAlloc. so that the top of stack-unwinding becomes the caller
    # which requests memory allocation in blink.
    'blink_disable_partition_allocator%': 0,
  },
  'targets': [
  {
    # GN version: //third_party/WebKit/Source:config
    #   (In GN this is a config rather than a target.)
    'target_name': 'config',
    'type': 'none',
    'direct_dependent_settings': {
      'include_dirs': [
        '.',
        '..',
      ],
      'msvs_disabled_warnings': [
        4305, 4324, 4714, 4800, 4996,
      ],
      'variables': {
        'chromium_code': 1,
        'clang_warning_flags': [ '-Wglobal-constructors' ],
      },
      'conditions': [
        ['OS=="win" and component=="shared_library"', {
          'defines': [
            'USING_V8_SHARED',
          ],
        }],
        ['OS=="win"', {
          'sources/': [
            ['exclude', 'Posix\\.cpp$'],
          ],
        },{ # OS!="win"
          'sources/': [
            ['exclude', 'Win\\.cpp$'],
          ],
        }],
        ['OS!="mac"', {
          'sources/': [
            ['exclude', 'Mac\\.mm$'],
          ],
        }],
        ['OS!="android"', {
          'sources/': [
            ['exclude', 'Android\\.cpp$'],
          ],
        }],
        ['OS!="win" and remove_webcore_debug_symbols==1', {
          # Remove -g from all targets defined here.
          'cflags!': ['-g'],
        }],
        ['gcc_version>=46', {
          # Disable warnings about c++0x compatibility, as some names (such as
          # nullptr) conflict with upcoming c++0x types.
          'cflags_cc': ['-Wno-c++0x-compat'],
        }],
        ['OS=="linux" and target_arch=="arm"', {
          # Due to a bug in gcc arm, we get warnings about uninitialized
          # timesNewRoman.unstatic.3258 and colorTransparent.unstatic.4879.
          'cflags': ['-Wno-uninitialized'],
        }],
        # Only enable the blink_gc_plugin when using clang and chrome plugins.
        ['blink_gc_plugin==1 and clang==1 and clang_use_chrome_plugins==1', {
          'cflags': ['<!@(python <(DEPTH)/tools/clang/scripts/blink_gc_plugin_flags.py enable-oilpan=<(enable_oilpan) <(blink_gc_plugin_flags))'],
          'xcode_settings': {
            'OTHER_CFLAGS': ['<!@(python <(DEPTH)/tools/clang/scripts/blink_gc_plugin_flags.py enable-oilpan=<(enable_oilpan) <(blink_gc_plugin_flags))'],
          },
          'msvs_settings': {
            'VCCLCompilerTool': {
              'AdditionalOptions': ['<!@(python <(DEPTH)/tools/clang/scripts/blink_gc_plugin_flags.py enable-oilpan=<(enable_oilpan) <(blink_gc_plugin_flags))'],
            },
          },
        }],
        ['blink_disable_partition_allocator==1', {
          'defines': [
            'MEMORY_TOOL_REPLACES_ALLOCATOR',
          ],
        }],
        ['use_system_icu==1', {
          'defines': [
            'USING_SYSTEM_ICU',
          ],
        }],
      ],
    },
  },
  {
    'target_name': 'unittest_config',
    'type': 'none',
    'dependencies': [
      'config',
      '<(DEPTH)/testing/gmock.gyp:gmock',
      '<(DEPTH)/testing/gtest.gyp:gtest',
    ],
    'export_dependent_settings': [
      'config',
      '<(DEPTH)/testing/gmock.gyp:gmock',
      '<(DEPTH)/testing/gtest.gyp:gtest',
    ],
    'direct_dependent_settings': {
      'variables': {
        'chromium_code': 1,
        'clang_warning_flags_unset': [ '-Wglobal-constructors' ],
      },
    },
  }
  ],
}
