// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/threaded_channel.h"

#include "base/bind.h"
#include "base/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"

namespace cc {

scoped_ptr<ThreadedChannel> ThreadedChannel::Create(
    ThreadProxy* thread_proxy,
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
    scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner)
{
    return make_scoped_ptr(
        new ThreadedChannel(thread_proxy, main_task_runner, impl_task_runner));
}

ThreadedChannel::ThreadedChannel(
    ThreadProxy* thread_proxy,
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
    scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner)
    : proxy_main_(thread_proxy)
    , proxy_impl_(thread_proxy)
    , main_task_runner_(main_task_runner)
    , impl_task_runner_(impl_task_runner)
{
}

void ThreadedChannel::SetThrottleFrameProductionOnImpl(bool throttle)
{
    ImplThreadTaskRunner()->PostTask(
        FROM_HERE, base::Bind(&ProxyImpl::SetThrottleFrameProductionOnImpl, proxy_impl_->GetImplWeakPtr(), throttle));
}

void ThreadedChannel::SetLayerTreeHostClientReadyOnImpl()
{
    ImplThreadTaskRunner()->PostTask(
        FROM_HERE, base::Bind(&ProxyImpl::SetLayerTreeHostClientReadyOnImpl, proxy_impl_->GetImplWeakPtr()));
}

void ThreadedChannel::DidCompleteSwapBuffers()
{
    MainThreadTaskRunner()->PostTask(
        FROM_HERE, base::Bind(&ProxyMain::DidCompleteSwapBuffers, proxy_main_->GetMainWeakPtr()));
}

ThreadedChannel::~ThreadedChannel()
{
    TRACE_EVENT0("cc", "ThreadChannel::~ThreadChannel");
}

base::SingleThreadTaskRunner* ThreadedChannel::MainThreadTaskRunner() const
{
    return main_task_runner_.get();
}

base::SingleThreadTaskRunner* ThreadedChannel::ImplThreadTaskRunner() const
{
    return impl_task_runner_.get();
}

} // namespace cc
