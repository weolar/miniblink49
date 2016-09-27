// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/netinfo/WorkerNavigatorNetworkInformation.h"

#include "core/workers/WorkerNavigator.h"
#include "modules/netinfo/NetworkInformation.h"

namespace blink {

WorkerNavigatorNetworkInformation::WorkerNavigatorNetworkInformation(WorkerNavigator& navigator, ExecutionContext* context)
{
}

WorkerNavigatorNetworkInformation& WorkerNavigatorNetworkInformation::from(WorkerNavigator& navigator, ExecutionContext* context)
{
    WorkerNavigatorNetworkInformation* supplement = toWorkerNavigatorNetworkInformation(navigator, context);
    if (!supplement) {
        supplement = new WorkerNavigatorNetworkInformation(navigator, context);
        provideTo(navigator, supplementName(), supplement);
    }
    return *supplement;
}

WorkerNavigatorNetworkInformation* WorkerNavigatorNetworkInformation::toWorkerNavigatorNetworkInformation(WorkerNavigator& navigator, ExecutionContext* context)
{
    return static_cast<WorkerNavigatorNetworkInformation*>(HeapSupplement<WorkerNavigator>::from(navigator, supplementName()));
}

const char* WorkerNavigatorNetworkInformation::supplementName()
{
    return "WorkerNavigatorNetworkInformation";
}

NetworkInformation* WorkerNavigatorNetworkInformation::connection(ExecutionContext* context, WorkerNavigator& navigator)
{
    return WorkerNavigatorNetworkInformation::from(navigator, context).connection(context);
}

DEFINE_TRACE(WorkerNavigatorNetworkInformation)
{
    visitor->trace(m_connection);
    HeapSupplement<WorkerNavigator>::trace(visitor);
}

NetworkInformation* WorkerNavigatorNetworkInformation::connection(ExecutionContext* context)
{
    ASSERT(context);
    if (!m_connection)
        m_connection = NetworkInformation::create(context);
    return m_connection.get();
}

} // namespace blink
