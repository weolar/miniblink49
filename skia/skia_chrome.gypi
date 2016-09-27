# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.


# This gypi file contains all the Chrome-specific enhancements to Skia.
# In component mode (shared_lib) it is folded into a single shared library with
# the Skia files but in all other cases it is a separate library.
{
  'dependencies': [
    'skia_library',
    'skia_chrome_opts',
    '../base/base.gyp:base',
    '../base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
  ],

  'direct_dependent_settings': {
    'include_dirs': [
      'ext',
    ],
  },

  'sources': [
    'ext/analysis_canvas.cc',
    'ext/analysis_canvas.h',
    'ext/benchmarking_canvas.cc',
    'ext/benchmarking_canvas.h',
    'ext/bitmap_platform_device.h',
    'ext/bitmap_platform_device_cairo.cc',
    'ext/bitmap_platform_device_cairo.h',
    'ext/bitmap_platform_device_mac.cc',
    'ext/bitmap_platform_device_mac.h',
    'ext/bitmap_platform_device_skia.cc',
    'ext/bitmap_platform_device_skia.h',
    'ext/bitmap_platform_device_win.cc',
    'ext/bitmap_platform_device_win.h',
    'ext/convolver.cc',
    'ext/convolver.h',
    'ext/event_tracer_impl.cc',
    'ext/event_tracer_impl.h',
    'ext/fontmgr_default_win.cc',
    'ext/fontmgr_default_win.h',
    'ext/google_logging.cc',
    'ext/image_operations.cc',
    'ext/image_operations.h',
    'ext/lazy_pixel_ref.cc',
    'ext/lazy_pixel_ref.h',
    'ext/opacity_draw_filter.cc',
    'ext/opacity_draw_filter.h',
    'ext/paint_simplifier.cc',
    'ext/paint_simplifier.h',
    'ext/pixel_ref_utils.cc',
    'ext/pixel_ref_utils.h',
    'ext/platform_canvas.cc',
    'ext/platform_canvas.h',
    'ext/platform_device.cc',
    'ext/platform_device.h',
    'ext/platform_device_linux.cc',
    'ext/platform_device_mac.cc',
    'ext/platform_device_win.cc',
    'ext/recursive_gaussian_convolution.cc',
    'ext/recursive_gaussian_convolution.h',
    'ext/refptr.h',
    'ext/SkDiscardableMemory_chrome.h',
    'ext/SkDiscardableMemory_chrome.cc',
    'ext/SkMemory_new_handler.cpp',
    'ext/skia_trace_shim.h',
    'ext/skia_utils_base.cc',
    'ext/skia_utils_base.h',
    'ext/skia_utils_ios.mm',
    'ext/skia_utils_ios.h',
    'ext/skia_utils_mac.mm',
    'ext/skia_utils_mac.h',
    'ext/skia_utils_win.cc',
    'ext/skia_utils_win.h',
    'ext/vector_canvas.h',
    'ext/vector_platform_device_emf_win.cc',
    'ext/vector_platform_device_emf_win.h',
    'ext/vector_platform_device_skia.cc',
    'ext/vector_platform_device_skia.h',
  ],
  'conditions': [
    [ 'OS == "android" and enable_printing == 0', {
      'sources!': [
        'ext/skia_utils_base.cc',
      ],
    }],
    [ 'enable_printing == 0', {
      'sources!': [
        'ext/vector_platform_device_skia.cc',
      ],
    }],
    ['OS == "ios"', {
      'sources/': [
        ['exclude', '^ext/vector_platform_device_skia\\.'],
      ],
      'dependencies!': [
        'skia_chrome_opts',
      ],
    }],
    # TODO(scottmg): http://crbug.com/177306
    ['clang==1', {
      'xcode_settings': {
        'WARNING_CFLAGS!': [
          # Don't warn about string->bool used in asserts.
          '-Wstring-conversion',
        ],
      },
      'cflags!': [
        '-Wstring-conversion',
      ],
    }],
    [ 'OS != "android" and (OS != "linux" or use_cairo==1)', {
      'sources!': [
        'ext/bitmap_platform_device_skia.cc',
      ],
    }],
  ],

  'target_conditions': [
    # Pull in specific linux files for android (which have been filtered out
    # by file name rules).
    [ 'OS == "android"', {
      'sources/': [
        ['include', 'ext/platform_device_linux\\.cc$'],
      ],
    }],
  ],
}
