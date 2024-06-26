// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>

#include "base/command_line.h"
#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_gl_api_implementation.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_switches.h"
#include "ui/gl/gpu_timing.h"

namespace gfx {

class GLContextFake : public GLContext {
public:
    bool Initialize(GLSurface* compatible_surface,
        GpuPreference gpu_preference) override
    {
        return true;
    }
    bool MakeCurrent(GLSurface* surface) override { return true; }
    void ReleaseCurrent(GLSurface* surface) override { }
    bool IsCurrent(GLSurface* surface) override { return true; }
    void* GetHandle() override { return NULL; }
    scoped_refptr<gfx::GPUTimingClient> CreateGPUTimingClient() override
    {
        return NULL;
    }
    void OnSetSwapInterval(int interval) override { }
    GLContextFake()
        : GLContext(NULL)
    {
    }

protected:
    ~GLContextFake() override { }
};

class GLApiTest : public testing::Test {
public:
    void SetUp() override
    {
        fake_extension_string_ = "";
        fake_version_string_ = "";
        num_fake_extension_strings_ = 0;
        fake_extension_strings_ = nullptr;

        DCHECK(!g_current_gl_context_tls);
        g_current_gl_context_tls = new base::ThreadLocalPointer<GLApi>;

        SetGLGetProcAddressProc(
            static_cast<GLGetProcAddressProc>(&FakeGLGetProcAddress));
    }

    static void* GL_BINDING_CALL FakeGLGetProcAddress(const char* proc)
    {
        return reinterpret_cast<void*>(0x1);
    }

    void TearDown() override
    {
        api_.reset(nullptr);
        delete g_current_gl_context_tls;
        g_current_gl_context_tls = nullptr;

        SetGLImplementation(kGLImplementationNone);
        fake_extension_string_ = "";
        fake_version_string_ = "";
        num_fake_extension_strings_ = 0;
        fake_extension_strings_ = nullptr;
    }

    void InitializeAPI(base::CommandLine* command_line)
    {
        api_.reset(new RealGLApi());
        g_current_gl_context_tls->Set(api_.get());

        g_driver_gl.ClearBindings();
        g_driver_gl.fn.glGetStringFn = &FakeGetString;
        g_driver_gl.fn.glGetStringiFn = &FakeGetStringi;
        g_driver_gl.fn.glGetIntegervFn = &FakeGetIntegervFn;

        fake_context_ = new GLContextFake();
        if (command_line)
            api_->InitializeWithCommandLine(&g_driver_gl, command_line);
        else
            api_->Initialize(&g_driver_gl);
        api_->InitializeFilteredExtensions();
        g_driver_gl.InitializeCustomDynamicBindings(fake_context_.get());
    }

    void SetFakeExtensionString(const char* fake_string)
    {
        SetGLImplementation(kGLImplementationDesktopGL);
        fake_extension_string_ = fake_string;
        fake_version_string_ = "2.1";
    }

    void SetFakeExtensionStrings(const char** fake_strings, uint32_t count)
    {
        SetGLImplementation(kGLImplementationDesktopGL);
        num_fake_extension_strings_ = count;
        fake_extension_strings_ = fake_strings;
        fake_version_string_ = "3.0";
    }

    static const GLubyte* GL_BINDING_CALL FakeGetString(GLenum name)
    {
        if (name == GL_VERSION)
            return reinterpret_cast<const GLubyte*>(fake_version_string_);
        return reinterpret_cast<const GLubyte*>(fake_extension_string_);
    }

    static void GL_BINDING_CALL FakeGetIntegervFn(GLenum pname, GLint* params)
    {
        *params = num_fake_extension_strings_;
    }

    static const GLubyte* GL_BINDING_CALL FakeGetStringi(GLenum name,
        GLuint index)
    {
        return (index < num_fake_extension_strings_) ? reinterpret_cast<const GLubyte*>(fake_extension_strings_[index]) : nullptr;
    }

