// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebSyncPermissionStatus_h
#define WebSyncPermissionStatus_h

namespace blink {

enum WebSyncPermissionStatus {
    WebSyncPermissionStatusGranted = 0,
    WebSyncPermissionStatusDenied,
    WebSyncPermissionStatusPrompt,

    // Used for IPC message range checks.
    WebSyncPermissionStatusLast = WebSyncPermissionStatusPrompt
};

} // namespace blink
#endif // WebSyncPermissionStatus_h
