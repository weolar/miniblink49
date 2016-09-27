// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/common/capabilities.h"

namespace gpu {

Capabilities::Capabilities()
    : post_sub_buffer(false),
      fast_npot_mo8_textures(false),
      egl_image_external(false),
      texture_format_bgra8888(false),
      texture_format_etc1(false),
      texture_rectangle(false),
      iosurface(false),
      texture_usage(false),
      texture_storage(false),
      discard_framebuffer(false),
      sync_query(false),
      map_image(false) {}

}  // namespace gpu
