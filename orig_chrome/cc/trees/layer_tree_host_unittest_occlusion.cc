// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/layer_tree_host.h"

#include "cc/layers/layer.h"
#include "cc/layers/picture_layer.h"
#include "cc/test/fake_content_layer_client.h"
#include "cc/test/layer_tree_test.h"
#include "cc/trees/layer_tree_impl.h"

namespace cc {
namespace {

#define EXPECT_OCCLUSION_EQ(expected, actual)                \
    EXPECT_TRUE(expected.IsEqual(actual))                    \
        << " Expected: " << expected.ToString() << std::endl \
        << " Actual: " << actual.ToString();

    class LayerTreeHostOcclusionTest : public LayerTreeTest {
    protected:
        void InitializeSettings(LayerTreeSettings* settings) override
        {
            settings->minimum_occlusion_tracking_size = gfx::Size();
        }
    };

    // Verify occlusion is set on the layer draw properties.
    class LayerTreeHostOcclusionTestDrawPropertiesOnLayer
        : public LayerTreeHostOcclusionTest {
    public:
        void SetupTree() override
        {
            scoped_refptr<Layer> root = Layer::Create(layer_settings());
            root->SetBounds(gfx::Size(100, 100));
            root->SetIsDrawable(true);

            scoped_refptr<Layer> child = Layer::Create(layer_settings());
            child->SetBounds(gfx::Size(50, 60));
            child->SetPosition(gfx::PointF(10.f, 5.5f));
            child->SetContentsOpaque(true);
            child->SetIsDrawable(true);
            root->AddChild(child);

            layer_tree_host()->SetRootLayer(root);
            LayerTreeTest::SetupTree();
        }

        void BeginTest() override { PostSetNeedsCommitToMainThread(); }

        void DrawLayersOnThread(LayerTreeHostImpl* impl) override
        {
            LayerImpl* root = impl->active_tree()->root_layer();
            LayerImpl* child = root->children()[0];

            // Verify the draw properties are valid.
            EXPECT_TRUE(root->IsDrawnRenderSurfaceLayerListMember());
            EXPECT_TRUE(child->IsDrawnRenderSurfaceLayerListMember());

            EXPECT_OCCLUSION_EQ(
                Occlusion(child->draw_transform(), SimpleEnclosedRegion(),
                    SimpleEnclosedRegion()),
                child->draw_properties().occlusion_in_content_space);
            EXPECT_OCCLUSION_EQ(
                Occlusion(root->draw_transform(), SimpleEnclosedRegion(),
                    SimpleEnclosedRegion(gfx::Rect(10, 6, 50, 59))),
                root->draw_properties().occlusion_in_content_space);
            EndTest();
        }

        void AfterTest() override { }
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(LayerTreeHostOcclusionTestDrawPropertiesOnLayer);

    // Verify occlusion is set on the render surfaces.
    class LayerTreeHostOcclusionTestDrawPropertiesOnSurface
        : public LayerTreeHostOcclusionTest {
    public:
        void SetupTree() override
        {
            scoped_refptr<Layer> root = Layer::Create(layer_settings());
            root->SetBounds(gfx::Size(100, 100));
            root->SetIsDrawable(true);

            scoped_refptr<Layer> child = Layer::Create(layer_settings());
            child->SetBounds(gfx::Size(1, 1));
            child->SetPosition(gfx::PointF(10.f, 5.5f));
            child->SetIsDrawable(true);
            child->SetForceRenderSurface(true);
            root->AddChild(child);

            scoped_refptr<Layer> child2 = Layer::Create(layer_settings());
            child2->SetBounds(gfx::Size(10, 12));
            child2->SetPosition(gfx::PointF(13.f, 8.5f));
            child2->SetContentsOpaque(true);
            child2->SetIsDrawable(true);
            root->AddChild(child2);

            layer_tree_host()->SetRootLayer(root);
            LayerTreeTest::SetupTree();
        }

        void BeginTest() override { PostSetNeedsCommitToMainThread(); }

        void DrawLayersOnThread(LayerTreeHostImpl* impl) override
        {
            LayerImpl* root = impl->active_tree()->root_layer();
            LayerImpl* child = root->children()[0];
            RenderSurfaceImpl* surface = child->render_surface();

            // Verify the draw properties are valid.
            EXPECT_TRUE(root->IsDrawnRenderSurfaceLayerListMember());
            EXPECT_TRUE(child->IsDrawnRenderSurfaceLayerListMember());
            EXPECT_EQ(child, child->render_target());

            EXPECT_OCCLUSION_EQ(
                Occlusion(surface->draw_transform(), SimpleEnclosedRegion(),
                    SimpleEnclosedRegion(gfx::Rect(13, 9, 10, 11))),
                surface->occlusion_in_content_space());
            EndTest();
        }

        void AfterTest() override { }
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(
        LayerTreeHostOcclusionTestDrawPropertiesOnSurface);

    // Verify occlusion is set on mask layers.
    class LayerTreeHostOcclusionTestDrawPropertiesOnMask
        : public LayerTreeHostOcclusionTest {
    public:
        void SetupTree() override
        {
            scoped_refptr<Layer> root = Layer::Create(layer_settings());
            root->SetBounds(gfx::Size(100, 100));
            root->SetIsDrawable(true);

            scoped_refptr<Layer> child = Layer::Create(layer_settings());
            child->SetBounds(gfx::Size(30, 40));
            child->SetPosition(gfx::PointF(10.f, 5.5f));
            child->SetIsDrawable(true);
            root->AddChild(child);

            scoped_refptr<Layer> make_surface_bigger = Layer::Create(layer_settings());
            make_surface_bigger->SetBounds(gfx::Size(100, 100));
            make_surface_bigger->SetPosition(gfx::PointF(-10.f, -15.f));
            make_surface_bigger->SetIsDrawable(true);
            child->AddChild(make_surface_bigger);

            scoped_refptr<Layer> mask = PictureLayer::Create(layer_settings(), &client_);
            mask->SetBounds(gfx::Size(30, 40));
            mask->SetIsDrawable(true);
            child->SetMaskLayer(mask.get());

            scoped_refptr<Layer> child2 = Layer::Create(layer_settings());
            child2->SetBounds(gfx::Size(10, 12));
            child2->SetPosition(gfx::PointF(13.f, 8.5f));
            child2->SetContentsOpaque(true);
            child2->SetIsDrawable(true);
            root->AddChild(child2);

            layer_tree_host()->SetRootLayer(root);
            LayerTreeTest::SetupTree();
        }

        void BeginTest() override { PostSetNeedsCommitToMainThread(); }

        void DrawLayersOnThread(LayerTreeHostImpl* impl) override
        {
            LayerImpl* root = impl->active_tree()->root_layer();
            LayerImpl* child = root->children()[0];
            RenderSurfaceImpl* surface = child->render_surface();
            LayerImpl* mask = child->mask_layer();

            // Verify the draw properties are valid.
            EXPECT_TRUE(root->IsDrawnRenderSurfaceLayerListMember());
            EXPECT_TRUE(child->IsDrawnRenderSurfaceLayerListMember());
            EXPECT_EQ(child, child->render_target());

            gfx::Transform transform = surface->draw_transform();
            transform.PreconcatTransform(child->draw_transform());

            EXPECT_OCCLUSION_EQ(
                Occlusion(transform, SimpleEnclosedRegion(),
                    SimpleEnclosedRegion(gfx::Rect(13, 9, 10, 11))),
                mask->draw_properties().occlusion_in_content_space);
            EndTest();
        }

        void AfterTest() override { }

        FakeContentLayerClient client_;
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(LayerTreeHostOcclusionTestDrawPropertiesOnMask);

    // Verify occlusion is set to empty inside the subtree of a replica. This is
    // done because the tile system does not know about replicas, and so would not
    // know that something is unoccluded on the replica even though it's occluded on
    // the original.
    class LayerTreeHostOcclusionTestDrawPropertiesInsideReplica
        : public LayerTreeHostOcclusionTest {
    public:
        void SetupTree() override
        {
            scoped_refptr<Layer> root = Layer::Create(layer_settings());
            root->SetBounds(gfx::Size(100, 100));
            root->SetIsDrawable(true);

            scoped_refptr<Layer> child = Layer::Create(layer_settings());
            child->SetBounds(gfx::Size(1, 1));
            child->SetPosition(gfx::PointF(10.f, 5.5f));
            child->SetIsDrawable(true);
            child->SetForceRenderSurface(true);
            root->AddChild(child);

            scoped_refptr<Layer> replica = Layer::Create(layer_settings());
            gfx::Transform translate;
            translate.Translate(20.f, 4.f);
            replica->SetTransform(translate);
            child->SetReplicaLayer(replica.get());

            scoped_refptr<Layer> mask = PictureLayer::Create(layer_settings(), &client_);
            mask->SetBounds(gfx::Size(30, 40));
            mask->SetIsDrawable(true);
            child->SetMaskLayer(mask.get());

            scoped_refptr<Layer> child2 = Layer::Create(layer_settings());
            child2->SetBounds(gfx::Size(10, 12));
            child2->SetPosition(gfx::PointF(13.f, 8.5f));
            child2->SetContentsOpaque(true);
            child2->SetIsDrawable(true);
            root->AddChild(child2);

            layer_tree_host()->SetRootLayer(root);
            LayerTreeTest::SetupTree();
        }

        void BeginTest() override { PostSetNeedsCommitToMainThread(); }

        void DrawLayersOnThread(LayerTreeHostImpl* impl) override
        {
            LayerImpl* root = impl->active_tree()->root_layer();
            LayerImpl* child = root->children()[0];
            RenderSurfaceImpl* surface = child->render_surface();
            LayerImpl* mask = child->mask_layer();

            // Verify the draw properties are valid.
            EXPECT_TRUE(root->IsDrawnRenderSurfaceLayerListMember());
            EXPECT_TRUE(child->IsDrawnRenderSurfaceLayerListMember());
            EXPECT_EQ(child, child->render_target());

            // No occlusion from on child, which is part of the replica.
            EXPECT_OCCLUSION_EQ(Occlusion(),
                child->draw_properties().occlusion_in_content_space);
            // Occlusion on the surface is okay.
            EXPECT_OCCLUSION_EQ(
                Occlusion(surface->draw_transform(), SimpleEnclosedRegion(),
                    SimpleEnclosedRegion(gfx::Rect(13, 9, 10, 11))),
                surface->occlusion_in_content_space());
            // No occlusion on the replica'd mask.
            EXPECT_OCCLUSION_EQ(Occlusion(),
                mask->draw_properties().occlusion_in_content_space);
            EndTest();
        }

        void AfterTest() override { }

        FakeContentLayerClient client_;
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(
        LayerTreeHostOcclusionTestDrawPropertiesInsideReplica);

} // namespace
} // namespace cc
