// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/bind.h"
#include "cc/output/compositor_frame.h"
#include "cc/output/delegated_frame_data.h"
#include "cc/resources/resource_provider.h"
#include "cc/surfaces/surface.h"
#include "cc/surfaces/surface_factory.h"
#include "cc/surfaces/surface_factory_client.h"
#include "cc/surfaces/surface_manager.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/size.h"

namespace cc {
namespace {

    class TestSurfaceFactoryClient : public SurfaceFactoryClient {
    public:
        TestSurfaceFactoryClient() { }
        ~TestSurfaceFactoryClient() override { }

        void ReturnResources(const ReturnedResourceArray& resources) override
        {
            returned_resources_.insert(
                returned_resources_.end(), resources.begin(), resources.end());
        }

        const ReturnedResourceArray& returned_resources() const
        {
            return returned_resources_;
        }

        void clear_returned_resources() { returned_resources_.clear(); }

    private:
        ReturnedResourceArray returned_resources_;

        DISALLOW_COPY_AND_ASSIGN(TestSurfaceFactoryClient);
    };

    class SurfaceFactoryTest : public testing::Test {
    public:
        SurfaceFactoryTest()
            : factory_(&manager_, &client_)
            , surface_id_(3)
        {
            factory_.Create(surface_id_);
        }

        ~SurfaceFactoryTest() override
        {
            if (!surface_id_.is_null())
                factory_.Destroy(surface_id_);
        }

        void SubmitCompositorFrameWithResources(ResourceId* resource_ids,
            size_t num_resource_ids)
        {
            scoped_ptr<DelegatedFrameData> frame_data(new DelegatedFrameData);
            for (size_t i = 0u; i < num_resource_ids; ++i) {
                TransferableResource resource;
                resource.id = resource_ids[i];
                resource.mailbox_holder.texture_target = GL_TEXTURE_2D;
                frame_data->resource_list.push_back(resource);
            }
            scoped_ptr<CompositorFrame> frame(new CompositorFrame);
            frame->delegated_frame_data = frame_data.Pass();
            factory_.SubmitCompositorFrame(surface_id_, frame.Pass(),
                SurfaceFactory::DrawCallback());
        }

        void UnrefResources(ResourceId* ids_to_unref,
            int* counts_to_unref,
            size_t num_ids_to_unref)
        {
            ReturnedResourceArray unref_array;
            for (size_t i = 0; i < num_ids_to_unref; ++i) {
                ReturnedResource resource;
                resource.id = ids_to_unref[i];
                resource.count = counts_to_unref[i];
                unref_array.push_back(resource);
            }
            factory_.UnrefResources(unref_array);
        }

        void CheckReturnedResourcesMatchExpected(ResourceId* expected_returned_ids,
            int* expected_returned_counts,
            size_t expected_resources)
        {
            const ReturnedResourceArray& actual_resources = client_.returned_resources();
            ASSERT_EQ(expected_resources, actual_resources.size());
            for (size_t i = 0; i < expected_resources; ++i) {
                ReturnedResource resource = actual_resources[i];
                EXPECT_EQ(expected_returned_ids[i], resource.id);
                EXPECT_EQ(expected_returned_counts[i], resource.count);
            }
            client_.clear_returned_resources();
        }

        void RefCurrentFrameResources()
        {
            Surface* surface = manager_.GetSurfaceForId(surface_id_);
            factory_.RefResources(
                surface->GetEligibleFrame()->delegated_frame_data->resource_list);
        }

    protected:
        SurfaceManager manager_;
        TestSurfaceFactoryClient client_;
        SurfaceFactory factory_;
        SurfaceId surface_id_;
    };

