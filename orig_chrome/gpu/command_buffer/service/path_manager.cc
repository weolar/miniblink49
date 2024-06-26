// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/path_manager.h"

#include <algorithm>

#include "base/logging.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"
#include "ui/gl/gl_bindings.h"

namespace gpu {
namespace gles2 {

    namespace {
        void CallDeletePaths(GLuint first_id, GLuint range)
        {
            while (range > 0) {
                GLsizei irange;
                if (range > static_cast<GLuint>(std::numeric_limits<GLsizei>::max()))
                    irange = std::numeric_limits<GLsizei>::max();
                else
                    irange = static_cast<GLsizei>(range);

                glDeletePathsNV(first_id, irange);
                range -= irange;
                first_id += irange;
            }
        }

        template <typename RangeIterator>
        static GLuint RangeSize(const RangeIterator& it)
        {
            return it->second.last_client_id - it->first + 1;
        }
        template <typename RangeIterator>
        static GLuint FirstClientId(const RangeIterator& it)
        {
            return it->first;
        }
        template <typename RangeIterator>
        static GLuint FirstServiceId(const RangeIterator& it)
        {
            return it->second.first_service_id;
        }
        template <typename RangeIterator>
        static GLuint LastServiceId(const RangeIterator& it)
        {
            return FirstServiceId(it) + RangeSize(it) - 1;
        }
        static GLuint LastClientId(PathManager::PathRangeMap::const_iterator& it)
        {
            return it->second.last_client_id;
        }
        // Note: this one can be assigned to.
        static GLuint& LastClientId(PathManager::PathRangeMap::iterator& it)
        {
            return it->second.last_client_id;
        }

        template <typename T>
        struct IteratorSelector {
            typedef typename T::iterator iterator;
        };
        template <typename T>
        struct IteratorSelector<const T> {
            typedef typename T::const_iterator iterator;
        };

        // Returns the range position that contains |client_id| or
        // |PathRangeMap::iterator::end()| if |client_id| is not found.
        template <typename MapType>
        static typename IteratorSelector<MapType>::iterator GetContainingRange(
            MapType& path_map,
            GLuint client_id)
        {
            auto it = path_map.lower_bound(client_id);
            if (it != path_map.end() && FirstClientId(it) == client_id)
                return it;
            if (it != path_map.begin()) {
                --it;
                if (LastClientId(it) >= client_id)
                    return it;
            }
            return path_map.end();
        }

        // Returns the range position that contains |client_id|. If that is
        // not available, returns the range that has smallest
        // |first_client_id| that is bigger than |client_id|. Returns
        // |PathRangeMap::iterator::end()| if there is no such range.
        template <typename MapType>
        static typename IteratorSelector<MapType>::iterator GetContainingOrNextRange(
            MapType& path_map,
            GLuint client_id)
        {
            auto it = path_map.lower_bound(client_id);
            if (it != path_map.end() && FirstClientId(it) == client_id) {
                return it;
            }
            if (it != path_map.begin()) {
                --it;
                if (LastClientId(it) >= client_id)
                    return it;
                ++it;
            }
            return it;
        }

    } // anonymous namespace

    PathManager::PathManager()
    {
    }

    PathManager::~PathManager()
    {
        DCHECK(path_map_.empty());
    }

    void PathManager::Destroy(bool have_context)
    {
        if (have_context) {
            for (PathRangeMap::const_iterator it = path_map_.begin();
                 it != path_map_.end(); ++it)
                CallDeletePaths(FirstServiceId(it), RangeSize(it));
        }
        path_map_.clear();
    }

