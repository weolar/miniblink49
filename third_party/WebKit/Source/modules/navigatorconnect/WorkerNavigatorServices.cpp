// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/navigatorconnect/WorkerNavigatorServices.h"

#include "core/workers/WorkerNavigator.h"
#include "modules/navigatorconnect/ServicePortCollection.h"

namespace blink {

WorkerNavigatorServices::~WorkerNavigatorServices()
{
}

WorkerNavigatorServices& WorkerNavigatorServices::from(WorkerNavigator& navigator)
{
    WorkerNavigatorServices* supplement = static_cast<WorkerNavigatorServices*>(HeapSupplement<WorkerNavigator>::from(navigator, supplementName()));
    if (!supplement) {
        supplement = new WorkerNavigatorServices();
        provideTo(navigator, supplementName(), supplement);
    }
    return *supplement;
}

ServicePortCollection* WorkerNavigatorServices::services(ExecutionContext* context, WorkerNavigator& navigator)
{
    return WorkerNavigatorServices::from(navigator).services(context);
}

ServicePortCollection* WorkerNavigatorServices::services(ExecutionContext* context)
{
    if (!m_services)
        m_services = ServicePortCollection::create(context);
    return m_services.get();
}

DEFINE_TRACE(WorkerNavigatorServices)
{
    visitor->trace(m_services);
    HeapSupplement<WorkerNavigator>::trace(visitor);
}

WorkerNavigatorServices::WorkerNavigatorServices()
{
}

const char* WorkerNavigatorServices::supplementName()
{
    return "WorkerNavigatorServices";
}

} // namespace blink
