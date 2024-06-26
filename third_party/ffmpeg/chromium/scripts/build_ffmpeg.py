#!/usr/bin/env python
#
# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from __future__ import print_function

import collections
import multiprocessing
import optparse
import os
import platform
import re
import shutil
import subprocess
import sys


SCRIPTS_DIR = os.path.abspath(os.path.dirname(__file__))
FFMPEG_DIR = os.path.abspath(os.path.join(SCRIPTS_DIR, '..', '..'))
CHROMIUM_ROOT_DIR = os.path.abspath(os.path.join(FFMPEG_DIR, '..', '..'))
NDK_ROOT_DIR = os.path.abspath(os.path.join(CHROMIUM_ROOT_DIR, 'third_party',
                                            'android_tools', 'ndk'))


BRANDINGS = [
    'Chrome',
    'ChromeOS',
    'Chromium',
    'ChromiumOS',
]


USAGE = """Usage: %prog TARGET_OS TARGET_ARCH [options] -- [configure_args]

Valid combinations are android     [ia32|x64|mipsel|mips64el|arm-neon|arm64]
                       linux       [ia32|x64|mipsel|arm|arm-neon|arm64]
                       linux-noasm [x64]
                       mac         [x64]
                       win         [ia32|x64]

Platform specific build notes:
  android:
    Script can be run on a normal x64 Ubuntu box with an Android-ready Chromium
    checkout: https://code.google.com/p/chromium/wiki/AndroidBuildInstructions

  linux ia32/x64:
    Script can run on a normal Ubuntu box.

  linux mipsel:
    Script must be run inside of ChromeOS SimpleChrome setup:
        cros chrome-sdk --board=mipsel-o32-generic --use-external-config

  linux arm/arm-neon:
    Script must be run inside of ChromeOS SimpleChrome setup:
        cros chrome-sdk --board=arm-generic

  linux arm64:
    Script can run on a normal Ubuntu with AArch64 cross-toolchain in $PATH.

  mac:
    Script must be run on OSX.  Additionally, ensure the Chromium (not Apple)
    version of clang is in the path; usually found under
    src/third_party/llvm-build/Release+Asserts/bin

  win:
    Script must be run on Windows with VS2013 or higher under Cygwin (or MinGW,
    but as of 1.0.11, it has serious performance issues with make which makes
    building take hours).

    Additionally, ensure you have the correct toolchain environment for building.
    The x86 toolchain environment is required for ia32 builds and the x64 one
    for x64 builds.  This can be verified by running "cl.exe" and checking if
    the version string ends with "for x64" or "for x86."

    Building on Windows also requires some additional Cygwin packages plus a
    wrapper script for converting Cygwin paths to DOS paths.
      - Add these packages at install time: diffutils, yasm, make, python.
      - Copy chromium/scripts/cygwin-wrapper to /usr/local/bin

Resulting binaries will be placed in:
  build.TARGET_ARCH.TARGET_OS/Chrome/out/
  build.TARGET_ARCH.TARGET_OS/ChromeOS/out/
  build.TARGET_ARCH.TARGET_OS/Chromium/out/
  build.TARGET_ARCH.TARGET_OS/ChromiumOS/out/
  """


def PrintAndCheckCall(argv, *args, **kwargs):
  print('Running %r' % argv)
  subprocess.check_call(argv, *args, **kwargs)


def DetermineHostOsAndArch():
  if platform.system() == 'Linux':
    host_os = 'linux'
  elif platform.system() == 'Darwin':
    host_os = 'mac'
  elif platform.system() == 'Windows' or 'CYGWIN_NT' in platform.system():
    host_os = 'win'
  else:
    return None

  if re.match(r'i.86', platform.machine()):
    host_arch = 'ia32'
  elif platform.machine() == 'x86_64' or platform.machine() == 'AMD64':
    host_arch = 'x64'
  elif platform.machine() == 'aarch64':
    host_arch = 'arm64'
  elif platform.machine().startswith('arm'):
    host_arch = 'arm'
  else:
    return None

  return (host_os, host_arch)


def GetDsoName(target_os, dso_name, dso_version):
  if target_os in ('linux', 'linux-noasm', 'android'):
    return 'lib%s.so.%s' % (dso_name, dso_version)
  elif target_os == 'mac':
    return 'lib%s.%s.dylib' % (dso_name, dso_version)
  elif target_os == 'win':
    return '%s-%s.dll' % (dso_name, dso_version)
  else:
    raise ValueError('Unexpected target_os %s' % target_os)


def RewriteFile(path, search, replace):
  with open(path) as f:
    contents = f.read()
  with open(path, 'w') as f:
    f.write(re.sub(search, replace, contents))


