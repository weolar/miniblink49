/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
#include "bindings/core/v8/BindingSecurity.h"

#include "bindings/core/v8/V8Binding.h"
#include "core/dom/Document.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLFrameElementBase.h"
#include "platform/weborigin/SecurityOrigin.h"

namespace blink {

static bool isOriginAccessibleFromDOMWindow(SecurityOrigin* targetOrigin, LocalDOMWindow* callingWindow)
{
    return callingWindow && callingWindow->document()->securityOrigin()->canAccessCheckSuborigins(targetOrigin);
}

static bool canAccessFrame(v8::Isolate* isolate, SecurityOrigin* targetFrameOrigin, DOMWindow* targetWindow, ExceptionState& exceptionState)
{
    LocalDOMWindow* callingWindow = callingDOMWindow(isolate);
    if (isOriginAccessibleFromDOMWindow(targetFrameOrigin, callingWindow))
        return true;

    if (targetWindow)
        exceptionState.throwSecurityError(targetWindow->sanitizedCrossDomainAccessErrorMessage(callingWindow), targetWindow->crossDomainAccessErrorMessage(callingWindow));
    return false;
}

static bool canAccessFrame(v8::Isolate* isolate, SecurityOrigin* targetFrameOrigin, DOMWindow* targetWindow, SecurityReportingOption reportingOption = ReportSecurityError)
{
    LocalDOMWindow* callingWindow = callingDOMWindow(isolate);
    if (isOriginAccessibleFromDOMWindow(targetFrameOrigin, callingWindow))
        return true;

    if (reportingOption == ReportSecurityError && targetWindow)
        callingWindow->printErrorMessage(targetWindow->crossDomainAccessErrorMessage(callingWindow));
    return false;
}

bool BindingSecurity::shouldAllowAccessToFrame(v8::Isolate* isolate, Frame* target, SecurityReportingOption reportingOption)
{
    if (!target || !target->securityContext())
        return false;
    if (!RuntimeEnabledFeatures::cspCheckEnabled())
        return true;
    return canAccessFrame(isolate, target->securityContext()->securityOrigin(), target->domWindow(), reportingOption);
}

bool BindingSecurity::shouldAllowAccessToFrame(v8::Isolate* isolate, Frame* target, ExceptionState& exceptionState)
{
    if (!target || !target->securityContext())
        return false;
    if (!RuntimeEnabledFeatures::cspCheckEnabled())
        return true;
    return canAccessFrame(isolate, target->securityContext()->securityOrigin(), target->domWindow(), exceptionState);
}

bool BindingSecurity::shouldAllowAccessToNode(v8::Isolate* isolate, Node* target, ExceptionState& exceptionState)
{
    return target && canAccessFrame(isolate, target->document().securityOrigin(), target->document().domWindow(), exceptionState);
}

}
