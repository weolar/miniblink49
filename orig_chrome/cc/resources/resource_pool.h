// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RESOURCES_RESOURCE_POOL_H_
#define CC_RESOURCES_RESOURCE_POOL_H_

#include <deque>

#include "base/containers/scoped_ptr_map.h"
#include "base/memory/scoped_ptr.h"
#include "base/trace_event/memory_dump_provider.h"
#include "cc/base/cc_export.h"
#include "cc/base/scoped_ptr_deque.h"
#include "cc/output/renderer.h"
#include "cc/resources/resource.h"
#include "cc/resources/resource_format.h"
#include "cc/resources/scoped_resource.h"

namespace cc {

class CC_EXPORT ResourcePool : public base::trace_event::MemoryDumpProvider {
public:
    static scoped_ptr<ResourcePool> Create(
        ResourceProvider* resource_provider,
        base::SingleThreadTaskRunner* task_runner)
    {
        return make_scoped_ptr(
            new ResourcePool(resource_provider, task_runner, 0 /* target */));
    }

    static scoped_ptr<ResourcePool> Create(
        ResourceProvider* resource_provider,
        base::SingleThreadTaskRunner* task_runner,
        GLenum target)
    {
        DCHECK_NE(0u, target);
        return make_scoped_ptr(
            new ResourcePool(resource_provider, task_runner, target));
    }

    ~ResourcePool() override;

    Resource* AcquireResource(const gfx::Size& size, ResourceFormat format);
    Resource* TryAcquireResourceWithContentId(uint64 content_id);
    void ReleaseResource(Resource* resource, uint64_t content_id);

    void SetResourceUsageLimits(size_t max_memory_usage_bytes,
        size_t max_resource_count);

    void ReduceResourceUsage();
    void CheckBusyResources();

    size_t memory_usage_bytes() const { return in_use_memory_usage_bytes_; }
    size_t resource_count() const { return in_use_resources_.size(); }

    // Overridden from base::trace_event::MemoryDumpProvider:
    bool OnMemoryDump(const base::trace_event::MemoryDumpArgs& args,
        base::trace_event::ProcessMemoryDump* pmd) override;

    size_t GetTotalMemoryUsageForTesting() const
    {
        return total_memory_usage_bytes_;
    }
    size_t GetTotalResourceCountForTesting() const
    {
        return total_resource_count_;
    }
    size_t GetBusyResourceCountForTesting() const
    {
        return busy_resources_.size();
    }
    void SetResourceExpirationDelayForTesting(base::TimeDelta delay)
    {
        resource_expiration_delay_ = delay;
    }

protected:
    ResourcePool(ResourceProvider* resource_provider,
        base::SingleThreadTaskRunner* task_runner,
        GLenum target);

    bool ResourceUsageTooHigh();

private:
    class PoolResource : public ScopedResource {
    public:
        static scoped_ptr<PoolResource> Create(
            ResourceProvider* resource_provider)
        {
            return make_scoped_ptr(new PoolResource(resource_provider));
        }
        void OnMemoryDump(base::trace_event::ProcessMemoryDump* pmd,
            const ResourceProvider* resource_provider,
            bool is_free) const;

        uint64_t content_id() const { return content_id_; }
        void set_content_id(uint64_t content_id) { content_id_ = content_id; }

        base::TimeTicks last_usage() const { return last_usage_; }
        void set_last_usage(base::TimeTicks time) { last_usage_ = time; }

    private:
        explicit PoolResource(ResourceProvider* resource_provider)
            : ScopedResource(resource_provider)
            , content_id_(0)
        {
        }
        uint64_t content_id_;
        base::TimeTicks last_usage_;
    };

    void DidFinishUsingResource(scoped_ptr<PoolResource> resource);
    void DeleteResource(scoped_ptr<PoolResource> resource);

    // Functions which manage periodic eviction of expired resources.
    void ScheduleEvictExpiredResourcesIn(base::TimeDelta time_from_now);
    void EvictExpiredResources();
    void EvictResourcesNotUsedSince(base::TimeTicks time_limit);
    bool HasEvictableResources() const;
    base::TimeTicks GetUsageTimeForLRUResource() const;

    ResourceProvider* resource_provider_;
    const GLenum target_;
    size_t max_memory_usage_bytes_;
    size_t max_resource_count_;
    size_t in_use_memory_usage_bytes_;
    size_t total_memory_usage_bytes_;
    size_t total_resource_count_;

    using ResourceDeque = ScopedPtrDeque<PoolResource>;
    ResourceDeque unused_resources_;
    ResourceDeque busy_resources_;

    using ResourceMap = base::ScopedPtrMap<ResourceId, scoped_ptr<PoolResource>>;
    ResourceMap in_use_resources_;

    scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
    bool evict_expired_resources_pending_;
    base::TimeDelta resource_expiration_delay_;

    base::WeakPtrFactory<ResourcePool> weak_ptr_factory_;

    DISALLOW_COPY_AND_ASSIGN(ResourcePool);
};

} // namespace cc

#endif // CC_RESOURCES_RESOURCE_POOL_H_
