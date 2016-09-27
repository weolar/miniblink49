// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ContentDecryptionModuleResult_h
#define ContentDecryptionModuleResult_h

#include "platform/heap/Handle.h"
#include "public/platform/WebContentDecryptionModuleException.h"
#include "public/platform/WebContentDecryptionModuleResult.h"

namespace blink {

class WebContentDecryptionModule;
class WebString;

// Used to notify completion of a CDM operation.
class ContentDecryptionModuleResult : public GarbageCollectedFinalized<ContentDecryptionModuleResult> {
public:
    virtual ~ContentDecryptionModuleResult() { }

    virtual void complete() = 0;
    virtual void completeWithContentDecryptionModule(WebContentDecryptionModule*) = 0;
    virtual void completeWithSession(WebContentDecryptionModuleResult::SessionStatus) = 0;
    virtual void completeWithError(WebContentDecryptionModuleException, unsigned long systemCode, const WebString&) = 0;

    WebContentDecryptionModuleResult result()
    {
        return WebContentDecryptionModuleResult(this);
    }

    DEFINE_INLINE_VIRTUAL_TRACE() { }
};

} // namespace blink

#endif // ContentDecryptionModuleResult_h
