// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FILTERS_DEFAULT_MEDIA_PERMISSION_H_
#define MEDIA_FILTERS_DEFAULT_MEDIA_PERMISSION_H_

#include "media/base/media_export.h"
#include "media/base/media_permission.h"

namespace media {

// Default MediaPermission implementation that will always allow or deny the
// permission request/check based on |allow|.
// WARNING: This class allows or denies permission request/check without real
// user's consent. It should NOT be used in a real user facing product.
class MEDIA_EXPORT DefaultMediaPermission : public MediaPermission {
public:
    explicit DefaultMediaPermission(bool allow);
    ~DefaultMediaPermission() override;

    // media::MediaPermission implementation.
    void HasPermission(Type type,
        const blink::WebURL& security_origin,
        const PermissionStatusCB& permission_status_cb) override;
    void RequestPermission(
        Type type,
        const blink::WebURL& security_origin,
        const PermissionStatusCB& permission_status_cb) override;

private:
    const bool allow_;

    DISALLOW_COPY_AND_ASSIGN(DefaultMediaPermission);
};

} // namespace media

#endif // MEDIA_FILTERS_DEFAULT_MEDIA_PERMISSION_H_
