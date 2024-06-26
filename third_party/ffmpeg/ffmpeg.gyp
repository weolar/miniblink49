# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# There's a couple key GYP variables that control how FFmpeg is built:
#   ffmpeg_branding
#     Controls whether we build the Chromium or Google Chrome version of
#     FFmpeg.  The Google Chrome version contains additional codecs.
#     Typical values are Chromium, Chrome, ChromiumOS, and ChromeOS.
#   ffmpeg_component
#     Set true to build ffmpeg as a shared library. NOTE: this means we should
#     always consult the value of 'ffmpeg_component' instead of 'component' for
#     this file. This helps linux chromium packagers that swap out our
#     ffmpeg.so with their own. See discussion here
#     https://groups.google.com/a/chromium.org/forum/#!msg/chromium-packagers/R5rcZXWxBEQ/B6k0zzmJbvcJ
#

{
  'target_defaults': {
    'variables': {
      # Since we are not often debugging FFmpeg, and performance is
      # unacceptable without optimization, freeze the optimizations to -O2.
      # If someone really wants -O1 , they can change these in their checkout.
      # If you want -O0, see the Gotchas in README.Chromium for why that
      # won't work.
      'release_optimize': '2',
      'debug_optimize': '2',
      'mac_debug_optimization': '2',
      # In addition to the above reasons, /Od optimization won't remove symbols
      # that are under "if (0)" style sections.  Which lead to link time errors
      # when for example it tries to link an ARM symbol on X86.
      'win_debug_Optimization': '2',
      # Run time checks are incompatible with any level of optimizations.
      'win_debug_RuntimeChecks': '0',
      'conditions': [
        ['OS == "win"', {
          # Setting the optimizations to 'speed' or to 'max' results in a lot of
          # unresolved symbols. The only supported mode is 'size_no_ltcg' (see
          # crbug.com/264459).
          'optimize' :'size_no_ltcg',
        }],
      ],
    },
  },
  'variables': {
    # Make sure asm_sources is always defined even if an arch doesn't have any
    # asm sources (e.g. mips or x86 with forcefully disabled asm).
    'asm_sources': [
    ],

    # weolar: mp4
    'proprietary_codecs%': 1,
    'ffmpeg_branding%': 'Chrome',

    # Allow overriding the selection of which FFmpeg binaries to copy via an
    # environment variable.  Affects the ffmpeg_binaries target.
    'conditions': [
      # Android ia32 can't handle textrels and ffmpeg can't compile without them.
      # http://crbug.com/559379
      ['(OS == "android" and target_arch == "ia32") or msan==1', {
        'disable_ffmpeg_asm%': 1,
      }, {
        'disable_ffmpeg_asm%': 0,
      }],
      ['target_arch == "arm" and arm_version == 7 and arm_neon == 1', {
        # Need a separate config for arm+neon vs arm
        'ffmpeg_config%': 'arm-neon',
      }, {
        'ffmpeg_config%': '<(target_arch)',
      }],
      ['OS == "mac" or OS == "win" or OS == "openbsd" or OS == "android"', {
        'os_config%': '<(OS)',
      }, {  # all other Unix OS's use the linux config
        'conditions': [
          ['msan==1', {
            # MemorySanitizer doesn't like assembly code.
            'os_config%': 'linux-noasm',
          }, {
            'os_config%': 'linux',
          }]
        ],
      }],
      # weolar: remove
      #['chromeos == 1', {
      #  'ffmpeg_branding%': '<(branding)OS',
      #}, {  # otherwise, assume Chrome/Chromium.
      #  'ffmpeg_branding%': '<(branding)',
      #}],
    ],

    'ffmpeg_component%': '<(component)',

    # Locations for generated artifacts.
    'shared_generated_dir': '<(SHARED_INTERMEDIATE_DIR)/third_party/ffmpeg',

    # Stub generator script and signatures of all functions used by Chrome.
    'generate_stubs_script': '../../tools/generate_stubs/generate_stubs.py',
    'sig_files': ['chromium/ffmpeg.sigs'],
  },
  'conditions': [
    ['(target_arch == "ia32" or target_arch == "x64") and disable_ffmpeg_asm == 0', {
      'targets': [
        {
          'target_name': 'ffmpeg_yasm',
          'type': 'static_library',
          'includes': [
            'ffmpeg_generated.gypi',
            '../yasm/yasm_compile.gypi',
          ],
          'sources': [
            '<@(asm_sources)',
            # XCode doesn't want to link a pure assembly target and will fail
            # to link when it creates an empty file list.  So add a dummy file
            # keep the linker happy.  See http://crbug.com/157073
            'xcode_hack.c',
          ],
          'variables': {
            # Path to platform configuration files.
            'platform_config_root': 'chromium/config/<(ffmpeg_branding)/<(os_config)/<(ffmpeg_config)',

            'conditions': [
              ['target_arch == "ia32"', {
                'more_yasm_flags': [
                  '-DARCH_X86_32',
                 ],
              }, {
                'more_yasm_flags': [
                  '-DARCH_X86_64',
                ],
              }],
              ['OS == "mac"', {
                'more_yasm_flags': [
                  # Necessary to ensure symbols end up with a _ prefix; added by
                  # yasm_compile.gypi for Windows, but not Mac.
                  '-DPREFIX',
                ]
              }],
            ],
            'yasm_flags': [
              '-DPIC',
              '>@(more_yasm_flags)',
              '-I', '<(platform_config_root)',
              '-I', 'libavcodec/x86/',
              '-I', 'libavutil/x86/',
              '-I', '.',
              # Disable warnings, prevents log spam for things we won't fix.
              '-w',
              '-P', 'config.asm',
            ],
            'yasm_includes': [
              # Sets visibility hidden for cglobal functions. Explicitly included
              # to avoid overlooking changes to this file in incremental builds.
              'libavutil/x86/x86inc.asm',
            ],
            'yasm_output_path': '<(shared_generated_dir)/yasm'
          },
        },
      ] # targets
    }], # (target_arch == "ia32" or target_arch == "x64")
  ],

  'targets': [{
    'target_name': 'ffmpeg',
    'type': '<(ffmpeg_component)',
    'variables': {
      # Path to platform configuration files.
      'platform_config_root': 'chromium/config/<(ffmpeg_branding)/<(os_config)/<(ffmpeg_config)',
    },
    'conditions': [
      # This looks crazy, but it seems to be the only way to make this work. Without this
      # conditions block, platform_config_root will be undefined when expanded below.
      ['1 == 1',
        {
          'direct_dependent_settings': {
            'include_dirs': [
              '../..',  # The chromium 'src' directory.
              '<(platform_config_root)',
              '.',
            ],
          },
          'includes': [
            'ffmpeg_generated.gypi',
          ],
          'sources': [
            '<@(c_sources)',
            '<(platform_config_root)/config.h',
            '<(platform_config_root)/libavutil/avconfig.h',
          ],
          'include_dirs': [
            '<(platform_config_root)',
            '.',
          ],
          'defines': [
            'HAVE_AV_CONFIG_H',
            '_POSIX_C_SOURCE=200112',
            '_XOPEN_SOURCE=600',
            'PIC',
            # Disable deprecated features that generate spammy warnings.
            # BUILD.gn & media/ffmpeg/ffmpeg_common.h must be kept in sync.
            'FF_API_CONVERGENCE_DURATION=0',
            # Upstream libavcodec/utils.c still uses the deprecated
            # av_dup_packet(), causing deprecation warnings.
            # The normal fix for such things is to disable the feature as below,
            # but the upstream code does not yet compile with it disabled.
            # (In this case, the fix is replacing the call with a new function.)
            # In the meantime, we directly disable those warnings in the C file.
            # 'FF_API_AVPACKET_OLD_API=0',
          ],
          'variables': {
            'clang_warning_flags': [
              '-Wno-absolute-value',
              # ffmpeg uses its own deprecated functions.
              '-Wno-deprecated-declarations',
              # ffmpeg doesn't care about pointer constness.
              '-Wno-incompatible-pointer-types',
              # ffmpeg doesn't follow usual parentheses conventions.
              '-Wno-parentheses',
              # ffmpeg doesn't care about pointer signedness.
              '-Wno-pointer-sign',
              # ffmpeg doesn't believe in exhaustive switch statements.
              '-Wno-switch',
              # matroskadec.c has a "failed:" label that's only used if some
              # CONFIG_ flags we don't set are set.
              '-Wno-unused-label',
              # ffmpeg has a lot of unused variables.
              '-Wno-unused-variable',
              # This fires on `av_assert0(!"valid element size")` in utils.c
              '-Wno-string-conversion',
              # This fires on `pos_min` and `pos_max` in
              # autorename_libavformat_utils.c
              '-Wno-sometimes-uninitialized',
              # ffmpeg contains static functions in header files, which lead
              # to unused function warnings. There are a few legit unused
              # functions too.
              '-Wno-unused-function',
              # vp3data.h's vp31_inter_dequant stores '128' in an int8_t array.
              '-Wno-constant-conversion',
            ],
          },
          'cflags': [
            '-fPIC',
            '-fomit-frame-pointer',
            # ffmpeg uses its own deprecated functions.
            '-Wno-deprecated-declarations',
          ],
          # Silence a warning in libc++ builds (C code doesn't need this flag).
          'ldflags!': [ '-stdlib=libc++', ],
          'conditions': [
            ['(target_arch == "ia32" or target_arch == "x64") and disable_ffmpeg_asm == 0', {
              'dependencies': [
                'ffmpeg_yasm',
              ],
            }],
            ['clang != 1', {
              'cflags': [
                # gcc doesn't have flags for specific warnings, so disable them
                # all.
                '-w',
              ],
            }],
            ['target_arch == "ia32"', {
              # Turn off valgrind build option that breaks ffmpeg builds.
              'cflags!': [
                '-fno-omit-frame-pointer',
              ],
              'debug_extra_cflags!': [
                '-fno-omit-frame-pointer',
              ],
              'release_extra_cflags!': [
                '-fno-omit-frame-pointer',
              ],
            }],  # target_arch == "ia32"
            ['target_arch == "arm" or target_arch == "arm64"', {
              # On arm we use gcc to compile the assembly.
              'sources': [
                '<@(asm_sources)',
              ],
              'conditions': [
                ['arm_float_abi == "hard"', {
                  'cflags': [
                    '-DHAVE_VFP_ARGS=1'
                  ],
                }, {
                  'cflags': [
                    '-DHAVE_VFP_ARGS=0'
                  ],
                }],
                ['clang==1', {
                  # TODO(hans) Enable integrated-as (crbug.com/124610).
                  'cflags': [ '-fno-integrated-as' ],
                  'conditions': [
                    ['OS == "android"', {
                      # Else /usr/bin/as gets picked up.
                      'cflags': [ '-B<(android_toolchain)' ],
                    }],
                  ],
                }],
              ],
            }],
            ['os_posix == 1 and OS != "mac"', {
              'defines': [
                '_ISOC99_SOURCE',
                '_LARGEFILE_SOURCE',
                # BUG(ihf): ffmpeg compiles with this define. But according to
                # ajwong: I wouldn't change _FILE_OFFSET_BITS.  That's a scary change
                # because it affects the default length of off_t, and fpos_t,
                # which can cause strange problems if the loading code doesn't
                # have it set and you start passing FILE*s or file descriptors
                # between symbol contexts.
                # '_FILE_OFFSET_BITS=64',
              ],
              'cflags': [
                '-std=c99',
                '-pthread',
                '-fno-math-errno',
                '-fno-signed-zeros',
                '-fno-tree-vectorize',
              ],
              'link_settings': {
                'libraries': [
                ],
              },
              'conditions': [
                ['OS != "android"', {
                  'link_settings': {
                    # OS=android requires that both -lz and -lm occur
                    # after -lc++_shared on the link command
                    # line. Android link rules already include -lm, and
                    # we get -lz as a transitive dependency of
                    # libandroid.so, so simply moving both to the
                    # non-Android section solves the problem.
                    #
                    # The root cause of this problem is certain system
                    # libraries (libm starting with MNC and libz before
                    # MNC, among others) re-export the libgcc unwinder,
                    # and libc++ exports the libc++abi unwinder. As we
                    # build against libc++ headers, libc++ must be the
                    # first in the runtime symbol lookup order (among
                    # all unwinder-providing libraries).
                    'libraries': [
                      '-lm',
                      '-lz',
                      '-lrt',
                    ],
                  },
                }],
                ['ffmpeg_component == "shared_library"', {
                  # Export all symbols when building as component.
                  'cflags!': [
                    '-fvisibility=hidden',
                  ],
                  # Fixes warnings PIC relocation when building as component.
                  # *WARNING* -- DO NOT put this inside of a link_settings
                  # section or these flags will be propagated outside of the
                  # ffmpeg target and cause debug allocator crashes.
                  'ldflags': [
                    '-Wl,-Bsymbolic',
                    '-L<(shared_generated_dir)',
                  ],
                }],
              ],
            }],  # os_posix == 1 and OS != "mac"
            ['OS == "openbsd"', {
              # OpenBSD's gcc (4.2.1) does not support this flag
              'cflags!': [
                '-fno-signed-zeros',
              ],
            }],
            ['OS == "mac"', {
              'defines': [
                '_DARWIN_C_SOURCE',
              ],
              'conditions': [
                ['ffmpeg_component == "shared_library"', {
                  'xcode_settings': {
                    # GCC version of no -fvisiliity=hidden. Ensures that all
                    # symbols are exported for component builds.
                    'GCC_SYMBOLS_PRIVATE_EXTERN': 'NO',
                  },
                }],
              ],
            }],  # OS == "mac"
            ['OS == "win"', {
              # Disable /analyze because of internal compiler error on log.c
              # with VS 2015 Update 1 when building with /analyze:
              # https://connect.microsoft.com/VisualStudio/feedback/details/2299303
              'msvs_settings': {
                'VCCLCompilerTool': {
                  'AdditionalOptions!': [ '/analyze:WX-' ]
                },
              },
              # TODO(dalecurtis): We should fix these.  http://crbug.com/154421
              'msvs_disabled_warnings': [
                4996, 4018, 4090, 4305, 4133, 4146, 4554, 4028, 4334, 4101, 4102,
                4116, 4307, 4273, 4005, 4056, 4756,
              ],
              'conditions': [
                ['clang == 1', {
                  'msvs_settings': {
                    'VCCLCompilerTool': {
                      # This corresponds to msvs_disabled_warnings 4273 above.
                      'AdditionalOptions': [ '-Wno-inconsistent-dllimport' ],
                    },
                  },
                }],
                ['clang == 1 or (MSVS_VERSION == "2013" or MSVS_VERSION == "2013e")', {
                  'defines': [
                    'inline=__inline',
                    'strtoll=_strtoi64',
                    '_ISOC99_SOURCE',
                    '_LARGEFILE_SOURCE',
                    'HAVE_AV_CONFIG_H',
                    'strtod=avpriv_strtod',
                    'snprintf=avpriv_snprintf',
                    '_snprintf=avpriv_snprintf',
                    'vsnprintf=avpriv_vsnprintf',
                  ],
                }],
                ['target_arch == "x64"', {
                  # TODO(wolenetz): We should fix this.  http://crbug.com/171009
                  'msvs_disabled_warnings' : [
                    4267
                  ],
                }],
                ['ffmpeg_component == "shared_library"', {
                  # Fix warnings about a local symbol being inefficiently imported.
                  'msvs_settings': {
                    'VCCLCompilerTool': {
                      'AdditionalOptions': [
                        '/FIcompat/msvcrt/snprintf.h',
                        '/FIcompat/msvcrt/strtod.h',
                      ],
                    },
                  },
                  'sources': [
                    '<(shared_generated_dir)/ffmpeg.def',
                  ],
                  'conditions': [
                    ['OS=="win" and win_use_allocator_shim==1', {
                      'dependencies': [
                        '../../base/allocator/allocator.gyp:allocator',
                      ],
                      'sources': [
                        'chromium/dllmain.cc',
                      ],
                    }],
                  ],
                  'actions': [
                    {
                      'action_name': 'generate_def',
                      'inputs': [
                        '<(generate_stubs_script)',
                        '<@(sig_files)',
                      ],
                      'outputs': [
                        '<(shared_generated_dir)/ffmpeg.def',
                      ],
                      'action': ['python',
                                 '<(generate_stubs_script)',
                                 '-i', '<(INTERMEDIATE_DIR)',
                                 '-o', '<(shared_generated_dir)',
                                 '-t', 'windows_def',
                                 '-m', 'ffmpeg.dll',
                                 '<@(_inputs)',
                      ],
                      'message': 'Generating FFmpeg export definitions',
                    },
                  ],
                }],
              ],
            }],
          ],
        },
      ],
    ],
  }],
}
