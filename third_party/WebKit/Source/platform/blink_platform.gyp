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
  'includes': [
    '../build/features.gypi',
    '../build/scripts/scripts.gypi',
    '../build/win/precompile.gypi',
    'blink_platform.gypi',
    'heap/blink_heap.gypi',
  ],
  'targets': [{
    'target_name': 'blink_common',
    'type': '<(component)',
    'variables': { 'enable_wexit_time_destructors': 1 },
    'dependencies': [
      '../config.gyp:config',
      '../wtf/wtf.gyp:wtf',
      # FIXME: Can we remove the dependency on Skia?
      '<(DEPTH)/skia/skia.gyp:skia',
    ],
    'all_dependent_settings': {
      'include_dirs': [
        '..',
      ],
    },
    'export_dependent_settings': [
      '<(DEPTH)/skia/skia.gyp:skia',
    ],
    'defines': [
      'BLINK_COMMON_IMPLEMENTATION=1',
      'INSIDE_BLINK',
    ],
    'include_dirs': [
      '<(SHARED_INTERMEDIATE_DIR)/blink',
    ],
    'sources': [
      'exported/WebCString.cpp',
      'exported/WebString.cpp',
      'exported/WebCommon.cpp',
    ],
  },
  {
    'target_name': 'blink_heap_asm_stubs',
    'type': 'static_library',
    # VS2010 does not correctly incrementally link obj files generated
    # from asm files. This flag disables UseLibraryDependencyInputs to
    # avoid this problem.
    'msvs_2010_disable_uldi_when_referenced': 1,
    'includes': [
      '../../../yasm/yasm_compile.gypi',
    ],
    'sources': [
      '<@(platform_heap_asm_files)',
    ],
    'variables': {
      'more_yasm_flags': [],
      'conditions': [
        ['OS == "mac"', {
          'more_yasm_flags': [
            # Necessary to ensure symbols end up with a _ prefix; added by
            # yasm_compile.gypi for Windows, but not Mac.
            '-DPREFIX',
          ],
        }],
        ['OS == "win" and target_arch == "x64"', {
          'more_yasm_flags': [
            '-DX64WIN=1',
          ],
        }],
        ['OS != "win" and target_arch == "x64"', {
          'more_yasm_flags': [
            '-DX64POSIX=1',
          ],
        }],
        ['target_arch == "ia32"', {
          'more_yasm_flags': [
            '-DIA32=1',
          ],
        }],
        ['target_arch == "arm"', {
          'more_yasm_flags': [
            '-DARM=1',
          ],
        }],
      ],
      'yasm_flags': [
        '>@(more_yasm_flags)',
      ],
      'yasm_output_path': '<(SHARED_INTERMEDIATE_DIR)/webcore/heap'
    },
  },
  {
    'target_name': 'blink_prerequisites',
    'type': 'none',
    'conditions': [
      ['OS=="mac"', {
        'direct_dependent_settings': {
          'defines': [
            # Chromium's version of WebCore includes the following Objective-C
            # classes. The system-provided WebCore framework may also provide
            # these classes. Because of the nature of Objective-C binding
            # (dynamically at runtime), it's possible for the
            # Chromium-provided versions to interfere with the system-provided
            # versions.  This may happen when a system framework attempts to
            # use core.framework, such as when converting an HTML-flavored
            # string to an NSAttributedString.  The solution is to force
            # Objective-C class names that would conflict to use alternate
            # names.
            #
            # This list will hopefully shrink but may also grow.  Its
            # performance is monitored by the "Check Objective-C Rename"
            # postbuild step, and any suspicious-looking symbols not handled
            # here or whitelisted in that step will cause a build failure.
            #
            # If this is unhandled, the console will receive log messages
            # such as:
            # com.google.Chrome[] objc[]: Class ScrollbarPrefsObserver is implemented in both .../Google Chrome.app/Contents/Versions/.../Google Chrome Helper.app/Contents/MacOS/../../../Google Chrome Framework.framework/Google Chrome Framework and /System/Library/Frameworks/WebKit.framework/Versions/A/Frameworks/WebCore.framework/Versions/A/WebCore. One of the two will be used. Which one is undefined.
            'WebCascadeList=ChromiumWebCoreObjCWebCascadeList',
            'WebScrollAnimationHelperDelegate=ChromiumWebCoreObjCWebScrollAnimationHelperDelegate',
            'WebScrollbarPainterControllerDelegate=ChromiumWebCoreObjCWebScrollbarPainterControllerDelegate',
            'WebScrollbarPainterDelegate=ChromiumWebCoreObjCWebScrollbarPainterDelegate',
            'WebScrollbarPartAnimation=ChromiumWebCoreObjCWebScrollbarPartAnimation',
            'WebCoreFlippedView=ChromiumWebCoreObjCWebCoreFlippedView',
            'WebCoreTextFieldCell=ChromiumWebCoreObjCWebCoreTextFieldCell',
          ],
          'postbuilds': [
            {
              # This step ensures that any Objective-C names that aren't
              # redefined to be "safe" above will cause a build failure.
              'postbuild_name': 'Check Objective-C Rename',
              'variables': {
                'class_whitelist_regex':
                    'ChromiumWebCoreObjC|TCMVisibleView|RTCMFlippedView|ScrollerStyleObserver|LayoutThemeNotificationObserver',
                'category_whitelist_regex':
                    'WebCoreFocusRingDrawing|WebCoreTheme',
              },
              'action': [
                '../build/scripts/check_objc_rename.sh',
                '<(class_whitelist_regex)',
                '<(category_whitelist_regex)',
              ],
            },
          ],
        },
      }],
    ],
  },
  {
    'target_name': 'blink_platform',
    'type': '<(component)',
    'dependencies': [
      '../config.gyp:config',
      '../wtf/wtf.gyp:wtf',
      'blink_common',
      'blink_heap_asm_stubs',
      'blink_prerequisites',
      '<(DEPTH)/gpu/gpu.gyp:gles2_c_lib',
      '<(DEPTH)/skia/skia.gyp:skia',
      '<(DEPTH)/third_party/icu/icu.gyp:icui18n',
      '<(DEPTH)/third_party/icu/icu.gyp:icuuc',
      '<(DEPTH)/third_party/libpng/libpng.gyp:libpng',
      '<(DEPTH)/third_party/libwebp/libwebp.gyp:libwebp',
      '<(DEPTH)/third_party/ots/ots.gyp:ots',
      '<(DEPTH)/third_party/qcms/qcms.gyp:qcms',
      '<(DEPTH)/url/url.gyp:url_lib',
      '<(DEPTH)/v8/tools/gyp/v8.gyp:v8',
      'platform_generated.gyp:make_platform_generated',
      '<(DEPTH)/third_party/iccjpeg/iccjpeg.gyp:iccjpeg',
      '<(libjpeg_gyp_path):libjpeg',
    ],
    'export_dependent_settings': [
      '<(DEPTH)/gpu/gpu.gyp:gles2_c_lib',
      '<(DEPTH)/skia/skia.gyp:skia',
      '<(DEPTH)/third_party/libpng/libpng.gyp:libpng',
      '<(DEPTH)/third_party/libwebp/libwebp.gyp:libwebp',
      '<(DEPTH)/third_party/ots/ots.gyp:ots',
      '<(DEPTH)/third_party/qcms/qcms.gyp:qcms',
      '<(DEPTH)/v8/tools/gyp/v8.gyp:v8',
      '<(DEPTH)/url/url.gyp:url_lib',
      '<(DEPTH)/third_party/iccjpeg/iccjpeg.gyp:iccjpeg',
      '<(libjpeg_gyp_path):libjpeg',
    ],
    'defines': [
      'BLINK_PLATFORM_IMPLEMENTATION=1',
      'INSIDE_BLINK',
    ],
    'include_dirs': [
      '<(angle_path)/include',
      '<(SHARED_INTERMEDIATE_DIR)/blink',
    ],
    'xcode_settings': {
      # Some Mac-specific parts of WebKit won't compile without having this
      # prefix header injected.
      'GCC_PREFIX_HEADER': '<(DEPTH)/third_party/WebKit/Source/build/mac/Prefix.h',
    },
    'sources': [
      '<@(platform_files)',
      '<@(platform_heap_files)',

      # Additional .cpp files from platform_generated.gyp:make_platform_generated actions.
      '<(blink_platform_output_dir)/FontFamilyNames.cpp',
      '<(blink_platform_output_dir)/RuntimeEnabledFeatures.cpp',
      '<(blink_platform_output_dir)/RuntimeEnabledFeatures.h',
      '<(blink_platform_output_dir)/ColorData.cpp',
    ],
    'sources/': [
      # Exclude all platform specific things, reinclude them below on a per-platform basis
      # FIXME: Figure out how to store these patterns in a variable.
      ['exclude', '(cf|cg|mac|opentype|win)/'],
      ['exclude', '(?<!Chromium)(CF|CG|Mac|Win)\\.(cpp|mm?)$'],

      # *NEON.cpp files need special compile options.
      # They are moved to the webcore_0_neon target.
      ['exclude', 'graphics/cpu/arm/.*NEON\\.(cpp|h)'],
      ['exclude', 'graphics/cpu/arm/filters/.*NEON\\.(cpp|h)'],
    ],
    # Disable c4267 warnings until we fix size_t to int truncations.
    # Disable c4724 warnings which is generated in VS2012 due to improper
    # compiler optimizations, see crbug.com/237063
    'msvs_disabled_warnings': [ 4267, 4334, 4724 ],
    'conditions': [
      ['target_arch == "ia32" or target_arch == "x64"', {
        'sources/': [
          ['include', 'graphics/cpu/x86/WebGLImageConversionSSE\\.h$'],
        ],
      }],
      ['OS=="linux" or OS=="android" or OS=="win"', {
        'sources/': [
          # Cherry-pick files excluded by the broader regular expressions above.
          ['include', 'fonts/opentype/OpenTypeTypes\\.h$'],
          ['include', 'fonts/opentype/OpenTypeVerticalData\\.(cpp|h)$'],
        ],
        'dependencies': [
          '<(DEPTH)/third_party/harfbuzz-ng/harfbuzz.gyp:harfbuzz-ng',
        ],
      },
      ],
      ['OS=="linux" or OS=="android"', {
        'sources/': [
          ['include', 'fonts/linux/FontPlatformDataLinux\\.cpp$'],
        ]
      }, { # OS!="linux" and OS!="android"
        'sources/': [
          ['exclude', 'fonts/linux/FontPlatformDataLinux\\.cpp$'],
        ]
      }],
      ['OS=="mac"', {
        'dependencies': [
          '<(DEPTH)/third_party/harfbuzz-ng/harfbuzz.gyp:harfbuzz-ng',
        ],
        'link_settings': {
          'libraries': [
            '$(SDKROOT)/System/Library/Frameworks/Accelerate.framework',
            '$(SDKROOT)/System/Library/Frameworks/Carbon.framework',
            '$(SDKROOT)/System/Library/Frameworks/Foundation.framework',
          ]
        },
        'sources/': [
          # We use LocaleMac.mm instead of LocaleICU.cpp
          ['exclude', 'text/LocaleICU\\.(cpp|h)$'],
          ['include', 'text/LocaleMac\\.mm$'],

          # The Mac uses mac/KillRingMac.mm instead of the dummy
          # implementation.
          ['exclude', 'KillRingNone\\.cpp$'],

          # The Mac build is USE(CF).
          ['include', 'CF\\.cpp$'],

          # Use native Mac font code from core.
          ['include', '(fonts/)?mac/[^/]*Font[^/]*\\.(cpp|mm?)$'],

          # TODO(dro): Merge the opentype vertical data files inclusion across all platforms.
          ['include', 'fonts/opentype/OpenTypeTypes\\.h$'],
          ['include', 'fonts/opentype/OpenTypeVerticalData\\.(cpp|h)$'],

          # Cherry-pick some files that can't be included by broader regexps.
          # Some of these are used instead of Chromium platform files, see
          # the specific exclusions in the "exclude" list below.
          ['include', 'audio/mac/FFTFrameMac\\.cpp$'],
          ['include', 'fonts/mac/GlyphPageTreeNodeMac\\.cpp$'],
          ['include', 'mac/ColorMac\\.mm$'],
          ['include', 'mac/BlockExceptions\\.mm$'],
          ['include', 'mac/KillRingMac\\.mm$'],
          ['include', 'mac/LocalCurrentGraphicsContext\\.mm$'],
          ['include', 'mac/NSScrollerImpDetails\\.mm$'],
          ['include', 'mac/ScrollAnimatorMac\\.mm$'],
          ['include', 'mac/ThemeMac\\.h$'],
          ['include', 'mac/ThemeMac\\.mm$'],
          ['include', 'mac/WebCoreNSCellExtras\\.h$'],
          ['include', 'mac/WebCoreNSCellExtras\\.mm$'],
 
          # Mac uses only ScrollAnimatorMac.
          ['exclude', 'scroll/ScrollbarThemeNonMacCommon\\.(cpp|h)$'],
          ['exclude', 'scroll/ScrollAnimatorNone\\.cpp$'],
          ['exclude', 'scroll/ScrollAnimatorNone\\.h$'],

          ['exclude', 'fonts/skia/FontCacheSkia\\.cpp$'],

          ['include', 'geometry/mac/FloatPointMac\\.mm$'],
          ['include', 'geometry/mac/FloatRectMac\\.mm$'],
          ['include', 'geometry/mac/FloatSizeMac\\.mm$'],
          ['include', 'geometry/mac/IntPointMac\\.mm$'],
          ['include', 'geometry/mac/IntRectMac\\.mm$'],

          ['include', 'geometry/cg/FloatPointCG\\.cpp$'],
          ['include', 'geometry/cg/FloatRectCG\\.cpp$'],
          ['include', 'geometry/cg/FloatSizeCG\\.cpp$'],
          ['include', 'geometry/cg/IntPointCG\\.cpp$'],
          ['include', 'geometry/cg/IntRectCG\\.cpp$'],
          ['include', 'geometry/cg/IntSizeCG\\.cpp$'],
        ],
      }, { # OS!="mac"
        'sources/': [
          ['exclude', 'mac/'],
          ['exclude', 'geometry/mac/'],
          ['exclude', 'geometry/cg/'],
          ['exclude', 'scroll/ScrollbarThemeMac'],
        ],
      }],
      ['OS != "linux" and OS != "mac" and OS != "win"', {
        'sources/': [
          ['exclude', 'VDMX[^/]+\\.(cpp|h)$'],
        ],
      }],
      ['OS=="win"', {
        'sources/': [
          # We use LocaleWin.cpp instead of LocaleICU.cpp
          ['exclude', 'text/LocaleICU\\.(cpp|h)$'],
          ['include', 'text/LocaleWin\\.(cpp|h)$'],

          ['include', 'clipboard/ClipboardUtilitiesWin\\.(cpp|h)$'],

          ['include', 'fonts/opentype/'],
          ['include', 'fonts/win/FontCacheSkiaWin\\.cpp$'],
          ['include', 'fonts/win/FontFallbackWin\\.(cpp|h)$'],
          ['include', 'fonts/win/FontPlatformDataWin\\.cpp$'],

          # SystemInfo.cpp is useful and we don't want to copy it.
          ['include', 'win/SystemInfo\\.cpp$'],
        ],
      }, { # OS!="win"
        'sources/': [
          ['exclude', 'win/'],
          ['exclude', 'Win\\.cpp$'],
          ['exclude', '/(Windows)[^/]*\\.cpp$'],
          ['include', 'fonts/opentype/OpenTypeSanitizer\\.cpp$'],
        ],
      }],
      ['OS=="win" and chromium_win_pch==1', {
        'sources/': [
          ['include', '<(DEPTH)/third_party/WebKit/Source/build/win/Precompile.cpp'],
        ],
      }],
      ['OS=="android"', {
        'sources/': [
          ['include', '^fonts/VDMXParser\\.cpp$'],
        ],
      }, { # OS!="android"
        'sources/': [
          ['exclude', 'Android\\.cpp$'],
        ],
      }],
      ['OS=="linux"', {
        'dependencies': [
          '<(DEPTH)/build/linux/system.gyp:fontconfig',
        ],
        'export_dependent_settings': [
          '<(DEPTH)/build/linux/system.gyp:fontconfig',
        ],
      }],
      ['use_default_render_theme==0', {
        'sources/': [
          ['exclude', 'scroll/ScrollbarThemeAura\\.(cpp|h)'],
        ],
      }],
      ['"WTF_USE_WEBAUDIO_FFMPEG=1" in feature_defines', {
        'include_dirs': [
          '<(DEPTH)/third_party/ffmpeg',
        ],
        'dependencies': [
          '<(DEPTH)/third_party/ffmpeg/ffmpeg.gyp:ffmpeg',
        ],
      }],
      ['"WTF_USE_WEBAUDIO_OPENMAX_DL_FFT=1" in feature_defines', {
         'include_dirs': [
           '<(DEPTH)/third_party/openmax_dl',
         ],
        'dependencies': [
          '<(DEPTH)/third_party/openmax_dl/dl/dl.gyp:openmax_dl',
        ],
      }],
      ['target_arch=="arm"', {
        'dependencies': [
          'blink_arm_neon',
        ],
      }],
    ],
    'target_conditions': [
      ['OS=="android"', {
        'sources/': [
            ['include', 'exported/linux/WebFontRenderStyle\\.cpp$'],
            ['include', 'fonts/linux/FontPlatformDataLinux\\.cpp$'],
        ],
      }],
    ],
  },
  # The *NEON.cpp files fail to compile when -mthumb is passed. Force
  # them to build in ARM mode.
  # See https://bugs.webkit.org/show_bug.cgi?id=62916.
  {
    'target_name': 'blink_arm_neon',
    'conditions': [
      ['target_arch=="arm"', {
        'type': 'static_library',
        'dependencies': [
          'blink_common',
        ],
        'hard_dependency': 1,
        'sources': [
          '<@(platform_files)',
        ],
        'sources/': [
          ['exclude', '.*'],
          ['include', 'graphics/cpu/arm/filters/.*NEON\\.(cpp|h)'],
        ],
        'cflags': ['-marm'],
        'conditions': [
          ['OS=="android"', {
            'cflags!': ['-mthumb'],
          }],
        ],
      },{  # target_arch!="arm"
        'type': 'none',
      }],
    ],
  }],
}
