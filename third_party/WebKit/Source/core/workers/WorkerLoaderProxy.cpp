// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "core/workers/WorkerLoaderProxy.h"

#include "core/dom/ExecutionContextTask.h"

namespace blink {

WorkerLoaderProxy::WorkerLoaderProxy(WorkerLoaderProxyProvider* loaderProxyProvider)
    : m_loaderProxyProvider(loaderProxyProvider)
{
}

WorkerLoaderProxy::~WorkerLoaderProxy()
{
    ASSERT(!m_loaderProxyProvider);
}

void WorkerLoaderProxy::detachProvider(WorkerLoaderProxyProvider* proxyProvider)
{
    MutexLocker locker(m_lock);
    ASSERT_UNUSED(proxyProvider, proxyProvider == m_loaderProxyProvider);
    m_loaderProxyProvider = nullptr;
}

void WorkerLoaderProxy::postTaskToLoader(PassOwnPtr<ExecutionContextTask> task)
{
    MutexLocker locker(m_lock);
    if (!m_loaderProxyProvider)
        return;

    m_loaderProxyProvider->postTaskToLoader(task);
}

bool WorkerLoaderProxy::postTaskToWorkerGlobalScope(PassOwnPtr<ExecutionContextTask> task)
{
    MutexLocker locker(m_lock);
    if (!m_loaderProxyProvider)
        return false;

    return m_loaderProxyProvider->postTaskToWorkerGlobalScope(task);
}

} // namespace blink
