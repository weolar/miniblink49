// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_DELEGATED_FRAME_RESOURCE_COLLECTION_H_
#define CC_LAYERS_DELEGATED_FRAME_RESOURCE_COLLECTION_H_

#include "base/containers/hash_tables.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread_checker.h"
#include "cc/base/cc_export.h"
#include "cc/resources/return_callback.h"
#include "cc/resources/returned_resource.h"
#include "cc/resources/transferable_resource.h"

namespace cc {

class CC_EXPORT DelegatedFrameResourceCollectionClient {
public:
    // Called to inform the client that returned resources can be
    // grabbed off the DelegatedFrameResourceCollection.
    virtual void UnusedResourcesAreAvailable() = 0;
};

class CC_EXPORT DelegatedFrameResourceCollection
    : public base::RefCounted<DelegatedFrameResourceCollection> {
public:
    DelegatedFrameResourceCollection();

    void SetClient(DelegatedFrameResourceCollectionClient* client);

    void TakeUnusedResourcesForChildCompositor(ReturnedResourceArray* array);

    // Considers all resources as lost, and returns true if it held any resource
    // to lose.
    bool LoseAllResources();

    // Methods for DelegatedFrameProvider.
    void RefResources(const TransferableResourceArray& resources);
    void UnrefResources(const ReturnedResourceArray& returned);
    void ReceivedResources(const TransferableResourceArray& resources);
    ReturnCallback GetReturnResourcesCallbackForImplThread();

private:
    friend class base::RefCounted<DelegatedFrameResourceCollection>;
    ~DelegatedFrameResourceCollection();

    DelegatedFrameResourceCollectionClient* client_;

    ReturnedResourceArray returned_resources_for_child_compositor_;
    bool lost_all_resources_;

    struct RefCount {
        int refs_to_return;
        int refs_to_wait_for;
    };
    typedef base::hash_map<unsigned, RefCount> ResourceIdRefCountMap;
    ResourceIdRefCountMap resource_id_ref_count_map_;

    base::ThreadChecker main_thread_checker_;
    base::WeakPtrFactory<DelegatedFrameResourceCollection> weak_ptr_factory_;

    DISALLOW_COPY_AND_ASSIGN(DelegatedFrameResourceCollection);
};

} // namespace cc

#endif // CC_LAYERS_DELEGATED_FRAME_RESOURCE_COLLECTION_H_
