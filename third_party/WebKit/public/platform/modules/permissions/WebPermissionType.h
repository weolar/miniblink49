// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPermissionType_h
#define WebPermissionType_h

namespace blink {

enum WebPermissionType {
    WebPermissionTypeGeolocation,
    WebPermissionTypeNotifications,
    WebPermissionTypePush,
    WebPermissionTypePushNotifications,
    WebPermissionTypeMidi,
    WebPermissionTypeMidiSysEx,
};

} // namespace blink

#endif // WebPermissionType_h
