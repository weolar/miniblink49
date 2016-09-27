// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebLockOrientationCallback_h
#define WebLockOrientationCallback_h

#include "public/platform/modules/screen_orientation/WebLockOrientationError.h"
#include "public/platform/modules/screen_orientation/WebScreenOrientationType.h"

namespace blink {

// WebScreenOrientationType is an interface to be used by the embedder in order
// to inform Blink when a screen lock operation has succeeded or failed.
// A success notification comes with the new orientation angle and orientation
// type and a failure notification comes with an information about the type of
// failure.
class WebLockOrientationCallback {
public:
    virtual ~WebLockOrientationCallback() { }

    virtual void onSuccess() = 0;
    virtual void onError(WebLockOrientationError) = 0;
};

} // namespace blink

#endif // WebLockOrientationCallback_h
