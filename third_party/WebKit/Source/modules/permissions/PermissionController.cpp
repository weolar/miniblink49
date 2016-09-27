// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/permissions/PermissionController.h"

#include "core/frame/LocalFrame.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "public/platform/modules/permissions/WebPermissionClient.h"

namespace blink {

PermissionController::~PermissionController()
{
}

void PermissionController::provideTo(LocalFrame& frame, WebPermissionClient* client)
{
    ASSERT(RuntimeEnabledFeatures::permissionsEnabled());

    PermissionController* controller = new PermissionController(frame, client);
    WillBeHeapSupplement<LocalFrame>::provideTo(frame, supplementName(), adoptPtrWillBeNoop(controller));
}

PermissionController* PermissionController::from(LocalFrame& frame)
{
    return static_cast<PermissionController*>(WillBeHeapSupplement<LocalFrame>::from(frame, supplementName()));
}

PermissionController::PermissionController(LocalFrame& frame, WebPermissionClient* client)
    : LocalFrameLifecycleObserver(&frame)
    , m_client(client)
{
}

const char* PermissionController::supplementName()
{
    return "PermissionController";
}

WebPermissionClient* PermissionController::client() const
{
    return m_client;
}

void PermissionController::willDetachFrameHost()
{
    m_client = nullptr;
}

DEFINE_TRACE(PermissionController)
{
    LocalFrameLifecycleObserver::trace(visitor);
    WillBeHeapSupplement<LocalFrame>::trace(visitor);
}

} // namespace blink
