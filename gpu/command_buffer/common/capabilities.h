// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_COMMON_CAPABILITIES_H_
#define GPU_COMMAND_BUFFER_COMMON_CAPABILITIES_H_

#include "gpu/gpu_export.h"

namespace gpu {

struct GPU_EXPORT Capabilities {
  bool post_sub_buffer;
  bool fast_npot_mo8_textures;
  bool egl_image_external;
  bool texture_format_bgra8888;
  bool texture_format_etc1;
  bool texture_rectangle;
  bool iosurface;
  bool texture_usage;
  bool texture_storage;
  bool discard_framebuffer;
  bool sync_query;
  bool map_image;

  Capabilities();
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_COMMON_CAPABILITIES_H_
