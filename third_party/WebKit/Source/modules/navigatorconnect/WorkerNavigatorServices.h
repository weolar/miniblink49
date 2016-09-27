// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WorkerNavigatorServices_h
#define WorkerNavigatorServices_h

#include "modules/ModulesExport.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class ExecutionContext;
class ServicePortCollection;
class WorkerNavigator;

class MODULES_EXPORT WorkerNavigatorServices final : public GarbageCollectedFinalized<WorkerNavigatorServices>, public HeapSupplement<WorkerNavigator> {
    USING_GARBAGE_COLLECTED_MIXIN(WorkerNavigatorServices);
    WTF_MAKE_NONCOPYABLE(WorkerNavigatorServices);
public:
    virtual ~WorkerNavigatorServices();
    static WorkerNavigatorServices& from(WorkerNavigator&);

    static ServicePortCollection* services(ExecutionContext*, WorkerNavigator&);
    ServicePortCollection* services(ExecutionContext*);

    DECLARE_VIRTUAL_TRACE();

private:
    WorkerNavigatorServices();
    static const char* supplementName();

    Member<ServicePortCollection> m_services;
};

} // namespace blink

#endif // WorkerNavigatorServices_h
