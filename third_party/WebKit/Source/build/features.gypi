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
  # The following defines turn WebKit features on and off.
  #
  # ====================================
  #
  #     Keep the GN version in Sync:
  #     Source/config.gni
  #
  # ====================================
  'variables': {
    'feature_defines': [
      'ENABLE_LAYOUT_UNIT_IN_INLINE_BOXES=0',
      # WTF_USE_DYNAMIC_ANNOTATIONS=1 may be defined in build/common.gypi
      # We can't define it here because it should be present only
      # in Debug or release_valgrind_build=1 builds.
    ],
    # We have to nest variables inside variables so that they can be overridden
    # through GYP_DEFINES.
    'variables': {
      # Enables the Oilpan garbage-collection infrastructure.
      'enable_oilpan%': 0,
      'blink_gc_profiling%': 0,
      'blink_logging_always_on%': 0,
      'link_core_modules_separately%': 1,
    },
    'conditions': [
      ['use_concatenated_impulse_responses==1', {
        # Use concatenated HRTF impulse responses
        'feature_defines': ['WTF_USE_CONCATENATED_IMPULSE_RESPONSES=1'],
      }],
      ['OS!="android"', {
        'feature_defines': [
          'ENABLE_INPUT_MULTIPLE_FIELDS_UI=1',
          'ENABLE_WEB_AUDIO=1'
        ],
      }],
      # Mac OS X uses Accelerate.framework FFT by default instead of FFmpeg.
      ['OS!="mac" and OS!="android"', {
        'feature_defines': [
          'WTF_USE_WEBAUDIO_FFMPEG=1',
        ],
      }],
      ['OS=="android" and use_openmax_dl_fft!=0', {
        'feature_defines': [
          'WTF_USE_WEBAUDIO_OPENMAX_DL_FFT=1',
          # Enabling the FFT is enough to enable WebAudio support to
          # allow most WebAudio features to work on Android.
          'ENABLE_WEB_AUDIO=1',
        ],
      }],
      ['use_default_render_theme==1', {
        'feature_defines': [
          'WTF_USE_DEFAULT_RENDER_THEME=1',
        ],
      }],
      ['enable_oilpan==1', {
        'feature_defines': [
          'ENABLE_OILPAN=1',
          'ENABLE_LAZY_SWEEPING=1',
          'ENABLE_IDLE_GC=1',
        ],
      }],
      ['enable_oilpan==0', {
        'feature_defines': [
          'ENABLE_LAZY_SWEEPING=1',
          'ENABLE_IDLE_GC=1',
        ],
      }],
      ['blink_gc_profiling==1', {
        'feature_defines': [
          'ENABLE_GC_PROFILING=1',
        ],
      }],
      ['blink_logging_always_on==1', {
        'feature_defines': [
          'LOG_DISABLED=0',
        ],
      }],
      ['link_core_modules_separately==1 and component=="shared_library"', {
        'feature_defines': [
          'LINK_CORE_MODULES_SEPARATELY',
        ],
      }],
    ],

    # shared build only. If set to 1, link web, core and modules separately.
    'link_core_modules_separately%': '<(link_core_modules_separately)',
  },
}
