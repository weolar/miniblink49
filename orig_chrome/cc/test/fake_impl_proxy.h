// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_IMPL_PROXY_H_
#define CC_TEST_FAKE_IMPL_PROXY_H_

#include "base/thread_task_runner_handle.h"
#include "cc/test/fake_proxy.h"
#include "cc/trees/single_thread_proxy.h"

namespace base {
class SingleThreadIdleTaskRunner;
}

namespace cc {

class FakeImplProxy : public FakeProxy {
public:
    FakeImplProxy()
        : FakeProxy(base::ThreadTaskRunnerHandle::Get(), nullptr)
        , set_impl_thread_(this)
    {
    }

    explicit FakeImplProxy(
        scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner)
        : FakeProxy(base::ThreadTaskRunnerHandle::Get(), impl_task_runner)
        , set_impl_thread_(this)
    {
    }

private:
    DebugScopedSetImplThread set_impl_thread_;
};

} // namespace cc

#endif // CC_TEST_FAKE_IMPL_PROXY_H_
