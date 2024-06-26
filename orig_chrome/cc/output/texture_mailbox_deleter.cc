// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/texture_mailbox_deleter.h"

#include "base/bind.h"
#include "base/location.h"
#include "base/memory/weak_ptr.h"
#include "base/single_thread_task_runner.h"
#include "cc/output/context_provider.h"
#include "cc/resources/single_release_callback.h"
#include "gpu/command_buffer/client/gles2_interface.h"

namespace cc {

static void DeleteTextureOnImplThread(
    const scoped_refptr<ContextProvider>& context_provider,
    unsigned texture_id,
    const gpu::SyncToken& sync_token,
    bool is_lost)
{
    if (sync_token.HasData())
        context_provider->ContextGL()->WaitSyncTokenCHROMIUM(sync_token.GetConstData());
    context_provider->ContextGL()->DeleteTextures(1, &texture_id);
}

static void PostTaskFromMainToImplThread(
    scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner,
    ReleaseCallback run_impl_callback,
    const gpu::SyncToken& sync_token,
    bool is_lost)
{
    // This posts the task to RunDeleteTextureOnImplThread().
    impl_task_runner->PostTask(
        FROM_HERE, base::Bind(run_impl_callback, sync_token, is_lost));
}

TextureMailboxDeleter::TextureMailboxDeleter(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner)
    : impl_task_runner_(task_runner)
    , weak_ptr_factory_(this)
{
}

TextureMailboxDeleter::~TextureMailboxDeleter()
{
    for (size_t i = 0; i < impl_callbacks_.size(); ++i)
        impl_callbacks_.at(i)->Run(gpu::SyncToken(), true);
}

scoped_ptr<SingleReleaseCallback> TextureMailboxDeleter::GetReleaseCallback(
    const scoped_refptr<ContextProvider>& context_provider,
    unsigned texture_id)
{
    // This callback owns a reference on the |context_provider|. It must be
    // destroyed on the impl thread. Upon destruction of this class, the
    // callback must immediately be destroyed.
    scoped_ptr<SingleReleaseCallback> impl_callback = SingleReleaseCallback::Create(base::Bind(&DeleteTextureOnImplThread,
        context_provider,
        texture_id));

    impl_callbacks_.push_back(impl_callback.Pass());

    // The raw pointer to the impl-side callback is valid as long as this
    // class is alive. So we guard it with a WeakPtr.
    ReleaseCallback run_impl_callback(
        base::Bind(&TextureMailboxDeleter::RunDeleteTextureOnImplThread,
            weak_ptr_factory_.GetWeakPtr(),
            impl_callbacks_.back()));

    // Provide a callback for the main thread that posts back to the impl
    // thread.
    scoped_ptr<SingleReleaseCallback> main_callback;
    if (impl_task_runner_) {
        main_callback = SingleReleaseCallback::Create(base::Bind(
            &PostTaskFromMainToImplThread, impl_task_runner_, run_impl_callback));
    } else {
        main_callback = SingleReleaseCallback::Create(run_impl_callback);
    }

    return main_callback.Pass();
}

void TextureMailboxDeleter::RunDeleteTextureOnImplThread(
    SingleReleaseCallback* impl_callback,
    const gpu::SyncToken& sync_token,
    bool is_lost)
{
    for (size_t i = 0; i < impl_callbacks_.size(); ++i) {
        if (impl_callbacks_.at(i) == impl_callback) {
            // Run the callback, then destroy it here on the impl thread.
            impl_callbacks_.at(i)->Run(sync_token, is_lost);
            impl_callbacks_.erase(impl_callbacks_.begin() + i);
            return;
        }
    }

    NOTREACHED() << "The Callback returned by GetDeleteCallback() was called "
                 << "more than once.";
}

} // namespace cc
