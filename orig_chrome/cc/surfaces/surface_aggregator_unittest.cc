// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/compositor_frame.h"
#include "cc/output/delegated_frame_data.h"
#include "cc/quads/render_pass.h"
#include "cc/quads/render_pass_draw_quad.h"
#include "cc/quads/solid_color_draw_quad.h"
#include "cc/quads/surface_draw_quad.h"
#include "cc/quads/texture_draw_quad.h"
#include "cc/resources/shared_bitmap_manager.h"
#include "cc/surfaces/surface.h"
#include "cc/surfaces/surface_aggregator.h"
#include "cc/surfaces/surface_aggregator_test_helpers.h"
#include "cc/surfaces/surface_factory.h"
#include "cc/surfaces/surface_factory_client.h"
#include "cc/surfaces/surface_id_allocator.h"
#include "cc/surfaces/surface_manager.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/fake_output_surface_client.h"
#include "cc/test/fake_resource_provider.h"
#include "cc/test/render_pass_test_utils.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkColor.h"

namespace cc {
namespace {

    SurfaceId InvalidSurfaceId()
    {
        static SurfaceId invalid;
        invalid.id = static_cast<uint64_t>(-1);
        return invalid;
    }

    gfx::Size SurfaceSize()
    {
        static gfx::Size size(5, 5);
        return size;
    }

    class EmptySurfaceFactoryClient : public SurfaceFactoryClient {
    public:
        void ReturnResources(const ReturnedResourceArray& resources) override { }
    };

    class SurfaceAggregatorTest : public testing::Test {
    public:
        explicit SurfaceAggregatorTest(bool use_damage_rect)
            : factory_(&manager_, &empty_client_)
            , aggregator_(&manager_, NULL, use_damage_rect)
        {
        }

        SurfaceAggregatorTest()
            : SurfaceAggregatorTest(false)
        {
        }

    protected:
        SurfaceManager manager_;
        EmptySurfaceFactoryClient empty_client_;
        SurfaceFactory factory_;
        SurfaceAggregator aggregator_;
    };

    TEST_F(SurfaceAggregatorTest, ValidSurfaceNoFrame)
    {
        SurfaceId one_id(7);
        factory_.Create(one_id);
        scoped_ptr<CompositorFrame> frame = aggregator_.Aggregate(one_id);
        EXPECT_FALSE(frame);
        factory_.Destroy(one_id);
    }

    class SurfaceAggregatorValidSurfaceTest : public SurfaceAggregatorTest {
    public:
        explicit SurfaceAggregatorValidSurfaceTest(bool use_damage_rect)
            : SurfaceAggregatorTest(use_damage_rect)
            , allocator_(1u)
            , child_allocator_(2u)
        {
        }
        SurfaceAggregatorValidSurfaceTest()
            : SurfaceAggregatorValidSurfaceTest(false)
        {
        }

        void SetUp() override
        {
            SurfaceAggregatorTest::SetUp();
            root_surface_id_ = allocator_.GenerateId();
            factory_.Create(root_surface_id_);
        }

        void TearDown() override
        {
            factory_.Destroy(root_surface_id_);
            SurfaceAggregatorTest::TearDown();
        }

        void AggregateAndVerify(test::Pass* expected_passes,
            size_t expected_pass_count,
            SurfaceId* surface_ids,
            size_t expected_surface_count)
        {
            scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

            ASSERT_TRUE(aggregated_frame);
            ASSERT_TRUE(aggregated_frame->delegated_frame_data);

            DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

            TestPassesMatchExpectations(
                expected_passes, expected_pass_count, &frame_data->render_pass_list);

            // Ensure no duplicate pass ids output.
            std::set<RenderPassId> used_passes;
            for (auto* pass : frame_data->render_pass_list) {
                EXPECT_TRUE(used_passes.insert(pass->id).second);
            }

            EXPECT_EQ(expected_surface_count,
                aggregator_.previous_contained_surfaces().size());
            for (size_t i = 0; i < expected_surface_count; i++) {
                EXPECT_TRUE(
                    aggregator_.previous_contained_surfaces().find(surface_ids[i]) != aggregator_.previous_contained_surfaces().end());
            }
        }

        void SubmitPassListAsFrame(SurfaceId surface_id, RenderPassList* pass_list)
        {
            scoped_ptr<DelegatedFrameData> frame_data(new DelegatedFrameData);
            pass_list->swap(frame_data->render_pass_list);

            scoped_ptr<CompositorFrame> frame(new CompositorFrame);
            frame->delegated_frame_data = frame_data.Pass();

            factory_.SubmitCompositorFrame(surface_id, frame.Pass(),
                SurfaceFactory::DrawCallback());
        }

        void SubmitCompositorFrame(test::Pass* passes,
            size_t pass_count,
            SurfaceId surface_id)
        {
            RenderPassList pass_list;
            AddPasses(&pass_list, gfx::Rect(SurfaceSize()), passes, pass_count);
            SubmitPassListAsFrame(surface_id, &pass_list);
        }

        void QueuePassAsFrame(scoped_ptr<RenderPass> pass, SurfaceId surface_id)
        {
            scoped_ptr<DelegatedFrameData> delegated_frame_data(new DelegatedFrameData);
            delegated_frame_data->render_pass_list.push_back(pass.Pass());

            scoped_ptr<CompositorFrame> child_frame(new CompositorFrame);
            child_frame->delegated_frame_data = delegated_frame_data.Pass();

            factory_.SubmitCompositorFrame(surface_id, child_frame.Pass(),
                SurfaceFactory::DrawCallback());
        }

    protected:
        SurfaceId root_surface_id_;
        SurfaceIdAllocator allocator_;
        SurfaceIdAllocator child_allocator_;
    };

    // Tests that a very simple frame containing only two solid color quads makes it
    // through the aggregator correctly.
    TEST_F(SurfaceAggregatorValidSurfaceTest, SimpleFrame)
    {
        test::Quad quads[] = { test::Quad::SolidColorQuad(SK_ColorRED),
            test::Quad::SolidColorQuad(SK_ColorBLUE) };
        test::Pass passes[] = { test::Pass(quads, arraysize(quads)) };

        SubmitCompositorFrame(passes, arraysize(passes), root_surface_id_);

        SurfaceId ids[] = { root_surface_id_ };
        AggregateAndVerify(passes, arraysize(passes), ids, arraysize(ids));
    }

    TEST_F(SurfaceAggregatorValidSurfaceTest, OpacityCopied)
    {
        SurfaceId embedded_surface_id = allocator_.GenerateId();
        factory_.Create(embedded_surface_id);

        test::Quad embedded_quads[] = { test::Quad::SolidColorQuad(SK_ColorGREEN),
            test::Quad::SolidColorQuad(SK_ColorBLUE) };
        test::Pass embedded_passes[] = {
            test::Pass(embedded_quads, arraysize(embedded_quads))
        };

        SubmitCompositorFrame(embedded_passes, arraysize(embedded_passes),
            embedded_surface_id);

        test::Quad quads[] = { test::Quad::SurfaceQuad(embedded_surface_id, .5f) };
        test::Pass passes[] = { test::Pass(quads, arraysize(quads)) };

        SubmitCompositorFrame(passes, arraysize(passes), root_surface_id_);

        scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

        ASSERT_TRUE(aggregated_frame);
        ASSERT_TRUE(aggregated_frame->delegated_frame_data);

        DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

        RenderPassList& render_pass_list(frame_data->render_pass_list);
        ASSERT_EQ(2u, render_pass_list.size());
        SharedQuadStateList& shared_quad_state_list(
            render_pass_list[0]->shared_quad_state_list);
        ASSERT_EQ(2u, shared_quad_state_list.size());
        EXPECT_EQ(1.f, shared_quad_state_list.ElementAt(0)->opacity);
        EXPECT_EQ(1.f, shared_quad_state_list.ElementAt(1)->opacity);

        SharedQuadStateList& shared_quad_state_list2(
            render_pass_list[1]->shared_quad_state_list);
        ASSERT_EQ(1u, shared_quad_state_list2.size());
        EXPECT_EQ(.5f, shared_quad_state_list2.ElementAt(0)->opacity);

        factory_.Destroy(embedded_surface_id);
    }

    TEST_F(SurfaceAggregatorValidSurfaceTest, MultiPassSimpleFrame)
    {
        test::Quad quads[][2] = { { test::Quad::SolidColorQuad(SK_ColorWHITE),
                                      test::Quad::SolidColorQuad(SK_ColorLTGRAY) },
            { test::Quad::SolidColorQuad(SK_ColorGRAY),
                test::Quad::SolidColorQuad(SK_ColorDKGRAY) } };
        test::Pass passes[] = {
            test::Pass(quads[0], arraysize(quads[0]), RenderPassId(1, 1)),
            test::Pass(quads[1], arraysize(quads[1]), RenderPassId(1, 2))
        };

        SubmitCompositorFrame(passes, arraysize(passes), root_surface_id_);

        SurfaceId ids[] = { root_surface_id_ };
        AggregateAndVerify(passes, arraysize(passes), ids, arraysize(ids));
    }

