// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/permissions/PermissionQueryCallback.h"

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "modules/permissions/PermissionStatus.h"

namespace blink {

PermissionQueryCallback::PermissionQueryCallback(PassRefPtr<ScriptPromiseResolver> resolver, WebPermissionType permissionType)
    : m_resolver(resolver)
    , m_permissionType(permissionType)
{
    ASSERT(m_resolver);
}

PermissionQueryCallback::~PermissionQueryCallback()
{
}

void PermissionQueryCallback::onSuccess(WebPermissionStatus* permissionStatus)
{
    if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped()) {
        PermissionStatus::dispose(permissionStatus);
        return;
    }
    m_resolver->resolve(PermissionStatus::take(m_resolver.get(), permissionStatus, m_permissionType));
}

void PermissionQueryCallback::onError()
{
    if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped()) {
        return;
    }
    m_resolver->reject();
}

} // namespace blink
