// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPermissionStatus_h
#define WebPermissionStatus_h

namespace blink {

enum WebPermissionStatus {
    WebPermissionStatusGranted,
    WebPermissionStatusDenied,
    WebPermissionStatusPrompt,
};

} // namespace blink

#endif // WebPermissionStatus_h
