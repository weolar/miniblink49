// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/beacon/NavigatorBeacon.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/modules/v8/UnionTypesModules.h"
#include "core/dom/DOMArrayBufferView.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "core/fileapi/Blob.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/frame/UseCounter.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/html/DOMFormData.h"
#include "core/loader/BeaconLoader.h"

namespace blink {

NavigatorBeacon::NavigatorBeacon(Navigator& navigator)
    : LocalFrameLifecycleObserver(navigator.frame())
    , m_transmittedBytes(0)
{
}

NavigatorBeacon::~NavigatorBeacon()
{
}

DEFINE_TRACE(NavigatorBeacon)
{
    LocalFrameLifecycleObserver::trace(visitor);
    HeapSupplement<Navigator>::trace(visitor);
}

const char* NavigatorBeacon::supplementName()
{
    return "NavigatorBeacon";
}

NavigatorBeacon& NavigatorBeacon::from(Navigator& navigator)
{
    NavigatorBeacon* supplement = static_cast<NavigatorBeacon*>(HeapSupplement<Navigator>::from(navigator, supplementName()));
    if (!supplement) {
        supplement = new NavigatorBeacon(navigator);
        provideTo(navigator, supplementName(), supplement);
    }
    return *supplement;
}

bool NavigatorBeacon::canSendBeacon(ExecutionContext* context, const KURL& url, ExceptionState& exceptionState)
{
    if (!url.isValid()) {
        exceptionState.throwDOMException(SyntaxError, "The URL argument is ill-formed or unsupported.");
        return false;
    }
    // For now, only support HTTP and related.
    if (!url.protocolIsInHTTPFamily()) {
        exceptionState.throwDOMException(SyntaxError, "Beacons are only supported over HTTP(S).");
        return false;
    }
    // FIXME: CSP is not enforced on redirects, crbug.com/372197
    if (!ContentSecurityPolicy::shouldBypassMainWorld(context) && !context->contentSecurityPolicy()->allowConnectToSource(url)) {
        // We can safely expose the URL to JavaScript, as these checks happen synchronously before redirection. JavaScript receives no new information.
        exceptionState.throwSecurityError("Refused to send beacon to '" + url.elidedString() + "' because it violates the document's Content Security Policy.");
        return false;
    }

    // If detached from frame, do not allow sending a Beacon.
    if (!frame() || !frame()->client())
        return false;

    return true;
}

int NavigatorBeacon::maxAllowance() const
{
    ASSERT(frame());
    const Settings* settings = frame()->settings();
    if (settings) {
        int maxAllowed = settings->maxBeaconTransmission();
        if (maxAllowed < m_transmittedBytes)
            return 0;
        return maxAllowed - m_transmittedBytes;
    }
    return m_transmittedBytes;
}

bool NavigatorBeacon::beaconResult(ExecutionContext* context, bool allowed, int sentBytes)
{
    if (allowed) {
        ASSERT(sentBytes >= 0);
        m_transmittedBytes += sentBytes;
    } else {
        UseCounter::count(context, UseCounter::SendBeaconQuotaExceeded);
    }
    return allowed;
}

bool NavigatorBeacon::sendBeacon(ExecutionContext* context, Navigator& navigator, const String& urlstring, const ArrayBufferViewOrBlobOrStringOrFormData& data, ExceptionState& exceptionState)
{
    NavigatorBeacon& impl = NavigatorBeacon::from(navigator);

    KURL url = context->completeURL(urlstring);
    if (!impl.canSendBeacon(context, url, exceptionState))
        return false;

    int allowance = impl.maxAllowance();
    int bytes = 0;
    bool allowed;

    if (data.isArrayBufferView())
        allowed = BeaconLoader::sendBeacon(impl.frame(), allowance, url, data.getAsArrayBufferView(), bytes);
    else if (data.isBlob())
        allowed = BeaconLoader::sendBeacon(impl.frame(), allowance, url, data.getAsBlob(), bytes);
    else if (data.isString())
        allowed = BeaconLoader::sendBeacon(impl.frame(), allowance, url, data.getAsString(), bytes);
    else if (data.isFormData())
        allowed = BeaconLoader::sendBeacon(impl.frame(), allowance, url, data.getAsFormData(), bytes);
    else
        allowed = BeaconLoader::sendBeacon(impl.frame(), allowance, url, String(), bytes);

    return impl.beaconResult(context, allowed, bytes);
}

} // namespace blink