    // Tests submitting a frame with resources followed by one with no resources
    // with no resource provider action in between.
    TEST_F(SurfaceFactoryTest, ResourceLifetimeSimple)
    {
        ResourceId first_frame_ids[] = { 1, 2, 3 };
        SubmitCompositorFrameWithResources(first_frame_ids,
            arraysize(first_frame_ids));

        // All of the resources submitted in the first frame are still in use at this
        // time by virtue of being in the pending frame, so none can be returned to
        // the client yet.
        EXPECT_EQ(0u, client_.returned_resources().size());
        client_.clear_returned_resources();

        // The second frame references no resources and thus should make all resources
        // available to be returned.
        SubmitCompositorFrameWithResources(NULL, 0);

        ResourceId expected_returned_ids[] = { 1, 2, 3 };
        int expected_returned_counts[] = { 1, 1, 1 };
        CheckReturnedResourcesMatchExpected(expected_returned_ids,
            expected_returned_counts,
            arraysize(expected_returned_counts));
    }

    // Tests submitting a frame with resources followed by one with no resources
    // with the resource provider holding everything alive.
    TEST_F(SurfaceFactoryTest, ResourceLifetimeSimpleWithProviderHoldingAlive)
    {
        ResourceId first_frame_ids[] = { 1, 2, 3 };
        SubmitCompositorFrameWithResources(first_frame_ids,
            arraysize(first_frame_ids));

        // All of the resources submitted in the first frame are still in use at this
        // time by virtue of being in the pending frame, so none can be returned to
        // the client yet.
        EXPECT_EQ(0u, client_.returned_resources().size());
        client_.clear_returned_resources();

        // Hold on to everything.
        RefCurrentFrameResources();

        // The second frame references no resources and thus should make all resources
        // available to be returned as soon as the resource provider releases them.
        SubmitCompositorFrameWithResources(NULL, 0);

        EXPECT_EQ(0u, client_.returned_resources().size());
        client_.clear_returned_resources();

        int release_counts[] = { 1, 1, 1 };
        UnrefResources(first_frame_ids, release_counts, arraysize(first_frame_ids));

        ResourceId expected_returned_ids[] = { 1, 2, 3 };
        int expected_returned_counts[] = { 1, 1, 1 };
        CheckReturnedResourcesMatchExpected(expected_returned_ids,
            expected_returned_counts,
            arraysize(expected_returned_counts));
    }

    // Tests referencing a resource, unref'ing it to zero, then using it again
    // before returning it to the client.
    TEST_F(SurfaceFactoryTest, ResourceReusedBeforeReturn)
    {
        ResourceId first_frame_ids[] = { 7 };
        SubmitCompositorFrameWithResources(first_frame_ids,
            arraysize(first_frame_ids));

        // This removes all references to resource id 7.
        SubmitCompositorFrameWithResources(NULL, 0);

        // This references id 7 again.
        SubmitCompositorFrameWithResources(first_frame_ids,
            arraysize(first_frame_ids));

        // This removes it again.
        SubmitCompositorFrameWithResources(NULL, 0);

        // Now it should be returned.
        // We don't care how many entries are in the returned array for 7, so long as
        // the total returned count matches the submitted count.
        const ReturnedResourceArray& returned = client_.returned_resources();
        size_t return_count = 0;
        for (size_t i = 0; i < returned.size(); ++i) {
            EXPECT_EQ(7u, returned[i].id);
            return_count += returned[i].count;
        }
        EXPECT_EQ(2u, return_count);
    }