    // This tests very simple embedding. root_surface has a frame containing a few
    // solid color quads and a surface quad referencing embedded_surface.
    // embedded_surface has a frame containing only a solid color quad. The solid
    // color quad should be aggregated into the final frame.
    TEST_F(SurfaceAggregatorValidSurfaceTest, SimpleSurfaceReference)
    {
        SurfaceId embedded_surface_id = allocator_.GenerateId();
        factory_.Create(embedded_surface_id);

        test::Quad embedded_quads[] = { test::Quad::SolidColorQuad(SK_ColorGREEN) };
        test::Pass embedded_passes[] = {
            test::Pass(embedded_quads, arraysize(embedded_quads))
        };

        SubmitCompositorFrame(embedded_passes, arraysize(embedded_passes),
            embedded_surface_id);

        test::Quad root_quads[] = { test::Quad::SolidColorQuad(SK_ColorWHITE),
            test::Quad::SurfaceQuad(embedded_surface_id, 1.f),
            test::Quad::SolidColorQuad(SK_ColorBLACK) };
        test::Pass root_passes[] = { test::Pass(root_quads, arraysize(root_quads)) };

        SubmitCompositorFrame(root_passes, arraysize(root_passes), root_surface_id_);

        test::Quad expected_quads[] = { test::Quad::SolidColorQuad(SK_ColorWHITE),
            test::Quad::SolidColorQuad(SK_ColorGREEN),
            test::Quad::SolidColorQuad(SK_ColorBLACK) };
        test::Pass expected_passes[] = {
            test::Pass(expected_quads, arraysize(expected_quads))
        };
        SurfaceId ids[] = { root_surface_id_, embedded_surface_id };
        AggregateAndVerify(
            expected_passes, arraysize(expected_passes), ids, arraysize(ids));

        factory_.Destroy(embedded_surface_id);
    }

    TEST_F(SurfaceAggregatorValidSurfaceTest, CopyRequest)
    {
        SurfaceId embedded_surface_id = allocator_.GenerateId();
        factory_.Create(embedded_surface_id);

        test::Quad embedded_quads[] = { test::Quad::SolidColorQuad(SK_ColorGREEN) };
        test::Pass embedded_passes[] = {
            test::Pass(embedded_quads, arraysize(embedded_quads))
        };

        SubmitCompositorFrame(embedded_passes, arraysize(embedded_passes),
            embedded_surface_id);
        scoped_ptr<CopyOutputRequest> copy_request(
            CopyOutputRequest::CreateEmptyRequest());
        CopyOutputRequest* copy_request_ptr = copy_request.get();
        factory_.RequestCopyOfSurface(embedded_surface_id, copy_request.Pass());

        test::Quad root_quads[] = { test::Quad::SolidColorQuad(SK_ColorWHITE),
            test::Quad::SurfaceQuad(embedded_surface_id, 1.f),
            test::Quad::SolidColorQuad(SK_ColorBLACK) };
        test::Pass root_passes[] = { test::Pass(root_quads, arraysize(root_quads)) };

        SubmitCompositorFrame(root_passes, arraysize(root_passes), root_surface_id_);

        scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

        ASSERT_TRUE(aggregated_frame);
        ASSERT_TRUE(aggregated_frame->delegated_frame_data);

        DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

        test::Quad expected_quads[] = {
            test::Quad::SolidColorQuad(SK_ColorWHITE),
            test::Quad::RenderPassQuad(frame_data->render_pass_list[0]->id),
            test::Quad::SolidColorQuad(SK_ColorBLACK)
        };
        test::Pass expected_passes[] = {
            test::Pass(embedded_quads, arraysize(embedded_quads)),
            test::Pass(expected_quads, arraysize(expected_quads))
        };
        TestPassesMatchExpectations(expected_passes,
            arraysize(expected_passes),
            &frame_data->render_pass_list);
        ASSERT_EQ(2u, frame_data->render_pass_list.size());
        ASSERT_EQ(1u, frame_data->render_pass_list[0]->copy_requests.size());
        DCHECK_EQ(copy_request_ptr,
            frame_data->render_pass_list[0]->copy_requests[0]);

        SurfaceId surface_ids[] = { root_surface_id_, embedded_surface_id };
        EXPECT_EQ(arraysize(surface_ids),
            aggregator_.previous_contained_surfaces().size());
        for (size_t i = 0; i < arraysize(surface_ids); i++) {
            EXPECT_TRUE(
                aggregator_.previous_contained_surfaces().find(surface_ids[i]) != aggregator_.previous_contained_surfaces().end());
        }

        factory_.Destroy(embedded_surface_id);
    }

    // Root surface may contain copy requests.
    TEST_F(SurfaceAggregatorValidSurfaceTest, RootCopyRequest)
    {
        SurfaceId embedded_surface_id = allocator_.GenerateId();
        factory_.Create(embedded_surface_id);

        test::Quad embedded_quads[] = { test::Quad::SolidColorQuad(SK_ColorGREEN) };
        test::Pass embedded_passes[] = {
            test::Pass(embedded_quads, arraysize(embedded_quads))
        };

        SubmitCompositorFrame(embedded_passes, arraysize(embedded_passes),
            embedded_surface_id);
        scoped_ptr<CopyOutputRequest> copy_request(
            CopyOutputRequest::CreateEmptyRequest());
        CopyOutputRequest* copy_request_ptr = copy_request.get();
        scoped_ptr<CopyOutputRequest> copy_request2(
            CopyOutputRequest::CreateEmptyRequest());
        CopyOutputRequest* copy_request2_ptr = copy_request2.get();

        test::Quad root_quads[] = { test::Quad::SolidColorQuad(SK_ColorWHITE),
            test::Quad::SurfaceQuad(embedded_surface_id, 1.f),
            test::Quad::SolidColorQuad(SK_ColorBLACK) };
        test::Quad root_quads2[] = { test::Quad::SolidColorQuad(SK_ColorRED) };
        test::Pass root_passes[] = {
            test::Pass(root_quads, arraysize(root_quads), RenderPassId(1, 1)),
            test::Pass(root_quads2, arraysize(root_quads2), RenderPassId(1, 2))
        };
        {
            RenderPassList pass_list;
            AddPasses(&pass_list,
                gfx::Rect(SurfaceSize()),
                root_passes,
                arraysize(root_passes));
            pass_list[0]->copy_requests.push_back(copy_request.Pass());
            pass_list[1]->copy_requests.push_back(copy_request2.Pass());

            scoped_ptr<DelegatedFrameData> frame_data(new DelegatedFrameData);
            pass_list.swap(frame_data->render_pass_list);

            scoped_ptr<CompositorFrame> frame(new CompositorFrame);
            frame->delegated_frame_data = frame_data.Pass();

            factory_.SubmitCompositorFrame(root_surface_id_, frame.Pass(),
                SurfaceFactory::DrawCallback());
        }

        scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

        ASSERT_TRUE(aggregated_frame);
        ASSERT_TRUE(aggregated_frame->delegated_frame_data);

        DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

        test::Quad expected_quads[] = { test::Quad::SolidColorQuad(SK_ColorWHITE),
            test::Quad::SolidColorQuad(SK_ColorGREEN),
            test::Quad::SolidColorQuad(SK_ColorBLACK) };
        test::Pass expected_passes[] = {
            test::Pass(expected_quads, arraysize(expected_quads)),
            test::Pass(root_quads2, arraysize(root_quads2))
        };
        TestPassesMatchExpectations(expected_passes,
            arraysize(expected_passes),
            &frame_data->render_pass_list);
        ASSERT_EQ(2u, frame_data->render_pass_list.size());
        ASSERT_EQ(1u, frame_data->render_pass_list[0]->copy_requests.size());
        DCHECK_EQ(copy_request_ptr,
            frame_data->render_pass_list[0]->copy_requests[0]);
        ASSERT_EQ(1u, frame_data->render_pass_list[1]->copy_requests.size());
        DCHECK_EQ(copy_request2_ptr,
            frame_data->render_pass_list[1]->copy_requests[0]);

        SurfaceId surface_ids[] = { root_surface_id_, embedded_surface_id };
        EXPECT_EQ(arraysize(surface_ids),
            aggregator_.previous_contained_surfaces().size());
        for (size_t i = 0; i < arraysize(surface_ids); i++) {
            EXPECT_TRUE(
                aggregator_.previous_contained_surfaces().find(surface_ids[i]) != aggregator_.previous_contained_surfaces().end());
        }

        // Ensure copy requests have been removed from root surface.
        const CompositorFrame* original_frame = manager_.GetSurfaceForId(root_surface_id_)->GetEligibleFrame();
        RenderPassList& original_pass_list = original_frame->delegated_frame_data->render_pass_list;
        ASSERT_EQ(2u, original_pass_list.size());
        DCHECK(original_pass_list[0]->copy_requests.empty());
        DCHECK(original_pass_list[1]->copy_requests.empty());

        factory_.Destroy(embedded_surface_id);
    }

