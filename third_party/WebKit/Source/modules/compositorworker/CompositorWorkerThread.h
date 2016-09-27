// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CompositorWorkerThread_h
#define CompositorWorkerThread_h

#include "core/workers/WorkerThread.h"
#include "modules/ModulesExport.h"

namespace blink {

class WorkerObjectProxy;

// This class is overridden in unit-tests.
class MODULES_EXPORT CompositorWorkerThread : public WorkerThread {
public:
    static PassRefPtr<CompositorWorkerThread> create(PassRefPtr<WorkerLoaderProxy>, WorkerObjectProxy&, double timeOrigin);
    ~CompositorWorkerThread() override;

    WorkerObjectProxy& workerObjectProxy() const { return m_workerObjectProxy; }

protected:
    CompositorWorkerThread(PassRefPtr<WorkerLoaderProxy>, WorkerObjectProxy&, double timeOrigin);

    // WorkerThread:
    PassRefPtrWillBeRawPtr<WorkerGlobalScope> createWorkerGlobalScope(PassOwnPtr<WorkerThreadStartupData>) override;
    WebThreadSupportingGC& backingThread() override;
    void didStartRunLoop() override { }
    void didStopRunLoop() override { }
    void initializeBackingThread() override;
    void shutdownBackingThread() override;
    v8::Isolate* initializeIsolate() override;
    void willDestroyIsolate() override;
    void destroyIsolate() override;
    void terminateV8Execution() override;

private:
    WorkerObjectProxy& m_workerObjectProxy;
    double m_timeOrigin;
};

} // namespace blink

#endif // CompositorWorkerThread_h
