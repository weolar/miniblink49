// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SetSinkIdCallbacks_h
#define SetSinkIdCallbacks_h

#include "platform/Timer.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebCallbacks.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class HTMLMediaElement;
class ScriptPromiseResolver;
struct WebSetSinkIdError;

class SetSinkIdCallbacks final : public WebCallbacks<void, WebSetSinkIdError> {
    WTF_MAKE_NONCOPYABLE(SetSinkIdCallbacks);
public:
    SetSinkIdCallbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver>, HTMLMediaElement&, const String& sinkId);
    ~SetSinkIdCallbacks() override;

    void onSuccess() override;
    void onError(WebSetSinkIdError*) override;

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
    RefPtrWillBePersistent<HTMLMediaElement> m_element;
    String m_sinkId;
};

} // namespace blink

#endif // SetSinkIdCallbacks_h
