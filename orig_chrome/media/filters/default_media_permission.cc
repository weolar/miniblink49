// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/default_media_permission.h"

#include "base/bind.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/single_thread_task_runner.h"
#include "base/thread_task_runner_handle.h"

namespace media {

DefaultMediaPermission::DefaultMediaPermission(bool allow)
    : allow_(allow)
{
}

DefaultMediaPermission::~DefaultMediaPermission()
{
}

static void FirePermissionStatusCallback(
    const MediaPermission::PermissionStatusCB& permission_status_cb,
    bool allow)
{
    LOG(WARNING) << (allow ? "Allowing" : "Denying")
                 << "media permission request with a default value instead of "
                    "real user's consent. This should NOT be used for in a real "
                    "user-facing product.";
    // Return the callback asynchronously.
    base::ThreadTaskRunnerHandle::Get()->PostTask(
        FROM_HERE, base::Bind(permission_status_cb, allow));
}

void DefaultMediaPermission::HasPermission(
    Type type,
    const blink::WebURL& /* security_origin */,
    const PermissionStatusCB& permission_status_cb)
{
    CHECK_EQ(PROTECTED_MEDIA_IDENTIFIER, type);
    FirePermissionStatusCallback(permission_status_cb, allow_);
}

void DefaultMediaPermission::RequestPermission(
    Type type,
    const blink::WebURL& /* security_origin */,
    const PermissionStatusCB& permission_status_cb)
{
    CHECK_EQ(PROTECTED_MEDIA_IDENTIFIER, type);
    FirePermissionStatusCallback(permission_status_cb, allow_);
}

} // namespace media
