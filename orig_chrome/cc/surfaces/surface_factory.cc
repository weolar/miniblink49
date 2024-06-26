// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/surfaces/surface_factory.h"

#include "base/trace_event/trace_event.h"
#include "cc/output/compositor_frame.h"
#include "cc/output/copy_output_request.h"
#include "cc/surfaces/surface.h"
#include "cc/surfaces/surface_manager.h"
#include "ui/gfx/geometry/size.h"

namespace cc {
SurfaceFactory::SurfaceFactory(SurfaceManager* manager,
    SurfaceFactoryClient* client)
    : manager_(manager)
    , client_(client)
    , holder_(client)
    , needs_sync_points_(true)
{
}

SurfaceFactory::~SurfaceFactory()
{
    if (!surface_map_.empty()) {
        LOG(ERROR) << "SurfaceFactory has " << surface_map_.size()
                   << " entries in map on destruction.";
    }
    DestroyAll();
}

void SurfaceFactory::DestroyAll()
{
    for (auto it = surface_map_.begin(); it != surface_map_.end(); ++it)
        manager_->Destroy(surface_map_.take(it));
    surface_map_.clear();
}

void SurfaceFactory::Create(SurfaceId surface_id)
{
    scoped_ptr<Surface> surface(new Surface(surface_id, this));
    manager_->RegisterSurface(surface.get());
    DCHECK(!surface_map_.count(surface_id));
    surface_map_.add(surface_id, surface.Pass());
}

void SurfaceFactory::Destroy(SurfaceId surface_id)
{
    OwningSurfaceMap::iterator it = surface_map_.find(surface_id);
    DCHECK(it != surface_map_.end());
    DCHECK(it->second->factory().get() == this);
    manager_->Destroy(surface_map_.take_and_erase(it));
}

void SurfaceFactory::SubmitCompositorFrame(SurfaceId surface_id,
    scoped_ptr<CompositorFrame> frame,
    const DrawCallback& callback)
{
    TRACE_EVENT0("cc", "SurfaceFactory::SubmitCompositorFrame");
    OwningSurfaceMap::iterator it = surface_map_.find(surface_id);
    DCHECK(it != surface_map_.end());
    DCHECK(it->second->factory().get() == this);
    it->second->QueueFrame(frame.Pass(), callback);
    if (!manager_->SurfaceModified(surface_id)) {
        TRACE_EVENT_INSTANT0("cc", "Damage not visible.", TRACE_EVENT_SCOPE_THREAD);
        it->second->RunDrawCallbacks(SurfaceDrawStatus::DRAW_SKIPPED);
    }
}

void SurfaceFactory::RequestCopyOfSurface(
    SurfaceId surface_id,
    scoped_ptr<CopyOutputRequest> copy_request)
{
    OwningSurfaceMap::iterator it = surface_map_.find(surface_id);
    if (it == surface_map_.end()) {
        copy_request->SendEmptyResult();
        return;
    }
    DCHECK(it->second->factory().get() == this);
    it->second->RequestCopyOfOutput(copy_request.Pass());
    manager_->SurfaceModified(surface_id);
}

void SurfaceFactory::ReceiveFromChild(
    const TransferableResourceArray& resources)
{
    holder_.ReceiveFromChild(resources);
}

void SurfaceFactory::RefResources(const TransferableResourceArray& resources)
{
    holder_.RefResources(resources);
}

void SurfaceFactory::UnrefResources(const ReturnedResourceArray& resources)
{
    holder_.UnrefResources(resources);
}

} // namespace cc
