// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/layer_tree_pixel_resource_test.h"

#include "cc/layers/layer.h"
#include "cc/raster/bitmap_tile_task_worker_pool.h"
#include "cc/raster/gpu_rasterizer.h"
#include "cc/raster/gpu_tile_task_worker_pool.h"
#include "cc/raster/one_copy_tile_task_worker_pool.h"
#include "cc/raster/tile_task_worker_pool.h"
#include "cc/raster/zero_copy_tile_task_worker_pool.h"
#include "cc/resources/resource_pool.h"
#include "cc/test/fake_output_surface.h"
#include "gpu/GLES2/gl2extchromium.h"

namespace cc {

namespace {

    bool IsTestCaseSupported(PixelResourceTestCase test_case)
    {
        switch (test_case) {
        case SOFTWARE:
        case GL_GPU_RASTER_2D_DRAW:
        case GL_ZERO_COPY_2D_DRAW:
        case GL_ZERO_COPY_RECT_DRAW:
        case GL_ONE_COPY_2D_STAGING_2D_DRAW:
        case GL_ONE_COPY_RECT_STAGING_2D_DRAW:
            return true;
        case GL_ZERO_COPY_EXTERNAL_DRAW:
        case GL_ONE_COPY_EXTERNAL_STAGING_2D_DRAW:
            // These should all be enabled in practice.
            // TODO(enne): look into getting texture external oes enabled.
            return false;
        }

        NOTREACHED();
        return false;
    }

} // namespace

LayerTreeHostPixelResourceTest::LayerTreeHostPixelResourceTest(
    PixelResourceTestCase test_case)
    : draw_texture_target_(GL_INVALID_VALUE)
    , resource_pool_option_(BITMAP_TILE_TASK_WORKER_POOL)
    , initialized_(false)
    , test_case_(test_case)
{
    InitializeFromTestCase(test_case);
}

LayerTreeHostPixelResourceTest::LayerTreeHostPixelResourceTest()
    : draw_texture_target_(GL_INVALID_VALUE)
    , resource_pool_option_(BITMAP_TILE_TASK_WORKER_POOL)
    , initialized_(false)
    , test_case_(SOFTWARE)
{
}

void LayerTreeHostPixelResourceTest::InitializeFromTestCase(
    PixelResourceTestCase test_case)
{
    DCHECK(!initialized_);
    initialized_ = true;
    switch (test_case) {
    case SOFTWARE:
        test_type_ = PIXEL_TEST_SOFTWARE;
        draw_texture_target_ = GL_INVALID_VALUE;
        resource_pool_option_ = BITMAP_TILE_TASK_WORKER_POOL;
        return;
    case GL_GPU_RASTER_2D_DRAW:
        test_type_ = PIXEL_TEST_GL;
        draw_texture_target_ = GL_TEXTURE_2D;
        resource_pool_option_ = GPU_TILE_TASK_WORKER_POOL;
        return;
    case GL_ONE_COPY_2D_STAGING_2D_DRAW:
        test_type_ = PIXEL_TEST_GL;
        draw_texture_target_ = GL_TEXTURE_2D;
        resource_pool_option_ = ONE_COPY_TILE_TASK_WORKER_POOL;
        return;
    case GL_ONE_COPY_RECT_STAGING_2D_DRAW:
        test_type_ = PIXEL_TEST_GL;
        draw_texture_target_ = GL_TEXTURE_2D;
        resource_pool_option_ = ONE_COPY_TILE_TASK_WORKER_POOL;
        return;
    case GL_ONE_COPY_EXTERNAL_STAGING_2D_DRAW:
        test_type_ = PIXEL_TEST_GL;
        draw_texture_target_ = GL_TEXTURE_2D;
        resource_pool_option_ = ONE_COPY_TILE_TASK_WORKER_POOL;
        return;
    case GL_ZERO_COPY_2D_DRAW:
        test_type_ = PIXEL_TEST_GL;
        draw_texture_target_ = GL_TEXTURE_2D;
        resource_pool_option_ = ZERO_COPY_TILE_TASK_WORKER_POOL;
        return;
    case GL_ZERO_COPY_RECT_DRAW:
        test_type_ = PIXEL_TEST_GL;
        draw_texture_target_ = GL_TEXTURE_RECTANGLE_ARB;
        resource_pool_option_ = ZERO_COPY_TILE_TASK_WORKER_POOL;
        return;
    case GL_ZERO_COPY_EXTERNAL_DRAW:
        test_type_ = PIXEL_TEST_GL;
        draw_texture_target_ = GL_TEXTURE_EXTERNAL_OES;
        resource_pool_option_ = ZERO_COPY_TILE_TASK_WORKER_POOL;
        return;
    }
    NOTREACHED();
}

void LayerTreeHostPixelResourceTest::CreateResourceAndTileTaskWorkerPool(
    LayerTreeHostImpl* host_impl,
    scoped_ptr<TileTaskWorkerPool>* tile_task_worker_pool,
    scoped_ptr<ResourcePool>* resource_pool)
{
    base::SingleThreadTaskRunner* task_runner = proxy()->HasImplThread() ? proxy()->ImplThreadTaskRunner()
                                                                         : proxy()->MainThreadTaskRunner();
    DCHECK(task_runner);
    DCHECK(initialized_);

    ContextProvider* context_provider = host_impl->output_surface()->context_provider();
    ResourceProvider* resource_provider = host_impl->resource_provider();
    int max_bytes_per_copy_operation = 1024 * 1024;
    int max_staging_buffer_usage_in_bytes = 32 * 1024 * 1024;

    switch (resource_pool_option_) {
    case BITMAP_TILE_TASK_WORKER_POOL:
        EXPECT_FALSE(context_provider);
        EXPECT_EQ(PIXEL_TEST_SOFTWARE, test_type_);
        *resource_pool = ResourcePool::Create(resource_provider, task_runner,
            draw_texture_target_);

        *tile_task_worker_pool = BitmapTileTaskWorkerPool::Create(
            task_runner, task_graph_runner(), resource_provider);
        break;
    case GPU_TILE_TASK_WORKER_POOL:
        EXPECT_TRUE(context_provider);
        EXPECT_EQ(PIXEL_TEST_GL, test_type_);
        *resource_pool = ResourcePool::Create(resource_provider, task_runner,
            draw_texture_target_);

        *tile_task_worker_pool = GpuTileTaskWorkerPool::Create(
            task_runner, task_graph_runner(), context_provider, resource_provider,
            false, 0);
        break;
    case ZERO_COPY_TILE_TASK_WORKER_POOL:
        EXPECT_TRUE(context_provider);
        EXPECT_EQ(PIXEL_TEST_GL, test_type_);
        EXPECT_TRUE(host_impl->GetRendererCapabilities().using_image);
        *resource_pool = ResourcePool::Create(resource_provider, task_runner,
            draw_texture_target_);

        *tile_task_worker_pool = ZeroCopyTileTaskWorkerPool::Create(
            task_runner, task_graph_runner(), resource_provider, false);
        break;
    case ONE_COPY_TILE_TASK_WORKER_POOL:
        EXPECT_TRUE(context_provider);
        EXPECT_EQ(PIXEL_TEST_GL, test_type_);
        EXPECT_TRUE(host_impl->GetRendererCapabilities().using_image);
        *resource_pool = ResourcePool::Create(resource_provider, task_runner,
            draw_texture_target_);

        *tile_task_worker_pool = OneCopyTileTaskWorkerPool::Create(
            task_runner, task_graph_runner(), context_provider, resource_provider,
            max_bytes_per_copy_operation, false,
            max_staging_buffer_usage_in_bytes, false);
        break;
    }
}

void LayerTreeHostPixelResourceTest::RunPixelResourceTest(
    scoped_refptr<Layer> content_root,
    base::FilePath file_name)
{
    if (!IsTestCaseSupported(test_case_))
        return;
    RunPixelTest(test_type_, content_root, file_name);
}

ParameterizedPixelResourceTest::ParameterizedPixelResourceTest()
    : LayerTreeHostPixelResourceTest(GetParam())
{
}

} // namespace cc