    void PathManager::CreatePathRange(GLuint first_client_id,
        GLuint last_client_id,
        GLuint first_service_id)
    {
        DCHECK(first_service_id > 0u);
        DCHECK(first_client_id > 0u);
        DCHECK(!HasPathsInRange(first_client_id, last_client_id));
        DCHECK(CheckConsistency());

        PathRangeMap::iterator range = GetContainingRange(path_map_, first_client_id - 1u);

        if (range != path_map_.end() && LastServiceId(range) == first_service_id - 1u) {
            DCHECK_EQ(LastClientId(range), first_client_id - 1u);
            LastClientId(range) = last_client_id;
        } else {
            auto result = path_map_.insert(
                std::make_pair(first_client_id,
                    PathRangeDescription(last_client_id, first_service_id)));
            DCHECK(result.second);
            range = result.first;
        }

        PathRangeMap::iterator next_range = range;
        ++next_range;
        if (next_range != path_map_.end()) {
            if (LastClientId(range) == FirstClientId(next_range) - 1u && LastServiceId(range) == FirstServiceId(next_range) - 1u) {
                LastClientId(range) = LastClientId(next_range);
                path_map_.erase(next_range);
            }
        }
        DCHECK(CheckConsistency());
    }

    bool PathManager::HasPathsInRange(GLuint first_client_id,
        GLuint last_client_id) const
    {
        PathRangeMap::const_iterator it = GetContainingOrNextRange(path_map_, first_client_id);
        if (it == path_map_.end())
            return false;

        return FirstClientId(it) <= last_client_id;
    }

    bool PathManager::GetPath(GLuint client_id, GLuint* service_id) const
    {
        PathRangeMap::const_iterator range = GetContainingRange(path_map_, client_id);
        if (range == path_map_.end())
            return false;

        *service_id = FirstServiceId(range) + client_id - FirstClientId(range);
        return true;
    }

    void PathManager::RemovePaths(GLuint first_client_id, GLuint last_client_id)
    {
        DCHECK(CheckConsistency());
        PathRangeMap::iterator it = GetContainingOrNextRange(path_map_, first_client_id);

        while (it != path_map_.end() && FirstClientId(it) <= last_client_id) {
            GLuint delete_first_client_id = std::max(first_client_id, FirstClientId(it));
            GLuint delete_last_client_id = std::min(last_client_id, LastClientId(it));
            GLuint delete_first_service_id = FirstServiceId(it) + delete_first_client_id - FirstClientId(it);
            GLuint delete_range = delete_last_client_id - delete_first_client_id + 1u;

            CallDeletePaths(delete_first_service_id, delete_range);

            PathRangeMap::iterator current = it;
            ++it;

            GLuint current_last_client_id = LastClientId(current);

            if (FirstClientId(current) < delete_first_client_id)
                LastClientId(current) = delete_first_client_id - 1u;
            else
                path_map_.erase(current);

            if (current_last_client_id > delete_last_client_id) {
                path_map_.insert(std::make_pair(
                    delete_last_client_id + 1u,
                    PathRangeDescription(current_last_client_id,
                        delete_first_service_id + delete_range)));
                DCHECK(delete_last_client_id == last_client_id);
                // This is necessarily the last range to check. Return early due to
                // consistency. Iterator increment would skip the inserted range. The
                // algorithm would work ok, but it looks weird.
                DCHECK(CheckConsistency());
                return;
            }
        }
        DCHECK(CheckConsistency());
    }

    bool PathManager::CheckConsistency()
    {
        GLuint prev_first_client_id = 0u;
        GLuint prev_last_client_id = 0u;
        GLuint prev_first_service_id = 0u;
        for (PathRangeMap::iterator range = path_map_.begin();
             range != path_map_.end(); ++range) {
            // Code relies on ranges not starting at 0. Also, the above initialization
            // is only
            // correct then.
            if (FirstClientId(range) == 0u || FirstServiceId(range) == 0u)
                return false;

            // Each range is consistent.
            if (FirstClientId(range) > LastClientId(range))
                return false;

            if (prev_first_client_id != 0u) {
                // No overlapping ranges. (The iteration is sorted).
                if (FirstClientId(range) <= prev_last_client_id)
                    return false;

                // No mergeable ranges.
                bool is_mergeable_client = FirstClientId(range) - 1u == prev_last_client_id;
                bool is_mergeable_service = FirstServiceId(range) - 1u == prev_first_service_id;
                if (is_mergeable_client && is_mergeable_service)
                    return false;
            }
            prev_first_client_id = FirstClientId(range);
            prev_last_client_id = LastClientId(range);
            prev_first_service_id = FirstServiceId(range);
        }
        return true;
    }

} // namespace gles2
} // namespace gpu
