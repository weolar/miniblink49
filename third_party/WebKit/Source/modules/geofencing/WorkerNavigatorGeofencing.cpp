// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/geofencing/WorkerNavigatorGeofencing.h"

#include "core/workers/WorkerNavigator.h"
#include "modules/geofencing/Geofencing.h"

namespace blink {

WorkerNavigatorGeofencing::WorkerNavigatorGeofencing()
{
}

WorkerNavigatorGeofencing::~WorkerNavigatorGeofencing()
{
}

const char* WorkerNavigatorGeofencing::supplementName()
{
    return "WorkerNavigatorGeofencing";
}

WorkerNavigatorGeofencing& WorkerNavigatorGeofencing::from(WorkerNavigator& navigator)
{
    WorkerNavigatorGeofencing* supplement = static_cast<WorkerNavigatorGeofencing*>(HeapSupplement<WorkerNavigator>::from(navigator, supplementName()));
    if (!supplement) {
        supplement = new WorkerNavigatorGeofencing();
        provideTo(navigator, supplementName(), supplement);
    }
    return *supplement;
}

Geofencing* WorkerNavigatorGeofencing::geofencing(WorkerNavigator& navigator)
{
    return WorkerNavigatorGeofencing::from(navigator).geofencing();
}

Geofencing* WorkerNavigatorGeofencing::geofencing()
{
    if (!m_geofencing)
        m_geofencing = Geofencing::create(nullptr);
    return m_geofencing.get();
}

DEFINE_TRACE(WorkerNavigatorGeofencing)
{
    visitor->trace(m_geofencing);
    HeapSupplement<WorkerNavigator>::trace(visitor);
}

} // namespace blink