    const char* GetExtensions()
    {
        return reinterpret_cast<const char*>(api_->glGetStringFn(GL_EXTENSIONS));
    }

    uint32_t GetNumExtensions()
    {
        GLint num_extensions = 0;
        api_->glGetIntegervFn(GL_NUM_EXTENSIONS, &num_extensions);
        return static_cast<uint32_t>(num_extensions);
    }

    const char* GetExtensioni(uint32_t index)
    {
        return reinterpret_cast<const char*>(api_->glGetStringiFn(GL_EXTENSIONS,
            index));
    }

protected:
    static const char* fake_extension_string_;
    static const char* fake_version_string_;

    static uint32_t num_fake_extension_strings_;
    static const char** fake_extension_strings_;

    scoped_refptr<GLContext> fake_context_;
    scoped_ptr<DriverGL> driver_;
    scoped_ptr<RealGLApi> api_;
};

const char* GLApiTest::fake_extension_string_ = "";
const char* GLApiTest::fake_version_string_ = "";

uint32_t GLApiTest::num_fake_extension_strings_ = 0;
const char** GLApiTest::fake_extension_strings_ = nullptr;

TEST_F(GLApiTest, DisabledExtensionStringTest)
{
    static const char* kFakeExtensions = "GL_EXT_1 GL_EXT_2 GL_EXT_3 GL_EXT_4";
    static const char* kFakeDisabledExtensions = "GL_EXT_1,GL_EXT_2,GL_FAKE";
    static const char* kFilteredExtensions = "GL_EXT_3 GL_EXT_4";

    SetFakeExtensionString(kFakeExtensions);
    InitializeAPI(nullptr);

    EXPECT_STREQ(kFakeExtensions, GetExtensions());

    base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
    command_line.AppendSwitchASCII(switches::kDisableGLExtensions,
        kFakeDisabledExtensions);
    InitializeAPI(&command_line);

    EXPECT_STREQ(kFilteredExtensions, GetExtensions());
}

TEST_F(GLApiTest, DisabledExtensionBitTest)
{
    static const char* kFakeExtensions[] = {
        "GL_ARB_timer_query"
    };
    static const char* kFakeDisabledExtensions = "GL_ARB_timer_query";

    SetFakeExtensionStrings(kFakeExtensions, arraysize(kFakeExtensions));
    InitializeAPI(nullptr);

    EXPECT_TRUE(g_driver_gl.ext.b_GL_ARB_timer_query);

    base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
    command_line.AppendSwitchASCII(switches::kDisableGLExtensions,
        kFakeDisabledExtensions);
    InitializeAPI(&command_line);

    EXPECT_FALSE(g_driver_gl.ext.b_GL_ARB_timer_query);
}

TEST_F(GLApiTest, DisabledExtensionStringIndexTest)
{
    static const char* kFakeExtensions[] = {
        "GL_EXT_1",
        "GL_EXT_2",
        "GL_EXT_3",
        "GL_EXT_4"
    };
    static const char* kFakeDisabledExtensions = "GL_EXT_1,GL_EXT_2,GL_FAKE";
    static const char* kFilteredExtensions[] = {
        "GL_EXT_3",
        "GL_EXT_4"
    };

    SetFakeExtensionStrings(kFakeExtensions, arraysize(kFakeExtensions));
    InitializeAPI(nullptr);

    EXPECT_EQ(arraysize(kFakeExtensions), GetNumExtensions());
    for (uint32_t i = 0; i < arraysize(kFakeExtensions); ++i) {
        EXPECT_STREQ(kFakeExtensions[i], GetExtensioni(i));
    }

    base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
    command_line.AppendSwitchASCII(switches::kDisableGLExtensions,
        kFakeDisabledExtensions);
    InitializeAPI(&command_line);

    EXPECT_EQ(arraysize(kFilteredExtensions), GetNumExtensions());
    for (uint32_t i = 0; i < arraysize(kFilteredExtensions); ++i) {
        EXPECT_STREQ(kFilteredExtensions[i], GetExtensioni(i));
    }
}

} // namespace gfx
