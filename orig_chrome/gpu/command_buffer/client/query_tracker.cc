// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/client/query_tracker.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2extchromium.h>

#include <limits.h>

#include "base/atomicops.h"
#include "base/numerics/safe_conversions.h"
#include "gpu/command_buffer/client/gles2_cmd_helper.h"
#include "gpu/command_buffer/client/gles2_implementation.h"
#include "gpu/command_buffer/client/mapped_memory.h"
#include "gpu/command_buffer/common/time.h"

namespace gpu {
namespace gles2 {

    QuerySyncManager::Bucket::Bucket(QuerySync* sync_mem,
        int32 shm_id,
        unsigned int shm_offset)
        : syncs(sync_mem)
        , shm_id(shm_id)
        , base_shm_offset(shm_offset)
    {
    }

    QuerySyncManager::Bucket::~Bucket() = default;

    QuerySyncManager::QuerySyncManager(MappedMemoryManager* manager)
        : mapped_memory_(manager)
    {
        DCHECK(manager);
    }

    QuerySyncManager::~QuerySyncManager()
    {
        while (!buckets_.empty()) {
            mapped_memory_->Free(buckets_.front()->syncs);
            delete buckets_.front();
            buckets_.pop_front();
        }
    }

    bool QuerySyncManager::Alloc(QuerySyncManager::QueryInfo* info)
    {
        DCHECK(info);
        Bucket* bucket = nullptr;
        for (Bucket* bucket_candidate : buckets_) {
            // In C++11 STL this could be replaced with
            // if (!bucket_candidate->in_use_queries.all()) { ... }
            if (bucket_candidate->in_use_queries.count() != kSyncsPerBucket) {
                bucket = bucket_candidate;
                break;
            }
        }
        if (!bucket) {
            int32 shm_id;
            unsigned int shm_offset;
            void* mem = mapped_memory_->Alloc(
                kSyncsPerBucket * sizeof(QuerySync), &shm_id, &shm_offset);
            if (!mem) {
                return false;
            }
            QuerySync* syncs = static_cast<QuerySync*>(mem);
            bucket = new Bucket(syncs, shm_id, shm_offset);
            buckets_.push_back(bucket);
        }

        unsigned short index_in_bucket = 0;
        for (size_t i = 0; i < kSyncsPerBucket; i++) {
            if (!bucket->in_use_queries[i]) {
                index_in_bucket = i;
                break;
            }
        }

        uint32 shm_offset = bucket->base_shm_offset + index_in_bucket * sizeof(QuerySync);
        QuerySync* sync = bucket->syncs + index_in_bucket;
        *info = QueryInfo(bucket, bucket->shm_id, shm_offset, sync);
        info->sync->Reset();
        bucket->in_use_queries[index_in_bucket] = true;
        return true;
    }

    void QuerySyncManager::Free(const QuerySyncManager::QueryInfo& info)
    {
        DCHECK_NE(info.bucket->in_use_queries.count(), 0u);
        unsigned short index_in_bucket = info.sync - info.bucket->syncs;
        DCHECK(info.bucket->in_use_queries[index_in_bucket]);
        info.bucket->in_use_queries[index_in_bucket] = false;
    }

    void QuerySyncManager::Shrink()
    {
        std::deque<Bucket*> new_buckets;
        while (!buckets_.empty()) {
            Bucket* bucket = buckets_.front();
            if (bucket->in_use_queries.any()) {
                new_buckets.push_back(bucket);
            } else {
                mapped_memory_->Free(bucket->syncs);
                delete bucket;
            }
            buckets_.pop_front();
        }
        buckets_.swap(new_buckets);
    }

    QueryTracker::Query::Query(GLuint id, GLenum target,
        const QuerySyncManager::QueryInfo& info)
        : id_(id)
        , target_(target)
        , info_(info)
        , state_(kUninitialized)
        , submit_count_(0)
        , token_(0)
        , flush_count_(0)
        , client_begin_time_us_(0)
        , result_(0)
    {
    }

    void QueryTracker::Query::Begin(GLES2Implementation* gl)
    {
        // init memory, inc count
        MarkAsActive();

        switch (target()) {
        case GL_GET_ERROR_QUERY_CHROMIUM:
            // To nothing on begin for error queries.
            break;
        case GL_LATENCY_QUERY_CHROMIUM:
            client_begin_time_us_ = MicrosecondsSinceOriginOfTime();
            // tell service about id, shared memory and count
            gl->helper()->BeginQueryEXT(target(), id(), shm_id(), shm_offset());
            break;
        case GL_ASYNC_PIXEL_PACK_COMPLETED_CHROMIUM:
        default:
            // tell service about id, shared memory and count
            gl->helper()->BeginQueryEXT(target(), id(), shm_id(), shm_offset());
            break;
        }
    }