    // Tests having resources referenced multiple times, as if referenced by
    // multiple providers.
    TEST_F(SurfaceFactoryTest, ResourceRefMultipleTimes)
    {
        ResourceId first_frame_ids[] = { 3, 4 };
        SubmitCompositorFrameWithResources(first_frame_ids,
            arraysize(first_frame_ids));

        // Ref resources from the first frame twice.
        RefCurrentFrameResources();
        RefCurrentFrameResources();

        ResourceId second_frame_ids[] = { 4, 5 };
        SubmitCompositorFrameWithResources(second_frame_ids,
            arraysize(second_frame_ids));

        // Ref resources from the second frame 3 times.
        RefCurrentFrameResources();
        RefCurrentFrameResources();
        RefCurrentFrameResources();

        // Submit a frame with no resources to remove all current frame refs from
        // submitted resources.
        SubmitCompositorFrameWithResources(NULL, 0);

        EXPECT_EQ(0u, client_.returned_resources().size());
        client_.clear_returned_resources();

        // Expected current refs:
        //  3 -> 2
        //  4 -> 2 + 3 = 5
        //  5 -> 3
        {
            SCOPED_TRACE("unref all 3");
            ResourceId ids_to_unref[] = { 3, 4, 5 };
            int counts[] = { 1, 1, 1 };
            UnrefResources(ids_to_unref, counts, arraysize(ids_to_unref));

            EXPECT_EQ(0u, client_.returned_resources().size());
            client_.clear_returned_resources();

            UnrefResources(ids_to_unref, counts, arraysize(ids_to_unref));

            ResourceId expected_returned_ids[] = { 3 };
            int expected_returned_counts[] = { 1 };
            CheckReturnedResourcesMatchExpected(expected_returned_ids,
                expected_returned_counts,
                arraysize(expected_returned_counts));
        }

        // Expected refs remaining:
        //  4 -> 3
        //  5 -> 1
        {
            SCOPED_TRACE("unref 4 and 5");
            ResourceId ids_to_unref[] = { 4, 5 };
            int counts[] = { 1, 1 };
            UnrefResources(ids_to_unref, counts, arraysize(ids_to_unref));

            ResourceId expected_returned_ids[] = { 5 };
            int expected_returned_counts[] = { 1 };
            CheckReturnedResourcesMatchExpected(expected_returned_ids,
                expected_returned_counts,
                arraysize(expected_returned_counts));
        }

        // Now, just 2 refs remaining on resource 4. Unref both at once and make sure
        // the returned count is correct.
        {
            SCOPED_TRACE("unref only 4");
            ResourceId ids_to_unref[] = { 4 };
            int counts[] = { 2 };
            UnrefResources(ids_to_unref, counts, arraysize(ids_to_unref));

            ResourceId expected_returned_ids[] = { 4 };
            int expected_returned_counts[] = { 2 };
            CheckReturnedResourcesMatchExpected(expected_returned_ids,
                expected_returned_counts,
                arraysize(expected_returned_counts));
        }
    }

    TEST_F(SurfaceFactoryTest, ResourceLifetime)
    {
        ResourceId first_frame_ids[] = { 1, 2, 3 };
        SubmitCompositorFrameWithResources(first_frame_ids,
            arraysize(first_frame_ids));

        // All of the resources submitted in the first frame are still in use at this
        // time by virtue of being in the pending frame, so none can be returned to
        // the client yet.
        EXPECT_EQ(0u, client_.returned_resources().size());
        client_.clear_returned_resources();

        // The second frame references some of the same resources, but some different
        // ones. We expect to receive back resource 1 with a count of 1 since it was
        // only referenced by the first frame.
        ResourceId second_frame_ids[] = { 2, 3, 4 };
        SubmitCompositorFrameWithResources(second_frame_ids,
            arraysize(second_frame_ids));

        {
            SCOPED_TRACE("second frame");
            ResourceId expected_returned_ids[] = { 1 };
            int expected_returned_counts[] = { 1 };
            CheckReturnedResourcesMatchExpected(expected_returned_ids,
                expected_returned_counts,
                arraysize(expected_returned_counts));
        }

        // The third frame references a disjoint set of resources, so we expect to
        // receive back all resources from the first and second frames. Resource IDs 2
        // and 3 will have counts of 2, since they were used in both frames, and
        // resource ID 4 will have a count of 1.
        ResourceId third_frame_ids[] = { 10, 11, 12, 13 };
        SubmitCompositorFrameWithResources(third_frame_ids,
            arraysize(third_frame_ids));

        {
            SCOPED_TRACE("third frame");
            ResourceId expected_returned_ids[] = { 2, 3, 4 };
            int expected_returned_counts[] = { 2, 2, 1 };
            CheckReturnedResourcesMatchExpected(expected_returned_ids,
                expected_returned_counts,
                arraysize(expected_returned_counts));
        }

        // Simulate a ResourceProvider taking a ref on all of the resources.
        RefCurrentFrameResources();

        ResourceId fourth_frame_ids[] = { 12, 13 };
        SubmitCompositorFrameWithResources(fourth_frame_ids,
            arraysize(fourth_frame_ids));

        EXPECT_EQ(0u, client_.returned_resources().size());

        RefCurrentFrameResources();

        // All resources are still being used by the external reference, so none can
        // be returned to the client.
        EXPECT_EQ(0u, client_.returned_resources().size());

        // Release resources associated with the first RefCurrentFrameResources() call
        // first.
        {
            ResourceId ids_to_unref[] = { 10, 11, 12, 13 };
            int counts[] = { 1, 1, 1, 1 };
            UnrefResources(ids_to_unref, counts, arraysize(ids_to_unref));
        }

        {
            SCOPED_TRACE("fourth frame, first unref");
            ResourceId expected_returned_ids[] = { 10, 11 };
            int expected_returned_counts[] = { 1, 1 };
            CheckReturnedResourcesMatchExpected(expected_returned_ids,
                expected_returned_counts,
                arraysize(expected_returned_counts));
        }

        {
            ResourceId ids_to_unref[] = { 12, 13 };
            int counts[] = { 1, 1 };
            UnrefResources(ids_to_unref, counts, arraysize(ids_to_unref));
        }

        // Resources 12 and 13 are still in use by the current frame, so they
        // shouldn't be available to be returned.
        EXPECT_EQ(0u, client_.returned_resources().size());

        // If we submit an empty frame, however, they should become available.
        SubmitCompositorFrameWithResources(NULL, 0u);

        {
            SCOPED_TRACE("fourth frame, second unref");
            ResourceId expected_returned_ids[] = { 12, 13 };
            int expected_returned_counts[] = { 2, 2 };
            CheckReturnedResourcesMatchExpected(expected_returned_ids,
                expected_returned_counts,
                arraysize(expected_returned_counts));
        }
    }

