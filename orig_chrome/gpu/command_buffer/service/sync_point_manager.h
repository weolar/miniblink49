// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_SYNC_POINT_MANAGER_H_
#define GPU_COMMAND_BUFFER_SERVICE_SYNC_POINT_MANAGER_H_

#include <functional>
#include <queue>
#include <vector>

#include "base/atomic_sequence_num.h"
#include "base/callback.h"
#include "base/containers/hash_tables.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/synchronization/condition_variable.h"
#include "base/synchronization/lock.h"
#include "base/threading/thread_checker.h"
#include "gpu/command_buffer/common/constants.h"
#include "gpu/gpu_export.h"

namespace base {
class SingleThreadTaskRunner;
} // namespace base

namespace gpu {

class SyncPointClient;
class SyncPointClientState;
class SyncPointManager;

class GPU_EXPORT SyncPointOrderData
    : public base::RefCountedThreadSafe<SyncPointOrderData> {
public:
    static scoped_refptr<SyncPointOrderData> Create();
    void Destroy();

    uint32_t GenerateUnprocessedOrderNumber(SyncPointManager* sync_point_manager);
    void BeginProcessingOrderNumber(uint32_t order_num);
    void FinishProcessingOrderNumber(uint32_t order_num);

    uint32_t processed_order_num() const
    {
        base::AutoLock auto_lock(lock_);
        return processed_order_num_;
    }

    uint32_t unprocessed_order_num() const
    {
        base::AutoLock auto_lock(lock_);
        return unprocessed_order_num_;
    }

    uint32_t current_order_num() const
    {
        DCHECK(processing_thread_checker_.CalledOnValidThread());
        return current_order_num_;
    }

private:
    friend class base::RefCountedThreadSafe<SyncPointOrderData>;
    friend class SyncPointClientState;

    struct OrderFence {
        uint32_t order_num;
        uint64_t fence_release;
        scoped_refptr<SyncPointClientState> client_state;

        OrderFence(uint32_t order,
            uint64_t release,
            scoped_refptr<SyncPointClientState> state);
        ~OrderFence();

        bool operator>(const OrderFence& rhs) const
        {
            return (order_num > rhs.order_num) || ((order_num == rhs.order_num) && (fence_release > rhs.fence_release));
        }
    };
    typedef std::priority_queue<OrderFence,
        std::vector<OrderFence>,
        std::greater<OrderFence>>
        OrderFenceQueue;

    SyncPointOrderData();
    ~SyncPointOrderData();

    bool ValidateReleaseOrderNumber(
        scoped_refptr<SyncPointClientState> client_state,
        uint32_t wait_order_num,
        uint64_t fence_release);

    // Non thread-safe functions need to be called from a single thread.
    base::ThreadChecker processing_thread_checker_;

    // Current IPC order number being processed (only used on processing thread).
    uint32_t current_order_num_;

    // This lock protects destroyed_, processed_order_num_,
    // unprocessed_order_num_, and order_fence_queue_. All order numbers (n) in
    // order_fence_queue_ must follow the invariant:
    //   processed_order_num_ < n <= unprocessed_order_num_.
    mutable base::Lock lock_;

    bool destroyed_;

    // Last finished IPC order number.
    uint32_t processed_order_num_;

    // Unprocessed order number expected to be processed under normal execution.
    uint32_t unprocessed_order_num_;

    // In situations where we are waiting on fence syncs that do not exist, we
    // validate by making sure the order number does not pass the order number
    // which the wait command was issued. If the order number reaches the
    // wait command's, we should automatically release up to the expected
    // release count. Note that this also releases other lower release counts,
    // so a single misbehaved fence sync is enough to invalidate/signal all
    // previous fence syncs.
    OrderFenceQueue order_fence_queue_;

    DISALLOW_COPY_AND_ASSIGN(SyncPointOrderData);
};

class GPU_EXPORT SyncPointClientState
    : public base::RefCountedThreadSafe<SyncPointClientState> {
public:
    scoped_refptr<SyncPointOrderData> order_data() { return order_data_; }

    bool IsFenceSyncReleased(uint64_t release)
    {
        return release <= fence_sync_release();
    }

    uint64_t fence_sync_release()
    {
        base::AutoLock auto_lock(fence_sync_lock_);
        return fence_sync_release_;
    }

private:
    friend class base::RefCountedThreadSafe<SyncPointClientState>;
    friend class SyncPointClient;
    friend class SyncPointClientWaiter;
    friend class SyncPointOrderData;

    struct ReleaseCallback {
        uint64_t release_count;
        base::Closure callback_closure;

        ReleaseCallback(uint64_t release, const base::Closure& callback);
        ~ReleaseCallback();

        bool operator>(const ReleaseCallback& rhs) const
        {
            return release_count > rhs.release_count;
        }
    };
    typedef std::priority_queue<ReleaseCallback,
        std::vector<ReleaseCallback>,
        std::greater<ReleaseCallback>>
        ReleaseCallbackQueue;

    SyncPointClientState(scoped_refptr<SyncPointOrderData> order_data);
    ~SyncPointClientState();

    // Queues the callback to be called if the release is valid. If the release
    // is invalid this function will return False and the callback will never
    // be called.
    bool WaitForRelease(uint32_t wait_order_num,
        uint64_t release,
        const base::Closure& callback);

    void ReleaseFenceSync(uint64_t release);
    void EnsureReleased(uint64_t release);
    void ReleaseFenceSyncLocked(uint64_t release,
        std::vector<base::Closure>* callback_list);

    // Global order data where releases will originate from.
    scoped_refptr<SyncPointOrderData> order_data_;

    // Protects fence_sync_release_, fence_callback_queue_.
    base::Lock fence_sync_lock_;

    // Current fence sync release that has been signaled.
    uint64_t fence_sync_release_;

    // In well defined fence sync operations, fence syncs are released in order
    // so simply having a priority queue for callbacks is enough.
    ReleaseCallbackQueue release_callback_queue_;

    DISALLOW_COPY_AND_ASSIGN(SyncPointClientState);
};

class GPU_EXPORT SyncPointClient {
public:
    ~SyncPointClient();

