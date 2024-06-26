// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_image_stub.h"

#include <GL/gl.h>

namespace gl {

GLImageStub::GLImageStub() { }

GLImageStub::~GLImageStub() { }

gfx::Size GLImageStub::GetSize()
{
    return gfx::Size(1, 1);
}

unsigned GLImageStub::GetInternalFormat() { return GL_RGBA; }

bool GLImageStub::BindTexImage(unsigned target) { return true; }

bool GLImageStub::CopyTexImage(unsigned target)
{
    return true;
}

bool GLImageStub::CopyTexSubImage(unsigned target,
    const gfx::Point& offset,
    const gfx::Rect& rect)
{
    return true;
}

bool GLImageStub::ScheduleOverlayPlane(gfx::AcceleratedWidget widget,
    int z_order,
    gfx::OverlayTransform transform,
    const gfx::Rect& bounds_rect,
    const gfx::RectF& crop_rect)
{
    return false;
}

} // namespace gl