    void QueryTracker::Query::End(GLES2Implementation* gl)
    {
        switch (target()) {
        case GL_GET_ERROR_QUERY_CHROMIUM: {
            GLenum error = gl->GetClientSideGLError();
            if (error == GL_NO_ERROR) {
                // There was no error so start the query on the service.
                // it will end immediately.
                gl->helper()->BeginQueryEXT(target(), id(), shm_id(), shm_offset());
            } else {
                // There's an error on the client, no need to bother the service. Just
                // set the query as completed and return the error.
                if (error != GL_NO_ERROR) {
                    state_ = kComplete;
                    result_ = error;
                    return;
                }
            }
        }
        }
        flush_count_ = gl->helper()->flush_generation();
        gl->helper()->EndQueryEXT(target(), submit_count());
        MarkAsPending(gl->helper()->InsertToken());
    }

    void QueryTracker::Query::QueryCounter(GLES2Implementation* gl)
    {
        MarkAsActive();
        flush_count_ = gl->helper()->flush_generation();
        gl->helper()->QueryCounterEXT(id(), target(), shm_id(), shm_offset(),
            submit_count());
        MarkAsPending(gl->helper()->InsertToken());
    }

    bool QueryTracker::Query::CheckResultsAvailable(
        CommandBufferHelper* helper)
    {
        if (Pending()) {
            if (base::subtle::Acquire_Load(&info_.sync->process_count) == submit_count_ || helper->IsContextLost()) {
                switch (target()) {
                case GL_COMMANDS_ISSUED_CHROMIUM:
                    result_ = info_.sync->result;
                    break;
                case GL_LATENCY_QUERY_CHROMIUM:
                    // Disabled DCHECK because of http://crbug.com/419236.
                    //DCHECK(info_.sync->result >= client_begin_time_us_);
                    result_ = info_.sync->result - client_begin_time_us_;
                    break;
                case GL_ASYNC_PIXEL_PACK_COMPLETED_CHROMIUM:
                default:
                    result_ = info_.sync->result;
                    break;
                }
                state_ = kComplete;
            } else {
                if ((helper->flush_generation() - flush_count_ - 1) >= 0x80000000) {
                    helper->Flush();
                } else {
                    // Insert no-ops so that eventually the GPU process will see more work.
                    helper->Noop(1);
                }
            }
        }
        return state_ == kComplete;
    }

    uint64 QueryTracker::Query::GetResult() const
    {
        DCHECK(state_ == kComplete || state_ == kUninitialized);
        return result_;
    }

    QueryTracker::QueryTracker(MappedMemoryManager* manager)
        : query_sync_manager_(manager)
        , mapped_memory_(manager)
        , disjoint_count_sync_shm_id_(-1)
        , disjoint_count_sync_shm_offset_(0)
        , disjoint_count_sync_(nullptr)
        , local_disjoint_count_(0)
    {
    }

    QueryTracker::~QueryTracker()
    {
        while (!queries_.empty()) {
            delete queries_.begin()->second;
            queries_.erase(queries_.begin());
        }
        while (!removed_queries_.empty()) {
            delete removed_queries_.front();
            removed_queries_.pop_front();
        }
        if (disjoint_count_sync_) {
            mapped_memory_->Free(disjoint_count_sync_);
            disjoint_count_sync_ = nullptr;
        }
    }

    QueryTracker::Query* QueryTracker::CreateQuery(GLuint id, GLenum target)
    {
        DCHECK_NE(0u, id);
        FreeCompletedQueries();
        QuerySyncManager::QueryInfo info;
        if (!query_sync_manager_.Alloc(&info)) {
            return nullptr;
        }
        Query* query = new Query(id, target, info);
        std::pair<QueryIdMap::iterator, bool> result = queries_.insert(std::make_pair(id, query));
        DCHECK(result.second);
        return query;
    }

    QueryTracker::Query* QueryTracker::GetQuery(GLuint client_id)
    {
        QueryIdMap::iterator it = queries_.find(client_id);
        return it != queries_.end() ? it->second : nullptr;
    }

