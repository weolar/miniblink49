# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# This gypi file handles the removal of platform-specific files from the
# Skia build.
{
  'includes': [
    # skia_for_chromium_defines.gypi defines skia_for_chromium_defines
    '../third_party/skia/gyp/skia_for_chromium_defines.gypi',
  ],

  'include_dirs': [
    '..',
    'config',
  ],

  'conditions': [
    [ 'OS != "android"', {
      'sources/': [
         ['exclude', '_android\\.(cc|cpp)$'],
      ],
    }],
    [ 'OS == "android"', {
      'defines': [
        'SK_FONTHOST_DOES_NOT_USE_FONTMGR',
      ],
    }],
    [ 'OS != "ios"', {
      'sources/': [
         ['exclude', '_ios\\.(cc|cpp|mm?)$'],
      ],
    }],
    [ 'OS == "ios"', {
      'defines': [
        'SK_BUILD_FOR_IOS',
      ],
    }],
    [ 'OS != "mac"', {
      'sources/': [
        ['exclude', '_mac\\.(cc|cpp|mm?)$'],
      ],
    }],
    [ 'OS == "mac"', {
      'defines': [
        'SK_BUILD_FOR_MAC',
      ],
    }],
    [ 'OS != "win"', {
      'sources/': [ ['exclude', '_win\\.(cc|cpp)$'] ],
    }],
    [ 'OS == "win"', {
      'defines': [
        # On windows, GDI handles are a scarse system-wide resource so we have to keep
        # the glyph cache, which holds up to 4 GDI handles per entry, to a fairly small
        # size.
        # http://crbug.com/314387
        'SK_DEFAULT_FONT_CACHE_COUNT_LIMIT=256',
      ],
    }],
    [ 'desktop_linux == 0 and chromeos == 0', {
      'sources/': [ ['exclude', '_linux\\.(cc|cpp)$'] ],
    }],
    [ 'use_cairo == 0', {
      'sources/': [ ['exclude', '_cairo\\.(cc|cpp)$'] ],
    }],

    #Settings for text blitting, chosen to approximate the system browser.
    [ 'OS == "linux"', {
      'defines': [
        'SK_GAMMA_EXPONENT=1.2',
        'SK_GAMMA_CONTRAST=0.2',
        'SK_HIGH_QUALITY_IS_LANCZOS',
      ],
    }],
    ['OS == "android"', {
      'defines': [
        'SK_GAMMA_APPLY_TO_A8',
        'SK_GAMMA_EXPONENT=1.4',
        'SK_GAMMA_CONTRAST=0.0',
      ],
      'android_enable_fdo': 1,
    }],
    ['OS == "win"', {
      'defines': [
        'SK_GAMMA_SRGB',
        'SK_GAMMA_CONTRAST=0.5',
        'SK_HIGH_QUALITY_IS_LANCZOS',
      ],
    }],
    ['OS == "mac"', {
      'defines': [
        'SK_GAMMA_SRGB',
        'SK_GAMMA_CONTRAST=0.0',
        'SK_HIGH_QUALITY_IS_LANCZOS',
      ],
    }],

    # For POSIX platforms, prefer the Mutex implementation provided by Skia
    # since it does not generate static initializers.
    [ 'os_posix == 1', {
      'defines+': [
        'SK_USE_POSIX_THREADS',
      ],
      'direct_dependent_settings': {
        'defines': [
          'SK_USE_POSIX_THREADS',
        ],
      },
    }],

    # Neon support.
    [ 'target_arch == "arm" and arm_version >= 7 and arm_neon == 1', {
      'defines': [
        '__ARM_HAVE_NEON',
      ],
    }],
    [ 'target_arch == "arm" and arm_version >= 7 and arm_neon_optional == 1', {
      'defines': [
        '__ARM_HAVE_OPTIONAL_NEON_SUPPORT',
      ],
    }],
  ],

  'variables': {
    'variables': {
      'conditions': [
        ['OS== "ios"', {
          'skia_support_gpu': 0,
        }, {
          'skia_support_gpu': 1,
        }],
        ['OS=="ios" or enable_printing == 0', {
          'skia_support_pdf': 0,
        }, {
          'skia_support_pdf': 1,
        }],
      ],
    },
    'skia_support_gpu': '<(skia_support_gpu)',
    'skia_support_pdf': '<(skia_support_pdf)',

    # These two set the paths so we can include skia/gyp/core.gypi
    'skia_src_path': '../third_party/skia/src',
    'skia_include_path': '../third_party/skia/include',

    # This list will contain all defines that also need to be exported to
    # dependent components.
    'skia_export_defines': [
      'SK_ENABLE_INST_COUNT=0',
      'SK_SUPPORT_GPU=<(skia_support_gpu)',
      'GR_GL_CUSTOM_SETUP_HEADER="GrGLConfig_chrome.h"',
      'SK_ENABLE_LEGACY_API_ALIASING=1',
      'SK_ATTR_DEPRECATED=SK_NOTHING_ARG1',
      'GR_GL_IGNORE_ES3_MSAA=0',
      'SK_WILL_NEVER_DRAW_PERSPECTIVE_TEXT',

      # This variable contains additional defines, specified in skia's
      # skia_for_chromium_defines.gypi file.
      '<@(skia_for_chromium_defines)',
      'SK_SUPPORT_LEGACY_GETTOTALCLIP',
    ],

    'default_font_cache_limit%': '(20*1024*1024)',

    'conditions': [
      ['OS== "android"', {
        # Android devices are typically more memory constrained, so
        # default to a smaller glyph cache (it may be overriden at runtime
        # when the renderer starts up, depending on the actual device memory).
        'default_font_cache_limit': '(1*1024*1024)',
        'skia_export_defines': [
          'SK_BUILD_FOR_ANDROID',
        ],
      }],
    ],
  },

  'defines': [
    '<@(skia_export_defines)',

    # skia uses static initializers to initialize the serialization logic
    # of its "pictures" library. This is currently not used in chrome; if
    # it ever gets used the processes that use it need to call
    # SkGraphics::Init().
    'SK_ALLOW_STATIC_GLOBAL_INITIALIZERS=0',

    # Forcing the unoptimized path for the offset image filter in skia until
    # all filters used in Blink support the optimized path properly
    'SK_DISABLE_OFFSETIMAGEFILTER_OPTIMIZATION',

    # Disable this check because it is too strict for some Chromium-specific
    # subclasses of SkPixelRef. See bug: crbug.com/171776.
    'SK_DISABLE_PIXELREF_LOCKCOUNT_BALANCE_CHECK',

    'IGNORE_ROT_AA_RECT_OPT',

    'SK_IGNORE_BLURRED_RRECT_OPT',

    'SK_IGNORE_QUAD_RR_CORNERS_OPT',

    # this flag forces Skia not to use typographic metrics with GDI.
    'SK_GDI_ALWAYS_USE_TEXTMETRICS_FOR_FONT_METRICS',

    'SK_DEFAULT_FONT_CACHE_LIMIT=<(default_font_cache_limit)',

    'SK_USE_DISCARDABLE_SCALEDIMAGECACHE',
  ],

  'direct_dependent_settings': {
    'defines': [
      '<@(skia_export_defines)',
    ],
  },

  # We would prefer this to be direct_dependent_settings,
  # however we currently have no means to enforce that direct dependents
  # re-export if they include Skia headers in their public headers.
  'all_dependent_settings': {
    'include_dirs': [
      '..',
      'config',
    ],
  },

  'msvs_disabled_warnings': [4244, 4267, 4341, 4345, 4390, 4554, 4748, 4800],
}
