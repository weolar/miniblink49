// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WorkerNavigatorPermissions_h
#define WorkerNavigatorPermissions_h

#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class WorkerNavigator;
class Permissions;

class WorkerNavigatorPermissions final : public GarbageCollected<WorkerNavigatorPermissions>, public HeapSupplement<WorkerNavigator> {
    USING_GARBAGE_COLLECTED_MIXIN(WorkerNavigatorPermissions);
public:
    static WorkerNavigatorPermissions& from(WorkerNavigator&);
    static Permissions* permissions(WorkerNavigator&);

    DECLARE_VIRTUAL_TRACE();

private:
    static const char* supplementName();

    WorkerNavigatorPermissions();

    Member<Permissions> m_permissions;
};

} // namespace blink

#endif // WorkerNavigatorPermissions_h
