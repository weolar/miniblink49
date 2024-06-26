// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/command_line.h"
#include "base/memory/scoped_ptr.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_egl_api_implementation.h"
#include "ui/gl/gl_switches.h"

namespace gfx {

class EGLApiTest : public testing::Test {
public:
    void SetUp() override
    {
        fake_client_extension_string_ = "";
        fake_extension_string_ = "";

        // TODO(dyen): Add a way to bind mock drivers for testing.
        g_driver_egl.ClearBindings();
        g_driver_egl.fn.eglInitializeFn = &FakeInitialize;
        g_driver_egl.fn.eglQueryStringFn = &FakeQueryString;
        g_driver_egl.fn.eglGetCurrentDisplayFn = &FakeGetCurrentDisplay;
        g_driver_egl.fn.eglGetDisplayFn = &FakeGetDisplay;
        g_driver_egl.fn.eglGetErrorFn = &FakeGetError;
    }

    void TearDown() override
    {
        g_current_egl_context = nullptr;
        api_.reset(nullptr);
        g_driver_egl.ClearBindings();

        fake_client_extension_string_ = "";
        fake_extension_string_ = "";
    }

    void InitializeAPI(base::CommandLine* command_line)
    {
        api_.reset(new RealEGLApi());
        g_current_egl_context = api_.get();
        if (command_line)
            api_->InitializeWithCommandLine(&g_driver_egl, command_line);
        else
            api_->Initialize(&g_driver_egl);
        g_driver_egl.InitializeExtensionBindings();
    }

    void SetFakeExtensionString(const char* fake_string,
        const char* fake_client_string)
    {
        fake_extension_string_ = fake_string;
        fake_client_extension_string_ = fake_client_string;
    }

    static EGLBoolean GL_BINDING_CALL FakeInitialize(EGLDisplay display,
        EGLint* major,
        EGLint* minor)
    {
        return EGL_TRUE;
    }

    static const char* GL_BINDING_CALL FakeQueryString(EGLDisplay dpy,
        EGLint name)
    {
        if (dpy == EGL_NO_DISPLAY) {
            return fake_client_extension_string_;
        } else {
            return fake_extension_string_;
        }
    }

    static EGLDisplay GL_BINDING_CALL FakeGetCurrentDisplay()
    {
        return reinterpret_cast<EGLDisplay>(0x1);
    }

    static EGLDisplay GL_BINDING_CALL FakeGetDisplay(
        EGLNativeDisplayType native_display)
    {
        return reinterpret_cast<EGLDisplay>(0x1);
    }

    static EGLint GL_BINDING_CALL FakeGetError()
    {
        return EGL_SUCCESS;
    }

    std::pair<const char*, const char*> GetExtensions()
    {
        return std::make_pair(
            api_->eglQueryStringFn(EGL_NO_DISPLAY, EGL_EXTENSIONS),
            api_->eglQueryStringFn(api_->eglGetCurrentDisplayFn(), EGL_EXTENSIONS));
    }

protected:
    static const char* fake_extension_string_;
    static const char* fake_client_extension_string_;

    scoped_ptr<RealEGLApi> api_;
};

const char* EGLApiTest::fake_extension_string_ = "";
const char* EGLApiTest::fake_client_extension_string_ = "";

TEST_F(EGLApiTest, DisabledExtensionBitTest)
{
    static const char* kFakeExtensions = "EGL_KHR_fence_sync";
    static const char* kFakeClientExtensions = "";
    static const char* kFakeDisabledExtensions = "EGL_KHR_fence_sync";

    SetFakeExtensionString(kFakeExtensions, kFakeClientExtensions);
    InitializeAPI(nullptr);

    EXPECT_TRUE(g_driver_egl.ext.b_EGL_KHR_fence_sync);

    base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
    command_line.AppendSwitchASCII(switches::kDisableGLExtensions,
        kFakeDisabledExtensions);
    InitializeAPI(&command_line);

    EXPECT_FALSE(g_driver_egl.ext.b_EGL_KHR_fence_sync);
}

TEST_F(EGLApiTest, DisabledExtensionStringTest)
{
    static const char* kFakeExtensions = "EGL_EXT_1 EGL_EXT_2"
                                         " EGL_EXT_3 EGL_EXT_4";
    static const char* kFakeClientExtensions = "EGL_CLIENT_EXT_1 EGL_CLIENT_EXT_2";
    static const char* kFakeDisabledExtensions = "EGL_EXT_1,EGL_EXT_2,EGL_FAKE,EGL_CLIENT_EXT_1";
    static const char* kFilteredExtensions = "EGL_EXT_3 EGL_EXT_4";
    static const char* kFilteredClientExtensions = "EGL_CLIENT_EXT_2";

    SetFakeExtensionString(kFakeExtensions, kFakeClientExtensions);
    InitializeAPI(nullptr);

    EXPECT_STREQ(kFakeClientExtensions, GetExtensions().first);
    EXPECT_STREQ(kFakeExtensions, GetExtensions().second);

    base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
    command_line.AppendSwitchASCII(switches::kDisableGLExtensions,
        kFakeDisabledExtensions);
    InitializeAPI(&command_line);

    EXPECT_STREQ(kFilteredClientExtensions, GetExtensions().first);
    EXPECT_STREQ(kFilteredExtensions, GetExtensions().second);
}

} // namespace gfx