    QueryTracker::Query* QueryTracker::GetCurrentQuery(GLenum target)
    {
        QueryTargetMap::iterator it = current_queries_.find(target);
        return it != current_queries_.end() ? it->second : nullptr;
    }

    void QueryTracker::RemoveQuery(GLuint client_id)
    {
        QueryIdMap::iterator it = queries_.find(client_id);
        if (it != queries_.end()) {
            Query* query = it->second;

            // Erase from current targets map if it is the current target.
            const GLenum target = query->target();
            QueryTargetMap::iterator target_it = current_queries_.find(target);
            if (target_it != current_queries_.end() && target_it->second == query) {
                current_queries_.erase(target_it);
            }

            // When you delete a query you can't mark its memory as unused until it's
            // completed.
            // Note: If you don't do this you won't mess up the service but you will
            // mess up yourself.
            removed_queries_.push_back(query);
            queries_.erase(it);
            FreeCompletedQueries();
        }
    }

    void QueryTracker::Shrink()
    {
        FreeCompletedQueries();
        query_sync_manager_.Shrink();
    }

    void QueryTracker::FreeCompletedQueries()
    {
        QueryList::iterator it = removed_queries_.begin();
        while (it != removed_queries_.end()) {
            Query* query = *it;
            if (query->Pending() && base::subtle::Acquire_Load(&query->info_.sync->process_count) != query->submit_count()) {
                ++it;
                continue;
            }

            query_sync_manager_.Free(query->info_);
            it = removed_queries_.erase(it);
            delete query;
        }
    }

    bool QueryTracker::BeginQuery(GLuint id, GLenum target,
        GLES2Implementation* gl)
    {
        QueryTracker::Query* query = GetQuery(id);
        if (!query) {
            query = CreateQuery(id, target);
            if (!query) {
                gl->SetGLError(GL_OUT_OF_MEMORY,
                    "glBeginQueryEXT",
                    "transfer buffer allocation failed");
                return false;
            }
        } else if (query->target() != target) {
            gl->SetGLError(GL_INVALID_OPERATION,
                "glBeginQueryEXT",
                "target does not match");
            return false;
        }

        current_queries_[query->target()] = query;
        query->Begin(gl);
        return true;
    }

    bool QueryTracker::EndQuery(GLenum target, GLES2Implementation* gl)
    {
        QueryTargetMap::iterator target_it = current_queries_.find(target);
        if (target_it == current_queries_.end()) {
            gl->SetGLError(GL_INVALID_OPERATION,
                "glEndQueryEXT", "no active query");
            return false;
        }

        target_it->second->End(gl);
        current_queries_.erase(target_it);
        return true;
    }

    bool QueryTracker::QueryCounter(GLuint id, GLenum target,
        GLES2Implementation* gl)
    {
        QueryTracker::Query* query = GetQuery(id);
        if (!query) {
            query = CreateQuery(id, target);
            if (!query) {
                gl->SetGLError(GL_OUT_OF_MEMORY,
                    "glQueryCounterEXT",
                    "transfer buffer allocation failed");
                return false;
            }
        } else if (query->target() != target) {
            gl->SetGLError(GL_INVALID_OPERATION,
                "glQueryCounterEXT",
                "target does not match");
            return false;
        }

        query->QueryCounter(gl);
        return true;
    }

    bool QueryTracker::SetDisjointSync(GLES2Implementation* gl)
    {
        if (!disjoint_count_sync_) {
            // Allocate memory for disjoint value sync.
            int32_t shm_id = -1;
            uint32_t shm_offset;
            void* mem = mapped_memory_->Alloc(sizeof(*disjoint_count_sync_),
                &shm_id,
                &shm_offset);
            if (mem) {
                disjoint_count_sync_shm_id_ = shm_id;
                disjoint_count_sync_shm_offset_ = shm_offset;
                disjoint_count_sync_ = static_cast<DisjointValueSync*>(mem);
                disjoint_count_sync_->Reset();
                gl->helper()->SetDisjointValueSyncCHROMIUM(shm_id, shm_offset);
            }
        }
        return disjoint_count_sync_ != nullptr;
    }

    bool QueryTracker::CheckAndResetDisjoint()
    {
        if (disjoint_count_sync_) {
            const uint32_t disjoint_count = disjoint_count_sync_->GetDisjointCount();
            if (local_disjoint_count_ != disjoint_count) {
                local_disjoint_count_ = disjoint_count;
                return true;
            }
        }
        return false;
    }

} // namespace gles2
} // namespace gpu
