// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/begin_frame_args_test.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/trees/layer_tree_impl.h"

namespace cc {

FakeLayerTreeHostImpl::FakeLayerTreeHostImpl(Proxy* proxy,
    SharedBitmapManager* manager,
    TaskGraphRunner* task_graph_runner)
    : FakeLayerTreeHostImpl(LayerTreeSettings(),
        proxy,
        manager,
        task_graph_runner,
        nullptr)
{
}

FakeLayerTreeHostImpl::FakeLayerTreeHostImpl(const LayerTreeSettings& settings,
    Proxy* proxy,
    SharedBitmapManager* manager,
    TaskGraphRunner* task_graph_runner)
    : FakeLayerTreeHostImpl(settings,
        proxy,
        manager,
        task_graph_runner,
        nullptr)
{
}

FakeLayerTreeHostImpl::FakeLayerTreeHostImpl(
    const LayerTreeSettings& settings,
    Proxy* proxy,
    SharedBitmapManager* manager,
    TaskGraphRunner* task_graph_runner,
    gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager)
    : LayerTreeHostImpl(settings,
        &client_,
        proxy,
        &stats_instrumentation_,
        manager,
        gpu_memory_buffer_manager,
        task_graph_runner,
        0)
{
    // Explicitly clear all debug settings.
    SetDebugState(LayerTreeDebugState());
    SetViewportSize(gfx::Size(100, 100));

    // Start an impl frame so tests have a valid frame_time to work with.
    base::TimeTicks time_ticks = base::TimeTicks::FromInternalValue(1);
    WillBeginImplFrame(
        CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, time_ticks));
}

FakeLayerTreeHostImpl::~FakeLayerTreeHostImpl() { }

void FakeLayerTreeHostImpl::CreatePendingTree()
{
    LayerTreeHostImpl::CreatePendingTree();
    float arbitrary_large_page_scale = 100000.f;
    pending_tree()->PushPageScaleFromMainThread(
        1.f, 1.f / arbitrary_large_page_scale, arbitrary_large_page_scale);
}

BeginFrameArgs FakeLayerTreeHostImpl::CurrentBeginFrameArgs() const
{
    return current_begin_frame_tracker_.DangerousMethodCurrentOrLast();
}

void FakeLayerTreeHostImpl::AdvanceToNextFrame(base::TimeDelta advance_by)
{
    BeginFrameArgs next_begin_frame_args = current_begin_frame_tracker_.Current();
    next_begin_frame_args.frame_time += advance_by;
    DidFinishImplFrame();
    WillBeginImplFrame(next_begin_frame_args);
}

int FakeLayerTreeHostImpl::RecursiveUpdateNumChildren(LayerImpl* layer)
{
    int num_children_that_draw_content = 0;
    for (size_t i = 0; i < layer->children().size(); ++i) {
        num_children_that_draw_content += RecursiveUpdateNumChildren(layer->children()[i]);
    }
    if (layer->DrawsContent() && layer->HasDelegatedContent())
        num_children_that_draw_content += 1000;
    layer->SetNumDescendantsThatDrawContent(num_children_that_draw_content);
    return num_children_that_draw_content + (layer->DrawsContent() ? 1 : 0);
}

void FakeLayerTreeHostImpl::UpdateNumChildrenAndDrawPropertiesForActiveTree()
{
    UpdateNumChildrenAndDrawProperties(active_tree());
}

void FakeLayerTreeHostImpl::UpdateNumChildrenAndDrawProperties(
    LayerTreeImpl* layerTree)
{
    RecursiveUpdateNumChildren(layerTree->root_layer());
    bool update_lcd_text = false;
    layerTree->BuildPropertyTreesForTesting();
    layerTree->UpdateDrawProperties(update_lcd_text);
}

} // namespace cc
