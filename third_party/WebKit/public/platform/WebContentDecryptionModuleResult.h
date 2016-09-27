// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebContentDecryptionModuleResult_h
#define WebContentDecryptionModuleResult_h

#include "WebCommon.h"
#include "WebContentDecryptionModuleException.h"
#include "WebPrivatePtr.h"

namespace blink {

class ContentDecryptionModuleResult;
class WebContentDecryptionModule;
class WebString;

class WebContentDecryptionModuleResult {
public:
    enum SessionStatus {
        // New session has been initialized.
        NewSession,

        // CDM could not find the requested session.
        SessionNotFound,

        // CDM already has a non-closed session that matches the provided
        // parameters.
        SessionAlreadyExists,
    };

    WebContentDecryptionModuleResult(const WebContentDecryptionModuleResult& o)
    {
        assign(o);
    }

    ~WebContentDecryptionModuleResult()
    {
        reset();
    }

    WebContentDecryptionModuleResult& operator=(const WebContentDecryptionModuleResult& o)
    {
        assign(o);
        return *this;
    }

    // Called when the CDM completes an operation and has no additional data to
    // pass back.
    BLINK_PLATFORM_EXPORT void complete();

    // Called when a CDM is created.
    BLINK_PLATFORM_EXPORT void completeWithContentDecryptionModule(WebContentDecryptionModule*);

    // Called when the CDM completes a session operation.
    BLINK_PLATFORM_EXPORT void completeWithSession(SessionStatus);

    // Called when the operation fails.
    BLINK_PLATFORM_EXPORT void completeWithError(WebContentDecryptionModuleException, unsigned long systemCode, const WebString& message);

#if INSIDE_BLINK
    BLINK_PLATFORM_EXPORT explicit WebContentDecryptionModuleResult(ContentDecryptionModuleResult*);
#endif

private:
    BLINK_PLATFORM_EXPORT void reset();
    BLINK_PLATFORM_EXPORT void assign(const WebContentDecryptionModuleResult&);

    WebPrivatePtr<ContentDecryptionModuleResult> m_impl;
};

} // namespace blink

#endif // WebContentDecryptionModuleSession_h
