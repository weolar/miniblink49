// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebNotificationPermission_h
#define WebNotificationPermission_h

namespace blink {

enum WebNotificationPermission {
    WebNotificationPermissionAllowed = 0,
    WebNotificationPermissionDefault,
    WebNotificationPermissionDenied,

    WebNotificationPermissionLast = WebNotificationPermissionDenied
};

} // namespace blink

#endif // WebNotificationPermission_h
