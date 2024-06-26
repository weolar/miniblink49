// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_scoped_ui_resource.h"

#include "cc/trees/layer_tree_host.h"

namespace cc {

namespace {

    UIResourceBitmap CreateMockUIResourceBitmap()
    {
        bool is_opaque = false;
        return UIResourceBitmap(gfx::Size(1, 1), is_opaque);
    }

} // anonymous namespace

scoped_ptr<FakeScopedUIResource> FakeScopedUIResource::Create(
    LayerTreeHost* host)
{
    return make_scoped_ptr(new FakeScopedUIResource(host));
}

FakeScopedUIResource::FakeScopedUIResource(LayerTreeHost* host)
    : ScopedUIResource(host, CreateMockUIResourceBitmap())
{
    // The constructor of ScopedUIResource already created a resource so we need
    // to delete the created resource to wipe the state clean.
    host_->DeleteUIResource(id_);
    ResetCounters();
    id_ = host_->CreateUIResource(this);
}

UIResourceBitmap FakeScopedUIResource::GetBitmap(UIResourceId uid,
    bool resource_lost)
{
    resource_create_count++;
    if (resource_lost)
        lost_resource_count++;
    return ScopedUIResource::GetBitmap(uid, resource_lost);
}

void FakeScopedUIResource::ResetCounters()
{
    resource_create_count = 0;
    lost_resource_count = 0;
}

} // namespace cc