    TEST_F(SurfaceFactoryTest, BlankNoIndexIncrement)
    {
        SurfaceId surface_id(6);
        factory_.Create(surface_id);
        Surface* surface = manager_.GetSurfaceForId(surface_id);
        ASSERT_NE(nullptr, surface);
        EXPECT_EQ(2, surface->frame_index());
        scoped_ptr<CompositorFrame> frame(new CompositorFrame);
        frame->delegated_frame_data.reset(new DelegatedFrameData);

        factory_.SubmitCompositorFrame(surface_id, frame.Pass(),
            SurfaceFactory::DrawCallback());
        EXPECT_EQ(2, surface->frame_index());
        factory_.Destroy(surface_id);
    }

    void DrawCallback(uint32* execute_count,
        SurfaceDrawStatus* result,
        SurfaceDrawStatus drawn)
    {
        *execute_count += 1;
        *result = drawn;
    }

    // Tests doing a DestroyAll before shutting down the factory;
    TEST_F(SurfaceFactoryTest, DestroyAll)
    {
        SurfaceId id(7);
        factory_.Create(id);

        scoped_ptr<DelegatedFrameData> frame_data(new DelegatedFrameData);
        TransferableResource resource;
        resource.id = 1;
        resource.mailbox_holder.texture_target = GL_TEXTURE_2D;
        frame_data->resource_list.push_back(resource);
        scoped_ptr<CompositorFrame> frame(new CompositorFrame);
        frame->delegated_frame_data = frame_data.Pass();
        uint32 execute_count = 0;
        SurfaceDrawStatus drawn = SurfaceDrawStatus::DRAW_SKIPPED;

        factory_.SubmitCompositorFrame(
            id, frame.Pass(), base::Bind(&DrawCallback, &execute_count, &drawn));

        surface_id_ = SurfaceId();
        factory_.DestroyAll();
        EXPECT_EQ(1u, execute_count);
        EXPECT_EQ(SurfaceDrawStatus::DRAW_SKIPPED, drawn);
    }

