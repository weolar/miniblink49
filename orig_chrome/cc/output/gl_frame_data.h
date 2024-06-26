// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_GL_FRAME_DATA_H_
#define CC_OUTPUT_GL_FRAME_DATA_H_

#include <string>

#include "base/basictypes.h"
#include "cc/base/cc_export.h"
#include "gpu/command_buffer/common/mailbox.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

class CC_EXPORT GLFrameData {
public:
    GLFrameData();
    ~GLFrameData();

    gpu::Mailbox mailbox;
    uint32 sync_point;
    gfx::Size size;
    gfx::Rect sub_buffer_rect;
};

} // namespace cc

#endif // CC_OUTPUT_GL_FRAME_DATA_H_
