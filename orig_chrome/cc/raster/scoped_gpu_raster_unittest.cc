// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/raster/scoped_gpu_raster.h"
#include "cc/test/test_context_provider.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    class ScopedGpuRasterTest : public testing::Test {
    public:
        ScopedGpuRasterTest() { }
    };

    // Releasing ScopedGpuRaster should restore GL_UNPACK_ALIGNMENT == 4.
    TEST(ScopedGpuRasterTest, RestoresUnpackAlignment)
    {
        scoped_refptr<TestContextProvider> provider = TestContextProvider::Create();
        EXPECT_TRUE(provider->BindToCurrentThread());
        gpu::gles2::GLES2Interface* gl = provider->ContextGL();
        GLint unpack_alignment = 0;
        gl->GetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);
        EXPECT_EQ(4, unpack_alignment);

        {
            scoped_ptr<ScopedGpuRaster> scoped_gpu_raster(
                new ScopedGpuRaster(provider.get()));
            gl->PixelStorei(GL_UNPACK_ALIGNMENT, 1);
            gl->GetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);
            EXPECT_EQ(1, unpack_alignment);
        }

        gl->GetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);
        EXPECT_EQ(4, unpack_alignment);
    }

} // namespace
} // namespace cc
