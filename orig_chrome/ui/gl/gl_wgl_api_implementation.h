// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_WGL_API_IMPLEMENTATION_H_
#define UI_GL_GL_WGL_API_IMPLEMENTATION_H_

#include <vector>

#include "base/compiler_specific.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_export.h"

namespace base {
class CommandLine;
}
namespace gfx {

class GLContext;
struct GLWindowSystemBindingInfo;

void InitializeStaticGLBindingsWGL();
void InitializeDebugGLBindingsWGL();
void ClearGLBindingsWGL();
bool GetGLWindowSystemBindingInfoWGL(GLWindowSystemBindingInfo* info);

class GL_EXPORT WGLApiBase : public WGLApi {
public:
// Include the auto-generated part of this class. We split this because
// it means we can easily edit the non-auto generated parts right here in
// this file instead of having to edit some template or the code generator.
#include "gl_bindings_api_autogen_wgl.h"

protected:
    WGLApiBase();
    ~WGLApiBase() override;
    void InitializeBase(DriverWGL* driver);

    DriverWGL* driver_;
};

class GL_EXPORT RealWGLApi : public WGLApiBase {
public:
    RealWGLApi();
    ~RealWGLApi() override;
    void Initialize(DriverWGL* driver);
    void InitializeWithCommandLine(DriverWGL* driver,
        base::CommandLine* command_line);

    const char* wglGetExtensionsStringARBFn(HDC hDC) override;
    const char* wglGetExtensionsStringEXTFn() override;

private:
    std::vector<std::string> disabled_exts_;
    std::string filtered_arb_exts_;
    std::string filtered_ext_exts_;
};

// Inserts a TRACE for every WGL call.
class GL_EXPORT TraceWGLApi : public WGLApi {
public:
    TraceWGLApi(WGLApi* wgl_api)
        : wgl_api_(wgl_api)
    {
    }
    ~TraceWGLApi() override;

// Include the auto-generated part of this class. We split this because
// it means we can easily edit the non-auto generated parts right here in
// this file instead of having to edit some template or the code generator.
#include "gl_bindings_api_autogen_wgl.h"

private:
    WGLApi* wgl_api_;
};

} // namespace gfx

#endif // UI_GL_GL_WGL_API_IMPLEMENTATION_H_
