// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/surfaces/surface_id_allocator.h"

#include "cc/surfaces/surface_manager.h"

namespace cc {

SurfaceIdAllocator::SurfaceIdAllocator(uint32_t id_namespace)
    : id_namespace_(id_namespace)
    , next_id_(1u)
    , manager_(nullptr)
{
}

void SurfaceIdAllocator::RegisterSurfaceIdNamespace(SurfaceManager* manager)
{
    DCHECK(!manager_);
    manager_ = manager;
    manager_->RegisterSurfaceIdNamespace(id_namespace_);
}

SurfaceIdAllocator::~SurfaceIdAllocator()
{
    if (manager_)
        manager_->InvalidateSurfaceIdNamespace(id_namespace_);
}

SurfaceId SurfaceIdAllocator::GenerateId()
{
    SurfaceId id(static_cast<uint64_t>(id_namespace_) << 32 | next_id_);
    next_id_++;
    return id;
}

// static
uint32_t SurfaceIdAllocator::NamespaceForId(SurfaceId id)
{
    return id.id >> 32;
}

} // namespace cc
