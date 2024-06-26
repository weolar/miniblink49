// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_ui_resource_layer_tree_host_impl.h"

#include "cc/resources/ui_resource_bitmap.h"
#include "cc/test/fake_layer_tree_host_impl.h"

namespace cc {

FakeUIResourceLayerTreeHostImpl::FakeUIResourceLayerTreeHostImpl(
    Proxy* proxy,
    SharedBitmapManager* manager,
    TaskGraphRunner* task_graph_runner)
    : FakeLayerTreeHostImpl(proxy, manager, task_graph_runner)
{
}

FakeUIResourceLayerTreeHostImpl::~FakeUIResourceLayerTreeHostImpl() { }

void FakeUIResourceLayerTreeHostImpl::CreateUIResource(
    UIResourceId uid,
    const UIResourceBitmap& bitmap)
{
    if (ResourceIdForUIResource(uid))
        DeleteUIResource(uid);

    UIResourceData data;
    data.resource_id = resource_provider()->CreateResource(
        bitmap.GetSize(), GL_CLAMP_TO_EDGE,
        ResourceProvider::TEXTURE_HINT_IMMUTABLE, RGBA_8888);

    data.size = bitmap.GetSize();
    data.opaque = bitmap.GetOpaque();
    fake_ui_resource_map_[uid] = data;
}

void FakeUIResourceLayerTreeHostImpl::DeleteUIResource(UIResourceId uid)
{
    ResourceId id = ResourceIdForUIResource(uid);
    if (id)
        fake_ui_resource_map_.erase(uid);
}

ResourceId FakeUIResourceLayerTreeHostImpl::ResourceIdForUIResource(
    UIResourceId uid) const
{
    UIResourceMap::const_iterator iter = fake_ui_resource_map_.find(uid);
    if (iter != fake_ui_resource_map_.end())
        return iter->second.resource_id;
    return 0;
}

bool FakeUIResourceLayerTreeHostImpl::IsUIResourceOpaque(UIResourceId uid)
    const
{
    UIResourceMap::const_iterator iter = fake_ui_resource_map_.find(uid);
    DCHECK(iter != fake_ui_resource_map_.end());
    return iter->second.opaque;
}

} // namespace cc
