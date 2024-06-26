// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/command_line.h"
#include "base/memory/scoped_ptr.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_glx_api_implementation.h"
#include "ui/gl/gl_implementation.h"

namespace gfx {

class GLXApiTest : public testing::Test {
public:
    void SetUp() override
    {
        fake_extension_string_ = "";

        g_driver_glx.ClearBindings();
        g_driver_glx.fn.glXQueryExtensionsStringFn = &FakeQueryExtensionsString;
        SetGLImplementation(kGLImplementationMockGL);
        SetGLGetProcAddressProc(
            static_cast<GLGetProcAddressProc>(&FakeGLGetProcAddress));
    }

    void TearDown() override
    {
        g_current_glx_context = nullptr;
        api_.reset(nullptr);
        g_driver_glx.ClearBindings();

        fake_extension_string_ = "";
    }

    void InitializeAPI(base::CommandLine* command_line)
    {
        api_.reset(new RealGLXApi());
        g_current_glx_context = api_.get();
        if (command_line)
            api_->InitializeWithCommandLine(&g_driver_glx, command_line);
        else
            api_->Initialize(&g_driver_glx);
        g_driver_glx.InitializeExtensionBindings();
    }

    void SetFakeExtensionString(const char* fake_string)
    {
        fake_extension_string_ = fake_string;
    }

    const char* GetExtensions()
    {
        return api_->glXQueryExtensionsStringFn(reinterpret_cast<Display*>(0x1), 0);
    }

    static GLXContext FakeCreateContextAttribsARB(Display* dpy,
        GLXFBConfig config,
        GLXContext share_context,
        int direct,
        const int* attrib_list)
    {
        return static_cast<GLXContext>(nullptr);
    }

    static void* GL_BINDING_CALL FakeGLGetProcAddress(const char* proc)
    {
        if (!strcmp("glXCreateContextAttribsARB", proc)) {
            return reinterpret_cast<void*>(&FakeCreateContextAttribsARB);
        }
        return NULL;
    }

    static const char* GL_BINDING_CALL FakeQueryExtensionsString(Display* dpy,
        int screen)
    {
        return fake_extension_string_;
    }

protected:
    static const char* fake_extension_string_;

    scoped_ptr<RealGLXApi> api_;
};

const char* GLXApiTest::fake_extension_string_ = "";

TEST_F(GLXApiTest, DisabledExtensionBitTest)
{
    static const char* kFakeExtensions = "GLX_ARB_create_context";
    static const char* kFakeDisabledExtensions = "GLX_ARB_create_context";

    SetFakeExtensionString(kFakeExtensions);
    InitializeAPI(nullptr);

    EXPECT_TRUE(g_driver_glx.ext.b_GLX_ARB_create_context);

    base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
    command_line.AppendSwitchASCII(switches::kDisableGLExtensions,
        kFakeDisabledExtensions);
    InitializeAPI(&command_line);

    EXPECT_FALSE(g_driver_glx.ext.b_GLX_ARB_create_context);
}

TEST_F(GLXApiTest, DisabledExtensionStringTest)
{
    static const char* kFakeExtensions = "EGL_EXT_1 EGL_EXT_2"
                                         " EGL_EXT_3 EGL_EXT_4";
    static const char* kFakeDisabledExtensions = "EGL_EXT_1,EGL_EXT_2,EGL_FAKE";
    static const char* kFilteredExtensions = "EGL_EXT_3 EGL_EXT_4";

    SetFakeExtensionString(kFakeExtensions);
    InitializeAPI(nullptr);

    EXPECT_STREQ(kFakeExtensions, GetExtensions());

    base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
    command_line.AppendSwitchASCII(switches::kDisableGLExtensions,
        kFakeDisabledExtensions);
    InitializeAPI(&command_line);

    EXPECT_STREQ(kFilteredExtensions, GetExtensions());
}

} // namespace gfx
