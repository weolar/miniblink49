// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_layer_tree_host_client.h"

#include "cc/output/context_provider.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/test_web_graphics_context_3d.h"
#include "cc/trees/layer_tree_host.h"

namespace cc {

FakeLayerTreeHostClient::FakeLayerTreeHostClient(RendererOptions options)
    : use_software_rendering_(options == DIRECT_SOFTWARE || options == DELEGATED_SOFTWARE)
    , use_delegating_renderer_(options == DELEGATED_3D || options == DELEGATED_SOFTWARE)
    , host_(NULL)
{
}

FakeLayerTreeHostClient::~FakeLayerTreeHostClient() { }

void FakeLayerTreeHostClient::RequestNewOutputSurface()
{
    DCHECK(host_);
    scoped_ptr<OutputSurface> surface;
    if (use_software_rendering_) {
        if (use_delegating_renderer_) {
            surface = FakeOutputSurface::CreateDelegatingSoftware(
                make_scoped_ptr(new SoftwareOutputDevice));
        } else {
            surface = FakeOutputSurface::CreateSoftware(
                make_scoped_ptr(new SoftwareOutputDevice));
        }
    } else if (use_delegating_renderer_) {
        surface = FakeOutputSurface::CreateDelegating3d();
    } else {
        surface = FakeOutputSurface::Create3d();
    }
    host_->SetOutputSurface(surface.Pass());
}

void FakeLayerTreeHostClient::DidFailToInitializeOutputSurface()
{
    RequestNewOutputSurface();
}

} // namespace cc
