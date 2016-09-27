// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PermissionQueryCallback_h
#define PermissionQueryCallback_h

#include "platform/heap/Handle.h"
#include "public/platform/WebCallbacks.h"
#include "public/platform/modules/permissions/WebPermissionStatus.h"
#include "public/platform/modules/permissions/WebPermissionType.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class ScriptPromiseResolver;

// PermissionQueryCallback is an implementation of WebPermissionQueryCallbacks
// that will resolve the underlying promise depending on the result passed to
// the callback. It takes a WebPermissionType in its constructor and will pass
// it to the PermissionStatus.
class PermissionQueryCallback final
    : public WebCallbacks<WebPermissionStatus, void> {
public:
    explicit PermissionQueryCallback(PassRefPtr<ScriptPromiseResolver>, WebPermissionType);
    ~PermissionQueryCallback() override;

    void onSuccess(WebPermissionStatus*) override;
    void onError() override;

private:
    RefPtr<ScriptPromiseResolver> m_resolver;
    WebPermissionType m_permissionType;

    WTF_MAKE_NONCOPYABLE(PermissionQueryCallback);
};

} // namespace blink

#endif // PermissionQueryCallback_h
