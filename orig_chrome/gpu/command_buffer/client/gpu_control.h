// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_CLIENT_GPU_CONTROL_H_
#define GPU_COMMAND_BUFFER_CLIENT_GPU_CONTROL_H_

#include <stdint.h>

#include <vector>

#include "base/callback.h"
#include "base/macros.h"
#include "gpu/command_buffer/common/capabilities.h"
#include "gpu/command_buffer/common/constants.h"
#include "gpu/command_buffer/common/mailbox.h"
#include "gpu/gpu_export.h"

extern "C" typedef struct _ClientBuffer* ClientBuffer;

namespace base {
class Lock;
}

namespace gfx {
class GpuMemoryBuffer;
}

namespace gpu {
struct SyncToken;

// Common interface for GpuControl implementations.
class GPU_EXPORT GpuControl {
public:
    GpuControl() { }
    virtual ~GpuControl() { }

    virtual Capabilities GetCapabilities() = 0;

    // Create an image for a client buffer with the given dimensions and
    // format. Returns its ID or -1 on error.
    virtual int32_t CreateImage(ClientBuffer buffer,
        size_t width,
        size_t height,
        unsigned internalformat)
        = 0;

    // Destroy an image. The ID must be positive.
    virtual void DestroyImage(int32_t id) = 0;

    // Create a gpu memory buffer backed image with the given dimensions and
    // format for |usage|. Returns its ID or -1 on error.
    virtual int32_t CreateGpuMemoryBufferImage(size_t width,
        size_t height,
        unsigned internalformat,
        unsigned usage)
        = 0;

    // Inserts a sync point, returning its ID. Sync point IDs are global and can
    // be used for cross-context synchronization.
    virtual uint32_t InsertSyncPoint() = 0;

    // Inserts a future sync point, returning its ID. Sync point IDs are global
    // and can be used for cross-context synchronization. The sync point won't be
    // retired immediately.
    virtual uint32_t InsertFutureSyncPoint() = 0;

    // Retires a future sync point. This will signal contexts that are waiting
    // on it to start executing.
    virtual void RetireSyncPoint(uint32_t sync_point) = 0;

    // Runs |callback| when a sync point is reached.
    virtual void SignalSyncPoint(uint32_t sync_point,
        const base::Closure& callback)
        = 0;

    // Runs |callback| when a query created via glCreateQueryEXT() has cleared
    // passed the glEndQueryEXT() point.
    virtual void SignalQuery(uint32_t query, const base::Closure& callback) = 0;

    virtual void SetSurfaceVisible(bool visible) = 0;

    // Attaches an external stream to the texture given by |texture_id| and
    // returns a stream identifier.
    virtual uint32_t CreateStreamTexture(uint32_t texture_id) = 0;

    // Sets a lock this will be held on every callback from the GPU
    // implementation. This lock must be set and must be held on every call into
    // the GPU implementation if it is to be used from multiple threads. This
    // may not be supported with all implementations.
    virtual void SetLock(base::Lock*) = 0;

    // Returns true if the channel to the Gpu is lost. When true, all contexts
    // should be considered as lost.
    virtual bool IsGpuChannelLost() = 0;

    // The namespace and command buffer ID forms a unique pair for all existing
    // GpuControl (on client) and matches for the corresponding command buffer
    // (on server) in a single server process.
    virtual CommandBufferNamespace GetNamespaceID() const = 0;
    virtual uint64_t GetCommandBufferID() const = 0;

    // Fence Syncs use release counters at a context level, these fence syncs
    // need to be flushed before they can be shared with other contexts across
    // channels. Subclasses should implement these functions and take care of
    // figuring out when a fence sync has been flushed. The difference between
    // IsFenceSyncFlushed and IsFenceSyncFlushReceived, one is testing is the
    // client has issued the flush, and the other is testing if the service
    // has received the flush.
    virtual uint64_t GenerateFenceSyncRelease() = 0;
    virtual bool IsFenceSyncRelease(uint64_t release) = 0;
    virtual bool IsFenceSyncFlushed(uint64_t release) = 0;
    virtual bool IsFenceSyncFlushReceived(uint64_t release) = 0;

    // Runs |callback| when sync token is signalled.
    virtual void SignalSyncToken(const SyncToken& sync_token,
        const base::Closure& callback)
        = 0;

    // Under some circumstances a sync token may be used which has not been
    // verified to have been flushed. For example, fence syncs queued on the
    // same channel as the wait command guarantee that the fence sync will
    // be enqueued first so does not need to be flushed.
    virtual bool CanWaitUnverifiedSyncToken(const SyncToken* sync_token) = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(GpuControl);
};

} // namespace gpu

#endif // GPU_COMMAND_BUFFER_CLIENT_GPU_CONTROL_H_
