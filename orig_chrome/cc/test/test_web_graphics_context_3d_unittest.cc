// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/memory/scoped_ptr.h"
#include "cc/test/test_web_graphics_context_3d.h"
#include "gpu/GLES2/gl2extchromium.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/khronos/GLES2/gl2ext.h"

namespace cc {
namespace {

    static bool check_parameter_value(TestWebGraphicsContext3D* context,
        GLenum pname,
        GLint expected_value)
    {
        GLint actual_value = 0;
        context->getTexParameteriv(GL_TEXTURE_2D, pname, &actual_value);
        return expected_value == actual_value;
    }

    static void expect_default_parameter_values(TestWebGraphicsContext3D* context)
    {
        EXPECT_TRUE(check_parameter_value(context, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        EXPECT_TRUE(check_parameter_value(
            context, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
        EXPECT_TRUE(check_parameter_value(context, GL_TEXTURE_WRAP_S, GL_REPEAT));
        EXPECT_TRUE(check_parameter_value(context, GL_TEXTURE_WRAP_T, GL_REPEAT));
        EXPECT_TRUE(check_parameter_value(
            context, GL_TEXTURE_POOL_CHROMIUM, GL_TEXTURE_POOL_UNMANAGED_CHROMIUM));
        EXPECT_TRUE(check_parameter_value(context, GL_TEXTURE_USAGE_ANGLE, GL_NONE));
    }

    TEST(TestWebGraphicsContext3DTest, GetDefaultTextureParameterValues)
    {
        scoped_ptr<TestWebGraphicsContext3D> context(
            TestWebGraphicsContext3D::Create());

        GLuint texture = context->createTexture();
        context->bindTexture(GL_TEXTURE_2D, texture);

        expect_default_parameter_values(context.get());
    }

    TEST(TestWebGraphicsContext3DTest, SetAndGetTextureParameter)
    {
        scoped_ptr<TestWebGraphicsContext3D> context(
            TestWebGraphicsContext3D::Create());

        GLuint texture = context->createTexture();
        context->bindTexture(GL_TEXTURE_2D, texture);
        context->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        EXPECT_TRUE(
            check_parameter_value(context.get(), GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    }

    TEST(TestWebGraphicsContext3DTest,
        SetAndGetMultipleTextureParametersOnMultipleTextures)
    {
        scoped_ptr<TestWebGraphicsContext3D> context(
            TestWebGraphicsContext3D::Create());

        // Set and get non-default texture parameters on the first texture.
        GLuint first_texture = context->createTexture();
        context->bindTexture(GL_TEXTURE_2D, first_texture);
        context->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        context->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        EXPECT_TRUE(
            check_parameter_value(context.get(), GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        EXPECT_TRUE(
            check_parameter_value(context.get(), GL_TEXTURE_MAG_FILTER, GL_NEAREST));

        // Set and get different, non-default texture parameters on the second
        // texture.
        GLuint second_texture = context->createTexture();
        context->bindTexture(GL_TEXTURE_2D, second_texture);
        context->texParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        context->texParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        EXPECT_TRUE(check_parameter_value(
            context.get(), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST));
        EXPECT_TRUE(check_parameter_value(
            context.get(), GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR));

        // Get texture parameters on the first texture and verify they are still
        // intact.
        context->bindTexture(GL_TEXTURE_2D, first_texture);

        EXPECT_TRUE(
            check_parameter_value(context.get(), GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        EXPECT_TRUE(
            check_parameter_value(context.get(), GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    }

    TEST(TestWebGraphicsContext3DTest, UseMultipleRenderAndFramebuffers)
    {
        scoped_ptr<TestWebGraphicsContext3D> context(
            TestWebGraphicsContext3D::Create());

        GLuint ids[2];
        context->genFramebuffers(2, ids);
        EXPECT_NE(ids[0], ids[1]);
        context->deleteFramebuffers(2, ids);

        context->genRenderbuffers(2, ids);
        EXPECT_NE(ids[0], ids[1]);
        context->deleteRenderbuffers(2, ids);
    }

} // namespace
} // namespace cc
