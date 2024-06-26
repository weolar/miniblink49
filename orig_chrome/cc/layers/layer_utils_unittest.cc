// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/layer_utils.h"

#include "cc/animation/transform_operations.h"
#include "cc/layers/layer_impl.h"
#include "cc/test/animation_test_common.h"
#include "cc/test/fake_impl_proxy.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_task_graph_runner.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/box_f.h"
#include "ui/gfx/test/gfx_util.h"

namespace cc {
namespace {

    float diagonal(float width, float height)
    {
        return std::sqrt(width * width + height * height);
    }

    class LayerUtilsGetAnimationBoundsTest : public testing::Test {
    public:
        LayerUtilsGetAnimationBoundsTest()
            : host_impl_(&proxy_, &shared_bitmap_manager_, &task_graph_runner_)
            , root_(CreateThreeNodeTree(&host_impl_))
            , parent_(root_->children()[0])
            , child_(parent_->children()[0])
        {
        }

        LayerImpl* root() { return root_.get(); }
        LayerImpl* parent() { return parent_; }
        LayerImpl* child() { return child_; }

    private:
        static scoped_ptr<LayerImpl> CreateThreeNodeTree(
            LayerTreeHostImpl* host_impl)
        {
            scoped_ptr<LayerImpl> root = LayerImpl::Create(host_impl->active_tree(), 1);
            root->AddChild(LayerImpl::Create(host_impl->active_tree(), 2));
            root->children()[0]->AddChild(
                LayerImpl::Create(host_impl->active_tree(), 3));
            return root.Pass();
        }

        FakeImplProxy proxy_;
        TestSharedBitmapManager shared_bitmap_manager_;
        TestTaskGraphRunner task_graph_runner_;
        FakeLayerTreeHostImpl host_impl_;
        scoped_ptr<LayerImpl> root_;
        LayerImpl* parent_;
        LayerImpl* child_;
    };

    TEST_F(LayerUtilsGetAnimationBoundsTest, ScaleRoot)
    {
        double duration = 1.0;

        TransformOperations start;
        start.AppendScale(1.f, 1.f, 1.f);
        TransformOperations end;
        end.AppendScale(2.f, 2.f, 1.f);
        AddAnimatedTransformToLayer(root(), duration, start, end);

        root()->SetPosition(gfx::PointF());
        parent()->SetPosition(gfx::PointF());
        parent()->SetBounds(gfx::Size(350, 200));

        child()->SetDrawsContent(true);
        child()->draw_properties().screen_space_transform_is_animating = true;
        child()->SetPosition(gfx::PointF(150.f, 50.f));
        child()->SetBounds(gfx::Size(100, 200));

        gfx::BoxF box;
        bool success = LayerUtils::GetAnimationBounds(*child(), &box);
        EXPECT_TRUE(success);
        gfx::BoxF expected(150.f, 50.f, 0.f, 350.f, 450.f, 0.f);
        EXPECT_BOXF_EQ(expected, box);
    }

    TEST_F(LayerUtilsGetAnimationBoundsTest, TranslateParentLayer)
    {
        double duration = 1.0;

        TransformOperations start;
        start.AppendTranslate(0.f, 0.f, 0.f);
        TransformOperations end;
        end.AppendTranslate(50.f, 50.f, 0.f);
        AddAnimatedTransformToLayer(parent(), duration, start, end);

        parent()->SetBounds(gfx::Size(350, 200));

        child()->SetDrawsContent(true);
        child()->draw_properties().screen_space_transform_is_animating = true;
        child()->SetPosition(gfx::PointF(150.f, 50.f));
        child()->SetBounds(gfx::Size(100, 200));

        gfx::BoxF box;
        bool success = LayerUtils::GetAnimationBounds(*child(), &box);
        EXPECT_TRUE(success);
        gfx::BoxF expected(150.f, 50.f, 0.f, 150.f, 250.f, 0.f);
        EXPECT_BOXF_EQ(expected, box);
    }

    TEST_F(LayerUtilsGetAnimationBoundsTest, TranslateChildLayer)
    {
        double duration = 1.0;

        TransformOperations start;
        start.AppendTranslate(0.f, 0.f, 0.f);
        TransformOperations end;
        end.AppendTranslate(50.f, 50.f, 0.f);
        AddAnimatedTransformToLayer(child(), duration, start, end);

        parent()->SetBounds(gfx::Size(350, 200));

        child()->SetDrawsContent(true);
        child()->draw_properties().screen_space_transform_is_animating = true;
        child()->SetPosition(gfx::PointF(150.f, 50.f));
        child()->SetBounds(gfx::Size(100, 200));

        gfx::BoxF box;
        bool success = LayerUtils::GetAnimationBounds(*child(), &box);
        EXPECT_TRUE(success);
        gfx::BoxF expected(150.f, 50.f, 0.f, 150.f, 250.f, 0.f);
        EXPECT_BOXF_EQ(expected, box);
    }

