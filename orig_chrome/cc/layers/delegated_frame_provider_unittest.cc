// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/delegated_frame_provider.h"
#include "cc/layers/delegated_frame_resource_collection.h"
#include "cc/layers/delegated_renderer_layer.h"
#include "cc/output/delegated_frame_data.h"
#include "cc/quads/texture_draw_quad.h"
#include "cc/resources/resource_provider.h"
#include "cc/resources/returned_resource.h"
#include "cc/resources/transferable_resource.h"
#include "cc/trees/layer_tree_settings.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    class DelegatedFrameProviderTest
        : public testing::Test,
          public DelegatedFrameResourceCollectionClient {
    protected:
        DelegatedFrameProviderTest()
            : resources_available_(false)
        {
        }

        scoped_ptr<DelegatedFrameData> CreateFrameData(
            const gfx::Rect& root_output_rect,
            const gfx::Rect& root_damage_rect)
        {
            scoped_ptr<DelegatedFrameData> frame(new DelegatedFrameData);

            scoped_ptr<RenderPass> root_pass(RenderPass::Create());
            root_pass->SetNew(RenderPassId(1, 1),
                root_output_rect,
                root_damage_rect,
                gfx::Transform());
            frame->render_pass_list.push_back(root_pass.Pass());
            return frame.Pass();
        }

        void AddTransferableResource(DelegatedFrameData* frame,
            ResourceId resource_id)
        {
            TransferableResource resource;
            resource.id = resource_id;
            resource.mailbox_holder.texture_target = GL_TEXTURE_2D;
            frame->resource_list.push_back(resource);
        }

        void AddTextureQuad(DelegatedFrameData* frame, ResourceId resource_id)
        {
            SharedQuadState* sqs = frame->render_pass_list[0]->CreateAndAppendSharedQuadState();
            TextureDrawQuad* quad = frame->render_pass_list[0]->CreateAndAppendDrawQuad<TextureDrawQuad>();
            float vertex_opacity[4] = { 1.f, 1.f, 1.f, 1.f };
            quad->SetNew(sqs,
                gfx::Rect(0, 0, 10, 10),
                gfx::Rect(0, 0, 10, 10),
                gfx::Rect(0, 0, 10, 10),
                resource_id,
                false,
                gfx::PointF(0.f, 0.f),
                gfx::PointF(1.f, 1.f),
                SK_ColorTRANSPARENT,
                vertex_opacity,
                false,
                false);
        }

        void SetUp() override
        {
            resource_collection_ = new DelegatedFrameResourceCollection;
            resource_collection_->SetClient(this);
        }

        void TearDown() override { resource_collection_->SetClient(nullptr); }

        void UnusedResourcesAreAvailable() override
        {
            resources_available_ = true;
            resource_collection_->TakeUnusedResourcesForChildCompositor(&resources_);
        }

        bool ReturnAndResetResourcesAvailable()
        {
            bool r = resources_available_;
            resources_available_ = false;
            return r;
        }

        void SetFrameProvider(scoped_ptr<DelegatedFrameData> frame_data)
        {
            frame_provider_ = new DelegatedFrameProvider(resource_collection_, frame_data.Pass());
        }

        scoped_refptr<DelegatedFrameResourceCollection> resource_collection_;
        scoped_refptr<DelegatedFrameProvider> frame_provider_;
        bool resources_available_;
        ReturnedResourceArray resources_;
        LayerSettings layer_settings_;
    };

    TEST_F(DelegatedFrameProviderTest, SameResources)
    {
        scoped_ptr<DelegatedFrameData> frame = CreateFrameData(gfx::Rect(1, 1), gfx::Rect(1, 1));
        AddTextureQuad(frame.get(), 444);
        AddTransferableResource(frame.get(), 444);
        SetFrameProvider(frame.Pass());

        frame = CreateFrameData(gfx::Rect(1, 1), gfx::Rect(1, 1));
        AddTextureQuad(frame.get(), 444);
        AddTransferableResource(frame.get(), 444);
        SetFrameProvider(frame.Pass());

        EXPECT_FALSE(ReturnAndResetResourcesAvailable());
        EXPECT_EQ(0u, resources_.size());

        frame_provider_ = nullptr;

        EXPECT_TRUE(ReturnAndResetResourcesAvailable());
        EXPECT_EQ(1u, resources_.size());
        EXPECT_EQ(444u, resources_[0].id);
    }

    TEST_F(DelegatedFrameProviderTest, ReplaceResources)
    {
        scoped_ptr<DelegatedFrameData> frame = CreateFrameData(gfx::Rect(1, 1), gfx::Rect(1, 1));
        AddTextureQuad(frame.get(), 444);
        AddTransferableResource(frame.get(), 444);
        SetFrameProvider(frame.Pass());

        EXPECT_FALSE(ReturnAndResetResourcesAvailable());

        frame = CreateFrameData(gfx::Rect(1, 1), gfx::Rect(1, 1));
        AddTextureQuad(frame.get(), 555);
        AddTransferableResource(frame.get(), 555);
        SetFrameProvider(frame.Pass());

        EXPECT_TRUE(ReturnAndResetResourcesAvailable());
        EXPECT_EQ(1u, resources_.size());
        EXPECT_EQ(444u, resources_[0].id);
        resources_.clear();

        frame_provider_ = nullptr;

        EXPECT_TRUE(ReturnAndResetResourcesAvailable());
        EXPECT_EQ(1u, resources_.size());
        EXPECT_EQ(555u, resources_[0].id);
    }

    TEST_F(DelegatedFrameProviderTest, RefResources)
    {
        scoped_ptr<DelegatedFrameData> frame = CreateFrameData(gfx::Rect(5, 5), gfx::Rect(2, 2));
        AddTextureQuad(frame.get(), 444);
        AddTransferableResource(frame.get(), 444);

        TransferableResourceArray reffed = frame->resource_list;
        ReturnedResourceArray returned;
        TransferableResource::ReturnResources(reffed, &returned);

        SetFrameProvider(frame.Pass());

        scoped_refptr<DelegatedRendererLayer> observer1 = DelegatedRendererLayer::Create(layer_settings_, frame_provider_);
        scoped_refptr<DelegatedRendererLayer> observer2 = DelegatedRendererLayer::Create(layer_settings_, frame_provider_);

        gfx::Rect damage;

        // Both observers get a full frame of damage on the first request.
        frame_provider_->GetFrameDataAndRefResources(observer1.get(), &damage);
        EXPECT_EQ(gfx::Rect(5, 5), damage);
        frame_provider_->GetFrameDataAndRefResources(observer2.get(), &damage);
        EXPECT_EQ(gfx::Rect(5, 5), damage);

        // And both get no damage on the 2nd request. This adds a second ref to the
        // resources.
        frame_provider_->GetFrameDataAndRefResources(observer1.get(), &damage);
        EXPECT_EQ(gfx::Rect(), damage);
        frame_provider_->GetFrameDataAndRefResources(observer2.get(), &damage);
        EXPECT_EQ(gfx::Rect(), damage);

        EXPECT_FALSE(ReturnAndResetResourcesAvailable());

        frame = CreateFrameData(gfx::Rect(5, 5), gfx::Rect(2, 2));
        AddTextureQuad(frame.get(), 555);
        AddTransferableResource(frame.get(), 555);
        frame_provider_->SetFrameData(frame.Pass());

        // The resources from the first frame are still reffed by the observers.
        EXPECT_FALSE(ReturnAndResetResourcesAvailable());

        // There are 4 refs taken.
        frame_provider_->UnrefResourcesOnMainThread(returned);
        EXPECT_FALSE(ReturnAndResetResourcesAvailable());
        frame_provider_->UnrefResourcesOnMainThread(returned);
        EXPECT_FALSE(ReturnAndResetResourcesAvailable());
        frame_provider_->UnrefResourcesOnMainThread(returned);
        EXPECT_FALSE(ReturnAndResetResourcesAvailable());

        // The 4th unref will release them.
        frame_provider_->UnrefResourcesOnMainThread(returned);

        EXPECT_TRUE(ReturnAndResetResourcesAvailable());
        EXPECT_EQ(1u, resources_.size());
        EXPECT_EQ(444u, resources_[0].id);
    }

    TEST_F(DelegatedFrameProviderTest, RefResourcesInFrameProvider)
    {
        scoped_ptr<DelegatedFrameData> frame = CreateFrameData(gfx::Rect(5, 5), gfx::Rect(2, 2));
        AddTextureQuad(frame.get(), 444);
        AddTransferableResource(frame.get(), 444);

        TransferableResourceArray reffed = frame->resource_list;
        ReturnedResourceArray returned;
        TransferableResource::ReturnResources(reffed, &returned);

        SetFrameProvider(frame.Pass());

        scoped_refptr<DelegatedRendererLayer> observer1 = DelegatedRendererLayer::Create(layer_settings_, frame_provider_);
        scoped_refptr<DelegatedRendererLayer> observer2 = DelegatedRendererLayer::Create(layer_settings_, frame_provider_);

        gfx::Rect damage;

        // Take a ref on each observer.
        frame_provider_->GetFrameDataAndRefResources(observer1.get(), &damage);
        frame_provider_->GetFrameDataAndRefResources(observer2.get(), &damage);

        EXPECT_FALSE(ReturnAndResetResourcesAvailable());

        // Release both refs. But there's still a ref held in the frame
        // provider itself.
        frame_provider_->UnrefResourcesOnMainThread(returned);
        frame_provider_->UnrefResourcesOnMainThread(returned);
        EXPECT_FALSE(ReturnAndResetResourcesAvailable());

        // Setting a new frame will release it.
        frame = CreateFrameData(gfx::Rect(5, 5), gfx::Rect(2, 2));
        AddTextureQuad(frame.get(), 555);
        AddTransferableResource(frame.get(), 555);
        frame_provider_->SetFrameData(frame.Pass());

        EXPECT_TRUE(ReturnAndResetResourcesAvailable());
        EXPECT_EQ(1u, resources_.size());
        EXPECT_EQ(444u, resources_[0].id);
    }

    TEST_F(DelegatedFrameProviderTest, RefResourcesInFrameProviderUntilDestroy)
    {
        scoped_ptr<DelegatedFrameData> frame = CreateFrameData(gfx::Rect(5, 5), gfx::Rect(2, 2));
        AddTextureQuad(frame.get(), 444);
        AddTransferableResource(frame.get(), 444);

        TransferableResourceArray reffed = frame->resource_list;
        ReturnedResourceArray returned;
        TransferableResource::ReturnResources(reffed, &returned);

        SetFrameProvider(frame.Pass());

        scoped_refptr<DelegatedRendererLayer> observer1 = DelegatedRendererLayer::Create(layer_settings_, frame_provider_);
        scoped_refptr<DelegatedRendererLayer> observer2 = DelegatedRendererLayer::Create(layer_settings_, frame_provider_);

        gfx::Rect damage;

        // Take a ref on each observer.
        frame_provider_->GetFrameDataAndRefResources(observer1.get(), &damage);
        frame_provider_->GetFrameDataAndRefResources(observer2.get(), &damage);

        EXPECT_FALSE(ReturnAndResetResourcesAvailable());

        // Release both refs. But there's still a ref held in the frame
        // provider itself.
        frame_provider_->UnrefResourcesOnMainThread(returned);
        frame_provider_->UnrefResourcesOnMainThread(returned);
        EXPECT_FALSE(ReturnAndResetResourcesAvailable());

        // Releasing all references to the frame provider will release
        // the frame.
        observer1 = nullptr;
        observer2 = nullptr;
        EXPECT_FALSE(ReturnAndResetResourcesAvailable());

        frame_provider_ = nullptr;

        EXPECT_TRUE(ReturnAndResetResourcesAvailable());
        EXPECT_EQ(1u, resources_.size());
        EXPECT_EQ(444u, resources_[0].id);
    }

    TEST_F(DelegatedFrameProviderTest, Damage)
    {
        scoped_ptr<DelegatedFrameData> frame = CreateFrameData(gfx::Rect(5, 5), gfx::Rect(2, 2));
        AddTextureQuad(frame.get(), 444);
        AddTransferableResource(frame.get(), 444);

        TransferableResourceArray reffed = frame->resource_list;
        ReturnedResourceArray returned;
        TransferableResource::ReturnResources(reffed, &returned);

        SetFrameProvider(frame.Pass());

        scoped_refptr<DelegatedRendererLayer> observer1 = DelegatedRendererLayer::Create(layer_settings_, frame_provider_);
        scoped_refptr<DelegatedRendererLayer> observer2 = DelegatedRendererLayer::Create(layer_settings_, frame_provider_);

        gfx::Rect damage;

        // Both observers get a full frame of damage on the first request.
        frame_provider_->GetFrameDataAndRefResources(observer1.get(), &damage);
        EXPECT_EQ(gfx::Rect(5, 5), damage);
        frame_provider_->GetFrameDataAndRefResources(observer2.get(), &damage);
        EXPECT_EQ(gfx::Rect(5, 5), damage);

        // And both get no damage on the 2nd request.
        frame_provider_->GetFrameDataAndRefResources(observer1.get(), &damage);
        EXPECT_EQ(gfx::Rect(), damage);
        frame_provider_->GetFrameDataAndRefResources(observer2.get(), &damage);
        EXPECT_EQ(gfx::Rect(), damage);

        frame = CreateFrameData(gfx::Rect(5, 5), gfx::Rect(2, 2));
        AddTextureQuad(frame.get(), 555);
        AddTransferableResource(frame.get(), 555);
        frame_provider_->SetFrameData(frame.Pass());

        // Both observers get the damage for the new frame.
        frame_provider_->GetFrameDataAndRefResources(observer1.get(), &damage);
        EXPECT_EQ(gfx::Rect(2, 2), damage);
        frame_provider_->GetFrameDataAndRefResources(observer2.get(), &damage);
        EXPECT_EQ(gfx::Rect(2, 2), damage);

        // And both get no damage on the 2nd request.
        frame_provider_->GetFrameDataAndRefResources(observer1.get(), &damage);
        EXPECT_EQ(gfx::Rect(), damage);
        frame_provider_->GetFrameDataAndRefResources(observer2.get(), &damage);
        EXPECT_EQ(gfx::Rect(), damage);
    }

    TEST_F(DelegatedFrameProviderTest, LostNothing)
    {
        scoped_ptr<DelegatedFrameData> frame = CreateFrameData(gfx::Rect(5, 5), gfx::Rect(5, 5));

        TransferableResourceArray reffed = frame->resource_list;

        SetFrameProvider(frame.Pass());

        // There is nothing to lose.
        EXPECT_FALSE(ReturnAndResetResourcesAvailable());
        EXPECT_FALSE(resource_collection_->LoseAllResources());
        EXPECT_FALSE(ReturnAndResetResourcesAvailable());
        EXPECT_EQ(0u, resources_.size());
    }

    TEST_F(DelegatedFrameProviderTest, LostSomething)
    {
        scoped_ptr<DelegatedFrameData> frame = CreateFrameData(gfx::Rect(5, 5), gfx::Rect(5, 5));
        AddTextureQuad(frame.get(), 444);
        AddTransferableResource(frame.get(), 444);

        SetFrameProvider(frame.Pass());

        // Add a second reference on the resource.
        frame = CreateFrameData(gfx::Rect(5, 5), gfx::Rect(5, 5));
        AddTextureQuad(frame.get(), 444);
        AddTransferableResource(frame.get(), 444);

        SetFrameProvider(frame.Pass());

        // There is something to lose.
        EXPECT_FALSE(ReturnAndResetResourcesAvailable());
        EXPECT_TRUE(resource_collection_->LoseAllResources());
        EXPECT_TRUE(ReturnAndResetResourcesAvailable());

        EXPECT_EQ(1u, resources_.size());
        EXPECT_EQ(444u, resources_[0].id);
        EXPECT_EQ(2, resources_[0].count);
    }

    TEST_F(DelegatedFrameProviderTest, NothingReturnedAfterLoss)
    {
        scoped_ptr<DelegatedFrameData> frame = CreateFrameData(gfx::Rect(1, 1), gfx::Rect(1, 1));
        AddTextureQuad(frame.get(), 444);
        AddTransferableResource(frame.get(), 444);
        SetFrameProvider(frame.Pass());

        EXPECT_FALSE(ReturnAndResetResourcesAvailable());

        // Lose all the resources.
        EXPECT_TRUE(resource_collection_->LoseAllResources());
        EXPECT_TRUE(ReturnAndResetResourcesAvailable());
        resources_.clear();

        frame_provider_ = nullptr;

        // Nothing is returned twice.
        EXPECT_FALSE(ReturnAndResetResourcesAvailable());
        EXPECT_EQ(0u, resources_.size());
    }

} // namespace
} // namespace cc