    scoped_refptr<SyncPointClientState> client_state() { return client_state_; }

    // Wait for a release count to be reached on a SyncPointClientState. If this
    // function returns false, that means the wait was invalid. Otherwise if it
    // returns True it means the release was valid. In the case where the release
    // is valid but has happened already, it will still return true. In all cases
    // wait_complete_callback will be called eventually. The callback function
    // may be called on another thread so it should be thread-safe. For
    // convenience, another non-threadsafe version is defined below where you
    // can supply a task runner.
    bool Wait(SyncPointClientState* release_state,
        uint64_t release_count,
        const base::Closure& wait_complete_callback);

    bool WaitNonThreadSafe(SyncPointClientState* release_state,
        uint64_t release_count,
        scoped_refptr<base::SingleThreadTaskRunner> runner,
        const base::Closure& wait_complete_callback);

    void ReleaseFenceSync(uint64_t release);

private:
    friend class SyncPointManager;

    SyncPointClient(SyncPointManager* sync_point_manager,
        scoped_refptr<SyncPointOrderData> order_data,
        CommandBufferNamespace namespace_id,
        uint64_t client_id);

    // Sync point manager is guaranteed to exist in the lifetime of the client.
    SyncPointManager* sync_point_manager_;

    // Keep the state that is sharable across multiple threads.
    scoped_refptr<SyncPointClientState> client_state_;

    // Unique namespace/client id pair for this sync point client.
    const CommandBufferNamespace namespace_id_;
    const uint64_t client_id_;