    // This tests referencing a surface that has multiple render passes.
    TEST_F(SurfaceAggregatorValidSurfaceTest, MultiPassSurfaceReference)
    {
        SurfaceId embedded_surface_id = child_allocator_.GenerateId();
        factory_.Create(embedded_surface_id);

        RenderPassId pass_ids[] = { RenderPassId(1, 1), RenderPassId(1, 2),
            RenderPassId(1, 3) };

        test::Quad embedded_quads[][2] = {
            { test::Quad::SolidColorQuad(1), test::Quad::SolidColorQuad(2) },
            { test::Quad::SolidColorQuad(3), test::Quad::RenderPassQuad(pass_ids[0]) },
            { test::Quad::SolidColorQuad(4), test::Quad::RenderPassQuad(pass_ids[1]) }
        };
        test::Pass embedded_passes[] = {
            test::Pass(embedded_quads[0], arraysize(embedded_quads[0]), pass_ids[0]),
            test::Pass(embedded_quads[1], arraysize(embedded_quads[1]), pass_ids[1]),
            test::Pass(embedded_quads[2], arraysize(embedded_quads[2]), pass_ids[2])
        };

        SubmitCompositorFrame(embedded_passes, arraysize(embedded_passes),
            embedded_surface_id);

        test::Quad root_quads[][2] = {
            { test::Quad::SolidColorQuad(5), test::Quad::SolidColorQuad(6) },
            { test::Quad::SurfaceQuad(embedded_surface_id, 1.f),
                test::Quad::RenderPassQuad(pass_ids[0]) },
            { test::Quad::SolidColorQuad(7), test::Quad::RenderPassQuad(pass_ids[1]) }
        };
        test::Pass root_passes[] = {
            test::Pass(root_quads[0], arraysize(root_quads[0]), pass_ids[0]),
            test::Pass(root_quads[1], arraysize(root_quads[1]), pass_ids[1]),
            test::Pass(root_quads[2], arraysize(root_quads[2]), pass_ids[2])
        };

        SubmitCompositorFrame(root_passes, arraysize(root_passes), root_surface_id_);

        scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

        ASSERT_TRUE(aggregated_frame);
        ASSERT_TRUE(aggregated_frame->delegated_frame_data);

        DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

        const RenderPassList& aggregated_pass_list = frame_data->render_pass_list;

        ASSERT_EQ(5u, aggregated_pass_list.size());
        RenderPassId actual_pass_ids[] = {
            aggregated_pass_list[0]->id, aggregated_pass_list[1]->id,
            aggregated_pass_list[2]->id, aggregated_pass_list[3]->id,
            aggregated_pass_list[4]->id
        };
        for (size_t i = 0; i < 5; ++i) {
            for (size_t j = 0; j < i; ++j) {
                EXPECT_NE(actual_pass_ids[i], actual_pass_ids[j]);
            }
        }

        {
            SCOPED_TRACE("First pass");
            // The first pass will just be the first pass from the root surfaces quad
            // with no render pass quads to remap.
            TestPassMatchesExpectations(root_passes[0], aggregated_pass_list[0]);
        }

        {
            SCOPED_TRACE("Second pass");
            // The next two passes will be from the embedded surface since we have to
            // draw those passes before they are referenced from the render pass draw
            // quad embedded into the root surface's second pass.
            // First, there's the first embedded pass which doesn't reference anything
            // else.
            TestPassMatchesExpectations(embedded_passes[0], aggregated_pass_list[1]);
        }

        {
            SCOPED_TRACE("Third pass");
            const QuadList& third_pass_quad_list = aggregated_pass_list[2]->quad_list;
            ASSERT_EQ(2u, third_pass_quad_list.size());
            TestQuadMatchesExpectations(embedded_quads[1][0],
                third_pass_quad_list.ElementAt(0));

            // This render pass pass quad will reference the first pass from the
            // embedded surface, which is the second pass in the aggregated frame.
            ASSERT_EQ(DrawQuad::RENDER_PASS,
                third_pass_quad_list.ElementAt(1)->material);
            const RenderPassDrawQuad* third_pass_render_pass_draw_quad = RenderPassDrawQuad::MaterialCast(third_pass_quad_list.ElementAt(1));
            EXPECT_EQ(actual_pass_ids[1],
                third_pass_render_pass_draw_quad->render_pass_id);
        }

        {
            SCOPED_TRACE("Fourth pass");
            // The fourth pass will have aggregated quads from the root surface's second
            // pass and the embedded surface's first pass.
            const QuadList& fourth_pass_quad_list = aggregated_pass_list[3]->quad_list;
            ASSERT_EQ(3u, fourth_pass_quad_list.size());

            // The first quad will be the yellow quad from the embedded surface's last
            // pass.
            TestQuadMatchesExpectations(embedded_quads[2][0],
                fourth_pass_quad_list.ElementAt(0));

            // The next quad will be a render pass quad referencing the second pass from
            // the embedded surface, which is the third pass in the aggregated frame.
            ASSERT_EQ(DrawQuad::RENDER_PASS,
                fourth_pass_quad_list.ElementAt(1)->material);
            const RenderPassDrawQuad* fourth_pass_first_render_pass_draw_quad = RenderPassDrawQuad::MaterialCast(fourth_pass_quad_list.ElementAt(1));
            EXPECT_EQ(actual_pass_ids[2],
                fourth_pass_first_render_pass_draw_quad->render_pass_id);

            // The last quad will be a render pass quad referencing the first pass from
            // the root surface, which is the first pass overall.
            ASSERT_EQ(DrawQuad::RENDER_PASS,
                fourth_pass_quad_list.ElementAt(2)->material);
            const RenderPassDrawQuad* fourth_pass_second_render_pass_draw_quad = RenderPassDrawQuad::MaterialCast(fourth_pass_quad_list.ElementAt(2));
            EXPECT_EQ(actual_pass_ids[0],
                fourth_pass_second_render_pass_draw_quad->render_pass_id);
        }

        {
            SCOPED_TRACE("Fifth pass");
            const QuadList& fifth_pass_quad_list = aggregated_pass_list[4]->quad_list;
            ASSERT_EQ(2u, fifth_pass_quad_list.size());

            TestQuadMatchesExpectations(root_quads[2][0],
                fifth_pass_quad_list.ElementAt(0));

            // The last quad in the last pass will reference the second pass from the
            // root surface, which after aggregating is the fourth pass in the overall
            // list.
            ASSERT_EQ(DrawQuad::RENDER_PASS,
                fifth_pass_quad_list.ElementAt(1)->material);
            const RenderPassDrawQuad* fifth_pass_render_pass_draw_quad = RenderPassDrawQuad::MaterialCast(fifth_pass_quad_list.ElementAt(1));
            EXPECT_EQ(actual_pass_ids[3],
                fifth_pass_render_pass_draw_quad->render_pass_id);
        }
        factory_.Destroy(embedded_surface_id);
    }

    // Tests an invalid surface reference in a frame. The surface quad should just
    // be dropped.
    TEST_F(SurfaceAggregatorValidSurfaceTest, InvalidSurfaceReference)
    {
        test::Quad quads[] = { test::Quad::SolidColorQuad(SK_ColorGREEN),
            test::Quad::SurfaceQuad(InvalidSurfaceId(), 1.f),
            test::Quad::SolidColorQuad(SK_ColorBLUE) };
        test::Pass passes[] = { test::Pass(quads, arraysize(quads)) };

        SubmitCompositorFrame(passes, arraysize(passes), root_surface_id_);

        test::Quad expected_quads[] = { test::Quad::SolidColorQuad(SK_ColorGREEN),
            test::Quad::SolidColorQuad(SK_ColorBLUE) };
        test::Pass expected_passes[] = {
            test::Pass(expected_quads, arraysize(expected_quads))
        };
        SurfaceId ids[] = { root_surface_id_, InvalidSurfaceId() };
        AggregateAndVerify(
            expected_passes, arraysize(expected_passes), ids, arraysize(ids));
    }

    // Tests a reference to a valid surface with no submitted frame. This quad
    // should also just be dropped.
    TEST_F(SurfaceAggregatorValidSurfaceTest, ValidSurfaceReferenceWithNoFrame)
    {
        SurfaceId surface_with_no_frame_id = allocator_.GenerateId();
        factory_.Create(surface_with_no_frame_id);
        test::Quad quads[] = { test::Quad::SolidColorQuad(SK_ColorGREEN),
            test::Quad::SurfaceQuad(surface_with_no_frame_id, 1.f),
            test::Quad::SolidColorQuad(SK_ColorBLUE) };
        test::Pass passes[] = { test::Pass(quads, arraysize(quads)) };

        SubmitCompositorFrame(passes, arraysize(passes), root_surface_id_);

        test::Quad expected_quads[] = { test::Quad::SolidColorQuad(SK_ColorGREEN),
            test::Quad::SolidColorQuad(SK_ColorBLUE) };
        test::Pass expected_passes[] = {
            test::Pass(expected_quads, arraysize(expected_quads))
        };
        SurfaceId ids[] = { root_surface_id_, surface_with_no_frame_id };
        AggregateAndVerify(
            expected_passes, arraysize(expected_passes), ids, arraysize(ids));
        factory_.Destroy(surface_with_no_frame_id);
    }

    // Tests a surface quad referencing itself, generating a trivial cycle.
    // The quad creating the cycle should be dropped from the final frame.
    TEST_F(SurfaceAggregatorValidSurfaceTest, SimpleCyclicalReference)
    {
        test::Quad quads[] = { test::Quad::SurfaceQuad(root_surface_id_, 1.f),
            test::Quad::SolidColorQuad(SK_ColorYELLOW) };
        test::Pass passes[] = { test::Pass(quads, arraysize(quads)) };

        SubmitCompositorFrame(passes, arraysize(passes), root_surface_id_);

        test::Quad expected_quads[] = { test::Quad::SolidColorQuad(SK_ColorYELLOW) };
        test::Pass expected_passes[] = {
            test::Pass(expected_quads, arraysize(expected_quads))
        };
        SurfaceId ids[] = { root_surface_id_ };
        AggregateAndVerify(
            expected_passes, arraysize(expected_passes), ids, arraysize(ids));
    }

