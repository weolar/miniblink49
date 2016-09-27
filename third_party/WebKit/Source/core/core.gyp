#
# Copyright (C) 2009 Google Inc. All rights reserved.
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
    '../build/win/precompile.gypi',
    '../build/features.gypi',
    '../build/scripts/scripts.gypi',
    '../bindings/core/core.gypi',  # core can depend on bindings/core, but not on bindings
    'core.gypi',
  ],

  'variables': {
    'enable_wexit_time_destructors': 1,

    'webcore_include_dirs': [
      '..',  # WebKit/Source
      '<(SHARED_INTERMEDIATE_DIR)/blink',  # gen/blink
    ],

    'conditions': [
      ['OS=="android" and use_openmax_dl_fft!=0', {
        'webcore_include_dirs': [
          '<(DEPTH)/third_party/openmax_dl'
        ]
      }],
    ],
  },  # variables

  'target_defaults': {
    'variables': {
      'optimize': 'max',
    },
  },

  'targets': [
    {
      # GN version: //third_party/WebKit/Source/core/inspector:protocol_sources
      'target_name': 'inspector_protocol_sources',
      'type': 'none',
      'dependencies': [
        'generate_inspector_protocol_version'
      ],
      'actions': [
        {
          'action_name': 'generateInspectorProtocolBackendSources',
          'inputs': [
            # The python script in action below.
            'inspector/CodeGeneratorInspector.py',
            # The helper script imported by CodeGeneratorInspector.py.
            'inspector/CodeGeneratorInspectorStrings.py',
            # Input file for the script.
            '../devtools/protocol.json',
          ],
          'outputs': [
            '<(blink_core_output_dir)/InspectorBackendDispatcher.cpp',
            '<(blink_core_output_dir)/InspectorBackendDispatcher.h',
            '<(blink_core_output_dir)/InspectorFrontend.cpp',
            '<(blink_core_output_dir)/InspectorFrontend.h',
            '<(blink_core_output_dir)/InspectorTypeBuilder.cpp',
            '<(blink_core_output_dir)/InspectorTypeBuilder.h',
          ],
          'variables': {
            'generator_include_dirs': [
            ],
          },
          'action': [
            'python',
            'inspector/CodeGeneratorInspector.py',
            '../devtools/protocol.json',
            '--output_dir', '<(blink_core_output_dir)',
          ],
          'message': 'Generating Inspector protocol backend sources from protocol.json',
        },
      ]
    },
    {
      # GN version: //third_party/WebKit/Source/core/inspector:instrumentation_sources
      'target_name': 'inspector_instrumentation_sources',
      'type': 'none',
      'dependencies': [],
      'actions': [
        {
          'action_name': 'generateInspectorInstrumentation',
          'inputs': [
            # The python script in action below.
            'inspector/CodeGeneratorInstrumentation.py',
            # Input file for the script.
            'inspector/InspectorInstrumentation.idl',
          ],
          'outputs': [
            '<(blink_core_output_dir)/InspectorConsoleInstrumentationInl.h',
            '<(blink_core_output_dir)/InspectorInstrumentationInl.h',
            '<(blink_core_output_dir)/InspectorOverridesInl.h',
            '<(blink_core_output_dir)/InstrumentingAgentsInl.h',
            '<(blink_core_output_dir)/InspectorInstrumentationImpl.cpp',
          ],
          'action': [
            'python',
            'inspector/CodeGeneratorInstrumentation.py',
            'inspector/InspectorInstrumentation.idl',
            '--output_dir', '<(blink_core_output_dir)',
          ],
          'message': 'Generating Inspector instrumentation code from InspectorInstrumentation.idl',
        }
      ]
    },
    {
      # GN version: //third_party/WebKit/Source/core/inspector:protocol_version
      'target_name': 'generate_inspector_protocol_version',
      'type': 'none',
      'actions': [
         {
          'action_name': 'generateInspectorProtocolVersion',
          'inputs': [
            'inspector/generate-inspector-protocol-version',
            '../devtools/protocol.json',
          ],
          'outputs': [
            '<(blink_core_output_dir)/InspectorProtocolVersion.h',
          ],
          'variables': {
            'generator_include_dirs': [
            ],
          },
          'action': [
            'python',
            'inspector/generate-inspector-protocol-version',
            '-o',
            '<@(_outputs)',
            '<@(_inputs)'
          ],
          'message': 'Validate inspector protocol for backwards compatibility and generate version file',
        }
      ]
    },
    {
      # GN version: //third_party/WebKit/Source/core:core_generated
      'target_name': 'webcore_generated',
      'type': 'static_library',
      'hard_dependency': 1,
      'dependencies': [
        'webcore_prerequisites',
        'core_generated.gyp:make_core_generated',
        'inspector_protocol_sources',
        'inspector_instrumentation_sources',
        '../bindings/core/v8/generated.gyp:bindings_core_v8_generated',
        # FIXME: don't depend on bindings_modules http://crbug.com/358074
        '../bindings/modules/generated.gyp:modules_event_generated',
        '../bindings/modules/v8/generated.gyp:bindings_modules_v8_generated',
        '../platform/platform_generated.gyp:make_platform_generated',
        '../wtf/wtf.gyp:wtf',
        '<(DEPTH)/gin/gin.gyp:gin',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(DEPTH)/third_party/iccjpeg/iccjpeg.gyp:iccjpeg',
        '<(DEPTH)/third_party/libpng/libpng.gyp:libpng',
        '<(DEPTH)/third_party/libwebp/libwebp.gyp:libwebp',
        '<(DEPTH)/third_party/libxml/libxml.gyp:libxml',
        '<(DEPTH)/third_party/libxslt/libxslt.gyp:libxslt',
        '<(DEPTH)/third_party/npapi/npapi.gyp:npapi',
        '<(DEPTH)/third_party/qcms/qcms.gyp:qcms',
        '<(DEPTH)/third_party/snappy/snappy.gyp:snappy',
        '<(DEPTH)/third_party/sqlite/sqlite.gyp:sqlite',
        '<(DEPTH)/url/url.gyp:url_lib',
        '<(DEPTH)/v8/tools/gyp/v8.gyp:v8',
      ],
      'include_dirs': [
        '<@(webcore_include_dirs)',
      ],
      'sources': [
        # FIXME: should be bindings_core_v8_files http://crbug.com/358074
        '<@(bindings_core_v8_files)',
        # These files include all the .cpp files generated from the .idl files
        # in webcore_files.
        '<@(bindings_core_v8_generated_aggregate_files)',
        '<@(bindings_core_v8_generated_union_type_files)',

        '<@(generated_core_additional_files)',

        # IDL dictionary impl files generated by IDL compiler
        '<@(generated_core_dictionary_files)',
      ],
      'conditions': [
        ['OS=="win" and component=="shared_library"', {
          'defines': [
            'USING_V8_SHARED',
          ],
        }],
        ['OS=="win"', {
          # In generated bindings code: 'switch contains default but no case'.
          # Disable c4267 warnings until we fix size_t to int truncations.
          # 4701 and 4702 are disabled because of issues in Bison-generated
          # XPathGrammar.cpp.cpp.
          'msvs_disabled_warnings': [ 4065, 4267, 4701, 4702 ],
        }],
        ['OS in ("linux", "android") and "WTF_USE_WEBAUDIO_IPP=1" in feature_defines', {
          'cflags': [
            '<!@(pkg-config --cflags-only-I ipp)',
          ],
        }],
      ],
    },
    {
      # We'll soon split libwebcore in multiple smaller libraries.
      # webcore_prerequisites will be the 'base' target of every sub-target.
      # GN version: //third_party/WebKit/Source/core:prerequisites
      'target_name': 'webcore_prerequisites',
      'type': 'none',
      'dependencies': [
        'inspector_protocol_sources',
        'inspector_instrumentation_sources',
        'core_generated.gyp:make_core_generated',
        '../bindings/core/v8/generated.gyp:bindings_core_v8_generated',
        # FIXME: don't depend on bindings_modules http://crbug.com/358074
        '../bindings/modules/v8/generated.gyp:bindings_modules_v8_generated',
        '../wtf/wtf.gyp:wtf',
        '../config.gyp:config',
        '../platform/blink_platform.gyp:blink_platform',
        '<(DEPTH)/gpu/gpu.gyp:gles2_c_lib',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(angle_path)/src/angle.gyp:translator',
        '<(DEPTH)/third_party/iccjpeg/iccjpeg.gyp:iccjpeg',
        '<(DEPTH)/third_party/libpng/libpng.gyp:libpng',
        '<(DEPTH)/third_party/libwebp/libwebp.gyp:libwebp',
        '<(DEPTH)/third_party/libxml/libxml.gyp:libxml',
        '<(DEPTH)/third_party/libxslt/libxslt.gyp:libxslt',
        '<(DEPTH)/third_party/npapi/npapi.gyp:npapi',
        '<(DEPTH)/third_party/ots/ots.gyp:ots',
        '<(DEPTH)/third_party/qcms/qcms.gyp:qcms',
        '<(DEPTH)/third_party/sqlite/sqlite.gyp:sqlite',
        '<(DEPTH)/third_party/zlib/zlib.gyp:zlib',
        '<(DEPTH)/url/url.gyp:url_lib',
        '<(DEPTH)/v8/tools/gyp/v8.gyp:v8',
      ],
      'export_dependent_settings': [
        '../wtf/wtf.gyp:wtf',
        '../config.gyp:config',
        '<(DEPTH)/gpu/gpu.gyp:gles2_c_lib',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(angle_path)/src/angle.gyp:translator',
        '<(DEPTH)/third_party/iccjpeg/iccjpeg.gyp:iccjpeg',
        '<(DEPTH)/third_party/libpng/libpng.gyp:libpng',
        '<(DEPTH)/third_party/libwebp/libwebp.gyp:libwebp',
        '<(DEPTH)/third_party/libxml/libxml.gyp:libxml',
        '<(DEPTH)/third_party/libxslt/libxslt.gyp:libxslt',
        '<(DEPTH)/third_party/npapi/npapi.gyp:npapi',
        '<(DEPTH)/third_party/ots/ots.gyp:ots',
        '<(DEPTH)/third_party/qcms/qcms.gyp:qcms',
        '<(DEPTH)/third_party/sqlite/sqlite.gyp:sqlite',
        '<(DEPTH)/third_party/zlib/zlib.gyp:zlib',
        '<(DEPTH)/url/url.gyp:url_lib',
        '<(DEPTH)/v8/tools/gyp/v8.gyp:v8',
      ],
      'direct_dependent_settings': {
        'defines': [
          'BLINK_IMPLEMENTATION=1',
          'INSIDE_BLINK',
        ],
        'include_dirs': [
          '<@(webcore_include_dirs)',
          '<(DEPTH)/gpu',
          '<(angle_path)/include',
        ],
        'xcode_settings': {
          # Some Mac-specific parts of WebKit won't compile without having this
          # prefix header injected.
          'GCC_PREFIX_HEADER': '<(DEPTH)/third_party/WebKit/Source/build/mac/Prefix.h',
        },
      },
      'conditions': [
        ['OS=="win" and component=="shared_library"', {
          'direct_dependent_settings': {
            'defines': [
               'USING_V8_SHARED',
            ],
          },
        }],
        ['use_x11 == 1', {
          'dependencies': [
            '<(DEPTH)/build/linux/system.gyp:fontconfig',
          ],
          'export_dependent_settings': [
            '<(DEPTH)/build/linux/system.gyp:fontconfig',
          ],
          'direct_dependent_settings': {
            'cflags': [
              # WebCore does not work with strict aliasing enabled.
              # https://bugs.webkit.org/show_bug.cgi?id=25864
              '-fno-strict-aliasing',
            ],
          },
        }],
        ['OS=="android"', {
          'sources/': [
            ['exclude', 'accessibility/'],
          ],
        }],
        ['OS in ("linux", "android") and "WTF_USE_WEBAUDIO_IPP=1" in feature_defines', {
          'direct_dependent_settings': {
            'cflags': [
              '<!@(pkg-config --cflags-only-I ipp)',
            ],
          },
        }],
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
        ['"WTF_USE_WEBAUDIO_FFMPEG=1" in feature_defines', {
          # This directory needs to be on the include path for multiple sub-targets of webcore.
          'direct_dependent_settings': {
            'include_dirs': [
              '<(DEPTH)/third_party/ffmpeg',
            ],
          },
          'dependencies': [
            '<(DEPTH)/third_party/ffmpeg/ffmpeg.gyp:ffmpeg',
          ],
        }],
       ['"WTF_USE_WEBAUDIO_OPENMAX_DL_FFT=1" in feature_defines', {
         'direct_dependent_settings': {
           'include_dirs': [
             '<(DEPTH)/third_party/openmax_dl',
           ],
         },
         'dependencies': [
           '<(DEPTH)/third_party/openmax_dl/dl/dl.gyp:openmax_dl',
         ],
       }],
        # Windows shared builder needs extra help for linkage
        ['OS=="win" and "WTF_USE_WEBAUDIO_FFMPEG=1" in feature_defines', {
          'export_dependent_settings': [
            '<(DEPTH)/third_party/ffmpeg/ffmpeg.gyp:ffmpeg',
          ],
        }],
      ],
    },
    {
      # GN version: //third_party/WebKit/Source/core:dom
      'target_name': 'webcore_dom',
      'type': 'static_library',
      'dependencies': [
        'webcore_prerequisites',
      ],
      'sources': [
        '<@(webcore_dom_files)',
      ],
      # Disable c4267 warnings until we fix size_t to int truncations.
      'msvs_disabled_warnings': [ 4267, ],
    },
    {
      # GN version: //third_party/WebKit/Source/core:html
      'target_name': 'webcore_html',
      'type': 'static_library',
      'dependencies': [
        'webcore_prerequisites',
      ],
      'sources': [
        '<@(webcore_html_files)',
      ],
      'conditions': [
        # Shard this taret into parts to work around linker limitations.
        # on link time code generation builds.
        ['OS=="win" and buildtype=="Official"', {
          'msvs_shard': 5,
        }],
        ['OS!="android"', {
          'sources/': [
            ['exclude', 'Android\\.cpp$'],
          ],
        }],
      ],
    },
    {
      # GN version: //third_party/WebKit/Source/core:svg
      'target_name': 'webcore_svg',
      'type': 'static_library',
      'dependencies': [
        'webcore_prerequisites',
      ],
      'sources': [
        '<@(webcore_svg_files)',
      ],
      'conditions': [
        ['OS=="win" and buildtype=="Official"', {
          'msvs_shard': 5,
        }],
      ],
    },
    {
      # GN version: //third_party/WebKit/Source/core:rendering
      'target_name': 'webcore_rendering',
      'type': 'static_library',
      'dependencies': [
        'webcore_prerequisites',
      ],
      'sources': [
        '<@(webcore_rendering_files)',
      ],
      'conditions': [
        # Shard this taret into parts to work around linker limitations.
        # on link time code generation builds.
        ['OS=="win" and buildtype=="Official"', {
          'msvs_shard': 5,
        }],
        ['use_default_render_theme==0 and OS != "android"', {
          'sources!': [
            'layout/LayoutThemeDefault.cpp',
            'layout/LayoutThemeDefault.h',
          ],
        }],
        ['OS!="win"', {
          'sources!': [
            'layout/LayoutThemeFontProviderWin.cpp',
            'layout/LayoutThemeWin.cpp',
            'layout/LayoutThemeWin.h',
          ],
        }],
        ['OS=="win" and chromium_win_pch==1', {
          'sources/': [
            ['include', '<(DEPTH)/third_party/WebKit/Source/build/win/Precompile.cpp'],
          ],
        }],
        ['OS=="mac"', {
          'sources!': [
            # LayoutThemeFontProvider is used by LayoutThemeDefault.
            'layout/LayoutThemeFontProvider.cpp',
            'layout/LayoutThemeFontProvider.h',
          ],
        },{ # OS!="mac"
          'sources!': [
            'layout/LayoutThemeMac.h',
            'layout/LayoutThemeMac.mm',
          ],
        }],
        ['OS == "android" and target_arch == "ia32" and gcc_version == 46', {
          # Due to a bug in gcc 4.6 in android NDK, we get warnings about uninitialized variable.
          'cflags': ['-Wno-uninitialized'],
        }],
        ['OS != "linux"', {
          'sources!': [
            'layout/LayoutThemeLinux.cpp',
            'layout/LayoutThemeLinux.h',
          ],
        }],
        ['OS != "linux" and OS != "android"', {
          'sources!': [
            'layout/LayoutThemeFontProviderLinux.cpp',
          ],
        }],
        ['OS!="android"', {
          'sources!': [
            'layout/LayoutThemeAndroid.cpp',
            'layout/LayoutThemeAndroid.h',
          ],
        }],
      ],
    },
    {
      # GN version: //third_party/WebKit/Source/core:remaining
      'target_name': 'webcore_remaining',
      'type': 'static_library',
      'dependencies': [
        'webcore_prerequisites',
      ],
      'sources': [
        '<@(webcore_non_rendering_files)',
      ],
      'conditions': [
        # Shard this target into parts to work around linker limitations.
        # on link time code generation builds.
        ['OS=="win" and (buildtype=="Official" or (fastbuild==0 and win_z7==1))', {
          'msvs_shard': 19,
        }],
        ['OS=="android"', {
          'cflags': [
            # WebCore does not work with strict aliasing enabled.
            # https://bugs.webkit.org/show_bug.cgi?id=25864
            '-fno-strict-aliasing',
          ],
        }],
        ['OS=="mac"', {
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/Carbon.framework',
            ],
          },
          'sources/': [
            # Additional files from the WebCore Mac build that are presently
            # used in the WebCore Chromium Mac build too.

            # Cherry-pick some files that can't be included by broader regexps.
            # Some of these are used instead of Chromium platform files, see
            # the specific exclusions in the "exclude" list below.
            ['include', 'platform/mac/WebCoreSystemInterface\\.h$'],
            ['include', 'platform/mac/WebCoreTextRenderer\\.mm$'],
            ['include', 'platform/text/mac/ShapeArabic\\.c$'],
            ['include', 'platform/text/mac/String(Impl)?Mac\\.mm$'],
            # Use USE_NEW_THEME on Mac.
            ['include', 'platform/Theme\\.cpp$'],
          ],
        }, { # OS!="mac"
          'sources!': [
            'editing/SmartReplaceCF.cpp',
          ],
        }],
        ['OS=="win" and chromium_win_pch==1', {
          'sources/': [
            ['include', '<(DEPTH)/third_party/WebKit/Source/build/win/Precompile.cpp'],
          ],
        }],
        ['use_default_render_theme==0 and OS != "android"', {
          'sources!': [
            'paint/ThemePainterDefault.cpp',
            'paint/ThemePainterDefault.h',
          ],
        }],
      ],
      # Disable c4267 warnings until we fix size_t to int truncations.
      'msvs_disabled_warnings': [ 4267, 4334, ],
    },
    {
      # GN version: //third_party/WebKit/Source/core:core
      'target_name': 'webcore',
      'type': 'none',
      'dependencies': [
        'webcore_dom',
        'webcore_html',
        'webcore_remaining',
        'webcore_rendering',
        'webcore_svg',
        # Exported.
        'webcore_generated',
        '../platform/blink_platform.gyp:blink_platform',
        '../wtf/wtf.gyp:wtf',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(DEPTH)/third_party/npapi/npapi.gyp:npapi',
        '<(DEPTH)/third_party/qcms/qcms.gyp:qcms',
        '<(DEPTH)/url/url.gyp:url_lib',
        '<(DEPTH)/v8/tools/gyp/v8.gyp:v8',
      ],
      'export_dependent_settings': [
        'webcore_generated',
        '../platform/blink_platform.gyp:blink_platform',
        '../wtf/wtf.gyp:wtf',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(DEPTH)/third_party/npapi/npapi.gyp:npapi',
        '<(DEPTH)/third_party/qcms/qcms.gyp:qcms',
        '<(DEPTH)/url/url.gyp:url_lib',
        '<(DEPTH)/v8/tools/gyp/v8.gyp:v8',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '<@(webcore_include_dirs)',
        ],
      },
      'conditions': [
        ['OS=="linux" and "WTF_USE_WEBAUDIO_IPP=1" in feature_defines', {
          'link_settings': {
            'ldflags': [
              '<!@(pkg-config --libs-only-L ipp)',
            ],
            'libraries': [
              '-lipps -lippcore',
            ],
          },
        }],
        # Use IPP static libraries for x86 Android.
        ['OS=="android" and "WTF_USE_WEBAUDIO_IPP=1" in feature_defines', {
          'link_settings': {
            'libraries': [
               '<!@(pkg-config --libs ipp|sed s/-L//)/libipps_l.a',
               '<!@(pkg-config --libs ipp|sed s/-L//)/libippcore_l.a',
            ]
          },
        }],
      ],
    },
    {
      # GN version: //third_party/WebKit/Source/core:testing
      'target_name': 'webcore_testing',
      'type': 'static_library',
      'dependencies': [
        '../config.gyp:config',
        '../wtf/wtf_tests.gyp:wtf_unittest_helpers',
      ],
      'defines': [
        'BLINK_IMPLEMENTATION=1',
        'INSIDE_BLINK',
      ],
      'include_dirs': [
        'testing',
        'testing/v8',
      ],
      'sources': [
        # Note: file list duplicated in GN build.
        '<@(generated_core_testing_dictionary_files)',
        '<@(webcore_testing_files)',
        '<@(generated_bindings_core_testing_files)',
      ],
      'sources/': [
        ['exclude', 'testing/js'],
      ],
      'conditions': [
        ['component!="shared_library" or link_core_modules_separately==0', {
          'dependencies': [
            'webcore',
            'webcore_generated',
          ],
        }, {
          'dependencies': [
            'webcore_shared',
          ],
        }]
      ],
    },
    {
      # GN version: //third_party/WebKit/Source/core:core
      'target_name': 'webcore_shared',
      'type': 'shared_library',
      'hard_dependency': 1,
      'dependencies': [
        'webcore_prerequisites',

        '../platform/blink_platform.gyp:blink_common',
        '../platform/blink_platform.gyp:blink_platform',

        # webcore_generated dependency
        'core_generated.gyp:make_core_generated',
        'inspector_protocol_sources',
        'inspector_instrumentation_sources',
        '../bindings/core/v8/generated.gyp:bindings_core_v8_generated',
        # FIXME: don't depend on bindings_modules http://crbug.com/358074
        '../bindings/modules/generated.gyp:modules_event_generated',
        '../bindings/modules/v8/generated.gyp:bindings_modules_v8_generated',
        '../platform/platform_generated.gyp:make_platform_generated',

        '../wtf/wtf.gyp:wtf',
        '<(DEPTH)/gin/gin.gyp:gin',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(DEPTH)/third_party/libxml/libxml.gyp:libxml',
        '<(DEPTH)/third_party/libxslt/libxslt.gyp:libxslt',
        '<(DEPTH)/third_party/npapi/npapi.gyp:npapi',
        '<(DEPTH)/third_party/qcms/qcms.gyp:qcms',
        '<(DEPTH)/third_party/snappy/snappy.gyp:snappy',
        '<(DEPTH)/third_party/sqlite/sqlite.gyp:sqlite',
        '<(DEPTH)/url/url.gyp:url_lib',
        '<(DEPTH)/v8/tools/gyp/v8.gyp:v8',
      ],
      'export_dependent_settings': [
        '../platform/blink_platform.gyp:blink_platform',
        '../wtf/wtf.gyp:wtf',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(DEPTH)/third_party/npapi/npapi.gyp:npapi',
        '<(DEPTH)/third_party/qcms/qcms.gyp:qcms',
        '<(DEPTH)/url/url.gyp:url_lib',
        '<(DEPTH)/v8/tools/gyp/v8.gyp:v8',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '<@(webcore_include_dirs)',
        ],
      },
      'conditions': [
        ['component!="shared_library" or link_core_modules_separately==0', {
        }, {
          'defines': [
            'BLINK_CORE_IMPLEMENTATION=1',
            'BLINK_IMPLEMENTATION=1',
            'INSIDE_BLINK',
          ],
          'sources': [
            # webcore_generated
            '<@(bindings_core_v8_files)',
            '<@(bindings_core_v8_generated_aggregate_files)',
            '<@(bindings_core_v8_generated_union_type_files)',
            '<@(generated_core_additional_files)',
            '<@(generated_core_dictionary_files)',

            # webcore_non_rendering, webcore_rendering
            '<@(webcore_rendering_files)',
            '<@(webcore_non_rendering_files)',
            # webcore_dom
            '<@(webcore_dom_files)',
            # webcore_svg
            '<@(webcore_svg_files)',
            # webcore_html
             '<@(webcore_html_files)',
          ],
          'include_dirs': [
            '<@(webcore_include_dirs)',
            '../..',  # WebKit
            # for testing_files
            'testing',
            'testing/v8',
          ],
          'conditions': [
            ['use_default_render_theme==0 and OS != "android"', {
              'sources!': [
                'layout/LayoutThemeDefault.cpp',
                'layout/LayoutThemeDefault.h',
                'paint/ThemePainterDefault.cpp',
                'paint/ThemePainterDefault.h',
              ],
            }],
            ['OS=="win"', {
              # In generated bindings code: 'switch contains default but no
              # case'.
              # Disable c4267 warnings until we fix size_t to int truncations.
              # 4701 and 4702 are disabled because of issues in Bison-generated
              # XPathGrammar.cpp.cpp.
              # Disable c4267 warnings until we fix size_t to int truncations.
              'msvs_disabled_warnings': [ 4065, 4267, 4305, 4334, 4701, 4702, 4910 ],
              # Disable incremental link when building debug binary to avoid
              # "LNK1210: exceeded internal ILK size limit;".
              'configurations': {
                'Debug_Base': {
                  'msvs_settings': {
                    'VCLinkerTool': {
                      'LinkIncremental': '1',
                    },
                  },
                },
              },
            }, {
              'sources!': [
                'layout/LayoutThemeFontProviderWin.cpp',
                'layout/LayoutThemeWin.cpp',
                'layout/LayoutThemeWin.h',
              ],
              'libraries': [
                '-lm -lstdc++',
              ],
            }],
            ['OS=="win" and chromium_win_pch==1', {
              'sources/': [
                ['include', '<(DEPTH)/third_party/WebKit/Source/build/win/Precompile.cpp'],
              ],
            }],
            ['OS=="mac"', {
              'sources!': [
                # LayoutThemeSkia is not used on mac since LayoutThemeMac
                # does not reference the Skia code that is used by Windows, Linux and Android.
                'layout/LayoutThemeSkia.cpp',
                'layout/LayoutThemeSkia.h',

                # LayoutThemeFontProvider is used by LayoutThemeSkia.
                'layout/LayoutThemeFontProvider.cpp',
                'layout/LayoutThemeFontProvider.h',
              ],
              'link_settings': {
                'libraries': [
                  '$(SDKROOT)/System/Library/Frameworks/Carbon.framework',
                ],
              },
            },{ # OS!="mac"
              'sources!': [
                'editing/SmartReplaceCF.cpp',
                'layout/LayoutThemeMac.h',
                'layout/LayoutThemeMac.mm',
              ],
            }],
            ['OS == "android" and target_arch == "ia32" and gcc_version == 46', {
              # Due to a bug in gcc 4.6 in android NDK, we get warnings about uninitialized variable.
              'cflags': ['-Wno-uninitialized'],
            }],
            ['OS != "linux"', {
              'sources!': [
                'layout/LayoutThemeLinux.cpp',
                'layout/LayoutThemeLinux.h',
              ],
            }],
            ['OS != "linux" and OS != "android"', {
              'sources!': [
                'layout/LayoutThemeFontProviderLinux.cpp',
              ],
            }],
            ['OS=="android"', {
              'cflags': [
                # WebCore does not work with strict aliasing enabled.
                # https://bugs.webkit.org/show_bug.cgi?id=25864
                '-fno-strict-aliasing',
              ],
            }, {
              'sources!': [
                'layout/LayoutThemeAndroid.cpp',
                'layout/LayoutThemeAndroid.h',
              ],
            }],
          ],
        }],
        ['OS=="linux" and "WTF_USE_WEBAUDIO_IPP=1" in feature_defines', {
          'link_settings': {
            'ldflags': [
              '<!@(pkg-config --libs-only-L ipp)',
            ],
            'libraries': [
              '-lipps -lippcore',
            ],
          },
        }],
        # Use IPP static libraries for x86 Android.
        ['OS=="android" and "WTF_USE_WEBAUDIO_IPP=1" in feature_defines', {
          'link_settings': {
            'libraries': [
               '<!@(pkg-config --libs ipp|sed s/-L//)/libipps_l.a',
               '<!@(pkg-config --libs ipp|sed s/-L//)/libippcore_l.a',
            ]
          },
        }],
      ],
    },
  ],  # targets
}
