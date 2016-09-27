/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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
#include "core/workers/AbstractWorker.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "platform/weborigin/SecurityOrigin.h"

namespace blink {

AbstractWorker::AbstractWorker(ExecutionContext* context)
    : ActiveDOMObject(context)
{
}

AbstractWorker::~AbstractWorker()
{
}

KURL AbstractWorker::resolveURL(const String& url, ExceptionState& exceptionState)
{
    // FIXME: This should use the dynamic global scope (bug #27887)
    KURL scriptURL = executionContext()->completeURL(url);
    if (!scriptURL.isValid()) {
        exceptionState.throwDOMException(SyntaxError, "'" + url + "' is not a valid URL.");
        return KURL();
    }

    // We can safely expose the URL in the following exceptions, as these checks happen synchronously before redirection. JavaScript receives no new information.
    if (!executionContext()->securityOrigin()->canRequest(scriptURL)) {
        exceptionState.throwSecurityError("Script at '" + scriptURL.elidedString() + "' cannot be accessed from origin '" + executionContext()->securityOrigin()->toString() + "'.");
        return KURL();
    }

    if (executionContext()->contentSecurityPolicy() && !executionContext()->contentSecurityPolicy()->allowWorkerContextFromSource(scriptURL)) {
        exceptionState.throwSecurityError("Access to the script at '" + scriptURL.elidedString() + "' is denied by the document's Content Security Policy.");
        return KURL();
    }

    return scriptURL;
}

DEFINE_TRACE(AbstractWorker)
{
    EventTargetWithInlineData::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

} // namespace blink
