// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WorkerNavigatorGeofencing_h
#define WorkerNavigatorGeofencing_h

#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class Geofencing;
class WorkerNavigator;

// FIXME: Delete this class once ServiceWorkerRegistration is exposed in service workers.
class WorkerNavigatorGeofencing final : public GarbageCollectedFinalized<WorkerNavigatorGeofencing>, public HeapSupplement<WorkerNavigator> {
    USING_GARBAGE_COLLECTED_MIXIN(WorkerNavigatorGeofencing);
    WTF_MAKE_NONCOPYABLE(WorkerNavigatorGeofencing);
public:
    virtual ~WorkerNavigatorGeofencing();
    static WorkerNavigatorGeofencing& from(WorkerNavigator&);

    static Geofencing* geofencing(WorkerNavigator&);
    Geofencing* geofencing();

    DECLARE_VIRTUAL_TRACE();

private:
    WorkerNavigatorGeofencing();
    static const char* supplementName();

    Member<Geofencing> m_geofencing;
};

} // namespace blink

#endif // WorkerNavigatorGeofencing_h
