// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RESOURCES_SINGLE_RELEASE_CALLBACK_IMPL_H_
#define CC_RESOURCES_SINGLE_RELEASE_CALLBACK_IMPL_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/resources/release_callback_impl.h"

namespace gpu {
struct SyncToken;
} // namespace gpu

namespace cc {

class CC_EXPORT SingleReleaseCallbackImpl {
public:
    static scoped_ptr<SingleReleaseCallbackImpl> Create(
        const ReleaseCallbackImpl& cb)
    {
        return make_scoped_ptr(new SingleReleaseCallbackImpl(cb));
    }

    ~SingleReleaseCallbackImpl();

    void Run(const gpu::SyncToken& sync_token,
        bool is_lost,
        BlockingTaskRunner* main_thread_task_runner);

private:
    explicit SingleReleaseCallbackImpl(const ReleaseCallbackImpl& callback);

    ReleaseCallbackImpl callback_;
};

} // namespace cc

#endif // CC_RESOURCES_SINGLE_RELEASE_CALLBACK_IMPL_H_
