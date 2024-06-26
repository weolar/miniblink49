// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2extchromium.h>
#include <GLES3/gl3.h>

#include "base/command_line.h"
#include "gpu/command_buffer/tests/gl_manager.h"
#include "gpu/command_buffer/tests/gl_test_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_switches.h"

#define SHADER_VERSION_300(Src) "#version 300 es\n" #Src

namespace gpu {

class OpenGLES3FunctionTest : public testing::Test {
protected:
    void SetUp() override
    {
        base::CommandLine command_line(*base::CommandLine::ForCurrentProcess());
        command_line.AppendSwitch(switches::kEnableUnsafeES3APIs);
        GLManager::Options options;
        options.context_type = gles2::CONTEXT_TYPE_OPENGLES3;
        gl_.InitializeWithCommandLine(options, &command_line);
    }
    void TearDown() override { gl_.Destroy(); }
    bool IsApplicable() const { return gl_.IsInitialized(); }
    GLManager gl_;
};

TEST_F(OpenGLES3FunctionTest, GetFragDataLocationInvalid)
{
    if (!IsApplicable()) {
        return;
    }
    // clang-format off
  static const char* kVertexShader =
      SHADER_VERSION_300(
          in vec4 position;
          void main() {
            gl_Position = position;
          });
  static const char* kFragColorShader =
      SHADER_VERSION_300(
          precision mediump float;
          uniform vec4 src;
          out vec4 FragColor;
          void main() {
            FragColor = src;
          });
    // clang-format on

    GLuint vsid = GLTestHelper::LoadShader(GL_VERTEX_SHADER, kVertexShader);
    GLuint fsid = GLTestHelper::LoadShader(GL_FRAGMENT_SHADER, kFragColorShader);
    GLuint program = glCreateProgram();
    glAttachShader(program, vsid);
    glAttachShader(program, fsid);
    glDeleteShader(vsid);
    glDeleteShader(fsid);

    GLint location = glGetFragDataLocation(program, "FragColor");
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_OPERATION), glGetError());
    EXPECT_EQ(-1, location);
    location = glGetFragDataLocation(program, "Unknown");
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_OPERATION), glGetError());
    EXPECT_EQ(-1, location);

    glLinkProgram(program);

    location = glGetFragDataLocation(program, "FragColor");
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
    EXPECT_EQ(0, location);
    location = glGetFragDataLocation(program, "Unknown");
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
    EXPECT_EQ(-1, location);

    glDeleteProgram(program);
}

} // namespace gpu