    // Tests a more complex cycle with one intermediate surface.
    TEST_F(SurfaceAggregatorValidSurfaceTest, TwoSurfaceCyclicalReference)
    {
        SurfaceId child_surface_id = allocator_.GenerateId();
        factory_.Create(child_surface_id);

        test::Quad parent_quads[] = { test::Quad::SolidColorQuad(SK_ColorBLUE),
            test::Quad::SurfaceQuad(child_surface_id, 1.f),
            test::Quad::SolidColorQuad(SK_ColorCYAN) };
        test::Pass parent_passes[] = {
            test::Pass(parent_quads, arraysize(parent_quads))
        };

        SubmitCompositorFrame(parent_passes, arraysize(parent_passes),
            root_surface_id_);

        test::Quad child_quads[] = { test::Quad::SolidColorQuad(SK_ColorGREEN),
            test::Quad::SurfaceQuad(root_surface_id_, 1.f),
            test::Quad::SolidColorQuad(SK_ColorMAGENTA) };
        test::Pass child_passes[] = { test::Pass(child_quads, arraysize(child_quads)) };

        SubmitCompositorFrame(child_passes, arraysize(child_passes),
            child_surface_id);

        // The child surface's reference to the root_surface_ will be dropped, so
        // we'll end up with:
        //   SK_ColorBLUE from the parent
        //   SK_ColorGREEN from the child
        //   SK_ColorMAGENTA from the child
        //   SK_ColorCYAN from the parent
        test::Quad expected_quads[] = { test::Quad::SolidColorQuad(SK_ColorBLUE),
            test::Quad::SolidColorQuad(SK_ColorGREEN),
            test::Quad::SolidColorQuad(SK_ColorMAGENTA),
            test::Quad::SolidColorQuad(SK_ColorCYAN) };
        test::Pass expected_passes[] = {
            test::Pass(expected_quads, arraysize(expected_quads))
        };
        SurfaceId ids[] = { root_surface_id_, child_surface_id };
        AggregateAndVerify(
            expected_passes, arraysize(expected_passes), ids, arraysize(ids));
        factory_.Destroy(child_surface_id);
    }

    // Tests that we map render pass IDs from different surfaces into a unified
    // namespace and update RenderPassDrawQuad's id references to match.
    TEST_F(SurfaceAggregatorValidSurfaceTest, RenderPassIdMapping)
    {
        SurfaceId child_surface_id = allocator_.GenerateId();
        factory_.Create(child_surface_id);

        RenderPassId child_pass_id[] = { RenderPassId(1, 1), RenderPassId(1, 2) };
        test::Quad child_quad[][1] = { { test::Quad::SolidColorQuad(SK_ColorGREEN) },
            { test::Quad::RenderPassQuad(child_pass_id[0]) } };
        test::Pass surface_passes[] = {
            test::Pass(child_quad[0], arraysize(child_quad[0]), child_pass_id[0]),
            test::Pass(child_quad[1], arraysize(child_quad[1]), child_pass_id[1])
        };

        SubmitCompositorFrame(surface_passes, arraysize(surface_passes),
            child_surface_id);

        // Pass IDs from the parent surface may collide with ones from the child.
        RenderPassId parent_pass_id[] = { RenderPassId(2, 1), RenderPassId(1, 2) };
        test::Quad parent_quad[][1] = {
            { test::Quad::SurfaceQuad(child_surface_id, 1.f) },
            { test::Quad::RenderPassQuad(parent_pass_id[0]) }
        };
        test::Pass parent_passes[] = {
            test::Pass(parent_quad[0], arraysize(parent_quad[0]), parent_pass_id[0]),
            test::Pass(parent_quad[1], arraysize(parent_quad[1]), parent_pass_id[1])
        };

        SubmitCompositorFrame(parent_passes, arraysize(parent_passes),
            root_surface_id_);
        scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

        ASSERT_TRUE(aggregated_frame);
        ASSERT_TRUE(aggregated_frame->delegated_frame_data);

        DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

        const RenderPassList& aggregated_pass_list = frame_data->render_pass_list;

        ASSERT_EQ(3u, aggregated_pass_list.size());
        RenderPassId actual_pass_ids[] = { aggregated_pass_list[0]->id,
            aggregated_pass_list[1]->id,
            aggregated_pass_list[2]->id };
        // Make sure the aggregated frame's pass IDs are all unique.
        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < i; ++j) {
                EXPECT_NE(actual_pass_ids[j], actual_pass_ids[i]) << "pass ids " << i
                                                                  << " and " << j;
            }
        }

        // Make sure the render pass quads reference the remapped pass IDs.
        DrawQuad* render_pass_quads[] = { aggregated_pass_list[1]->quad_list.front(),
            aggregated_pass_list[2]->quad_list.front() };
        ASSERT_EQ(render_pass_quads[0]->material, DrawQuad::RENDER_PASS);
        EXPECT_EQ(
            actual_pass_ids[0],
            RenderPassDrawQuad::MaterialCast(render_pass_quads[0])->render_pass_id);

