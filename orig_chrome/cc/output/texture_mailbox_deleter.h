// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_TEXTURE_MAILBOX_DELETER_H_
#define CC_OUTPUT_TEXTURE_MAILBOX_DELETER_H_

#include "base/memory/weak_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/base/scoped_ptr_vector.h"

namespace base {
class SingleThreadTaskRunner;
}

namespace gpu {
struct SyncToken;
} // namespace gpu

namespace cc {
class ContextProvider;
class SingleReleaseCallback;

class CC_EXPORT TextureMailboxDeleter {
public:
    // task_runner corresponds with the thread the delete task should be posted
    // to. If null, the delete will happen on the calling thread.
    explicit TextureMailboxDeleter(
        const scoped_refptr<base::SingleThreadTaskRunner>& task_runner);
    ~TextureMailboxDeleter();

    // Returns a Callback that can be used as the ReleaseCallback for a
    // TextureMailbox attached to the |texture_id|. The ReleaseCallback can
    // be passed to other threads and will destroy the texture, once it is
    // run, on the impl thread. If the TextureMailboxDeleter is destroyed
    // due to the compositor shutting down, then the ReleaseCallback will
    // become a no-op and the texture will be deleted immediately on the
    // impl thread, along with dropping the reference to the ContextProvider.
    scoped_ptr<SingleReleaseCallback> GetReleaseCallback(
        const scoped_refptr<ContextProvider>& context_provider,
        unsigned texture_id);

private:
    // Runs the |impl_callback| to delete the texture and removes the callback
    // from the |impl_callbacks_| list.
    void RunDeleteTextureOnImplThread(SingleReleaseCallback* impl_callback,
        const gpu::SyncToken& sync_token,
        bool is_lost);

    scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner_;
    ScopedPtrVector<SingleReleaseCallback> impl_callbacks_;
    base::WeakPtrFactory<TextureMailboxDeleter> weak_ptr_factory_;
};

} // namespace cc

#endif // CC_OUTPUT_TEXTURE_MAILBOX_DELETER_H_
