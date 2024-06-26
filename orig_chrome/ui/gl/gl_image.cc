// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_image.h"

#include "base/logging.h"

namespace gfx {

GLImage::GLImage() { }

void GLImage::SetReleaseAfterUse()
{
    // Default no-op implementation for workaround.
}

GLImage::~GLImage() { }

} // namespace gfx