    TEST_F(SurfaceFactoryTest, DestroySequence)
    {
        SurfaceId id2(5);
        factory_.Create(id2);

        manager_.RegisterSurfaceIdNamespace(0);

        // Check that waiting before the sequence is satisfied works.
        manager_.GetSurfaceForId(id2)
            ->AddDestructionDependency(SurfaceSequence(0, 4));
        factory_.Destroy(id2);

        scoped_ptr<DelegatedFrameData> frame_data(new DelegatedFrameData);
        scoped_ptr<CompositorFrame> frame(new CompositorFrame);
        frame->metadata.satisfies_sequences.push_back(6);
        frame->metadata.satisfies_sequences.push_back(4);
        frame->delegated_frame_data = frame_data.Pass();
        DCHECK(manager_.GetSurfaceForId(id2));
        factory_.SubmitCompositorFrame(surface_id_, frame.Pass(),
            SurfaceFactory::DrawCallback());
        DCHECK(!manager_.GetSurfaceForId(id2));

        // Check that waiting after the sequence is satisfied works.
        factory_.Create(id2);
        DCHECK(manager_.GetSurfaceForId(id2));
        manager_.GetSurfaceForId(id2)
            ->AddDestructionDependency(SurfaceSequence(0, 6));
        factory_.Destroy(id2);
        DCHECK(!manager_.GetSurfaceForId(id2));
    }

    // Tests that Surface ID namespace invalidation correctly allows
    // Sequences to be ignored.
    TEST_F(SurfaceFactoryTest, InvalidIdNamespace)
    {
        uint32_t id_namespace = 9u;
        SurfaceId id(5);
        factory_.Create(id);

        manager_.RegisterSurfaceIdNamespace(id_namespace);
        manager_.GetSurfaceForId(id)
            ->AddDestructionDependency(SurfaceSequence(id_namespace, 4));
        factory_.Destroy(id);

        // Verify the dependency has prevented the surface from getting destroyed.
        EXPECT_TRUE(manager_.GetSurfaceForId(id));

        manager_.InvalidateSurfaceIdNamespace(id_namespace);

        // Verify that the invalidated namespace caused the unsatisfied sequence
        // to be ignored.
        EXPECT_FALSE(manager_.GetSurfaceForId(id));
    }

    TEST_F(SurfaceFactoryTest, DestroyCycle)
    {
        SurfaceId id2(5);
        factory_.Create(id2);

        manager_.RegisterSurfaceIdNamespace(0);

        manager_.GetSurfaceForId(id2)
            ->AddDestructionDependency(SurfaceSequence(0, 4));

        // Give id2 a frame that references surface_id_.
        {
            scoped_ptr<RenderPass> render_pass(RenderPass::Create());
            render_pass->referenced_surfaces.push_back(surface_id_);
            scoped_ptr<DelegatedFrameData> frame_data(new DelegatedFrameData);
            frame_data->render_pass_list.push_back(render_pass.Pass());
            scoped_ptr<CompositorFrame> frame(new CompositorFrame);
            frame->delegated_frame_data = frame_data.Pass();
            factory_.SubmitCompositorFrame(id2, frame.Pass(),
                SurfaceFactory::DrawCallback());
        }
        factory_.Destroy(id2);

        // Give surface_id_ a frame that references id2.
        {
            scoped_ptr<RenderPass> render_pass(RenderPass::Create());
            render_pass->referenced_surfaces.push_back(id2);
            scoped_ptr<DelegatedFrameData> frame_data(new DelegatedFrameData);
            frame_data->render_pass_list.push_back(render_pass.Pass());
            scoped_ptr<CompositorFrame> frame(new CompositorFrame);
            frame->delegated_frame_data = frame_data.Pass();
            factory_.SubmitCompositorFrame(surface_id_, frame.Pass(),
                SurfaceFactory::DrawCallback());
        }
        factory_.Destroy(surface_id_);
        EXPECT_TRUE(manager_.GetSurfaceForId(id2));
        // surface_id_ should be retained by reference from id2.
        EXPECT_TRUE(manager_.GetSurfaceForId(surface_id_));

        // Satisfy last destruction dependency for id2.
        std::vector<uint32_t> to_satisfy;
        to_satisfy.push_back(4);
        manager_.DidSatisfySequences(0, &to_satisfy);

        // id2 and surface_id_ are in a reference cycle that has no surface
        // sequences holding on to it, so they should be destroyed.
        EXPECT_TRUE(!manager_.GetSurfaceForId(id2));
        EXPECT_TRUE(!manager_.GetSurfaceForId(surface_id_));

        surface_id_ = SurfaceId();
    }

} // namespace
} // namespace cc
