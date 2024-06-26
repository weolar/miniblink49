// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_UI_RESOURCE_LAYER_TREE_HOST_IMPL_H_
#define CC_TEST_FAKE_UI_RESOURCE_LAYER_TREE_HOST_IMPL_H_

#include "base/containers/hash_tables.h"
#include "cc/test/fake_layer_tree_host_impl.h"

namespace cc {
class TaskGraphRunner;

class FakeUIResourceLayerTreeHostImpl : public FakeLayerTreeHostImpl {
public:
    explicit FakeUIResourceLayerTreeHostImpl(Proxy* proxy,
        SharedBitmapManager* manager,
        TaskGraphRunner* task_graph_runner);
    ~FakeUIResourceLayerTreeHostImpl() override;

    void CreateUIResource(UIResourceId uid,
        const UIResourceBitmap& bitmap) override;

    void DeleteUIResource(UIResourceId uid) override;

    ResourceId ResourceIdForUIResource(UIResourceId uid) const override;

    bool IsUIResourceOpaque(UIResourceId uid) const override;

private:
    typedef base::hash_map<UIResourceId, LayerTreeHostImpl::UIResourceData>
        UIResourceMap;
    UIResourceMap fake_ui_resource_map_;
};

} // namespace cc

#endif // CC_TEST_FAKE_UI_RESOURCE_LAYER_TREE_HOST_IMPL_H_
