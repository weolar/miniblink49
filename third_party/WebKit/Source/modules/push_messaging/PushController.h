// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PushController_h
#define PushController_h

#include "core/frame/LocalFrame.h"
#include "modules/ModulesExport.h"
#include "platform/Supplementable.h"
#include "wtf/Forward.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class WebPushClient;

class PushController final : public NoBaseWillBeGarbageCollected<PushController>, public WillBeHeapSupplement<LocalFrame> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(PushController);
    WTF_MAKE_NONCOPYABLE(PushController);

public:
    static PassOwnPtrWillBeRawPtr<PushController> create(WebPushClient*);
    static const char* supplementName();
    static PushController* from(LocalFrame* frame) { return static_cast<PushController*>(WillBeHeapSupplement<LocalFrame>::from(frame, supplementName())); }
    static WebPushClient& clientFrom(LocalFrame*);

    DEFINE_INLINE_VIRTUAL_TRACE() { WillBeHeapSupplement<LocalFrame>::trace(visitor); }

private:
    explicit PushController(WebPushClient*);

    WebPushClient* client() const { return m_client; }

    WebPushClient* m_client;
};

MODULES_EXPORT void providePushControllerTo(LocalFrame&, WebPushClient*);

} // namespace blink

#endif // PushController_h