    TEST_F(LayerUtilsGetAnimationBoundsTest, TranslateBothLayers)
    {
        double duration = 1.0;

        TransformOperations start;
        start.AppendTranslate(0.f, 0.f, 0.f);
        TransformOperations child_end;
        child_end.AppendTranslate(50.f, 0.f, 0.f);
        AddAnimatedTransformToLayer(parent(), duration, start, child_end);

        TransformOperations grand_child_end;
        grand_child_end.AppendTranslate(0.f, 50.f, 0.f);
        AddAnimatedTransformToLayer(child(), duration, start, grand_child_end);

        parent()->SetBounds(gfx::Size(350, 200));

        child()->SetDrawsContent(true);
        child()->draw_properties().screen_space_transform_is_animating = true;
        child()->SetPosition(gfx::PointF(150.f, 50.f));
        child()->SetBounds(gfx::Size(100, 200));

        gfx::BoxF box;
        bool success = LayerUtils::GetAnimationBounds(*child(), &box);
        EXPECT_TRUE(success);
        gfx::BoxF expected(150.f, 50.f, 0.f, 150.f, 250.f, 0.f);
        EXPECT_BOXF_EQ(expected, box);
    }

    TEST_F(LayerUtilsGetAnimationBoundsTest, RotateXNoPerspective)
    {
        double duration = 1.0;

        TransformOperations start;
        start.AppendRotate(1.f, 0.f, 0.f, 0.f);
        TransformOperations end;
        end.AppendRotate(1.f, 0.f, 0.f, 90.f);
        AddAnimatedTransformToLayer(child(), duration, start, end);

        parent()->SetBounds(gfx::Size(350, 200));

        gfx::Size bounds(100, 100);
        child()->SetDrawsContent(true);
        child()->draw_properties().screen_space_transform_is_animating = true;
        child()->SetPosition(gfx::PointF(150.f, 50.f));
        child()->SetBounds(bounds);
        child()->SetTransformOrigin(
            gfx::Point3F(bounds.width() * 0.5f, bounds.height() * 0.5f, 0));

        gfx::BoxF box;
        bool success = LayerUtils::GetAnimationBounds(*child(), &box);
        EXPECT_TRUE(success);
        gfx::BoxF expected(150.f, 50.f, -50.f, 100.f, 100.f, 100.f);
        EXPECT_BOXF_EQ(expected, box);
    }

    TEST_F(LayerUtilsGetAnimationBoundsTest, RotateXWithPerspective)
    {
        double duration = 1.0;

        TransformOperations start;
        start.AppendRotate(1.f, 0.f, 0.f, 0.f);
        TransformOperations end;
        end.AppendRotate(1.f, 0.f, 0.f, 90.f);
        AddAnimatedTransformToLayer(child(), duration, start, end);

        // Make the anchor point not the default 0.5 value and line up with the
        // child center to make the math easier.
        parent()->SetTransformOrigin(
            gfx::Point3F(0.375f * 400.f, 0.375f * 400.f, 0.f));
        parent()->SetBounds(gfx::Size(400, 400));

        gfx::Transform perspective;
        perspective.ApplyPerspectiveDepth(100.f);
        parent()->SetTransform(perspective);

        gfx::Size bounds(100, 100);
        child()->SetDrawsContent(true);
        child()->draw_properties().screen_space_transform_is_animating = true;
        child()->SetPosition(gfx::PointF(100.f, 100.f));
        child()->SetBounds(bounds);
        child()->SetTransformOrigin(
            gfx::Point3F(bounds.width() * 0.5f, bounds.height() * 0.5f, 0));

        gfx::BoxF box;
        bool success = LayerUtils::GetAnimationBounds(*child(), &box);
        EXPECT_TRUE(success);
        gfx::BoxF expected(50.f, 50.f, -33.333336f, 200.f, 200.f, 133.333344f);
        EXPECT_BOXF_EQ(expected, box);
    }

    TEST_F(LayerUtilsGetAnimationBoundsTest, RotateZ)
    {
        double duration = 1.0;

        TransformOperations start;
        start.AppendRotate(0.f, 0.f, 1.f, 0.f);
        TransformOperations end;
        end.AppendRotate(0.f, 0.f, 1.f, 90.f);
        AddAnimatedTransformToLayer(child(), duration, start, end);

        parent()->SetBounds(gfx::Size(350, 200));

        gfx::Size bounds(100, 100);
        child()->SetDrawsContent(true);
        child()->draw_properties().screen_space_transform_is_animating = true;
        child()->SetPosition(gfx::PointF(150.f, 50.f));
        child()->SetBounds(bounds);
        child()->SetTransformOrigin(
            gfx::Point3F(bounds.width() * 0.5f, bounds.height() * 0.5f, 0));

        gfx::BoxF box;
        bool success = LayerUtils::GetAnimationBounds(*child(), &box);
        EXPECT_TRUE(success);
        float diag = diagonal(bounds.width(), bounds.height());
        gfx::BoxF expected(150.f + 0.5f * (bounds.width() - diag),
            50.f + 0.5f * (bounds.height() - diag),
            0.f,
            diag,
            diag,
            0.f);
        EXPECT_BOXF_EQ(expected, box);
    }

    TEST_F(LayerUtilsGetAnimationBoundsTest, MismatchedTransforms)
    {
        double duration = 1.0;

        TransformOperations start;
        start.AppendTranslate(5, 6, 7);
        TransformOperations end;
        end.AppendRotate(0.f, 0.f, 1.f, 90.f);
        AddAnimatedTransformToLayer(child(), duration, start, end);

        parent()->SetBounds(gfx::Size(350, 200));

        gfx::Size bounds(100, 100);
        child()->SetDrawsContent(true);
        child()->draw_properties().screen_space_transform_is_animating = true;
        child()->SetPosition(gfx::PointF(150.f, 50.f));
        child()->SetBounds(bounds);

        gfx::BoxF box;
        bool success = LayerUtils::GetAnimationBounds(*child(), &box);
        EXPECT_FALSE(success);
    }

} // namespace
} // namespace cc
