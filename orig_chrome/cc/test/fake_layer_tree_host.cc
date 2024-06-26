// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_layer_tree_host.h"

#include "cc/layers/layer.h"
#include "cc/test/test_task_graph_runner.h"

namespace cc {
FakeLayerTreeHost::FakeLayerTreeHost(FakeLayerTreeHostClient* client,
    LayerTreeHost::InitParams* params)
    : LayerTreeHost(params)
    , client_(client)
    , host_impl_(*params->settings,
          &proxy_,
          &manager_,
          params->task_graph_runner)
    , needs_commit_(false)
{
    client_->SetLayerTreeHost(this);
}

scoped_ptr<FakeLayerTreeHost> FakeLayerTreeHost::Create(
    FakeLayerTreeHostClient* client,
    TestTaskGraphRunner* task_graph_runner)
{
    LayerTreeSettings settings;
    settings.verify_property_trees = true;
    return Create(client, task_graph_runner, settings);
}

scoped_ptr<FakeLayerTreeHost> FakeLayerTreeHost::Create(
    FakeLayerTreeHostClient* client,
    TestTaskGraphRunner* task_graph_runner,
    const LayerTreeSettings& settings)
{
    LayerTreeHost::InitParams params;
    params.client = client;
    params.settings = &settings;
    params.task_graph_runner = task_graph_runner;
    return make_scoped_ptr(new FakeLayerTreeHost(client, &params));
}

FakeLayerTreeHost::~FakeLayerTreeHost()
{
    client_->SetLayerTreeHost(NULL);
}

void FakeLayerTreeHost::SetNeedsCommit() { needs_commit_ = true; }

LayerImpl* FakeLayerTreeHost::CommitAndCreateLayerImplTree()
{
    scoped_ptr<LayerImpl> old_root_layer_impl = active_tree()->DetachLayerTree();

    scoped_ptr<LayerImpl> layer_impl = TreeSynchronizer::SynchronizeTrees(
        root_layer(), old_root_layer_impl.Pass(), active_tree());
    active_tree()->SetPropertyTrees(*property_trees());
    TreeSynchronizer::PushProperties(root_layer(), layer_impl.get());

    active_tree()->SetRootLayer(layer_impl.Pass());

    if (page_scale_layer() && inner_viewport_scroll_layer()) {
        active_tree()->SetViewportLayersFromIds(
            overscroll_elasticity_layer() ? overscroll_elasticity_layer()->id()
                                          : Layer::INVALID_ID,
            page_scale_layer()->id(), inner_viewport_scroll_layer()->id(),
            outer_viewport_scroll_layer() ? outer_viewport_scroll_layer()->id()
                                          : Layer::INVALID_ID);
    }

    active_tree()->UpdatePropertyTreesForBoundsDelta();
    return active_tree()->root_layer();
}

} // namespace cc
