/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WorkerLoaderProxy_h
#define WorkerLoaderProxy_h

#include "core/CoreExport.h"
#include "core/dom/ExecutionContext.h"
#include "wtf/Forward.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/ThreadSafeRefCounted.h"

namespace blink {

// The WorkerLoaderProxy is a proxy to the loader context. Normally, the
// document on the main thread provides loading services for the subordinate
// workers. WorkerLoaderProxy provides 2-way communications to the Document
// context and back to the worker.
//
// Note that in multi-process browsers, the Worker object context and the Document
// context can be distinct.

// The abstract interface providing the methods for actually posting tasks; separated
// from the thread-safe & ref-counted WorkerLoaderProxy object which keeps a protected
// reference to the provider object. This to support non-overlapping lifetimes, the
// provider may be destructed before all references to the WorkerLoaderProxy object
// have been dropped.
//
// A provider implementation must detach itself when finalizing by calling
// WorkerLoaderProxy::detachProvider(). This stops the WorkerLoaderProxy from accessing
// the now-dead object, but it will remain alive while ref-ptrs are still kept to it.
class CORE_EXPORT WorkerLoaderProxyProvider {
public:
    virtual ~WorkerLoaderProxyProvider() { }

    // Posts a task to the thread which runs the loading code (normally, the main thread).
    virtual void postTaskToLoader(PassOwnPtr<ExecutionContextTask>) = 0;

    // Posts callbacks from loading code to the WorkerGlobalScope.
    // Returns true if the task was posted successfully.
    virtual bool postTaskToWorkerGlobalScope(PassOwnPtr<ExecutionContextTask>) = 0;
};

class CORE_EXPORT WorkerLoaderProxy : public ThreadSafeRefCounted<WorkerLoaderProxy>, public WorkerLoaderProxyProvider {
public:
    static PassRefPtr<WorkerLoaderProxy> create(WorkerLoaderProxyProvider* loaderProxyProvider)
    {
        return adoptRef(new WorkerLoaderProxy(loaderProxyProvider));
    }

    ~WorkerLoaderProxy() override;

    void postTaskToLoader(PassOwnPtr<ExecutionContextTask>) override;
    bool postTaskToWorkerGlobalScope(PassOwnPtr<ExecutionContextTask>) override;

    // Notification from the provider that it can no longer be
    // accessed. An implementation of WorkerLoaderProxyProvider is
    // required to call detachProvider() when finalizing.
    void detachProvider(WorkerLoaderProxyProvider*);

private:
    explicit WorkerLoaderProxy(WorkerLoaderProxyProvider*);

    Mutex m_lock;
    WorkerLoaderProxyProvider* m_loaderProxyProvider;
};

} // namespace blink

#endif // WorkerLoaderProxy_h
