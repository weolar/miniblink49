// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/sync_point_manager.h"

#include <climits>

#include "base/bind.h"
#include "base/containers/hash_tables.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/rand_util.h"
#include "base/sequence_checker.h"
#include "base/single_thread_task_runner.h"

namespace gpu {

static const int kMaxSyncBase = INT_MAX;

namespace {

    void RunOnThread(scoped_refptr<base::SingleThreadTaskRunner> task_runner,
        const base::Closure& callback)
    {
        if (task_runner->BelongsToCurrentThread()) {
            callback.Run();
        } else {
            task_runner->PostTask(FROM_HERE, callback);
        }
    }

} // namespace

scoped_refptr<SyncPointOrderData> SyncPointOrderData::Create()
{
    return new SyncPointOrderData;
}

void SyncPointOrderData::Destroy()
{
    // Because of circular references between the SyncPointOrderData and
    // SyncPointClientState, we must remove the references on destroy. Releasing
    // the fence syncs in the order fence queue would be redundant at this point
    // because they are assumed to be released on the destruction of the
    // SyncPointClient.
    base::AutoLock auto_lock(lock_);
    destroyed_ = true;
    while (!order_fence_queue_.empty()) {
        order_fence_queue_.pop();
    }
}

uint32_t SyncPointOrderData::GenerateUnprocessedOrderNumber(
    SyncPointManager* sync_point_manager)
{
    const uint32_t order_num = sync_point_manager->GenerateOrderNumber();
    base::AutoLock auto_lock(lock_);
    unprocessed_order_num_ = order_num;
    return order_num;
}

void SyncPointOrderData::BeginProcessingOrderNumber(uint32_t order_num)
{
    DCHECK(processing_thread_checker_.CalledOnValidThread());
    DCHECK_GE(order_num, current_order_num_);
    current_order_num_ = order_num;

    // Catch invalid waits which were waiting on fence syncs that do not exist.
    // When we begin processing an order number, we should release any fence
    // syncs which were enqueued but the order number never existed.
    // Release without the lock to avoid possible deadlocks.
    std::vector<OrderFence> ensure_releases;
    {
        base::AutoLock auto_lock(lock_);
        while (!order_fence_queue_.empty()) {
            const OrderFence& order_fence = order_fence_queue_.top();
            if (order_fence_queue_.top().order_num < order_num) {
                ensure_releases.push_back(order_fence);
                order_fence_queue_.pop();
                continue;
            }
            break;
        }
    }

    for (OrderFence& order_fence : ensure_releases) {
        order_fence.client_state->EnsureReleased(order_fence.fence_release);
    }
}

void SyncPointOrderData::FinishProcessingOrderNumber(uint32_t order_num)
{
    DCHECK(processing_thread_checker_.CalledOnValidThread());
    DCHECK_EQ(current_order_num_, order_num);

    // Catch invalid waits which were waiting on fence syncs that do not exist.
    // When we end processing an order number, we should release any fence syncs
    // which were suppose to be released during this order number.
    // Release without the lock to avoid possible deadlocks.
    std::vector<OrderFence> ensure_releases;
    {
        base::AutoLock auto_lock(lock_);
        DCHECK_GT(order_num, processed_order_num_);
        processed_order_num_ = order_num;

        while (!order_fence_queue_.empty()) {
            const OrderFence& order_fence = order_fence_queue_.top();
            if (order_fence_queue_.top().order_num <= order_num) {
                ensure_releases.push_back(order_fence);
                order_fence_queue_.pop();
                continue;
            }
            break;
        }
    }

    for (OrderFence& order_fence : ensure_releases) {
        order_fence.client_state->EnsureReleased(order_fence.fence_release);
    }
}

SyncPointOrderData::OrderFence::OrderFence(
    uint32_t order,
    uint64_t release,
    scoped_refptr<SyncPointClientState> state)
    : order_num(order)
    , fence_release(release)
    , client_state(state)
{
}

SyncPointOrderData::OrderFence::~OrderFence() { }

SyncPointOrderData::SyncPointOrderData()
    : current_order_num_(0)
    , destroyed_(false)
    , processed_order_num_(0)
    , unprocessed_order_num_(0)
{
}

SyncPointOrderData::~SyncPointOrderData() { }

bool SyncPointOrderData::ValidateReleaseOrderNumber(
    scoped_refptr<SyncPointClientState> client_state,
    uint32_t wait_order_num,
    uint64_t fence_release)
{
    base::AutoLock auto_lock(lock_);
    if (destroyed_)
        return false;

    // Release should have a possible unprocessed order number lower
    // than the wait order number.
    if ((processed_order_num_ + 1) >= wait_order_num)
        return false;

    // Release should have more unprocessed numbers if we are waiting.
    if (unprocessed_order_num_ <= processed_order_num_)
        return false;

    // So far it could be valid, but add an order fence guard to be sure it
    // gets released eventually.
    const uint32_t expected_order_num = std::min(unprocessed_order_num_, wait_order_num);
    order_fence_queue_.push(
        OrderFence(expected_order_num, fence_release, client_state));
    return true;
}

SyncPointClientState::ReleaseCallback::ReleaseCallback(
    uint64_t release,
    const base::Closure& callback)
    : release_count(release)
    , callback_closure(callback)
{
}

SyncPointClientState::ReleaseCallback::~ReleaseCallback() { }

SyncPointClientState::SyncPointClientState(
    scoped_refptr<SyncPointOrderData> order_data)
    : order_data_(order_data)
    , fence_sync_release_(0)
{
}

SyncPointClientState::~SyncPointClientState()
{
}

bool SyncPointClientState::WaitForRelease(uint32_t wait_order_num,
    uint64_t release,
    const base::Closure& callback)
{
    // Lock must be held the whole time while we validate otherwise it could be
    // released while we are checking.
    {
        base::AutoLock auto_lock(fence_sync_lock_);
        if (release > fence_sync_release_) {
            if (!order_data_->ValidateReleaseOrderNumber(this, wait_order_num,
                    release)) {
                return false;
            } else {
                // Add the callback which will be called upon release.
                release_callback_queue_.push(ReleaseCallback(release, callback));
                return true;
            }
        }
    }

    // Already released, run the callback now.
    callback.Run();
    return true;
}

void SyncPointClientState::ReleaseFenceSync(uint64_t release)
{
    // Call callbacks without the lock to avoid possible deadlocks.
    std::vector<base::Closure> callback_list;
    {
        base::AutoLock auto_lock(fence_sync_lock_);
        ReleaseFenceSyncLocked(release, &callback_list);
    }

    for (const base::Closure& closure : callback_list) {
        closure.Run();
    }
}

void SyncPointClientState::EnsureReleased(uint64_t release)
{
    // Call callbacks without the lock to avoid possible deadlocks.
    std::vector<base::Closure> callback_list;
    {
        base::AutoLock auto_lock(fence_sync_lock_);
        if (release <= fence_sync_release_)
            return;

        ReleaseFenceSyncLocked(release, &callback_list);
    }

    for (const base::Closure& closure : callback_list) {
        closure.Run();
    }
}

void SyncPointClientState::ReleaseFenceSyncLocked(
    uint64_t release,
    std::vector<base::Closure>* callback_list)
{
    fence_sync_lock_.AssertAcquired();
    DCHECK_GT(release, fence_sync_release_);

    fence_sync_release_ = release;
    while (!release_callback_queue_.empty() && release_callback_queue_.top().release_count <= release) {
        callback_list->push_back(release_callback_queue_.top().callback_closure);
        release_callback_queue_.pop();
    }
}

SyncPointClient::~SyncPointClient()
{
    // Release all fences on destruction.
    ReleaseFenceSync(UINT64_MAX);

    sync_point_manager_->DestroySyncPointClient(namespace_id_, client_id_);
}

bool SyncPointClient::Wait(SyncPointClientState* release_state,
    uint64_t release_count,
    const base::Closure& wait_complete_callback)
{
    const uint32_t wait_order_number = client_state_->order_data()->current_order_num();

    // If waiting on self or wait was invalid, call the callback and return false.
    if (client_state_ == release_state || !release_state->WaitForRelease(wait_order_number, release_count, wait_complete_callback)) {
        wait_complete_callback.Run();
        return false;
    }
    return true;
}

bool SyncPointClient::WaitNonThreadSafe(
    SyncPointClientState* release_state,
    uint64_t release_count,
    scoped_refptr<base::SingleThreadTaskRunner> runner,
    const base::Closure& wait_complete_callback)
{
    return Wait(release_state, release_count,
        base::Bind(&RunOnThread, runner, wait_complete_callback));
}

void SyncPointClient::ReleaseFenceSync(uint64_t release)
{
    client_state_->ReleaseFenceSync(release);
}

SyncPointClient::SyncPointClient(SyncPointManager* sync_point_manager,
    scoped_refptr<SyncPointOrderData> order_data,
    CommandBufferNamespace namespace_id,
    uint64_t client_id)
    : sync_point_manager_(sync_point_manager)
    , client_state_(new SyncPointClientState(order_data))
    , namespace_id_(namespace_id)
    , client_id_(client_id)
{
}

bool SyncPointClientWaiter::Wait(SyncPointClientState* release_state,
    uint64_t release_count,
    const base::Closure& wait_complete_callback)
{
    // No order number associated with the current execution context, using
    // UINT32_MAX will just assume the release is in the SyncPointClientState's
    // order numbers to be executed.
    if (!release_state->WaitForRelease(UINT32_MAX, release_count,
            wait_complete_callback)) {
        wait_complete_callback.Run();
        return false;
    }
    return true;
}

bool SyncPointClientWaiter::WaitNonThreadSafe(
    SyncPointClientState* release_state,
    uint64_t release_count,
    scoped_refptr<base::SingleThreadTaskRunner> runner,
    const base::Closure& wait_complete_callback)
{
    return Wait(release_state, release_count,
        base::Bind(&RunOnThread, runner, wait_complete_callback));
}

SyncPointManager::SyncPointManager(bool allow_threaded_wait)
    : allow_threaded_wait_(allow_threaded_wait)
    ,
    // To reduce the risk that a sync point created in a previous GPU process
    // will be in flight in the next GPU process, randomize the starting sync
    // point number. http://crbug.com/373452
    next_sync_point_(base::RandInt(1, kMaxSyncBase))
    , retire_cond_var_(&lock_)
{
    global_order_num_.GetNext();
}

SyncPointManager::~SyncPointManager()
{
    for (const ClientMap& client_map : client_maps_) {
        DCHECK(client_map.empty());
    }
}

scoped_ptr<SyncPointClient> SyncPointManager::CreateSyncPointClient(
    scoped_refptr<SyncPointOrderData> order_data,
    CommandBufferNamespace namespace_id,
    uint64_t client_id)
{
    DCHECK_GE(namespace_id, 0);
    DCHECK_LT(static_cast<size_t>(namespace_id), arraysize(client_maps_));
    base::AutoLock auto_lock(client_maps_lock_);

    ClientMap& client_map = client_maps_[namespace_id];
    std::pair<ClientMap::iterator, bool> result = client_map.insert(
        std::make_pair(client_id, new SyncPointClient(this, order_data, namespace_id, client_id)));
    DCHECK(result.second);

    return make_scoped_ptr(result.first->second);
}

scoped_refptr<SyncPointClientState> SyncPointManager::GetSyncPointClientState(
    CommandBufferNamespace namespace_id, uint64_t client_id)
{
    DCHECK_GE(namespace_id, 0);
    DCHECK_LT(static_cast<size_t>(namespace_id), arraysize(client_maps_));
    base::AutoLock auto_lock(client_maps_lock_);

    ClientMap& client_map = client_maps_[namespace_id];
    ClientMap::iterator it = client_map.find(client_id);
    if (it != client_map.end()) {
        return it->second->client_state();
    }
    return nullptr;
}

uint32 SyncPointManager::GenerateSyncPoint()
{
    base::AutoLock lock(lock_);
    uint32 sync_point = next_sync_point_++;
    // When an integer overflow occurs, don't return 0.
    if (!sync_point)
        sync_point = next_sync_point_++;

    // Note: wrapping would take days for a buggy/compromized renderer that would
    // insert sync points in a loop, but if that were to happen, better explicitly
    // crash the GPU process than risk worse.
    // For normal operation (at most a few per frame), it would take ~a year to
    // wrap.
    CHECK(sync_point_map_.find(sync_point) == sync_point_map_.end());
    sync_point_map_.insert(std::make_pair(sync_point, ClosureList()));
    return sync_point;
}

void SyncPointManager::RetireSyncPoint(uint32 sync_point)
{
    ClosureList list;
    {
        base::AutoLock lock(lock_);
        SyncPointMap::iterator it = sync_point_map_.find(sync_point);
        if (it == sync_point_map_.end()) {
            LOG(ERROR) << "Attempted to retire sync point that"
                          " didn't exist or was already retired.";
            return;
        }
        list.swap(it->second);
        sync_point_map_.erase(it);
        if (allow_threaded_wait_)
            retire_cond_var_.Broadcast();
    }
    for (ClosureList::iterator i = list.begin(); i != list.end(); ++i)
        i->Run();
}

void SyncPointManager::AddSyncPointCallback(uint32 sync_point,
    const base::Closure& callback)
{
    {
        base::AutoLock lock(lock_);
        SyncPointMap::iterator it = sync_point_map_.find(sync_point);
        if (it != sync_point_map_.end()) {
            it->second.push_back(callback);
            return;
        }
    }
    callback.Run();
}

bool SyncPointManager::IsSyncPointRetired(uint32 sync_point)
{
    base::AutoLock lock(lock_);
    return IsSyncPointRetiredLocked(sync_point);
}

void SyncPointManager::WaitSyncPoint(uint32 sync_point)
{
    if (!allow_threaded_wait_) {
        DCHECK(IsSyncPointRetired(sync_point));
        return;
    }

    base::AutoLock lock(lock_);
    while (!IsSyncPointRetiredLocked(sync_point)) {
        retire_cond_var_.Wait();
    }
}

bool SyncPointManager::IsSyncPointRetiredLocked(uint32 sync_point)
{
    lock_.AssertAcquired();
    return sync_point_map_.find(sync_point) == sync_point_map_.end();
}

uint32_t SyncPointManager::GenerateOrderNumber()
{
    return global_order_num_.GetNext();
}

void SyncPointManager::DestroySyncPointClient(
    CommandBufferNamespace namespace_id, uint64_t client_id)
{
    DCHECK_GE(namespace_id, 0);
    DCHECK_LT(static_cast<size_t>(namespace_id), arraysize(client_maps_));

    base::AutoLock auto_lock(client_maps_lock_);
    ClientMap& client_map = client_maps_[namespace_id];
    ClientMap::iterator it = client_map.find(client_id);
    DCHECK(it != client_map.end());
    client_map.erase(it);
}

} // namespace gpu
