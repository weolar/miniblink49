// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_LAYER_TREE_PIXEL_RESOURCE_TEST_H_
#define CC_TEST_LAYER_TREE_PIXEL_RESOURCE_TEST_H_

#include "base/memory/ref_counted.h"
#include "cc/test/layer_tree_pixel_test.h"

namespace cc {

class LayerTreeHostImpl;
class TileTaskWorkerPool;
class ResourcePool;

// Enumerate the various combinations of renderer, resource pool, staging
// texture type, and drawing texture types.  Not all of the combinations
// are possible (or worth testing independently), so this is the minimal
// list to hit all codepaths.
enum PixelResourceTestCase {
    SOFTWARE,
    GL_GPU_RASTER_2D_DRAW,
    GL_ONE_COPY_2D_STAGING_2D_DRAW,
    GL_ONE_COPY_RECT_STAGING_2D_DRAW,
    GL_ONE_COPY_EXTERNAL_STAGING_2D_DRAW,
    GL_ZERO_COPY_2D_DRAW,
    GL_ZERO_COPY_RECT_DRAW,
    GL_ZERO_COPY_EXTERNAL_DRAW,
};

class LayerTreeHostPixelResourceTest : public LayerTreePixelTest {
public:
    explicit LayerTreeHostPixelResourceTest(PixelResourceTestCase test_case);
    LayerTreeHostPixelResourceTest();

    void CreateResourceAndTileTaskWorkerPool(
        LayerTreeHostImpl* host_impl,
        scoped_ptr<TileTaskWorkerPool>* tile_task_worker_pool,
        scoped_ptr<ResourcePool>* resource_pool) override;

    void RunPixelResourceTest(scoped_refptr<Layer> content_root,
        base::FilePath file_name);

    enum TileTaskWorkerPoolOption {
        BITMAP_TILE_TASK_WORKER_POOL,
        GPU_TILE_TASK_WORKER_POOL,
        ZERO_COPY_TILE_TASK_WORKER_POOL,
        ONE_COPY_TILE_TASK_WORKER_POOL
    };

protected:
    unsigned draw_texture_target_;
    TileTaskWorkerPoolOption resource_pool_option_;
    bool initialized_;

    void InitializeFromTestCase(PixelResourceTestCase test_case);

private:
    PixelResourceTestCase test_case_;
};

#define INSTANTIATE_PIXEL_RESOURCE_TEST_CASE_P(framework_name)               \
    INSTANTIATE_TEST_CASE_P(                                                 \
        PixelResourceTest, framework_name,                                   \
        ::testing::Values(                                                   \
            SOFTWARE, GL_GPU_RASTER_2D_DRAW, GL_ONE_COPY_2D_STAGING_2D_DRAW, \
            GL_ONE_COPY_RECT_STAGING_2D_DRAW,                                \
            GL_ONE_COPY_EXTERNAL_STAGING_2D_DRAW, GL_ZERO_COPY_2D_DRAW,      \
            GL_ZERO_COPY_RECT_DRAW, GL_ZERO_COPY_EXTERNAL_DRAW))

class ParameterizedPixelResourceTest
    : public LayerTreeHostPixelResourceTest,
      public ::testing::WithParamInterface<PixelResourceTestCase> {
public:
    ParameterizedPixelResourceTest();
};

} // namespace cc

#endif // CC_TEST_LAYER_TREE_PIXEL_RESOURCE_TEST_H_
