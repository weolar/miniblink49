// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PushPermissionStatusCallbacks_h
#define PushPermissionStatusCallbacks_h

#include "platform/heap/Handle.h"
#include "public/platform/modules/push_messaging/WebPushPermissionStatus.h"
#include "public/platform/modules/push_messaging/WebPushProvider.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace WTF {
class String;
}

namespace blink {

class ScriptPromiseResolver;

// Will resolve the underlying promise depending on the permission received.
class PushPermissionStatusCallbacks final : public WebPushPermissionStatusCallbacks {
    WTF_MAKE_NONCOPYABLE(PushPermissionStatusCallbacks);

public:
    explicit PushPermissionStatusCallbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver>);
    ~PushPermissionStatusCallbacks() override;

    void onSuccess(WebPushPermissionStatus*) override;

    // Called if for some reason the status of the push permission cannot be checked.
    void onError(WebPushError*) override;

private:
    static WTF::String permissionString(WebPushPermissionStatus);
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};

} // namespace blink

#endif // PushPermissionStatusCallbacks_h
