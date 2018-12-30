# Copyright (c) 2009 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'conditions': [
      ['os_posix == 1 and OS != "mac"', {
        'os_include': 'linux'
      }],
      ['OS=="mac"', {'os_include': 'mac'}],
      ['OS=="win"', {'os_include': 'win32'}],
    ],
    # We used to have a separate flag for using the system
    # libxslt, but it seems mixing Chrome libxml and system
    # libxslt causes crashes that nobody has had time to diagnose.
    # So just put them both behind the same flag for now.
    'use_system_libxml%': 0,
  },
  'targets': [
    {
      'target_name': 'libxslt',
      'conditions': [
        ['os_posix == 1 and OS != "mac" and use_system_libxml', {
          'type': 'none',
          'direct_dependent_settings': {
            'cflags': [
              '<!@(pkg-config --cflags libxslt)',
            ],
          },
          'link_settings': {
            'ldflags': [
              '<!@(pkg-config --libs-only-L --libs-only-other libxslt)',
            ],
            'libraries': [
              '<!@(pkg-config --libs-only-l libxslt)',
            ],
          },
        }, { # else: os_posix != 1 or OS == "mac" or ! use_system_libxml
          'type': 'static_library',
          'sources': [
            'libxslt/attributes.c',
            'libxslt/attributes.h',
            'libxslt/attrvt.c',
            'libxslt/documents.c',
            'libxslt/documents.h',
            'libxslt/extensions.c',
            'libxslt/extensions.h',
            'libxslt/extra.c',
            'libxslt/extra.h',
            'libxslt/functions.c',
            'libxslt/functions.h',
            'libxslt/imports.c',
            'libxslt/imports.h',
            'libxslt/keys.c',
            'libxslt/keys.h',
            'libxslt/libxslt.h',
            'libxslt/namespaces.c',
            'libxslt/namespaces.h',
            'libxslt/numbers.c',
            'libxslt/numbersInternals.h',
            'libxslt/pattern.c',
            'libxslt/pattern.h',
            'libxslt/preproc.c',
            'libxslt/preproc.h',
            'libxslt/security.c',
            'libxslt/security.h',
            'libxslt/templates.c',
            'libxslt/templates.h',
            'libxslt/transform.c',
            'libxslt/transform.h',
            'libxslt/trio.h',
            'libxslt/triodef.h',
            'libxslt/variables.c',
            'libxslt/variables.h',
            'libxslt/win32config.h',
            'libxslt/xslt.c',
            'libxslt/xslt.h',
            'libxslt/xsltconfig.h',
            'libxslt/xsltexports.h',
            'libxslt/xsltInternals.h',
            'libxslt/xsltlocale.c',
            'libxslt/xsltlocale.h',
            'libxslt/xsltutils.c',
            'libxslt/xsltutils.h',
            'libxslt/xsltwin32config.h',
            'linux/config.h',
            'mac/config.h',
            # TODO(port): Need a pregenerated win32/config.h?
          ],
          'defines': [
            'LIBXSLT_STATIC',
          ],
          'include_dirs': [
            '<(os_include)',
            '.',
          ],
          'dependencies': [
            '../libxml/libxml.gyp:libxml',
          ],
          'direct_dependent_settings': {
            'defines': [
              'LIBXSLT_STATIC',
            ],
            'include_dirs': [
              '.',
            ],
          },
          'msvs_disabled_warnings': [
            # size_t to int conversion.
            4267,
          ],
          'variables': {
            'clang_warning_flags': [
              # libxslt stores a char[3] in a `const unsigned char*`.
              '-Wno-pointer-sign',
              # xsltDefaultRegion and xsltCalibrateTimestamps are only
              # used with certain preprocessor defines set.
              '-Wno-unused-function',
            ],
          },
          'conditions': [
            ['OS!="win"', {'product_name': 'xslt'}],
          ],
        }],
      ],
    },
  ],
}
