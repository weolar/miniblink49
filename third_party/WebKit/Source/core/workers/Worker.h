// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Worker_h
#define Worker_h

#include "core/workers/InProcessWorkerBase.h"

namespace blink {

class ExceptionState;
class ExecutionContext;
class WorkerGlobalScopeProxy;

class CORE_EXPORT Worker final : public InProcessWorkerBase {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<Worker> create(ExecutionContext*, const String& url, ExceptionState&);
    virtual ~Worker();

protected:
    explicit Worker(ExecutionContext*);

    WorkerGlobalScopeProxy* createWorkerGlobalScopeProxy(ExecutionContext*) override;
    const AtomicString& interfaceName() const override;
};

} // namespace blink

#endif // Worker_h
