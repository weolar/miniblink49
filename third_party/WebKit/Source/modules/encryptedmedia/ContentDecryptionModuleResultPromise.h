// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ContentDecryptionModuleResultPromise_h
#define ContentDecryptionModuleResultPromise_h

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/ExceptionCode.h"
#include "platform/ContentDecryptionModuleResult.h"

namespace blink {

ExceptionCode WebCdmExceptionToExceptionCode(WebContentDecryptionModuleException);

// This class wraps the promise resolver to simplify creation of
// ContentDecryptionModuleResult objects. The default implementations of the
// complete(), completeWithSession(), etc. methods will reject the promise
// with an error. It needs to be subclassed and the appropriate complete()
// method overridden to resolve the promise as needed.
class ContentDecryptionModuleResultPromise : public ContentDecryptionModuleResult {
public:
    ~ContentDecryptionModuleResultPromise() override;

    // ContentDecryptionModuleResult implementation.
    void complete() override;
    void completeWithContentDecryptionModule(WebContentDecryptionModule*) override;
    void completeWithSession(WebContentDecryptionModuleResult::SessionStatus) override;
    void completeWithError(WebContentDecryptionModuleException, unsigned long systemCode, const WebString&) final;

    // It is only valid to call this before completion.
    ScriptPromise promise();

    DECLARE_VIRTUAL_TRACE();

protected:
    explicit ContentDecryptionModuleResultPromise(ScriptState*);

    // Resolves the promise with |value|. Used by subclasses to resolve the
    // promise.
    template <typename... T>
    void resolve(T... value)
    {
        m_resolver->resolve(value...);
        m_resolver.clear();
    }

    // Rejects the promise with a DOMException.
    void reject(ExceptionCode, const String& errorMessage);

    ExecutionContext* executionContext() const;

private:
    RefPtrWillBeMember<ScriptPromiseResolver> m_resolver;
};

} // namespace blink

#endif // ContentDecryptionModuleResultPromise_h