# Extracts the Android toolchain version and api level from the Android
# config.gni.  Returns (api level, api 64 level, toolchain version).
def GetAndroidApiLevelAndToolchainVersion():
  android_config_gni = os.path.join(CHROMIUM_ROOT_DIR, 'build', 'config',
                                    'android', 'config.gni')
  with open(android_config_gni, 'r') as f:
    gni_contents = f.read()
    api64_match = re.search('_android64_api_level\s*=\s*(\d{2})', gni_contents)
    api_match = re.search('_android_api_level\s*=\s*(\d{2})', gni_contents)
    toolchain_match = re.search('_android_toolchain_version\s*=\s*"([.\d]+)"',
                                gni_contents)
    if not api_match or not toolchain_match or not api64_match:
      raise Exception('Failed to find the android api level or toolchain '
                      'version in ' + android_config_gni)

    return (api_match.group(1), api64_match.group(1), toolchain_match.group(1))


# Sets up cross-compilation (regardless of host arch) for compiling Android.
# Returns the necessary configure flags as a list.
def SetupAndroidToolchain(target_arch):
  api_level, api64_level, toolchain_version = (
      GetAndroidApiLevelAndToolchainVersion())

  # Toolchain prefix misery, for when just one pattern is not enough :/
  toolchain_level = api_level
  sysroot_arch = target_arch
  toolchain_dir_prefix = target_arch
  toolchain_bin_prefix = target_arch
  if target_arch in ('arm', 'arm-neon'):
    toolchain_bin_prefix = toolchain_dir_prefix = 'arm-linux-androideabi'
    sysroot_arch = 'arm'
  elif target_arch == 'arm64':
    toolchain_level = api64_level
    toolchain_bin_prefix = toolchain_dir_prefix = 'aarch64-linux-android'
  elif target_arch == 'ia32':
    toolchain_dir_prefix = sysroot_arch = 'x86'
    toolchain_bin_prefix = 'i686-linux-android'
  elif target_arch == 'x64':
    toolchain_level = api64_level
    toolchain_dir_prefix = sysroot_arch = 'x86_64'
    toolchain_bin_prefix = 'x86_64-linux-android'
  elif target_arch == 'mipsel':
    sysroot_arch = 'mips'
    toolchain_bin_prefix = toolchain_dir_prefix = 'mipsel-linux-android'
  elif target_arch == 'mips64el':
    toolchain_level = api64_level
    sysroot_arch = 'mips64'
    toolchain_bin_prefix = toolchain_dir_prefix = 'mips64el-linux-android'

  sysroot = (NDK_ROOT_DIR + '/platforms/android-' + toolchain_level +
             '/arch-' + sysroot_arch)
  cross_prefix = (NDK_ROOT_DIR + '/toolchains/' + toolchain_dir_prefix + '-' +
                  toolchain_version + '/prebuilt/linux-x86_64/bin/' +
                  toolchain_bin_prefix + '-')

  return [
      '--enable-cross-compile',
      '--sysroot=' + sysroot,
      '--cross-prefix=' + cross_prefix,
      '--target-os=linux',
  ]


def BuildFFmpeg(target_os, target_arch, host_os, host_arch, parallel_jobs,
                config_only, config, configure_flags):
  config_dir = 'build.%s.%s/%s' % (target_arch, target_os, config)
  shutil.rmtree(config_dir, ignore_errors=True)
  os.makedirs(os.path.join(config_dir, 'out'))

  PrintAndCheckCall(
      [os.path.join(FFMPEG_DIR, 'configure')] + configure_flags, cwd=config_dir)

  if target_os in (host_os, host_os + '-noasm', 'android') and not config_only:
    libraries = [
        os.path.join('libavcodec', GetDsoName(target_os, 'avcodec', 57)),
        os.path.join('libavformat', GetDsoName(target_os, 'avformat', 57)),
        os.path.join('libavutil', GetDsoName(target_os, 'avutil', 55)),
    ]
    PrintAndCheckCall(
        ['make', '-j%d' % parallel_jobs] + libraries, cwd=config_dir)
    for lib in libraries:
      shutil.copy(os.path.join(config_dir, lib),
                  os.path.join(config_dir, 'out'))
  elif config_only:
    print('Skipping build step as requested.')
  else:
    print('Skipping compile as host configuration differs from target.\n'
          'Please compare the generated config.h with the previous version.\n'
          'You may also patch the script to properly cross-compile.\n'
          'Host OS : %s\n'
          'Target OS : %s\n'
          'Host arch : %s\n'
          'Target arch : %s\n' % (host_os, target_os, host_arch, target_arch))

  if target_arch in ('arm', 'arm-neon'):
    RewriteFile(
        os.path.join(config_dir, 'config.h'),
        r'(#define HAVE_VFP_ARGS [01])',
        r'/* \1 -- Disabled to allow softfp/hardfp selection at gyp time */')


