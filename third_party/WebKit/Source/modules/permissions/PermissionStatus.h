// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PermissionStatus_h
#define PermissionStatus_h

#include "core/dom/ActiveDOMObject.h"
#include "core/events/EventTarget.h"
#include "platform/heap/Handle.h"
#include "public/platform/modules/permissions/WebPermissionObserver.h"
#include "public/platform/modules/permissions/WebPermissionStatus.h"
#include "public/platform/modules/permissions/WebPermissionType.h"
#include "wtf/text/AtomicString.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ExecutionContext;
class ScriptPromiseResolver;

// Expose the status of a given WebPermissionType for the current
// ExecutionContext.
class PermissionStatus final
    : public RefCountedGarbageCollectedEventTargetWithInlineData<PermissionStatus>
    , public ActiveDOMObject
    , public WebPermissionObserver {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(PermissionStatus);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(PermissionStatus);
    DEFINE_WRAPPERTYPEINFO();
public:
    static PermissionStatus* take(ScriptPromiseResolver*, WebPermissionStatus*, WebPermissionType);
    static void dispose(WebPermissionStatus*);

    static PermissionStatus* create(ExecutionContext*, WebPermissionStatus, WebPermissionType);
    ~PermissionStatus() override;

    // EventTarget implementation.
    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override;

    // WebPermissionObserver implementation.
    void permissionChanged(WebPermissionType, WebPermissionStatus) override;

    // ActiveDOMObject implementation.
    bool hasPendingActivity() const override;
    void suspend() override;
    void resume() override;
    void stop() override;

    String state() const;

    DEFINE_ATTRIBUTE_EVENT_LISTENER(change);

    DECLARE_VIRTUAL_TRACE();

private:
    PermissionStatus(ExecutionContext*, WebPermissionStatus, WebPermissionType);

    void startListening();
    void stopListening();

    WebPermissionStatus m_status;
    WebPermissionType m_type;
    bool m_listening;
};

} // namespace blink

#endif // PermissionStatus_h
