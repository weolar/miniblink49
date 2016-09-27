// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebLockOrientationError_h
#define WebLockOrientationError_h

namespace blink {

enum WebLockOrientationError {
    // If locking isn't available on the platform.
    WebLockOrientationErrorNotAvailable,

    // If fullscreen is required to lock.
    WebLockOrientationErrorFullScreenRequired,

    // If another lock/unlock got called before that one ended.
    WebLockOrientationErrorCanceled,
};

} // namespace blink

#endif // WebLockOrientationError_h
