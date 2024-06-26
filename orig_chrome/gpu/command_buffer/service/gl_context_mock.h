// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_GL_CONTEXT_MOCK_H_
#define GPU_COMMAND_BUFFER_SERVICE_GL_CONTEXT_MOCK_H_

#include "testing/gmock/include/gmock/gmock.h"
#include "ui/gl/gl_context_stub_with_extensions.h"

namespace gpu {

class GLContextMock : public gfx::GLContextStubWithExtensions {
public:
    GLContextMock();

    MOCK_METHOD1(MakeCurrent, bool(gfx::GLSurface* surface));

protected:
    virtual ~GLContextMock();
};

} // namespace gpu

#endif // GPU_COMMAND_BUFFER_SERVICE_GL_CONTEXT_MOCK_H_
