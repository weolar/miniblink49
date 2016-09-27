// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPushPermissionStatus_h
#define WebPushPermissionStatus_h

namespace blink {

enum WebPushPermissionStatus {
    WebPushPermissionStatusGranted = 0,
    WebPushPermissionStatusDenied,
    WebPushPermissionStatusPrompt,

    // Used for IPC message range checks.
    WebPushPermissionStatusLast = WebPushPermissionStatusPrompt
};

} // namespace blink
#endif // WebPushPermissionStatus_h
