// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RESOURCES_TEXTURE_MAILBOX_H_
#define CC_RESOURCES_TEXTURE_MAILBOX_H_

#include <string>

#include "base/memory/shared_memory.h"
#include "cc/base/cc_export.h"
#include "gpu/command_buffer/common/mailbox_holder.h"
#include "ui/gfx/geometry/size.h"

namespace cc {
class SharedBitmap;

// TODO(skaslev, danakj) Rename this class more apropriately since now it
// can hold a shared memory resource as well as a texture mailbox.
class CC_EXPORT TextureMailbox {
public:
    TextureMailbox();
    explicit TextureMailbox(const gpu::MailboxHolder& mailbox_holder);
    TextureMailbox(const gpu::Mailbox& mailbox, uint32 target, const gpu::SyncToken& sync_token);
    TextureMailbox(const gpu::Mailbox& mailbox,
        uint32 target,
        const gpu::SyncToken& sync_token,
        const gfx::Size& size_in_pixels,
        bool allow_overlay);
    TextureMailbox(SharedBitmap* shared_bitmap, const gfx::Size& size_in_pixels);

    ~TextureMailbox();

    bool IsValid() const { return IsTexture() || IsSharedMemory(); }
    bool IsTexture() const { return !mailbox_holder_.mailbox.IsZero(); }
    bool IsSharedMemory() const { return shared_bitmap_ != NULL; }

    bool Equals(const TextureMailbox&) const;

    const gpu::Mailbox& mailbox() const { return mailbox_holder_.mailbox; }
    const int8* name() const { return mailbox().name; }
    uint32 target() const { return mailbox_holder_.texture_target; }
    const gpu::SyncToken& sync_token() const
    {
        return mailbox_holder_.sync_token;
    }
    void set_sync_token(const gpu::SyncToken& sync_token)
    {
        mailbox_holder_.sync_token = sync_token;
    }

    bool allow_overlay() const { return allow_overlay_; }
    bool nearest_neighbor() const { return nearest_neighbor_; }
    void set_nearest_neighbor(bool nearest_neighbor)
    {
        nearest_neighbor_ = nearest_neighbor;
    }

    // This is valid if allow_overlau() or IsSharedMemory() is true.
    gfx::Size size_in_pixels() const { return size_in_pixels_; }

    SharedBitmap* shared_bitmap() const { return shared_bitmap_; }
    size_t SharedMemorySizeInBytes() const;

private:
    gpu::MailboxHolder mailbox_holder_;
    SharedBitmap* shared_bitmap_;
    gfx::Size size_in_pixels_;
    bool allow_overlay_;
    bool nearest_neighbor_;
};

} // namespace cc

#endif // CC_RESOURCES_TEXTURE_MAILBOX_H_