    DISALLOW_COPY_AND_ASSIGN(SyncPointClient);
};

// A SyncPointClientWaiter is a Sync Point Client which can only wait and on
// fence syncs and not release any fence syncs itself. Because they cannot
// release any fence syncs they do not need an associated order number since
// deadlocks cannot happen. Note that it is important that this class does
// not exist in the same execution context as a SyncPointClient, or else a
// deadlock could occur. Basically, SyncPointClientWaiter::Wait() should never
// be called between SyncPointOrderData::BeginProcessingOrderNumber() and
// SyncPointOrderData::FinishProcessingOrderNumber() on the same thread.
class GPU_EXPORT SyncPointClientWaiter {
public:
    SyncPointClientWaiter() { }
    ~SyncPointClientWaiter() { }

    bool Wait(SyncPointClientState* release_state,
        uint64_t release_count,
        const base::Closure& wait_complete_callback);

    bool WaitNonThreadSafe(SyncPointClientState* release_state,
        uint64_t release_count,
        scoped_refptr<base::SingleThreadTaskRunner> runner,
        const base::Closure& wait_complete_callback);

private:
    DISALLOW_COPY_AND_ASSIGN(SyncPointClientWaiter);
};

// This class manages the sync points, which allow cross-channel
// synchronization.
class GPU_EXPORT SyncPointManager {
public:
    explicit SyncPointManager(bool allow_threaded_wait);
    ~SyncPointManager();

    // Creates/Destroy a sync point client which message processors should hold.
    scoped_ptr<SyncPointClient> CreateSyncPointClient(
        scoped_refptr<SyncPointOrderData> order_data,
        CommandBufferNamespace namespace_id,
        uint64_t client_id);

    // Finds the state of an already created sync point client.
    scoped_refptr<SyncPointClientState> GetSyncPointClientState(
        CommandBufferNamespace namespace_id, uint64_t client_id);

    // Generates a sync point, returning its ID. This can me called on any thread.
    // IDs start at a random number. Never return 0.
    uint32 GenerateSyncPoint();

    // Retires a sync point. This will call all the registered callbacks for this
    // sync point. This can only be called on the main thread.
    void RetireSyncPoint(uint32 sync_point);

    // Adds a callback to the sync point. The callback will be called when the
    // sync point is retired, or immediately (from within that function) if the
    // sync point was already retired (or not created yet). This can only be
    // called on the main thread.
    void AddSyncPointCallback(uint32 sync_point, const base::Closure& callback);

    bool IsSyncPointRetired(uint32 sync_point);

    // Block and wait until a sync point is signaled. This is only useful when
    // the sync point is signaled on another thread.
    void WaitSyncPoint(uint32 sync_point);

private:
    friend class SyncPointClient;
    friend class SyncPointOrderData;

    typedef std::vector<base::Closure> ClosureList;
    typedef base::hash_map<uint32, ClosureList> SyncPointMap;
    typedef base::hash_map<uint64_t, SyncPointClient*> ClientMap;

    bool IsSyncPointRetiredLocked(uint32 sync_point);
    uint32_t GenerateOrderNumber();
    void DestroySyncPointClient(CommandBufferNamespace namespace_id,
        uint64_t client_id);

    const bool allow_threaded_wait_;

    // Order number is global for all clients.
    base::AtomicSequenceNumber global_order_num_;

    // Client map holds a map of clients id to client for each namespace.
    base::Lock client_maps_lock_;
    ClientMap client_maps_[NUM_COMMAND_BUFFER_NAMESPACES];

    // Protects the 2 fields below. Note: callbacks shouldn't be called with this
    // held.
    base::Lock lock_;
    SyncPointMap sync_point_map_;
    uint32 next_sync_point_;
    base::ConditionVariable retire_cond_var_;

    DISALLOW_COPY_AND_ASSIGN(SyncPointManager);
};

} // namespace gpu

#endif // GPU_COMMAND_BUFFER_SERVICE_SYNC_POINT_MANAGER_H_
