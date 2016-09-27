// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PresentationSessionClientCallbacks_h
#define PresentationSessionClientCallbacks_h

#include "platform/heap/Handle.h"
#include "public/platform/modules/presentation/WebPresentationClient.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class Presentation;
class ScriptPromiseResolver;

// The implementation of the WebCallbacks interface passed to the embedder to
// get the result of start/joinSession() calls. Will in its turn resolve the
// corresponding Promise given to the frame.
// When creating the session succeeds, onSuccess() is invoked with the
// implementation of the WebPresentationSessionClient interface. If creating the
// session fails, onError() is invoked with the implementation of the
// WebPresentationError interface.
// Owned by the receiver of the callback and must be deleted after use.
class PresentationSessionClientCallbacks final : public WebPresentationSessionClientCallbacks {
    WTF_MAKE_NONCOPYABLE(PresentationSessionClientCallbacks);
public:
    PresentationSessionClientCallbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver>, Presentation*);
    ~PresentationSessionClientCallbacks() override;

    // WebPresentationSessionClientCallbacks implementation.
    void onSuccess(WebPresentationSessionClient*) override;
    void onError(WebPresentationError*) override;

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
    Persistent<Presentation> m_presentation;
};

} // namespace blink

#endif // PresentationSessionClientCallbacks_h
