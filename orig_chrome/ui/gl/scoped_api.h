// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_SCOPED_API_H_
#define UI_GL_SCOPED_API_H_

#include "base/basictypes.h"
#include "ui/gl/gl_export.h"

namespace gfx {

class GLApi;

class GL_EXPORT ScopedSetGLToRealGLApi {
public:
    ScopedSetGLToRealGLApi();
    ~ScopedSetGLToRealGLApi();

private:
    GLApi* old_gl_api_;

    DISALLOW_COPY_AND_ASSIGN(ScopedSetGLToRealGLApi);
};

} // namespace gfx

#endif // UI_GL_SCOPED_API_H_
