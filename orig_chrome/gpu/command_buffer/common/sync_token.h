// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_COMMON_SYNC_TOKEN_H_
#define GPU_COMMAND_BUFFER_COMMON_SYNC_TOKEN_H_

#include <stdint.h>
#include <string.h>

#include "gpu/command_buffer/common/constants.h"
#include "gpu/gpu_export.h"

// From glextchromium.h.
#ifndef GL_SYNC_TOKEN_SIZE_CHROMIUM
#define GL_SYNC_TOKEN_SIZE_CHROMIUM 24
#endif

namespace gpu {

// A Sync Token is a binary blob which represents a waitable fence sync
// on a particular command buffer namespace and id.
// See src/gpu/GLES2/extensions/CHROMIUM/CHROMIUM_sync_point.txt for more
// details.
struct GPU_EXPORT SyncToken {
    SyncToken()
        : verified_flush_(false)
        , namespace_id_(CommandBufferNamespace::INVALID)
        , command_buffer_id_(0)
        , release_count_(0)
    {
    }

    // TODO(dyen): This is an intermediate conversion constructor while we
    // are converting from the old sync point system. Remove once conversion
    // is finished.
    explicit SyncToken(uint32_t sync_point)
        : verified_flush_(sync_point ? true : false)
        , namespace_id_(sync_point ? gpu::CommandBufferNamespace::OLD_SYNC_POINTS
                                   : gpu::CommandBufferNamespace::INVALID)
        , command_buffer_id_(0)
        , release_count_(sync_point)
    {
    }

    SyncToken(CommandBufferNamespace namespace_id,
        uint64_t command_buffer_id,
        uint64_t release_count)
        : verified_flush_(false)
        , namespace_id_(namespace_id)
        , command_buffer_id_(command_buffer_id)
        , release_count_(release_count)
    {
    }

    void Set(CommandBufferNamespace namespace_id,
        uint64_t command_buffer_id,
        uint64_t release_count)
    {
        namespace_id_ = namespace_id;
        command_buffer_id_ = command_buffer_id;
        release_count_ = release_count;
    }

    void Clear()
    {
        verified_flush_ = false;
        namespace_id_ = CommandBufferNamespace::INVALID;
        command_buffer_id_ = 0;
        release_count_ = 0;
    }

    void SetVerifyFlush()
    {
        verified_flush_ = true;
    }

    bool HasData() const
    {
        return namespace_id_ != CommandBufferNamespace::INVALID;
    }

    int8_t* GetData() { return reinterpret_cast<int8_t*>(this); }

    const int8_t* GetConstData() const
    {
        return reinterpret_cast<const int8_t*>(this);
    }

    bool verified_flush() const { return verified_flush_; }
    CommandBufferNamespace namespace_id() const { return namespace_id_; }
    uint64_t command_buffer_id() const { return command_buffer_id_; }
    uint64_t release_count() const { return release_count_; }

    bool operator<(const SyncToken& other) const
    {
        // TODO(dyen): Once all our compilers support c++11, we can replace this
        // long list of comparisons with std::tie().
        return (namespace_id_ < other.namespace_id()) || ((namespace_id_ == other.namespace_id()) && ((command_buffer_id_ < other.command_buffer_id()) || ((command_buffer_id_ == other.command_buffer_id()) && (release_count_ < other.release_count()))));
    }

    bool operator==(const SyncToken& other) const
    {
        return verified_flush_ == other.verified_flush() && namespace_id_ == other.namespace_id() && command_buffer_id_ == other.command_buffer_id() && release_count_ == other.release_count();
    }

    bool operator!=(const SyncToken& other) const { return !(*this == other); }

private:
    bool verified_flush_;
    CommandBufferNamespace namespace_id_;
    uint64_t command_buffer_id_;
    uint64_t release_count_;
};

static_assert(sizeof(SyncToken) <= GL_SYNC_TOKEN_SIZE_CHROMIUM,
    "SyncToken size must not exceed GL_SYNC_TOKEN_SIZE_CHROMIUM");

} // namespace gpu

#endif // GPU_COMMAND_BUFFER_COMMON_SYNC_TOKEN_H_
