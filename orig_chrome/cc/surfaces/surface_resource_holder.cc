// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/surfaces/surface_resource_holder.h"

#include "cc/surfaces/surface_factory_client.h"

namespace cc {

SurfaceResourceHolder::SurfaceResourceHolder(SurfaceFactoryClient* client)
    : client_(client)
{
}

SurfaceResourceHolder::~SurfaceResourceHolder()
{
}

SurfaceResourceHolder::ResourceRefs::ResourceRefs()
    : refs_received_from_child(0)
    , refs_holding_resource_alive(0)
{
}

void SurfaceResourceHolder::ReceiveFromChild(
    const TransferableResourceArray& resources)
{
    for (TransferableResourceArray::const_iterator it = resources.begin();
         it != resources.end();
         ++it) {
        ResourceRefs& ref = resource_id_use_count_map_[it->id];
        ref.refs_holding_resource_alive++;
        ref.refs_received_from_child++;
    }
}

void SurfaceResourceHolder::RefResources(
    const TransferableResourceArray& resources)
{
    for (TransferableResourceArray::const_iterator it = resources.begin();
         it != resources.end();
         ++it) {
        ResourceIdCountMap::iterator count_it = resource_id_use_count_map_.find(it->id);
        DCHECK(count_it != resource_id_use_count_map_.end());
        count_it->second.refs_holding_resource_alive++;
    }
}

void SurfaceResourceHolder::UnrefResources(
    const ReturnedResourceArray& resources)
{
    ReturnedResourceArray resources_available_to_return;

    for (ReturnedResourceArray::const_iterator it = resources.begin();
         it != resources.end();
         ++it) {
        unsigned id = it->id;
        ResourceIdCountMap::iterator count_it = resource_id_use_count_map_.find(id);
        if (count_it == resource_id_use_count_map_.end())
            continue;
        ResourceRefs& ref = count_it->second;
        ref.refs_holding_resource_alive -= it->count;
        if (ref.refs_holding_resource_alive == 0) {
            ReturnedResource returned = *it;
            returned.count = ref.refs_received_from_child;
            resources_available_to_return.push_back(returned);
            resource_id_use_count_map_.erase(count_it);
        }
    }

    client_->ReturnResources(resources_available_to_return);
}

} // namespace cc
