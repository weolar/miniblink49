// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CompositorWorker_h
#define CompositorWorker_h

#include "core/workers/InProcessWorkerBase.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/AtomicString.h"

namespace blink {

class ExceptionState;
class ExecutionContext;
class WorkerGlobalScopeProxy;

class CompositorWorker final : public InProcessWorkerBase {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<CompositorWorker> create(ExecutionContext*, const String& url, ExceptionState&);
    ~CompositorWorker() override;

    const AtomicString& interfaceName() const override;
    WorkerGlobalScopeProxy* createWorkerGlobalScopeProxy(ExecutionContext*) override;

private:
    explicit CompositorWorker(ExecutionContext*);
};

} // namespace blink

#endif // CompositorWorker_h
