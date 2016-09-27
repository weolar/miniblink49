# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.


# This gyp file contains the platform-specific optimizations for Skia
{
  'targets': [
    # Due to an unfortunate intersection of lameness between gcc and gyp,
    # we have to build the *_SSE2.cpp files in a separate target.  The
    # gcc lameness is that, in order to compile SSE2 intrinsics code, it
    # must be passed the -msse2 flag.  However, with this flag, it may
    # emit SSE2 instructions even for scalar code, such as the CPUID
    # test used to test for the presence of SSE2.  So that, and all other
    # code must be compiled *without* -msse2.  The gyp lameness is that it
    # does not allow file-specific CFLAGS, so we must create this extra
    # target for those files to be compiled with -msse2.
    #
    # This is actually only a problem on 32-bit Linux (all Intel Macs have
    # SSE2, Linux x86_64 has SSE2 by definition, and MSC will happily emit
    # SSE2 from instrinsics, which generating plain ol' 386 for everything
    # else).  However, to keep the .gyp file simple and avoid platform-specific
    # build breakage, we do this on all platforms.

    # For about the same reason, we need to compile the ARM opts files
    # separately as well.
    {
      'target_name': 'skia_opts',
      'type': 'static_library',
      'includes': [
        'skia_common.gypi',
      ],
      'include_dirs': [
        '../third_party/skia/include/core',
        '../third_party/skia/include/effects',
        '../third_party/skia/src/core',
        '../third_party/skia/src/opts',
      ],
      'conditions': [
        [ 'os_posix == 1 and OS != "mac" and OS != "android" and \
           target_arch != "arm" and target_arch != "arm64" and \
           target_arch != "mipsel"', {
          'cflags': [
            '-msse2',
          ],
        }],
        [ 'target_arch != "arm" and target_arch != "mipsel" and \
           target_arch != "arm64"', {
          'sources': [
            '../third_party/skia/src/opts/SkBitmapProcState_opts_SSE2.cpp',
            '../third_party/skia/src/opts/SkBlitRect_opts_SSE2.cpp',
            '../third_party/skia/src/opts/SkBlitRow_opts_SSE2.cpp',
            '../third_party/skia/src/opts/SkUtils_opts_SSE2.cpp',
            '../third_party/skia/src/opts/SkXfermode_opts_SSE2.cpp',
            '../third_party/skia/src/opts/SkBitmapFilter_opts_SSE2.cpp',
            '../third_party/skia/src/opts/SkMorphology_opts_SSE2.cpp',
            '../third_party/skia/src/opts/SkBlurImage_opts_SSE2.cpp',
          ],
          'dependencies': [
            'skia_opts_ssse3',
          ],
        }],
        [ 'target_arch == "arm"', {
          'conditions': [
            [ 'arm_version >= 7 and arm_neon == 1', {
              'defines': [
                '__ARM_HAVE_NEON',
              ],
            }],
            [ 'arm_version >= 7 and arm_neon_optional == 1', {
              'defines': [
                '__ARM_HAVE_OPTIONAL_NEON_SUPPORT',
              ],
            }],
            [ 'arm_version >= 7 and (arm_neon == 1 or arm_neon_optional == 1)', {
              'cflags': [
                # The neon assembly contains conditional instructions which
                # aren't enclosed in an IT block. The assembler complains
                # without this option.
                # See #86592.
                '-Wa,-mimplicit-it=always',
              ],
              'dependencies': [
                'skia_opts_neon',
              ]
           }],
          ],
          # The assembly uses the frame pointer register (r7 in Thumb/r11 in
          # ARM), the compiler doesn't like that. Explicitly remove the
          # -fno-omit-frame-pointer flag for Android, as that gets added to all
          # targets via common.gypi.
          'cflags!': [
            '-fno-omit-frame-pointer',
            '-marm',
            '-mapcs-frame',
          ],
          'cflags': [
            '-fomit-frame-pointer',
          ],
          'sources': [
            '../third_party/skia/src/opts/SkBitmapProcState_opts_arm.cpp',
          ],
        }],
        [ 'target_arch == "arm" and (arm_version < 7 or (arm_neon == 0 and arm_neon_optional == 1))', {
          'sources': [
            '../third_party/skia/src/opts/memset.arm.S',
          ],
        }],
        [ 'target_arch == "arm" and arm_version < 6', {
          'sources': [
            '../third_party/skia/src/opts/SkBlitMask_opts_none.cpp',
            '../third_party/skia/src/opts/SkBlitRow_opts_none.cpp',
            '../third_party/skia/src/opts/SkUtils_opts_none.cpp',
            '../third_party/skia/src/opts/SkXfermode_opts_none.cpp',
            '../third_party/skia/src/opts/SkMorphology_opts_none.cpp',
            '../third_party/skia/src/opts/SkBlurImage_opts_none.cpp',
          ],
        }],
        [ 'target_arch == "arm" and arm_version >= 6', {
          'sources': [
            '../third_party/skia/src/opts/SkBlitMask_opts_arm.cpp',
            '../third_party/skia/src/opts/SkBlitRow_opts_arm.cpp',
            '../third_party/skia/src/opts/SkBlitRow_opts_arm.h',
            '../third_party/skia/src/opts/SkBlurImage_opts_arm.cpp',
            '../third_party/skia/src/opts/SkMorphology_opts_arm.cpp',
            '../third_party/skia/src/opts/SkUtils_opts_arm.cpp',
            '../third_party/skia/src/opts/SkXfermode_opts_none.cpp',
          ],
        }],
        [ 'target_arch == "mipsel"',{
          'cflags': [
            '-fomit-frame-pointer',
          ],
          'sources': [
            '../third_party/skia/src/opts/SkBitmapProcState_opts_none.cpp',
            '../third_party/skia/src/opts/SkBlitMask_opts_none.cpp',
            '../third_party/skia/src/opts/SkBlitRow_opts_none.cpp',
            '../third_party/skia/src/opts/SkUtils_opts_none.cpp',
            '../third_party/skia/src/opts/SkXfermode_opts_none.cpp',
            '../third_party/skia/src/opts/SkMorphology_opts_none.cpp',
            '../third_party/skia/src/opts/SkBlurImage_opts_none.cpp',
          ],
        }],
        [ 'target_arch == "arm64"', {
          'sources': [
            '../third_party/skia/src/opts/SkBitmapProcState_arm_neon.cpp',
            '../third_party/skia/src/opts/SkBitmapProcState_matrixProcs_neon.cpp',
            '../third_party/skia/src/opts/SkBitmapProcState_opts_arm.cpp',
            '../third_party/skia/src/opts/SkBlitMask_opts_arm.cpp',
            '../third_party/skia/src/opts/SkBlitMask_opts_arm_neon.cpp',
            '../third_party/skia/src/opts/SkBlitRow_opts_none.cpp',
            '../third_party/skia/src/opts/SkBlurImage_opts_arm.cpp',
            '../third_party/skia/src/opts/SkBlurImage_opts_neon.cpp',
            '../third_party/skia/src/opts/SkMorphology_opts_arm.cpp',
            '../third_party/skia/src/opts/SkMorphology_opts_neon.cpp',
            '../third_party/skia/src/opts/SkUtils_opts_none.cpp',
            '../third_party/skia/src/opts/SkXfermode_opts_arm.cpp',
            '../third_party/skia/src/opts/SkXfermode_opts_arm_neon.cpp',
          ],
        }],
      ],
    },
    # For the same lame reasons as what is done for skia_opts, we have to
    # create another target specifically for SSSE3 code as we would not want
    # to compile the SSE2 code with -mssse3 which would potentially allow
    # gcc to generate SSSE3 code.
    {
      'target_name': 'skia_opts_ssse3',
      'type': 'static_library',
      'includes': [
        'skia_common.gypi',
      ],
      'include_dirs': [
        '../third_party/skia/include/core',
        '../third_party/skia/include/effects',
        '../third_party/skia/src/core',
      ],
      'conditions': [
        [ 'OS in ["linux", "freebsd", "openbsd", "solaris", "android"]', {
          'cflags': [
            '-mssse3',
          ],
        }],
        [ 'OS == "mac"', {
          'xcode_settings': {
            'GCC_ENABLE_SUPPLEMENTAL_SSE3_INSTRUCTIONS': 'YES',
          },
        }],
        [ 'OS == "win"', {
          'include_dirs': [
            'config/win',
          ],
          'direct_dependent_settings': {
            'include_dirs': [
              'config/win',
            ],
          },
        }],
        [ 'target_arch != "arm" and target_arch != "arm64" and \
           target_arch != "mipsel"', {
          'sources': [
            '../third_party/skia/src/opts/SkBitmapProcState_opts_SSSE3.cpp',
          ],
        }],
      ],
    },
    {
      'target_name': 'skia_opts_none',
      'type': 'static_library',
      'includes': [
        'skia_common.gypi',
      ],
      'include_dirs': [
        '../third_party/skia/include/core',
        '../third_party/skia/include/effects',
        '../third_party/skia/src/core',
      ],
      'sources': [
        '../third_party/skia/src/opts/SkBitmapProcState_opts_none.cpp',
        '../third_party/skia/src/opts/SkBlitMask_opts_none.cpp',
        '../third_party/skia/src/opts/SkBlitRow_opts_none.cpp',
        '../third_party/skia/src/opts/SkUtils_opts_none.cpp',
        '../third_party/skia/src/opts/SkXfermode_opts_none.cpp',
        '../third_party/skia/src/opts/SkMorphology_opts_none.cpp',
        '../third_party/skia/src/opts/SkBlurImage_opts_none.cpp',
      ],
    },
  ],
  'conditions': [
    # NEON code must be compiled with -mfpu=neon which also affects scalar
    # code. To support dynamic NEON code paths, we need to build all
    # NEON-specific sources in a separate static library. The situation
    # is very similar to the SSSE3 one.
    ['target_arch == "arm" and (arm_neon == 1 or arm_neon_optional == 1)', {
      'targets': [
        {
          'target_name': 'skia_opts_neon',
          'type': 'static_library',
          'includes': [
            'skia_common.gypi',
          ],
          'include_dirs': [
            '../third_party/skia/include/core',
            '../third_party/skia/include/effects',
            '../third_party/skia/src/core',
            '../third_party/skia/src/opts',
          ],
          'cflags!': [
            '-fno-omit-frame-pointer',
            '-mfpu=vfp',  # remove them all, just in case.
            '-mfpu=vfpv3',
            '-mfpu=vfpv3-d16',
          ],
          'cflags': [
            '-mfpu=neon',
            '-fomit-frame-pointer',
          ],
          'ldflags': [
            '-march=armv7-a',
            '-Wl,--fix-cortex-a8',
          ],
          'sources': [
            '../third_party/skia/src/opts/memset16_neon.S',
            '../third_party/skia/src/opts/memset32_neon.S',
            '../third_party/skia/src/opts/SkBitmapProcState_arm_neon.cpp',
            '../third_party/skia/src/opts/SkBitmapProcState_matrixProcs_neon.cpp',
            '../third_party/skia/src/opts/SkBitmapProcState_matrix_clamp_neon.h',
            '../third_party/skia/src/opts/SkBitmapProcState_matrix_repeat_neon.h',
            '../third_party/skia/src/opts/SkBlitMask_opts_arm_neon.cpp',
            '../third_party/skia/src/opts/SkBlitRow_opts_arm_neon.cpp',
            '../third_party/skia/src/opts/SkXfermode_opts_arm_neon.cpp',
            '../third_party/skia/src/opts/SkBlurImage_opts_neon.cpp',
            '../third_party/skia/src/opts/SkMorphology_opts_neon.cpp',
          ],
          'conditions': [
            ['arm_neon == 1', {
              'defines': [
                '__ARM_HAVE_NEON',
              ],
            }],
            ['arm_neon_optional == 1', {
              'defines': [
                '__ARM_HAVE_OPTIONAL_NEON_SUPPORT',
              ],
            }],
          ],
        },
      ],
    }],
  ],
}
