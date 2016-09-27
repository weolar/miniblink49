/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ServiceWorkerError.h"

#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"

using blink::WebServiceWorkerError;

namespace blink {

static DOMException* createException(ExceptionCode code, const String& defaultMessage, const String& message)
{
    return DOMException::create(code, message.isEmpty() ? defaultMessage : message);
}

// static
DOMException* ServiceWorkerError::take(ScriptPromiseResolver*, PassOwnPtr<WebType> webError)
{
    switch (webError->errorType) {
    case WebServiceWorkerError::ErrorTypeAbort:
        return createException(AbortError, "The Service Worker operation was aborted.", webError->message);
    case WebServiceWorkerError::ErrorTypeActivate:
        // Not currently returned as a promise rejection.
        // FIXME: Introduce new ActivateError type to ExceptionCodes?
        return createException(AbortError, "The Service Worker activation failed.", webError->message);
    case WebServiceWorkerError::ErrorTypeDisabled:
        return createException(NotSupportedError, "Service Worker support is disabled.", webError->message);
    case WebServiceWorkerError::ErrorTypeInstall:
        // FIXME: Introduce new InstallError type to ExceptionCodes?
        return createException(AbortError, "The Service Worker installation failed.", webError->message);
    case WebServiceWorkerError::ErrorTypeNetwork:
        return createException(NetworkError, "The Service Worker failed by network.", webError->message);
    case WebServiceWorkerError::ErrorTypeNotFound:
        return createException(NotFoundError, "The specified Service Worker resource was not found.", webError->message);
    case WebServiceWorkerError::ErrorTypeSecurity:
        return createException(SecurityError, "The Service Worker security policy prevented an action.", webError->message);
    case WebServiceWorkerError::ErrorTypeState:
        return createException(InvalidStateError, "The Service Worker state was not valid.", webError->message);
    case WebServiceWorkerError::ErrorTypeTimeout:
        return createException(AbortError, "The Service Worker operation timed out.", webError->message);
    case WebServiceWorkerError::ErrorTypeUnknown:
        return createException(UnknownError, "An unknown error occurred within Service Worker.", webError->message);
    }
    ASSERT_NOT_REACHED();
    return DOMException::create(UnknownError);
}

} // namespace blink
