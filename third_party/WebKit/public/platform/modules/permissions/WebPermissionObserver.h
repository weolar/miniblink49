// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPermissionObserver_h
#define WebPermissionObserver_h

#include "public/platform/modules/permissions/WebPermissionStatus.h"
#include "public/platform/modules/permissions/WebPermissionType.h"

namespace blink {

class WebPermissionObserver {
public:
    virtual void permissionChanged(WebPermissionType, WebPermissionStatus) = 0;
};

} // namespace blink

#endif // WebPermissionObserver_h
