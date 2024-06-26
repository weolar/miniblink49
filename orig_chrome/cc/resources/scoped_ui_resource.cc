// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/resources/scoped_ui_resource.h"

#include "base/basictypes.h"
#include "base/bind.h"
#include "cc/trees/layer_tree_host.h"

namespace cc {

scoped_ptr<ScopedUIResource> ScopedUIResource::Create(
    LayerTreeHost* host,
    const UIResourceBitmap& bitmap)
{
    return make_scoped_ptr(new ScopedUIResource(host, bitmap));
}

ScopedUIResource::ScopedUIResource(LayerTreeHost* host,
    const UIResourceBitmap& bitmap)
    : bitmap_(bitmap)
    , host_(host)
{
    DCHECK(host_);
    id_ = host_->CreateUIResource(this);
}

// User must make sure that host is still valid before this object goes out of
// scope.
ScopedUIResource::~ScopedUIResource()
{
    if (id_) {
        DCHECK(host_);
        host_->DeleteUIResource(id_);
    }
}

UIResourceBitmap ScopedUIResource::GetBitmap(UIResourceId uid,
    bool resource_lost)
{
    return bitmap_;
}

} // namespace cc
