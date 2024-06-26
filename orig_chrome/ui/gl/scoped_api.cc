// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/scoped_api.h"

#include "ui/gl/gl_gl_api_implementation.h"

namespace gfx {

ScopedSetGLToRealGLApi::ScopedSetGLToRealGLApi()
    : old_gl_api_(GetCurrentGLApi())
{
    SetGLToRealGLApi();
}

ScopedSetGLToRealGLApi::~ScopedSetGLToRealGLApi()
{
    SetGLApi(old_gl_api_);
}

} // namespace gfx
