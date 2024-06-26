// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_MEDIA_PERMISSION_H_
#define MEDIA_BASE_MEDIA_PERMISSION_H_

#include "base/callback.h"
#include "base/macros.h"
#include "media/base/media_export.h"

namespace blink {
class WebURL;
}

namespace media {

// Interface to handle media related permission checks and requests.
class MEDIA_EXPORT MediaPermission {
public:
    typedef base::Callback<void(bool)> PermissionStatusCB;

    enum Type {
        PROTECTED_MEDIA_IDENTIFIER,
        AUDIO_CAPTURE,
        VIDEO_CAPTURE,
    };

    MediaPermission();
    virtual ~MediaPermission();

    // Checks whether |type| is permitted for |security_origion| without
    // triggering user interaction (e.g. permission prompt). The status will be
    // |false| if the permission has never been set.
    virtual void HasPermission(
        Type type,
        const blink::WebURL& security_origin,
        const PermissionStatusCB& permission_status_cb)
        = 0;

    // Requests |type| permission for |security_origion|. This may trigger user
    // interaction (e.g. permission prompt) if the permission has never been set.
    virtual void RequestPermission(
        Type type,
        const blink::WebURL& security_origin,
        const PermissionStatusCB& permission_status_cb)
        = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(MediaPermission);
};

} // namespace media

#endif // MEDIA_BASE_MEDIA_PERMISSION_H_
