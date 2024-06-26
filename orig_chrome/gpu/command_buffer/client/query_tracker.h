// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_CLIENT_QUERY_TRACKER_H_
#define GPU_COMMAND_BUFFER_CLIENT_QUERY_TRACKER_H_

#include <GLES2/gl2.h>

#include <bitset>
#include <deque>
#include <list>

#include "base/atomicops.h"
#include "base/containers/hash_tables.h"
#include "gles2_impl_export.h"
#include "gpu/command_buffer/common/gles2_cmd_format.h"

namespace gpu {

class CommandBufferHelper;
class MappedMemoryManager;

namespace gles2 {

    class GLES2Implementation;

    // Manages buckets of QuerySync instances in mapped memory.
    class GLES2_IMPL_EXPORT QuerySyncManager {
    public:
        static const size_t kSyncsPerBucket = 256;

        struct Bucket {
            Bucket(QuerySync* sync_mem, int32 shm_id, uint32 shm_offset);
            ~Bucket();
            QuerySync* syncs;
            int32 shm_id;
            uint32 base_shm_offset;
            std::bitset<kSyncsPerBucket> in_use_queries;
        };
        struct QueryInfo {
            QueryInfo(Bucket* bucket, int32 id, uint32 offset, QuerySync* sync_mem)
                : bucket(bucket)
                , shm_id(id)
                , shm_offset(offset)
                , sync(sync_mem)
            {
            }

            QueryInfo()
                : bucket(NULL)
                , shm_id(0)
                , shm_offset(0)
                , sync(NULL)
            {
            }

            Bucket* bucket;
            int32 shm_id;
            uint32 shm_offset;
            QuerySync* sync;
        };

        explicit QuerySyncManager(MappedMemoryManager* manager);
        ~QuerySyncManager();

        bool Alloc(QueryInfo* info);
        void Free(const QueryInfo& sync);
        void Shrink();

    private:
        MappedMemoryManager* mapped_memory_;
        std::deque<Bucket*> buckets_;

        DISALLOW_COPY_AND_ASSIGN(QuerySyncManager);
    };

    // Tracks queries for client side of command buffer.
    class GLES2_IMPL_EXPORT QueryTracker {
    public:
        class GLES2_IMPL_EXPORT Query {
        public:
            enum State {
                kUninitialized, // never used
                kActive, // between begin - end
                kPending, // not yet complete
                kComplete // completed
            };

            Query(GLuint id, GLenum target, const QuerySyncManager::QueryInfo& info);

            GLenum target() const
            {
                return target_;
            }

            GLuint id() const
            {
                return id_;
            }

            int32 shm_id() const
            {
                return info_.shm_id;
            }

            uint32 shm_offset() const
            {
                return info_.shm_offset;
            }

            void MarkAsActive()
            {
                state_ = kActive;
                ++submit_count_;
                if (submit_count_ == INT_MAX)
                    submit_count_ = 1;
            }

            void MarkAsPending(int32 token)
            {
                token_ = token;
                state_ = kPending;
            }

            base::subtle::Atomic32 submit_count() const { return submit_count_; }

            int32 token() const
            {
                return token_;
            }

            bool NeverUsed() const
            {
                return state_ == kUninitialized;
            }

            bool Active() const
            {
                return state_ == kActive;
            }

            bool Pending() const
            {
                return state_ == kPending;
            }

            bool CheckResultsAvailable(CommandBufferHelper* helper);

            uint64 GetResult() const;

        private:
            friend class QueryTracker;
            friend class QueryTrackerTest;

            void Begin(GLES2Implementation* gl);
            void End(GLES2Implementation* gl);
            void QueryCounter(GLES2Implementation* gl);

            GLuint id_;
            GLenum target_;
            QuerySyncManager::QueryInfo info_;
            State state_;
            base::subtle::Atomic32 submit_count_;
            int32 token_;
            uint32 flush_count_;
            uint64 client_begin_time_us_; // Only used for latency query target.
            uint64 result_;
        };

        QueryTracker(MappedMemoryManager* manager);
        ~QueryTracker();

        Query* CreateQuery(GLuint id, GLenum target);
        Query* GetQuery(GLuint id);
        Query* GetCurrentQuery(GLenum target);
        void RemoveQuery(GLuint id);
        void Shrink();
        void FreeCompletedQueries();

        bool BeginQuery(GLuint id, GLenum target, GLES2Implementation* gl);
        bool EndQuery(GLenum target, GLES2Implementation* gl);
        bool QueryCounter(GLuint id, GLenum target, GLES2Implementation* gl);
        bool SetDisjointSync(GLES2Implementation* gl);
        bool CheckAndResetDisjoint();

        int32_t DisjointCountSyncShmID() const
        {
            return disjoint_count_sync_shm_id_;
        }

        uint32_t DisjointCountSyncShmOffset() const
        {
            return disjoint_count_sync_shm_offset_;
        }

    private:
        typedef base::hash_map<GLuint, Query*> QueryIdMap;
        typedef base::hash_map<GLenum, Query*> QueryTargetMap;
        typedef std::list<Query*> QueryList;

        QueryIdMap queries_;
        QueryTargetMap current_queries_;
        QueryList removed_queries_;
        QuerySyncManager query_sync_manager_;

        // The shared memory used for synchronizing timer disjoint values.
        MappedMemoryManager* mapped_memory_;
        int32_t disjoint_count_sync_shm_id_;
        uint32_t disjoint_count_sync_shm_offset_;
        DisjointValueSync* disjoint_count_sync_;
        uint32_t local_disjoint_count_;

        DISALLOW_COPY_AND_ASSIGN(QueryTracker);
    };

} // namespace gles2
} // namespace gpu

#endif // GPU_COMMAND_BUFFER_CLIENT_QUERY_TRACKER_H_
