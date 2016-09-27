# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'dependencies': [
    '../third_party/re2/re2.gyp:re2',
    '../base/base.gyp:base',
    '../ui/gl/gl.gyp:gl',
  ],
  'include_dirs': [
    '..',
  ],
  'sources': [
    'config/dx_diag_node.cc',
    'config/dx_diag_node.h',
    'config/gpu_blacklist.cc',
    'config/gpu_blacklist.h',
    'config/gpu_control_list_jsons.h',
    'config/gpu_control_list.cc',
    'config/gpu_control_list.h',
    'config/gpu_driver_bug_list_json.cc',
    'config/gpu_driver_bug_list.cc',
    'config/gpu_driver_bug_list.h',
    'config/gpu_driver_bug_workaround_type.h',
    'config/gpu_dx_diagnostics_win.cc',
    'config/gpu_feature_type.h',
    'config/gpu_info.cc',
    'config/gpu_info.h',
    'config/gpu_info_collector_android.cc',
    'config/gpu_info_collector_mac.mm',
    'config/gpu_info_collector_ozone.cc',
    'config/gpu_info_collector_win.cc',
    'config/gpu_info_collector_x11.cc',
    'config/gpu_info_collector.cc',
    'config/gpu_info_collector.h',
    'config/gpu_performance_stats.h',
    'config/gpu_test_config.cc',
    'config/gpu_test_config.h',
    'config/gpu_test_expectations_parser.cc',
    'config/gpu_test_expectations_parser.h',
    'config/gpu_util.cc',
    'config/gpu_util.h',
    'config/software_rendering_list_json.cc',
    'config/gpu_bug_manager.cc',
    'config/gpu_bug_manager.h',
  ],
  'conditions': [
    ['OS=="win"', {
      'dependencies': [
        '../third_party/libxml/libxml.gyp:libxml',
      ],
      'link_settings': {
        'libraries': [
          '-ldxguid.lib',
          '-lsetupapi.lib',
        ],
      },
      # TODO(jschuh): crbug.com/167187 fix size_t to int truncations.
      'msvs_disabled_warnings': [ 4267, ],
    }],
    ['OS=="win" and branding=="Chrome"', {
      'sources': [
        '../third_party/amd/AmdCfxPxExt.h',
        '../third_party/amd/amd_videocard_info_win.cc',
      ],
    }],
    ['OS=="linux" and use_x11==1', {
      'dependencies': [
        '../build/linux/system.gyp:x11',
        '../build/linux/system.gyp:xext',
        '../build/linux/system.gyp:libpci',
        '../third_party/libXNVCtrl/libXNVCtrl.gyp:libXNVCtrl',
      ],
    }],
  ],
}