def main(argv):
  parser = optparse.OptionParser(usage=USAGE)
  parser.add_option('--branding', action='append', dest='brandings',
                    choices=BRANDINGS,
                    help='Branding to build; determines e.g. supported codecs')
  parser.add_option('--config-only', action='store_true',
                    help='Skip the build step. Useful when a given platform '
                    'is not necessary for generate_gyp.py')
  options, args = parser.parse_args(argv)

  if len(args) < 2:
    parser.print_help()
    return 1

  target_os = args[0]
  target_arch = args[1]
  configure_args = args[2:]

  if target_os not in ('android', 'linux', 'linux-noasm', 'mac', 'win'):
    parser.print_help()
    return 1

  host_tuple = DetermineHostOsAndArch()
  if not host_tuple:
    print('Unrecognized host OS and architecture.', file=sys.stderr)
    return 1

  host_os, host_arch = host_tuple
  parallel_jobs = multiprocessing.cpu_count()

  if target_os == 'android' and host_os != 'linux' and host_arch != 'x64':
    print('Android cross compilation can only be done from a linux x64 host.')
    return 1

  print('System information:\n'
        'Host OS       : %s\n'
        'Target OS     : %s\n'
        'Host arch     : %s\n'
        'Target arch   : %s\n'
        'Parallel jobs : %d\n' % (
            host_os, target_os, host_arch, target_arch, parallel_jobs))

  configure_flags = collections.defaultdict(list)

  # Common configuration.  Note: --disable-everything does not in fact disable
  # everything, just non-library components such as decoders and demuxers.
  configure_flags['Common'].extend([
      '--disable-everything',
      '--disable-all',
      '--disable-doc',
      '--disable-htmlpages',
      '--disable-manpages',
      '--disable-podpages',
      '--disable-txtpages',
      '--disable-static',
      '--enable-avcodec',
      '--enable-avformat',
      '--enable-avutil',
      '--enable-fft',
      '--enable-rdft',
      '--enable-static',

      # Disable features.
      '--disable-bzlib',
      '--disable-error-resilience',
      '--disable-iconv',
      '--disable-lzo',
      '--disable-network',
      '--disable-schannel',
      '--disable-sdl',
      '--disable-symver',
      '--disable-xlib',
      '--disable-zlib',
      '--disable-securetransport',

      # Disable hardware decoding options which will sometimes turn on
      # via autodetect.
      '--disable-d3d11va',
      '--disable-dxva2',
      '--disable-vaapi',
      '--disable-vda',
      '--disable-vdpau',
      '--disable-videotoolbox',

      # Common codecs.
      '--enable-decoder=vorbis',
      '--enable-decoder=pcm_u8,pcm_s16le,pcm_s24le,pcm_s32le,pcm_f32le',
      '--enable-decoder=pcm_s16be,pcm_s24be,pcm_mulaw,pcm_alaw',
      '--enable-demuxer=ogg,matroska,wav',
      '--enable-parser=opus,vorbis',
  ])

  if target_os == 'android':
    configure_flags['Common'].extend([
        # --optflags doesn't append multiple entries, so set all at once.
        '--optflags="-Os"',
        '--enable-small',
    ])

    configure_flags['Common'].extend(SetupAndroidToolchain(target_arch))
  else:
    configure_flags['Common'].extend([
        # --optflags doesn't append multiple entries, so set all at once.
        '--optflags="-O2"',

        '--enable-decoder=theora,vp8',
        '--enable-parser=vp3,vp8',
    ])

  if target_os in ('linux', 'linux-noasm', 'android'):
    if target_arch == 'x64':
      if target_os == 'android':
        configure_flags['Common'].extend([
            '--arch=x86_64',
        ])
      if target_os != 'android':
        # TODO(krasin): move this to Common, when https://crbug.com/537368
        # is fixed and CFI is unblocked from launching on ChromeOS.
        configure_flags['EnableLTO'].extend(['--enable-lto'])
      pass
    elif target_arch == 'ia32':
      configure_flags['Common'].extend([
          '--arch=i686',
          '--extra-cflags="-m32"',
          '--extra-ldflags="-m32"',
      ])
      # Android ia32 can't handle textrels and ffmpeg can't compile without
      # them.  http://crbug.com/559379
      if target_os != 'android':
        configure_flags['Common'].extend([
          '--enable-yasm',
        ])
      else:
        configure_flags['Common'].extend([
          '--disable-yasm',
        ])
    elif target_arch == 'arm' or target_arch == 'arm-neon':
      # TODO(ihf): ARM compile flags are tricky. The final options
      # overriding everything live in chroot /build/*/etc/make.conf
      # (some of them coming from src/overlays/overlay-<BOARD>/make.conf).
      # We try to follow these here closely. In particular we need to
      # set ffmpeg internal #defines to conform to make.conf.
      # TODO(ihf): For now it is not clear if thumb or arm settings would be
      # faster. I ran experiments in other contexts and performance seemed
      # to be close and compiler version dependent. In practice thumb builds are
      # much smaller than optimized arm builds, hence we go with the global
      # CrOS settings.
      configure_flags['Common'].extend([
          '--arch=arm',
          '--enable-armv6',
          '--enable-armv6t2',
          '--enable-vfp',
          '--enable-thumb',
          '--extra-cflags=-march=armv7-a',
      ])

      if target_os == 'android':
        configure_flags['Common'].extend([
            # Runtime neon detection requires /proc/cpuinfo access, so ensure
            # av_get_cpu_flags() is run outside of the sandbox when enabled.
            '--enable-neon',
            '--extra-cflags=-mtune=generic-armv7-a',
            # NOTE: softfp/hardfp selected at gyp time.
            '--extra-cflags=-mfloat-abi=softfp',
        ])
        if target_arch == 'arm-neon':
          configure_flags['Common'].extend([
              '--extra-cflags=-mfpu=neon',
          ])
        else:
          configure_flags['Common'].extend([
              '--extra-cflags=-mfpu=vfpv3-d16',
          ])
      else:
        configure_flags['Common'].extend([
            # Location is for CrOS chroot. If you want to use this, enter chroot
            # and copy ffmpeg to a location that is reachable.
            '--enable-cross-compile',
            '--target-os=linux',
            '--cross-prefix=armv7a-cros-linux-gnueabi-',
            '--extra-cflags=-mtune=cortex-a8',
            # NOTE: softfp/hardfp selected at gyp time.
            '--extra-cflags=-mfloat-abi=hard',
        ])

        if target_arch == 'arm-neon':
          configure_flags['Common'].extend([
              '--enable-neon',
              '--extra-cflags=-mfpu=neon',
          ])
        else:
          configure_flags['Common'].extend([
              '--disable-neon',
              '--extra-cflags=-mfpu=vfpv3-d16',
          ])
    elif target_arch == 'arm64':
      if target_os != 'android':
        configure_flags['Common'].extend([
            '--enable-cross-compile',
            '--cross-prefix=/usr/bin/aarch64-linux-gnu-',
            '--target-os=linux',
        ])
      configure_flags['Common'].extend([
          '--arch=aarch64',
          '--enable-armv8',
          '--extra-cflags=-march=armv8-a',
      ])
    elif target_arch == 'mipsel':
      if target_os != 'android':
        configure_flags['Common'].extend([
            '--enable-cross-compile',
            '--cross-prefix=mipsel-cros-linux-gnu-',
            '--target-os=linux',
            '--extra-cflags=-EL',
            '--extra-ldflags=-EL',
            '--extra-ldflags=-mips32',
        ])
      else:
        configure_flags['Common'].extend([
            '--extra-cflags=-mhard-float',
        ])
      configure_flags['Common'].extend([
          '--arch=mips',
          '--extra-cflags=-mips32',
          '--disable-mipsfpu',
          '--disable-mipsdsp',
          '--disable-mipsdspr2',
      ])
    elif target_arch == 'mips64el' and target_os == "android":
      configure_flags['Common'].extend([
          '--arch=mips',
          '--cpu=i6400',
          '--extra-cflags=-mhard-float',
          '--extra-cflags=-mips64r6',
          '--disable-msa',
      ])
    else:
      print('Error: Unknown target arch %r for target OS %r!' % (
          target_arch, target_os), file=sys.stderr)
      return 1

  if target_os == 'linux-noasm':
    configure_flags['Common'].extend([
        '--disable-asm',
        '--disable-inline-asm',
    ])

  if 'win' not in target_os:
    configure_flags['Common'].append('--enable-pic')

  # Should be run on Mac.
  if target_os == 'mac':
    if host_os != 'mac':
      print('Script should be run on a Mac host. If this is not possible\n'
            'try a merge of config files with new linux ia32 config.h\n'
            'by hand.\n', file=sys.stderr)
      return 1

    configure_flags['Common'].extend([
        '--enable-yasm',
        '--cc=clang',
        '--cxx=clang++',
    ])
    if target_arch == 'x64':
      configure_flags['Common'].extend([
          '--arch=x86_64',
          '--extra-cflags=-m64',
          '--extra-ldflags=-m64',
      ])
    else:
      print('Error: Unknown target arch %r for target OS %r!' % (
          target_arch, target_os), file=sys.stderr)

  # Should be run on Windows.
  if target_os == 'win':
    if host_os != 'win':
      print('Script should be run on a Windows host.\n', file=sys.stderr)
      return 1

    configure_flags['Common'].extend([
        '--toolchain=msvc',
        '--enable-yasm',
        '--extra-cflags=-I' + os.path.join(FFMPEG_DIR, 'chromium/include/win'),
    ])

    if 'CYGWIN_NT' in platform.system():
      configure_flags['Common'].extend([
          '--cc=cygwin-wrapper cl',
          '--ld=cygwin-wrapper link',
          '--nm=cygwin-wrapper dumpbin -symbols',
          '--ar=cygwin-wrapper lib',
      ])

  # Google Chrome & ChromeOS specific configuration.
  configure_flags['Chrome'].extend([
      '--enable-decoder=aac,h264,mp3',
      '--enable-demuxer=aac,mp3,mov',
      '--enable-parser=aac,h264,mpegaudio',
  ])

  # ChromiumOS specific configuration.
  # Warning: do *NOT* add avi, aac, h264, mp3, mp4, amr*
  # Flac support.
  configure_flags['ChromiumOS'].extend([
      '--enable-demuxer=flac',
      '--enable-decoder=flac',
      '--enable-parser=flac',
  ])

  # Google ChromeOS specific configuration.
  # We want to make sure to play everything Android generates and plays.
  # http://developer.android.com/guide/appendix/media-formats.html
  configure_flags['ChromeOS'].extend([
      # Enable playing avi files.
      '--enable-decoder=mpeg4',
      '--enable-parser=h263,mpeg4video',
      '--enable-demuxer=avi',
      # Enable playing Android 3gp files.
      '--enable-demuxer=amr',
      '--enable-decoder=amrnb,amrwb',
      # Flac support.
      '--enable-demuxer=flac',
      '--enable-decoder=flac',
      '--enable-parser=flac',
      # Wav files for playing phone messages.
      '--enable-decoder=gsm_ms',
      '--enable-demuxer=gsm',
      '--enable-parser=gsm',
  ])

  configure_flags['ChromeAndroid'].extend([
      '--enable-demuxer=aac,mp3,mov',
      '--enable-parser=aac,mpegaudio',
      '--enable-decoder=aac,mp3',

      # TODO(dalecurtis, watk): Figure out if we need h264 parser for now?
  ])

  def do_build_ffmpeg(branding, configure_flags):
    if options.brandings and branding not in options.brandings:
      print('%s skipped' % branding)
      return

    print('%s configure/build:' % branding)
    BuildFFmpeg(target_os, target_arch, host_os, host_arch, parallel_jobs,
                options.config_only, branding, configure_flags)

  # Only build Chromium, Chrome for ia32, x86 non-android platforms.
  if target_os != 'android':
    do_build_ffmpeg('Chromium',
                    configure_flags['Common'] +
                    configure_flags['Chromium'] +
                    configure_flags['EnableLTO'] +
                    configure_args)
    do_build_ffmpeg('Chrome',
                    configure_flags['Common'] +
                    configure_flags['Chrome'] +
                    configure_flags['EnableLTO'] +
                    configure_args)
  else:
    do_build_ffmpeg('Chromium',
                    configure_flags['Common'] +
                    configure_args)
    do_build_ffmpeg('Chrome',
                    configure_flags['Common'] +
                    configure_flags['ChromeAndroid'] +
                    configure_args)

  if target_os in ['linux', 'linux-noasm']:
    do_build_ffmpeg('ChromiumOS',
                    configure_flags['Common'] +
                    configure_flags['Chromium'] +
                    configure_flags['ChromiumOS'] +
                    configure_args)
    # ChromeOS enables MPEG4 which requires error resilience :(
    chrome_os_flags = (configure_flags['Common'] +
                       configure_flags['Chrome'] +
                       configure_flags['ChromeOS'] +
                       configure_args)
    chrome_os_flags.remove('--disable-error-resilience')
    do_build_ffmpeg('ChromeOS', chrome_os_flags)

  print('Done. If desired you may copy config.h/config.asm into the '
        'source/config tree using copy_config.sh.')
  return 0


if __name__ == '__main__':
  sys.exit(main(sys.argv[1:]))
