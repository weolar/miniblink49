// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PermissionController_h
#define PermissionController_h

#include "core/frame/LocalFrameLifecycleObserver.h"
#include "modules/ModulesExport.h"
#include "platform/Supplementable.h"

namespace blink {

class WebPermissionClient;

class MODULES_EXPORT PermissionController final
    : public NoBaseWillBeGarbageCollectedFinalized<PermissionController>
    , public WillBeHeapSupplement<LocalFrame>
    , public LocalFrameLifecycleObserver {
    WTF_MAKE_NONCOPYABLE(PermissionController);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(PermissionController);
public:
    virtual ~PermissionController();

    static void provideTo(LocalFrame&, WebPermissionClient*);
    static PermissionController* from(LocalFrame&);
    static const char* supplementName();

    WebPermissionClient* client() const;

    DECLARE_VIRTUAL_TRACE();

private:
    PermissionController(LocalFrame&, WebPermissionClient*);

    // Inherited from LocalFrameLifecycleObserver.
    void willDetachFrameHost() override;

    WebPermissionClient* m_client;
};

} // namespace blink

#endif // PermissionController_h
