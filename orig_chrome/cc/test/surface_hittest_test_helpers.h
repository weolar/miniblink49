// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_SURFACE_HITTEST_TEST_HELPERS_H_
#define CC_TEST_SURFACE_HITTEST_TEST_HELPERS_H_

#include "base/memory/scoped_ptr.h"
#include "cc/quads/render_pass.h"
#include "cc/surfaces/surface_factory_client.h"

namespace gfx {
class Transform;
}

namespace cc {

class CompositorFrame;
class RenderPassDrawQuad;
class SolidColorDrawQuad;

namespace test {

    class EmptySurfaceFactoryClient : public SurfaceFactoryClient {
    public:
        void ReturnResources(const ReturnedResourceArray& resources) override { }
        void SetBeginFrameSource(SurfaceId surface_id,
            BeginFrameSource* begin_frame_source) override { }
    };

    void CreateSharedQuadState(RenderPass* pass,
        const gfx::Transform& transform,
        const gfx::Rect& root_rect);

    void CreateSolidColorDrawQuad(RenderPass* pass,
        const gfx::Transform& transform,
        const gfx::Rect& root_rect,
        const gfx::Rect& quad_rect);

    void CreateRenderPassDrawQuad(RenderPass* pass,
        const gfx::Transform& transform,
        const gfx::Rect& root_rect,
        const gfx::Rect& quad_rect,
        const RenderPassId& render_pass_id);

    void CreateSurfaceDrawQuad(RenderPass* pass,
        const gfx::Transform& transform,
        const gfx::Rect& root_rect,
        const gfx::Rect& quad_rect,
        SurfaceId surface_id);

    void CreateRenderPass(const RenderPassId& render_pass_id,
        const gfx::Rect& rect,
        const gfx::Transform& transform_to_root_target,
        RenderPassList* render_pass_list);

    scoped_ptr<CompositorFrame> CreateCompositorFrameWithRenderPassList(
        RenderPassList* render_pass_list);

    scoped_ptr<CompositorFrame> CreateCompositorFrame(const gfx::Rect& root_rect,
        RenderPass** render_pass);

} // namespace test
} // namespace cc

#endif // CC_TEST_SURFACE_HITTEST_TEST_HELPERS_H_
