// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DedicatedWorkerMessagingProxy_h
#define DedicatedWorkerMessagingProxy_h

#include "core/CoreExport.h"
#include "core/workers/WorkerMessagingProxy.h"

namespace blink {

class CORE_EXPORT DedicatedWorkerMessagingProxy final : public WorkerMessagingProxy {
    WTF_MAKE_NONCOPYABLE(DedicatedWorkerMessagingProxy);
    WTF_MAKE_FAST_ALLOCATED(WorkerMessagingProxy);
public:
    DedicatedWorkerMessagingProxy(InProcessWorkerBase*, PassOwnPtrWillBeRawPtr<WorkerClients>);
    virtual ~DedicatedWorkerMessagingProxy();

    PassRefPtr<WorkerThread> createWorkerThread(double originTime) override;
};

} // namespace blink

#endif // DedicatedWorkerMessagingProxy_h
