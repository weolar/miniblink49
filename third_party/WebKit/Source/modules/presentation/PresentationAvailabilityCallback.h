// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PresentationAvailabilityCallback_h
#define PresentationAvailabilityCallback_h

#include "platform/heap/Handle.h"
#include "public/platform/WebCallbacks.h"
#include "wtf/Noncopyable.h"

namespace blink {

class ScriptPromiseResolver;
struct WebPresentationError;

using WebPresentationAvailabilityCallbacks = WebCallbacks<bool, WebPresentationError>;

// PresentationAvailabilityCallback is an implementation of
// WebPresentationAvailabilityCallbacks that will resolve the underlying promise
// depending on the result passed to the callback.
class PresentationAvailabilityCallback final
    : public WebPresentationAvailabilityCallbacks {
    WTF_MAKE_NONCOPYABLE(PresentationAvailabilityCallback);
public:
    explicit PresentationAvailabilityCallback(PassRefPtrWillBeRawPtr<ScriptPromiseResolver>);
    ~PresentationAvailabilityCallback() override = default;

    void onSuccess(bool*) override;
    void onError(WebPresentationError*) override;

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};

} // namespace blink

#endif // PresentationAvailabilityCallback_h
