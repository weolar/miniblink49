// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/presentation/PresentationAvailabilityCallback.h"

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/DOMException.h"
#include "modules/presentation/PresentationAvailability.h"
#include "modules/presentation/PresentationError.h"

namespace blink {

PresentationAvailabilityCallback::PresentationAvailabilityCallback(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
    : m_resolver(resolver)
{
    ASSERT(m_resolver);
}

void PresentationAvailabilityCallback::onSuccess(bool* result)
{
    OwnPtr<bool> availability = adoptPtr(result);
    if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
        return;
    m_resolver->resolve(PresentationAvailability::take(m_resolver.get(), *availability));
}

void PresentationAvailabilityCallback::onError(WebPresentationError* result)
{
    OwnPtr<WebPresentationError> error = adoptPtr(result);
    if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
        return;
    m_resolver->reject(PresentationError::take(*error));
}

} // namespace blink