        ASSERT_EQ(render_pass_quads[1]->material, DrawQuad::RENDER_PASS);
        EXPECT_EQ(
            actual_pass_ids[1],
            RenderPassDrawQuad::MaterialCast(render_pass_quads[1])->render_pass_id);
        factory_.Destroy(child_surface_id);
    }

    void AddSolidColorQuadWithBlendMode(const gfx::Size& size,
        RenderPass* pass,
        const SkXfermode::Mode blend_mode)
    {
        const gfx::Transform layer_to_target_transform;
        const gfx::Size layer_bounds(size);
        const gfx::Rect visible_layer_rect(size);
        const gfx::Rect clip_rect(size);

        bool is_clipped = false;
        float opacity = 1.f;

        bool force_anti_aliasing_off = false;
        SharedQuadState* sqs = pass->CreateAndAppendSharedQuadState();
        sqs->SetAll(layer_to_target_transform, layer_bounds, visible_layer_rect,
            clip_rect, is_clipped, opacity, blend_mode, 0);

        SolidColorDrawQuad* color_quad = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad->SetNew(pass->shared_quad_state_list.back(), visible_layer_rect,
            visible_layer_rect, SK_ColorGREEN,
            force_anti_aliasing_off);
    }

    // This tests that we update shared quad state pointers correctly within
    // aggregated passes.  The shared quad state list on the aggregated pass will
    // include the shared quad states from each pass in one list so the quads will
    // end up pointed to shared quad state objects at different offsets. This test
    // uses the blend_mode value stored on the shared quad state to track the shared
    // quad state, but anything saved on the shared quad state would work.
    //
    // This test has 4 surfaces in the following structure:
    // root_surface -> quad with kClear_Mode,
    //                 [child_one_surface],
    //                 quad with kDstOver_Mode,
    //                 [child_two_surface],
    //                 quad with kDstIn_Mode
    // child_one_surface -> quad with kSrc_Mode,
    //                      [grandchild_surface],
    //                      quad with kSrcOver_Mode
    // child_two_surface -> quad with kSrcIn_Mode
    // grandchild_surface -> quad with kDst_Mode
    //
    // Resulting in the following aggregated pass:
    //  quad_root_0       - blend_mode kClear_Mode
    //  quad_child_one_0  - blend_mode kSrc_Mode
    //  quad_grandchild_0 - blend_mode kDst_Mode
    //  quad_child_one_1  - blend_mode kSrcOver_Mode
    //  quad_root_1       - blend_mode kDstOver_Mode
    //  quad_child_two_0  - blend_mode kSrcIn_Mode
    //  quad_root_2       - blend_mode kDstIn_Mode
    TEST_F(SurfaceAggregatorValidSurfaceTest, AggregateSharedQuadStateProperties)
    {
        const SkXfermode::Mode blend_modes[] = {
            SkXfermode::kClear_Mode, // 0
            SkXfermode::kSrc_Mode, // 1
            SkXfermode::kDst_Mode, // 2
            SkXfermode::kSrcOver_Mode, // 3
            SkXfermode::kDstOver_Mode, // 4
            SkXfermode::kSrcIn_Mode, // 5
            SkXfermode::kDstIn_Mode, // 6
        };

        RenderPassId pass_id(1, 1);
        SurfaceId grandchild_surface_id = allocator_.GenerateId();
        factory_.Create(grandchild_surface_id);
        scoped_ptr<RenderPass> grandchild_pass = RenderPass::Create();
        gfx::Rect output_rect(SurfaceSize());
        gfx::Rect damage_rect(SurfaceSize());
        gfx::Transform transform_to_root_target;
        grandchild_pass->SetNew(
            pass_id, output_rect, damage_rect, transform_to_root_target);
        AddSolidColorQuadWithBlendMode(
            SurfaceSize(), grandchild_pass.get(), blend_modes[2]);
        QueuePassAsFrame(grandchild_pass.Pass(), grandchild_surface_id);

        SurfaceId child_one_surface_id = allocator_.GenerateId();
        factory_.Create(child_one_surface_id);

        scoped_ptr<RenderPass> child_one_pass = RenderPass::Create();
        child_one_pass->SetNew(
            pass_id, output_rect, damage_rect, transform_to_root_target);
        AddSolidColorQuadWithBlendMode(
            SurfaceSize(), child_one_pass.get(), blend_modes[1]);
        SurfaceDrawQuad* grandchild_surface_quad = child_one_pass->CreateAndAppendDrawQuad<SurfaceDrawQuad>();
        grandchild_surface_quad->SetNew(child_one_pass->shared_quad_state_list.back(),
            gfx::Rect(SurfaceSize()),
            gfx::Rect(SurfaceSize()),
            grandchild_surface_id);
        AddSolidColorQuadWithBlendMode(
            SurfaceSize(), child_one_pass.get(), blend_modes[3]);
        QueuePassAsFrame(child_one_pass.Pass(), child_one_surface_id);

        SurfaceId child_two_surface_id = allocator_.GenerateId();
        factory_.Create(child_two_surface_id);

        scoped_ptr<RenderPass> child_two_pass = RenderPass::Create();
        child_two_pass->SetNew(
            pass_id, output_rect, damage_rect, transform_to_root_target);
        AddSolidColorQuadWithBlendMode(
            SurfaceSize(), child_two_pass.get(), blend_modes[5]);
        QueuePassAsFrame(child_two_pass.Pass(), child_two_surface_id);

        scoped_ptr<RenderPass> root_pass = RenderPass::Create();
        root_pass->SetNew(
            pass_id, output_rect, damage_rect, transform_to_root_target);

        AddSolidColorQuadWithBlendMode(
            SurfaceSize(), root_pass.get(), blend_modes[0]);
        SurfaceDrawQuad* child_one_surface_quad = root_pass->CreateAndAppendDrawQuad<SurfaceDrawQuad>();
        child_one_surface_quad->SetNew(root_pass->shared_quad_state_list.back(),
            gfx::Rect(SurfaceSize()),
            gfx::Rect(SurfaceSize()),
            child_one_surface_id);
        AddSolidColorQuadWithBlendMode(
            SurfaceSize(), root_pass.get(), blend_modes[4]);
        SurfaceDrawQuad* child_two_surface_quad = root_pass->CreateAndAppendDrawQuad<SurfaceDrawQuad>();
        child_two_surface_quad->SetNew(root_pass->shared_quad_state_list.back(),
            gfx::Rect(SurfaceSize()),
            gfx::Rect(SurfaceSize()),
            child_two_surface_id);
        AddSolidColorQuadWithBlendMode(
            SurfaceSize(), root_pass.get(), blend_modes[6]);

        QueuePassAsFrame(root_pass.Pass(), root_surface_id_);

        scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

        ASSERT_TRUE(aggregated_frame);
        ASSERT_TRUE(aggregated_frame->delegated_frame_data);

        DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

        const RenderPassList& aggregated_pass_list = frame_data->render_pass_list;

        ASSERT_EQ(1u, aggregated_pass_list.size());

        const QuadList& aggregated_quad_list = aggregated_pass_list[0]->quad_list;

        ASSERT_EQ(7u, aggregated_quad_list.size());

        for (auto iter = aggregated_quad_list.cbegin();
             iter != aggregated_quad_list.cend();
             ++iter) {
            EXPECT_EQ(blend_modes[iter.index()], iter->shared_quad_state->blend_mode)
                << iter.index();
        }
        factory_.Destroy(child_one_surface_id);
        factory_.Destroy(child_two_surface_id);
        factory_.Destroy(grandchild_surface_id);
    }

    // This tests that when aggregating a frame with multiple render passes that we
    // map the transforms for the root pass but do not modify the transform on child
    // passes.
    //
    // The root surface has one pass with a surface quad transformed by +10 in the y
    // direction.
    //
    // The middle surface has one pass with a surface quad scaled by 2 in the x
    // and 3 in the y directions.
    //
    // The child surface has two passes. The first pass has a quad with a transform
    // of +5 in the x direction. The second pass has a reference to the first pass'
    // pass id and a transform of +8 in the x direction.
    //
    // After aggregation, the child surface's root pass quad should have all
    // transforms concatenated for a total transform of +23 x, +10 y. The
    // contributing render pass' transform in the aggregate frame should not be
    // affected.
    TEST_F(SurfaceAggregatorValidSurfaceTest, AggregateMultiplePassWithTransform)
    {
        // Innermost child surface.
        SurfaceId child_surface_id = allocator_.GenerateId();
        {
            factory_.Create(child_surface_id);
            RenderPassId child_pass_id[] = { RenderPassId(1, 1), RenderPassId(1, 2) };
            test::Quad child_quads[][1] = {
                { test::Quad::SolidColorQuad(SK_ColorGREEN) },
                { test::Quad::RenderPassQuad(child_pass_id[0]) },
            };
            test::Pass child_passes[] = {
                test::Pass(child_quads[0], arraysize(child_quads[0]), child_pass_id[0]),
                test::Pass(child_quads[1], arraysize(child_quads[1]),
                    child_pass_id[1])
            };

            RenderPassList child_pass_list;
            AddPasses(&child_pass_list, gfx::Rect(SurfaceSize()), child_passes,
                arraysize(child_passes));

            RenderPass* child_nonroot_pass = child_pass_list.at(0u);
            child_nonroot_pass->transform_to_root_target.Translate(8, 0);
            SharedQuadState* child_nonroot_pass_sqs = child_nonroot_pass->shared_quad_state_list.front();
            child_nonroot_pass_sqs->quad_to_target_transform.Translate(5, 0);

            RenderPass* child_root_pass = child_pass_list.at(1u);
            SharedQuadState* child_root_pass_sqs = child_root_pass->shared_quad_state_list.front();
            child_root_pass_sqs->quad_to_target_transform.Translate(8, 0);
            child_root_pass_sqs->is_clipped = true;
            child_root_pass_sqs->clip_rect = gfx::Rect(0, 0, 5, 5);

            scoped_ptr<DelegatedFrameData> child_frame_data(new DelegatedFrameData);
            child_pass_list.swap(child_frame_data->render_pass_list);

            scoped_ptr<CompositorFrame> child_frame(new CompositorFrame);
            child_frame->delegated_frame_data = child_frame_data.Pass();

            factory_.SubmitCompositorFrame(child_surface_id, child_frame.Pass(),
                SurfaceFactory::DrawCallback());
        }

        // Middle child surface.
        SurfaceId middle_surface_id = allocator_.GenerateId();
        {
            factory_.Create(middle_surface_id);
            test::Quad middle_quads[] = {
                test::Quad::SurfaceQuad(child_surface_id, 1.f)
            };
            test::Pass middle_passes[] = {
                test::Pass(middle_quads, arraysize(middle_quads)),
            };

            RenderPassList middle_pass_list;
            AddPasses(&middle_pass_list, gfx::Rect(SurfaceSize()), middle_passes,
                arraysize(middle_passes));

            RenderPass* middle_root_pass = middle_pass_list.at(0u);
            middle_root_pass->quad_list.ElementAt(0)->visible_rect = gfx::Rect(0, 1, 100, 7);
            SharedQuadState* middle_root_pass_sqs = middle_root_pass->shared_quad_state_list.front();
            middle_root_pass_sqs->quad_to_target_transform.Scale(2, 3);

            scoped_ptr<DelegatedFrameData> middle_frame_data(new DelegatedFrameData);
            middle_pass_list.swap(middle_frame_data->render_pass_list);

            scoped_ptr<CompositorFrame> middle_frame(new CompositorFrame);
            middle_frame->delegated_frame_data = middle_frame_data.Pass();

            factory_.SubmitCompositorFrame(middle_surface_id, middle_frame.Pass(),
                SurfaceFactory::DrawCallback());
        }

        // Root surface.
        test::Quad secondary_quads[] = {
            test::Quad::SolidColorQuad(1),
            test::Quad::SurfaceQuad(middle_surface_id, 1.f)
        };
        test::Quad root_quads[] = { test::Quad::SolidColorQuad(1) };
        test::Pass root_passes[] = {
            test::Pass(secondary_quads, arraysize(secondary_quads)),
            test::Pass(root_quads, arraysize(root_quads))
        };

        RenderPassList root_pass_list;
        AddPasses(&root_pass_list,
            gfx::Rect(SurfaceSize()),
            root_passes,
            arraysize(root_passes));

        root_pass_list.at(0)
            ->shared_quad_state_list.front()
            ->quad_to_target_transform.Translate(0, 7);
        root_pass_list.at(0)
            ->shared_quad_state_list.ElementAt(1)
            ->quad_to_target_transform.Translate(0, 10);
        root_pass_list.at(0)->quad_list.ElementAt(1)->visible_rect = gfx::Rect(0, 0, 8, 100);

        root_pass_list[0]->transform_to_root_target.Translate(10, 5);

        scoped_ptr<DelegatedFrameData> root_frame_data(new DelegatedFrameData);
        root_pass_list.swap(root_frame_data->render_pass_list);

        scoped_ptr<CompositorFrame> root_frame(new CompositorFrame);
        root_frame->delegated_frame_data = root_frame_data.Pass();

        factory_.SubmitCompositorFrame(root_surface_id_, root_frame.Pass(),
            SurfaceFactory::DrawCallback());

        scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

        ASSERT_TRUE(aggregated_frame);
        ASSERT_TRUE(aggregated_frame->delegated_frame_data);

        DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

        const RenderPassList& aggregated_pass_list = frame_data->render_pass_list;

        ASSERT_EQ(3u, aggregated_pass_list.size());

        ASSERT_EQ(1u, aggregated_pass_list[0]->shared_quad_state_list.size());

        // The first pass should have one shared quad state for the one solid color
        // quad.
        EXPECT_EQ(1u, aggregated_pass_list[0]->shared_quad_state_list.size());
        // The second pass should have just two shared quad states. We'll
        // verify the properties through the quads.
        EXPECT_EQ(2u, aggregated_pass_list[1]->shared_quad_state_list.size());

        EXPECT_EQ(1u, aggregated_pass_list[2]->shared_quad_state_list.size());

        SharedQuadState* aggregated_first_pass_sqs = aggregated_pass_list[0]->shared_quad_state_list.front();

        // The first pass's transform should be unaffected by the embedding and still
        // be a translation by +5 in the x direction.
        gfx::Transform expected_aggregated_first_pass_sqs_transform;
        expected_aggregated_first_pass_sqs_transform.Translate(5, 0);
        EXPECT_EQ(expected_aggregated_first_pass_sqs_transform.ToString(),
            aggregated_first_pass_sqs->quad_to_target_transform.ToString());

        // The first pass's transform to the root target should include the aggregated
        // transform, including the transform from the child pass to the root.
        gfx::Transform expected_first_pass_transform_to_root_target;
        expected_first_pass_transform_to_root_target.Translate(10, 5);
        expected_first_pass_transform_to_root_target.Translate(0, 10);
        expected_first_pass_transform_to_root_target.Scale(2, 3);
        expected_first_pass_transform_to_root_target.Translate(8, 0);
        EXPECT_EQ(expected_first_pass_transform_to_root_target.ToString(),
            aggregated_pass_list[0]->transform_to_root_target.ToString());

        ASSERT_EQ(2u, aggregated_pass_list[1]->quad_list.size());

        gfx::Transform expected_root_pass_quad_transforms[2];
        // The first quad in the root pass is the solid color quad from the original
        // root surface. Its transform should be unaffected by the aggregation and
        // still be +7 in the y direction.
        expected_root_pass_quad_transforms[0].Translate(0, 7);
        // The second quad in the root pass is aggregated from the child surface so
        // its transform should be the combination of its original translation
        // (0, 10), the middle surface draw quad's scale of (2, 3), and the
        // child surface draw quad's translation (8, 0).
        expected_root_pass_quad_transforms[1].Translate(0, 10);
        expected_root_pass_quad_transforms[1].Scale(2, 3);
        expected_root_pass_quad_transforms[1].Translate(8, 0);

        for (auto iter = aggregated_pass_list[1]->quad_list.cbegin();
             iter != aggregated_pass_list[1]->quad_list.cend();
             ++iter) {
            EXPECT_EQ(expected_root_pass_quad_transforms[iter.index()].ToString(),
                iter->shared_quad_state->quad_to_target_transform.ToString())
                << iter.index();
        }

        EXPECT_TRUE(
            aggregated_pass_list[1]->shared_quad_state_list.ElementAt(1)->is_clipped);

        // The second quad in the root pass is aggregated from the child, so its
        // clip rect must be transformed by the child's translation/scale and
        // clipped be the visible_rects for both children.
        EXPECT_EQ(gfx::Rect(0, 13, 8, 12).ToString(),
            aggregated_pass_list[1]
                ->shared_quad_state_list.ElementAt(1)
                ->clip_rect.ToString());

        factory_.Destroy(middle_surface_id);
        factory_.Destroy(child_surface_id);
    }

    // Tests that damage rects are aggregated correctly when surfaces change.
    TEST_F(SurfaceAggregatorValidSurfaceTest, AggregateDamageRect)
    {
        test::Quad child_quads[] = { test::Quad::RenderPassQuad(RenderPassId(1, 1)) };
        test::Pass child_passes[] = {
            test::Pass(child_quads, arraysize(child_quads), RenderPassId(1, 1))
        };

        RenderPassList child_pass_list;
        AddPasses(&child_pass_list,
            gfx::Rect(SurfaceSize()),
            child_passes,
            arraysize(child_passes));

        RenderPass* child_root_pass = child_pass_list.at(0u);
        SharedQuadState* child_root_pass_sqs = child_root_pass->shared_quad_state_list.front();
        child_root_pass_sqs->quad_to_target_transform.Translate(8, 0);

        scoped_ptr<DelegatedFrameData> child_frame_data(new DelegatedFrameData);
        child_pass_list.swap(child_frame_data->render_pass_list);

        scoped_ptr<CompositorFrame> child_frame(new CompositorFrame);
        child_frame->delegated_frame_data = child_frame_data.Pass();

        SurfaceId child_surface_id = allocator_.GenerateId();
        factory_.Create(child_surface_id);
        factory_.SubmitCompositorFrame(child_surface_id, child_frame.Pass(),
            SurfaceFactory::DrawCallback());

        test::Quad parent_surface_quads[] = {
            test::Quad::SurfaceQuad(child_surface_id, 1.f)
        };
        test::Pass parent_surface_passes[] = {
            test::Pass(parent_surface_quads, arraysize(parent_surface_quads),
                RenderPassId(1, 1))
        };

        RenderPassList parent_surface_pass_list;
        AddPasses(&parent_surface_pass_list,
            gfx::Rect(SurfaceSize()),
            parent_surface_passes,
            arraysize(parent_surface_passes));

        // Parent surface is only used to test if the transform is applied correctly
        // to the child surface's damage.
        scoped_ptr<DelegatedFrameData> parent_surface_frame_data(
            new DelegatedFrameData);
        parent_surface_pass_list.swap(parent_surface_frame_data->render_pass_list);

        scoped_ptr<CompositorFrame> parent_surface_frame(new CompositorFrame);
        parent_surface_frame->delegated_frame_data = parent_surface_frame_data.Pass();

        SurfaceId parent_surface_id = allocator_.GenerateId();
        factory_.Create(parent_surface_id);
        factory_.SubmitCompositorFrame(parent_surface_id, parent_surface_frame.Pass(),
            SurfaceFactory::DrawCallback());

        test::Quad root_surface_quads[] = {
            test::Quad::SurfaceQuad(parent_surface_id, 1.f)
        };
        test::Quad root_render_pass_quads[] = {
            test::Quad::RenderPassQuad(RenderPassId(1, 1))
        };

        test::Pass root_passes[] = {
            test::Pass(root_surface_quads, arraysize(root_surface_quads),
                RenderPassId(1, 1)),
            test::Pass(root_render_pass_quads, arraysize(root_render_pass_quads),
                RenderPassId(2, 1))
        };

        RenderPassList root_pass_list;
        AddPasses(&root_pass_list,
            gfx::Rect(SurfaceSize()),
            root_passes,
            arraysize(root_passes));

        root_pass_list.at(0)
            ->shared_quad_state_list.front()
            ->quad_to_target_transform.Translate(0, 10);
        root_pass_list.at(0)->damage_rect = gfx::Rect(5, 5, 10, 10);
        root_pass_list.at(1)->damage_rect = gfx::Rect(5, 5, 100, 100);

        scoped_ptr<DelegatedFrameData> root_frame_data(new DelegatedFrameData);
        root_pass_list.swap(root_frame_data->render_pass_list);

        scoped_ptr<CompositorFrame> root_frame(new CompositorFrame);
        root_frame->delegated_frame_data = root_frame_data.Pass();

        factory_.SubmitCompositorFrame(root_surface_id_, root_frame.Pass(),
            SurfaceFactory::DrawCallback());

        scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

        ASSERT_TRUE(aggregated_frame);
        ASSERT_TRUE(aggregated_frame->delegated_frame_data);

        DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

        const RenderPassList& aggregated_pass_list = frame_data->render_pass_list;

        ASSERT_EQ(2u, aggregated_pass_list.size());

        // Damage rect for first aggregation should contain entire root surface.
        EXPECT_TRUE(
            aggregated_pass_list[1]->damage_rect.Contains(gfx::Rect(SurfaceSize())));

        {
            AddPasses(&child_pass_list,
                gfx::Rect(SurfaceSize()),
                child_passes,
                arraysize(child_passes));

            RenderPass* child_root_pass = child_pass_list.at(0u);
            SharedQuadState* child_root_pass_sqs = child_root_pass->shared_quad_state_list.front();
            child_root_pass_sqs->quad_to_target_transform.Translate(8, 0);
            child_root_pass->damage_rect = gfx::Rect(10, 10, 10, 10);

            scoped_ptr<DelegatedFrameData> child_frame_data(new DelegatedFrameData);
            child_pass_list.swap(child_frame_data->render_pass_list);

            scoped_ptr<CompositorFrame> child_frame(new CompositorFrame);
            child_frame->delegated_frame_data = child_frame_data.Pass();

            factory_.SubmitCompositorFrame(child_surface_id, child_frame.Pass(),
                SurfaceFactory::DrawCallback());

            scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

            ASSERT_TRUE(aggregated_frame);
            ASSERT_TRUE(aggregated_frame->delegated_frame_data);

            DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

            const RenderPassList& aggregated_pass_list = frame_data->render_pass_list;

            ASSERT_EQ(2u, aggregated_pass_list.size());

            // Outer surface didn't change, so transformed inner damage rect should be
            // used.
            EXPECT_EQ(gfx::Rect(10, 20, 10, 10).ToString(),
                aggregated_pass_list[1]->damage_rect.ToString());
        }

        {
            RenderPassList root_pass_list;
            AddPasses(&root_pass_list,
                gfx::Rect(SurfaceSize()),
                root_passes,
                arraysize(root_passes));

            root_pass_list.at(0)
                ->shared_quad_state_list.front()
                ->quad_to_target_transform.Translate(0, 10);
            root_pass_list.at(0)->damage_rect = gfx::Rect(0, 0, 1, 1);

            scoped_ptr<DelegatedFrameData> root_frame_data(new DelegatedFrameData);
            root_pass_list.swap(root_frame_data->render_pass_list);

            scoped_ptr<CompositorFrame> root_frame(new CompositorFrame);
            root_frame->delegated_frame_data = root_frame_data.Pass();

            factory_.SubmitCompositorFrame(root_surface_id_, root_frame.Pass(),
                SurfaceFactory::DrawCallback());
        }

        {
            RenderPassList root_pass_list;
            AddPasses(&root_pass_list,
                gfx::Rect(SurfaceSize()),
                root_passes,
                arraysize(root_passes));

            root_pass_list.at(0)
                ->shared_quad_state_list.front()
                ->quad_to_target_transform.Translate(0, 10);
            root_pass_list.at(0)->damage_rect = gfx::Rect(1, 1, 1, 1);

            scoped_ptr<DelegatedFrameData> root_frame_data(new DelegatedFrameData);
            root_pass_list.swap(root_frame_data->render_pass_list);

            scoped_ptr<CompositorFrame> root_frame(new CompositorFrame);
            root_frame->delegated_frame_data = root_frame_data.Pass();

            factory_.SubmitCompositorFrame(root_surface_id_, root_frame.Pass(),
                SurfaceFactory::DrawCallback());

            scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

            ASSERT_TRUE(aggregated_frame);
            ASSERT_TRUE(aggregated_frame->delegated_frame_data);

            DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

            const RenderPassList& aggregated_pass_list = frame_data->render_pass_list;

            ASSERT_EQ(2u, aggregated_pass_list.size());

            // The root surface was enqueued without being aggregated once, so it should
            // be treated as completely damaged.
            EXPECT_TRUE(aggregated_pass_list[1]->damage_rect.Contains(
                gfx::Rect(SurfaceSize())));
        }

        // No Surface changed, so no damage should be given.
        {
            scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

            ASSERT_TRUE(aggregated_frame);
            ASSERT_TRUE(aggregated_frame->delegated_frame_data);

            DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

            const RenderPassList& aggregated_pass_list = frame_data->render_pass_list;

            ASSERT_EQ(2u, aggregated_pass_list.size());

            EXPECT_TRUE(aggregated_pass_list[1]->damage_rect.IsEmpty());
        }

        // SetFullDamageRectForSurface should cause the entire output to be
        // marked as damaged.
        {
            aggregator_.SetFullDamageForSurface(root_surface_id_);
            scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

            ASSERT_TRUE(aggregated_frame);
            ASSERT_TRUE(aggregated_frame->delegated_frame_data);

            DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

            const RenderPassList& aggregated_pass_list = frame_data->render_pass_list;

            ASSERT_EQ(2u, aggregated_pass_list.size());

            EXPECT_TRUE(aggregated_pass_list[1]->damage_rect.Contains(
                gfx::Rect(SurfaceSize())));
        }

        factory_.Destroy(child_surface_id);
    }

    class SurfaceAggregatorPartialSwapTest
        : public SurfaceAggregatorValidSurfaceTest {
    public:
        SurfaceAggregatorPartialSwapTest()
            : SurfaceAggregatorValidSurfaceTest(true)
        {
        }
    };

    // Tests that quads outside the damage rect are ignored.
    TEST_F(SurfaceAggregatorPartialSwapTest, IgnoreOutside)
    {
        SurfaceId child_surface_id = allocator_.GenerateId();
        factory_.Create(child_surface_id);
        // The child surface has two quads, one with a visible rect of 13,13 4x4 and
        // the other other with a visible rect of 10,10 2x2 (relative to root target
        // space).
        {
            RenderPassId child_pass_id = RenderPassId(1, 1);
            test::Quad child_quads1[] = { test::Quad::RenderPassQuad(child_pass_id) };
            test::Quad child_quads2[] = { test::Quad::RenderPassQuad(child_pass_id) };
            test::Pass child_passes[] = {
                test::Pass(child_quads1, arraysize(child_quads1), child_pass_id),
                test::Pass(child_quads2, arraysize(child_quads2), child_pass_id)
            };

            RenderPassList child_pass_list;
            AddPasses(&child_pass_list, gfx::Rect(SurfaceSize()), child_passes,
                arraysize(child_passes));

            child_pass_list.at(0u)->quad_list.ElementAt(0)->visible_rect = gfx::Rect(1, 1, 2, 2);
            SharedQuadState* child_sqs = child_pass_list.at(0u)->shared_quad_state_list.ElementAt(0u);
            child_sqs->quad_to_target_transform.Translate(1, 1);
            child_sqs->quad_to_target_transform.Scale(2, 2);

            child_pass_list.at(1u)->quad_list.ElementAt(0)->visible_rect = gfx::Rect(0, 0, 2, 2);

            SubmitPassListAsFrame(child_surface_id, &child_pass_list);
        }

        {
            test::Quad root_quads[] = { test::Quad::SurfaceQuad(child_surface_id, 1.f) };

            test::Pass root_passes[] = { test::Pass(root_quads, arraysize(root_quads)) };

            RenderPassList root_pass_list;
            AddPasses(&root_pass_list, gfx::Rect(SurfaceSize()), root_passes,
                arraysize(root_passes));

            RenderPass* root_pass = root_pass_list.at(0u);
            root_pass->shared_quad_state_list.front()
                ->quad_to_target_transform.Translate(10, 10);
            root_pass->damage_rect = gfx::Rect(0, 0, 1, 1);

            SubmitPassListAsFrame(root_surface_id_, &root_pass_list);
        }

        scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

        ASSERT_TRUE(aggregated_frame);
        ASSERT_TRUE(aggregated_frame->delegated_frame_data);

        DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

        const RenderPassList& aggregated_pass_list = frame_data->render_pass_list;

        ASSERT_EQ(2u, aggregated_pass_list.size());

        // Damage rect for first aggregation should contain entire root surface.
        EXPECT_EQ(gfx::Rect(0, 0, 15, 15), aggregated_pass_list[1]->damage_rect);
        EXPECT_EQ(1u, aggregated_pass_list[0]->quad_list.size());
        EXPECT_EQ(1u, aggregated_pass_list[1]->quad_list.size());

        // Create a root surface with a smaller damage rect.
        {
            test::Quad root_quads[] = { test::Quad::SurfaceQuad(child_surface_id, 1.f) };

            test::Pass root_passes[] = { test::Pass(root_quads, arraysize(root_quads)) };

            RenderPassList root_pass_list;
            AddPasses(&root_pass_list, gfx::Rect(SurfaceSize()), root_passes,
                arraysize(root_passes));

            RenderPass* root_pass = root_pass_list.at(0u);
            root_pass->shared_quad_state_list.front()
                ->quad_to_target_transform.Translate(10, 10);
            root_pass->damage_rect = gfx::Rect(10, 10, 2, 2);
            SubmitPassListAsFrame(root_surface_id_, &root_pass_list);
        }

        {
            scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

            ASSERT_TRUE(aggregated_frame);
            ASSERT_TRUE(aggregated_frame->delegated_frame_data);

            DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

            const RenderPassList& aggregated_pass_list = frame_data->render_pass_list;

            ASSERT_EQ(2u, aggregated_pass_list.size());

            // Only first quad from surface is inside damage rect and should be
            // included.
            EXPECT_EQ(gfx::Rect(10, 10, 2, 2), aggregated_pass_list[1]->damage_rect);
            EXPECT_EQ(0u, aggregated_pass_list[0]->quad_list.size());
            EXPECT_EQ(1u, aggregated_pass_list[1]->quad_list.size());
            EXPECT_EQ(gfx::Rect(0, 0, 2, 2),
                aggregated_pass_list[1]->quad_list.back()->visible_rect);
        }

        // New child frame has same content and no damage, but has a
        // CopyOutputRequest.
        {
            RenderPassId child_pass_id = RenderPassId(1, 1);
            test::Quad child_quads1[] = { test::Quad::RenderPassQuad(child_pass_id) };
            test::Quad child_quads2[] = { test::Quad::RenderPassQuad(child_pass_id) };
            test::Pass child_passes[] = {
                test::Pass(child_quads1, arraysize(child_quads1), child_pass_id),
                test::Pass(child_quads2, arraysize(child_quads2), child_pass_id)
            };

            RenderPassList child_pass_list;
            AddPasses(&child_pass_list, gfx::Rect(SurfaceSize()), child_passes,
                arraysize(child_passes));

            child_pass_list.at(0u)->quad_list.ElementAt(0)->visible_rect = gfx::Rect(1, 1, 2, 2);
            SharedQuadState* child_sqs = child_pass_list.at(0u)->shared_quad_state_list.ElementAt(0u);
            child_sqs->quad_to_target_transform.Translate(1, 1);
            child_sqs->quad_to_target_transform.Scale(2, 2);

            child_pass_list.at(1u)->quad_list.ElementAt(0)->visible_rect = gfx::Rect(0, 0, 2, 2);

            RenderPass* child_root_pass = child_pass_list.at(1u);

            child_root_pass->copy_requests.push_back(
                CopyOutputRequest::CreateEmptyRequest());
            child_root_pass->damage_rect = gfx::Rect();
            SubmitPassListAsFrame(child_surface_id, &child_pass_list);
        }

        {
            scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

            ASSERT_TRUE(aggregated_frame);
            ASSERT_TRUE(aggregated_frame->delegated_frame_data);

            DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

            const RenderPassList& aggregated_pass_list = frame_data->render_pass_list;

            // Output frame should have no damage, but all quads included.
            ASSERT_EQ(3u, aggregated_pass_list.size());

            EXPECT_TRUE(aggregated_pass_list[1]->damage_rect.IsEmpty());
            ASSERT_EQ(1u, aggregated_pass_list[0]->quad_list.size());
            ASSERT_EQ(1u, aggregated_pass_list[1]->quad_list.size());
            EXPECT_EQ(gfx::Rect(1, 1, 2, 2),
                aggregated_pass_list[0]->quad_list.ElementAt(0)->visible_rect);
            EXPECT_EQ(gfx::Rect(0, 0, 2, 2),
                aggregated_pass_list[1]->quad_list.ElementAt(0)->visible_rect);
        }

        {
            scoped_ptr<CompositorFrame> aggregated_frame = aggregator_.Aggregate(root_surface_id_);

            ASSERT_TRUE(aggregated_frame);
            ASSERT_TRUE(aggregated_frame->delegated_frame_data);

            DelegatedFrameData* frame_data = aggregated_frame->delegated_frame_data.get();

            const RenderPassList& aggregated_pass_list = frame_data->render_pass_list;
            // There were no changes since last aggregation, so output should be empty
            // and have no damage.
            ASSERT_EQ(1u, aggregated_pass_list.size());
            EXPECT_TRUE(aggregated_pass_list[0]->damage_rect.IsEmpty());
            ASSERT_EQ(0u, aggregated_pass_list[0]->quad_list.size());
        }

        factory_.Destroy(child_surface_id);
    }

    class SurfaceAggregatorWithResourcesTest : public testing::Test {
    public:
        void SetUp() override
        {
            output_surface_ = FakeOutputSurface::CreateSoftware(
                make_scoped_ptr(new SoftwareOutputDevice));
            output_surface_->BindToClient(&output_surface_client_);
            shared_bitmap_manager_.reset(new TestSharedBitmapManager);

            resource_provider_ = FakeResourceProvider::Create(
                output_surface_.get(), shared_bitmap_manager_.get());

            aggregator_.reset(
                new SurfaceAggregator(&manager_, resource_provider_.get(), false));
        }

    protected:
        SurfaceManager manager_;
        FakeOutputSurfaceClient output_surface_client_;
        scoped_ptr<OutputSurface> output_surface_;
        scoped_ptr<SharedBitmapManager> shared_bitmap_manager_;
        scoped_ptr<ResourceProvider> resource_provider_;
        scoped_ptr<SurfaceAggregator> aggregator_;
    };

    class ResourceTrackingSurfaceFactoryClient : public SurfaceFactoryClient {
    public:
        ResourceTrackingSurfaceFactoryClient() { }
        ~ResourceTrackingSurfaceFactoryClient() override { }

        void ReturnResources(const ReturnedResourceArray& resources) override
        {
            returned_resources_ = resources;
        }

        ReturnedResourceArray returned_resources() const
        {
            return returned_resources_;
        }

    private:
        ReturnedResourceArray returned_resources_;

        DISALLOW_COPY_AND_ASSIGN(ResourceTrackingSurfaceFactoryClient);
    };

    void SubmitCompositorFrameWithResources(ResourceId* resource_ids,
        size_t num_resource_ids,
        bool valid,
        SurfaceId child_id,
        SurfaceFactory* factory,
        SurfaceId surface_id)
    {
        scoped_ptr<DelegatedFrameData> frame_data(new DelegatedFrameData);
        scoped_ptr<RenderPass> pass = RenderPass::Create();
        pass->id = RenderPassId(1, 1);
        SharedQuadState* sqs = pass->CreateAndAppendSharedQuadState();
        sqs->opacity = 1.f;
        if (!child_id.is_null()) {
            SurfaceDrawQuad* surface_quad = pass->CreateAndAppendDrawQuad<SurfaceDrawQuad>();
            surface_quad->SetNew(sqs, gfx::Rect(0, 0, 1, 1), gfx::Rect(0, 0, 1, 1),
                child_id);
        }

        for (size_t i = 0u; i < num_resource_ids; ++i) {
            TransferableResource resource;
            resource.id = resource_ids[i];
            // ResourceProvider is software, so only software resources are valid.
            resource.is_software = valid;
            frame_data->resource_list.push_back(resource);
            TextureDrawQuad* quad = pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
            const gfx::Rect rect;
            const gfx::Rect opaque_rect;
            const gfx::Rect visible_rect;
            bool needs_blending = false;
            bool premultiplied_alpha = false;
            const gfx::PointF uv_top_left;
            const gfx::PointF uv_bottom_right;
            SkColor background_color = SK_ColorGREEN;
            const float vertex_opacity[4] = { 0.f, 0.f, 1.f, 1.f };
            bool flipped = false;
            bool nearest_neighbor = false;
            quad->SetAll(sqs, rect, opaque_rect, visible_rect, needs_blending,
                resource_ids[i], gfx::Size(), false, premultiplied_alpha,
                uv_top_left, uv_bottom_right, background_color, vertex_opacity,
                flipped, nearest_neighbor);
        }
        frame_data->render_pass_list.push_back(pass.Pass());
        scoped_ptr<CompositorFrame> frame(new CompositorFrame);
        frame->delegated_frame_data = frame_data.Pass();
        factory->SubmitCompositorFrame(surface_id, frame.Pass(),
            SurfaceFactory::DrawCallback());
    }

    TEST_F(SurfaceAggregatorWithResourcesTest, TakeResourcesOneSurface)
    {
        ResourceTrackingSurfaceFactoryClient client;
        SurfaceFactory factory(&manager_, &client);
        SurfaceId surface_id(7u);
        factory.Create(surface_id);

        ResourceId ids[] = { 11, 12, 13 };
        SubmitCompositorFrameWithResources(ids, arraysize(ids), true, SurfaceId(),
            &factory, surface_id);

        scoped_ptr<CompositorFrame> frame = aggregator_->Aggregate(surface_id);

        // Nothing should be available to be returned yet.
        EXPECT_TRUE(client.returned_resources().empty());

        SubmitCompositorFrameWithResources(NULL, 0u, true, SurfaceId(), &factory,
            surface_id);

        frame = aggregator_->Aggregate(surface_id);

        ASSERT_EQ(3u, client.returned_resources().size());
        ResourceId returned_ids[3];
        for (size_t i = 0; i < 3; ++i) {
            returned_ids[i] = client.returned_resources()[i].id;
        }
        EXPECT_THAT(returned_ids,
            testing::WhenSorted(testing::ElementsAreArray(ids)));
        factory.Destroy(surface_id);
    }

    TEST_F(SurfaceAggregatorWithResourcesTest, TakeInvalidResources)
    {
        ResourceTrackingSurfaceFactoryClient client;
        SurfaceFactory factory(&manager_, &client);
        SurfaceId surface_id(7u);
        factory.Create(surface_id);

        scoped_ptr<DelegatedFrameData> frame_data(new DelegatedFrameData);
        scoped_ptr<RenderPass> pass = RenderPass::Create();
        pass->id = RenderPassId(1, 1);
        TransferableResource resource;
        resource.id = 11;
        // ResourceProvider is software but resource is not, so it should be
        // ignored.
        resource.is_software = false;
        frame_data->resource_list.push_back(resource);
        frame_data->render_pass_list.push_back(pass.Pass());
        scoped_ptr<CompositorFrame> frame(new CompositorFrame);
        frame->delegated_frame_data = frame_data.Pass();
        factory.SubmitCompositorFrame(surface_id, frame.Pass(),
            SurfaceFactory::DrawCallback());

        scoped_ptr<CompositorFrame> returned_frame = aggregator_->Aggregate(surface_id);

        // Nothing should be available to be returned yet.
        EXPECT_TRUE(client.returned_resources().empty());

        SubmitCompositorFrameWithResources(NULL, 0, true, SurfaceId(), &factory,
            surface_id);
        ASSERT_EQ(1u, client.returned_resources().size());
        EXPECT_EQ(11u, client.returned_resources()[0].id);

        factory.Destroy(surface_id);
    }

    TEST_F(SurfaceAggregatorWithResourcesTest, TwoSurfaces)
    {
        ResourceTrackingSurfaceFactoryClient client;
        SurfaceFactory factory(&manager_, &client);
        SurfaceId surface_id(7u);
        factory.Create(surface_id);
        SurfaceId surface_id2(8u);
        factory.Create(surface_id2);

        ResourceId ids[] = { 11, 12, 13 };
        SubmitCompositorFrameWithResources(ids, arraysize(ids), true, SurfaceId(),
            &factory, surface_id);
        ResourceId ids2[] = { 14, 15, 16 };
        SubmitCompositorFrameWithResources(ids2, arraysize(ids2), true, SurfaceId(),
            &factory, surface_id2);

        scoped_ptr<CompositorFrame> frame = aggregator_->Aggregate(surface_id);

        SubmitCompositorFrameWithResources(NULL, 0, true, SurfaceId(), &factory,
            surface_id);

        // Nothing should be available to be returned yet.
        EXPECT_TRUE(client.returned_resources().empty());

        frame = aggregator_->Aggregate(surface_id2);

        // surface_id wasn't referenced, so its resources should be returned.
        ASSERT_EQ(3u, client.returned_resources().size());
        ResourceId returned_ids[3];
        for (size_t i = 0; i < 3; ++i) {
            returned_ids[i] = client.returned_resources()[i].id;
        }
        EXPECT_THAT(returned_ids,
            testing::WhenSorted(testing::ElementsAreArray(ids)));
        EXPECT_EQ(3u, resource_provider_->num_resources());
        factory.Destroy(surface_id);
        factory.Destroy(surface_id2);
    }

    // Ensure that aggregator completely ignores Surfaces that reference invalid
    // resources.
    TEST_F(SurfaceAggregatorWithResourcesTest, InvalidChildSurface)
    {
        ResourceTrackingSurfaceFactoryClient client;
        SurfaceFactory factory(&manager_, &client);
        SurfaceId root_surface_id(7u);
        factory.Create(root_surface_id);
        SurfaceId middle_surface_id(8u);
        factory.Create(middle_surface_id);
        SurfaceId child_surface_id(9u);
        factory.Create(child_surface_id);

        ResourceId ids[] = { 14, 15, 16 };
        SubmitCompositorFrameWithResources(ids, arraysize(ids), true, SurfaceId(),
            &factory, child_surface_id);

        ResourceId ids2[] = { 17, 18, 19 };
        SubmitCompositorFrameWithResources(ids2, arraysize(ids2), false,
            child_surface_id, &factory,
            middle_surface_id);

        ResourceId ids3[] = { 20, 21, 22 };
        SubmitCompositorFrameWithResources(ids3, arraysize(ids3), true,
            middle_surface_id, &factory,
            root_surface_id);

        scoped_ptr<CompositorFrame> frame;
        frame = aggregator_->Aggregate(root_surface_id);

        RenderPassList* pass_list = &frame->delegated_frame_data->render_pass_list;
        ASSERT_EQ(1u, pass_list->size());
        EXPECT_EQ(1u, pass_list->back()->shared_quad_state_list.size());
        EXPECT_EQ(3u, pass_list->back()->quad_list.size());

        SubmitCompositorFrameWithResources(ids2, arraysize(ids), true,
            child_surface_id, &factory,
            middle_surface_id);

        frame = aggregator_->Aggregate(root_surface_id);

        pass_list = &frame->delegated_frame_data->render_pass_list;
        ASSERT_EQ(1u, pass_list->size());
        EXPECT_EQ(3u, pass_list->back()->shared_quad_state_list.size());
        EXPECT_EQ(9u, pass_list->back()->quad_list.size());

        factory.Destroy(root_surface_id);
        factory.Destroy(child_surface_id);
        factory.Destroy(middle_surface_id);
    }

} // namespace
} // namespace cc
