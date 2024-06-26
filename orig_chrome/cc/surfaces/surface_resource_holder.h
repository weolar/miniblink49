// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SURFACES_SURFACE_RESOURCE_HOLDER_H_
#define CC_SURFACES_SURFACE_RESOURCE_HOLDER_H_

#include "base/containers/hash_tables.h"
#include "base/macros.h"
#include "cc/base/resource_id.h"
#include "cc/resources/returned_resource.h"
#include "cc/resources/transferable_resource.h"
#include "cc/surfaces/surfaces_export.h"

namespace cc {
class SurfaceFactoryClient;

// A SurfaceResourceHolder manages the lifetime of resources submitted by a
// particular SurfaceFactory. Each resource is held by the service until
// it is no longer referenced by any pending frames or by any
// resource providers.
class CC_SURFACES_EXPORT SurfaceResourceHolder {
public:
    explicit SurfaceResourceHolder(SurfaceFactoryClient* client);
    ~SurfaceResourceHolder();

    void ReceiveFromChild(const TransferableResourceArray& resources);
    void RefResources(const TransferableResourceArray& resources);
    void UnrefResources(const ReturnedResourceArray& resources);

private:
    SurfaceFactoryClient* client_;

    struct ResourceRefs {
        ResourceRefs();

        int refs_received_from_child;
        int refs_holding_resource_alive;
    };
    // Keeps track of the number of users currently in flight for each resource
    // ID we've received from the client. When this counter hits zero for a
    // particular resource, that ID is available to return to the client.
    typedef base::hash_map<ResourceId, ResourceRefs> ResourceIdCountMap;
    ResourceIdCountMap resource_id_use_count_map_;

    DISALLOW_COPY_AND_ASSIGN(SurfaceResourceHolder);
};

} // namespace cc

#endif // CC_SURFACES_SURFACE_RESOURCE_HOLDER_H_
