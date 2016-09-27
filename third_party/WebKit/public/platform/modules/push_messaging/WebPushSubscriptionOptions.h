// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPushSubscriptionOptions_h
#define WebPushSubscriptionOptions_h

namespace blink {

struct WebPushSubscriptionOptions {
    WebPushSubscriptionOptions()
        : userVisibleOnly(false)
    {
    }

    // Indicates that the subscription will only be used for push messages
    // that result in UI visible to the user.
    bool userVisibleOnly;
};

} // namespace blink

#endif // WebPushSubscriptionOptions_h
