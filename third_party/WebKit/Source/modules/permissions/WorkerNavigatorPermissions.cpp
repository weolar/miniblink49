// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/permissions/WorkerNavigatorPermissions.h"

#include "core/workers/WorkerNavigator.h"
#include "modules/permissions/Permissions.h"

namespace blink {

WorkerNavigatorPermissions::WorkerNavigatorPermissions()
{
}

// static
const char* WorkerNavigatorPermissions::supplementName()
{
    return "WorkerNavigatorPermissions";
}

// static
WorkerNavigatorPermissions& WorkerNavigatorPermissions::from(WorkerNavigator& workerNavigator)
{
    WorkerNavigatorPermissions* supplement = static_cast<WorkerNavigatorPermissions*>(HeapSupplement<WorkerNavigator>::from(workerNavigator, supplementName()));
    if (!supplement) {
        supplement = new WorkerNavigatorPermissions();
        provideTo(workerNavigator, supplementName(), supplement);
    }
    return *supplement;
}

// static
Permissions* WorkerNavigatorPermissions::permissions(WorkerNavigator& workerNavigator)
{
    WorkerNavigatorPermissions& self = WorkerNavigatorPermissions::from(workerNavigator);
    if (!self.m_permissions)
        self.m_permissions = new Permissions();
    return self.m_permissions;
}

DEFINE_TRACE(WorkerNavigatorPermissions)
{
    visitor->trace(m_permissions);
    HeapSupplement<WorkerNavigator>::trace(visitor);
}

} // namespace blink
