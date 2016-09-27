// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/encryptedmedia/ContentDecryptionModuleResultPromise.h"

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/dom/DOMException.h"
#include "public/platform/WebString.h"
#include "wtf/Assertions.h"

namespace blink {

ExceptionCode WebCdmExceptionToExceptionCode(WebContentDecryptionModuleException cdmException)
{
    switch (cdmException) {
    case WebContentDecryptionModuleExceptionNotSupportedError:
        return NotSupportedError;
    case WebContentDecryptionModuleExceptionInvalidStateError:
        return InvalidStateError;
    case WebContentDecryptionModuleExceptionInvalidAccessError:
        return InvalidAccessError;
    case WebContentDecryptionModuleExceptionQuotaExceededError:
        return QuotaExceededError;
    case WebContentDecryptionModuleExceptionUnknownError:
        return UnknownError;
    case WebContentDecryptionModuleExceptionClientError:
    case WebContentDecryptionModuleExceptionOutputError:
        // Currently no matching DOMException for these 2 errors.
        // FIXME: Update DOMException to handle these if actually added to
        // the EME spec.
        return UnknownError;
    }

    ASSERT_NOT_REACHED();
    return UnknownError;
}

ContentDecryptionModuleResultPromise::ContentDecryptionModuleResultPromise(ScriptState* scriptState)
    : m_resolver(ScriptPromiseResolver::create(scriptState))
{
}

ContentDecryptionModuleResultPromise::~ContentDecryptionModuleResultPromise()
{
}

void ContentDecryptionModuleResultPromise::complete()
{
    ASSERT_NOT_REACHED();
    reject(InvalidStateError, "Unexpected completion.");
}

void ContentDecryptionModuleResultPromise::completeWithContentDecryptionModule(WebContentDecryptionModule* cdm)
{
    ASSERT_NOT_REACHED();
    reject(InvalidStateError, "Unexpected completion.");
}

void ContentDecryptionModuleResultPromise::completeWithSession(WebContentDecryptionModuleResult::SessionStatus status)
{
    ASSERT_NOT_REACHED();
    reject(InvalidStateError, "Unexpected completion.");
}

void ContentDecryptionModuleResultPromise::completeWithError(WebContentDecryptionModuleException exceptionCode, unsigned long systemCode, const WebString& errorMessage)
{
    // Non-zero |systemCode| is appended to the |errorMessage|. If the
    // |errorMessage| is empty, we'll report "Rejected with system code
    // (systemCode)".
    String errorString = errorMessage;
    if (systemCode != 0) {
        if (errorString.isEmpty())
            errorString.append("Rejected with system code");
        errorString.append(" (" + String::number(systemCode) + ")");
    }
    reject(WebCdmExceptionToExceptionCode(exceptionCode), errorString);
}

ScriptPromise ContentDecryptionModuleResultPromise::promise()
{
    return m_resolver->promise();
}

void ContentDecryptionModuleResultPromise::reject(ExceptionCode code, const String& errorMessage)
{
    m_resolver->reject(DOMException::create(code, errorMessage));
    m_resolver.clear();
}

ExecutionContext* ContentDecryptionModuleResultPromise::executionContext() const
{
    return m_resolver->executionContext();
}

DEFINE_TRACE(ContentDecryptionModuleResultPromise)
{
    visitor->trace(m_resolver);
    ContentDecryptionModuleResult::trace(visitor);
}

} // namespace blink
