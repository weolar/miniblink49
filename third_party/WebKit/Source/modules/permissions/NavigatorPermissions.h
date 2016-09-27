// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NavigatorPermissions_h
#define NavigatorPermissions_h

#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class Navigator;
class Permissions;

class NavigatorPermissions final : public GarbageCollected<NavigatorPermissions>, public HeapSupplement<Navigator> {
    USING_GARBAGE_COLLECTED_MIXIN(NavigatorPermissions);
public:
    static NavigatorPermissions& from(Navigator&);
    static Permissions* permissions(Navigator&);

    DECLARE_VIRTUAL_TRACE();

private:
    NavigatorPermissions();

    static const char* supplementName();

    Member<Permissions> m_permissions;
};

} // namespace blink

#endif // NavigatorPermissions_h
